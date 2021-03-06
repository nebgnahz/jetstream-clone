#
# Graph data structure and algorithms for representing JetStream queries/computations.
#

import logging
from sets import Set
from operator_schemas import SchemaError

from jetstream_types_pb2 import *

logger = logging.getLogger('JetStream')


def get_oid (obj):
  """Returns the ID of a node in a JetStream query graph, which is currently either an
  operator or a cube."""
  return obj.id.task if isinstance(obj, TaskMeta) else obj.name


class JSNode (object):
  """Internal representation of a node in a JetStream computation graph. A JSNode 
  should not be returned to an outside caller; return the underlying object instead."""

  # Color enum for graph algorithm
  class Color:
    WHITE = 1
    BLACK = 2

  
  def __init__ (self, id, obj):
    # ID may be any hashable type (integer, string, etc.)
    self.id = id
    self.object = obj
    # Declare (and reset) fields used by graph algorithms
    self.reset()

    
  def reset (self):
    self.rank = -1
    self.color = JSNode.Color.WHITE
    self.parent = None
    self.ancestor = None


  def __hash__ (self):
    return hash(self.id)


  def __eq__ (self, other):
    if isinstance(other, JSNode):
      if (self.id != other.id):
        return False
      assert(self.object == other.object)
      return True
    return NotImplemented


  def __ne__ (self, other):
    result = self.__eq__(other)
    if result is NotImplemented:
      return result
    return not result


class JSGraph (object):
  """A JetStream computation graph"""

  def __init__ (self, operators, cubes, edges):
    # Map of node ID -> JSNode. Id will be a string for cubes, and an int for operators
    self.nodes = {}
    # Map of node ID -> JSNode for sources (nodes with no incoming edge)
    self.sources = {}
    # Map of node ID -> JSNode for sinks (nodes with no outgoing edge), usually there
    # is only one sink.
    self.sinks = {}
    # Map of node -> [outgoing neighbor]
    self.adjList = {}
    # Map of node -> [incoming neighbor]
    self.radjList = {}

    for op in operators:
      self.nodes[op.id.task] = JSNode(op.id.task, op)
    for c in cubes:
      self.nodes[c.name] = JSNode(c.name, c)

    # Construct the forward and reverse adjacency lists
    for e in edges:
      if not e.dest_cube and not e.dest:
        #pseudo-edge, so we ignore
        continue

      if e.src:
        src = self.nodes[e.src]
      else:
        src = self.nodes[e.src_cube]
      if src not in self.adjList:
        self.adjList[src] = []
      # Determine if the destination is a cube or operator
      dest = self.nodes[e.dest_cube] if e.dest_cube != "" else self.nodes[e.dest]
      if dest not in self.radjList:
        self.radjList[dest] = []
      self.adjList[src].append(dest)
      self.radjList[dest].append(src)

    # Determine the source nodes and sink nodes
    for node in self.nodes.values():
      if node not in self.radjList:
        self.sources[node.id] = node
      if node not in self.adjList:
        self.sinks[node.id] = node
    # There should be at least one source and sink
    if (len(self.sources) == 0) or (len(self.sinks) == 0):
      logger.error("Computation graph has no sources or sinks (just cycles)")
      raise SchemaError("Computation graph has no sources or sinks (just cycles)")
    if len(self.sinks) > 1:
      logger.warn("Multiple sinks found in computation graph")


    # To properly validate a computation graph, use the validate_* methods below
        

  def validate (self):
    #TODO: Ascertain connectivity, tree-like structure (condense parallel paths and cycles)
    return True


  def get_sources (self):
    return [node.object for node in self.sources.values()]


  def get_sinks (self):
    return [node.object for node in self.sinks.values()]

    
  def reset_nodes (self):
    """Clears any node state of prior graph algorithm runs."""
    for node in self.nodes.values():
      node.reset()


  #TODO: Use colors to mark visited vertices and avoid redundant exploration
  #TODO:SOSP: Return the descendants of a node in topological order, optionally stopping when a (specified) end node is reached.
  def get_descendants (self, startObj, endObj=None):
    start = self.nodes[get_oid(startObj)]
    end = None
    if endObj != None:
      end = self.nodes[get_oid(endObj)]
    self.reset_nodes()
    descendantObjs = [start.object]
    if start in self.adjList:
      toExplore = self.adjList[start]
      while len(toExplore) > 0:
        next = toExplore.pop()
        descendantObjs.append(next.object)
        if (next != end) and (next in self.adjList):
          toExplore.extend(self.adjList[next])
    return descendantObjs
      

  def get_sources_lca (self):
    """Returns the least-common ancestor (or descendant, more accurately) of the source
    nodes in the graph. Assumes the graph is (reducible to) a tree."""

    # Designate some sink as the root. We may have to try multiple sinks as roots
    # before finding an LCA of all sources.
    for root in self.sinks.values():
      # Start with all sources and repeatedly compute the LCA of pairs until one LCA
      # remains. Since LCA(x,x) = x, use a Set to track the LCAs.
      lcas = Set(self.sources.values())
      lcaPairs = {}
      while len(lcas) >= 2:
        (u, v) = (lcas.pop(), lcas.pop())
        # setdefault() returns the value if key exists and initializes otherwise
        lcaPairs.setdefault(u, []).append(v)
        lcaPairs.setdefault(v, []).append(u)
      # Clear any prior node state
      self.reset_nodes()
      self.compute_lcas(root, lcaPairs, lcas, True)
      # If all LCA pairs were successfully computed, we are done
      if len(lcaPairs) == 0:
        assert(len(lcas) == 1)
        break
      else:
        logger.warn("Tried sink " + str(root.id) + " as root, could not find sources LCA")
        lcas.clear()

    if len(lcas) > 0:
      return lcas.pop().object
    else:
      raise SchemaError("Graph lacks a union point for all sources")
    

  def compute_lcas (self, u, lcaPairs, lcas, recurse):
    uf_make_set(u)
    u.ancestor = u
    # JetStream graphs are directed towards the root, so use the reverse adjacency list.
    if u in self.radjList:
      for v in self.radjList[u]:
        self.compute_lcas(v, lcaPairs, lcas, recurse)
        uf_union(u,v)
        uf_find(u).ancestor = u
    u.color = JSNode.Color.BLACK
    if u in lcaPairs:
      i = 0
      while i < len(lcaPairs[u]):
        v = lcaPairs[u][i]
        # LCA(u,v) is uf_find(v).ancestor immediately after u is colored black, provided
        # v is already black; otherwise it is uf_find(u).ancestor immediately after v is
        # colored black
        if v.color == JSNode.Color.BLACK:
          lcas.add(uf_find(v).ancestor)
          # Remove the pair (from both u and v's lists) so we know its LCA was found
          del lcaPairs[u][i]
          i -= 1
          # Generally not efficient, but in our case the LCA pair lists should be short
          # (surely at the onset, where its over disjoint pairs of sources)
          lcaPairs[v].remove(u)
          if len(lcaPairs[v]) == 0:
            del lcaPairs[v]
          # Find the LCA of LCAs, if asked to do so
          if recurse and (len(lcas) >= 2):
            (w, x) = (lcas.pop(), lcas.pop())
            lcaPairs.setdefault(w, []).append(x)
            lcaPairs.setdefault(x, []).append(w)
        i += 1
      # Again, this cleanup helps us determine if all LCAs were found
      if len(lcaPairs[u]) == 0:
        del lcaPairs[u]
      


##### Union-find implementation #####

def uf_make_set (x):
  x.parent = x
  x.rank = 0

 
def uf_union (x, y):
  xRoot = uf_find(x)
  yRoot = uf_find(y)
  if xRoot.rank > yRoot.rank:
    yRoot.parent = xRoot
  elif xRoot.rank < yRoot.rank:
    xRoot.parent = yRoot
  elif xRoot != yRoot:
    yRoot.parent = xRoot
    xRoot.rank = xRoot.rank + 1

  
def uf_find (x):
  if x.parent == x:
    return x
  else:
    x.parent = uf_find(x.parent)
    return x.parent

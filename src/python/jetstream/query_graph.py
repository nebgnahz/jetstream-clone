import types

from itertools import izip, tee

from jetstream_types_pb2 import *
Dimension = CubeSchema.Dimension
from operator_schemas import SCHEMAS, OpType,SchemaError
from base_constructs import *

# from python itertools recipes
def pairwise(iterable):
    "s -> (s0,s1), (s1,s2), (s2, s3), ..."
    a, b = tee(iterable)
    next(b, None)
    return izip(a, b)

class QueryGraph(object):
  """Represents the client's-eye-view of a computation.

  We use the task IDs of operators internally. We also use the same numeric ID
  space for cubes, in this class. However, the cube names are substituted in at
  serialization time.
  This means, in particular, that you should be able to share a cube across
  computations.

  Note that cube names are, as a result, NOT required to be unique across a
  computation here. However the server does impose this limitation.
  """

  def __init__(self):
    self.nID = 1            # the NEXT ID to hand out
    self.edges = {}    # maps pairs of nIDs to aux info
    self.operators = {}     # maps id -> value
    self.cubes = {}         # maps id -> value
    self.externalEdges = [] # literal protobuf edges


  def get_sources(self):
    """Returns a list of IDs corresponding to nodes without an in-edge"""
    non_sources = set([x for (y,x) in self.edges.keys()])
    return [x for x in (self.operators.keys() + self.cubes.keys()) if x not in non_sources]

  def forward_edge_map(self):
    """Returns a map from each node to its set of forward edges"""
    ret = {}
    for (s,d) in self.edges.keys():
      if s not in ret:
        ret[s] = [d]
      else:
        ret[s].append(d)
    return ret

  def add_operator(self, type, cfg):
    """Add an operator to the graph"""
    o = Operator(self, type, cfg, self.nID)
    self.operators[self.nID] = o
    self.nID += 1
    return o

  def add_existing_operator(self, o):
    id = self.nID
    self.operators[id] = o
    self.nID += 1
    return id

  def remove(self, o):  # NOTE: this routine is not yet carefully tested -- asr, 11/27/12
    if isinstance(o, int):
      oid = o
      o = self.operators[oid] if oid in self.operators else self.cubes[oid]
    else:
      oid = o.get_id()

    for p in o.preds:   #remove backward edges first
      del self.edges[ (p.id,oid) ]
    to_drop = []
    for src,dest in self.edges.keys():
      if src == oid:   
        to_drop.append (  (src,dest) )
        d = self.operators[dest] if dest in self.operators else self.cubes[dest]
        d.remove_pred(o)
    for e in to_drop:   #now remove forward edges
      del self.edges[e]

    if oid in self.operators:
      del self.operators[oid]
    else:
      del self.cubes[oid]

  def add_cube(self, name, desc = {}):
    """Add a cube to the graph"""
    c = Cube(self, name, desc, self.nID)
    self.cubes[self.nID] = c
    self.nID += 1
    return c

  def add_to_PB(self, alter):

    alter.computationID = 0
    for id,operator in self.operators.items():
      operator.add_to_PB(alter)
    for id,cube in self.cubes.items():
      cube.add_to_PB(alter)
    for e in self.externalEdges:
        pb_e = alter.edges.add()
        pb_e.CopyFrom(e)
        assert pb_e.IsInitialized()
    for e,aux in self.edges.items():
      if aux.get('dummy', False):
        continue  #silently ignore
      pb_e = alter.edges.add()
      pb_e.computation = 0

      if 'max_kb_per_sec' in aux:
        pb_e.max_kb_per_sec = aux['max_kb_per_sec']
      else:
        assert(len(aux) == 0)

      if e[0] in self.operators:
        pb_e.src = e[0]
      else:
        pb_e.src_cube=self.cubes[e[0]].name

      if e[1] in self.operators:
        pb_e.dest = e[1]
      else:  #dest wasn't an operator, so must be cube
        if e[1] not in self.cubes:
          print "ERR: edge",e,"connects to nonexistant node"
          print "operators:",self.operators
          print "cubes:",self.cubes
        assert(e[1] in self.cubes)
        pb_e.dest_cube = self.cubes[e[1]].name

  def connect(self, oper1, oper2, bwLimit=-1):
    """ Add an edge from the the first operator to the second. """
    aux = {}
    if bwLimit > 0:
      aux['max_kb_per_sec'] = bwLimit
    elif bwLimit == 0:
      aux['dummy'] = True
    
    self.edges[ (oper1.get_id(), oper2.get_id()) ] = aux
    oper2.add_pred(oper1)

  def chain(self, operators):
    """ Add edges from each destination in the list to the next destination in
        the list. Cubes are allowed. """
    assert all(isinstance(op, Destination) for op in operators)
    for oper, next_oper in pairwise(operators):
      self.connect(oper, next_oper)

  # right now this is for adding an edge to the client so it can act as a
  # receiver
  def connectExternal(self, operator, nodeid):
      """ Add an edge from an operator to a node """
      e = Edge()
      e.computation = 0  # dummy
      e.src = operator.get_id()
      e.dest_addr.CopyFrom(nodeid)

      self.externalEdges.append(e)

  def clone_back_from(self, head, numcopies):
    to_copy = {}  #maps id to object
    to_inspect = set([head])
    edges_to_copy = []
    while len(to_inspect) > 0:
      e = to_inspect.pop()
      to_copy[e.get_id()] = e
      for pred in e.preds:
        edges_to_copy.append( (pred.get_id(), e.get_id()) )
        if pred not in to_copy and pred not in to_inspect:
          to_inspect.add(pred)
    #at this point we have a set of nodes to inspect, with their IDs
    id_to_obj = {}
    newheads = []
    for n in range(numcopies):
      old_to_new_ids = {}
      for id,dest in to_copy.items():
        new_dest = self.copy_dest(dest)
        id_to_obj[new_dest.get_id()] = new_dest
        old_to_new_ids[id] = new_dest.get_id()
      for (e1,e2) in edges_to_copy:
        self.connect( id_to_obj[old_to_new_ids[e1]] , id_to_obj[old_to_new_ids[e2]])
      new_head =  id_to_obj[ old_to_new_ids[head.get_id()]]
      newheads.append(new_head)
    return newheads


  def copy_dest(self, dest):
    if isinstance(dest,Operator):
      new_cfg = {}
      new_cfg.update(dest.cfg)
      return self.add_operator(dest.type, new_cfg)
    elif isinstance(dest,Cube):
      return self.add_cube(dest.name, dest.desc)
    else:
      raise "unexpected param to copy_dest"

  def get_deploy_pb(self):
    self.validate_schemas()
    req = ControlMessage()
    req.type = ControlMessage.ALTER
    self.add_to_PB(req.alter)
    return req

  def node_type(self, id):
    if id in self.operators:
      return self.operators[id].type
    else:
      return self.cubes[id].name + " cube"

  def is_filtering_subsc(self, o):
    return o in self.operators and self.operators[o].type == OpType.FILTER_SUBSCRIBER

  def validate_schemas(self):
    worklist = self.get_sources()  #worklist is a list of IDs
#    print "initial source operators",worklist
    input_schema = {}
    for n in worklist:
      input_schema[n] = ()

    forward_edges = self.forward_edge_map();
    for n in worklist:
      is_operator = n in self.operators
    
      # print "validating schemas for outputs of %d" % n
      # note that cubes don't have an output schema and subscribers are a special case
      if self.is_filtering_subsc(n):
        out_schema = filter_subsc_validate(self.operators[n], input_schema)
      elif is_operator:
#        print "found operator",n,"of type",self.operators[n].type
        out_schema = self.operators[n].out_schema( input_schema[n])
#        print "out schema is %s, have %d out-edges" % (str(out_schema), len(forward_edges.get(n, []) ))
      else:
        out_schema = self.cubes[n].out_schema (input_schema[n])

#      print "out-schema for", n, self.node_type(n), "is", out_schema
      for o in forward_edges.get(n, []):
        if o in input_schema: #already have a schema:
          if input_schema[o] != out_schema:
            err_msg = "Edge from %d of type %s to %d of type %s (%s) doesn't match existing schema %s" \
                % (n, self.node_type(n), o, self.node_type(o), str(out_schema), str(input_schema[o]))
            raise SchemaError(err_msg)
        else:
          if n in self.operators or not self.is_filtering_subsc(o):
            input_schema[o] = out_schema
          worklist.append(o)


##### Useful operators #####

def FileRead(graph, file, skip_empty=False):
   assert isinstance(file, str)
   cfg = { "file":file, "skip_empty" : str(skip_empty)}
   return graph.add_operator(OpType.FILE_READ, cfg)


def StringGrepOp(graph, pattern):
   cfg = {"pattern":pattern, "id": 0}
   import re
   re.compile(pattern) # throw an error on bad patterns
   return graph.add_operator(OpType.STRING_GREP, cfg)

def CSVParse(graph, types, fields_to_keep='all'):
   assert isinstance(types, str)

   keepStr = fields_to_keep
   if fields_to_keep != 'all':
     if not all(isinstance(f, int) for f in fields_to_keep):
       raise SchemaError("CSVParse needs either \"all\" or " \
        "list of field indices to keep, got {0}".format(str(fields_to_keep)))
     keepStr = ' '.join(map(str, fields_to_keep))
   assert isinstance(keepStr, str)
   cfg = {"types" : types, "fields_to_keep" : keepStr}
   return graph.add_operator(OpType.CSV_PARSE, cfg)


def ExtendOperator(graph, typeStr, fldValsList):
  cfg = {"types": typeStr}
  assert len(fldValsList) == len(typeStr) and len(typeStr) < 11 and len(typeStr) > 0
  i = 0
  for x in fldValsList:
    cfg[str(i)] = str(x)
    i += 1
  return graph.add_operator(OpType.EXTEND, cfg)

def TimestampOperator(graph, typeStr):
  cfg = {"type": typeStr}
  assert len(typeStr) < 3 and len(typeStr) > 0
  return graph.add_operator(OpType.TIMESTAMP, cfg)


def GenericParse(graph,
                 pattern, typeStr, field_to_parse = 0, keep_unparsed=True):
    cfg = {"types" : typeStr,
           "pattern" : pattern,
           "field_to_parse" :field_to_parse,
           "keep_unparsed" : str(keep_unparsed)}
    import re
    re.compile(pattern) # throw an error on bad patterns
    return graph.add_operator(OpType.PARSE, cfg)


def RandSource(graph, n, k):
   cfg = {"n": str(n), "k": str(k)}  # "rate":str(rate)
   return graph.add_operator(OpType.RAND_SOURCE, cfg)


def RandEval(graph):
  return graph.add_operator(OpType.RAND_EVAL, {} )


def TRoundOperator(graph, fld, round_to, add_offset=0):
  cfg = {"fld_offset": str(fld),
         "round_to": str(round_to),
         "add_offset": str(add_offset)}  # "rate":str(rate)
  return graph.add_operator(OpType.T_ROUND_OPERATOR, cfg)


def NoOp(graph):
   cfg = {}
   return graph.add_operator(OpType.EXTEND, cfg)

class TimeSubscriber(Operator):
  def __init__ (self, graph, my_filter, interval, sort_order = "", num_results = 0):
    super(TimeSubscriber,self).__init__(graph,OpType.TIME_SUBSCRIBE, {}, 0)
    self.filter = my_filter  #maps
    self.cfg["window_size"] = interval
    self.cfg["sort_order"] = sort_order
    self.cfg["num_results"] = num_results

    self.id = graph.add_existing_operator(self)
    assert self.id != 0


  def add_to_PB(self, alter):
    assert( len(self.preds) == 1)
    pred_cube = list(self.preds)[0]
    # Need to convert the user-specified selection keys into positional form
    # for the DB
    dims = pred_cube.get_output_dimensions()
    tuple = Tuple()

#    max_dim = max(dims_by_id.keys())
#   print dims_by_id
#   for id in range(0, max_dim+1):
#      el = tuple.e.add()
#      if id not in dims_by_id:
#        continue
    filter = {}
    filter.update(self.filter)
    for dim_name,dim_type in dims:
      el = tuple.e.add()
      if dim_name in self.filter:
        val = filter[dim_name]
        del filter[dim_name]
        if dim_type == Element.STRING:
          el.s_val = val
        else:
          raise ValueError("Panic; trying to filter on dimension without type")

    if len(filter) > 0:
      unmatched_fields = ",".join(self.filter.keys())
      raise ValueError("Panic: filter field unknown in cube. Unmatched "
                       "fields: {}".format(unmatched_fields))
#    print "final filter tuple:", tuple
    #We do this in two phases

    serialized_filter = tuple.SerializeToString()
#    print "Filter length, serialized: ",len(serialized_filter)
    self.cfg["slice_tuple"] = serialized_filter

    my_meta = Operator.add_to_PB(self,alter)

  def out_schema(self, in_schema):
#    print "time subscriber schema"
    if 'ts_field' in self.cfg:
      ts_field = int(self.cfg['ts_field'])
      if ts_field >= len(in_schema):
        raise SchemaError('ts_field %d illegal for operator; only %d real inputs' \
          % (ts_field, len(in_schema)))
      if in_schema[ts_field][0] != 'T':
        raise SchemaError('Expected a time element for ts_field %d' % ts_field)
    return in_schema  #everything is just passed through


def VariableCoarseningSubscriber(*args, **kwargs):
   op = TimeSubscriber(*args, **kwargs)
   op.type = OpType.VAR_TIME_SUBSCRIBE
   return op


def LatencyMeasureSubscriber(graph, time_tuple_index, hostname_tuple_index, interval_ms=1000):
   cfg = {"time_tuple_index" : str(time_tuple_index),
          "hostname_tuple_index" : str(hostname_tuple_index),
          "interval_ms" : str(interval_ms)}
   return graph.add_operator(OpType.LATENCY_MEASURE_SUBSCRIBER, cfg)
   
##### Test operators #####

def SendK(graph, k):
   cfg = {"k" : str(k)}
   return graph.add_operator(OpType.SEND_K, cfg)

def RateRecord(graph):
   cfg = {}
   return graph.add_operator(OpType.RATE_RECEIVER, cfg)

def DummySerialize(g):
  return g.add_operator("SerDeOverhead", {})

def Echo(g):
  return g.add_operator(OpType.ECHO, {})

def VariableSampling(g):
  return g.add_operator(OpType.VARIABLE_SAMPLING, {})

def SamplingController(g):
  return g.add_operator(OpType.CONGEST_CONTROL, {})

def Quantile(graph, q, field):
   cfg = {"q":str(q), "field":field}
   return graph.add_operator(OpType.QUANTILE, cfg)

def ToSummary(graph, size, field):
   cfg = {"size":str(size), "field":field}
   return graph.add_operator(OpType.TO_SUMMARY, cfg)

def SummaryToCount(graph, field):
   cfg = {"field":field}
   return graph.add_operator(OpType.SUMMARY_TO_COUNT, cfg)

def URLToDomain(graph, field):
   cfg = {"field":field}
   return graph.add_operator(OpType.URLToDomain, cfg)

def TimeWarp(graph, field, warp):
   cfg = {"field":field, "warp":warp}
   return graph.add_operator(OpType.TIMEWARP, cfg)

def CountLogger(graph, field):
   cfg = {"field":field}
   return graph.add_operator(OpType.COUNT_LOGGER, cfg)

def FilterSubscriber(graph, cube_field = 2, level_in_field=0):
   cfg = {"cube_field":int(cube_field), "level_in_field":int(level_in_field)}
   return graph.add_operator(OpType.FILTER_SUBSCRIBER, cfg)
  
def filter_subsc_validate(filter_op, input_schemas):
  saw_cube = False
  saw_filter = False  
  ret = []
  cfg = filter_op.cfg
  for pred in filter_op.preds:

    if isinstance(pred, Cube):
      if saw_cube:
        raise SchemaError("filter should have a cube input and at most one other")
      saw_cube = True
      ret = input_schemas[pred.get_id()]
      print "picking out_schema for filter. Guessing %s" % ret
      #todo check that relevant field is int
    else:
      in_s = input_schemas[filter_op.get_id()]

      if saw_filter:
        raise SchemaError("filter should have a cube input and at most one other")
      saw_filter = True
      if not "level_in_field" in cfg:
        raise SchemaError("must specify numeric 'level_in_field' if adding a filter edge")
      level_in = cfg['level_in_field']
      if len(in_s) <= level_in or  in_s[ level_in ][0] != 'I':
        print input_schemas
        raise SchemaError("level_in_field for FilterSubscriber should be int. " \
            "Schema was %s." % str(in_s))
  return ret
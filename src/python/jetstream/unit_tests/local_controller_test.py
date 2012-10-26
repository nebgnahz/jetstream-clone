import random
import socket
import time
import unittest

from local_controller import LocalController
from query_graph import QueryGraph,Operator
from jetstream_types_pb2 import *


class TestLocalController(unittest.TestCase):

  def setUp(self):
    self.server = LocalController()

    
  def test_get_nodes(self):
    nodes = self.server.all_nodes()
    self.assertEquals(len(nodes), 1)
    
    node = self.server.get_a_node()
    self.assertEquals(str(node), str(nodes[0]))
    
    node_as_str = str(node)
    self.assertTrue(str(LocalController.DEFAULT_PORTNO) in node_as_str )
    my_hostname =  "localhost" #socket.gethostbyname(socket.gethostname())
    self.assertTrue(my_hostname in node_as_str )


  def test_op_graph(self):
    
    n = self.server.get_a_node()
    g = QueryGraph()
    op = g.add_operator(Operator.OpType.UNIX, {"cmd":"cat /etc/shells"})
    cube = g.add_cube("storeddata", {})
    g.connect(op, cube)
    cube.instantiate_on(n)
    self.assertTrue( cube.get_name().endswith("/storeddata"))
    
    self.assertTrue(cube.is_placed())
    self.assertEquals(cube.location(), n)
    
    self.server.deploy(g)
    time.sleep(5)
    
    cube_data = self.server.get_cube(cube.get_name())
    self.assertTrue(len(cube_data) > 4)


  def test_clone_back(self):
    g = QueryGraph()
    op = g.add_operator(Operator.OpType.UNIX, {"cmd":"cat /etc/shells"})
    cube = g.add_cube("storeddata", {})
    g.connect(op, cube)
    self.assertEquals(g.nID, 3)
    g.clone_back_from(cube, 1)
    self.assertEquals(g.nID, 5)
    self.assertEquals( len(cube.preds), 1)
    self.assertEquals( len(g.edges), 2)

    self.assertTrue( (1,2) in  g.edges)
    self.assertTrue( (3,4) in  g.edges  or  (4,3) in  g.edges)
    

  def test_multi_place(self):
    n = self.server.get_a_node()
    
    n2 = NodeID()
    n2.portno = 123
    n2.address = "dummy host"
    
    g = QueryGraph()
    op = g.add_operator(Operator.OpType.UNIX,{"cmd":"cat /etc/shells"})
    cube = g.add_cube("storeddata", {})
    g.connect(op, cube)
    
    cube.instantiate_on([n, n2])
    
    
if __name__ == '__main__':
    unittest.main()
#
# Graph data structure and algorithms for representing JetStream queries/computations.
#

from jetstream_types_pb2 import *

from optparse import OptionParser 

import random
import socket
import time


from remote_controller import RemoteController
import query_graph as jsapi
from jetstream_types_pb2 import *


def main():

  parser = OptionParser()
  parser.add_option("-C", "--config", dest="config_file",
                  help="read config from FILE", metavar="FILE")

  parser.add_option("-a", "--controller", dest="controller",
                  help="controller address", default="localhost:3456")
  (options, args) = parser.parse_args()
  pattern = ".*" + args[0] + ".*"
  file_to_grep = args[1]


  if ':' in options.controller:
    (serv_addr, serv_port) = options.controller.split(':')
    serv_port = int(serv_port)
  else:
    serv_addr = options.controller
    serv_port = 3456
  
  
  ### Define the graph abstractly, without a computation
  g = jsapi.QueryGraph()
  reader = jsapi.FileRead(g, file_to_grep)
  grepper = jsapi.StringGrep(g, pattern)
  host_extend = jsapi.ExtendOperator(g, "s", ["${HOSTNAME}"])
  
  cube = g.add_cube("local_results")
  cube.add_dim("log_line", Element.STRING, 0)
  cube.add_dim("hostname", Element.STRING, 1)
  cube.add_agg("count", jsapi.Cube.AggType.COUNT, 2)

  cube.set_overwrite(True)  #fresh results

  g.connect(reader,grepper)
  g.connect(grepper, host_extend)
  g.connect(host_extend, cube)
  
  #### Finished building in memory, now to join
  server = RemoteController()
  server.connect(serv_addr, serv_port)
  n = server.get_a_node()
  assert isinstance(n, NodeID)
  nodes = server.all_nodes()
  
  cube.instantiate_on(n)
  host_extend.instantiate_on(nodes)

  result_reader = ClientDataReader()
  net_addr = result_reader.prep_to_receive_data()
  g.connect(cube, net_addr)

  server.deploy(g)
  result_reader.blocking_read(lambda x: print x )


if __name__ == '__main__':
    main()

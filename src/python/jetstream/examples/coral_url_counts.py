from collections import defaultdict
import csv
from optparse import OptionParser
import random
import sys
import time

from jetstream_types_pb2 import *
from remote_controller import *
import query_graph as jsapi
from query_planner import QueryPlanner

from coral_parse import coral_fnames,coral_fidxs, coral_types
from coral_util import *   #find_root_node, standard_option_parser,

def main():

  parser = standard_option_parser()

  (options, args) = parser.parse_args()

  if not options.fname:
    print "you must specify the input file name [with -f]"
    sys.exit(1)

  all_nodes,server = get_all_nodes(options)
  
  root_node = find_root_node(options, all_nodes)

  g = get_graph(all_nodes, root_node,  options)

  deploy_or_dummy(options, server, g)
  

def define_cube(cube, ids = [0,1,2,3]):
  cube.add_dim("time", CubeSchema.Dimension.TIME_CONTAINMENT, ids[0])
  cube.add_dim("response_code", Element.INT32, ids[1])
  cube.add_dim("url", CubeSchema.Dimension.STRING, ids[2]
#  cube.add_agg("sizes", jsapi.Cube.AggType.HISTO, ids[2])
#  cube.add_agg("latencies", jsapi.Cube.AggType.HISTO, ids[3])
  cube.add_agg("count", jsapi.Cube.AggType.COUNT, ids[3])

  cube.set_overwrite(True)


def get_graph(all_nodes, root_node, options):
  g= jsapi.QueryGraph()

  ANALYZE = not options.load_only
  LOADING = not options.analyze_only

  if not LOADING and not ANALYZE:
    print "can't do neither load nor analysis"
    sys.exit(0)

  start_ts = parse_ts(options.start_ts)

  central_cube = g.add_cube("global_coral_urls")
  central_cube.instantiate_on(root_node)
  define_cube(central_cube)


  parsed_field_offsets = [coral_fidxs['timestamp'], coral_fidxs['HTTP_stat'],\
      coral_fidxs['URL_requested'], len(coral_types) ]

  for node, i in zip(all_nodes, range(0, len(all_nodes))):
    local_cube = g.add_cube("local_coral_quant_%d" %i)
    define_cube(local_cube, parsed_field_offsets)
    print "cube output dimensions:", local_cube.get_output_dimensions()

    if LOADING:
      f = jsapi.FileRead(g, options.fname, skip_empty=True)
      csvp = jsapi.CSVParse(g, coral_types)
      csvp.set_cfg("discard_off_size", "true")
      round = jsapi.TRoundOperator(g, fld=1, round_to=1)
      dom_to_url = jsapi.DomToURL(g, fld=coral_fidxs['URL_requested'])
      g.chain( [f, csvp, round, dom_to_url, local_cube] )
    else:
       local_cube.set_overwrite(False)

    pull_from_local = jsapi.TimeSubscriber(g, {}, 1000) #every second
    pull_from_local.instantiate_on(node)
    pull_from_local.set_cfg("simulation_rate", options.warp_factor)
    pull_from_local.set_cfg("ts_field", 0)
    pull_from_local.set_cfg("start_ts", start_ts)
    pull_from_local.set_cfg("window_offset", 2000) #but trailing by a few

    local_cube.instantiate_on(node)
    timestamp_op= jsapi.TimestampOperator(g, "ms")
    count_extend_op = jsapi.ExtendOperator(g, "i", ["1"])
    count_extend_op.instantiate_on(node)

    timestamp_cube_op= jsapi.TimestampOperator(g, "ms")
    timestamp_cube_op.instantiate_on(root_node)
    g.chain([local_cube, pull_from_local,timestamp_op, timestamp_cube_op, central_cube])

  return g
  

if __name__ == '__main__':
    main()

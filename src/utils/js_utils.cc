#include "js_utils.h"
#include <iostream>

#include <time.h>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#else
#include <sys/prctl.h>
#endif


#include <glog/logging.h>

using namespace jetstream;

timespec jetstream::tsnow;

void
jetstream::jetstream_init () {
  refresh_time();
}

void
jetstream::refresh_time () {
#ifdef __MACH__
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  tsnow.tv_sec  = mts.tv_sec;
  tsnow.tv_nsec = mts.tv_nsec;
#else
  clock_gettime (CLOCK_REALTIME, &tsnow);
#endif
}

sec_t
jetstream::get_sec () {
  refresh_time();
  return tsnow.tv_sec;
}

msec_t jetstream::get_msec () {
  return get_usec()/1000;
}

usec_t
jetstream::get_usec () {
  refresh_time();
  return ((tsnow.tv_sec * (usec_t) 1000000) + (tsnow.tv_nsec / 1000));
}

timespec
jetstream::get_time () {
  refresh_time();
  return tsnow;
}

int
jetstream::js_usleep(useconds_t usecs)
{
  return usleep(usecs);
}

/*
std::string
jetstream::get_strtime () {
  timespec ts = get_time();
  string s2 = str( format("%2% %1%") % 36 % 77 );
  str buf = strbuf("%d.%06d", int (ts.tv_sec), int (ts.tv_nsec / 1000));
  return buf;
}
*/

namespace jetstream {

inline void add_one_el(std::ostringstream& buf, const Element& el) {
  bool had_el = false;
  if (el.has_s_val()) {
    buf << '"'<<el.s_val()<< '"';
    had_el = true;
  }
  if (el.has_d_val()) {
    buf << "D=" << el.d_val()<< " ";
    had_el = true;  
  }
  if (el.has_i_val()) {
    buf << "I="<<el.i_val()<< " ";
    had_el = true;  
  }
  
  if (el.has_t_val()) {
    time_t t = (time_t)el.t_val();
    struct tm parsed_time;
    gmtime_r(&t, &parsed_time);

    char tmbuf[80];
    strftime(tmbuf, sizeof(tmbuf), "%d-%m-%y %H:%M:%S", &parsed_time);
    buf << tmbuf<< " ";
    had_el = true;      
  } else if (el.has_summary()) {
     if(el.summary().has_histo()) {
      const JSHistogram & h = el.summary().histo();
      unsigned tally = 0;
      for (size_t i = 0; i < (size_t) h.bucket_vals_size(); ++i)
        tally += h.bucket_vals(i);
      buf << "histogram(" << h.bucket_vals_size()<<")-" << tally << " elems";
    } else {
      buf << "misc. summary";
    }
  } else if (el.has_blob()) {
    buf << el.blob().size() << "-byte blob";
  } else
    if(!had_el)
      buf << "UNDEF";
}

std::string fmt(const jetstream::Tuple& t) {
  std::ostringstream buf;
  buf << "(";
  if (t.e_size() > 0) {
    add_one_el(buf, t.e(0));
  }
  for (int i =1; i < t.e_size(); ++i) {
    buf << ",";
    add_one_el(buf, t.e(i));
  }
  buf<< ")";
  return buf.str();
}

std::string fmt(const jetstream::Element& e) {
  std::ostringstream buf;
  buf << "[";
    add_one_el(buf, e);
  buf<< "]";
  return buf.str();
}

void set_thread_name(std::string name) {
#ifdef __MACH__
#include <pthread.h>
pthread_setname_np(name.c_str());
#else
prctl(PR_SET_NAME,name.c_str(),0,0,0);
#endif
}

TaskMeta*
add_operator_to_alter(AlterTopo& topo, operator_id_t dest_id, const std::string& name) {
  TaskMeta* task = topo.add_tostart();
  TaskID* id = task->mutable_id();
  id->set_computationid(dest_id.computation_id);
  id->set_task(dest_id.task_id);
  task->set_op_typename(name);
  return task;
}



jetstream::CubeSchema_Dimension *
 add_dimension(CubeMeta* m, CubeSchema_Dimension_DimensionType d, const std::string& name, int idx) {
  jetstream::CubeSchema * sc = m->mutable_schema();
  jetstream::CubeSchema_Dimension * dim = sc->add_dimensions();
  dim->set_type(d);
  dim->set_name(name);
  dim->add_tuple_indexes(idx);
  return dim;
}

jetstream::CubeSchema_Aggregate *
add_aggregate(CubeMeta* m, const std::string& agg_name, const std::string& name, int idx) {
  jetstream::CubeSchema * sc = m->mutable_schema();
  jetstream::CubeSchema_Aggregate * agg = sc->add_aggregates();
  agg->set_name(name);
  agg->set_type(agg_name);
  agg->add_tuple_indexes(idx);
  if (agg_name == "avg")
    agg->add_tuple_indexes(idx + 1);

  return agg;
}

Edge *
add_edge_to_alter(AlterTopo& topo, operator_id_t src_id, operator_id_t dest_id) {
  LOG_IF(FATAL, src_id.computation_id !=  dest_id.computation_id) << "can't add edge from "
    << src_id << " to " << dest_id;
  return add_edge_to_alter(topo, src_id.computation_id, src_id.task_id, dest_id.task_id);
}


Edge *
add_edge_to_alter(AlterTopo& topo, int computation, int src_id, int dest_id) {
  Edge * edge = topo.add_edges();

  edge->set_computation(computation);
  edge->set_dest(dest_id);
  edge->set_src(src_id);
  return edge;
}


Edge *
add_edge_to_alter(AlterTopo& topo, std::string src_id, operator_id_t dest_id) {
  Edge * edge = topo.add_edges();
  edge->set_computation(dest_id.computation_id);
  edge->set_dest(dest_id.task_id);
  edge->set_src_cube(src_id);
  return edge;
}

Edge *
add_edge_to_alter(AlterTopo& topo, operator_id_t src_id, std::string dest_id) {
  Edge * edge = topo.add_edges();
  edge->set_computation(src_id.computation_id);
  edge->set_dest_cube(dest_id);
  edge->set_src(src_id.task_id);
  return edge;
}


void
add_cfg_to_task(TaskMeta* task, std::string optname, std::string val) {
  TaskMeta_DictEntry* op_cfg = task->add_config();
  op_cfg->set_opt_name(optname);
  op_cfg->set_val(val);
}


}

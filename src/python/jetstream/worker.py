import asyncore
import asynchat

import logging
import socket
import struct
import subprocess
import threading
import time

from jetstream_types_pb2 import *
from jetstream_controlplane_pb2 import *
from jetstream_dataplane_pb2 import *
from local_controller import LocalUnix

from generic_netinterface import JSServer
from controller import DEFAULT_BIND_PORT

logger = logging.getLogger('JetStream')
def main():
#  Could read config here
  
  server_address = ('localhost', DEFAULT_BIND_PORT)
  worker_thread = create_worker(server_address)
  print "connected, starting heartbeat thread" 
  worker_thread.heartbeat_thread()
 
def create_worker(server_address):
  my_address = ('localhost', 0) 
  cli_loop = Worker(my_address)
  cli_loop.connect_to_server(server_address)
  cli_loop.start_as_thread()
  return cli_loop


class Worker(JSServer):
  
  def __init__(self, addr):
    JSServer.__init__(self, addr)
    self.tasks = {}
    self.looping = True
    self.hbThread = None

  def stop(self):
    self.stop_heartbeat_thread()
    JSServer.stop(self)

  def connect_to_server(self, server_address):
    self.connection_to_server = self.connect_to(server_address)

  def handle_deploy(self, altertopo):
    #TODO: Assumes operators are all command line execs
    print "IN WORKER DEPLOY!!"
    for task in altertopo.toStart:
      if task.cmd != "":
        #TODO: Why is TaskID unhashable?
        print "STARTING NEW TASK"
        self.tasks[task.id.task] = LocalUnix(task.id.task, task.cmd)
        self.tasks[task.id.task].start()
    for taskId in altertopo.taskToStop:
      if taskId.task in self.tasks:
        #TODO Add stop() method to operator interface and stop tasks here
        del self.tasks[taskId.task]
    print "LEAVING WORKER DEPLOY!!"

  def process_message(self, buf, handler):
  
    req = WorkerRequest()
    req.ParseFromString(buf)
    response = WorkerResponse()
    # Always send some message so length is not 0
    response.error = "No error"
    if req.type == WorkerRequest.ALTER:
      self.handle_deploy(req.alteration)
    handler.send_pb(response)
    
  def start_heartbeat_thread(self):
    self.hbThread = threading.Thread(group = None, target =self.heartbeat_thread, args = ())
    self.hbThread.daemon = True
    self.hbThread.start()

  def stop_heartbeat_thread(self):
    self.looping = False
    if (self.hbThread != None) and (self.hbThread.is_alive()):
      self.hbThread.join()

  def heartbeat_thread(self):
    req = ServerRequest()
    while self.looping:
      print "sending HB"
      req.type = ServerRequest.HEARTBEAT
      req.heartbeat.freemem_mb = 100
      req.heartbeat.cpuload_pct = 100 #TODO: find real values here
      self.connection_to_server.send_pb(req)
  
      time.sleep(3)

if __name__ == '__main__':
  main()
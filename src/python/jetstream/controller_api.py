
class ControllerAPI():
  def all_nodes(self):
    """Returns a list of all nodes in the system."""
    raise "Unimplemented!"
    
  def get_a_node(self):
    """Picks a node for work assignment. Will round-robin around nodes"""
    raise "Unimplemented!"

  def deploy(self, op_graph):
    """Deploys an operator graph"""
    raise "Unimplemented!"

  def stop_computation(self, comput_id):
    raise "Unimplemented!"

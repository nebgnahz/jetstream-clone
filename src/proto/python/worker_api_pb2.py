# Generated by the protocol buffer compiler.  DO NOT EDIT!

from google.protobuf import descriptor
from google.protobuf import message
from google.protobuf import reflection
from google.protobuf import descriptor_pb2
# @@protoc_insertion_point(imports)


import jetstream_types_pb2

DESCRIPTOR = descriptor.FileDescriptor(
  name='worker_api.proto',
  package='edu.princeton.jetstream',
  serialized_pb='\n\x10worker_api.proto\x12\x17\x65\x64u.princeton.jetstream\x1a\x15jetstream_types.proto\";\n\x0bgetTaskMeta\x12,\n\x03tid\x18\x01 \x02(\x0b\x32\x1f.edu.princeton.jetstream.TaskID')




_GETTASKMETA = descriptor.Descriptor(
  name='getTaskMeta',
  full_name='edu.princeton.jetstream.getTaskMeta',
  filename=None,
  file=DESCRIPTOR,
  containing_type=None,
  fields=[
    descriptor.FieldDescriptor(
      name='tid', full_name='edu.princeton.jetstream.getTaskMeta.tid', index=0,
      number=1, type=11, cpp_type=10, label=2,
      has_default_value=False, default_value=None,
      message_type=None, enum_type=None, containing_type=None,
      is_extension=False, extension_scope=None,
      options=None),
  ],
  extensions=[
  ],
  nested_types=[],
  enum_types=[
  ],
  options=None,
  is_extendable=False,
  extension_ranges=[],
  serialized_start=68,
  serialized_end=127,
)

_GETTASKMETA.fields_by_name['tid'].message_type = jetstream_types_pb2._TASKID
DESCRIPTOR.message_types_by_name['getTaskMeta'] = _GETTASKMETA

class getTaskMeta(message.Message):
  __metaclass__ = reflection.GeneratedProtocolMessageType
  DESCRIPTOR = _GETTASKMETA
  
  # @@protoc_insertion_point(class_scope:edu.princeton.jetstream.getTaskMeta)

# @@protoc_insertion_point(module_scope)

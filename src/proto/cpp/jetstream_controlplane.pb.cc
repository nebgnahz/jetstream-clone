// Generated by the protocol buffer compiler.  DO NOT EDIT!

#define INTERNAL_SUPPRESS_PROTOBUF_FIELD_DEPRECATION
#include "jetstream_controlplane.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/once.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)

namespace edu {
namespace princeton {
namespace jetstream {

namespace {

const ::google::protobuf::Descriptor* AlterTopo_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  AlterTopo_reflection_ = NULL;
const ::google::protobuf::Descriptor* ServerRequest_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ServerRequest_reflection_ = NULL;
const ::google::protobuf::EnumDescriptor* ServerRequest_Type_descriptor_ = NULL;
const ::google::protobuf::Descriptor* ServerResponse_descriptor_ = NULL;
const ::google::protobuf::internal::GeneratedMessageReflection*
  ServerResponse_reflection_ = NULL;

}  // namespace


void protobuf_AssignDesc_jetstream_5fcontrolplane_2eproto() {
  protobuf_AddDesc_jetstream_5fcontrolplane_2eproto();
  const ::google::protobuf::FileDescriptor* file =
    ::google::protobuf::DescriptorPool::generated_pool()->FindFileByName(
      "jetstream_controlplane.proto");
  GOOGLE_CHECK(file != NULL);
  AlterTopo_descriptor_ = file->message_type(0);
  static const int AlterTopo_offsets_[5] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AlterTopo, tostart_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AlterTopo, tocreate_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AlterTopo, tasktostop_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AlterTopo, tabletostop_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AlterTopo, edges_),
  };
  AlterTopo_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      AlterTopo_descriptor_,
      AlterTopo::default_instance_,
      AlterTopo_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AlterTopo, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(AlterTopo, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(AlterTopo));
  ServerRequest_descriptor_ = file->message_type(1);
  static const int ServerRequest_offsets_[2] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerRequest, type_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerRequest, alter_),
  };
  ServerRequest_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ServerRequest_descriptor_,
      ServerRequest::default_instance_,
      ServerRequest_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerRequest, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerRequest, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ServerRequest));
  ServerRequest_Type_descriptor_ = ServerRequest_descriptor_->enum_type(0);
  ServerResponse_descriptor_ = file->message_type(2);
  static const int ServerResponse_offsets_[3] = {
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerResponse, error_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerResponse, nodes_),
    GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerResponse, count_nodes_),
  };
  ServerResponse_reflection_ =
    new ::google::protobuf::internal::GeneratedMessageReflection(
      ServerResponse_descriptor_,
      ServerResponse::default_instance_,
      ServerResponse_offsets_,
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerResponse, _has_bits_[0]),
      GOOGLE_PROTOBUF_GENERATED_MESSAGE_FIELD_OFFSET(ServerResponse, _unknown_fields_),
      -1,
      ::google::protobuf::DescriptorPool::generated_pool(),
      ::google::protobuf::MessageFactory::generated_factory(),
      sizeof(ServerResponse));
}

namespace {

GOOGLE_PROTOBUF_DECLARE_ONCE(protobuf_AssignDescriptors_once_);
inline void protobuf_AssignDescriptorsOnce() {
  ::google::protobuf::GoogleOnceInit(&protobuf_AssignDescriptors_once_,
                 &protobuf_AssignDesc_jetstream_5fcontrolplane_2eproto);
}

void protobuf_RegisterTypes(const ::std::string&) {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    AlterTopo_descriptor_, &AlterTopo::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ServerRequest_descriptor_, &ServerRequest::default_instance());
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedMessage(
    ServerResponse_descriptor_, &ServerResponse::default_instance());
}

}  // namespace

void protobuf_ShutdownFile_jetstream_5fcontrolplane_2eproto() {
  delete AlterTopo::default_instance_;
  delete AlterTopo_reflection_;
  delete ServerRequest::default_instance_;
  delete ServerRequest_reflection_;
  delete ServerResponse::default_instance_;
  delete ServerResponse_reflection_;
}

void protobuf_AddDesc_jetstream_5fcontrolplane_2eproto() {
  static bool already_here = false;
  if (already_here) return;
  already_here = true;
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  ::edu::princeton::jetstream::protobuf_AddDesc_jetstream_5ftypes_2eproto();
  ::google::protobuf::DescriptorPool::InternalAddGeneratedFile(
    "\n\034jetstream_controlplane.proto\022\027edu.prin"
    "ceton.jetstream\032\025jetstream_types.proto\"\355"
    "\001\n\tAlterTopo\0222\n\007toStart\030\001 \003(\0132!.edu.prin"
    "ceton.jetstream.TaskMeta\0224\n\010toCreate\030\002 \003"
    "(\0132\".edu.princeton.jetstream.TableMeta\0223"
    "\n\ntaskToStop\030\003 \003(\0132\037.edu.princeton.jetst"
    "ream.TaskID\022\023\n\013tableToStop\030\004 \003(\t\022,\n\005edge"
    "s\030\005 \003(\0132\035.edu.princeton.jetstream.Edge\"\261"
    "\001\n\rServerRequest\0229\n\004type\030\001 \002(\0162+.edu.pri"
    "nceton.jetstream.ServerRequest.Type\0221\n\005a"
    "lter\030\003 \001(\0132\".edu.princeton.jetstream.Alt"
    "erTopo\"2\n\004Type\022\r\n\tGET_NODES\020\001\022\n\n\006DEPLOY\020"
    "\003\022\017\n\013COUNT_NODES\020\004\"g\n\016ServerResponse\022\016\n\005"
    "error\030\223\001 \001(\t\022/\n\005nodes\030\224\001 \003(\0132\037.edu.princ"
    "eton.jetstream.NodeID\022\024\n\013count_nodes\030\225\001 "
    "\001(\005", 603);
  ::google::protobuf::MessageFactory::InternalRegisterGeneratedFile(
    "jetstream_controlplane.proto", &protobuf_RegisterTypes);
  AlterTopo::default_instance_ = new AlterTopo();
  ServerRequest::default_instance_ = new ServerRequest();
  ServerResponse::default_instance_ = new ServerResponse();
  AlterTopo::default_instance_->InitAsDefaultInstance();
  ServerRequest::default_instance_->InitAsDefaultInstance();
  ServerResponse::default_instance_->InitAsDefaultInstance();
  ::google::protobuf::internal::OnShutdown(&protobuf_ShutdownFile_jetstream_5fcontrolplane_2eproto);
}

// Force AddDescriptors() to be called at static initialization time.
struct StaticDescriptorInitializer_jetstream_5fcontrolplane_2eproto {
  StaticDescriptorInitializer_jetstream_5fcontrolplane_2eproto() {
    protobuf_AddDesc_jetstream_5fcontrolplane_2eproto();
  }
} static_descriptor_initializer_jetstream_5fcontrolplane_2eproto_;


// ===================================================================

#ifndef _MSC_VER
const int AlterTopo::kToStartFieldNumber;
const int AlterTopo::kToCreateFieldNumber;
const int AlterTopo::kTaskToStopFieldNumber;
const int AlterTopo::kTableToStopFieldNumber;
const int AlterTopo::kEdgesFieldNumber;
#endif  // !_MSC_VER

AlterTopo::AlterTopo()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void AlterTopo::InitAsDefaultInstance() {
}

AlterTopo::AlterTopo(const AlterTopo& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void AlterTopo::SharedCtor() {
  _cached_size_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

AlterTopo::~AlterTopo() {
  SharedDtor();
}

void AlterTopo::SharedDtor() {
  if (this != default_instance_) {
  }
}

void AlterTopo::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* AlterTopo::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return AlterTopo_descriptor_;
}

const AlterTopo& AlterTopo::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_jetstream_5fcontrolplane_2eproto();  return *default_instance_;
}

AlterTopo* AlterTopo::default_instance_ = NULL;

AlterTopo* AlterTopo::New() const {
  return new AlterTopo;
}

void AlterTopo::Clear() {
  tostart_.Clear();
  tocreate_.Clear();
  tasktostop_.Clear();
  tabletostop_.Clear();
  edges_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool AlterTopo::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated .edu.princeton.jetstream.TaskMeta toStart = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_toStart:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_tostart()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(10)) goto parse_toStart;
        if (input->ExpectTag(18)) goto parse_toCreate;
        break;
      }
      
      // repeated .edu.princeton.jetstream.TableMeta toCreate = 2;
      case 2: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_toCreate:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_tocreate()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(18)) goto parse_toCreate;
        if (input->ExpectTag(26)) goto parse_taskToStop;
        break;
      }
      
      // repeated .edu.princeton.jetstream.TaskID taskToStop = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_taskToStop:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_tasktostop()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_taskToStop;
        if (input->ExpectTag(34)) goto parse_tableToStop;
        break;
      }
      
      // repeated string tableToStop = 4;
      case 4: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_tableToStop:
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->add_tabletostop()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->tabletostop(0).data(), this->tabletostop(0).length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(34)) goto parse_tableToStop;
        if (input->ExpectTag(42)) goto parse_edges;
        break;
      }
      
      // repeated .edu.princeton.jetstream.Edge edges = 5;
      case 5: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_edges:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_edges()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(42)) goto parse_edges;
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void AlterTopo::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // repeated .edu.princeton.jetstream.TaskMeta toStart = 1;
  for (int i = 0; i < this->tostart_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, this->tostart(i), output);
  }
  
  // repeated .edu.princeton.jetstream.TableMeta toCreate = 2;
  for (int i = 0; i < this->tocreate_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, this->tocreate(i), output);
  }
  
  // repeated .edu.princeton.jetstream.TaskID taskToStop = 3;
  for (int i = 0; i < this->tasktostop_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      3, this->tasktostop(i), output);
  }
  
  // repeated string tableToStop = 4;
  for (int i = 0; i < this->tabletostop_size(); i++) {
  ::google::protobuf::internal::WireFormat::VerifyUTF8String(
    this->tabletostop(i).data(), this->tabletostop(i).length(),
    ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      4, this->tabletostop(i), output);
  }
  
  // repeated .edu.princeton.jetstream.Edge edges = 5;
  for (int i = 0; i < this->edges_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      5, this->edges(i), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* AlterTopo::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // repeated .edu.princeton.jetstream.TaskMeta toStart = 1;
  for (int i = 0; i < this->tostart_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        1, this->tostart(i), target);
  }
  
  // repeated .edu.princeton.jetstream.TableMeta toCreate = 2;
  for (int i = 0; i < this->tocreate_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        2, this->tocreate(i), target);
  }
  
  // repeated .edu.princeton.jetstream.TaskID taskToStop = 3;
  for (int i = 0; i < this->tasktostop_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        3, this->tasktostop(i), target);
  }
  
  // repeated string tableToStop = 4;
  for (int i = 0; i < this->tabletostop_size(); i++) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->tabletostop(i).data(), this->tabletostop(i).length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target = ::google::protobuf::internal::WireFormatLite::
      WriteStringToArray(4, this->tabletostop(i), target);
  }
  
  // repeated .edu.princeton.jetstream.Edge edges = 5;
  for (int i = 0; i < this->edges_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        5, this->edges(i), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int AlterTopo::ByteSize() const {
  int total_size = 0;
  
  // repeated .edu.princeton.jetstream.TaskMeta toStart = 1;
  total_size += 1 * this->tostart_size();
  for (int i = 0; i < this->tostart_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->tostart(i));
  }
  
  // repeated .edu.princeton.jetstream.TableMeta toCreate = 2;
  total_size += 1 * this->tocreate_size();
  for (int i = 0; i < this->tocreate_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->tocreate(i));
  }
  
  // repeated .edu.princeton.jetstream.TaskID taskToStop = 3;
  total_size += 1 * this->tasktostop_size();
  for (int i = 0; i < this->tasktostop_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->tasktostop(i));
  }
  
  // repeated string tableToStop = 4;
  total_size += 1 * this->tabletostop_size();
  for (int i = 0; i < this->tabletostop_size(); i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::StringSize(
      this->tabletostop(i));
  }
  
  // repeated .edu.princeton.jetstream.Edge edges = 5;
  total_size += 1 * this->edges_size();
  for (int i = 0; i < this->edges_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->edges(i));
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void AlterTopo::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const AlterTopo* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const AlterTopo*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void AlterTopo::MergeFrom(const AlterTopo& from) {
  GOOGLE_CHECK_NE(&from, this);
  tostart_.MergeFrom(from.tostart_);
  tocreate_.MergeFrom(from.tocreate_);
  tasktostop_.MergeFrom(from.tasktostop_);
  tabletostop_.MergeFrom(from.tabletostop_);
  edges_.MergeFrom(from.edges_);
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void AlterTopo::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void AlterTopo::CopyFrom(const AlterTopo& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool AlterTopo::IsInitialized() const {
  
  for (int i = 0; i < tostart_size(); i++) {
    if (!this->tostart(i).IsInitialized()) return false;
  }
  for (int i = 0; i < tocreate_size(); i++) {
    if (!this->tocreate(i).IsInitialized()) return false;
  }
  for (int i = 0; i < tasktostop_size(); i++) {
    if (!this->tasktostop(i).IsInitialized()) return false;
  }
  for (int i = 0; i < edges_size(); i++) {
    if (!this->edges(i).IsInitialized()) return false;
  }
  return true;
}

void AlterTopo::Swap(AlterTopo* other) {
  if (other != this) {
    tostart_.Swap(&other->tostart_);
    tocreate_.Swap(&other->tocreate_);
    tasktostop_.Swap(&other->tasktostop_);
    tabletostop_.Swap(&other->tabletostop_);
    edges_.Swap(&other->edges_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata AlterTopo::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = AlterTopo_descriptor_;
  metadata.reflection = AlterTopo_reflection_;
  return metadata;
}


// ===================================================================

const ::google::protobuf::EnumDescriptor* ServerRequest_Type_descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ServerRequest_Type_descriptor_;
}
bool ServerRequest_Type_IsValid(int value) {
  switch(value) {
    case 1:
    case 3:
    case 4:
      return true;
    default:
      return false;
  }
}

#ifndef _MSC_VER
const ServerRequest_Type ServerRequest::GET_NODES;
const ServerRequest_Type ServerRequest::DEPLOY;
const ServerRequest_Type ServerRequest::COUNT_NODES;
const ServerRequest_Type ServerRequest::Type_MIN;
const ServerRequest_Type ServerRequest::Type_MAX;
const int ServerRequest::Type_ARRAYSIZE;
#endif  // _MSC_VER
#ifndef _MSC_VER
const int ServerRequest::kTypeFieldNumber;
const int ServerRequest::kAlterFieldNumber;
#endif  // !_MSC_VER

ServerRequest::ServerRequest()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ServerRequest::InitAsDefaultInstance() {
  alter_ = const_cast< ::edu::princeton::jetstream::AlterTopo*>(&::edu::princeton::jetstream::AlterTopo::default_instance());
}

ServerRequest::ServerRequest(const ServerRequest& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ServerRequest::SharedCtor() {
  _cached_size_ = 0;
  type_ = 1;
  alter_ = NULL;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ServerRequest::~ServerRequest() {
  SharedDtor();
}

void ServerRequest::SharedDtor() {
  if (this != default_instance_) {
    delete alter_;
  }
}

void ServerRequest::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ServerRequest::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ServerRequest_descriptor_;
}

const ServerRequest& ServerRequest::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_jetstream_5fcontrolplane_2eproto();  return *default_instance_;
}

ServerRequest* ServerRequest::default_instance_ = NULL;

ServerRequest* ServerRequest::New() const {
  return new ServerRequest;
}

void ServerRequest::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    type_ = 1;
    if (has_alter()) {
      if (alter_ != NULL) alter_->::edu::princeton::jetstream::AlterTopo::Clear();
    }
  }
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ServerRequest::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // required .edu.princeton.jetstream.ServerRequest.Type type = 1;
      case 1: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
          int value;
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   int, ::google::protobuf::internal::WireFormatLite::TYPE_ENUM>(
                 input, &value)));
          if (::edu::princeton::jetstream::ServerRequest_Type_IsValid(value)) {
            set_type(static_cast< ::edu::princeton::jetstream::ServerRequest_Type >(value));
          } else {
            mutable_unknown_fields()->AddVarint(1, value);
          }
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(26)) goto parse_alter;
        break;
      }
      
      // optional .edu.princeton.jetstream.AlterTopo alter = 3;
      case 3: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_alter:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
               input, mutable_alter()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ServerRequest::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // required .edu.princeton.jetstream.ServerRequest.Type type = 1;
  if (has_type()) {
    ::google::protobuf::internal::WireFormatLite::WriteEnum(
      1, this->type(), output);
  }
  
  // optional .edu.princeton.jetstream.AlterTopo alter = 3;
  if (has_alter()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      3, this->alter(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ServerRequest::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // required .edu.princeton.jetstream.ServerRequest.Type type = 1;
  if (has_type()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteEnumToArray(
      1, this->type(), target);
  }
  
  // optional .edu.princeton.jetstream.AlterTopo alter = 3;
  if (has_alter()) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        3, this->alter(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ServerRequest::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // required .edu.princeton.jetstream.ServerRequest.Type type = 1;
    if (has_type()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::EnumSize(this->type());
    }
    
    // optional .edu.princeton.jetstream.AlterTopo alter = 3;
    if (has_alter()) {
      total_size += 1 +
        ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
          this->alter());
    }
    
  }
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ServerRequest::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ServerRequest* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ServerRequest*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ServerRequest::MergeFrom(const ServerRequest& from) {
  GOOGLE_CHECK_NE(&from, this);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_type()) {
      set_type(from.type());
    }
    if (from.has_alter()) {
      mutable_alter()->::edu::princeton::jetstream::AlterTopo::MergeFrom(from.alter());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ServerRequest::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ServerRequest::CopyFrom(const ServerRequest& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ServerRequest::IsInitialized() const {
  if ((_has_bits_[0] & 0x00000001) != 0x00000001) return false;
  
  if (has_alter()) {
    if (!this->alter().IsInitialized()) return false;
  }
  return true;
}

void ServerRequest::Swap(ServerRequest* other) {
  if (other != this) {
    std::swap(type_, other->type_);
    std::swap(alter_, other->alter_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ServerRequest::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ServerRequest_descriptor_;
  metadata.reflection = ServerRequest_reflection_;
  return metadata;
}


// ===================================================================

#ifndef _MSC_VER
const int ServerResponse::kErrorFieldNumber;
const int ServerResponse::kNodesFieldNumber;
const int ServerResponse::kCountNodesFieldNumber;
#endif  // !_MSC_VER

ServerResponse::ServerResponse()
  : ::google::protobuf::Message() {
  SharedCtor();
}

void ServerResponse::InitAsDefaultInstance() {
}

ServerResponse::ServerResponse(const ServerResponse& from)
  : ::google::protobuf::Message() {
  SharedCtor();
  MergeFrom(from);
}

void ServerResponse::SharedCtor() {
  _cached_size_ = 0;
  error_ = const_cast< ::std::string*>(&::google::protobuf::internal::kEmptyString);
  count_nodes_ = 0;
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
}

ServerResponse::~ServerResponse() {
  SharedDtor();
}

void ServerResponse::SharedDtor() {
  if (error_ != &::google::protobuf::internal::kEmptyString) {
    delete error_;
  }
  if (this != default_instance_) {
  }
}

void ServerResponse::SetCachedSize(int size) const {
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
}
const ::google::protobuf::Descriptor* ServerResponse::descriptor() {
  protobuf_AssignDescriptorsOnce();
  return ServerResponse_descriptor_;
}

const ServerResponse& ServerResponse::default_instance() {
  if (default_instance_ == NULL) protobuf_AddDesc_jetstream_5fcontrolplane_2eproto();  return *default_instance_;
}

ServerResponse* ServerResponse::default_instance_ = NULL;

ServerResponse* ServerResponse::New() const {
  return new ServerResponse;
}

void ServerResponse::Clear() {
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (has_error()) {
      if (error_ != &::google::protobuf::internal::kEmptyString) {
        error_->clear();
      }
    }
    count_nodes_ = 0;
  }
  nodes_.Clear();
  ::memset(_has_bits_, 0, sizeof(_has_bits_));
  mutable_unknown_fields()->Clear();
}

bool ServerResponse::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!(EXPRESSION)) return false
  ::google::protobuf::uint32 tag;
  while ((tag = input->ReadTag()) != 0) {
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // optional string error = 147;
      case 147: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_error()));
          ::google::protobuf::internal::WireFormat::VerifyUTF8String(
            this->error().data(), this->error().length(),
            ::google::protobuf::internal::WireFormat::PARSE);
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(1186)) goto parse_nodes;
        break;
      }
      
      // repeated .edu.princeton.jetstream.NodeID nodes = 148;
      case 148: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_LENGTH_DELIMITED) {
         parse_nodes:
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
                input, add_nodes()));
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectTag(1186)) goto parse_nodes;
        if (input->ExpectTag(1192)) goto parse_count_nodes;
        break;
      }
      
      // optional int32 count_nodes = 149;
      case 149: {
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_VARINT) {
         parse_count_nodes:
          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &count_nodes_)));
          set_has_count_nodes();
        } else {
          goto handle_uninterpreted;
        }
        if (input->ExpectAtEnd()) return true;
        break;
      }
      
      default: {
      handle_uninterpreted:
        if (::google::protobuf::internal::WireFormatLite::GetTagWireType(tag) ==
            ::google::protobuf::internal::WireFormatLite::WIRETYPE_END_GROUP) {
          return true;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, mutable_unknown_fields()));
        break;
      }
    }
  }
  return true;
#undef DO_
}

void ServerResponse::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // optional string error = 147;
  if (has_error()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->error().data(), this->error().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    ::google::protobuf::internal::WireFormatLite::WriteString(
      147, this->error(), output);
  }
  
  // repeated .edu.princeton.jetstream.NodeID nodes = 148;
  for (int i = 0; i < this->nodes_size(); i++) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      148, this->nodes(i), output);
  }
  
  // optional int32 count_nodes = 149;
  if (has_count_nodes()) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(149, this->count_nodes(), output);
  }
  
  if (!unknown_fields().empty()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        unknown_fields(), output);
  }
}

::google::protobuf::uint8* ServerResponse::SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* target) const {
  // optional string error = 147;
  if (has_error()) {
    ::google::protobuf::internal::WireFormat::VerifyUTF8String(
      this->error().data(), this->error().length(),
      ::google::protobuf::internal::WireFormat::SERIALIZE);
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        147, this->error(), target);
  }
  
  // repeated .edu.princeton.jetstream.NodeID nodes = 148;
  for (int i = 0; i < this->nodes_size(); i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteMessageNoVirtualToArray(
        148, this->nodes(i), target);
  }
  
  // optional int32 count_nodes = 149;
  if (has_count_nodes()) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(149, this->count_nodes(), target);
  }
  
  if (!unknown_fields().empty()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        unknown_fields(), target);
  }
  return target;
}

int ServerResponse::ByteSize() const {
  int total_size = 0;
  
  if (_has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    // optional string error = 147;
    if (has_error()) {
      total_size += 2 +
        ::google::protobuf::internal::WireFormatLite::StringSize(
          this->error());
    }
    
    // optional int32 count_nodes = 149;
    if (has_count_nodes()) {
      total_size += 2 +
        ::google::protobuf::internal::WireFormatLite::Int32Size(
          this->count_nodes());
    }
    
  }
  // repeated .edu.princeton.jetstream.NodeID nodes = 148;
  total_size += 2 * this->nodes_size();
  for (int i = 0; i < this->nodes_size(); i++) {
    total_size +=
      ::google::protobuf::internal::WireFormatLite::MessageSizeNoVirtual(
        this->nodes(i));
  }
  
  if (!unknown_fields().empty()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        unknown_fields());
  }
  GOOGLE_SAFE_CONCURRENT_WRITES_BEGIN();
  _cached_size_ = total_size;
  GOOGLE_SAFE_CONCURRENT_WRITES_END();
  return total_size;
}

void ServerResponse::MergeFrom(const ::google::protobuf::Message& from) {
  GOOGLE_CHECK_NE(&from, this);
  const ServerResponse* source =
    ::google::protobuf::internal::dynamic_cast_if_available<const ServerResponse*>(
      &from);
  if (source == NULL) {
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
    MergeFrom(*source);
  }
}

void ServerResponse::MergeFrom(const ServerResponse& from) {
  GOOGLE_CHECK_NE(&from, this);
  nodes_.MergeFrom(from.nodes_);
  if (from._has_bits_[0 / 32] & (0xffu << (0 % 32))) {
    if (from.has_error()) {
      set_error(from.error());
    }
    if (from.has_count_nodes()) {
      set_count_nodes(from.count_nodes());
    }
  }
  mutable_unknown_fields()->MergeFrom(from.unknown_fields());
}

void ServerResponse::CopyFrom(const ::google::protobuf::Message& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void ServerResponse::CopyFrom(const ServerResponse& from) {
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool ServerResponse::IsInitialized() const {
  
  for (int i = 0; i < nodes_size(); i++) {
    if (!this->nodes(i).IsInitialized()) return false;
  }
  return true;
}

void ServerResponse::Swap(ServerResponse* other) {
  if (other != this) {
    std::swap(error_, other->error_);
    nodes_.Swap(&other->nodes_);
    std::swap(count_nodes_, other->count_nodes_);
    std::swap(_has_bits_[0], other->_has_bits_[0]);
    _unknown_fields_.Swap(&other->_unknown_fields_);
    std::swap(_cached_size_, other->_cached_size_);
  }
}

::google::protobuf::Metadata ServerResponse::GetMetadata() const {
  protobuf_AssignDescriptorsOnce();
  ::google::protobuf::Metadata metadata;
  metadata.descriptor = ServerResponse_descriptor_;
  metadata.reflection = ServerResponse_reflection_;
  return metadata;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace jetstream
}  // namespace princeton
}  // namespace edu

// @@protoc_insertion_point(global_scope)

// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: jetstream_dataplane.proto

#ifndef PROTOBUF_jetstream_5fdataplane_2eproto__INCLUDED
#define PROTOBUF_jetstream_5fdataplane_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 2004000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 2004001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_message_reflection.h>
#include "jetstream_types.pb.h"
// @@protoc_insertion_point(includes)

namespace edu {
namespace princeton {
namespace jetstream {

// Internal implementation detail -- do not call these.
void  protobuf_AddDesc_jetstream_5fdataplane_2eproto();
void protobuf_AssignDesc_jetstream_5fdataplane_2eproto();
void protobuf_ShutdownFile_jetstream_5fdataplane_2eproto();

class TableRequest;
class TableResponse;

// ===================================================================

class TableRequest : public ::google::protobuf::Message {
 public:
  TableRequest();
  virtual ~TableRequest();
  
  TableRequest(const TableRequest& from);
  
  inline TableRequest& operator=(const TableRequest& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const TableRequest& default_instance();
  
  void Swap(TableRequest* other);
  
  // implements Message ----------------------------------------------
  
  TableRequest* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TableRequest& from);
  void MergeFrom(const TableRequest& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // repeated .edu.princeton.jetstream.TableName names = 1;
  inline int names_size() const;
  inline void clear_names();
  static const int kNamesFieldNumber = 1;
  inline const ::edu::princeton::jetstream::TableName& names(int index) const;
  inline ::edu::princeton::jetstream::TableName* mutable_names(int index);
  inline ::edu::princeton::jetstream::TableName* add_names();
  inline const ::google::protobuf::RepeatedPtrField< ::edu::princeton::jetstream::TableName >&
      names() const;
  inline ::google::protobuf::RepeatedPtrField< ::edu::princeton::jetstream::TableName >*
      mutable_names();
  
  // @@protoc_insertion_point(class_scope:edu.princeton.jetstream.TableRequest)
 private:
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::google::protobuf::RepeatedPtrField< ::edu::princeton::jetstream::TableName > names_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_jetstream_5fdataplane_2eproto();
  friend void protobuf_AssignDesc_jetstream_5fdataplane_2eproto();
  friend void protobuf_ShutdownFile_jetstream_5fdataplane_2eproto();
  
  void InitAsDefaultInstance();
  static TableRequest* default_instance_;
};
// -------------------------------------------------------------------

class TableResponse : public ::google::protobuf::Message {
 public:
  TableResponse();
  virtual ~TableResponse();
  
  TableResponse(const TableResponse& from);
  
  inline TableResponse& operator=(const TableResponse& from) {
    CopyFrom(from);
    return *this;
  }
  
  inline const ::google::protobuf::UnknownFieldSet& unknown_fields() const {
    return _unknown_fields_;
  }
  
  inline ::google::protobuf::UnknownFieldSet* mutable_unknown_fields() {
    return &_unknown_fields_;
  }
  
  static const ::google::protobuf::Descriptor* descriptor();
  static const TableResponse& default_instance();
  
  void Swap(TableResponse* other);
  
  // implements Message ----------------------------------------------
  
  TableResponse* New() const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const TableResponse& from);
  void MergeFrom(const TableResponse& from);
  void Clear();
  bool IsInitialized() const;
  
  int ByteSize() const;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }
  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  public:
  
  ::google::protobuf::Metadata GetMetadata() const;
  
  // nested types ----------------------------------------------------
  
  // accessors -------------------------------------------------------
  
  // required .edu.princeton.jetstream.Cube tables = 1;
  inline bool has_tables() const;
  inline void clear_tables();
  static const int kTablesFieldNumber = 1;
  inline const ::edu::princeton::jetstream::Cube& tables() const;
  inline ::edu::princeton::jetstream::Cube* mutable_tables();
  inline ::edu::princeton::jetstream::Cube* release_tables();
  
  // @@protoc_insertion_point(class_scope:edu.princeton.jetstream.TableResponse)
 private:
  inline void set_has_tables();
  inline void clear_has_tables();
  
  ::google::protobuf::UnknownFieldSet _unknown_fields_;
  
  ::edu::princeton::jetstream::Cube* tables_;
  
  mutable int _cached_size_;
  ::google::protobuf::uint32 _has_bits_[(1 + 31) / 32];
  
  friend void  protobuf_AddDesc_jetstream_5fdataplane_2eproto();
  friend void protobuf_AssignDesc_jetstream_5fdataplane_2eproto();
  friend void protobuf_ShutdownFile_jetstream_5fdataplane_2eproto();
  
  void InitAsDefaultInstance();
  static TableResponse* default_instance_;
};
// ===================================================================


// ===================================================================

// TableRequest

// repeated .edu.princeton.jetstream.TableName names = 1;
inline int TableRequest::names_size() const {
  return names_.size();
}
inline void TableRequest::clear_names() {
  names_.Clear();
}
inline const ::edu::princeton::jetstream::TableName& TableRequest::names(int index) const {
  return names_.Get(index);
}
inline ::edu::princeton::jetstream::TableName* TableRequest::mutable_names(int index) {
  return names_.Mutable(index);
}
inline ::edu::princeton::jetstream::TableName* TableRequest::add_names() {
  return names_.Add();
}
inline const ::google::protobuf::RepeatedPtrField< ::edu::princeton::jetstream::TableName >&
TableRequest::names() const {
  return names_;
}
inline ::google::protobuf::RepeatedPtrField< ::edu::princeton::jetstream::TableName >*
TableRequest::mutable_names() {
  return &names_;
}

// -------------------------------------------------------------------

// TableResponse

// required .edu.princeton.jetstream.Cube tables = 1;
inline bool TableResponse::has_tables() const {
  return (_has_bits_[0] & 0x00000001u) != 0;
}
inline void TableResponse::set_has_tables() {
  _has_bits_[0] |= 0x00000001u;
}
inline void TableResponse::clear_has_tables() {
  _has_bits_[0] &= ~0x00000001u;
}
inline void TableResponse::clear_tables() {
  if (tables_ != NULL) tables_->::edu::princeton::jetstream::Cube::Clear();
  clear_has_tables();
}
inline const ::edu::princeton::jetstream::Cube& TableResponse::tables() const {
  return tables_ != NULL ? *tables_ : *default_instance_->tables_;
}
inline ::edu::princeton::jetstream::Cube* TableResponse::mutable_tables() {
  set_has_tables();
  if (tables_ == NULL) tables_ = new ::edu::princeton::jetstream::Cube;
  return tables_;
}
inline ::edu::princeton::jetstream::Cube* TableResponse::release_tables() {
  clear_has_tables();
  ::edu::princeton::jetstream::Cube* temp = tables_;
  tables_ = NULL;
  return temp;
}


// @@protoc_insertion_point(namespace_scope)

}  // namespace jetstream
}  // namespace princeton
}  // namespace edu

#ifndef SWIG
namespace google {
namespace protobuf {


}  // namespace google
}  // namespace protobuf
#endif  // SWIG

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_jetstream_5fdataplane_2eproto__INCLUDED

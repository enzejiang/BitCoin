// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Order.proto

#ifndef PROTOBUF_INCLUDED_Order_2eproto
#define PROTOBUF_INCLUDED_Order_2eproto

#include <limits>
#include <string>

#include <google/protobuf/port_def.inc>
#if PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers. Please update
#error your headers.
#endif
#if 3006001 < PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers. Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/port_undef.inc>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
#include "WalletTx.pb.h"
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Order_2eproto

// Internal implementation detail -- do not use these members.
struct TableStruct_Order_2eproto {
  static const ::google::protobuf::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::ParseTable schema[1]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors_Order_2eproto();
namespace Enze {
class Order;
class OrderDefaultTypeInternal;
extern OrderDefaultTypeInternal _Order_default_instance_;
}  // namespace Enze
namespace google {
namespace protobuf {
template<> ::Enze::Order* Arena::CreateMaybeMessage<::Enze::Order>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace Enze {

// ===================================================================

class Order : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:Enze.Order) */ {
 public:
  Order();
  virtual ~Order();

  Order(const Order& from);

  inline Order& operator=(const Order& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  Order(Order&& from) noexcept
    : Order() {
    *this = ::std::move(from);
  }

  inline Order& operator=(Order&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor() {
    return default_instance().GetDescriptor();
  }
  static const Order& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Order* internal_default_instance() {
    return reinterpret_cast<const Order*>(
               &_Order_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(Order* other);
  friend void swap(Order& a, Order& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline Order* New() const final {
    return CreateMaybeMessage<Order>(NULL);
  }

  Order* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<Order>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const Order& from);
  void MergeFrom(const Order& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  #if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  static const char* _InternalParse(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx);
  ::google::protobuf::internal::ParseFunc _ParseFunc() const final { return _InternalParse; }
  #else
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  #endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(Order* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bytes hashReply = 1;
  void clear_hashreply();
  static const int kHashReplyFieldNumber = 1;
  const ::std::string& hashreply() const;
  void set_hashreply(const ::std::string& value);
  #if LANG_CXX11
  void set_hashreply(::std::string&& value);
  #endif
  void set_hashreply(const char* value);
  void set_hashreply(const void* value, size_t size);
  ::std::string* mutable_hashreply();
  ::std::string* release_hashreply();
  void set_allocated_hashreply(::std::string* hashreply);

  // .Enze.WalletTx txOrder = 2;
  bool has_txorder() const;
  void clear_txorder();
  static const int kTxOrderFieldNumber = 2;
  const ::Enze::WalletTx& txorder() const;
  ::Enze::WalletTx* release_txorder();
  ::Enze::WalletTx* mutable_txorder();
  void set_allocated_txorder(::Enze::WalletTx* txorder);

  // @@protoc_insertion_point(class_scope:Enze.Order)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr hashreply_;
  ::Enze::WalletTx* txorder_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_Order_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// Order

// bytes hashReply = 1;
inline void Order::clear_hashreply() {
  hashreply_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Order::hashreply() const {
  // @@protoc_insertion_point(field_get:Enze.Order.hashReply)
  return hashreply_.GetNoArena();
}
inline void Order::set_hashreply(const ::std::string& value) {
  
  hashreply_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Enze.Order.hashReply)
}
#if LANG_CXX11
inline void Order::set_hashreply(::std::string&& value) {
  
  hashreply_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Enze.Order.hashReply)
}
#endif
inline void Order::set_hashreply(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  hashreply_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Enze.Order.hashReply)
}
inline void Order::set_hashreply(const void* value, size_t size) {
  
  hashreply_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Enze.Order.hashReply)
}
inline ::std::string* Order::mutable_hashreply() {
  
  // @@protoc_insertion_point(field_mutable:Enze.Order.hashReply)
  return hashreply_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Order::release_hashreply() {
  // @@protoc_insertion_point(field_release:Enze.Order.hashReply)
  
  return hashreply_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Order::set_allocated_hashreply(::std::string* hashreply) {
  if (hashreply != NULL) {
    
  } else {
    
  }
  hashreply_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), hashreply);
  // @@protoc_insertion_point(field_set_allocated:Enze.Order.hashReply)
}

// .Enze.WalletTx txOrder = 2;
inline bool Order::has_txorder() const {
  return this != internal_default_instance() && txorder_ != NULL;
}
inline const ::Enze::WalletTx& Order::txorder() const {
  const ::Enze::WalletTx* p = txorder_;
  // @@protoc_insertion_point(field_get:Enze.Order.txOrder)
  return p != NULL ? *p : *reinterpret_cast<const ::Enze::WalletTx*>(
      &::Enze::_WalletTx_default_instance_);
}
inline ::Enze::WalletTx* Order::release_txorder() {
  // @@protoc_insertion_point(field_release:Enze.Order.txOrder)
  
  ::Enze::WalletTx* temp = txorder_;
  txorder_ = NULL;
  return temp;
}
inline ::Enze::WalletTx* Order::mutable_txorder() {
  
  if (txorder_ == NULL) {
    auto* p = CreateMaybeMessage<::Enze::WalletTx>(GetArenaNoVirtual());
    txorder_ = p;
  }
  // @@protoc_insertion_point(field_mutable:Enze.Order.txOrder)
  return txorder_;
}
inline void Order::set_allocated_txorder(::Enze::WalletTx* txorder) {
  ::google::protobuf::Arena* message_arena = GetArenaNoVirtual();
  if (message_arena == NULL) {
    delete reinterpret_cast< ::google::protobuf::MessageLite*>(txorder_);
  }
  if (txorder) {
    ::google::protobuf::Arena* submessage_arena = NULL;
    if (message_arena != submessage_arena) {
      txorder = ::google::protobuf::internal::GetOwnedMessage(
          message_arena, txorder, submessage_arena);
    }
    
  } else {
    
  }
  txorder_ = txorder;
  // @@protoc_insertion_point(field_set_allocated:Enze.Order.txOrder)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__

// @@protoc_insertion_point(namespace_scope)

}  // namespace Enze

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // PROTOBUF_INCLUDED_Order_2eproto

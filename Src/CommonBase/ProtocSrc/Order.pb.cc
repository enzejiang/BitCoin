// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Order.proto

#include "Order.pb.h"

#include <algorithm>

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite_inl.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

extern PROTOBUF_INTERNAL_EXPORT_WalletTx_2eproto ::google::protobuf::internal::SCCInfo<5> scc_info_WalletTx_WalletTx_2eproto;
namespace Enze {
class OrderDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<Order> _instance;
} _Order_default_instance_;
}  // namespace Enze
static void InitDefaultsOrder_Order_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Enze::_Order_default_instance_;
    new (ptr) ::Enze::Order();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Enze::Order::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<1> scc_info_Order_Order_2eproto =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 1, InitDefaultsOrder_Order_2eproto}, {
      &scc_info_WalletTx_WalletTx_2eproto.base,}};

void InitDefaults_Order_2eproto() {
  ::google::protobuf::internal::InitSCC(&scc_info_Order_Order_2eproto.base);
}

::google::protobuf::Metadata file_level_metadata_Order_2eproto[1];
constexpr ::google::protobuf::EnumDescriptor const** file_level_enum_descriptors_Order_2eproto = nullptr;
constexpr ::google::protobuf::ServiceDescriptor const** file_level_service_descriptors_Order_2eproto = nullptr;

const ::google::protobuf::uint32 TableStruct_Order_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Enze::Order, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Enze::Order, hashreply_),
  PROTOBUF_FIELD_OFFSET(::Enze::Order, txorder_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Enze::Order)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::Enze::_Order_default_instance_),
};

::google::protobuf::internal::AssignDescriptorsTable assign_descriptors_table_Order_2eproto = {
  {}, AddDescriptors_Order_2eproto, "Order.proto", schemas,
  file_default_instances, TableStruct_Order_2eproto::offsets,
  file_level_metadata_Order_2eproto, 1, file_level_enum_descriptors_Order_2eproto, file_level_service_descriptors_Order_2eproto,
};

::google::protobuf::internal::DescriptorTable descriptor_table_Order_2eproto = {
  false, InitDefaults_Order_2eproto, 
  "\n\013Order.proto\022\004Enze\032\016WalletTx.proto\";\n\005O"
  "rder\022\021\n\thashReply\030\001 \001(\014\022\037\n\007txOrder\030\002 \001(\013"
  "2\016.Enze.WalletTxb\006proto3"
,
  "Order.proto", &assign_descriptors_table_Order_2eproto, 104,
};

void AddDescriptors_Order_2eproto() {
  static constexpr ::google::protobuf::internal::InitFunc deps[1] =
  {
    ::AddDescriptors_WalletTx_2eproto,
  };
 ::google::protobuf::internal::AddDescriptors(&descriptor_table_Order_2eproto, deps, 1);
}

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_Order_2eproto = []() { AddDescriptors_Order_2eproto(); return true; }();
namespace Enze {

// ===================================================================

void Order::InitAsDefaultInstance() {
  ::Enze::_Order_default_instance_._instance.get_mutable()->txorder_ = const_cast< ::Enze::WalletTx*>(
      ::Enze::WalletTx::internal_default_instance());
}
class Order::HasBitSetters {
 public:
  static const ::Enze::WalletTx& txorder(const Order* msg);
};

const ::Enze::WalletTx&
Order::HasBitSetters::txorder(const Order* msg) {
  return *msg->txorder_;
}
void Order::clear_txorder() {
  if (GetArenaNoVirtual() == NULL && txorder_ != NULL) {
    delete txorder_;
  }
  txorder_ = NULL;
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int Order::kHashReplyFieldNumber;
const int Order::kTxOrderFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

Order::Order()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Enze.Order)
}
Order::Order(const Order& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  hashreply_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.hashreply().size() > 0) {
    hashreply_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.hashreply_);
  }
  if (from.has_txorder()) {
    txorder_ = new ::Enze::WalletTx(*from.txorder_);
  } else {
    txorder_ = NULL;
  }
  // @@protoc_insertion_point(copy_constructor:Enze.Order)
}

void Order::SharedCtor() {
  ::google::protobuf::internal::InitSCC(
      &scc_info_Order_Order_2eproto.base);
  hashreply_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  txorder_ = NULL;
}

Order::~Order() {
  // @@protoc_insertion_point(destructor:Enze.Order)
  SharedDtor();
}

void Order::SharedDtor() {
  hashreply_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete txorder_;
}

void Order::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Order& Order::default_instance() {
  ::google::protobuf::internal::InitSCC(&::scc_info_Order_Order_2eproto.base);
  return *internal_default_instance();
}


void Order::Clear() {
// @@protoc_insertion_point(message_clear_start:Enze.Order)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  hashreply_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (GetArenaNoVirtual() == NULL && txorder_ != NULL) {
    delete txorder_;
  }
  txorder_ = NULL;
  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* Order::_InternalParse(const char* begin, const char* end, void* object,
                  ::google::protobuf::internal::ParseContext* ctx) {
  auto msg = static_cast<Order*>(object);
  ::google::protobuf::uint32 size; (void)size;
  int depth; (void)depth;
  ::google::protobuf::internal::ParseFunc parser_till_end; (void)parser_till_end;
  auto ptr = begin;
  while (ptr < end) {
    ::google::protobuf::uint32 tag;
    ptr = Varint::Parse32Inline(ptr, &tag);
    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
    switch (tag >> 3) {
      // bytes hashReply = 1;
      case 1: {
        if (static_cast<::google::protobuf::uint8>(tag) != 10) goto handle_unusual;
        ptr = Varint::Parse32Inline(ptr, &size);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        parser_till_end = ::google::protobuf::internal::StringParser;
        ::std::string* str = msg->mutable_hashreply();
        str->clear();
        object = str;
        if (size > end - ptr) goto len_delim_till_end;
        str->append(ptr, size);
        ptr += size;
        break;
      }
      // .Enze.WalletTx txOrder = 2;
      case 2: {
        if (static_cast<::google::protobuf::uint8>(tag) != 18) goto handle_unusual;
        ptr = Varint::Parse32Inline(ptr, &size);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        parser_till_end = ::Enze::WalletTx::_InternalParse;
        object = msg->mutable_txorder();
        if (size > end - ptr) goto len_delim_till_end;
        auto newend = ptr + size;
        bool ok = ctx->ParseExactRange({parser_till_end, object},
                                       ptr, newend);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ok);
        ptr = newend;
        break;
      }
      default: {
      handle_unusual: (void)&&handle_unusual;
        if ((tag & 7) == 4 || tag == 0) {
          bool ok = ctx->ValidEndGroup(tag);
          GOOGLE_PROTOBUF_PARSER_ASSERT(ok);
          return ptr;
        }
        auto res = UnknownFieldParse(tag, {_InternalParse, msg},
          ptr, end, msg->_internal_metadata_.mutable_unknown_fields(), ctx);
        ptr = res.first;
        if (res.second) return ptr;
      }
    }  // switch
  }  // while
  return ptr;
len_delim_till_end: (void)&&len_delim_till_end;
  return ctx->StoreAndTailCall(ptr, end, {_InternalParse, msg},
                                 {parser_till_end, object}, size);
group_continues: (void)&&group_continues;
  GOOGLE_DCHECK(ptr >= end);
  ctx->StoreGroup({_InternalParse, msg}, {parser_till_end, object}, depth);
  return ptr;
}
#else  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
bool Order::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Enze.Order)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // bytes hashReply = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (10 & 0xFF)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_hashreply()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // .Enze.WalletTx txOrder = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (18 & 0xFF)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessage(
               input, mutable_txorder()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      default: {
      handle_unusual:
        if (tag == 0) {
          goto success;
        }
        DO_(::google::protobuf::internal::WireFormat::SkipField(
              input, tag, _internal_metadata_.mutable_unknown_fields()));
        break;
      }
    }
  }
success:
  // @@protoc_insertion_point(parse_success:Enze.Order)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Enze.Order)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void Order::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Enze.Order)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // bytes hashReply = 1;
  if (this->hashreply().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      1, this->hashreply(), output);
  }

  // .Enze.WalletTx txOrder = 2;
  if (this->has_txorder()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      2, HasBitSetters::txorder(this), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Enze.Order)
}

::google::protobuf::uint8* Order::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:Enze.Order)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // bytes hashReply = 1;
  if (this->hashreply().size() > 0) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        1, this->hashreply(), target);
  }

  // .Enze.WalletTx txOrder = 2;
  if (this->has_txorder()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageToArray(
        2, HasBitSetters::txorder(this), deterministic, target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Enze.Order)
  return target;
}

size_t Order::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Enze.Order)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bytes hashReply = 1;
  if (this->hashreply().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::BytesSize(
        this->hashreply());
  }

  // .Enze.WalletTx txOrder = 2;
  if (this->has_txorder()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::MessageSize(
        *txorder_);
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Order::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Enze.Order)
  GOOGLE_DCHECK_NE(&from, this);
  const Order* source =
      ::google::protobuf::DynamicCastToGenerated<Order>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Enze.Order)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Enze.Order)
    MergeFrom(*source);
  }
}

void Order::MergeFrom(const Order& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Enze.Order)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.hashreply().size() > 0) {

    hashreply_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.hashreply_);
  }
  if (from.has_txorder()) {
    mutable_txorder()->::Enze::WalletTx::MergeFrom(from.txorder());
  }
}

void Order::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Enze.Order)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Order::CopyFrom(const Order& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Enze.Order)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Order::IsInitialized() const {
  return true;
}

void Order::Swap(Order* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Order::InternalSwap(Order* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  hashreply_.Swap(&other->hashreply_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(txorder_, other->txorder_);
}

::google::protobuf::Metadata Order::GetMetadata() const {
  ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_Order_2eproto);
  return ::file_level_metadata_Order_2eproto[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace Enze
namespace google {
namespace protobuf {
template<> PROTOBUF_NOINLINE ::Enze::Order* Arena::CreateMaybeMessage< ::Enze::Order >(Arena* arena) {
  return Arena::CreateInternal< ::Enze::Order >(arena);
}
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>

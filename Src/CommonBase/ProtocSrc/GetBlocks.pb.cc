// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: GetBlocks.proto

#include "GetBlocks.pb.h"

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

extern PROTOBUF_INTERNAL_EXPORT_GetBlocks_2eproto ::google::protobuf::internal::SCCInfo<0> scc_info_GetBlocks_BlockLocator_GetBlocks_2eproto;
namespace Enze {
class GetBlocks_BlockLocatorDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<GetBlocks_BlockLocator> _instance;
} _GetBlocks_BlockLocator_default_instance_;
class GetBlocksDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<GetBlocks> _instance;
} _GetBlocks_default_instance_;
}  // namespace Enze
static void InitDefaultsGetBlocks_BlockLocator_GetBlocks_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Enze::_GetBlocks_BlockLocator_default_instance_;
    new (ptr) ::Enze::GetBlocks_BlockLocator();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Enze::GetBlocks_BlockLocator::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_GetBlocks_BlockLocator_GetBlocks_2eproto =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsGetBlocks_BlockLocator_GetBlocks_2eproto}, {}};

static void InitDefaultsGetBlocks_GetBlocks_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Enze::_GetBlocks_default_instance_;
    new (ptr) ::Enze::GetBlocks();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Enze::GetBlocks::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<1> scc_info_GetBlocks_GetBlocks_2eproto =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 1, InitDefaultsGetBlocks_GetBlocks_2eproto}, {
      &scc_info_GetBlocks_BlockLocator_GetBlocks_2eproto.base,}};

void InitDefaults_GetBlocks_2eproto() {
  ::google::protobuf::internal::InitSCC(&scc_info_GetBlocks_BlockLocator_GetBlocks_2eproto.base);
  ::google::protobuf::internal::InitSCC(&scc_info_GetBlocks_GetBlocks_2eproto.base);
}

::google::protobuf::Metadata file_level_metadata_GetBlocks_2eproto[2];
constexpr ::google::protobuf::EnumDescriptor const** file_level_enum_descriptors_GetBlocks_2eproto = nullptr;
constexpr ::google::protobuf::ServiceDescriptor const** file_level_service_descriptors_GetBlocks_2eproto = nullptr;

const ::google::protobuf::uint32 TableStruct_GetBlocks_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Enze::GetBlocks_BlockLocator, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Enze::GetBlocks_BlockLocator, vhave_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Enze::GetBlocks, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Enze::GetBlocks, locator_),
  PROTOBUF_FIELD_OFFSET(::Enze::GetBlocks, hashstop_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, sizeof(::Enze::GetBlocks_BlockLocator)},
  { 6, -1, sizeof(::Enze::GetBlocks)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::Enze::_GetBlocks_BlockLocator_default_instance_),
  reinterpret_cast<const ::google::protobuf::Message*>(&::Enze::_GetBlocks_default_instance_),
};

::google::protobuf::internal::AssignDescriptorsTable assign_descriptors_table_GetBlocks_2eproto = {
  {}, AddDescriptors_GetBlocks_2eproto, "GetBlocks.proto", schemas,
  file_default_instances, TableStruct_GetBlocks_2eproto::offsets,
  file_level_metadata_GetBlocks_2eproto, 2, file_level_enum_descriptors_GetBlocks_2eproto, file_level_service_descriptors_GetBlocks_2eproto,
};

::google::protobuf::internal::DescriptorTable descriptor_table_GetBlocks_2eproto = {
  false, InitDefaults_GetBlocks_2eproto, 
  "\n\017GetBlocks.proto\022\004Enze\"k\n\tGetBlocks\022-\n\007"
  "locator\030\001 \001(\0132\034.Enze.GetBlocks.BlockLoca"
  "tor\022\020\n\010hashStop\030\002 \001(\014\032\035\n\014BlockLocator\022\r\n"
  "\005vHave\030\001 \003(\014b\006proto3"
,
  "GetBlocks.proto", &assign_descriptors_table_GetBlocks_2eproto, 140,
};

void AddDescriptors_GetBlocks_2eproto() {
  static constexpr ::google::protobuf::internal::InitFunc deps[1] =
  {
  };
 ::google::protobuf::internal::AddDescriptors(&descriptor_table_GetBlocks_2eproto, deps, 0);
}

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_GetBlocks_2eproto = []() { AddDescriptors_GetBlocks_2eproto(); return true; }();
namespace Enze {

// ===================================================================

void GetBlocks_BlockLocator::InitAsDefaultInstance() {
}
class GetBlocks_BlockLocator::HasBitSetters {
 public:
};

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int GetBlocks_BlockLocator::kVHaveFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

GetBlocks_BlockLocator::GetBlocks_BlockLocator()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Enze.GetBlocks.BlockLocator)
}
GetBlocks_BlockLocator::GetBlocks_BlockLocator(const GetBlocks_BlockLocator& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL),
      vhave_(from.vhave_) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Enze.GetBlocks.BlockLocator)
}

void GetBlocks_BlockLocator::SharedCtor() {
  ::google::protobuf::internal::InitSCC(
      &scc_info_GetBlocks_BlockLocator_GetBlocks_2eproto.base);
}

GetBlocks_BlockLocator::~GetBlocks_BlockLocator() {
  // @@protoc_insertion_point(destructor:Enze.GetBlocks.BlockLocator)
  SharedDtor();
}

void GetBlocks_BlockLocator::SharedDtor() {
}

void GetBlocks_BlockLocator::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const GetBlocks_BlockLocator& GetBlocks_BlockLocator::default_instance() {
  ::google::protobuf::internal::InitSCC(&::scc_info_GetBlocks_BlockLocator_GetBlocks_2eproto.base);
  return *internal_default_instance();
}


void GetBlocks_BlockLocator::Clear() {
// @@protoc_insertion_point(message_clear_start:Enze.GetBlocks.BlockLocator)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  vhave_.Clear();
  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* GetBlocks_BlockLocator::_InternalParse(const char* begin, const char* end, void* object,
                  ::google::protobuf::internal::ParseContext* ctx) {
  auto msg = static_cast<GetBlocks_BlockLocator*>(object);
  ::google::protobuf::uint32 size; (void)size;
  int depth; (void)depth;
  ::google::protobuf::internal::ParseFunc parser_till_end; (void)parser_till_end;
  auto ptr = begin;
  while (ptr < end) {
    ::google::protobuf::uint32 tag;
    ptr = Varint::Parse32Inline(ptr, &tag);
    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
    switch (tag >> 3) {
      // repeated bytes vHave = 1;
      case 1: {
        if (static_cast<::google::protobuf::uint8>(tag) != 10) goto handle_unusual;
        do {
          ptr = Varint::Parse32Inline(ptr, &size);
          GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
          parser_till_end = ::google::protobuf::internal::StringParser;
          ::std::string* str = msg->add_vhave();
          str->clear();
          object = str;
          if (size > end - ptr) goto len_delim_till_end;
          str->append(ptr, size);
          ptr += size;
          if (ptr >= end) break;
        } while ((::google::protobuf::io::UnalignedLoad<::google::protobuf::uint64>(ptr) & 255) == 10 && (ptr += 1));
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
bool GetBlocks_BlockLocator::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Enze.GetBlocks.BlockLocator)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // repeated bytes vHave = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (10 & 0xFF)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->add_vhave()));
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
  // @@protoc_insertion_point(parse_success:Enze.GetBlocks.BlockLocator)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Enze.GetBlocks.BlockLocator)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void GetBlocks_BlockLocator::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Enze.GetBlocks.BlockLocator)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated bytes vHave = 1;
  for (int i = 0, n = this->vhave_size(); i < n; i++) {
    ::google::protobuf::internal::WireFormatLite::WriteBytes(
      1, this->vhave(i), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Enze.GetBlocks.BlockLocator)
}

::google::protobuf::uint8* GetBlocks_BlockLocator::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:Enze.GetBlocks.BlockLocator)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // repeated bytes vHave = 1;
  for (int i = 0, n = this->vhave_size(); i < n; i++) {
    target = ::google::protobuf::internal::WireFormatLite::
      WriteBytesToArray(1, this->vhave(i), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Enze.GetBlocks.BlockLocator)
  return target;
}

size_t GetBlocks_BlockLocator::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Enze.GetBlocks.BlockLocator)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated bytes vHave = 1;
  total_size += 1 *
      ::google::protobuf::internal::FromIntSize(this->vhave_size());
  for (int i = 0, n = this->vhave_size(); i < n; i++) {
    total_size += ::google::protobuf::internal::WireFormatLite::BytesSize(
      this->vhave(i));
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void GetBlocks_BlockLocator::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Enze.GetBlocks.BlockLocator)
  GOOGLE_DCHECK_NE(&from, this);
  const GetBlocks_BlockLocator* source =
      ::google::protobuf::DynamicCastToGenerated<GetBlocks_BlockLocator>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Enze.GetBlocks.BlockLocator)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Enze.GetBlocks.BlockLocator)
    MergeFrom(*source);
  }
}

void GetBlocks_BlockLocator::MergeFrom(const GetBlocks_BlockLocator& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Enze.GetBlocks.BlockLocator)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  vhave_.MergeFrom(from.vhave_);
}

void GetBlocks_BlockLocator::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Enze.GetBlocks.BlockLocator)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void GetBlocks_BlockLocator::CopyFrom(const GetBlocks_BlockLocator& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Enze.GetBlocks.BlockLocator)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GetBlocks_BlockLocator::IsInitialized() const {
  return true;
}

void GetBlocks_BlockLocator::Swap(GetBlocks_BlockLocator* other) {
  if (other == this) return;
  InternalSwap(other);
}
void GetBlocks_BlockLocator::InternalSwap(GetBlocks_BlockLocator* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  vhave_.InternalSwap(CastToBase(&other->vhave_));
}

::google::protobuf::Metadata GetBlocks_BlockLocator::GetMetadata() const {
  ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_GetBlocks_2eproto);
  return ::file_level_metadata_GetBlocks_2eproto[kIndexInFileMessages];
}


// ===================================================================

void GetBlocks::InitAsDefaultInstance() {
  ::Enze::_GetBlocks_default_instance_._instance.get_mutable()->locator_ = const_cast< ::Enze::GetBlocks_BlockLocator*>(
      ::Enze::GetBlocks_BlockLocator::internal_default_instance());
}
class GetBlocks::HasBitSetters {
 public:
  static const ::Enze::GetBlocks_BlockLocator& locator(const GetBlocks* msg);
};

const ::Enze::GetBlocks_BlockLocator&
GetBlocks::HasBitSetters::locator(const GetBlocks* msg) {
  return *msg->locator_;
}
#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int GetBlocks::kLocatorFieldNumber;
const int GetBlocks::kHashStopFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

GetBlocks::GetBlocks()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Enze.GetBlocks)
}
GetBlocks::GetBlocks(const GetBlocks& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  hashstop_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.hashstop().size() > 0) {
    hashstop_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.hashstop_);
  }
  if (from.has_locator()) {
    locator_ = new ::Enze::GetBlocks_BlockLocator(*from.locator_);
  } else {
    locator_ = NULL;
  }
  // @@protoc_insertion_point(copy_constructor:Enze.GetBlocks)
}

void GetBlocks::SharedCtor() {
  ::google::protobuf::internal::InitSCC(
      &scc_info_GetBlocks_GetBlocks_2eproto.base);
  hashstop_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  locator_ = NULL;
}

GetBlocks::~GetBlocks() {
  // @@protoc_insertion_point(destructor:Enze.GetBlocks)
  SharedDtor();
}

void GetBlocks::SharedDtor() {
  hashstop_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (this != internal_default_instance()) delete locator_;
}

void GetBlocks::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const GetBlocks& GetBlocks::default_instance() {
  ::google::protobuf::internal::InitSCC(&::scc_info_GetBlocks_GetBlocks_2eproto.base);
  return *internal_default_instance();
}


void GetBlocks::Clear() {
// @@protoc_insertion_point(message_clear_start:Enze.GetBlocks)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  hashstop_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (GetArenaNoVirtual() == NULL && locator_ != NULL) {
    delete locator_;
  }
  locator_ = NULL;
  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* GetBlocks::_InternalParse(const char* begin, const char* end, void* object,
                  ::google::protobuf::internal::ParseContext* ctx) {
  auto msg = static_cast<GetBlocks*>(object);
  ::google::protobuf::uint32 size; (void)size;
  int depth; (void)depth;
  ::google::protobuf::internal::ParseFunc parser_till_end; (void)parser_till_end;
  auto ptr = begin;
  while (ptr < end) {
    ::google::protobuf::uint32 tag;
    ptr = Varint::Parse32Inline(ptr, &tag);
    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
    switch (tag >> 3) {
      // .Enze.GetBlocks.BlockLocator locator = 1;
      case 1: {
        if (static_cast<::google::protobuf::uint8>(tag) != 10) goto handle_unusual;
        ptr = Varint::Parse32Inline(ptr, &size);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        parser_till_end = ::Enze::GetBlocks_BlockLocator::_InternalParse;
        object = msg->mutable_locator();
        if (size > end - ptr) goto len_delim_till_end;
        auto newend = ptr + size;
        bool ok = ctx->ParseExactRange({parser_till_end, object},
                                       ptr, newend);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ok);
        ptr = newend;
        break;
      }
      // bytes hashStop = 2;
      case 2: {
        if (static_cast<::google::protobuf::uint8>(tag) != 18) goto handle_unusual;
        ptr = Varint::Parse32Inline(ptr, &size);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        parser_till_end = ::google::protobuf::internal::StringParser;
        ::std::string* str = msg->mutable_hashstop();
        str->clear();
        object = str;
        if (size > end - ptr) goto len_delim_till_end;
        str->append(ptr, size);
        ptr += size;
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
bool GetBlocks::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Enze.GetBlocks)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // .Enze.GetBlocks.BlockLocator locator = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (10 & 0xFF)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessage(
               input, mutable_locator()));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // bytes hashStop = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (18 & 0xFF)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadBytes(
                input, this->mutable_hashstop()));
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
  // @@protoc_insertion_point(parse_success:Enze.GetBlocks)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Enze.GetBlocks)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void GetBlocks::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Enze.GetBlocks)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .Enze.GetBlocks.BlockLocator locator = 1;
  if (this->has_locator()) {
    ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(
      1, HasBitSetters::locator(this), output);
  }

  // bytes hashStop = 2;
  if (this->hashstop().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::WriteBytesMaybeAliased(
      2, this->hashstop(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Enze.GetBlocks)
}

::google::protobuf::uint8* GetBlocks::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:Enze.GetBlocks)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // .Enze.GetBlocks.BlockLocator locator = 1;
  if (this->has_locator()) {
    target = ::google::protobuf::internal::WireFormatLite::
      InternalWriteMessageToArray(
        1, HasBitSetters::locator(this), deterministic, target);
  }

  // bytes hashStop = 2;
  if (this->hashstop().size() > 0) {
    target =
      ::google::protobuf::internal::WireFormatLite::WriteBytesToArray(
        2, this->hashstop(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Enze.GetBlocks)
  return target;
}

size_t GetBlocks::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Enze.GetBlocks)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // bytes hashStop = 2;
  if (this->hashstop().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::BytesSize(
        this->hashstop());
  }

  // .Enze.GetBlocks.BlockLocator locator = 1;
  if (this->has_locator()) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::MessageSize(
        *locator_);
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void GetBlocks::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Enze.GetBlocks)
  GOOGLE_DCHECK_NE(&from, this);
  const GetBlocks* source =
      ::google::protobuf::DynamicCastToGenerated<GetBlocks>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Enze.GetBlocks)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Enze.GetBlocks)
    MergeFrom(*source);
  }
}

void GetBlocks::MergeFrom(const GetBlocks& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Enze.GetBlocks)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  if (from.hashstop().size() > 0) {

    hashstop_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.hashstop_);
  }
  if (from.has_locator()) {
    mutable_locator()->::Enze::GetBlocks_BlockLocator::MergeFrom(from.locator());
  }
}

void GetBlocks::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Enze.GetBlocks)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void GetBlocks::CopyFrom(const GetBlocks& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Enze.GetBlocks)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool GetBlocks::IsInitialized() const {
  return true;
}

void GetBlocks::Swap(GetBlocks* other) {
  if (other == this) return;
  InternalSwap(other);
}
void GetBlocks::InternalSwap(GetBlocks* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  hashstop_.Swap(&other->hashstop_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(locator_, other->locator_);
}

::google::protobuf::Metadata GetBlocks::GetMetadata() const {
  ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_GetBlocks_2eproto);
  return ::file_level_metadata_GetBlocks_2eproto[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace Enze
namespace google {
namespace protobuf {
template<> PROTOBUF_NOINLINE ::Enze::GetBlocks_BlockLocator* Arena::CreateMaybeMessage< ::Enze::GetBlocks_BlockLocator >(Arena* arena) {
  return Arena::CreateInternal< ::Enze::GetBlocks_BlockLocator >(arena);
}
template<> PROTOBUF_NOINLINE ::Enze::GetBlocks* Arena::CreateMaybeMessage< ::Enze::GetBlocks >(Arena* arena) {
  return Arena::CreateInternal< ::Enze::GetBlocks >(arena);
}
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>

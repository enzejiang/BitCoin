// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Review.proto

#include "Review.pb.h"

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

extern PROTOBUF_INTERNAL_EXPORT_Review_2eproto ::google::protobuf::internal::SCCInfo<0> scc_info_Review_MapValueEntry_DoNotUse_Review_2eproto;
namespace Enze {
class Review_MapValueEntry_DoNotUseDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<Review_MapValueEntry_DoNotUse> _instance;
} _Review_MapValueEntry_DoNotUse_default_instance_;
class ReviewDefaultTypeInternal {
 public:
  ::google::protobuf::internal::ExplicitlyConstructed<Review> _instance;
} _Review_default_instance_;
}  // namespace Enze
static void InitDefaultsReview_MapValueEntry_DoNotUse_Review_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Enze::_Review_MapValueEntry_DoNotUse_default_instance_;
    new (ptr) ::Enze::Review_MapValueEntry_DoNotUse();
  }
  ::Enze::Review_MapValueEntry_DoNotUse::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<0> scc_info_Review_MapValueEntry_DoNotUse_Review_2eproto =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 0, InitDefaultsReview_MapValueEntry_DoNotUse_Review_2eproto}, {}};

static void InitDefaultsReview_Review_2eproto() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;

  {
    void* ptr = &::Enze::_Review_default_instance_;
    new (ptr) ::Enze::Review();
    ::google::protobuf::internal::OnShutdownDestroyMessage(ptr);
  }
  ::Enze::Review::InitAsDefaultInstance();
}

::google::protobuf::internal::SCCInfo<1> scc_info_Review_Review_2eproto =
    {{ATOMIC_VAR_INIT(::google::protobuf::internal::SCCInfoBase::kUninitialized), 1, InitDefaultsReview_Review_2eproto}, {
      &scc_info_Review_MapValueEntry_DoNotUse_Review_2eproto.base,}};

void InitDefaults_Review_2eproto() {
  ::google::protobuf::internal::InitSCC(&scc_info_Review_MapValueEntry_DoNotUse_Review_2eproto.base);
  ::google::protobuf::internal::InitSCC(&scc_info_Review_Review_2eproto.base);
}

::google::protobuf::Metadata file_level_metadata_Review_2eproto[2];
constexpr ::google::protobuf::EnumDescriptor const** file_level_enum_descriptors_Review_2eproto = nullptr;
constexpr ::google::protobuf::ServiceDescriptor const** file_level_service_descriptors_Review_2eproto = nullptr;

const ::google::protobuf::uint32 TableStruct_Review_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  PROTOBUF_FIELD_OFFSET(::Enze::Review_MapValueEntry_DoNotUse, _has_bits_),
  PROTOBUF_FIELD_OFFSET(::Enze::Review_MapValueEntry_DoNotUse, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Enze::Review_MapValueEntry_DoNotUse, key_),
  PROTOBUF_FIELD_OFFSET(::Enze::Review_MapValueEntry_DoNotUse, value_),
  0,
  1,
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Enze::Review, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  PROTOBUF_FIELD_OFFSET(::Enze::Review, nversion_),
  PROTOBUF_FIELD_OFFSET(::Enze::Review, hashto_),
  PROTOBUF_FIELD_OFFSET(::Enze::Review, mapvalue_),
  PROTOBUF_FIELD_OFFSET(::Enze::Review, vchpubkeyfrom_),
  PROTOBUF_FIELD_OFFSET(::Enze::Review, vchsig_),
  PROTOBUF_FIELD_OFFSET(::Enze::Review, ntime_),
  PROTOBUF_FIELD_OFFSET(::Enze::Review, natoms_),
};
static const ::google::protobuf::internal::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, 7, sizeof(::Enze::Review_MapValueEntry_DoNotUse)},
  { 9, -1, sizeof(::Enze::Review)},
};

static ::google::protobuf::Message const * const file_default_instances[] = {
  reinterpret_cast<const ::google::protobuf::Message*>(&::Enze::_Review_MapValueEntry_DoNotUse_default_instance_),
  reinterpret_cast<const ::google::protobuf::Message*>(&::Enze::_Review_default_instance_),
};

::google::protobuf::internal::AssignDescriptorsTable assign_descriptors_table_Review_2eproto = {
  {}, AddDescriptors_Review_2eproto, "Review.proto", schemas,
  file_default_instances, TableStruct_Review_2eproto::offsets,
  file_level_metadata_Review_2eproto, 2, file_level_enum_descriptors_Review_2eproto, file_level_service_descriptors_Review_2eproto,
};

::google::protobuf::internal::DescriptorTable descriptor_table_Review_2eproto = {
  false, InitDefaults_Review_2eproto, 
  "\n\014Review.proto\022\004Enze\"\317\001\n\006Review\022\020\n\010nVers"
  "ion\030\001 \001(\005\022\016\n\006hashTo\030\002 \001(\t\022,\n\010mapValue\030\003 "
  "\003(\0132\032.Enze.Review.MapValueEntry\022\025\n\rvchPu"
  "bKeyFrom\030\004 \001(\t\022\016\n\006vchSig\030\005 \001(\t\022\r\n\005nTime\030"
  "\006 \001(\r\022\016\n\006nAtoms\030\007 \001(\005\032/\n\rMapValueEntry\022\013"
  "\n\003key\030\001 \001(\t\022\r\n\005value\030\002 \001(\t:\0028\001b\006proto3"
,
  "Review.proto", &assign_descriptors_table_Review_2eproto, 238,
};

void AddDescriptors_Review_2eproto() {
  static constexpr ::google::protobuf::internal::InitFunc deps[1] =
  {
  };
 ::google::protobuf::internal::AddDescriptors(&descriptor_table_Review_2eproto, deps, 0);
}

// Force running AddDescriptors() at dynamic initialization time.
static bool dynamic_init_dummy_Review_2eproto = []() { AddDescriptors_Review_2eproto(); return true; }();
namespace Enze {

// ===================================================================

Review_MapValueEntry_DoNotUse::Review_MapValueEntry_DoNotUse() {}
Review_MapValueEntry_DoNotUse::Review_MapValueEntry_DoNotUse(::google::protobuf::Arena* arena)
    : SuperType(arena) {}
void Review_MapValueEntry_DoNotUse::MergeFrom(const Review_MapValueEntry_DoNotUse& other) {
  MergeFromInternal(other);
}
::google::protobuf::Metadata Review_MapValueEntry_DoNotUse::GetMetadata() const {
  ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_Review_2eproto);
  return ::file_level_metadata_Review_2eproto[0];
}
void Review_MapValueEntry_DoNotUse::MergeFrom(
    const ::google::protobuf::Message& other) {
  ::google::protobuf::Message::MergeFrom(other);
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
bool Review_MapValueEntry_DoNotUse::_ParseMap(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx) {
  using MF = ::google::protobuf::internal::MapField<
      Review_MapValueEntry_DoNotUse, EntryKeyType, EntryValueType,
      kEntryKeyFieldType, kEntryValueFieldType,
      kEntryDefaultEnumValue>;
  auto mf = static_cast<MF*>(object);
  Parser<MF, ::google::protobuf::Map<EntryKeyType, EntryValueType>> parser(mf);
#define DO_(x) if (!(x)) return false
  DO_(parser.ParseMap(begin, end));
  DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
    parser.key().data(), static_cast<int>(parser.key().length()),
    ::google::protobuf::internal::WireFormatLite::PARSE,
    "Enze.Review.MapValueEntry.key"));
  DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
    parser.value().data(), static_cast<int>(parser.value().length()),
    ::google::protobuf::internal::WireFormatLite::PARSE,
    "Enze.Review.MapValueEntry.value"));
#undef DO_
  return true;
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER


// ===================================================================

void Review::InitAsDefaultInstance() {
}
class Review::HasBitSetters {
 public:
};

#if !defined(_MSC_VER) || _MSC_VER >= 1900
const int Review::kNVersionFieldNumber;
const int Review::kHashToFieldNumber;
const int Review::kMapValueFieldNumber;
const int Review::kVchPubKeyFromFieldNumber;
const int Review::kVchSigFieldNumber;
const int Review::kNTimeFieldNumber;
const int Review::kNAtomsFieldNumber;
#endif  // !defined(_MSC_VER) || _MSC_VER >= 1900

Review::Review()
  : ::google::protobuf::Message(), _internal_metadata_(NULL) {
  SharedCtor();
  // @@protoc_insertion_point(constructor:Enze.Review)
}
Review::Review(const Review& from)
  : ::google::protobuf::Message(),
      _internal_metadata_(NULL) {
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  mapvalue_.MergeFrom(from.mapvalue_);
  hashto_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.hashto().size() > 0) {
    hashto_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.hashto_);
  }
  vchpubkeyfrom_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.vchpubkeyfrom().size() > 0) {
    vchpubkeyfrom_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.vchpubkeyfrom_);
  }
  vchsig_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  if (from.vchsig().size() > 0) {
    vchsig_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.vchsig_);
  }
  ::memcpy(&nversion_, &from.nversion_,
    static_cast<size_t>(reinterpret_cast<char*>(&natoms_) -
    reinterpret_cast<char*>(&nversion_)) + sizeof(natoms_));
  // @@protoc_insertion_point(copy_constructor:Enze.Review)
}

void Review::SharedCtor() {
  ::google::protobuf::internal::InitSCC(
      &scc_info_Review_Review_2eproto.base);
  hashto_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  vchpubkeyfrom_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  vchsig_.UnsafeSetDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&nversion_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&natoms_) -
      reinterpret_cast<char*>(&nversion_)) + sizeof(natoms_));
}

Review::~Review() {
  // @@protoc_insertion_point(destructor:Enze.Review)
  SharedDtor();
}

void Review::SharedDtor() {
  hashto_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  vchpubkeyfrom_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  vchsig_.DestroyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}

void Review::SetCachedSize(int size) const {
  _cached_size_.Set(size);
}
const Review& Review::default_instance() {
  ::google::protobuf::internal::InitSCC(&::scc_info_Review_Review_2eproto.base);
  return *internal_default_instance();
}


void Review::Clear() {
// @@protoc_insertion_point(message_clear_start:Enze.Review)
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  mapvalue_.Clear();
  hashto_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  vchpubkeyfrom_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  vchsig_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
  ::memset(&nversion_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&natoms_) -
      reinterpret_cast<char*>(&nversion_)) + sizeof(natoms_));
  _internal_metadata_.Clear();
}

#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
const char* Review::_InternalParse(const char* begin, const char* end, void* object,
                  ::google::protobuf::internal::ParseContext* ctx) {
  auto msg = static_cast<Review*>(object);
  ::google::protobuf::uint32 size; (void)size;
  int depth; (void)depth;
  ::google::protobuf::internal::ParseFunc parser_till_end; (void)parser_till_end;
  auto ptr = begin;
  while (ptr < end) {
    ::google::protobuf::uint32 tag;
    ptr = Varint::Parse32Inline(ptr, &tag);
    GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
    switch (tag >> 3) {
      // int32 nVersion = 1;
      case 1: {
        if (static_cast<::google::protobuf::uint8>(tag) != 8) goto handle_unusual;
        ::google::protobuf::uint64 val;
        ptr = Varint::Parse64(ptr, &val);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        ::google::protobuf::int32 value = val;
        msg->set_nversion(value);
        break;
      }
      // string hashTo = 2;
      case 2: {
        if (static_cast<::google::protobuf::uint8>(tag) != 18) goto handle_unusual;
        ptr = Varint::Parse32Inline(ptr, &size);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        ctx->extra_parse_data().SetFieldName("Enze.Review.hashTo");
        parser_till_end = ::google::protobuf::internal::StringParserUTF8;
        ::std::string* str = msg->mutable_hashto();
        str->clear();
        object = str;
        if (size > end - ptr) goto len_delim_till_end;
        auto newend = ptr + size;
        if (size) ptr = parser_till_end(ptr, newend, object, ctx);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr == newend);
        break;
      }
      // map<string, string> mapValue = 3;
      case 3: {
        if (static_cast<::google::protobuf::uint8>(tag) != 26) goto handle_unusual;
        do {
          ptr = Varint::Parse32Inline(ptr, &size);
          GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
          parser_till_end = ::google::protobuf::internal::SlowMapEntryParser;
          auto parse_map = ::Enze::Review_MapValueEntry_DoNotUse::_ParseMap;
          ctx->extra_parse_data().payload.clear();
          ctx->extra_parse_data().parse_map = parse_map;
          object = &msg->mapvalue_;
          if (size > end - ptr) goto len_delim_till_end;
          auto newend = ptr + size;
          GOOGLE_PROTOBUF_PARSER_ASSERT(parse_map(ptr, newend, object, ctx));
          ptr = newend;
          if (ptr >= end) break;
        } while ((::google::protobuf::io::UnalignedLoad<::google::protobuf::uint64>(ptr) & 255) == 26 && (ptr += 1));
        break;
      }
      // string vchPubKeyFrom = 4;
      case 4: {
        if (static_cast<::google::protobuf::uint8>(tag) != 34) goto handle_unusual;
        ptr = Varint::Parse32Inline(ptr, &size);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        ctx->extra_parse_data().SetFieldName("Enze.Review.vchPubKeyFrom");
        parser_till_end = ::google::protobuf::internal::StringParserUTF8;
        ::std::string* str = msg->mutable_vchpubkeyfrom();
        str->clear();
        object = str;
        if (size > end - ptr) goto len_delim_till_end;
        auto newend = ptr + size;
        if (size) ptr = parser_till_end(ptr, newend, object, ctx);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr == newend);
        break;
      }
      // string vchSig = 5;
      case 5: {
        if (static_cast<::google::protobuf::uint8>(tag) != 42) goto handle_unusual;
        ptr = Varint::Parse32Inline(ptr, &size);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        ctx->extra_parse_data().SetFieldName("Enze.Review.vchSig");
        parser_till_end = ::google::protobuf::internal::StringParserUTF8;
        ::std::string* str = msg->mutable_vchsig();
        str->clear();
        object = str;
        if (size > end - ptr) goto len_delim_till_end;
        auto newend = ptr + size;
        if (size) ptr = parser_till_end(ptr, newend, object, ctx);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr == newend);
        break;
      }
      // uint32 nTime = 6;
      case 6: {
        if (static_cast<::google::protobuf::uint8>(tag) != 48) goto handle_unusual;
        ::google::protobuf::uint64 val;
        ptr = Varint::Parse64(ptr, &val);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        ::google::protobuf::uint32 value = val;
        msg->set_ntime(value);
        break;
      }
      // int32 nAtoms = 7;
      case 7: {
        if (static_cast<::google::protobuf::uint8>(tag) != 56) goto handle_unusual;
        ::google::protobuf::uint64 val;
        ptr = Varint::Parse64(ptr, &val);
        GOOGLE_PROTOBUF_PARSER_ASSERT(ptr);
        ::google::protobuf::int32 value = val;
        msg->set_natoms(value);
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
bool Review::MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input) {
#define DO_(EXPRESSION) if (!PROTOBUF_PREDICT_TRUE(EXPRESSION)) goto failure
  ::google::protobuf::uint32 tag;
  // @@protoc_insertion_point(parse_start:Enze.Review)
  for (;;) {
    ::std::pair<::google::protobuf::uint32, bool> p = input->ReadTagWithCutoffNoLastTag(127u);
    tag = p.first;
    if (!p.second) goto handle_unusual;
    switch (::google::protobuf::internal::WireFormatLite::GetTagFieldNumber(tag)) {
      // int32 nVersion = 1;
      case 1: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (8 & 0xFF)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &nversion_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string hashTo = 2;
      case 2: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (18 & 0xFF)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_hashto()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->hashto().data(), static_cast<int>(this->hashto().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "Enze.Review.hashTo"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // map<string, string> mapValue = 3;
      case 3: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (26 & 0xFF)) {
          Review_MapValueEntry_DoNotUse::Parser< ::google::protobuf::internal::MapField<
              Review_MapValueEntry_DoNotUse,
              ::std::string, ::std::string,
              ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
              ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
              0 >,
            ::google::protobuf::Map< ::std::string, ::std::string > > parser(&mapvalue_);
          DO_(::google::protobuf::internal::WireFormatLite::ReadMessageNoVirtual(
              input, &parser));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            parser.key().data(), static_cast<int>(parser.key().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "Enze.Review.MapValueEntry.key"));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            parser.value().data(), static_cast<int>(parser.value().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "Enze.Review.MapValueEntry.value"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string vchPubKeyFrom = 4;
      case 4: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (34 & 0xFF)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_vchpubkeyfrom()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->vchpubkeyfrom().data(), static_cast<int>(this->vchpubkeyfrom().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "Enze.Review.vchPubKeyFrom"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // string vchSig = 5;
      case 5: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (42 & 0xFF)) {
          DO_(::google::protobuf::internal::WireFormatLite::ReadString(
                input, this->mutable_vchsig()));
          DO_(::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
            this->vchsig().data(), static_cast<int>(this->vchsig().length()),
            ::google::protobuf::internal::WireFormatLite::PARSE,
            "Enze.Review.vchSig"));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // uint32 nTime = 6;
      case 6: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (48 & 0xFF)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::uint32, ::google::protobuf::internal::WireFormatLite::TYPE_UINT32>(
                 input, &ntime_)));
        } else {
          goto handle_unusual;
        }
        break;
      }

      // int32 nAtoms = 7;
      case 7: {
        if (static_cast< ::google::protobuf::uint8>(tag) == (56 & 0xFF)) {

          DO_((::google::protobuf::internal::WireFormatLite::ReadPrimitive<
                   ::google::protobuf::int32, ::google::protobuf::internal::WireFormatLite::TYPE_INT32>(
                 input, &natoms_)));
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
  // @@protoc_insertion_point(parse_success:Enze.Review)
  return true;
failure:
  // @@protoc_insertion_point(parse_failure:Enze.Review)
  return false;
#undef DO_
}
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER

void Review::SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const {
  // @@protoc_insertion_point(serialize_start:Enze.Review)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 nVersion = 1;
  if (this->nversion() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(1, this->nversion(), output);
  }

  // string hashTo = 2;
  if (this->hashto().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->hashto().data(), static_cast<int>(this->hashto().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "Enze.Review.hashTo");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      2, this->hashto(), output);
  }

  // map<string, string> mapValue = 3;
  if (!this->mapvalue().empty()) {
    typedef ::google::protobuf::Map< ::std::string, ::std::string >::const_pointer
        ConstPtr;
    typedef ConstPtr SortItem;
    typedef ::google::protobuf::internal::CompareByDerefFirst<SortItem> Less;
    struct Utf8Check {
      static void Check(ConstPtr p) {
        ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
          p->first.data(), static_cast<int>(p->first.length()),
          ::google::protobuf::internal::WireFormatLite::SERIALIZE,
          "Enze.Review.MapValueEntry.key");
        ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
          p->second.data(), static_cast<int>(p->second.length()),
          ::google::protobuf::internal::WireFormatLite::SERIALIZE,
          "Enze.Review.MapValueEntry.value");
      }
    };

    if (output->IsSerializationDeterministic() &&
        this->mapvalue().size() > 1) {
      ::std::unique_ptr<SortItem[]> items(
          new SortItem[this->mapvalue().size()]);
      typedef ::google::protobuf::Map< ::std::string, ::std::string >::size_type size_type;
      size_type n = 0;
      for (::google::protobuf::Map< ::std::string, ::std::string >::const_iterator
          it = this->mapvalue().begin();
          it != this->mapvalue().end(); ++it, ++n) {
        items[static_cast<ptrdiff_t>(n)] = SortItem(&*it);
      }
      ::std::sort(&items[0], &items[static_cast<ptrdiff_t>(n)], Less());
      ::std::unique_ptr<Review_MapValueEntry_DoNotUse> entry;
      for (size_type i = 0; i < n; i++) {
        entry.reset(mapvalue_.NewEntryWrapper(items[static_cast<ptrdiff_t>(i)]->first, items[static_cast<ptrdiff_t>(i)]->second));
        ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(3, *entry, output);
        Utf8Check::Check(&(*items[static_cast<ptrdiff_t>(i)]));
      }
    } else {
      ::std::unique_ptr<Review_MapValueEntry_DoNotUse> entry;
      for (::google::protobuf::Map< ::std::string, ::std::string >::const_iterator
          it = this->mapvalue().begin();
          it != this->mapvalue().end(); ++it) {
        entry.reset(mapvalue_.NewEntryWrapper(it->first, it->second));
        ::google::protobuf::internal::WireFormatLite::WriteMessageMaybeToArray(3, *entry, output);
        Utf8Check::Check(&(*it));
      }
    }
  }

  // string vchPubKeyFrom = 4;
  if (this->vchpubkeyfrom().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->vchpubkeyfrom().data(), static_cast<int>(this->vchpubkeyfrom().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "Enze.Review.vchPubKeyFrom");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      4, this->vchpubkeyfrom(), output);
  }

  // string vchSig = 5;
  if (this->vchsig().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->vchsig().data(), static_cast<int>(this->vchsig().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "Enze.Review.vchSig");
    ::google::protobuf::internal::WireFormatLite::WriteStringMaybeAliased(
      5, this->vchsig(), output);
  }

  // uint32 nTime = 6;
  if (this->ntime() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteUInt32(6, this->ntime(), output);
  }

  // int32 nAtoms = 7;
  if (this->natoms() != 0) {
    ::google::protobuf::internal::WireFormatLite::WriteInt32(7, this->natoms(), output);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    ::google::protobuf::internal::WireFormat::SerializeUnknownFields(
        _internal_metadata_.unknown_fields(), output);
  }
  // @@protoc_insertion_point(serialize_end:Enze.Review)
}

::google::protobuf::uint8* Review::InternalSerializeWithCachedSizesToArray(
    bool deterministic, ::google::protobuf::uint8* target) const {
  (void)deterministic; // Unused
  // @@protoc_insertion_point(serialize_to_array_start:Enze.Review)
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  // int32 nVersion = 1;
  if (this->nversion() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(1, this->nversion(), target);
  }

  // string hashTo = 2;
  if (this->hashto().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->hashto().data(), static_cast<int>(this->hashto().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "Enze.Review.hashTo");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        2, this->hashto(), target);
  }

  // map<string, string> mapValue = 3;
  if (!this->mapvalue().empty()) {
    typedef ::google::protobuf::Map< ::std::string, ::std::string >::const_pointer
        ConstPtr;
    typedef ConstPtr SortItem;
    typedef ::google::protobuf::internal::CompareByDerefFirst<SortItem> Less;
    struct Utf8Check {
      static void Check(ConstPtr p) {
        ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
          p->first.data(), static_cast<int>(p->first.length()),
          ::google::protobuf::internal::WireFormatLite::SERIALIZE,
          "Enze.Review.MapValueEntry.key");
        ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
          p->second.data(), static_cast<int>(p->second.length()),
          ::google::protobuf::internal::WireFormatLite::SERIALIZE,
          "Enze.Review.MapValueEntry.value");
      }
    };

    if (deterministic &&
        this->mapvalue().size() > 1) {
      ::std::unique_ptr<SortItem[]> items(
          new SortItem[this->mapvalue().size()]);
      typedef ::google::protobuf::Map< ::std::string, ::std::string >::size_type size_type;
      size_type n = 0;
      for (::google::protobuf::Map< ::std::string, ::std::string >::const_iterator
          it = this->mapvalue().begin();
          it != this->mapvalue().end(); ++it, ++n) {
        items[static_cast<ptrdiff_t>(n)] = SortItem(&*it);
      }
      ::std::sort(&items[0], &items[static_cast<ptrdiff_t>(n)], Less());
      ::std::unique_ptr<Review_MapValueEntry_DoNotUse> entry;
      for (size_type i = 0; i < n; i++) {
        entry.reset(mapvalue_.NewEntryWrapper(items[static_cast<ptrdiff_t>(i)]->first, items[static_cast<ptrdiff_t>(i)]->second));
        target = ::google::protobuf::internal::WireFormatLite::InternalWriteMessageNoVirtualToArray(3, *entry, deterministic, target);
        Utf8Check::Check(&(*items[static_cast<ptrdiff_t>(i)]));
      }
    } else {
      ::std::unique_ptr<Review_MapValueEntry_DoNotUse> entry;
      for (::google::protobuf::Map< ::std::string, ::std::string >::const_iterator
          it = this->mapvalue().begin();
          it != this->mapvalue().end(); ++it) {
        entry.reset(mapvalue_.NewEntryWrapper(it->first, it->second));
        target = ::google::protobuf::internal::WireFormatLite::InternalWriteMessageNoVirtualToArray(3, *entry, deterministic, target);
        Utf8Check::Check(&(*it));
      }
    }
  }

  // string vchPubKeyFrom = 4;
  if (this->vchpubkeyfrom().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->vchpubkeyfrom().data(), static_cast<int>(this->vchpubkeyfrom().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "Enze.Review.vchPubKeyFrom");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        4, this->vchpubkeyfrom(), target);
  }

  // string vchSig = 5;
  if (this->vchsig().size() > 0) {
    ::google::protobuf::internal::WireFormatLite::VerifyUtf8String(
      this->vchsig().data(), static_cast<int>(this->vchsig().length()),
      ::google::protobuf::internal::WireFormatLite::SERIALIZE,
      "Enze.Review.vchSig");
    target =
      ::google::protobuf::internal::WireFormatLite::WriteStringToArray(
        5, this->vchsig(), target);
  }

  // uint32 nTime = 6;
  if (this->ntime() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteUInt32ToArray(6, this->ntime(), target);
  }

  // int32 nAtoms = 7;
  if (this->natoms() != 0) {
    target = ::google::protobuf::internal::WireFormatLite::WriteInt32ToArray(7, this->natoms(), target);
  }

  if (_internal_metadata_.have_unknown_fields()) {
    target = ::google::protobuf::internal::WireFormat::SerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields(), target);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Enze.Review)
  return target;
}

size_t Review::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Enze.Review)
  size_t total_size = 0;

  if (_internal_metadata_.have_unknown_fields()) {
    total_size +=
      ::google::protobuf::internal::WireFormat::ComputeUnknownFieldsSize(
        _internal_metadata_.unknown_fields());
  }
  ::google::protobuf::uint32 cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // map<string, string> mapValue = 3;
  total_size += 1 *
      ::google::protobuf::internal::FromIntSize(this->mapvalue_size());
  {
    ::std::unique_ptr<Review_MapValueEntry_DoNotUse> entry;
    for (::google::protobuf::Map< ::std::string, ::std::string >::const_iterator
        it = this->mapvalue().begin();
        it != this->mapvalue().end(); ++it) {
      entry.reset(mapvalue_.NewEntryWrapper(it->first, it->second));
      total_size += ::google::protobuf::internal::WireFormatLite::
          MessageSizeNoVirtual(*entry);
    }
  }

  // string hashTo = 2;
  if (this->hashto().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->hashto());
  }

  // string vchPubKeyFrom = 4;
  if (this->vchpubkeyfrom().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->vchpubkeyfrom());
  }

  // string vchSig = 5;
  if (this->vchsig().size() > 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::StringSize(
        this->vchsig());
  }

  // int32 nVersion = 1;
  if (this->nversion() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->nversion());
  }

  // uint32 nTime = 6;
  if (this->ntime() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::UInt32Size(
        this->ntime());
  }

  // int32 nAtoms = 7;
  if (this->natoms() != 0) {
    total_size += 1 +
      ::google::protobuf::internal::WireFormatLite::Int32Size(
        this->natoms());
  }

  int cached_size = ::google::protobuf::internal::ToCachedSize(total_size);
  SetCachedSize(cached_size);
  return total_size;
}

void Review::MergeFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_merge_from_start:Enze.Review)
  GOOGLE_DCHECK_NE(&from, this);
  const Review* source =
      ::google::protobuf::DynamicCastToGenerated<Review>(
          &from);
  if (source == NULL) {
  // @@protoc_insertion_point(generalized_merge_from_cast_fail:Enze.Review)
    ::google::protobuf::internal::ReflectionOps::Merge(from, this);
  } else {
  // @@protoc_insertion_point(generalized_merge_from_cast_success:Enze.Review)
    MergeFrom(*source);
  }
}

void Review::MergeFrom(const Review& from) {
// @@protoc_insertion_point(class_specific_merge_from_start:Enze.Review)
  GOOGLE_DCHECK_NE(&from, this);
  _internal_metadata_.MergeFrom(from._internal_metadata_);
  ::google::protobuf::uint32 cached_has_bits = 0;
  (void) cached_has_bits;

  mapvalue_.MergeFrom(from.mapvalue_);
  if (from.hashto().size() > 0) {

    hashto_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.hashto_);
  }
  if (from.vchpubkeyfrom().size() > 0) {

    vchpubkeyfrom_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.vchpubkeyfrom_);
  }
  if (from.vchsig().size() > 0) {

    vchsig_.AssignWithDefault(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), from.vchsig_);
  }
  if (from.nversion() != 0) {
    set_nversion(from.nversion());
  }
  if (from.ntime() != 0) {
    set_ntime(from.ntime());
  }
  if (from.natoms() != 0) {
    set_natoms(from.natoms());
  }
}

void Review::CopyFrom(const ::google::protobuf::Message& from) {
// @@protoc_insertion_point(generalized_copy_from_start:Enze.Review)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

void Review::CopyFrom(const Review& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Enze.Review)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool Review::IsInitialized() const {
  return true;
}

void Review::Swap(Review* other) {
  if (other == this) return;
  InternalSwap(other);
}
void Review::InternalSwap(Review* other) {
  using std::swap;
  _internal_metadata_.Swap(&other->_internal_metadata_);
  mapvalue_.Swap(&other->mapvalue_);
  hashto_.Swap(&other->hashto_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  vchpubkeyfrom_.Swap(&other->vchpubkeyfrom_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  vchsig_.Swap(&other->vchsig_, &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    GetArenaNoVirtual());
  swap(nversion_, other->nversion_);
  swap(ntime_, other->ntime_);
  swap(natoms_, other->natoms_);
}

::google::protobuf::Metadata Review::GetMetadata() const {
  ::google::protobuf::internal::AssignDescriptors(&::assign_descriptors_table_Review_2eproto);
  return ::file_level_metadata_Review_2eproto[kIndexInFileMessages];
}


// @@protoc_insertion_point(namespace_scope)
}  // namespace Enze
namespace google {
namespace protobuf {
template<> PROTOBUF_NOINLINE ::Enze::Review_MapValueEntry_DoNotUse* Arena::CreateMaybeMessage< ::Enze::Review_MapValueEntry_DoNotUse >(Arena* arena) {
  return Arena::CreateInternal< ::Enze::Review_MapValueEntry_DoNotUse >(arena);
}
template<> PROTOBUF_NOINLINE ::Enze::Review* Arena::CreateMaybeMessage< ::Enze::Review >(Arena* arena) {
  return Arena::CreateInternal< ::Enze::Review >(arena);
}
}  // namespace protobuf
}  // namespace google

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>

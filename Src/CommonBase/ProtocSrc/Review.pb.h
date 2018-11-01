// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Review.proto

#ifndef PROTOBUF_INCLUDED_Review_2eproto
#define PROTOBUF_INCLUDED_Review_2eproto

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
#include <google/protobuf/map.h>  // IWYU pragma: export
#include <google/protobuf/map_entry.h>
#include <google/protobuf/map_field_inl.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>
#define PROTOBUF_INTERNAL_EXPORT_Review_2eproto

// Internal implementation detail -- do not use these members.
struct TableStruct_Review_2eproto {
  static const ::google::protobuf::internal::ParseTableField entries[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::ParseTable schema[2]
    PROTOBUF_SECTION_VARIABLE(protodesc_cold);
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors_Review_2eproto();
namespace Enze {
class Review;
class ReviewDefaultTypeInternal;
extern ReviewDefaultTypeInternal _Review_default_instance_;
class Review_MapValueEntry_DoNotUse;
class Review_MapValueEntry_DoNotUseDefaultTypeInternal;
extern Review_MapValueEntry_DoNotUseDefaultTypeInternal _Review_MapValueEntry_DoNotUse_default_instance_;
}  // namespace Enze
namespace google {
namespace protobuf {
template<> ::Enze::Review* Arena::CreateMaybeMessage<::Enze::Review>(Arena*);
template<> ::Enze::Review_MapValueEntry_DoNotUse* Arena::CreateMaybeMessage<::Enze::Review_MapValueEntry_DoNotUse>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace Enze {

// ===================================================================

class Review_MapValueEntry_DoNotUse : public ::google::protobuf::internal::MapEntry<Review_MapValueEntry_DoNotUse, 
    ::std::string, ::std::string,
    ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
    ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
    0 > {
public:
#if GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
static bool _ParseMap(const char* begin, const char* end, void* object, ::google::protobuf::internal::ParseContext* ctx);
#endif  // GOOGLE_PROTOBUF_ENABLE_EXPERIMENTAL_PARSER
  typedef ::google::protobuf::internal::MapEntry<Review_MapValueEntry_DoNotUse, 
    ::std::string, ::std::string,
    ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
    ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
    0 > SuperType;
  Review_MapValueEntry_DoNotUse();
  Review_MapValueEntry_DoNotUse(::google::protobuf::Arena* arena);
  void MergeFrom(const Review_MapValueEntry_DoNotUse& other);
  static const Review_MapValueEntry_DoNotUse* internal_default_instance() { return reinterpret_cast<const Review_MapValueEntry_DoNotUse*>(&_Review_MapValueEntry_DoNotUse_default_instance_); }
  void MergeFrom(const ::google::protobuf::Message& other) final;
  ::google::protobuf::Metadata GetMetadata() const;
};

// -------------------------------------------------------------------

class Review : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:Enze.Review) */ {
 public:
  Review();
  virtual ~Review();

  Review(const Review& from);

  inline Review& operator=(const Review& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  Review(Review&& from) noexcept
    : Review() {
    *this = ::std::move(from);
  }

  inline Review& operator=(Review&& from) noexcept {
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
  static const Review& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const Review* internal_default_instance() {
    return reinterpret_cast<const Review*>(
               &_Review_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(Review* other);
  friend void swap(Review& a, Review& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline Review* New() const final {
    return CreateMaybeMessage<Review>(NULL);
  }

  Review* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<Review>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const Review& from);
  void MergeFrom(const Review& from);
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
  void InternalSwap(Review* other);
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

  // map<string, string> mapValue = 3;
  int mapvalue_size() const;
  void clear_mapvalue();
  static const int kMapValueFieldNumber = 3;
  const ::google::protobuf::Map< ::std::string, ::std::string >&
      mapvalue() const;
  ::google::protobuf::Map< ::std::string, ::std::string >*
      mutable_mapvalue();

  // bytes hashTo = 2;
  void clear_hashto();
  static const int kHashToFieldNumber = 2;
  const ::std::string& hashto() const;
  void set_hashto(const ::std::string& value);
  #if LANG_CXX11
  void set_hashto(::std::string&& value);
  #endif
  void set_hashto(const char* value);
  void set_hashto(const void* value, size_t size);
  ::std::string* mutable_hashto();
  ::std::string* release_hashto();
  void set_allocated_hashto(::std::string* hashto);

  // bytes vchPubKeyFrom = 4;
  void clear_vchpubkeyfrom();
  static const int kVchPubKeyFromFieldNumber = 4;
  const ::std::string& vchpubkeyfrom() const;
  void set_vchpubkeyfrom(const ::std::string& value);
  #if LANG_CXX11
  void set_vchpubkeyfrom(::std::string&& value);
  #endif
  void set_vchpubkeyfrom(const char* value);
  void set_vchpubkeyfrom(const void* value, size_t size);
  ::std::string* mutable_vchpubkeyfrom();
  ::std::string* release_vchpubkeyfrom();
  void set_allocated_vchpubkeyfrom(::std::string* vchpubkeyfrom);

  // bytes vchSig = 5;
  void clear_vchsig();
  static const int kVchSigFieldNumber = 5;
  const ::std::string& vchsig() const;
  void set_vchsig(const ::std::string& value);
  #if LANG_CXX11
  void set_vchsig(::std::string&& value);
  #endif
  void set_vchsig(const char* value);
  void set_vchsig(const void* value, size_t size);
  ::std::string* mutable_vchsig();
  ::std::string* release_vchsig();
  void set_allocated_vchsig(::std::string* vchsig);

  // int32 nVersion = 1;
  void clear_nversion();
  static const int kNVersionFieldNumber = 1;
  ::google::protobuf::int32 nversion() const;
  void set_nversion(::google::protobuf::int32 value);

  // uint32 nTime = 6;
  void clear_ntime();
  static const int kNTimeFieldNumber = 6;
  ::google::protobuf::uint32 ntime() const;
  void set_ntime(::google::protobuf::uint32 value);

  // int32 nAtoms = 7;
  void clear_natoms();
  static const int kNAtomsFieldNumber = 7;
  ::google::protobuf::int32 natoms() const;
  void set_natoms(::google::protobuf::int32 value);

  // @@protoc_insertion_point(class_scope:Enze.Review)
 private:
  class HasBitSetters;

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::MapField<
      Review_MapValueEntry_DoNotUse,
      ::std::string, ::std::string,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      ::google::protobuf::internal::WireFormatLite::TYPE_STRING,
      0 > mapvalue_;
  ::google::protobuf::internal::ArenaStringPtr hashto_;
  ::google::protobuf::internal::ArenaStringPtr vchpubkeyfrom_;
  ::google::protobuf::internal::ArenaStringPtr vchsig_;
  ::google::protobuf::int32 nversion_;
  ::google::protobuf::uint32 ntime_;
  ::google::protobuf::int32 natoms_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::TableStruct_Review_2eproto;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// -------------------------------------------------------------------

// Review

// int32 nVersion = 1;
inline void Review::clear_nversion() {
  nversion_ = 0;
}
inline ::google::protobuf::int32 Review::nversion() const {
  // @@protoc_insertion_point(field_get:Enze.Review.nVersion)
  return nversion_;
}
inline void Review::set_nversion(::google::protobuf::int32 value) {
  
  nversion_ = value;
  // @@protoc_insertion_point(field_set:Enze.Review.nVersion)
}

// bytes hashTo = 2;
inline void Review::clear_hashto() {
  hashto_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Review::hashto() const {
  // @@protoc_insertion_point(field_get:Enze.Review.hashTo)
  return hashto_.GetNoArena();
}
inline void Review::set_hashto(const ::std::string& value) {
  
  hashto_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Enze.Review.hashTo)
}
#if LANG_CXX11
inline void Review::set_hashto(::std::string&& value) {
  
  hashto_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Enze.Review.hashTo)
}
#endif
inline void Review::set_hashto(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  hashto_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Enze.Review.hashTo)
}
inline void Review::set_hashto(const void* value, size_t size) {
  
  hashto_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Enze.Review.hashTo)
}
inline ::std::string* Review::mutable_hashto() {
  
  // @@protoc_insertion_point(field_mutable:Enze.Review.hashTo)
  return hashto_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Review::release_hashto() {
  // @@protoc_insertion_point(field_release:Enze.Review.hashTo)
  
  return hashto_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Review::set_allocated_hashto(::std::string* hashto) {
  if (hashto != NULL) {
    
  } else {
    
  }
  hashto_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), hashto);
  // @@protoc_insertion_point(field_set_allocated:Enze.Review.hashTo)
}

// map<string, string> mapValue = 3;
inline int Review::mapvalue_size() const {
  return mapvalue_.size();
}
inline void Review::clear_mapvalue() {
  mapvalue_.Clear();
}
inline const ::google::protobuf::Map< ::std::string, ::std::string >&
Review::mapvalue() const {
  // @@protoc_insertion_point(field_map:Enze.Review.mapValue)
  return mapvalue_.GetMap();
}
inline ::google::protobuf::Map< ::std::string, ::std::string >*
Review::mutable_mapvalue() {
  // @@protoc_insertion_point(field_mutable_map:Enze.Review.mapValue)
  return mapvalue_.MutableMap();
}

// bytes vchPubKeyFrom = 4;
inline void Review::clear_vchpubkeyfrom() {
  vchpubkeyfrom_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Review::vchpubkeyfrom() const {
  // @@protoc_insertion_point(field_get:Enze.Review.vchPubKeyFrom)
  return vchpubkeyfrom_.GetNoArena();
}
inline void Review::set_vchpubkeyfrom(const ::std::string& value) {
  
  vchpubkeyfrom_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Enze.Review.vchPubKeyFrom)
}
#if LANG_CXX11
inline void Review::set_vchpubkeyfrom(::std::string&& value) {
  
  vchpubkeyfrom_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Enze.Review.vchPubKeyFrom)
}
#endif
inline void Review::set_vchpubkeyfrom(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  vchpubkeyfrom_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Enze.Review.vchPubKeyFrom)
}
inline void Review::set_vchpubkeyfrom(const void* value, size_t size) {
  
  vchpubkeyfrom_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Enze.Review.vchPubKeyFrom)
}
inline ::std::string* Review::mutable_vchpubkeyfrom() {
  
  // @@protoc_insertion_point(field_mutable:Enze.Review.vchPubKeyFrom)
  return vchpubkeyfrom_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Review::release_vchpubkeyfrom() {
  // @@protoc_insertion_point(field_release:Enze.Review.vchPubKeyFrom)
  
  return vchpubkeyfrom_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Review::set_allocated_vchpubkeyfrom(::std::string* vchpubkeyfrom) {
  if (vchpubkeyfrom != NULL) {
    
  } else {
    
  }
  vchpubkeyfrom_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), vchpubkeyfrom);
  // @@protoc_insertion_point(field_set_allocated:Enze.Review.vchPubKeyFrom)
}

// bytes vchSig = 5;
inline void Review::clear_vchsig() {
  vchsig_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& Review::vchsig() const {
  // @@protoc_insertion_point(field_get:Enze.Review.vchSig)
  return vchsig_.GetNoArena();
}
inline void Review::set_vchsig(const ::std::string& value) {
  
  vchsig_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:Enze.Review.vchSig)
}
#if LANG_CXX11
inline void Review::set_vchsig(::std::string&& value) {
  
  vchsig_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:Enze.Review.vchSig)
}
#endif
inline void Review::set_vchsig(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  vchsig_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:Enze.Review.vchSig)
}
inline void Review::set_vchsig(const void* value, size_t size) {
  
  vchsig_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:Enze.Review.vchSig)
}
inline ::std::string* Review::mutable_vchsig() {
  
  // @@protoc_insertion_point(field_mutable:Enze.Review.vchSig)
  return vchsig_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* Review::release_vchsig() {
  // @@protoc_insertion_point(field_release:Enze.Review.vchSig)
  
  return vchsig_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void Review::set_allocated_vchsig(::std::string* vchsig) {
  if (vchsig != NULL) {
    
  } else {
    
  }
  vchsig_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), vchsig);
  // @@protoc_insertion_point(field_set_allocated:Enze.Review.vchSig)
}

// uint32 nTime = 6;
inline void Review::clear_ntime() {
  ntime_ = 0u;
}
inline ::google::protobuf::uint32 Review::ntime() const {
  // @@protoc_insertion_point(field_get:Enze.Review.nTime)
  return ntime_;
}
inline void Review::set_ntime(::google::protobuf::uint32 value) {
  
  ntime_ = value;
  // @@protoc_insertion_point(field_set:Enze.Review.nTime)
}

// int32 nAtoms = 7;
inline void Review::clear_natoms() {
  natoms_ = 0;
}
inline ::google::protobuf::int32 Review::natoms() const {
  // @@protoc_insertion_point(field_get:Enze.Review.nAtoms)
  return natoms_;
}
inline void Review::set_natoms(::google::protobuf::int32 value) {
  
  natoms_ = value;
  // @@protoc_insertion_point(field_set:Enze.Review.nAtoms)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace Enze

// @@protoc_insertion_point(global_scope)

#include <google/protobuf/port_undef.inc>
#endif  // PROTOBUF_INCLUDED_Review_2eproto

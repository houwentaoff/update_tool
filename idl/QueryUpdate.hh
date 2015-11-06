// This file is generated by omniidl (C++ backend)- omniORB_4_1. Do not edit.
#ifndef __QueryUpdate_hh__
#define __QueryUpdate_hh__

#ifndef __CORBA_H_EXTERNAL_GUARD__
#include <omniORB4/CORBA.h>
#endif

#ifndef  USE_stub_in_nt_dll
# define USE_stub_in_nt_dll_NOT_DEFINED_QueryUpdate
#endif
#ifndef  USE_core_stub_in_nt_dll
# define USE_core_stub_in_nt_dll_NOT_DEFINED_QueryUpdate
#endif
#ifndef  USE_dyn_stub_in_nt_dll
# define USE_dyn_stub_in_nt_dll_NOT_DEFINED_QueryUpdate
#endif






#ifdef USE_stub_in_nt_dll
# ifndef USE_core_stub_in_nt_dll
#  define USE_core_stub_in_nt_dll
# endif
# ifndef USE_dyn_stub_in_nt_dll
#  define USE_dyn_stub_in_nt_dll
# endif
#endif

#ifdef _core_attr
# error "A local CPP macro _core_attr has already been defined."
#else
# ifdef  USE_core_stub_in_nt_dll
#  define _core_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _core_attr
# endif
#endif

#ifdef _dyn_attr
# error "A local CPP macro _dyn_attr has already been defined."
#else
# ifdef  USE_dyn_stub_in_nt_dll
#  define _dyn_attr _OMNIORB_NTDLL_IMPORT
# else
#  define _dyn_attr
# endif
#endif





class bfstream_var;

class bfstream : public _CORBA_Unbounded_Sequence_Octet {
public:
  typedef bfstream_var _var_type;
  inline bfstream() {}
  inline bfstream(const bfstream& _s)
    : _CORBA_Unbounded_Sequence_Octet(_s) {}

  inline bfstream(_CORBA_ULong _max)
    : _CORBA_Unbounded_Sequence_Octet(_max) {}
  inline bfstream(_CORBA_ULong _max, _CORBA_ULong _len, ::CORBA::Octet* _val, _CORBA_Boolean _rel=0)
    : _CORBA_Unbounded_Sequence_Octet(_max, _len, _val, _rel) {}



  inline bfstream& operator = (const bfstream& _s) {
    _CORBA_Unbounded_Sequence_Octet::operator=(_s);
    return *this;
  }
};

class bfstream_out;

class bfstream_var {
public:
  inline bfstream_var() : _pd_seq(0) {}
  inline bfstream_var(bfstream* _s) : _pd_seq(_s) {}
  inline bfstream_var(const bfstream_var& _s) {
    if( _s._pd_seq )  _pd_seq = new bfstream(*_s._pd_seq);
    else              _pd_seq = 0;
  }
  inline ~bfstream_var() { if( _pd_seq )  delete _pd_seq; }
    
  inline bfstream_var& operator = (bfstream* _s) {
    if( _pd_seq )  delete _pd_seq;
    _pd_seq = _s;
    return *this;
  }
  inline bfstream_var& operator = (const bfstream_var& _s) {
    if( _s._pd_seq ) {
      if( !_pd_seq )  _pd_seq = new bfstream;
      *_pd_seq = *_s._pd_seq;
    } else if( _pd_seq ) {
      delete _pd_seq;
      _pd_seq = 0;
    }
    return *this;
  }
  inline ::CORBA::Octet& operator [] (_CORBA_ULong _s) {
    return (*_pd_seq)[_s];
  }



  inline bfstream* operator -> () { return _pd_seq; }
  inline const bfstream* operator -> () const { return _pd_seq; }
#if defined(__GNUG__)
  inline operator bfstream& () const { return *_pd_seq; }
#else
  inline operator const bfstream& () const { return *_pd_seq; }
  inline operator bfstream& () { return *_pd_seq; }
#endif
    
  inline const bfstream& in() const { return *_pd_seq; }
  inline bfstream&       inout()    { return *_pd_seq; }
  inline bfstream*&      out() {
    if( _pd_seq ) { delete _pd_seq; _pd_seq = 0; }
    return _pd_seq;
  }
  inline bfstream* _retn() { bfstream* tmp = _pd_seq; _pd_seq = 0; return tmp; }
    
  friend class bfstream_out;
  
private:
  bfstream* _pd_seq;
};

class bfstream_out {
public:
  inline bfstream_out(bfstream*& _s) : _data(_s) { _data = 0; }
  inline bfstream_out(bfstream_var& _s)
    : _data(_s._pd_seq) { _s = (bfstream*) 0; }
  inline bfstream_out(const bfstream_out& _s) : _data(_s._data) {}
  inline bfstream_out& operator = (const bfstream_out& _s) {
    _data = _s._data;
    return *this;
  }
  inline bfstream_out& operator = (bfstream* _s) {
    _data = _s;
    return *this;
  }
  inline operator bfstream*&()  { return _data; }
  inline bfstream*& ptr()       { return _data; }
  inline bfstream* operator->() { return _data; }

  inline ::CORBA::Octet& operator [] (_CORBA_ULong _i) {
    return (*_data)[_i];
  }



  bfstream*& _data;

private:
  bfstream_out();
  bfstream_out& operator=(const bfstream_var&);
};

typedef ::CORBA::Long patchSet_t[100][2];
typedef ::CORBA::Long patchSet_t_slice[2];

inline patchSet_t_slice* patchSet_t_alloc() {
  return new patchSet_t_slice[100];
}

inline patchSet_t_slice* patchSet_t_dup(const patchSet_t_slice* _s) {
  if (!_s) return 0;
  patchSet_t_slice* _data = patchSet_t_alloc();
  if (_data) {
    for (_CORBA_ULong _0i0 = 0; _0i0 < 100; _0i0++){
      for (_CORBA_ULong _0i1 = 0; _0i1 < 2; _0i1++){
        
        _data[_0i0][_0i1] = _s[_0i0][_0i1];

      }
    }

  }
  return _data;
}

inline void patchSet_t_copy(patchSet_t_slice* _to, const patchSet_t_slice* _from){
  for (_CORBA_ULong _0i0 = 0; _0i0 < 100; _0i0++){
    for (_CORBA_ULong _0i1 = 0; _0i1 < 2; _0i1++){
      
      _to[_0i0][_0i1] = _from[_0i0][_0i1];

    }
  }

}

inline void patchSet_t_free(patchSet_t_slice* _s) {
  delete [] _s;
}

class patchSet_t_copyHelper {
public:
  static inline patchSet_t_slice* alloc() { return ::patchSet_t_alloc(); }
  static inline patchSet_t_slice* dup(const patchSet_t_slice* p) { return ::patchSet_t_dup(p); }
  static inline void free(patchSet_t_slice* p) { ::patchSet_t_free(p); }
};

typedef _CORBA_Array_Fix_Var<patchSet_t_copyHelper,patchSet_t_slice> patchSet_t_var;
typedef _CORBA_Array_Fix_Forany<patchSet_t_copyHelper,patchSet_t_slice> patchSet_t_forany;

typedef patchSet_t_slice* patchSet_t_out;

enum e { FISHARE_MDS, FISHARE_CLIENT /*, __max_e=0xffffffff */ };
typedef e& e_out;

struct PlatformInfoEx {
  typedef _CORBA_ConstrType_Variable_Var<PlatformInfoEx> _var_type;

  
  ::CORBA::Long platform;

  ::CORBA::String_member OSName;

  ::CORBA::String_member OSPackName;

  ::CORBA::Long OSMainVersion;

  ::CORBA::Long OSSubVersion;

  ::CORBA::Long OSRunMode;



  void operator>>= (cdrStream &) const;
  void operator<<= (cdrStream &);
};

typedef PlatformInfoEx::_var_type PlatformInfoEx_var;

typedef _CORBA_ConstrType_Variable_OUT_arg< PlatformInfoEx,PlatformInfoEx_var > PlatformInfoEx_out;

#ifndef __FiUpdateMgr__
#define __FiUpdateMgr__

class FiUpdateMgr;
class _objref_FiUpdateMgr;
class _impl_FiUpdateMgr;

typedef _objref_FiUpdateMgr* FiUpdateMgr_ptr;
typedef FiUpdateMgr_ptr FiUpdateMgrRef;

class FiUpdateMgr_Helper {
public:
  typedef FiUpdateMgr_ptr _ptr_type;

  static _ptr_type _nil();
  static _CORBA_Boolean is_nil(_ptr_type);
  static void release(_ptr_type);
  static void duplicate(_ptr_type);
  static void marshalObjRef(_ptr_type, cdrStream&);
  static _ptr_type unmarshalObjRef(cdrStream&);
};

typedef _CORBA_ObjRef_Var<_objref_FiUpdateMgr, FiUpdateMgr_Helper> FiUpdateMgr_var;
typedef _CORBA_ObjRef_OUT_arg<_objref_FiUpdateMgr,FiUpdateMgr_Helper > FiUpdateMgr_out;

#endif

// interface FiUpdateMgr
class FiUpdateMgr {
public:
  // Declarations for this interface type.
  typedef FiUpdateMgr_ptr _ptr_type;
  typedef FiUpdateMgr_var _var_type;

  static _ptr_type _duplicate(_ptr_type);
  static _ptr_type _narrow(::CORBA::Object_ptr);
  static _ptr_type _unchecked_narrow(::CORBA::Object_ptr);
  
  static _ptr_type _nil();

  static inline void _marshalObjRef(_ptr_type, cdrStream&);

  static inline _ptr_type _unmarshalObjRef(cdrStream& s) {
    omniObjRef* o = omniObjRef::_unMarshal(_PD_repoId,s);
    if (o)
      return (_ptr_type) o->_ptrToObjRef(_PD_repoId);
    else
      return _nil();
  }

  static _core_attr const char* _PD_repoId;

  // Other IDL defined within this scope.
  
};

class _objref_FiUpdateMgr :
  public virtual ::CORBA::Object,
  public virtual omniObjRef
{
public:
  ::CORBA::Long QueryCurVersion(const char* inversion, const char* indate, const char* inpatchno, ::CORBA::String_out version, ::CORBA::String_out date, ::CORBA::String_out patchno, ::CORBA::String_out hash);
  ::CORBA::Long StartupUpdate(const ::PlatformInfoEx& PInfo, ::CORBA::Long which, const char* version, const char* date, const char* patchno, ::CORBA::String_out filename, ::CORBA::Long& size, ::CORBA::LongLong& ref);
  ::CORBA::Long TranslateFile(::CORBA::LongLong ref, ::CORBA::Long packid, ::bfstream_out bfs, ::CORBA::Long& checknum, ::CORBA::Long& flag);
  ::CORBA::Long starupTrans(const char* fileName, ::CORBA::Long& size, ::CORBA::LongLong& fileRef);
  ::CORBA::Long transFile(::CORBA::LongLong fileRef, ::CORBA::Long packid, ::bfstream_out bfs, ::CORBA::Long& checknum, ::CORBA::Long& flag);
  ::CORBA::Long queryPatchs(const char* version, ::patchSet_t patchs);
  ::CORBA::Long getMD5FromFile(const char* fileName, ::CORBA::String_out md5Value);

  inline _objref_FiUpdateMgr()  { _PR_setobj(0); }  // nil
  _objref_FiUpdateMgr(omniIOR*, omniIdentity*);

protected:
  virtual ~_objref_FiUpdateMgr();

  
private:
  virtual void* _ptrToObjRef(const char*);

  _objref_FiUpdateMgr(const _objref_FiUpdateMgr&);
  _objref_FiUpdateMgr& operator = (const _objref_FiUpdateMgr&);
  // not implemented

  friend class FiUpdateMgr;
};

class _pof_FiUpdateMgr : public _OMNI_NS(proxyObjectFactory) {
public:
  inline _pof_FiUpdateMgr() : _OMNI_NS(proxyObjectFactory)(FiUpdateMgr::_PD_repoId) {}
  virtual ~_pof_FiUpdateMgr();

  virtual omniObjRef* newObjRef(omniIOR*,omniIdentity*);
  virtual _CORBA_Boolean is_a(const char*) const;
};

class _impl_FiUpdateMgr :
  public virtual omniServant
{
public:
  virtual ~_impl_FiUpdateMgr();

  virtual ::CORBA::Long QueryCurVersion(const char* inversion, const char* indate, const char* inpatchno, ::CORBA::String_out version, ::CORBA::String_out date, ::CORBA::String_out patchno, ::CORBA::String_out hash) = 0;
  virtual ::CORBA::Long StartupUpdate(const ::PlatformInfoEx& PInfo, ::CORBA::Long which, const char* version, const char* date, const char* patchno, ::CORBA::String_out filename, ::CORBA::Long& size, ::CORBA::LongLong& ref) = 0;
  virtual ::CORBA::Long TranslateFile(::CORBA::LongLong ref, ::CORBA::Long packid, ::bfstream_out bfs, ::CORBA::Long& checknum, ::CORBA::Long& flag) = 0;
  virtual ::CORBA::Long starupTrans(const char* fileName, ::CORBA::Long& size, ::CORBA::LongLong& fileRef) = 0;
  virtual ::CORBA::Long transFile(::CORBA::LongLong fileRef, ::CORBA::Long packid, ::bfstream_out bfs, ::CORBA::Long& checknum, ::CORBA::Long& flag) = 0;
  virtual ::CORBA::Long queryPatchs(const char* version, ::patchSet_t patchs) = 0;
  virtual ::CORBA::Long getMD5FromFile(const char* fileName, ::CORBA::String_out md5Value) = 0;
  
public:  // Really protected, workaround for xlC
  virtual _CORBA_Boolean _dispatch(omniCallHandle&);

private:
  virtual void* _ptrToInterface(const char*);
  virtual const char* _mostDerivedRepoId();
  
};




class POA_FiUpdateMgr :
  public virtual _impl_FiUpdateMgr,
  public virtual ::PortableServer::ServantBase
{
public:
  virtual ~POA_FiUpdateMgr();

  inline ::FiUpdateMgr_ptr _this() {
    return (::FiUpdateMgr_ptr) _do_this(::FiUpdateMgr::_PD_repoId);
  }
};







#undef _core_attr
#undef _dyn_attr

inline void operator >>=(e _e, cdrStream& s) {
  ::operator>>=((::CORBA::ULong)_e, s);
}

inline void operator <<= (e& _e, cdrStream& s) {
  ::CORBA::ULong _0RL_e;
  ::operator<<=(_0RL_e,s);
  if (_0RL_e <= FISHARE_CLIENT) {
    _e = (e) _0RL_e;
  }
  else {
    OMNIORB_THROW(MARSHAL,_OMNI_NS(MARSHAL_InvalidEnumValue),
                  (::CORBA::CompletionStatus)s.completion());
  }
}



inline void
FiUpdateMgr::_marshalObjRef(::FiUpdateMgr_ptr obj, cdrStream& s) {
  omniObjRef::_marshal(obj->_PR_getobj(),s);
}




#ifdef   USE_stub_in_nt_dll_NOT_DEFINED_QueryUpdate
# undef  USE_stub_in_nt_dll
# undef  USE_stub_in_nt_dll_NOT_DEFINED_QueryUpdate
#endif
#ifdef   USE_core_stub_in_nt_dll_NOT_DEFINED_QueryUpdate
# undef  USE_core_stub_in_nt_dll
# undef  USE_core_stub_in_nt_dll_NOT_DEFINED_QueryUpdate
#endif
#ifdef   USE_dyn_stub_in_nt_dll_NOT_DEFINED_QueryUpdate
# undef  USE_dyn_stub_in_nt_dll
# undef  USE_dyn_stub_in_nt_dll_NOT_DEFINED_QueryUpdate
#endif

#endif  // __QueryUpdate_hh__


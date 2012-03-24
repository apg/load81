#ifndef _SCHEMEDEF_H
#define _SCHEMEDEF_H
// these are things we likely need in the code

#define SCM_DEFINE(a, b) l81.SC->vptr->scheme_define(l81.SC, l81.SC->global_env, (a), (b))
#define SCM_CONS(a, b) l81.SC->vptr->cons(l81.SC, (a), (b))
#define SCM_INT(a) l81.SC->vptr->mk_integer(l81.SC, (a))
#define SCM_REAL(a) l81.SC->vptr->mk_real(l81.SC, (a))
#define SCM_SYM(a) l81.SC->vptr->mk_symbol(l81.SC, (a))
#define SCM_STR(a) l81.SC->vptr->mk_string(l81.SC, (a))
#define SCM_CHAR(a) l81.SC->vptr->mk_character(l81.SC, (a))
#define SCM_VECTOR(a) l81.SC->vptr->mk_vector(l81.SC, (a))
#define SCM_ISSTR(a) l81.SC->vptr->is_string((a))
#define SCM_ISNUM(a) l81.SC->vptr->is_number((a))
#define SCM_ISINT(a) l81.SC->vptr->is_integer((a))
#define SCM_ISREAL(a) l81.SC->vptr->is_isreal((a))
#define SCM_ISCHAR(a) l81.SC->vptr->is_ischaracter((a))
#define SCM_IVAL(a) l81.SC->vptr->ivalue((a))
#define SCM_RVAL(a) l81.SC->vptr->rvalue((a))
#define SCM_SVAL(a) l81.SC->vptr->string_value((a))
#define SCM_CVAL(a) l81.SC->vptr->charvalue((a))
#define SCM_ISLIST(a) l81.SC->vptr->is_list(l81.SC, (a))
#define SCM_ISVEC(a) l81.SC->vptr->is_vector((a))
#define SCM_LLEN(a) l81.SC->vptr->list_length(l81.SC, (a))
#define SCM_VLEN(a) l81.SC->vptr->vector_length((a))
#define SCM_VNTH(a, b) l81.SC->vptr->vector_elem((a), (b))
#define SCM_ISPAIR(a) l81.SC->vptr->is_pair((a))
#define SCM_CAR(a) l81.SC->vptr->pair_car((a))
#define SCM_CDR(a) l81.SC->vptr->pair_cdr((a))
#define SCM_ISSYM(a) l81.SC->vptr->is_string((a))
#define SCM_SYMNAME(a) l81.SC->vptr->symname((a))
#define SCM_LOADF(a) l81.SC->vptr->load_file(l81.SC, (a))
#define SCM_LOADS(a) l81.SC->vptr->load_string(l81.SC, (a))


#define SCM_POPINT(var, args, error, more) do {       \
    _tmpp = SCM_CAR(args);                            \
    if (SCM_ISNUM(_tmpp)) {                           \
      var = (int) SCM_IVAL(_tmpp);                    \
    }                                                 \
    else {                                            \
      programError(error);                            \
      return l81.SC->NIL;                             \
    }                                                 \
    if (more) {                                       \
      args = SCM_CDR(args);                           \
    }                                                 \
  } while(0)

#define SCM_POPREAL(var, args, error, more) do {       \
    _tmpp = SCM_CAR(args);                             \
    if (SCM_ISNUM(_tmpp)) {                            \
      var = (double) SCM_RVAL(_tmpp);                  \
    }                                                  \
    else {                                             \
      programError(error);                             \
      return l81.SC->NIL;                              \
    }                                                  \
    if (more) {                                        \
      args = SCM_CDR(args);                            \
    }                                                  \
  } while(0)


#define SCM_POPSTR(var, args, error, more) do {       \
    _tmpp = SCM_CAR(args);                            \
    if (SCM_ISSTR(_tmpp)) {                           \
      var = SCM_SVAL(_tmpp);                          \
    }                                                 \
    else {                                            \
      programError(error);                            \
      return l81.SC->NIL;                             \
    }                                                 \
    if (more) {                                       \
      args = SCM_CDR(args);                           \
    }                                                 \
  } while(0)


#define SCM_POPCHAR(var, args, error, more) do {       \
    _tmpp = SCM_CAR(args);                             \
    if (SCM_ISCHAR(_tmpp)) {                           \
      var = SCM_CVAL(_tmpp);                           \
    }                                                  \
    else {                                             \
      programError(error);                             \
      return l81.SC->NIL;                              \
    }                                                  \
    if (more) {                                        \
      args = SCM_CDR(args);                            \
    }                                                  \
  } while(0)


#define SCM_STRVALUE(p)      ((p)->_object._string._svalue)
#define SCM_STRLENGTH(p)        ((p)->_object._string._length)


#endif

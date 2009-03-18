// -*-c++-*-
/* $Id: parr.h 2784 2007-04-20 16:32:00Z max $ */


#ifndef _LIBPUB_PUB3DATA_H_
#define _LIBPUB_PUB3DATA_H_

#include "pub.h"
#include "parr.h"

namespace pub3 {

  typedef enum { REL_LT, REL_GT, REL_LTE, REL_GTE } relop_t;

  //-----------------------------------------------------------------------

  class expr_t {
  public:
    expr_t () {}
    virtual ~expr_t () {}
    virtual bool eval_as_bool (penv_t *e) const = 0;
    virtual int64_t eval_as_int (penv_t *e) const = 0;
    virtual str eval_as_str (penv_t *e) const = 0;
    virtual scalar_obj_t eval_as_scalar (penv_t *e) const = 0;
    virtual bool is_null (penv_t *e) const = 0;
  };
  
  //-----------------------------------------------------------------------

  class expr_logical_t : public expr_t {
  public:
    expr_logical_t () {}
    int64_t eval_as_int (penv_t *e) const { return eval_as_bool (e); }
    scalar_obj_t eval_as_scalar (penv_t *e) const;
    str eval_as_str (penv_t *e) const;
    bool is_null (penv_t *e) const { return false; }
  };

  //-----------------------------------------------------------------------

  class expr_OR_t : public expr_logical_t {
  public:
    expr_OR_t (ptr<expr_t> t1, ptr<expr_t> t2) : _t1 (t1), _t2 (t2) {}
    bool eval_as_bool (penv_t *e) const;
    ptr<expr_t> _t1, _t2;
  };

  //-----------------------------------------------------------------------

  class expr_AND_t : public expr_logical_t  {
  public:
    expr_AND_t (ptr<expr_t> f1, ptr<expr_t> f2) : _f1 (f1), _f2 (f2) {}
    bool eval_as_bool (penv_t *e) const;
    ptr<expr_t> _f1, _f2;
  };

  //-----------------------------------------------------------------------

  class expr_NOT_t : public expr_logical_t  {
  public:
    bool eval_as_bool (penv_t *e) const;
    ptr<expr_t> _e;
  };

  //-----------------------------------------------------------------------

  class expr_relational_t : public expr_logical_t {
  public:
    expr_relational_t (ptr<expr_t> l, ptr<expr_t> r, relop_t op)
      : _l (l), _r (r), _op (op) {}
    bool eval_as_bool (penv_t *e) const;

    ptr<expr_t> _l, _r;
    relop_t _op;
  };

  //-----------------------------------------------------------------------

  class expr_EQ_t : public expr_logical_t {
  public:
    expr_EQ_t (ptr<expr_t> o1, ptr<expr_t> o2, bool pos) : 
      _o1 (o1), _o2 (o2), _pos (pos) {}

    bool eval_as_bool (penv_t *e) const;

    ptr<expr_t> _o1, _o2;
    bool _pos;
  };

  //-----------------------------------------------------------------------

};

#endif /* _LIBPUB_PUB3EXPR_H_ */

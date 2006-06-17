// -*-c++-*-
/* $Id: okcgi.h 1682 2006-04-26 19:17:22Z max $ */

/*
 *
 * Copyright (C) 2002-2004 Maxwell Krohn (max@okcupid.com)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 *
 */

#ifndef _LIBAHTTP_OKXML_DATA_H
#define _LIBAHTTP_OKXML_DATA_H

#include "async.h"
#include "qhash.h"
#include "zstr.h"
#include "smartvec.h"
#include "okws_sfs.h"

class xml_struct_t;
class xml_array_t;
class xml_int_t;
class xml_bool_t;
class xml_double_t;
class xml_str_t;
class xml_base64_t;
class xml_null_t;
class xml_method_call_t;
class xml_params_t;
class xml_param_t;
class xml_value_t;
class xml_name_t;
class xml_member_t;
class xml_data_t;
class xml_method_name_t;
class xml_method_response_t;
class xml_container_t;
class xml_fault_t;

class xml_element_t : public virtual refcount {
public:
  xml_element_t () {}
  virtual ~xml_element_t () {} 

  virtual ptr<xml_array_t> to_xml_array () { return NULL; }
  virtual ptr<xml_int_t> to_xml_int () { return NULL; }
  virtual ptr<xml_bool_t> to_xml_bool () { return NULL; }
  virtual ptr<xml_double_t> to_xml_double () { return NULL; }
  virtual ptr<xml_str_t> to_xml_str () { return NULL; }
  virtual ptr<xml_base64_t> to_xml_base64 () { return NULL; }
  virtual ptr<xml_null_t> to_xml_null () { return NULL; }
  virtual ptr<xml_method_call_t> to_xml_method_call () { return NULL; }
  virtual ptr<xml_params_t> to_xml_params () { return NULL; }
  virtual ptr<xml_param_t> to_xml_param () { return NULL; }
  virtual ptr<xml_value_t> to_xml_value () { return NULL; }
  virtual ptr<xml_name_t> to_xml_name () { return NULL; }
  virtual ptr<xml_member_t> to_xml_member () { return NULL; }
  virtual ptr<xml_data_t> to_xml_data () { return NULL; }
  virtual ptr<xml_method_name_t> to_xml_method_name () { return NULL; }
  virtual ptr<xml_method_response_t> to_xml_method_response () { return NULL; }
  virtual ptr<xml_fault_t> to_xml_fault () { return NULL; }

  virtual ptr<xml_container_t> to_xml_container () { return NULL; }
  virtual ptr<const xml_container_t> to_xml_container () const { return NULL; }
  virtual ptr<xml_struct_t> to_xml_struct () { return NULL; }
  virtual ptr<const xml_struct_t> to_xml_struct () const { return NULL; }
  virtual ptr<const xml_method_call_t> to_xml_method_call () const
  { return NULL; }
  virtual ptr<const xml_member_t> to_xml_member () const { return NULL; }

  virtual bool put (const str &s, ptr<xml_element_t> el) { return false; }
  virtual bool put (size_t i, ptr<xml_element_t> el) { return false; }

  // Assign the 'this' element to the 'to' internally if possible; if not
  // just use 'to' and discard this (signaled by returning false).
  virtual bool assign_to (ptr<xml_element_t> to) { return false; }
  virtual bool set_pointer_to_me (ptr<xml_value_t> *v) { return false; }
  virtual bool set_pointer_to_me (ptr<xml_data_t> *v) { return false; }

  virtual int to_int () const { return 0; }
  virtual str to_str () const { return ""; }
  virtual str to_bool () const { return false; }
  virtual str to_base64 () const { return armor64 (NULL, 0); }
  virtual bool is_value () const { return false; }

  virtual ptr<xml_element_t> clone () const 
  { return New refcounted<xml_element_t> (); }

  // call this to signal that fault happened on the given object;
  // only does any interesting for methodResponse objects
  virtual void fault (ptr<xml_fault_t> f) {}

  // should it be a strbuf or a zbuf?
  virtual void dump (zbuf &b, int lev) const;
  virtual void dump_data (zbuf &b, int lev) const {}
  virtual bool dump_to_python (strbuf &b) const { return true; }
  void dump (zbuf &b) const { dump (b, 0); }

  operator int () const { return to_int (); }
  operator str () const { return to_str (); }
  operator bool () const { return to_bool (); }

  virtual ptr<xml_element_t> generate (const char *) const
  { return New refcounted<xml_element_t> (); }
  virtual const char *name () const { return NULL; }
  
  // used during parsing
  virtual bool is_a (const char *s) const { return !strcmp (name (), s); }
  virtual bool add (const char *buf, int len) { return false; }
  virtual bool gets_char_data () const { return false; }
  virtual bool add (ptr<xml_element_t> e) { return false; }
  virtual bool close_tag () { return true; }
};

class xml_container_t : public xml_element_t,
			public vec<ptr<xml_element_t> >
{
public:
  xml_container_t () : vec<ptr<xml_element_t> > () {}
  xml_container_t (const xml_container_t &x)
    : vec<ptr<xml_element_t> > (x) {}

  virtual ~xml_container_t () {}

  virtual bool add (ptr<xml_element_t> e);
  virtual bool can_contain (ptr<xml_element_t> e) const { return false; }
  void dump_data (zbuf &b, int len) const;
  virtual bool dump_to_python (strbuf &b) const;

  virtual ptr<xml_element_t> &get_r (size_t i, bool mk = true);
  virtual ptr<const xml_element_t> get (size_t i) const;

  ptr<xml_container_t> to_xml_container () { return mkref (this); }
  ptr<const xml_container_t> to_xml_container () const 
  { return mkref (const_cast<xml_container_t *> (this)); }

  ptr<xml_container_t> clone_typed () const
  { return New refcounted<xml_container_t> (*this); }
  virtual ptr<xml_element_t> clone () const
  { return clone_typed (); }

  virtual const char * py_open_container () const { return "("; }
  virtual const char * py_close_container () const { return ")"; }
};

class xml_top_level_t : public xml_container_t {
public:
  const char *name () const { return "topLevel"; }
  bool can_contain (ptr<xml_element_t> e) const 
  { return e->to_xml_method_call (); }

  ptr<xml_top_level_t> clone_typed () const 
  { return New refcounted<xml_top_level_t> (*this); }
  virtual ptr<xml_element_t> clone () const 
  { return clone_typed (); }
  bool dump_to_python (strbuf &b) const;
};

class xml_name_t : public xml_element_t {
public:
  xml_name_t () {} 
  xml_name_t (const str &s) : _value (s) {}
  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_name_t> (); }
  virtual const char *name () const { return "name"; }
  virtual ptr<xml_name_t> to_xml_name () { return mkref (this); }
  str value () const { return _value ;}
  void set_value (const str &v) { _value = v;}
  static ptr<xml_name_t> alloc (const str &s) 
  { return New refcounted<xml_name_t> (s); }
  void dump_data (zbuf &z, int lev) const 
  { if (_value) z << _value; }
  bool dump_to_python (strbuf &b) const;

  bool gets_char_data () const { return true; }
  bool add (const char *s, int l);

  ptr<xml_name_t> clone_typed () const 
  { return New refcounted<xml_name_t> (_value); }
  virtual ptr<xml_element_t> clone () const { return clone_typed (); }
protected:
  str _value;
};

class xml_method_name_t : public xml_name_t {
public:
  xml_method_name_t () : xml_name_t () {}
  xml_method_name_t (const str &s) : xml_name_t (s) {}

  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_method_name_t> (); }
  static ptr<xml_method_name_t> alloc (const str &s)
  { return New refcounted<xml_method_name_t> (s); }

  ptr<xml_name_t> to_xml_name () { return NULL; }
  ptr<xml_method_name_t> to_xml_method_name () { return mkref (this); }

  const char *name () const { return "methodName"; }
  bool dump_to_python (strbuf &b) const;

  ptr<xml_method_name_t> clone_typed () const
  { return New refcounted<xml_method_name_t> (*this); }
  ptr<xml_element_t> clone () const
  { return xml_method_name_t::clone_typed (); }

};

class xml_method_call_t : public xml_element_t {
public:
  xml_method_call_t (const str &n) 
    : _method_name (xml_method_name_t::alloc (n)) {}
  xml_method_call_t () {}
  xml_method_call_t (ptr<xml_method_name_t> n, ptr<xml_params_t> p)
    : _method_name (n), _params (p) {}

  str method_name () const 
  { return _method_name ? _method_name->value () : NULL; }

  void set_method_name (const str &s) 
  { _method_name = xml_method_name_t::alloc (s); }

  ptr<xml_params_t> params () { return _params; }
  ptr<const xml_params_t> params () const { return _params; }
  void set_params (ptr<xml_params_t> p) { _params = p ;}
  
  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_method_call_t> (); }
  const char *name () const { return "methodCall"; }

  ptr<xml_method_call_t> to_xml_method_call () { return mkref (this); }
  ptr<const xml_method_call_t> to_xml_method_call () const
  { return mkref (const_cast<xml_method_call_t *> (this)); }

  bool add (ptr<xml_element_t> e);
  void dump_data (zbuf &b, int lev) const;
  bool dump_to_python (strbuf &b) const;

  ptr<xml_method_call_t> clone_typed () const;
  ptr<xml_element_t> clone () const { return clone_typed (); }

private:
  ptr<xml_method_name_t> _method_name;
  ptr<xml_params_t> _params;
};

class xml_value_wrapper_t : public xml_element_t {
public:
  xml_value_wrapper_t () {}
  xml_value_wrapper_t (ptr<xml_value_t> v) : _value (v) {}
  
  ptr<xml_container_t> to_xml_container ();
  ptr<const xml_container_t> to_xml_container () const;
  ptr<xml_struct_t> to_xml_struct ();
  ptr<const xml_struct_t> to_xml_struct () const;
  void dump_data (zbuf &b, int lev) const;
  bool dump_to_python (strbuf &b) const;
  bool add (ptr<xml_element_t> e);
  ptr<xml_value_t> mkvalue ();
  ptr<xml_value_t> cpvalue () const;

  virtual int to_int () const;
  virtual str to_str () const;
  virtual str to_base64 () const;

  

  ptr<xml_value_wrapper_t> clone_typed () const
  { return New refcounted<xml_value_wrapper_t> (cpvalue ()); }
  ptr<xml_element_t> clone() const { return clone_typed (); }

  bool assign_to (ptr<xml_element_t> to);
  bool set_pointer_to_me (ptr<xml_value_t> *v);

protected:
  ptr<xml_value_t> _value;
};

class xml_param_t : public xml_value_wrapper_t {
public:
  xml_param_t () {}
  xml_param_t (ptr<xml_value_t> v) : xml_value_wrapper_t (v) {}
  void set_value (ptr<xml_value_t> x) { _value = x; }
  ptr<xml_value_t> value () { return _value; }
  ptr<xml_param_t> to_xml_param () { return mkref (this); }

  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_param_t> (); }
  const char *name () const { return "param"; }

  static ptr<xml_param_t> alloc ()
  { return New refcounted<xml_param_t> (); }

  ptr<xml_param_t> clone_typed () const
  { return New refcounted<xml_param_t> (cpvalue ()); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
};

class xml_params_t : public xml_container_t {
public:
  xml_params_t () {}
  xml_params_t (const xml_params_t &x) : xml_container_t (x) {}

  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_params_t> (); }
  const char *name () const { return "params"; }
  bool can_contain (ptr<xml_element_t> e) const { return e->to_xml_param (); }
  ptr<xml_params_t> to_xml_params () { return mkref (this); }

  ptr<xml_element_t> &get_r (size_t s, bool mk = true) ;

  ptr<xml_params_t> clone_typed () const
  { return New refcounted<xml_params_t> (*this); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
};

class xml_fault_t : public xml_value_wrapper_t {
public:
  xml_fault_t () {}
  xml_fault_t (ptr<xml_value_t> v) : xml_value_wrapper_t (v) {}

  ptr<xml_element_t> generate (const char *) const
  { return New refcounted<xml_fault_t> (); }
  const char *name () const { return "fault"; }
  ptr<xml_fault_t> to_xml_fault () { return mkref (this); }
  static ptr<xml_fault_t> alloc (int rc, const str &s);

  ptr<xml_fault_t> clone_typed () const
  { return New refcounted<xml_fault_t> (cpvalue ()); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
};

class xml_method_response_t : public xml_element_t {
public:
  xml_method_response_t () {}
  xml_method_response_t (ptr<xml_params_t> p, ptr<xml_element_t> b)
    : _params (p), _body (b) {}
  ptr<xml_params_t> params () const { return _params; }
  void set_params (ptr<xml_params_t> p) { _params = p; }
  ptr<xml_element_t> generate (const char *) const
  { return New refcounted<xml_method_response_t> (); }
  const char *name () const { return "methodReponse"; }
  ptr<xml_method_response_t> to_xml_method_response () { return mkref (this); }
  bool add (ptr<xml_element_t> e);
  void dump_data (zbuf &b, int lev) const 
  { if (_body) _body->dump (b, lev); }

  static ptr<xml_method_response_t> alloc () 
  { return New refcounted<xml_method_response_t> (); }
  
  ptr<xml_container_t> to_xml_container ();
  ptr<const xml_container_t> to_xml_container () const { return _params; }

  void fault (int c, const str &s) { fault (xml_fault_t::alloc (c, s)); }
  void fault (ptr<xml_fault_t> f) { _body = f; }

  ptr<xml_method_response_t> clone_typed () const;
  ptr<xml_element_t> clone () const { return clone_typed (); }
  bool dump_to_python (strbuf &b) const;
private:
  ptr<xml_params_t> _params;
  ptr<xml_element_t> _body;
};

extern ptr<xml_null_t> null_element;
extern ptr<xml_value_t> null_value;

class xml_null_t : public xml_element_t {
public:
  xml_null_t () {}
  ptr<xml_null_t> to_xml_null () { return mkref (this); }
  static ptr<xml_null_t> alloc () { return null_element; }
  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_params_t> (); }
  const char *name () const { return "null"; }

  ptr<xml_null_t> clone_typed () const { return xml_null_t::alloc (); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
};

class xml_scalar_t : public xml_element_t {
public:
  xml_scalar_t () {}
  bool add (const char *b, int s);
  bool gets_char_data () const { return true; }
  bool is_value () const { return true; }
  bool set_pointer_to_me (ptr<xml_value_t> *v);
protected:
  strbuf _buf;
};

class xml_double_t : public xml_scalar_t {
public:
  xml_double_t () : _val (0) {}
  xml_double_t (double v) : _val (v) {}
  ptr<xml_double_t> to_xml_double () { return mkref (this); }
  double to_double () const { return _val; }
  str to_str () const;
  void set (double d) { _val = d; }

  ptr<xml_element_t> generate (const char *n) const 
  { return New refcounted<xml_double_t> (); }

  const char *name () const { return "double"; }
  bool close_tag ();
  void dump_data (zbuf &b, int lev) const;
  bool dump_to_python (strbuf &b) const;

  ptr<xml_double_t> clone_typed () const
  { return New refcounted<xml_double_t> (_val); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
private:
  const char *to_const_char (int *sz) const;
  double _val;
};

class xml_int_t : public xml_scalar_t {
public:
  xml_int_t (const char *tag) : _tag (tag), _val (0) {}
  xml_int_t (const str &t, int v) : _tag (t), _val (v) {}
  xml_int_t (int i = 0) : _val (i) {}
  ptr<xml_int_t> to_xml_int () { return mkref (this); }
  int to_int () const { return _val; }
  str to_str () const { strbuf b; b << _val; return b; }
  void set (int i) { _val = i; }
  bool is_value () const { return true; }

  ptr<xml_element_t> generate (const char *n) const 
  { return New refcounted<xml_int_t> (n); }

  static ptr<xml_int_t> alloc (int i)
  { return New refcounted<xml_int_t> (i); }

  const char *name () const { return "int"; }
  bool is_a (const char *n) const { return !strcmp (_tag.cstr (), n); }
  bool close_tag ();
  void dump_data (zbuf &b, int lev) const { b << _val; }
  bool dump_to_python (strbuf &b) const { b << _val; return true; }

  ptr<xml_int_t> clone_typed () const 
  { return New refcounted<xml_int_t> (_tag, _val); }
  ptr<xml_element_t> clone () const { return clone_typed (); }

private:
  const str _tag;
  int _val;
};

class xml_str_t : public xml_scalar_t {
public:
  xml_str_t (const str &s) : _val (s) {}
  xml_str_t () {}
  ptr<xml_str_t> to_xml_str () { return mkref (this); }
  str to_str () const { return _val; }
  void set (const str &v) { _val = v; }
  const char *name () const { return "string"; }
  bool close_tag ();
  ptr<xml_element_t> generate (const char *n) const 
  { return New refcounted<xml_str_t> (); }
  void dump_data (zbuf &z, int level) const { if (_val) z << _val; }
  bool dump_to_python (strbuf &b) const;

  static str escape (const str &in);

  static ptr<xml_str_t> alloc (const str &s)
  { return New refcounted<xml_str_t> (s); }

  ptr<xml_str_t> clone_typed () const 
  { return New refcounted<xml_str_t> (_val); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
private:
  str _val;
};

class xml_base64_t : public xml_scalar_t {
public:
  xml_base64_t (const str &b, bool encoded = false) : 
    _val (encoded ? b : armor64 (b)),
    _d_val (!encoded ? b : sNULL) {}
  xml_base64_t () : _val (armor64 (NULL, 0)) {}

  ptr<xml_base64_t> to_xml_base64 () { return mkref (this); }

  str to_base64 () const { return _val; }
  str to_str () const { return decode (); }

  str decode () const;
  void set (const str &v, bool encoded = false) 
  { _val = encoded ? v : armor64 (v); _d_val = NULL; }
  
  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_base64_t>(); }
  const char *name () const { return "base64"; }
  bool close_tag ();

  static ptr<xml_base64_t> alloc (const str &s, bool encoded = false)
  { return New refcounted<xml_base64_t> (s, encoded); }

  ptr<xml_base64_t> clone_typed () const 
  { return New refcounted<xml_base64_t> (_val); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
  void dump_data (zbuf &z, int lev) const
  { if (_val) z << _val ; }
  bool dump_to_python (strbuf &b) const;
private:
  str _val;
  mutable str _d_val;
};

class xml_struct_t : public xml_container_t {
public:
  xml_struct_t () {}
  xml_struct_t (const xml_struct_t &s);
  ptr<const xml_element_t> get (const str &s) const;
  ptr<xml_element_t> &get_r (const str &s);
  bool put (const str &s, ptr<xml_element_t> el);
  ptr<xml_struct_t> to_xml_struct () { return mkref (this); }
  ptr<const xml_struct_t> to_xml_struct () const 
  { return mkref (const_cast<xml_struct_t *> (this)); }

  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_struct_t>(); }
  const char *name () const { return "struct"; }
  bool is_value () const { return true; }
  bool can_contain (ptr<xml_element_t> e) const { return e->to_xml_member (); }
  bool close_tag ();

  bool set_pointer_to_me (ptr<xml_value_t> *v);
  
  ptr<xml_struct_t> clone_typed () const
  { return New refcounted<xml_struct_t> (*this); }
  ptr<xml_element_t> clone () const { return clone_typed (); }

  const char * py_open_container () const { return "{"; }
  const char * py_close_container () const { return "}"; }

private:
  qhash<str, size_t> _members;
};

class xml_array_t : public xml_element_t {
public:
  xml_array_t () {}
  xml_array_t (ptr<xml_data_t> d) : _data (d) {}

  bool put (size_t i, ptr<xml_element_t> el);
  ptr<xml_array_t> to_xml_array () { return mkref (this); }

  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_array_t>(); }
  const char *name () const { return "array"; }
  bool is_value () const { return true; }
  ptr<xml_data_t> data () { return _data; }
  bool add (ptr<xml_element_t> e);

  ptr<xml_container_t> to_xml_container ();
  ptr<const xml_container_t> to_xml_container () const { return _data; }
  void dump_data (zbuf &z, int lev) const;
  bool dump_to_python (strbuf &b) const;

  bool assign_to (ptr<xml_element_t> to);
  bool set_pointer_to_me (ptr<xml_data_t> *d);
  bool set_pointer_to_me (ptr<xml_value_t> *v);

  ptr<xml_array_t> clone_typed () const ;
  ptr<xml_element_t> clone () const { return clone_typed (); }
private:
  ptr<xml_data_t> _data;
};

class xml_value_t : public xml_element_t {
public:
  xml_value_t (ptr<xml_element_t> e = NULL) : _e (e) {}
  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_value_t> (); }
  const char *name () const { return "value"; }

  ptr<xml_element_t> &element () { return _e; }
  ptr<const xml_element_t> element_const () const { return _e; }

  void set_element (ptr<xml_element_t> e) { _e = e; }
  bool add (ptr<xml_element_t> e);
  ptr<xml_value_t> to_xml_value () { return mkref (this); }

  int to_int () const { return _e ? _e->to_int () : xml_element_t::to_int (); }
  str to_str () const { return _e ? _e->to_str () : xml_element_t::to_str (); }
  str to_base64 () const 
  { return _e ? _e->to_base64 () : xml_element_t::to_base64(); }
  void dump_data (zbuf &b, int level) const { if (_e) _e->dump (b, level); }
  bool dump_to_python (strbuf &b) const;

  static ptr<xml_value_t> alloc (ptr<xml_element_t> e = NULL)
  { return New refcounted<xml_value_t> (e); }

  ptr<xml_container_t> to_xml_container ();
  ptr<xml_struct_t> to_xml_struct ();

  ptr<const xml_container_t> to_xml_container () const 
  { return _e->to_xml_container (); }
  ptr<const xml_struct_t> to_xml_struct () const 
  { return _e->to_xml_struct (); }

  bool assign_to (ptr<xml_element_t> to);
  bool set_pointer_to_me (ptr<xml_value_t> *v);

  ptr<xml_value_t> clone_typed () const
  { return New refcounted<xml_value_t> (_e ? _e->clone () : _e); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
  
private:
  ptr<xml_element_t> _e;
};

class xml_member_t : public xml_element_t {
public:
  xml_member_t () {}

  xml_member_t (const str &s) : 
    _member_name (xml_name_t::alloc (s)),
    _member_value (xml_value_t::alloc ()) {}

  xml_member_t (ptr<xml_name_t> n, ptr<xml_element_t> v)
    : _member_name (n), _member_value (v) {}

  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_member_t> (); }
  const char *name () const { return "member"; }
  ptr<xml_member_t> to_xml_member () { return mkref (this); }
  ptr<const xml_member_t> to_xml_member () const
  { return mkref (const_cast<xml_member_t *>  (this)); }

  ptr<xml_name_t> member_name () const { return _member_name ; }

  str member_name_str () const 
  { return _member_name ? _member_name->value () : sNULL; }

  ptr<const xml_element_t> member_value () const { return _member_value; }
  ptr<xml_element_t> &member_value () { return _member_value; }

  bool add (ptr<xml_element_t> e);
  void dump_data (zbuf &b, int lev) const;
  bool dump_to_python (strbuf &b) const;

  ptr<xml_member_t> clone_typed () const;
  ptr<xml_element_t> clone () const { return clone_typed (); }

private:
  ptr<xml_name_t> _member_name;
  ptr<xml_element_t> _member_value;
};

class xml_data_t : public xml_container_t {
public:
  xml_data_t () {}
  xml_data_t (const xml_data_t &x) : xml_container_t (x) {} 
  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_data_t> (); }
  const char *name () const { return "data"; }
  ptr<xml_data_t> to_xml_data () { return mkref (this); }
  bool can_contain (ptr<xml_element_t> e) const { return e->to_xml_value (); }
  static ptr<xml_data_t> alloc () { return New refcounted<xml_data_t> (); }

  bool set_pointer_to_me (ptr<xml_data_t> *d);

  ptr<xml_data_t> clone_typed () const
  { return New refcounted<xml_data_t> (*this); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
};

class xml_bool_t : public xml_scalar_t {
public:
  xml_bool_t () {}
  xml_bool_t (bool b) : _val (b) {}

  ptr<xml_element_t> generate (const char *) const 
  { return New refcounted<xml_bool_t> (); }
  const char *name () const { return "boolean"; }
  ptr<xml_bool_t> to_xml_bool () { return mkref (this); }
  bool close_tag ();

  static ptr<xml_bool_t> alloc (bool b) 
  { return New refcounted<xml_bool_t> (b); }

  void dump_data (zbuf &b, int lev) const { b << (_val ? 1 : 0); }
  bool dump_to_python (strbuf &b) const;

  ptr<xml_bool_t> clone_typed () const 
  { return New refcounted<xml_bool_t> (_val); }
  ptr<xml_element_t> clone () const { return clone_typed (); }
private:
  bool _val;
};

bool has_non_ws (const char *buf, int len);



#endif /* _LIBAHTTP_OKXML_DATA_H */
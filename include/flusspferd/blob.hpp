// vim:ts=2:sw=2:expandtab:autoindent:filetype=cpp:
/*
Copyright (c) 2008 Aristid Breitkreuz, Ruediger Sonderfeld

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/
#ifndef FLUSSPFERD_BLOB_HPP
#define FLUSSPFERD_BLOB_HPP

#include "native_object_base.hpp"
#include "class.hpp"
#include <vector>

namespace flusspferd {

class blob : public native_object_base {
public:
  blob(object const &obj, call_context &x);
  blob(object const &obj, unsigned char const *data, std::size_t length);
  ~blob();

  struct class_info : flusspferd::class_info {
    static char const *full_name() { return "Blob"; }

    static char const *constructor_name() { return "Blob"; }
    typedef boost::mpl::size_t<1> constructor_arity;

    static object create_prototype();
    static void augment_constructor(object &);
  };

  unsigned char *data() { return &data_[0]; }
  std::size_t size() { return data_.size(); }

  std::vector<unsigned char> &get() { return data_; }

protected:
  void property_op(property_mode mode, value const &id, value &data);
  bool property_resolve(value const &id, unsigned access);

private: // JS methods
  void append(blob const &o);
  object slice(int from, boost::optional<int> to);
  object to_array();
  object clone();
  string as_utf8();
  string as_utf16();

  value set_index(int index, value x);
  value get_index(int index);
private: // JS properties
  void prop_length(property_mode mode, value &data);

private: // JS constructor methods
  static object from_utf8(string const &);
  static object from_utf16(string const &);

private:
  std::vector<unsigned char> data_;

  static unsigned char el_from_value(value const &);

  void init();
};

}

#endif
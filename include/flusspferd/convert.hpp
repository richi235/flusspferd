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

#ifndef FLUSSPFERD_CONVERT_HPP
#define FLUSSPFERD_CONVERT_HPP

#include "value.hpp"
#include "root.hpp"
#include "exception.hpp"
#include "implementation/string.hpp"
#include <boost/noncopyable.hpp>
#include <boost/utility/enable_if.hpp>
#include <boost/type_traits/is_float.hpp>
#include <boost/type_traits/is_integral.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/optional.hpp>
#include <limits>

namespace flusspferd {

class string;
class object;
class function;
class array;
class native_object_base;
class native_function_base;

namespace detail {

template<typename T, typename Condition = void>
struct convert;

template<
    typename T,
    typename O = typename boost::remove_cv<T>::type,
    typename Condition = void
  >
struct convert_ptr{
  struct to_value {
    typename convert<O>::to_value base;

    value perform(T *p) {
      return base.perform(*p);
    }
  };

  struct from_value {
    typename convert<O>::from_value base;

    O holder;

    T *perform(value const &v) {
      holder = base.perform(v);
      return &holder;
    }
  };
};

template<typename T, typename C = T>
struct to_value_helper {
  value perform(T const &x) {
    return value(C(x));
  }
};

template<typename T>
struct to_value_helper<T, T> {
  value perform(T const &x) {
    return value(x);
  }
};

template<>
struct convert<value> {
  struct to_value {
    value const &perform(value const &v) {
      return v;
    }
  };

  typedef to_value from_value;
};

template<>
struct convert<bool> {
  typedef to_value_helper<bool> to_value;

  struct from_value {
    bool perform(value const &v) {
      return v.to_boolean();
    }
  };
};

template<typename T>
struct convert<T *> : convert_ptr<T> {};

template<typename T>
struct convert<T &> {
  struct to_value {
    typename convert_ptr<T>::to_value base;

    value perform(T &x) {
      return base.perform(&x);
    }
  };

  struct from_value {
    typename convert_ptr<T>::from_value base;

    T &perform(value const &x) {
      return *base.perform(x);
    }
  };
};

template<typename T>
struct convert<
    T,
    typename boost::enable_if<boost::is_integral<T> >::type
  >
{
  typedef to_value_helper<T, double> to_value;

  struct from_value {
    typedef std::numeric_limits<T> limits;

    T perform(value const &v) {
      return v.to_integral_number(limits::digits, limits::is_signed);
    }
  };
};

template<typename T>
struct convert<
    T,
    typename boost::enable_if<boost::is_float<T> >::type
  >
{
  typedef to_value_helper<T, double> to_value;

  struct from_value {
    T perform(value const &v) {
      return v.to_number();
    }
  };
};

template<typename T>
struct convert< boost::optional<T> > {
  struct to_value {
    typename convert<T>::to_value base;

    value perform(boost::optional<T> const &x) {
      if (!x)
        return value();
      return base.perform(x.get());
    }
  };

  struct from_value {
    typename convert<T>::from_value base;

    boost::optional<T> perform(value const &v) {
      if (v.is_undefined() || v.is_null())
        return boost::optional<T>();
      return base.perform(v);
    }
  };
};

template<typename T, std::size_t N>
struct convert<T [N]> {
  typedef typename convert<T const *>::to_value to_value;
};

template<>
struct convert<object>;

template<>
struct convert<function>;

template<>
struct convert<array>;

template<>
struct convert<string>;

template<>
struct convert<char const *>;

template<>
struct convert<std::string>;

template<>
struct convert<std::basic_string<char16_t> >;

template<typename T>
struct convert_ptr<T, native_object_base>;

template<typename T>
struct convert_ptr<T, native_function_base>;

}

template<typename T>
class convert : private detail::convert<T>, private boost::noncopyable {
private:
  typedef detail::convert<T> base;

public:
  typedef typename base::to_value to_value;
  typedef typename base::from_value from_value;
};

}

#endif
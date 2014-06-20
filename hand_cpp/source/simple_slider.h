// Copyright (c) 2011, Marin Saric <marin.saric@gmail.com>
// All rights reserved.
//
// This file is a part of LibHand. LibHand is open-source software. You can
// redistribute it and/or modify it under the terms of the LibHand
// license. The LibHand license is the BSD license with an added clause that
// requires academic citation. You should have received a copy of the
// LibHand license (the license.txt file) along with LibHand. If not, see
// <http://www.libhand.org/>

// SimpleSlider

#ifndef SIMPLE_SLIDER_H
#define SIMPLE_SLIDER_H

# include <string>

namespace libhand {

using namespace std;

class SimpleSlider;

class SliderRefresh {
public:
  virtual ~SliderRefresh() {}

  virtual void RefreshMe(SimpleSlider *slider) {}
};

class SimpleSlider {
 public:
  static const int kDefaultRawMaxVal = 100;

  SimpleSlider(const string &name,
               const string &window_name,
               float interval_start = 0,
               float interval_end = 1,
               int raw_max_val = kDefaultRawMaxVal,
               SliderRefresh *refresh = NULL);

  const string &name() const { return name_; }
  const string &window_name() const { return window_name_; }

  float interval_start() { return interval_start_; }
  float interval_end() { return interval_end_; }

  int raw_val() const;
  void set_raw_val(int raw_val);

  float val() const;
  void set_val(float val);

 private:
  string name_;
  string window_name_;
  float interval_start_;
  float interval_end_;
  int raw_max_val_;
  int cur_raw_val_;
  SliderRefresh *refresh_;

  static void Callback(int state, void *data);

  // Disallow
  SimpleSlider();
  SimpleSlider(const SimpleSlider &rhs);
  SimpleSlider& operator= (const SimpleSlider &rhs);
};

// To automatically generate a SliderRefresh class that will call any
// method inside an existing class MyClass, you can do
//
// MyClass *my_class = ...;
//
// SliderRefreshFn<MyClass> slider_refresh(my_class, &MyClass::MyMethod);

template<class T>
class SliderRefreshFn : public SliderRefresh {
 public:
  typedef void (T::*CallbackFn)(SimpleSlider *);

  SliderRefreshFn(T *object, CallbackFn callback) :
    object_(object), callback_(callback) {}
  virtual ~SliderRefreshFn() {}
  void RefreshMe(SimpleSlider *slider) { (object_->*callback_)(slider); }

 private:
  T *object_;
  CallbackFn callback_;
};

}  // namespace libhand
#endif  // SIMPLE_SLIDER_H

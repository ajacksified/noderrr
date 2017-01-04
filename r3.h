#ifndef R3_H
#define R3_H

#include <nan.h>

namespace r3 {
  #include <r3/r3.h>
}

class R3 : public Nan::ObjectWrap {
 public:
  static void Init(v8::Local<v8::Object> exports);

 private:
  explicit R3(r3::node * tree);
  ~R3();

  static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void Insert(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void Compile(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void Match(const Nan::FunctionCallbackInfo<v8::Value>& info);

  static Nan::Persistent<v8::Function> constructor;

  r3::node* tree_;
};

#endif

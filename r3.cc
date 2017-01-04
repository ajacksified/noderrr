#include "r3.h"
#include <cstring>
#include <iostream>

#include <stdio.h>
#include <stdlib.h>

Nan::Persistent<v8::Function> R3::constructor;

R3::R3(r3::node* tree) : tree_(tree) {
}

R3::~R3() {
}

r3::route *matched_route;
R3* tree;
v8::Local<v8::Array> tokens;
v8::Local<v8::Object> matchRet;
v8::Isolate* isolate;
char* path;
v8::Local<v8::String::Utf8Value> str;
r3::match_entry *entry;

void *ptr_from_value_persistent(const v8::Local<v8::Value> &value) {
    Nan::Persistent<v8::Value> *data = new Nan::Persistent<v8::Value>(value);
    return data;
}

const char* ToCString(const v8::String::Utf8Value& value) {
  return *value ? *value : "";
}

void R3::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  // Prepare constructor template
  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("R3").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  // Prototype
  Nan::SetPrototypeMethod(tpl, "insert", Insert);
  Nan::SetPrototypeMethod(tpl, "compile", Compile);
  Nan::SetPrototypeMethod(tpl, "match", Match);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("R3").ToLocalChecked(), tpl->GetFunction());
}

void R3::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  double length = info[0]->IsUndefined() ? 20 : info[0]->NumberValue();
  r3::node* tree = r3::r3_tree_create(length);
  R3* obj = new R3(tree);
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void R3::Insert(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  tree = ObjectWrap::Unwrap<R3>(info.Holder());

  v8::String::Utf8Value str(info[0]);
  path = *str;

  int method = 0;

  r3::r3_tree_insert_routel(
    tree->tree_,
    method,
    path,
    strlen(path),
    ptr_from_value_persistent(info[1])
  );

  info.GetReturnValue().Set(Nan::Null());
}

void R3::Compile(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  tree = ObjectWrap::Unwrap<R3>(info.Holder());

  char *errstr = NULL;
  int err = r3::r3_tree_compile(tree->tree_, &errstr);

  if (err) {
    Nan::ThrowError(errstr);
    delete errstr;
  }

  entry = r3::match_entry_createl("/", 1);
  r3::str_array_free(entry->vars);
}

void R3::Match(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  R3* tree = ObjectWrap::Unwrap<R3>(info.Holder());
  v8::Isolate* isolate = info.GetIsolate();

  v8::String::Utf8Value str(info[0]);
  path = *str;

  entry->path = path;
  entry->path_len = str.length();
  entry->vars = r3::str_array_create(3);

  r3::route *matched_route = r3::r3_tree_match_route(tree->tree_, entry);

  if (matched_route) {
    v8::Local<v8::Object> obj = v8::Object::New(isolate);
    v8::Local<v8::Array> tokens = v8::Array::New(isolate);

    for (int i = 0; i < entry->vars->len; i++) {
      tokens->Set(i, v8::String::NewFromUtf8(isolate, entry->vars->tokens[i]));
    }

    obj->Set(v8::String::NewFromUtf8(isolate, "params"), tokens);

    v8::Local<v8::Value> fn = Nan::New(*reinterpret_cast<Nan::Persistent<v8::Value> *>(matched_route->data));
    obj->Set(v8::String::NewFromUtf8(isolate, "fn"), fn);

    info.GetReturnValue().Set(obj);
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }

  r3::str_array_free(entry->vars);
  entry->data = NULL;
}

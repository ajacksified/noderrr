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

R3* tree;
v8::Local<v8::Array> tokens;
v8::Local<v8::Object> matchRet;
v8::Isolate* isolate;
const char* path;
v8::Local<v8::String::Utf8Value> str;
v8::Local<v8::Function> fn;

struct Work {
  uv_work_t  request;
  Nan::Callback* callback;

  R3* tree;
  const char* path;
  r3::match_entry * entry;
  r3::route route;
};

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

  const char* path = ToCString(str);

  r3::r3_tree_insert_routel(
    tree->tree_,
    0,
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
}

// called by libuv worker in separate thread
static void WorkAsync(uv_work_t *req) {
  Work *work = static_cast<Work *>(req->data);
  //work->entry = r3::match_entry_create(work->path);
  //work->route = r3::r3_tree_match_route(work->tree, work->entry);

  /*
  if (matched_route) {
    matchRet = v8::Object::New(isolate);
    tokens = v8::Array::New(isolate);

    for (int i = 0; i < entry->vars->len ; i++) {
      tokens->Set(i, v8::String::NewFromUtf8(isolate, entry->vars->tokens[i]));
    }

    matchRet->Set(v8::String::NewFromUtf8(isolate, "params"), tokens);

    v8::Local<v8::Value> fn = Nan::New(*reinterpret_cast<Nan::Persistent<v8::Value> *>(matched_route->data));
    matchRet->Set(v8::String::NewFromUtf8(isolate, "fn"), fn);
    info.GetReturnValue().Set(matchRet);
  } else {
    info.GetReturnValue().Set(Nan::Null());
  }
  */
}

// called by libuv in event loop when async function completes
static void WorkAsyncComplete(uv_work_t *req, int status) {
  isolate = v8::Isolate::GetCurrent();
  v8::HandleScope handleScope(isolate);

  Work *work = static_cast<Work *>(req->data);
  v8::Handle<v8::Value> argv[] = { };

  // execute the callback
  v8::Local<v8::Function>::New(isolate, work->callback->GetFunction())->
    Call(isolate->GetCurrentContext()->Global(), 1, argv);

  // Free up the persistent function callback
  work->callback->Reset();

  delete work;
}

void R3::Match(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  tree = ObjectWrap::Unwrap<R3>(info.Holder());
  isolate = info.GetIsolate();
  v8::HandleScope handleScope(isolate);

  v8::String::Utf8Value str(info[0]); // super slow
  path = ToCString(str);

  fn = v8::Local<v8::Function>::Cast(info[1]);
  Nan::Callback callback(fn);

  Work * work = new Work();
  work->request.data = work;
  work->tree = tree;
  work->path = path;

  work->callback = &callback;
  //work->callback->Reset(isolate, callback);

  uv_queue_work(uv_default_loop(), &work->request, WorkAsync, WorkAsyncComplete);

  info.GetReturnValue().Set(Undefined(isolate));
}

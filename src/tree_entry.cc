/*
Copyright (c) 2011, Tim Branyen @tbranyen <tim@tabdeveloper.com>
*/

#include <v8.h>
#include <node.h>

#include "../vendor/libgit2/include/git2.h"

#include "../include/repo.h"
#include "../include/blob.h"
#include "../include/tree.h"
#include "../include/object.h"
#include "../include/oid.h"
#include "../include/tree_entry.h"

using namespace v8;
using namespace node;

void GitTreeEntry::Initialize(Handle<v8::Object> target) {
  Local<FunctionTemplate> tpl = FunctionTemplate::New(New);

  tpl->InstanceTemplate()->SetInternalFieldCount(1);
  tpl->SetClassName(String::NewSymbol("TreeEntry"));

  NODE_SET_PROTOTYPE_METHOD(tpl, "name", Name);
  NODE_SET_PROTOTYPE_METHOD(tpl, "attributes", Attributes);
  NODE_SET_PROTOTYPE_METHOD(tpl, "id", Id);
  NODE_SET_PROTOTYPE_METHOD(tpl, "toObject", ToObject);

  constructor_template = Persistent<Function>::New(tpl->GetFunction());
  target->Set(String::NewSymbol("TreeEntry"), constructor_template);
}

git_tree_entry* GitTreeEntry::GetValue() {
  return this->entry;
}

void GitTreeEntry::SetValue(git_tree_entry* entry) {
  this->entry = entry;
}

const char* GitTreeEntry::Name() {
  return git_tree_entry_name(this->entry);
}

int GitTreeEntry::Attributes() {
  return git_tree_entry_filemode(this->entry);
}

const git_oid* GitTreeEntry::Id() {
  return git_tree_entry_id(this->entry);
}

int GitTreeEntry::ToObject(git_repository* repo, git_object** obj) {
  return git_tree_entry_to_object(obj, repo, this->entry);
}

Handle<Value> GitTreeEntry::New(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = new GitTreeEntry();

  entry->Wrap(args.This());

  return scope.Close(args.This());
}

Handle<Value> GitTreeEntry::Name(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());

  return scope.Close( String::New(entry->Name()) );
}

Handle<Value> GitTreeEntry::Attributes(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());

  return scope.Close( Number::New(entry->Attributes()) );
}

Handle<Value> GitTreeEntry::Id(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Oid is required and must be an Object.")));
  }

  GitOid* oid = ObjectWrap::Unwrap<GitOid>(args[0]->ToObject());

  oid->SetValue(*const_cast<git_oid *>(entry->Id()));

  return scope.Close( Undefined() );
}

Handle<Value> GitTreeEntry::ToObject(const Arguments& args) {
  HandleScope scope;

  GitTreeEntry *entry = ObjectWrap::Unwrap<GitTreeEntry>(args.This());

  if(args.Length() == 0 || !args[0]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Repo is required and must be an Object.")));
  }

  if(args.Length() == 1 || !args[1]->IsObject()) {
    return ThrowException(Exception::Error(String::New("Object is required and must be an Object.")));
  }

  GitRepo* repo = ObjectWrap::Unwrap<GitRepo>(args[0]->ToObject());
  GitObject* object = ObjectWrap::Unwrap<GitObject>(args[1]->ToObject());

  git_object* out;
  entry->ToObject(repo->GetValue(), &out);

  object->SetValue(out);

  return scope.Close( Undefined() );
}
Persistent<Function> GitTreeEntry::constructor_template;


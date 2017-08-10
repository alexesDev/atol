#include "atol.h"

#include <dto_errors.h>
#include <dto_const.h>

#include <ifptr.h>
#include <dlfcn.h>
#include <vector>
#include <cstdlib>
#include <clocale>
#include <sstream>

PrinterPtr createPrinterPtr(const char* path) {
  void *handle = dlopen(path, RTLD_LAZY);
  char *error;

  if (!handle) {
    Nan::ThrowError("Cannon open so");
    return nullptr;
  }

  auto create = reinterpret_cast<CreateFptrInterfacePtr>(dlsym(handle, "CreateFptrInterface"));

  if ((error = dlerror()) != NULL) {
    Nan::ThrowError("Cannot find CreateFptrInterface in so");
    return nullptr;
  }

  auto release = reinterpret_cast<ReleaseFptrInterfacePtr>(dlsym(handle, "ReleaseFptrInterface"));

  if ((error = dlerror()) != NULL) {
    Nan::ThrowError("Cannot find ReleaseFptrInterface in so");
    return nullptr;
  }

  return PrinterPtr(create(DTO_IFPTR_VER1), [release](TED::Fptr::IFptr* ifptr) { release(&ifptr); });
}

void throwError(TED::IDTOBase *iface) {
  if (!iface) {
    Nan::ThrowError("Invalid interface");
    return;
  }

  auto rc = EC_OK;
  iface->get_ResultCode(&rc);

  if (rc == 0) {
    return;
  }

  std::vector<wchar_t> v(256);
  auto size = iface->get_ResultDescription(&v[0], v.size());

  if (size <= 0) {
    Nan::ThrowError("get_ResultDescription error");
    return;
  }

  auto usize = static_cast<unsigned int>(size);

  if (usize > v.size()) {
    v.clear();
    v.resize(size + 1);
    iface->get_ResultDescription(&v[0], v.size());
  }

  std::setlocale(LC_ALL, "en_US.utf8");
  char description[1024];
  std::wcstombs(description, &v[0], 1024);

  if (rc != EC_INVALID_PARAM) {
    std::stringstream ss;
    ss << "Code: " << rc;
    ss << " " << description;
    Nan::ThrowError(ss.str().c_str());
    return;
  }

  v.clear();
  size = iface->get_BadParamDescription(&v[0], v.size());

  if (size <= 0) {
    Nan::ThrowError("get_BadParamDescription error");
    return;
  }

  usize = static_cast<unsigned int>(size);

  if (usize > v.size()) {
    v.clear();
    v.resize(size + 1);
    iface->get_ResultDescription(&v[0], v.size());
  }

  char paramDescription[1024];
  std::wcstombs(paramDescription, &v[0], 1024);

  std::stringstream ss;
  ss << "Code: " << rc;
  ss << " " << description;
  ss << " - " << paramDescription;

  Nan::ThrowError(ss.str().c_str());
}

Nan::Persistent<v8::Function> Atol::constructor;

#define ATOL_SETTING(key, value) if(printer->put_DeviceSingleSetting((key), (value)) < 0) { throwError(printer.get()); return; }

Atol::Atol(const char *libPath, const char *tty, int model, int protocol) : printer(createPrinterPtr(libPath)) {
  wchar_t wideTTY[128];
  mbstowcs(wideTTY, tty, 128);

  ATOL_SETTING(S_PORT, SV_PORT_TTY)
  ATOL_SETTING(S_DEVICEFILE, wideTTY)
  ATOL_SETTING(S_PROTOCOL, protocol)
  ATOL_SETTING(S_MODEL, model)
  ATOL_SETTING(S_BAUDRATE, 115200)

  if(printer->ApplySingleSettings() < 0) {
    throwError(printer.get());
    return;
  }

  if(printer->put_DeviceEnabled(1) < 0) {
    throwError(printer.get());
    return;
  }

  if(printer->GetStatus() < 0) {
    throwError(printer.get());
    return;
  }
}

Atol::~Atol() {
}

void Atol::New(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  if (!info.IsConstructCall()) {
    Nan::ThrowError("Cannot call constructor as function, you need to use 'new' keyword");
  }

  if (info.Length() != 4) {
    Nan::ThrowError("Wrong number of arguments (expected 4)");
    return;
  }

  if (!info[0]->IsString()) {
    Nan::ThrowError("(1) arguments must be a string");
    return;
  }

  if (!info[1]->IsString()) {
    Nan::ThrowError("(2) arguments must be a string");
    return;
  }

  if (!info[2]->IsNumber()) {
    Nan::ThrowError("(3) arguments must be a number");
    return;
  }

  if (!info[3]->IsNumber()) {
    Nan::ThrowError("(4) arguments must be a number");
    return;
  }

  v8::String::Utf8Value libPath(info[0]->ToString());
  v8::String::Utf8Value tty(info[1]->ToString());

  Atol *obj = new Atol(*libPath, *tty, info[2]->Int32Value(), info[3]->Int32Value());
  obj->Wrap(info.This());
  info.GetReturnValue().Set(info.This());
}

void Atol::Init(v8::Local<v8::Object> exports) {
  Nan::HandleScope scope;

  v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
  tpl->SetClassName(Nan::New("Atol").ToLocalChecked());
  tpl->InstanceTemplate()->SetInternalFieldCount(1);

  Nan::SetPrototypeMethod(tpl, "printText", PrintText);
  Nan::SetPrototypeMethod(tpl, "printFooter", PrintFooter);

  constructor.Reset(tpl->GetFunction());
  exports->Set(Nan::New("Atol").ToLocalChecked(), tpl->GetFunction());

  exports->Set(Nan::New("ChequeSell").ToLocalChecked(), Nan::New<v8::Integer>(TED::Fptr::ChequeSell));

  exports->Set(Nan::New("ProtocolAtol20").ToLocalChecked(), Nan::New<v8::Integer>(TED::Fptr::ProtocolAtol20));
  exports->Set(Nan::New("ProtocolAtol30").ToLocalChecked(), Nan::New<v8::Integer>(TED::Fptr::ProtocolAtol30));

  exports->Set(Nan::New("ModelFPrint22K").ToLocalChecked(), Nan::New<v8::Integer>(TED::Fptr::ModelFPrint22K));
  exports->Set(Nan::New("ModelFPrint22PTK").ToLocalChecked(), Nan::New<v8::Integer>(TED::Fptr::ModelFPrint22NEW));
}

void Atol::PrintText(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  auto obj = ObjectWrap::Unwrap<Atol>(info.Holder());

  if (info.Length() != 1) {
    Nan::ThrowError("Wrong number of arguments (expected 1)");
    return;
  }

  if (!info[0]->IsString()) {
    Nan::ThrowError("First argument must be a string");
    return;
  }

  if(!obj->printer) {
    Nan::ThrowError("Invalid interface");
    return;
  }

  v8::String::Utf8Value textValue(info[0]->ToString());
  std::unique_ptr<wchar_t> buffer(new wchar_t[textValue.length() + 1]);
  mbstowcs(buffer.get(), *textValue, textValue.length());

  if(obj->printer->put_Caption(buffer.get()) < 0) {
    throwError(obj->printer.get());
    return;
  }

  if(obj->printer->PrintString() < 0) {
    throwError(obj->printer.get());
    return;
  }
}

void Atol::PrintFooter(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  auto obj = ObjectWrap::Unwrap<Atol>(info.Holder());

  if(!obj->printer) {
    Nan::ThrowError("Invalid interface");
    return;
  }

  if(obj->printer->put_Mode(TED::Fptr::ModeReportNoClear) < 0) {
    throwError(obj->printer.get());
    return;
  }

  if(obj->printer->SetMode() < 0) {
    throwError(obj->printer.get());
    return;
  }

  if(obj->printer->PrintFooter() < 0) {
    throwError(obj->printer.get());
    return;
  }
}

void Atol::OpenCheck(const Nan::FunctionCallbackInfo<v8::Value>& info) {
  auto obj = ObjectWrap::Unwrap<Atol>(info.Holder());
  auto ifptr = obj->printer.get();

  if (info.Length() != 1) {
    Nan::ThrowError("Wrong number of arguments (expected 1)");
    return;
  }

  if (!info[0]->IsNumber()) {
    Nan::ThrowError("First argument must be a number");
    return;
  }

  if(!obj->printer) {
    Nan::ThrowError("Invalid interface");
    return;
  }

  if(ifptr->put_Mode(TED::Fptr::ModeRegistration) < 0) {
    throwError(ifptr);
    return;
  }

  if(ifptr->SetMode() < 0) {
    throwError(ifptr);
    return;
  }

  auto type = info[0]->Int32Value();

  if(ifptr->put_CheckType(type) < 0) {
    throwError(ifptr);
    return;
  }

  if(ifptr->OpenCheck() < 0) {
    throwError(ifptr);
  }
}

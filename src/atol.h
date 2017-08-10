#ifndef H_ATOL
#define H_ATOL

#include <memory>
#include <functional>
#include <nan.h>

namespace TED {
  namespace Fptr {
    class IFptr;
  }
}

typedef std::function<void(TED::Fptr::IFptr*)> PrinterPtrDeleter;
typedef std::unique_ptr<TED::Fptr::IFptr, PrinterPtrDeleter> PrinterPtr;

class Atol : public Nan::ObjectWrap {
  PrinterPtr printer;

  explicit Atol(const char *libPath, const char *tty);
  ~Atol();

  static void New(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void PrintText(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static void PrintFooter(const Nan::FunctionCallbackInfo<v8::Value>& info);
  static Nan::Persistent<v8::Function> constructor;

  public:
    static void Init(v8::Local<v8::Object> exports);
};

#endif

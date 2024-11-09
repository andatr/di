#ifndef YAGA_DI_ARGS
#define YAGA_DI_ARGS

#include <unordered_map>
#include <typeinfo>
#include <typeindex>

#include "di/type_traits.h"

namespace yaga {
namespace di {

class Args;

// -----------------------------------------------------------------------------------------------------------------------------
class ArgsIter
{
friend class Args;
public:
  inline ArgsIter();
  inline explicit operator bool() const;

private:
  Args* args_;
  std::unordered_map<std::type_index, void*>::iterator iter_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class Args
{
friend class ArgsIter;
public:
  template <typename... Params>
  Args(Params&&...params);

  template <typename T>
  ArgsIter find();

  template <typename T>
  T& get(ArgsIter iter);

private:
  std::unordered_map<std::type_index, void*> args_;
};

// -----------------------------------------------------------------------------------------------------------------------------
ArgsIter::ArgsIter() :
  args_(nullptr)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
ArgsIter::operator bool() const
{
  return args_ != nullptr && iter_ != args_->args_.end();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename... Params>
Args::Args(Params&&...params)
{
  (..., (args_[typeid(RemoveCVRef<Params>)] = static_cast<void*>(std::addressof(params))));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
ArgsIter Args::find()
{
  ArgsIter iter;
  iter.args_ = this;
  iter.iter_ = args_.find(typeid(RemoveCVRef<T>));
  return iter;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T& Args::get(ArgsIter iter)
{
  return (T&)(*reinterpret_cast<RemoveCVRef<T>*>(iter.iter_->second));
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_ARGS
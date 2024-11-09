#ifndef YAGA_DI_SHARED_FUNCTOR_FACTORY
#define YAGA_DI_SHARED_FUNCTOR_FACTORY

#include <memory>

#include "di/shared_factory.h"

namespace yaga {
namespace di {

template <typename I, typename T, typename F>
class SharedFunctorFactory : public SharedFactory<I, T>
{
public:
  explicit SharedFunctorFactory(F functor);
  
protected:
  virtual T* createInstance(Container* container, Args* args);

private:
  F functor_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename F>
SharedFunctorFactory<I, T, F>::SharedFunctorFactory(F functor) :
  functor_(functor)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename F>
T* SharedFunctorFactory<I, T, F>::createInstance(Container* container, Args*)
{
  return FunctorInvoker::invoke<F>(functor_, container);
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_SHARED_FUNCTOR_FACTORY
#ifndef YAGA_DI_SHARED_IMPL_FUNCTOR_FACTORY
#define YAGA_DI_SHARED_IMPL_FUNCTOR_FACTORY

#include <memory>

#include "di/shared_impl_factory.h"

namespace yaga {
namespace di {

template <typename I, typename T, typename F>
class SharedImlpFunctorFactory : public SharedImlpFactory<I, T>
{
public:
  explicit SharedImlpFunctorFactory(FactoryContext* context, F functor);

protected:
  T* createInstance(Container* container, Args* args) override;

protected:
  F functor_;
};
// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename F>
SharedImlpFunctorFactory<I, T, F>::SharedImlpFunctorFactory(FactoryContext* context, F functor) :
  SharedImlpFactory<I, T>(context),
  functor_(functor)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename F>
T* SharedImlpFunctorFactory<I, T, F>::createInstance(Container* container, Args*)
{
  return FunctorInvoker::invoke<F>(functor_, container);
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_SHARED_IMPL_FUNCTOR_FACTORY
#ifndef YAGA_DI_UNIQUE_FUNCTOR_FACTORY
#define YAGA_DI_UNIQUE_FUNCTOR_FACTORY

#include <memory>

#include "di/unique_factory.h"

namespace yaga {
namespace di {

template <typename I, typename T, typename F>
class UniqueFunctorFactory : public UniqueFactory<I, T>
{
public:
  explicit UniqueFunctorFactory(F functor);

protected:  
  T* createInstance(Container* container, Args* args) override;

private:
  F functor_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename F>
UniqueFunctorFactory<I, T, F>::UniqueFunctorFactory(F functor) :
  functor_(functor)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename F>
T* UniqueFunctorFactory<I, T, F>::createInstance(Container* container, Args*)
{
  return FunctorInvoker::invoke<F>(functor_, container);
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_UNIQUE_FUNCTOR_FACTORY
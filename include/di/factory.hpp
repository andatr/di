#ifndef YAGA_DI_FACTORY_HPP
#define YAGA_DI_FACTORY_HPP

#include <memory>
#include <stdexcept>

#include "di/factory.h"
#include "di/factory_context.h"
#include "di/object_factory.h"
#include "di/shared_factory.h"
#include "di/shared_functor_factory.h"
#include "di/shared_impl_factory.h"
#include "di/shared_impl_functor_factory.h"
#include "di/unique_factory.h"
#include "di/unique_functor_factory.h"

namespace yaga {
namespace di {

// -----------------------------------------------------------------------------------------------------------------------------
template <typename S, typename I, typename T>
EnableIf<IsSame<S, UniqueScope>, FactorySPtr> createFactory(bool callInit, FactoryContext*)
{
  return std::make_shared<UniqueFactory<I, T>>(callInit);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename S, typename I, typename T>
EnableIf<IsSame<S, SharedScope>, FactorySPtr> createFactory(bool callInit, FactoryContext*)
{
  return std::make_shared<SharedFactory<I, T>>(callInit);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename S, typename I, typename T>
EnableIf<IsSame<S, SharedImlpScope>, FactorySPtr> createFactory(bool callInit, FactoryContext* context)
{
  return std::make_shared<SharedImlpFactory<I, T>>(context, callInit);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename S, typename I, typename T>
EnableIf<IsSame<S, SharedScope>, FactorySPtr> createFactory(std::shared_ptr<T> instance, FactoryContext*)
{
  return std::make_shared<SharedFactory<I, T>>(false, instance);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename S, typename I, typename T>
EnableIf<IsSame<S, SharedImlpScope>, FactorySPtr> createFactory(
  std::shared_ptr<T> instance,
  FactoryContext* context)
{
  return std::make_shared<SharedImlpFactory<I, T>>(context, false, instance);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename S, typename I, typename T, typename F>
EnableIf<IsSame<S, UniqueScope>, FactorySPtr> createFunctorFactory(F functor, FactoryContext*)
{
  return std::make_shared<UniqueFunctorFactory<I, T, F>>(functor);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename S, typename I, typename T, typename F>
EnableIf<IsSame<S, SharedScope>, FactorySPtr> createFunctorFactory(F functor, FactoryContext*)
{
  return std::make_shared<SharedFunctorFactory<I, T, F>>(functor);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename S, typename I, typename T, typename F>
EnableIf<IsSame<S, SharedImlpScope>, FactorySPtr> createFunctorFactory(F functor, FactoryContext* context)
{
  return std::make_shared<SharedImlpFunctorFactory<I, T, F>>(context, functor);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
EnableIf<IsPurePtr<T>, T> Factory::createObject(Container* container, Args* args)
{
  return static_cast<T>(createPure(container, args));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
EnableIf<IsSharedPtr<T>, T> Factory::createObject(Container* container, Args* args)
{
  using E = typename std::pointer_traits<T>::element_type;
  return std::static_pointer_cast<E>(createShared(container, args));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
EnableIf<IsIniquePtr<T>, T> Factory::createObject(Container* container, Args* args)
{
  using E = typename std::pointer_traits<T>::element_type;
  return std::unique_ptr<E>(static_cast<E*>(createUnique(container, args)));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
EnableIf<IsReference<T>, RemoveCVRef<T>&> Factory::createObject(Container* container, Args* args)
{
  auto ptr = static_cast<RemoveCVRef<T>*>(createReference(container, args));
  return *ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
EnableIf<
  !IsPointer<T> &&
  !IsReference<T>
, T> Factory::createObject(Container* container, Args* args)
{
  if (allowInstanceCreation()) {
    return ObjectFactory::create<T>(container, args, callInit_);
  }
  throw std::runtime_error(std::string("Class ") + typeid(T).name() + " instantiation is not allowed by scope");
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_FACTORY_HPP
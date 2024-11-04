#ifndef YAGA_DI_FACTORY_HPP
#define YAGA_DI_FACTORY_HPP

#include <memory>
#include <stdexcept>

#include "di/factory.h"
#include "di/factory_context.h"
#include "di/object_factory.h"
#include "di/shared_factory.h"
#include "di/shared_impl_factory.h"
#include "di/unique_factory.h"

namespace yaga {
namespace di {

// -----------------------------------------------------------------------------------------------------------------------------
template <typename P, typename I, typename T>
std::enable_if_t<std::is_same_v<P, UniquePolicy>, FactorySPtr> createFactory(bool callInit, FactoryContext*)
{
  return std::make_shared<UniqueFactory<I, T>>(callInit);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename P, typename I, typename T>
std::enable_if_t<std::is_same_v<P, SharedPolicy>, FactorySPtr> createFactory(bool callInit, FactoryContext*)
{
  return std::make_shared<SharedFactory<I, T>>(callInit);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename P, typename I, typename T>
std::enable_if_t<std::is_same_v<P, SharedImlpPolicy>, FactorySPtr> createFactory(bool callInit, FactoryContext* context)
{
  return std::make_shared<SharedImlpFactory<I, T>>(callInit, context);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename P, typename I, typename T>
FactorySPtr createFactory(std::shared_ptr<T> instance, FactoryContext*)
{
  return std::make_shared<SharedFactory<I, T>>(false, instance);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<is_pure_ptr<T>, T> Factory::createObject(Container* container, Args* args)
{
  return static_cast<T>(createPure(container, args));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<is_shared_ptr<T>, T> Factory::createObject(Container* container, Args* args)
{
  using E = typename std::pointer_traits<T>::element_type;
  return std::static_pointer_cast<E>(createShared(container, args));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<is_unique_ptr<T>, T> Factory::createObject(Container* container, Args* args)
{
  using E = typename std::pointer_traits<T>::element_type;
  return std::unique_ptr<E>(static_cast<E*>(createUnique(container, args)));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<std::is_reference_v<T>, std::remove_cvref_t<T>&> Factory::createObject(Container* container, Args* args)
{
  auto ptr = static_cast<std::remove_cvref_t<T>*>(createReference(container, args));
  return *ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<
  !is_pointer<T> &&
  !std::is_reference_v<T>
, T> Factory::createObject(Container* container, Args* args)
{
  if (allowInstanceCreation()) {
    return ObjectFactory::create<T>(container, args, callInit_);
  }
  throw std::runtime_error(std::string("Class ") + typeid(T).name() + " instantiation is not allowed by policy");
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_FACTORY_HPP
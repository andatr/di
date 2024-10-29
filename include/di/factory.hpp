#ifndef YAGA_DI_FACTORY_HPP
#define YAGA_DI_FACTORY_HPP

#include <memory>

#include "di/factory.h"
#include "di/factory_context.h"
#include "di/policy.h"
#include "di/single_factory.h"
#include "di/single_impl_factory.h"
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
std::enable_if_t<std::is_same_v<P, SinglePolicy>, FactorySPtr> createFactory(bool callInit, FactoryContext*)
{
  return std::make_shared<SingleFactory<I, T>>(callInit);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename P, typename I, typename T>
std::enable_if_t<std::is_same_v<P, SingleImlpPolicy>, FactorySPtr> createFactory(bool callInit, FactoryContext* context)
{
  return std::make_shared<SingleImlpFactory<I, T>>(callInit, context);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename P, typename I, typename T>
FactorySPtr createFactory(std::shared_ptr<T> instance, bool callInit, FactoryContext*)
{
  return std::make_shared<SingleFactory<I, T>>(callInit, instance);
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_FACTORY_HPP
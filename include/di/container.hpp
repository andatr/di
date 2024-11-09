#ifndef YAGA_DI_CONTAINER_HPP
#define YAGA_DI_CONTAINER_HPP

#include <iostream>
#include <stdexcept>

#include "di/container.h"
#include "di/factory.hpp"

#define THROW_NOT_REGISTERED throw std::runtime_error(std::string("Class ") + typeid(T).name() + " not registered");

namespace yaga {
namespace di {

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
struct LambdaHelper;

template<typename Ret, typename... Params>
struct LambdaHelper<std::function<Ret(Params...)>>
{
  template <typename Container>
  static std::function<Ret(Params...)> createLambda(Container* container)
  {
    return [container](Params&&... params) {
      Args args(std::forward<Params>(params)...);
      std::lock_guard<std::mutex> lock(container->factoryMutex_);
      return container->template createImpl<Ret>(&args);
    };
  }
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename S, bool CallInit>
EnableIf<IsBaseOf<I, T, S>, Container&> Container::add()
{
  using Interface = RemoveCVRef<I>;
  std::lock_guard<std::mutex> lock(factoryMutex_);
  throwIfExists<Interface>();
  factories_[typeid(Interface)] = createFactory<S, I, T>(CallInit, &factoryContext_);
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename S, bool CallInit>
EnableIf<IsBaseOf<T, T, S>, Container&> Container::add()
{
  return add<T, T, S, CallInit>();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename S>
EnableIf<IsBaseOf<I, T, S>, Container&> Container::add(std::shared_ptr<T> instance)
{
  using Interface = RemoveCVRef<I>;
  std::lock_guard<std::mutex> lock(factoryMutex_);
  throwIfExists<Interface>();
  factories_[typeid(Interface)] = createFactory<S, I, T>(instance, &factoryContext_);
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename S>
EnableIf<IsBaseOf<T, T, S>, Container&> Container::add(std::shared_ptr<T> instance)
{
  return add<T, T, S>(instance);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename S, typename F>
EnableIf<IsBaseOf<
  I,
  typename PointerTraits<typename FunctionTraits<F>::ReturnType>::ElementType,
  S>,
Container&> Container::addFactory(F functor)
{
  using ReturnType = typename FunctionTraits<F>::ReturnType;
  using T = typename PointerTraits<ReturnType>::ElementType;
  throwIfExists<I>();
  factories_[typeid(I)] = createFunctorFactory<S, I, T, F>(functor, &factoryContext_);
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename S, typename F>
Container& Container::addFactory(F functor)
{
  using ReturnType = typename FunctionTraits<F>::ReturnType;
  using T = typename PointerTraits<ReturnType>::ElementType;
  return addFactory<T, S, F>(functor);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename S, bool CallInit>
EnableIf<IsBaseOf<I, T, S>, Container&> Container::addMulti()
{
  using Interface = RemoveCVRef<I>;
  std::lock_guard<std::mutex> lock(factoryMutex_);
  multiFactories_.emplace(typeid(Interface), createFactory<S, I, T>(CallInit, &factoryContext_));
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename S, bool CallInit>
EnableIf<IsBaseOf<T, T, S>, Container&> Container::addMulti()
{
  return addMulti<T, T, S, CallInit>();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Container::throwIfExists()
{
  auto it = factories_.find(typeid(T));
  if (it != factories_.end()) throw std::runtime_error(std::string("Class ") + typeid(T).name() + " already registered");
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Container::create()
{
  std::lock_guard<std::mutex> lock(factoryMutex_);
  return createImpl<T>(nullptr);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T Container::createImpl(Args* args)
{
  if (auto it = args ? args->find<T>() : ArgsIter()) {
    return std::forward<T>(args->get<T>(it));
  }
  if (auto factory = findFactory<T>(false)) {
    return factory->template createObject<RemoveCV<T>>(this, args);
  }
  return createSpecial<T>(args);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
EnableIf<IsPointer<T>, T> Container::createSpecial(Args* args)
{
  using E = typename PointerTraits<T>::ElementType;
  auto factory = findFactory<E>();
  return factory->template createObject<RemoveCV<T>>(this, args);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
EnableIf<IsFunction<T>, T> Container::createSpecial(Args*)
{
  return LambdaHelper<RemoveCVRef<T>>::createLambda(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
EnableIf<IsVector<T>, T> Container::createSpecial(Args* args)
{
  using Vector = RemoveCV<T>;
  using VectorElement = typename VectorTraits<Vector>::ElementType;
  using Element = typename PointerTraits<VectorElement>::ElementType;
  Vector result {};
  auto range = multiFactories_.equal_range(typeid(Element));
  for (decltype(range.first) it = range.first; it != range.second; ++it) {
    result.push_back(it->second->template createObject<VectorElement>(this, args));
  }
  if (result.empty()) THROW_NOT_REGISTERED;
  return result;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
EnableIf<
  !IsVector<T> &&
  !IsFunction<T> &&
  !IsPointer<T>
, T> Container::createSpecial(Args*)
{
  THROW_NOT_REGISTERED;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
Factory* Container::findFactory(bool throwEx)
{
  auto it = factories_.find(typeid(RemoveCVRef<T>));
  if (it == factories_.end()) {
    if (throwEx) THROW_NOT_REGISTERED;
    return nullptr;
  }
  return it->second.get();
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_CONTAINER_HPP
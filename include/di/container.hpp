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
template <typename I, typename T, typename P, bool CallInit>
std::enable_if_t<di::is_base_of<I, T, P>, Container&> Container::add()
{
  using Interface = std::remove_cvref_t<I>;
  std::lock_guard<std::mutex> lock(factoryMutex_);
  throwIfExists<Interface>();
  factories_[typeid(Interface)] = createFactory<P, I, T>(CallInit, &factoryContext_);
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename P, bool CallInit>
std::enable_if_t<di::is_base_of<T, T, P>, Container&> Container::add()
{
  return add<T, T, P, CallInit>();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::enable_if_t<std::is_base_of_v<I, T>, Container&> Container::add(std::shared_ptr<T> instance)
{
  using Interface = std::remove_cvref_t<I>;
  std::lock_guard<std::mutex> lock(factoryMutex_);
  throwIfExists<Interface>();
  factories_[typeid(Interface)] = createFactory<SharedPolicy, I, T>(instance, &factoryContext_);
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
Container& Container::add(const std::shared_ptr<T> instance)
{
  return add<T, T>(instance);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename P, bool CallInit>
std::enable_if_t<di::is_base_of<I, T, P>, Container&> Container::addMulti()
{
  using Interface = std::remove_cvref_t<I>;
  std::lock_guard<std::mutex> lock(factoryMutex_);
  multiFactories_.emplace(typeid(Interface), createFactory<P, I, T>(CallInit, &factoryContext_));
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename P, bool CallInit>
std::enable_if_t<di::is_base_of<T, T, P>, Container&> Container::addMulti()
{
  return addMulti<T, T, P, CallInit>();
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
    return factory->template createObject<std::remove_cv_t<T>>(this, args);
  }
  return createSpecial<T>(args);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<is_pointer<T>, T> Container::createSpecial(Args* args)
{
  using E = typename pointer_traits<T>::element_type;
  auto factory = findFactory<E>();
  return factory->template createObject<std::remove_cv_t<T>>(this, args);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<is_function_v<T>, T> Container::createSpecial(Args*)
{
  return LambdaHelper<std::remove_cvref_t<T>>::createLambda(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<is_vector<T>, T> Container::createSpecial(Args* args)
{
  using Vector = std::remove_cv_t<T>;
  using Element = typename vector_traits<Vector>::element_type;
  Vector result {};
  createSpecialMulti<Vector, Element>(result, args);
  if (result.empty()) THROW_NOT_REGISTERED;
  return result;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Vector, typename T>
std::enable_if_t<is_pointer<T>, Container&> Container::createSpecialMulti(Vector& vector, Args* args)
{
  using Element = std::remove_cvref_t<typename std::pointer_traits<T>::element_type>;
  auto range = multiFactories_.equal_range(typeid(Element));
  for (decltype(range.first) it = range.first; it != range.second; ++it) {
    vector.push_back(it->second->template createObject<T>(this, args));
  }
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename Vector, typename T>
std::enable_if_t<!is_pointer<T>, Container&> Container::createSpecialMulti(Vector& vector, Args* args)
{
  using Element = std::remove_cvref_t<T>;
  auto range = multiFactories_.equal_range(typeid(Element));
  for (decltype(range.first) it = range.first; it != range.second; ++it) {
    vector.push_back(it->second->template createObject<T>(this, args));
  }
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<
  !is_vector<T>     &&
  !is_function_v<T> &&
  !is_pointer<T>
, T> Container::createSpecial(Args*)
{
  THROW_NOT_REGISTERED;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
Factory* Container::findFactory(bool throwEx)
{
  auto it = factories_.find(typeid(std::remove_cvref_t<T>));
  if (it == factories_.end()) {
    if (throwEx) THROW_NOT_REGISTERED;
    return nullptr;
  }
  return it->second.get();
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_CONTAINER_HPP
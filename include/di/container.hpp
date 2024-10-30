#ifndef YAGA_DI_CONTAINER_HPP
#define YAGA_DI_CONTAINER_HPP

#include <stdexcept>

#include "di/container.h"
#include "di/factory.hpp"
#include "di/exception.h"

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
      return container->createImpl<Ret>(&args);
    };
  }
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename P, bool CallInit>
std::enable_if_t<di::is_base_of<I, T, P>, Container&> Container::add()
{
  std::lock_guard<std::mutex> lock(factoryMutex_);
  throwIfExists<I>();
  factories_[typeid(std::decay_t<I>)] = createFactory<P, I, T>(CallInit, &factoryContext_);
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename P, bool CallInit>
std::enable_if_t<di::is_base_of<I, T, P>, Container&> Container::addMulti()
{
  std::lock_guard<std::mutex> lock(factoryMutex_);
  using D = std::decay_t<I>;
  multiFactories_.emplace(typeid(D), createFactory<P, I, T>(CallInit, &factoryContext_));
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename P, bool CallInit>
std::enable_if_t<di::is_base_of<T, T, P>, Container&> Container::add()
{
  return add<T, T, P, CallInit>();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename P, bool CallInit>
std::enable_if_t<di::is_base_of<T, T, P>, Container&> Container::addMulti()
{
  return addMulti<T, T, P, CallInit>();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename P, bool CallInit>
std::enable_if_t<di::is_base_of<I, T, P>, Container&> Container::add(std::shared_ptr<T> instance)
{
  std::lock_guard<std::mutex> lock(factoryMutex_);
  throwIfExists<I>();
  factories_[typeid(std::decay_t<I>)] = createFactory<P, I, T>(instance, CallInit, &factoryContext_);
  return *this;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
void Container::throwIfExists()
{
  using D = std::decay_t<T>;
  auto it = factories_.find(typeid(D));
  if (it != factories_.end()) throw std::runtime_error(std::string("Class ") + typeid(T).name() + " already registered");
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename P, bool CallInit>
std::enable_if_t<std::is_base_of_v<Policy, P>, Container&> Container::add(const std::shared_ptr<T> instance)
{
  return add<T, T>(instance, CallInit);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::decay_t<T> Container::create()
{
  std::lock_guard<std::mutex> lock(factoryMutex_);
  return createImpl<T>(nullptr);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<!is_vector<T> && is_pointer<T>, T> Container::createImpl(Args* args)
{
  if (auto it = args ? args->find<T>() : ArgsIter()) {
    return std::move(args->get<T>(it));
  }
  using E = pointer_traits<T>::element_type;
  auto factory = findFactory<E>();
  return createObject<T>(factory, args);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<!is_vector<T> && std::is_reference_v<T>, const std::remove_reference_t<T>> Container::createImpl(Args* args)
{
  if (auto it = args ? args->find<T>() : ArgsIter()) {
    return std::move(args->get<T>(it));
  }
  using D = std::decay_t<T>;
  auto factory = findFactory<D>();
  return createObject<D>(factory, args);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<!is_vector<T> && !is_function_v<T> && !is_pointer_or_ref<T>, T> Container::createImpl(Args* args)
{
  if (auto it = args ? args->find<T>() : ArgsIter()) {
    return std::move(args->get<T>(it));
  }
  auto factory = findFactory<T>();
  return createObject<T>(factory, args);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<!is_vector<T> && is_function_v<T>, T> Container::createImpl(Args* args)
{
  if (auto it = args ? args->find<T>() : ArgsIter()) {
    return std::move(args->get<T>(it));
  }
  if (auto factory = findFactory<T>(false)) {
    return createObject<T>(factory, args);
  }
  return LambdaHelper<T>::createLambda(this);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<is_vector<T>, T> Container::createImpl(Args* args)
{
  using D = std::decay_t<T>;
  using P = vector_traits<D>::element_type;
  using E = std::pointer_traits<P>::element_type;
  auto range = multiFactories_.equal_range(typeid(E));
  if (range.first == range.second) {
    range = multiFactories_.equal_range(typeid(std::decay_t<E>));
  }
  if (range.first == range.second) throw std::runtime_error(std::string("Class ") + typeid(T).name() + " not registered");
  D result {};
  for (decltype(range.first) it = range.first; it != range.second; ++it) {
    result.push_back(createObject<P>(it->second.get(), args));
  }
  return result;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
Factory* Container::findFactory(bool throwEx)
{
  auto it = factories_.find(typeid(T));
  if (it == factories_.end()) {
    it = factories_.find(typeid(std::decay_t<T>));
  }
  if (it == factories_.end()) {
    if (throwEx) throw std::runtime_error(std::string("Class ") + typeid(T).name() + " not registered");
    return nullptr;
  }
  return it->second.get();
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<std::is_pointer_v<T>, T> Container::createObject(Factory* factory, Args* args) 
{
  return static_cast<T>(factory->createPure(this, args));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<is_shared_ptr<T>, T> Container::createObject(Factory* factory, Args* args)
{
  using E = std::pointer_traits<T>::element_type;
  return std::static_pointer_cast<E>(factory->createShared(this, args));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<is_unique_ptr<T>, T> Container::createObject(Factory* factory, Args* args)
{
  using E = std::pointer_traits<T>::element_type;
  using D = std::decay_t<E>;
  return std::unique_ptr<E>(static_cast<D*>(factory->createUnique(this, args)));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::enable_if_t<!is_pointer_or_ref<T>, T> Container::createObject(Factory* factory, Args* args)
{
  return ObjectFactory::create<T>(this, args, factory->initObject());
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_CONTAINER_HPP
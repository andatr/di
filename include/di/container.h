#ifndef YAGA_DI_CONTAINER_H
#define YAGA_DI_CONTAINER_H

#include <mutex>
#include <typeindex>
#include <unordered_map>

#include "di/factory.h"
#include "di/traits.h"
#include "di/factory_context.h"

namespace yaga {
namespace di {

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T, typename P>
constexpr bool is_base_of = 
  std::is_base_of_v<I, T> &&
  std::is_base_of_v<Policy, P>;

// -----------------------------------------------------------------------------------------------------------------------------
class Container
{
template <typename T, int N> friend struct CtorArg;
template<typename T> friend struct LambdaHelper;

public:
  template <typename I, typename T, typename P = UniquePolicy, bool CallInit = true>
  std::enable_if_t<di::is_base_of<I, T, P>, Container&> add();

  template <typename I, typename T, typename P = SinglePolicy, bool CallInit = true>
  std::enable_if_t<di::is_base_of<I, T, P>, Container&> add(std::shared_ptr<T> instance);

  template <typename T, typename P = UniquePolicy, bool CallInit = true>
  std::enable_if_t<di::is_base_of<T, T, P>, Container&> add();

  template <typename T, typename P = SinglePolicy, bool CallInit = true>
  std::enable_if_t<std::is_base_of_v<Policy, P>, Container&> add(const std::shared_ptr<T> instance);

  template <typename I, typename T, typename P = UniquePolicy, bool CallInit = true>
  std::enable_if_t<di::is_base_of<I, T, P>, Container&> addMulti();

  template <typename T, typename P = UniquePolicy, bool CallInit = true>
  std::enable_if_t<di::is_base_of<T, T, P>, Container&> addMulti();

  template <typename T>
  std::decay_t<T> create();

  template <typename T>
  T* createPtr() { return create<T*>(); }

  template <typename T>
  std::shared_ptr<T> createShared() { return create<std::shared_ptr<T>>(); }

  template <typename T>
  std::unique_ptr<T> createUnique() { return create<std::unique_ptr<T>>(); }

private:
  template <typename T>
  std::enable_if_t<!is_vector<T> && is_pointer<T>, T> createImpl(Args* args = nullptr);

  template <typename T>
  std::enable_if_t<!is_vector<T> && std::is_reference_v<T>, const std::remove_reference_t<T>> createImpl(Args* args = nullptr);

  template <typename T>
  std::enable_if_t<!is_vector<T> && !is_function_v<T> && !is_pointer_or_ref<T>, T> createImpl(Args* args = nullptr);

  template <typename T>
  std::enable_if_t<!is_vector<T> && is_function_v<T>, T> createImpl(Args* args = nullptr);

  template <typename T>
  std::enable_if_t<is_vector<T>, T> createImpl(Args* args = nullptr);

  template <typename T>
  void throwIfExists();

  template <typename T>
  Factory* findFactory(bool throwEx = true);

  template <typename T>
  std::enable_if_t<std::is_pointer_v<T>, T> createObject(Factory* factory, Args* args);

  template <typename T>
  std::enable_if_t<is_shared_ptr<T>, T> createObject(Factory* factory, Args* args);

  template <typename T>
  std::enable_if_t<is_unique_ptr<T>, T> createObject(Factory*factory, Args* args);

  template <typename T>
  std::enable_if_t<!is_pointer_or_ref<T>, T> createObject(Factory* factory, Args* args);

private:
  std::mutex factoryMutex_;
  FactoryContext factoryContext_;
  std::unordered_map<std::type_index, FactorySPtr> factories_;
  std::unordered_multimap<std::type_index, FactorySPtr> multiFactories_;
};

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_CONTAINER_H
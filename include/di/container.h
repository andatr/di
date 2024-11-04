#ifndef YAGA_DI_CONTAINER_H
#define YAGA_DI_CONTAINER_H

#include <mutex>
#include <typeindex>
#include <unordered_map>

#include "di/factory.h"
#include "di/type_traits.h"
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
template <typename T> friend struct LambdaHelper;

public:
  /*
   * @brief Registers the class `T` in the container, associating it with the interface `I` and using the policy `P`.
   *
   * @tparam I The interface type under which the class `T` is registered.
   * @tparam T The class type being registered, which must be derived from `I`.
   * @tparam P The policy type for object registration, defaulting to `UniquePolicy`.
   *           Possible values: UniquePolicy, SharedPolicy, SharedImlpPolicy.
   * @tparam CallInit A boolean flag indicating whether to call the `init` method of `T` during instantiation, defaulting to true.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename I, typename T, typename P = UniquePolicy, bool CallInit = true>
  std::enable_if_t<di::is_base_of<I, T, P>, Container&> add();

  /*
   * @brief Registers the class `T` in the container using the policy `P`.
   *
   * @tparam T The class type being registered.
   * @tparam P The policy type for object registration, defaulting to `UniquePolicy`.
   *           Possible values: UniquePolicy, SharedPolicy, SharedImlpPolicy.
   * @tparam CallInit A boolean flag indicating whether to call the `init` method of `T` during instantiation, defaulting to true.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename T, typename P = UniquePolicy, bool CallInit = true>
  std::enable_if_t<di::is_base_of<T, T, P>, Container&> add();

  /*
   * @brief Registers a provided instance of the class `T` in the container, associating it with the interface `I`.
   * 
   * Unstance is registered under the `SharedPolicy`.
   *
   * @tparam I The interface type under which the instance is registered.
   * @tparam T The class type of the provided instance, which must be derived from `I`.
   * @param instance A `std::shared_ptr` to the instance of `T` that will be registered.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename I, typename T>
  std::enable_if_t<std::is_base_of_v<I, T>, Container&> add(std::shared_ptr<T> instance);

  /*
   * @brief Registers a provided instance of the class `T` in the container.
   *
   * Unstance is registered under the `SharedPolicy`.
   * 
   * @tparam T The class type of the provided instance.
   * @param instance A `std::shared_ptr` to the instance of `T` that will be registered.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename T>
  Container& add(const std::shared_ptr<T> instance);

  /*
   * @brief Registers the class `T` in the container, associating it with the interface `I` and using the policy `P`.
   *
   * Unlike single registration methods, `addMulti` supports multiple classes being registered under the same interface `I`.
   * When using a constructor parameter of type `std::vector<I>`, all registered classes will be instantiated.
   *
   * @tparam I The interface type under which the class `T` is registered.
   * @tparam T The class type being registered, which must be derived from `I`.
   * @tparam P The policy type for object registration, defaulting to `UniquePolicy`.
   *           Possible values: UniquePolicy, SharedPolicy, SharedImlpPolicy.
   * @tparam CallInit A boolean flag indicating whether to call the `init` method of `T` during instantiation, defaulting to true.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename I, typename T, typename P = UniquePolicy, bool CallInit = true>
  std::enable_if_t<di::is_base_of<I, T, P>, Container&> addMulti();

  /*
   * @brief Registers the class `T` in the container using the policy `P`.
   *
   * Unlike single registration methods, `addMulti` supports multiple classes being registered under the same interface `T`.
   * When using a constructor parameter of type `std::vector<T>`, all registered classes will be instantiated.
   *
   * @tparam T The class type being registered.
   * @tparam P The policy type for object registration, defaulting to `UniquePolicy`.
   *           Possible values: UniquePolicy, SharedPolicy, SharedImlpPolicy.
   * @tparam CallInit A boolean flag indicating whether to call the `init` method of `T` during instantiation, defaulting to true.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename T, typename P = UniquePolicy, bool CallInit = true>
  std::enable_if_t<di::is_base_of<T, T, P>, Container&> addMulti();

  /*
   * @brief Creates an instance of the class `T` from the container, resolving dependencies.
   *
   * All constructor arguments required by `T` will also be created from the container recursively.
   *
   * @tparam T The class type to be created.
   * @return T An instance of the class `T`.
   */
  template <typename T>
  T create();

  /*
   * @brief Creates a pointer to and instance of the class `T` from the container, resolving dependencies.
   *
   * All constructor arguments required by `T` will also be created from the container recursively.
   *
   * @tparam T The class type for which a pointer will be created.
   * @return T* A pointer to an instance of the class `T`.
   */
  template <typename T>
  T* createPtr() { return create<T*>(); }

  /*
   * @brief Creates a shared pointer to and instance of the class `T` from the container, resolving dependencies.
   *
   * All constructor arguments required by `T` will also be created from the container recursively.
   *
   * @tparam T The class type for which a shared pointer will be created.
   * @return std::shared_ptr<T> A shared pointer to an instance of the class `T`.
   */
  template <typename T>
  std::shared_ptr<T> createShared() { return create<std::shared_ptr<T>>(); }

  /*
   * @brief Creates a unique pointer to and instance of the class `T` from the container, resolving dependencies.
   *
   * All constructor arguments required by `T` will also be created from the container recursively.
   *
   * @tparam T The class type for which a unique pointer will be created.
   * @return std::unique_ptr<T> A unique pointer to an instance of the class `T`.
   */
  template <typename T>
  std::unique_ptr<T> createUnique() { return create<std::unique_ptr<T>>(); }

private:
  template <typename T>
  T createImpl(Args* args);

  template <typename T>
  std::enable_if_t<is_pointer<T>, T> createSpecial(Args* args);

  template <typename T>
  std::enable_if_t<is_function_v<T>, T> createSpecial(Args* args);

  template <typename T>
  std::enable_if_t<is_vector<T>, T> createSpecial(Args* args);

  template <typename T>
  std::enable_if_t<
    !is_vector<T>     &&
    !is_function_v<T> &&
    !is_pointer<T>
  , T> createSpecial(Args* args);

  template <typename Vector, typename T>
  std::enable_if_t<is_pointer<T>, Container&> createSpecialMulti(Vector& vector, Args* args);

  template <typename Vector, typename T>
  std::enable_if_t<!is_pointer<T>, Container&> createSpecialMulti(Vector& vector, Args* args);

  template <typename T>
  void throwIfExists();

  template <typename T>
  Factory* findFactory(bool throwEx = true);

private:
  std::mutex factoryMutex_;
  FactoryContext factoryContext_;
  std::unordered_map<std::type_index, FactorySPtr> factories_;
  std::unordered_multimap<std::type_index, FactorySPtr> multiFactories_;
};

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_CONTAINER_H
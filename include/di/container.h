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
template <typename I, typename T, typename S>
constexpr bool IsBaseOf =
  (std::is_same_v<I, T> || std::is_base_of_v<I, T>) &&
  std::is_base_of_v<Scope, S>;

// -----------------------------------------------------------------------------------------------------------------------------
class Container
{
template <typename T, int N> friend struct CtorArg;
template <int N> friend struct FunctorArg;
template <typename T> friend struct LambdaHelper;

public:
  /*
   * @brief Registers the class `T` in the container, associating it with the interface `I` and using the scope `S`.
   *
   * @tparam I The interface type under which the class `T` is registered.
   * @tparam T The class type being registered, which must be derived from `I`.
   * @tparam S The scope type for object registration, defaulting to `UniqueScope`.
   *           Possible values: UniqueScope, SharedScope, SharedImlpScope.
   * @tparam CallInit A boolean flag indicating whether to call the `init` method of `T` during instantiation, defaulting to false.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename I, typename T, typename S = UniqueScope, bool CallInit = false>
  EnableIf<IsBaseOf<I, T, S>, Container&> add();

  /*
   * @brief Registers the class `T` in the container using the scope `S`.
   *
   * @tparam T The class type being registered.
   * @tparam S The scope type for object registration, defaulting to `UniqueScope`.
   *           Possible values: UniqueScope, SharedScope, SharedImlpScope.
   * @tparam CallInit A boolean flag indicating whether to call the `init` method of `T` during instantiation, defaulting to false.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename T, typename S = UniqueScope, bool CallInit = false>
  EnableIf<IsBaseOf<T, T, S>, Container&> add();

  /*
   * @brief Registers a provided instance of the class `T` in the container, associating it with the interface `I`.
   * 
   * @tparam I The interface type under which the instance is registered.
   * @tparam T The class type of the provided instance, which must be derived from `I`.
   * @param instance A `std::shared_ptr` to the instance of `T` that will be registered.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename I, typename T, typename S = SharedScope>
  EnableIf<IsBaseOf<I, T, S>, Container&> add(std::shared_ptr<T> instance);

  /*
   * @brief Registers a provided instance of the class `T` in the container.
   *
   * @tparam T The class type of the provided instance.
   * @param instance A `std::shared_ptr` to the instance of `T` that will be registered.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename T, typename S = SharedScope>
  EnableIf<IsBaseOf<T, T, S>, Container&> add(std::shared_ptr<T> instance);

  /*
   * @brief Registers a factory function in the container, associating it with the interface `I` and using the specified scope `S`.
   * 
   * The container will use the factory function to create instances of the interface type when requested.
   * 
   * @tparam I The interface type under which the factory function is registered.
   * @tparam S The scope type for object registration, defaulting to `UniqueScope`.
   *           Possible values: UniqueScope, SharedScope, SharedImlpScope.
   * @tparam F The factory function type, which must return a pointer or a smart pointer to a type derived from `I`.
   * @param functor The factory function that will create instances of `I`. The return type of `functor` should be a pointer 
   *                or smart pointer to a type derived from `I`.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename I, typename S, typename F>
  EnableIf<IsBaseOf<
    I,
    typename PointerTraits<typename FunctionTraits<F>::ReturnType>::ElementType,
    S>,
  Container&> addFactory(F functor);

  /*
   * @brief Registers a factory function in the container using the specified scope `S`.
   * 
   * The container will use the factory function to create instances of the type when requested.
   * 
   * @tparam S The scope type for object registration, defaulting to `UniqueScope`.
   *           Possible values: UniqueScope, SharedScope, SharedImlpScope.
   * @tparam F The factory function type, which must return a pointer or a smart pointer.
   * @param functor The factory function that will create instances of `I`.
   *                The return type of `functor` should be a pointer or a smart pointer.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename S, typename F>
  Container& addFactory(F functor);

  /*
   * @brief Registers the class `T` in the container, associating it with the interface `I` and using the scope `S`.
   *
   * Unlike single registration methods, `addMulti` supports multiple classes being registered under the same interface `I`.
   * When using a constructor parameter of type `std::vector<I>`, all registered classes will be instantiated.
   *
   * @tparam I The interface type under which the class `T` is registered.
   * @tparam T The class type being registered, which must be derived from `I`.
   * @tparam S The scope type for object registration, defaulting to `UniqueScope`.
   *           Possible values: UniqueScope, SharedScope, SharedImlpScope.
   * @tparam CallInit A boolean flag indicating whether to call the `init` method of `T` during instantiation, defaulting to false.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename I, typename T, typename S = UniqueScope, bool CallInit = false>
  EnableIf<IsBaseOf<I, T, S>, Container&> addMulti();

  /*
   * @brief Registers the class `T` in the container using the scope `S`.
   *
   * Unlike single registration methods, `addMulti` supports multiple classes being registered under the same interface `T`.
   * When using a constructor parameter of type `std::vector<T>`, all registered classes will be instantiated.
   *
   * @tparam T The class type being registered.
   * @tparam S The scope type for object registration, defaulting to `UniqueScope`.
   *           Possible values: UniqueScope, SharedScope, SharedImlpScope.
   * @tparam CallInit A boolean flag indicating whether to call the `init` method of `T` during instantiation, defaulting to false.
   * @return Container& A reference to the container for method chaining.
   */
  template <typename T, typename S = UniqueScope, bool CallInit = false>
  EnableIf<IsBaseOf<T, T, S>, Container&> addMulti();

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
  EnableIf<IsPointer<T>, T> createSpecial(Args* args);

  template <typename T>
  EnableIf<IsFunction<T>, T> createSpecial(Args* args);

  template <typename T>
  EnableIf<IsVector<T>, T> createSpecial(Args* args);

  template <typename T>
  EnableIf<
    !IsVector<T>     &&
    !IsFunction<T> &&
    !IsPointer<T>,
  T> createSpecial(Args* args);

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
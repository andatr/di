#ifndef YAGA_DI_OBJECT_FACTORY
#define YAGA_DI_OBJECT_FACTORY

#include <memory>
#include <utility>

#include "di/container.h"
#include "di/type_traits.h"

namespace yaga {
namespace di {

// -----------------------------------------------------------------------------------------------------------------------------
class ObjectFactory
{
public:
  template <typename T>
  static T create(Container* container, Args* args, bool callInit);

  template <typename T>
  static T* createPtr(Container* container, Args* args, bool callInit);
};

// -----------------------------------------------------------------------------------------------------------------------------
template<typename U>
constexpr auto IsCreatable(int) -> decltype(std::declval<Container>().template create<U>(), true) {
  return true;
}

template<typename U>
constexpr auto IsCreatable(...) -> bool {
  return false;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, int N>
struct CtorArg
{
  template <typename U, typename = typename EnableIf<!IsSame<T, U> && IsCreatable<U>(0)>>
  operator U() { return container_->createImpl<U>(args_); }

  operator Container*() { return container_; }

  template <typename U, typename = typename EnableIf<!IsSame<T, U> && !IsCreatable<U>(0)>>
  operator U&() { return container_->createImpl<U&>(args_); }

  operator Container&() { return *container_; }

  Container* container_;
  Args* args_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, int... N>
constexpr decltype(T(CtorArg<T, N>{}...), 0) countCtorArgs(int)
{
  return sizeof...(N);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, int... N>
constexpr int countCtorArgs(...)
{
  return countCtorArgs<T, N..., sizeof...(N)>(0);
}

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
auto initPtr(T* obj, int) -> decltype(obj->init()) {
  obj->init();
}

template<typename T>
void initPtr(T*, ...) { }

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
auto initCopy(T& obj, int) -> decltype(obj.init()) {
  obj.init();
}

template<typename T>
void initCopy(T&, ...) { }

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename U>
struct ObjectFactoryPtrHelper;

template <typename T, int... N>
struct ObjectFactoryPtrHelper<T, std::integer_sequence<int, N...>>
{
  static T* createPtr(Container* container, Args* args, bool callInit) { 
    (void)container;
    (void)args;
    T* ptr = new T(CtorArg<T, N>{ container, args }...);
    if (callInit) initPtr(ptr, 0);
    return ptr;
  }
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename U>
struct ObjectFactoryHelper;

template <typename T, int... N>
struct ObjectFactoryHelper<T, std::integer_sequence<int, N...>>
{
  static T create(Container* container, Args* args, bool callInit) { 
    (void)container;
    (void)args;
    T obj(CtorArg<T, N>{ container, args }...);
    if (callInit) initCopy(obj, 0);
    return obj;
  }
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T ObjectFactory::create(Container* container, Args* args, bool callInit)
{
  using H = ObjectFactoryHelper<T, std::make_integer_sequence<int, countCtorArgs<T>(0)>>;
  return H::create(container, args, callInit);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T* ObjectFactory::createPtr(Container* container, Args* args, bool callInit)
{
  using H = ObjectFactoryPtrHelper<T, std::make_integer_sequence<int, countCtorArgs<T>(0)>>;
  return H::createPtr(container, args, callInit);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <int N>
struct FunctorArg
{
  template <typename U, typename = typename EnableIf<IsCreatable<U>(0)>>
  operator U() { return container_->createImpl<U>(nullptr); }

  operator Container*() { return container_; }

  template <typename U, typename = typename EnableIf<!IsCreatable<U>(0)>>
  operator U&() { return container_->createImpl<U&>(nullptr); }

  operator Container&() { return *container_; }

  Container* container_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename F, typename U>
struct FunctorInvokerHelper;

template <typename F, int... N>
struct FunctorInvokerHelper<F, std::integer_sequence<int, N...>>
{
  using T = typename FunctionTraits<F>::ReturnType;
  static T invoke(F functor, Container* container) { 
    (void)container;
    return functor(FunctorArg<N>{ container }...);
  }
};

// -----------------------------------------------------------------------------------------------------------------------------
class FunctorInvoker
{
public:
  template <typename F>
  static typename FunctionTraits<F>::ReturnType invoke(F functor, Container* container);
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename F>
typename FunctionTraits<F>::ReturnType FunctorInvoker::invoke(F functor, Container* container)
{
  using H = FunctorInvokerHelper<F, std::make_integer_sequence<int, FunctionTraits<F>::Arity>>;
  return H::invoke(functor, container);
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_OBJECT_FACTORY
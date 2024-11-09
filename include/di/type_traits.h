#ifndef YAGA_DI_TYPE_TRAITS
#define YAGA_DI_TYPE_TRAITS

#include <functional>
#include <memory>
#include <vector>
#include <tuple>
#include <type_traits>

namespace yaga {
namespace di {

// -----------------------------------------------------------------------------------------------------------------------------
template <bool T, typename R = void>
using EnableIf = typename std::enable_if_t<T, R>;

template <class T>
using RemoveCV = typename std::remove_cv_t<T>;

template <class T>
using RemoveCVRef = typename std::remove_cvref_t<T>;

template <typename T>
constexpr bool IsReference = std::is_reference_v<T>;

template <typename T, typename U>
constexpr bool IsSame = std::is_same_v<RemoveCVRef<T>, RemoveCVRef<U>>;

template <typename T>
constexpr bool IsPurePtr = std::is_pointer_v<T>;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct SharedPtrTraits : std::false_type {};

template <typename T>
struct SharedPtrTraits<std::shared_ptr<T>> : std::true_type
{
  using ElementType = T;
};

template <typename T>
constexpr bool IsSharedPtr = SharedPtrTraits<T>::value;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct UniquePtrTraits : std::false_type {};

template <typename T>
struct UniquePtrTraits<std::unique_ptr<T>> : std::true_type
{
  using ElementType = T;
};

template <typename T>
constexpr bool IsIniquePtr = UniquePtrTraits<T>::value;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct VectorTraits : std::false_type {};

template <typename T>
struct VectorTraits<std::vector<T>> : std::true_type
{
  using ElementType = T;
};

template <typename T>
constexpr bool IsVector = VectorTraits<RemoveCV<T>>::value;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct PointerTraits
{
  using ElementType = RemoveCVRef<typename std::pointer_traits<T>::element_type>;
};

template <typename T>
struct PointerTraits<T&>
{
  using ElementType = RemoveCVRef<T>;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
constexpr bool IsPointer =
  IsPurePtr<T> ||
  IsIniquePtr<T> ||
  IsSharedPtr<T>;

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
struct IsFunctionT : std::false_type {};

template<typename Ret, typename... Args>
struct IsFunctionT<std::function<Ret(Args...)>> : std::true_type {};

template<typename T>
inline constexpr bool IsFunction = IsFunctionT<T>::value;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct FunctionTraits;

template <typename Ret, typename... Args>
struct FunctionTraits<Ret(Args...)> {
  using ReturnType = Ret;
  using ArgumentTypes = std::tuple<Args...>;
  static constexpr std::size_t Arity = sizeof...(Args);
};

template <typename Ret, typename... Args>
struct FunctionTraits<Ret(*)(Args...)> : FunctionTraits<Ret(Args...)> {};

template <typename Ret, typename... Args>
struct FunctionTraits<std::function<Ret(Args...)>> : FunctionTraits<Ret(Args...)> {};

template <typename ClassType, typename Ret, typename... Args>
struct FunctionTraits<Ret(ClassType::*)(Args...)> : FunctionTraits<Ret(Args...)> {};

template <typename ClassType, typename Ret, typename... Args>
struct FunctionTraits<Ret(ClassType::*)(Args...) const> : FunctionTraits<Ret(Args...)> {};

template <typename Functor>
struct FunctionTraits : FunctionTraits<decltype(&Functor::operator())> {};

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_TYPE_TRAITS
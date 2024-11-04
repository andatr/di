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
template <typename T>
constexpr bool is_reference = std::is_reference_v<T>;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct shared_ptr_traits : std::false_type {};

template <typename T>
struct shared_ptr_traits<std::shared_ptr<T>> : std::true_type
{
  using element_type = T;
};

template <typename T>
constexpr bool is_shared_ptr = shared_ptr_traits<T>::value;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct unique_ptr_traits : std::false_type {};

template <typename T>
struct unique_ptr_traits<std::unique_ptr<T>> : std::true_type
{
  using element_type = T;
};

template <typename T>
constexpr bool is_unique_ptr = unique_ptr_traits<T>::value;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct vector_traits : std::false_type {};

template <typename T>
struct vector_traits<std::vector<T>> : std::true_type
{
  using element_type = T;
};

template <typename T>
constexpr bool is_vector = vector_traits<std::remove_cv_t<T>>::value;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
struct pointer_traits
{
  using element_type = typename std::pointer_traits<T>::element_type;
};

template <typename T>
struct pointer_traits<T&>
{
  using element_type = T;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
constexpr bool is_pure_ptr = std::is_pointer_v<T>;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
constexpr bool is_pointer =
  di::is_pure_ptr<T> ||
  di::is_unique_ptr<T> ||
  di::is_shared_ptr<T>;

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T, typename U>
constexpr bool is_same = std::is_same_v<std::remove_cvref_t<T>, std::remove_cvref_t<U>>;

// -----------------------------------------------------------------------------------------------------------------------------
template<typename T>
struct is_function : std::false_type {};

template<typename Ret, typename... Args>
struct is_function<std::function<Ret(Args...)>> : std::true_type {};

template<typename T>
inline constexpr bool is_function_v = is_function<T>::value;

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_TYPE_TRAITS
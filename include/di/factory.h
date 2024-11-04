#ifndef YAGA_DI_FACTORY_H
#define YAGA_DI_FACTORY_H

#include <memory>
#include <type_traits>

#include "di/args.h"
#include "di/type_traits.h"

namespace yaga {
namespace di {

struct Policy {};

/*
 * @brief Policy that enforces unique object creation in the dependency injection container.
 *
 * The `UniquePolicy` specifies that the `create` method will generate a new instance of 
 * a given class type every time it is called. This policy ensures that requests for the 
 * same type will result in distinct instances.
 */
struct UniquePolicy : public Policy {};

/**
 * @brief Policy that allows shared ownership of an object in the dependency injection container.
 *
 * The `SharedPolicy` specifies that the `create` method will return the same instance of a 
 * given class type each time it is called. This policy allows multiple components to share 
 * ownership of the same instance managed by the dependency injection container.
 */
struct SharedPolicy : public Policy {};

/**
 * @brief Policy that allows shared ownership with interface-based instance sharing.
 *
 * The `SharedImlpPolicy` extends the behavior of the `SharedPolicy` by allowing the same 
 * instance of a class to be returned when requested under multiple interfaces. For example, 
 * if a class `MyClass` implements both `MyInterface1` and `MyInterface2`, this policy ensures 
 * that a single instance of `MyClass` is provided when requested via either interface.
 * In contrast `SharedPolicy` would return separate instances for different interfaces. 
 */
struct SharedImlpPolicy : public Policy {};

class Container;

class Factory
{
public:
  virtual ~Factory() {}

  explicit Factory(bool callInit) : callInit_(callInit) {}
  
  template <typename T>
  std::enable_if_t<is_pure_ptr<T>, T> createObject(Container* container, Args* args);

  template <typename T>
  std::enable_if_t<is_shared_ptr<T>, T> createObject(Container* container, Args* args);

  template <typename T>
  std::enable_if_t<is_unique_ptr<T>, T> createObject(Container* container, Args* args);

  template <typename T>
  std::enable_if_t<std::is_reference_v<T>, std::remove_cvref_t<T>&> createObject(Container* container, Args* args);

  template <typename T>
  std::enable_if_t<
    !is_pointer<T> &&
    !std::is_reference_v<T>
  , T> createObject(Container* container, Args* args);

  virtual void* createPure(Container* container, Args* args) = 0;

  virtual std::shared_ptr<void> createShared(Container* container, Args* args) = 0;

  virtual void* createUnique(Container* container, Args* args) = 0;

  virtual void* createReference(Container* container, Args* args) = 0;

  virtual bool allowInstanceCreation() = 0;

protected:
  bool callInit_;
};

using FactorySPtr = std::shared_ptr<Factory>;

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_FACTORY_H
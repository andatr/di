#ifndef YAGA_DI_SCOPE_H
#define YAGA_DI_SCOPE_H

namespace yaga {
namespace di {

struct Scope {};

/*
 * @brief Scope that enforces unique object creation in the dependency injection container.
 *
 * The `UniqueScope` specifies that the `create` method will generate a new instance of 
 * a given class type every time it is called. This scope ensures that requests for the 
 * same type will result in distinct instances.
 */
struct UniqueScope : public Scope {};

/**
 * @brief Scope that allows shared ownership of an object in the dependency injection container.
 *
 * The `SharedScope` specifies that the `create` method will return the same instance of a 
 * given class type each time it is called. This scope allows multiple components to share 
 * ownership of the same instance managed by the dependency injection container.
 */
struct SharedScope : public Scope {};

/**
 * @brief Scope that allows shared ownership with interface-based instance sharing.
 *
 * The `SharedImlpScope` extends the behavior of the `SharedScope` by allowing the same 
 * instance of a class to be returned when requested under multiple interfaces. For example, 
 * if a class `MyClass` implements both `MyInterface1` and `MyInterface2`, this scope ensures 
 * that a single instance of `MyClass` is provided when requested via either interface.
 * In contrast `SharedScope` would return separate instances for different interfaces. 
 */
struct SharedImlpScope : public Scope {};

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_SCOPE_H
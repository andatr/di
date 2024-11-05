# Dependency Injection Container (C++)

[![linux](https://github.com/andatr/di/actions/workflows/linux.yml/badge.svg)](https://github.com/andatr/di/actions/workflows/linux.yml)
[![windows](https://github.com/andatr/di/actions/workflows/windows.yml/badge.svg)](https://github.com/andatr/di/actions/workflows/windows.yml)
[![macos](https://github.com/andatr/di/actions/workflows/macos.yml/badge.svg)](https://github.com/andatr/di/actions/workflows/macos.yml)

Dependency Injection (DI) is a design pattern that allows an objects to receive its dependencies from external sources rather than creating them internally.
This approach promotes the separation of concerns of constructing objects and using them, leading to loosely coupled code.

While DI can be implemented without any specific libraries or frameworks, simply by passing an object's dependencies to its constructor, doing so can present challenges in practice.
This library aims to address those challenges.

One of the primary goals of this library is to keep client code free from dependencies on the DI framework itself.
The library is designed solely for the purposes of registration and object creation, allowing client classes to remain unaware of its details.
This design makes it easy to switch to alternative DI libraries if needed.

## Requirements:

- **C++20 or higher** is required.

## Features

1. The library separates registration and object creation, allowing you to register classes in one place and create instances elsewhere.
This introduces some overhead, as explained below, but adds flexibility to your code structure.

2. The library is multi-threaded, meaning you can register and create objects safely from different threads.
It employs a simple locking mechanism for object registration and creation, ensuring that only one object can be registered or created at a time.
Be careful when working with shared dependencies, as the library only ensures that they are created correctly.
All access to these shared dependencies should be synchronized externally to avoid potential issues.

3. Supports raw pointers, standard smart pointers, and references, meaning you do not need to register them separately.
If you register just your class, the library will automatically create dependencies of these types as needed.

4. Includes three built-in policies that control object instantiation behavior: `UniquePolicy`, `SharedPolicy`, and `SharedImlpPolicy`.
- **`UniquePolicy`** specifies that the `create` method will generate a new instance of a given class type every time it is called.
This policy ensures that requests for the same type will result in distinct instances.
- **`SharedPolicy`** specifies that the `create` method will return the same instance of a given class type each time it is called.
This policy allows multiple components to share ownership of the same instance managed by the dependency injection container.
- **`SharedImlpPolicy`** extends the behavior of the `SharedPolicy` by allowing the same instance of a class to be returned when requested under multiple interfaces.
For example, if a class `MyClass` implements both `MyInterface1` and `MyInterface2`, this policy ensures that a single instance of `MyClass` is provided when requested via either interface.
In contrast `SharedPolicy` would return separate instances for different interfaces. 

One thing to keep in mind is that this library intentionally doesn't manage object lifetimes.
When using `SharedPolicy`, the library must store a `shared_ptr` to each instance to ensure the same instance is provided every time.
However, with `UniquePolicy` and raw pointers, it's your responsibility to delete the objects once you're done with them.

Obviously, not every type of smart pointer is supported by each policy.
Here is a table of allowed types for each policy:

```
| Type                | UniquePolicy  | SharedPolicy   | SharedImplPolicy    |
|---------------------|---------------|----------------|---------------------|
| std::shared_ptr     | ✔️            | ✔️            | ✔️                  |
| std::unique_ptr     | ✔️            | ✘             | ✘                   |
| raw pointer         | ✔️            | ✔️            | ✔️                  |
| object copy         | ✔️            | ✘             | ✘                   |
| object reference    | ✘             | ✔️            | ✔️                  |
```
Policies can be easily extended by creating a new empty structure that inherits from `Policy` along with a corresponding factory class.

5. Sometimes, full object initialization can’t be achieved in the constructor alone.
For instance, you may want to implement virtual behavior during initialization, which typically requires a separate init() method to complete the setup.
Although considered an anti-pattern today, this approach was common in the past.
This library can accommodate this pattern by automatically calling an init() method if it’s defined in your class.
You can enable this behavior by passing a special flag during object creation.

6. Another common scenario arises when you want to instantiate a factory rather than a specific class.
In such cases, you may only provide a subset of the required arguments, while the remaining dependencies should be instantiated from the container.
Typically, this can be accomplished by registering the factory in the container.
This library simplifies this process: if you attempt to instantiate a `std::function` without registering it, the library will automatically generate the function for you.
It captures the container pointer and uses it to instantiate any missing arguments, streamlining your dependency management.

 ```cpp
class Invoice
{
public:
  Invoice(int user, Logger* logger) ...
...
}

di::Container container;
container.add<ILogger, ConsoleLogger, di::SharedPolicy>();
container.add<Invoice, di::UniquePolicy>();
auto invoiceFactory = container.create<std::function<Invoice*(int)>>();
Invoice* invoice = invoiceFactory(123); // Logger* logger is instantinated automatically
```

7. The last feature worth mentioning is the ability to register multiple classes under the same interface and instantiate them using `std::vector`.
Imagine you have an application that supports plugins through an `IPlugin` interface and you want to pass all of them to your `Application` class.
This is the situation when you want to use this feature: you register all your plugins under the `IPlugin` interface and then introduce a `std::vector<IPlugin>` argument for your `Application` class.

```cpp
class IPlugin ...
class Plugin1 : public IPlugin ...
class Plugin2 : public IPlugin ...
class Application(std::vector<IPlugin> plugins) ...

di::Container container;
container.addMulti<IPlugin, Plugin1>();
container.addMulti<IPlugin, Plugin2>();
container.add<Application>();
auto app = container.create<Application>();
```

The standard `add` method does not allow registering classes under the same interface more than once, so a special method called `addMulti` was introduced to eliminate confusion.
When the library detects a `std::vector<...>` constructor argument, it first checks for appropriate vector registrations.
If none are found, it then looks for classes registered with `addMulti` to populate the vector.

## Limitations

1. This library inherits the fundamental limitation of not being able to resolve different dependencies for the same type.
For example, if your class's constructor is defined as `MyClass(int minValue, int maxValue)`, the DI container cannot differentiate between `minValue` and `maxValue` as separate dependencies.
To address this, these values must be wrapped in different structs, such as:

 ```cpp
struct MinValue { int value; };
struct MaxValue { int value; };
MyClass(MinValue minValue, MaxValue maxValue) { ... }
```
 
2. Due to the way this library is implemented, it always attempts to use the constructor with the minimal number of arguments.
Therefore, if your class has multiple constructors, only the first one with the smallest number of parameters will be used in an attempt to instantiate the class.

## Overhead

1. Since this library separates registration and object creation, it requires storing registration data in memory.
This introduces some minor memory overhead, though it is generally insignificant unless you are an embedded developer.
Additionally, shared dependencies are stored as shared pointers to ensure that the same instance can be provided to multiple classes when needed.

2. Each dependency resolution involves searching the registration dictionary, sometimes twice.
While this search has a time complexity of O(1), it is still significantly more complex than simply passing dependencies directly to an object.
For instance, if your class has four constructor parameters, there may be between five to nine dictionary searches involved.
This overhead is manageable for objects that are created only once, but you may want to consider a different approach for objects requiring frequent allocations in performance-critical code.

## Warning

The DI approach can make your code difficult to debug, as it can obscure function calls and hide interface implementations behind the registration process.
Please consider this when using the library.
  

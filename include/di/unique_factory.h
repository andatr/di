#ifndef YAGA_DI_UNIQUE_FACTORY
#define YAGA_DI_UNIQUE_FACTORY

#include <memory>

#include "di/factory.h"
#include "di/object_factory.h"

namespace yaga {
namespace di {

template <typename I, typename T>
class UniqueFactory : public Factory
{
public:
  explicit UniqueFactory(bool callInit = false);

protected:  
  void* createPure(Container* container, Args* args) override;
  
  std::shared_ptr<void> createShared(Container* container, Args* args) override;
  
  void* createUnique(Container* container, Args* args) override;
  
  void* createReference(Container* container, Args* args) override;
  
  bool allowInstanceCreation() override { return true; }

  virtual T* createInstance(Container* container, Args* args);
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
UniqueFactory<I, T>::UniqueFactory(bool callInit) :
  Factory(callInit)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
T* UniqueFactory<I, T>::createInstance(Container* container, Args* args)
{
  return ObjectFactory::createPtr<T>(container, args, callInit_);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* UniqueFactory<I, T>::createPure(Container* container, Args* args)
{
  I* ptr = createInstance(container, args);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<void> UniqueFactory<I, T>::createShared(Container* container, Args* args)
{
  std::shared_ptr<I> ptr = std::shared_ptr<I>(createInstance(container, args));
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* UniqueFactory<I, T>::createUnique(Container* container, Args* args)
{
  I* ptr = createInstance(container, args);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* UniqueFactory<I, T>::createReference(Container*, Args*)
{
  throw std::runtime_error(std::string("Creating a reference to ") + typeid(T).name() + " is not allowed under the Unique Scope");
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_UNIQUE_FACTORY
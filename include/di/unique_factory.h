#ifndef YAGA_DI_UNIQUE_FACTORY
#define YAGA_DI_UNIQUE_FACTORY

#include <memory>

#include "di/factory.h"
#include "di/object_factory.h"

namespace yaga {
namespace di {

template <typename I, typename T>
class UniqueFactory final : public Factory
{
public:
  explicit UniqueFactory(bool init) : Factory(init) {}

protected:  
  void* createPure(Container* container, Args* args) override;
  
  std::shared_ptr<void> createShared(Container* container, Args* args) override;
  
  void* createUnique(Container* container, Args* args) override;
  
  void* createReference(Container* container, Args* args) override;
  
  bool allowInstanceCreation() override { return true; }
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* UniqueFactory<I, T>::createPure(Container* container, Args* args)
{
  I* ptr = ObjectFactory::createPtr<T>(container, args, callInit_);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<void> UniqueFactory<I, T>::createShared(Container* container, Args* args)
{
  std::shared_ptr<I> ptr = std::shared_ptr<I>(ObjectFactory::createPtr<T>(container, args, callInit_));
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* UniqueFactory<I, T>::createUnique(Container* container, Args* args)
{
  return createPure(container, args);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* UniqueFactory<I, T>::createReference(Container*, Args*)
{
  throw std::runtime_error(std::string("Creating a reference to ") + typeid(T).name() + " is not allowed under the Unique policy");
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_UNIQUE_FACTORY
#ifndef YAGA_DI_SHARED_IMPL_FACTORY
#define YAGA_DI_SHARED_IMPL_FACTORY

#include <unordered_map>
#include <memory>

#include "di/factory.h"
#include "di/factory_context.h"
#include "di/object_factory.h"

namespace yaga {
namespace di {

// -----------------------------------------------------------------------------------------------------------------------------
struct SharedImlpFactoryContext
{
  std::unordered_map<std::type_index, std::shared_ptr<void>> instances;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
class SharedImlpFactory : public Factory
{
public:
  explicit SharedImlpFactory(FactoryContext* context, bool callInit = false, std::shared_ptr<T> instance = nullptr);

protected:
  void* createPure(Container* container, Args* args) override;
  
  std::shared_ptr<void> createShared(Container* container, Args* args) override;
  
  void* createUnique(Container* container, Args* args) override;
  
  void* createReference(Container* container, Args* args) override;

  bool allowInstanceCreation() override { return false; }

  std::shared_ptr<T> getInstance(Container* container, Args* args);

  virtual T* createInstance(Container* container, Args* args);

protected:
  SharedImlpFactoryContext* context_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
SharedImlpFactory<I, T>::SharedImlpFactory(FactoryContext* context, bool callInit, std::shared_ptr<T> instance) :
  Factory(callInit),
  context_(context->get<SharedImlpFactoryContext>()) 
{
  if (instance) {
    context_->instances[typeid(T)] = instance;
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<T> SharedImlpFactory<I, T>::getInstance(Container* container, Args* args)
{
  auto it = context_->instances.find(typeid(T));
  if (it != context_->instances.end() && it->second) return std::static_pointer_cast<T>(it->second);
  auto instance = std::shared_ptr<T>(createInstance(container, args));
  context_->instances[typeid(T)] = instance;
  return std::static_pointer_cast<T>(instance);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
T* SharedImlpFactory<I, T>::createInstance(Container* container, Args* args)
{
  return ObjectFactory::createPtr<T>(container, args, callInit_);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SharedImlpFactory<I, T>::createPure(Container* container, Args* args)
{
  I* ptr = getInstance(container, args).get();
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<void> SharedImlpFactory<I, T>::createShared(Container* container, Args* args)
{
  std::shared_ptr<I> ptr = getInstance(container, args);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SharedImlpFactory<I, T>::createUnique(Container* container, Args* args)
{
  I* ptr = ObjectFactory::createPtr<T>(container, args, callInit_);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SharedImlpFactory<I, T>::createReference(Container* container, Args* args)
{
  return createPure(container, args);
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_SHARED_IMPL_FACTORY
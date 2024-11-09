#ifndef YAGA_DI_SHARED_FACTORY
#define YAGA_DI_SHARED_FACTORY

#include <memory>

#include "di/factory.h"
#include "di/object_factory.h"

namespace yaga {
namespace di {

template <typename I, typename T>
class SharedFactory : public Factory
{
public:
  explicit SharedFactory(bool callInit = false, std::shared_ptr<T> instance = nullptr);
  
protected:
  void* createPure(Container* container, Args* args) override;
  
  std::shared_ptr<void> createShared(Container* container, Args* args) override;
  
  void* createUnique(Container* container, Args* args) override;
  
  void* createReference(Container* container, Args* args) override;

  bool allowInstanceCreation() override { return false; }

  std::shared_ptr<T> getInstance(Container* container, Args* args);

  virtual T* createInstance(Container* container, Args* args);

protected:
  std::shared_ptr<T> instance_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
SharedFactory<I, T>::SharedFactory(bool callInit, std::shared_ptr<T> instance) :
  Factory(callInit),
  instance_(instance)
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
T* SharedFactory<I, T>::createInstance(Container* container, Args* args)
{
  return ObjectFactory::createPtr<T>(container, args, callInit_);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<T> SharedFactory<I, T>::getInstance(Container* container, Args* args)
{
  if (!instance_) {
    instance_ = std::shared_ptr<T>(createInstance(container, args));
  }
  return instance_;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SharedFactory<I, T>::createPure(Container* container, Args* args)
{
  I* ptr = getInstance(container, args).get();
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<void> SharedFactory<I, T>::createShared(Container* container, Args* args)
{
  std::shared_ptr<I> ptr = getInstance(container, args);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SharedFactory<I, T>::createUnique(Container* container, Args* args)
{
  I* ptr = ObjectFactory::createPtr<T>(container, args, callInit_);
  return ptr;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SharedFactory<I, T>::createReference(Container* container, Args* args)
{
  return createPure(container, args);
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_SHARED_FACTORY
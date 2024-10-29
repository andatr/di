#ifndef YAGA_DI_SINGLE_FACTORY
#define YAGA_DI_SINGLE_FACTORY

#include <memory>

#include "di/factory.h"
#include "di/object_factory.h"

namespace yaga {
namespace di {

template <typename I, typename T>
class SingleFactory final : public Factory
{
public:
  explicit SingleFactory(bool init, std::shared_ptr<T> instance = nullptr) : Factory(init), instance_(instance) {}
  void*                 createPure  (Container* container, Args* args) override;
  std::shared_ptr<void> createShared(Container* container, Args* args) override;
  void*                 createUnique(Container* container, Args* args) override;

private:
  std::shared_ptr<T> getInstance(Container* container, Args* args);

private:
  std::shared_ptr<T> instance_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<T> SingleFactory<I, T>::getInstance(Container* container, Args* args)
{
  if (!instance_) instance_ = std::shared_ptr<T>(ObjectFactory::createPtr<T>(container, args, init_));
  return instance_;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SingleFactory<I, T>::createPure(Container* container, Args* args)
{
  return static_cast<I*>(getInstance(container, args).get());
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<void> SingleFactory<I, T>::createShared(Container* container, Args* args)
{
  return std::static_pointer_cast<I>(getInstance(container, args));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SingleFactory<I, T>::createUnique(Container* container, Args* args)
{
  return dynamic_cast<I*>(ObjectFactory::createPtr<T>(container, args, init_));
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_SINGLE_FACTORY
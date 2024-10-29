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
  void*                 createPure  (Container* container, Args* args) override;
  std::shared_ptr<void> createShared(Container* container, Args* args) override;
  void*                 createUnique(Container* container, Args* args) override;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* UniqueFactory<I, T>::createPure(Container* container, Args* args)
{
  return dynamic_cast<I*>(ObjectFactory::createPtr<T>(container, args, init_));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<void> UniqueFactory<I, T>::createShared(Container* container, Args* args)
{
  return std::shared_ptr<I>(ObjectFactory::createPtr<T>(container, args, init_));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* UniqueFactory<I, T>::createUnique(Container* container, Args* args)
{
  return createPure(container, args);
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_UNIQUE_FACTORY
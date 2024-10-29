#ifndef YAGA_DI_SINGLE_IMPL_FACTORY
#define YAGA_DI_SINGLE_IMPL_FACTORY

#include <unordered_map>
#include <memory>

#include "di/factory.h"
#include "di/factory_context.h"
#include "di/object_factory.h"

namespace yaga {
namespace di {

// -----------------------------------------------------------------------------------------------------------------------------
struct SingleImlpFactoryContext
{
  std::unordered_map<std::type_index, std::shared_ptr<void>> instances;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
class SingleImlpFactory final : public Factory
{
public:
  explicit SingleImlpFactory(bool init, FactoryContext* context);
  void*                 createPure  (Container* container, Args* args) override;
  std::shared_ptr<void> createShared(Container* container, Args* args) override;
  void*                 createUnique(Container* container, Args* args) override;

private:
  std::shared_ptr<T> getInstance(Container* container, Args* args);

private:
  SingleImlpFactoryContext* context_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
SingleImlpFactory<I, T>::SingleImlpFactory(bool init, FactoryContext* context) :
  Factory(init),
  context_(context->get<SingleImlpFactoryContext>())
{
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<T> SingleImlpFactory<I, T>::getInstance(Container* container, Args* args)
{
  auto it = context_->instances.find(typeid(T));
  if (it != context_->instances.end()) return std::static_pointer_cast<T>(it->second);
  auto instance = std::shared_ptr<T>(ObjectFactory::createPtr<T>(container, args, init_));
  context_->instances[typeid(T)] = instance;
  return std::static_pointer_cast<T>(instance);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SingleImlpFactory<I, T>::createPure(Container* container, Args* args)
{
  return static_cast<I*>(getInstance(container, args).get());
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
std::shared_ptr<void> SingleImlpFactory<I, T>::createShared(Container* container, Args* args)
{
  return std::static_pointer_cast<I>(getInstance(container, args));
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename I, typename T>
void* SingleImlpFactory<I, T>::createUnique(Container* container, Args* args)
{
  return dynamic_cast<I*>(ObjectFactory::createPtr<T>(container, args, init_));
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_SINGLE_IMPL_FACTORY
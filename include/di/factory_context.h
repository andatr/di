#ifndef YAGA_DI_FACTORY_CONTEXT
#define YAGA_DI_FACTORY_CONTEXT

#include <memory>
#include <typeindex>
#include <unordered_map>

namespace yaga {
namespace di {

struct FactoryContext
{
public:
  template <typename T>
  T* get();

private:
  std::unordered_map<std::type_index, std::shared_ptr<void>> storage_;
};

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T* FactoryContext::get()
{
  auto it = storage_.find(typeid(T));
  if (it != storage_.end()) return std::static_pointer_cast<T>(it->second).get();
  auto ptr = std::make_shared<T>();
  storage_[typeid(T)] = ptr;
  return ptr.get();
}

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_FACTORY_CONTEXT
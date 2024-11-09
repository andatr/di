#ifndef YAGA_DI_FACTORY_H
#define YAGA_DI_FACTORY_H

#include <memory>
#include <type_traits>

#include "di/args.h"
#include "di/scope.h"
#include "di/type_traits.h"

namespace yaga {
namespace di {

class Container;

class Factory
{
public:
  virtual ~Factory() {}

  explicit Factory(bool callInit) : callInit_(callInit) {}
  
  template <typename T>
  EnableIf<IsPurePtr<T>, T> createObject(Container* container, Args* args);

  template <typename T>
  EnableIf<IsSharedPtr<T>, T> createObject(Container* container, Args* args);

  template <typename T>
  EnableIf<IsIniquePtr<T>, T> createObject(Container* container, Args* args);

  template <typename T>
  EnableIf<IsReference<T>, RemoveCVRef<T>&> createObject(Container* container, Args* args);

  template <typename T>
  EnableIf<
    !IsPointer<T> &&
    !IsReference<T>
  , T> createObject(Container* container, Args* args);

  virtual void* createPure(Container* container, Args* args) = 0;

  virtual std::shared_ptr<void> createShared(Container* container, Args* args) = 0;

  virtual void* createUnique(Container* container, Args* args) = 0;

  virtual void* createReference(Container* container, Args* args) = 0;

  virtual bool allowInstanceCreation() = 0;

protected:
  bool callInit_;
};

using FactorySPtr = std::shared_ptr<Factory>;

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_FACTORY_H
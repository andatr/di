#ifndef YAGA_DI_FACTORY_H
#define YAGA_DI_FACTORY_H

#include <memory>

#include "di/args.h"

namespace yaga {
namespace di {

struct Policy {};

struct UniquePolicy : public Policy {};

struct SinglePolicy : public Policy {};

struct SingleImlpPolicy : public Policy {};

class Container;

class Factory
{
public:
  virtual ~Factory() {}
  explicit Factory(bool init) : init_(init) {}
  virtual void*                 createPure  (Container* container, Args* args) = 0;
  virtual std::shared_ptr<void> createShared(Container* container, Args* args) = 0;
  virtual void*                 createUnique(Container* container, Args* args) = 0;
  bool initObject() const { return init_; }

protected:
  bool init_;
};

using FactorySPtr = std::shared_ptr<Factory>;

} // !namespace di
} // !namespace yaga

#endif // !YAGA_DI_FACTORY_H
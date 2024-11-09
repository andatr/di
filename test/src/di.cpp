#include "di/di.h"
#include <string>
#include <type_traits>
#include <boost/test/unit_test.hpp>

using namespace yaga;

BOOST_AUTO_TEST_SUITE(DiTest)

// -----------------------------------------------------------------------------------------------------------------------------
class IDependency
{
public:
  virtual ~IDependency() {}
  virtual std::string& str() = 0;
};

// -----------------------------------------------------------------------------------------------------------------------------
class Dependency1 final : public IDependency
{
public:
  static int dtorCalls;

public:
  ~Dependency1() { ++dtorCalls; }
  std::string& str() override { return str_; }

private:
  std::string str_;
};

int Dependency1::dtorCalls = 0;

// -----------------------------------------------------------------------------------------------------------------------------
class Dependency2 final : public IDependency
{
public:
  std::string& str() override { return str_; }

private:
  std::string str_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class Dependency3 final : public IDependency
{
public:
  std::string& str() override { return str_; }

private:
  std::string str_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class PurePtrDependant
{
public:
  PurePtrDependant(IDependency* d) : d_(d) {}
  IDependency* dependency() { return d_; }

private:
  IDependency* d_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class UniquePtrDependant
{
public:
  UniquePtrDependant(std::unique_ptr<IDependency> d) : d_(std::move(d)) {}
  std::unique_ptr<IDependency>& dependency() { return d_; }

private:
  std::unique_ptr<IDependency> d_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class SharedPtrDependant
{
public:
  SharedPtrDependant(std::shared_ptr<IDependency> d) : d_(d) {}
  std::shared_ptr<IDependency> dependency() { return d_; }

private:
  std::shared_ptr<IDependency> d_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class ConstPtrDependant
{
public:
  ConstPtrDependant(const IDependency* const d) : d_(d) {}
  const IDependency* dependency() { return d_; }

private:
  const IDependency* d_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class ConstUniquePtrDependant
{
public:
  ConstUniquePtrDependant(std::unique_ptr<const IDependency> d) : d_(std::move(d)) {}
  const std::unique_ptr<const IDependency>& dependency() { return d_; }

private:
  std::unique_ptr<const IDependency> d_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class ConstSharedPtrDependant
{
public:
  ConstSharedPtrDependant(std::shared_ptr<const IDependency> const d) : d_(d) {}
  const std::shared_ptr<const IDependency> dependency() { return d_; }

private:
  std::shared_ptr<const IDependency> d_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class CopyDependant
{
public:
  explicit CopyDependant(Dependency1 d) : d_(d) {}
  std::string& str() { return d_.str(); }

private:
  Dependency1 d_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class InitDependency final : public IDependency
{
public:
  InitDependency() : counter_(0) {}
  std::string& str() override { return str_; }
  void init() { ++counter_; }
  int counter() const { return counter_; }

private:
  std::string str_;
  int counter_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class IDependency2
{
public:
  virtual ~IDependency2() {}
  virtual int& integer() = 0;
};

// -----------------------------------------------------------------------------------------------------------------------------
class DoubleDependency : public IDependency, public IDependency2
{
public:
  DoubleDependency() : integer_(0) {}
  std::string& str() override { return str_; }
  int& integer() override { return integer_; }

private:
  std::string str_;
  int integer_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class VectorDependant
{
public:
  VectorDependant(std::vector<std::shared_ptr<IDependency>> v, std::shared_ptr<IDependency> d) : v_(v), d_(d) {}
  std::vector<std::shared_ptr<IDependency>>& dependencies() { return v_; }
  std::string& str() { return d_->str(); }

private:
  std::vector<std::shared_ptr<IDependency>> v_;
  std::shared_ptr<IDependency> d_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class VectorRefDependant
{
public:
  VectorRefDependant(const std::vector<std::shared_ptr<IDependency>>& v, std::shared_ptr<IDependency> d) : v_(v), d_(d) {}
  std::vector<std::shared_ptr<IDependency>>& dependencies() { return v_; }
  std::string& str() { return d_->str(); }

private:
  std::vector<std::shared_ptr<IDependency>> v_;
  std::shared_ptr<IDependency> d_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class DiDependant
{
public:
  explicit DiDependant(di::Container* container) : container_(container) {}
  di::Container* container() { return container_; }

private:
  di::Container* container_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class DiDependantRef
{
public:
  explicit DiDependantRef(di::Container& container) : container_(container) {}
  di::Container& container() { return container_; }

private:
  di::Container& container_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class IDependencyChar
{
public:
  virtual ~IDependencyChar() {}
  virtual char& value() = 0;
};

// -----------------------------------------------------------------------------------------------------------------------------
class DependencyChar final : public IDependencyChar
{
public:
  DependencyChar() : value_('A') {}
  char& value() override { return value_; }

private:
  char value_;
};

// -----------------------------------------------------------------------------------------------------------------------------
struct FactoryArg1
{
  int value;
};

// -----------------------------------------------------------------------------------------------------------------------------
struct FactoryArg2
{
  int value;
};

// -----------------------------------------------------------------------------------------------------------------------------
struct FactoryArg3
{
  int value;
};

// -----------------------------------------------------------------------------------------------------------------------------
struct FactoryResultPure
{
  FactoryResultPure(FactoryArg1* arg1, FactoryArg2* arg2, FactoryArg3* arg3) : 
    arg1(arg1), arg2(arg2), arg3(arg3) {}
  FactoryArg1* arg1;
  FactoryArg2* arg2;
  FactoryArg3* arg3;
};

// -----------------------------------------------------------------------------------------------------------------------------
struct FactoryResultUnique
{
  FactoryResultUnique(std::unique_ptr<FactoryArg1> arg1, std::unique_ptr<FactoryArg2> arg2, std::unique_ptr<FactoryArg3> arg3) : 
    arg1(std::move(arg1)), arg2(std::move(arg2)), arg3(std::move(arg3)) {}
  std::unique_ptr<FactoryArg1> arg1;
  std::unique_ptr<FactoryArg2> arg2;
  std::unique_ptr<FactoryArg3> arg3;
};

// -----------------------------------------------------------------------------------------------------------------------------
struct FactoryResultShared
{
  FactoryResultShared(std::shared_ptr<FactoryArg1> arg1, std::shared_ptr<FactoryArg2> arg2, std::shared_ptr<FactoryArg3> arg3) : 
    arg1(arg1), arg2(arg2), arg3(arg3) {}
  std::shared_ptr<FactoryArg1> arg1;
  std::shared_ptr<FactoryArg2> arg2;
  std::shared_ptr<FactoryArg3> arg3;
};

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ClassNotRegistered)
{
  try {
    di::Container container;
    container.add<IDependency, Dependency1>();
    container.create<PurePtrDependant>();
    BOOST_TEST(false);
  }
  catch (...) {
    BOOST_TEST(true);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ClassAlreadyRegistered)
{
  try {
    di::Container container;
    container.add<IDependency, Dependency1>();
    container.add<IDependency, Dependency2>();
    BOOST_TEST(false);
  }
  catch (...) {
    BOOST_TEST(true);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(PurePtrShared)
{
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::SharedScope>();
    auto inst1 = container.create<IDependency*>();
    BOOST_TEST(inst1 != nullptr);
    inst1->str() = "instance1";
    auto inst2 = container.create<IDependency*>();
    BOOST_TEST(inst1 == inst2);
    inst2->str() = "instance2";
    BOOST_TEST(inst1->str() == "instance2");
    BOOST_TEST(inst2->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(PurePtrUnique)
{
  Dependency1::dtorCalls = 0;
  IDependency* inst1 = nullptr;
  IDependency* inst2 = nullptr;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::UniqueScope>();
    inst1 = container.createPtr<IDependency>();
    BOOST_TEST(inst1 != nullptr);
    inst1->str() = "instance1";
    inst2 = container.createPtr<IDependency>();
    BOOST_TEST(inst2 != nullptr);
    BOOST_TEST(inst1 != inst2);
    inst2->str() = "instance2";
    BOOST_TEST(inst1->str() == "instance1");
    BOOST_TEST(inst2->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
  delete inst1;
  delete inst2;
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SharedPtrShared)
{
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    {
      container.add<IDependency, Dependency1, di::SharedScope>();
      auto inst1 = container.createShared<IDependency>();
      BOOST_TEST(inst1 != nullptr);
      inst1->str() = "instance1";
      auto inst2 = container.createShared<IDependency>();
      BOOST_TEST(inst2 != nullptr);
      BOOST_TEST(inst1 == inst2);
      inst2->str() = "instance2";
      BOOST_TEST(inst1->str() == "instance2");
      BOOST_TEST(inst2->str() == "instance2");
    }
    BOOST_TEST(Dependency1::dtorCalls == 0);
  }
  BOOST_TEST(Dependency1::dtorCalls == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SharedPtrUnique)
{
  Dependency1::dtorCalls = 0;
  std::shared_ptr<IDependency> inst1;
  std::shared_ptr<IDependency> inst2;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::UniqueScope>();
    inst1 = container.createShared<IDependency>();
    BOOST_TEST(inst1 != nullptr);
    inst1->str() = "instance1";
    inst2 = container.createShared<IDependency>();
    BOOST_TEST(inst2 != nullptr);
    BOOST_TEST(inst1 != inst2);
    inst2->str() = "instance2";
    BOOST_TEST(inst1->str() == "instance1");
    BOOST_TEST(inst2->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(UniquePtrShared)
{
  Dependency1::dtorCalls = 0;
  std::unique_ptr<IDependency> inst1;
  std::unique_ptr<IDependency> inst2;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::SharedScope>();
    inst1 = container.createUnique<IDependency>();
    BOOST_TEST(inst1 != nullptr);
    inst1->str() = "instance1";
    inst2 = container.createUnique<IDependency>();
    BOOST_TEST(inst2 != nullptr);
    BOOST_TEST(inst1 != inst2);
    inst2->str() = "instance2";
    BOOST_TEST(inst1->str() == "instance1");
    BOOST_TEST(inst2->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(UniquePtrUnique)
{
  Dependency1::dtorCalls = 0;
  std::unique_ptr<IDependency> inst1;
  std::unique_ptr<IDependency> inst2;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::UniqueScope>();
    inst1 = container.createUnique<IDependency>();
    BOOST_TEST(inst1 != nullptr);
    inst1->str() = "instance1";
    inst2 = container.createUnique<IDependency>();
    BOOST_TEST(inst2 != nullptr);
    BOOST_TEST(inst1 != inst2);
    inst2->str() = "instance2";
    BOOST_TEST(inst1->str() == "instance1");
    BOOST_TEST(inst2->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(RefShared)
{
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<Dependency1, di::SharedScope>();
    const Dependency1& inst1 = container.create<const Dependency1&>();
    const Dependency1& inst2 = container.create<const Dependency1&>();
    BOOST_TEST(&inst1 == &inst2);
  }
  BOOST_TEST(Dependency1::dtorCalls == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(RefUnique)
{
  Dependency1::dtorCalls = 0;
  try
  {
    di::Container container;
    {      
      container.add<Dependency1, di::UniqueScope>();
      container.create<const Dependency1&>();
      BOOST_TEST(false);
    }
  }
  catch(...)
  {
    BOOST_TEST(true);
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(CopyShared)
{
  try
  {
    di::Container container;
    container.add<Dependency1, di::SharedScope>();
    Dependency1 inst = container.create<Dependency1>();
    BOOST_TEST(false);
  }
  catch(...)
  {
    BOOST_TEST(true);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(CopyUnique)
{
  di::Container container;
  container.add<Dependency1, di::UniqueScope>();
  Dependency1 inst1 = container.create<Dependency1>();
  Dependency1 inst2 = container.create<Dependency1>();
  inst1.str() = "instance1";
  inst2.str() = "instance2";
  BOOST_TEST(inst1.str() == "instance1");
  BOOST_TEST(inst2.str() == "instance2");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(PurePtrSharedChain)
{
  PurePtrDependant* inst1 = nullptr;
  PurePtrDependant* inst2 = nullptr;
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::SharedScope>();
    container.add<PurePtrDependant>();
    inst1 = container.createPtr<PurePtrDependant>();
    BOOST_TEST(inst1->dependency() != nullptr);
    inst1->dependency()->str() = "instance1";
    inst2 = container.createPtr<PurePtrDependant>();
    BOOST_TEST(inst2->dependency() != nullptr);
    BOOST_TEST(inst1->dependency() == inst2->dependency());
    inst2->dependency()->str() = "instance2";
    BOOST_TEST(inst1->dependency()->str() == "instance2");
    BOOST_TEST(inst2->dependency()->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 1);
  delete inst1;
  delete inst2; 
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(PurePtrUniqueChain)
{
  PurePtrDependant* inst1 = nullptr;
  PurePtrDependant* inst2 = nullptr;
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::UniqueScope>();
    container.add<PurePtrDependant>();
    inst1 = container.createPtr<PurePtrDependant>();
    BOOST_TEST(inst1->dependency() != nullptr);
    inst1->dependency()->str() = "instance1";
    inst2 = container.createPtr<PurePtrDependant>();
    BOOST_TEST(inst2->dependency() != nullptr);
    BOOST_TEST(inst1->dependency() != inst2->dependency());
    inst2->dependency()->str() = "instance2";
    BOOST_TEST(inst1->dependency()->str() == "instance1");
    BOOST_TEST(inst2->dependency()->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
  delete inst1->dependency();
  delete inst2->dependency();
  delete inst1;
  delete inst2; 
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SharedPtrSharedChain)
{
  SharedPtrDependant* inst1 = nullptr;
  SharedPtrDependant* inst2 = nullptr;
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::SharedScope>();
    container.add<SharedPtrDependant>();
    inst1 = container.createPtr<SharedPtrDependant>();
    BOOST_TEST(inst1->dependency() != nullptr);
    inst1->dependency()->str() = "instance1";
    inst2 = container.createPtr<SharedPtrDependant>();
    BOOST_TEST(inst2->dependency() != nullptr);
    BOOST_TEST(inst1->dependency() == inst2->dependency());
    inst2->dependency()->str() = "instance2";
    BOOST_TEST(inst1->dependency()->str() == "instance2");
    BOOST_TEST(inst2->dependency()->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
  delete inst1;
  delete inst2;
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SharedPtrUniqueChain)
{
  SharedPtrDependant* inst1 = nullptr;
  SharedPtrDependant* inst2 = nullptr;
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::UniqueScope>();
    container.add<SharedPtrDependant>();
    inst1 = container.createPtr<SharedPtrDependant>();
    BOOST_TEST(inst1->dependency() != nullptr);
    inst1->dependency()->str() = "instance1";
    inst2 = container.createPtr<SharedPtrDependant>();
    BOOST_TEST(inst2->dependency() != nullptr);
    BOOST_TEST(inst1->dependency() != inst2->dependency());
    inst2->dependency()->str() = "instance2";
    BOOST_TEST(inst1->dependency()->str() == "instance1");
    BOOST_TEST(inst2->dependency()->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
  delete inst1;
  delete inst2;
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(UniquePtrSharedChain)
{
  UniquePtrDependant* inst1 = nullptr;
  UniquePtrDependant* inst2 = nullptr;
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::SharedScope>();
    container.add<UniquePtrDependant>();
    inst1 = container.createPtr<UniquePtrDependant>();
    BOOST_TEST(inst1->dependency() != nullptr);
    inst1->dependency()->str() = "instance1";
    inst2 = container.createPtr<UniquePtrDependant>();
    BOOST_TEST(inst2->dependency() != nullptr);
    BOOST_TEST(inst1->dependency() != inst2->dependency());
    inst2->dependency()->str() = "instance2";
    BOOST_TEST(inst1->dependency()->str() == "instance1");
    BOOST_TEST(inst2->dependency()->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
  delete inst1;
  delete inst2;
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(UniquePtrUniqueChain)
{
  UniquePtrDependant* inst1 = nullptr; 
  UniquePtrDependant* inst2 = nullptr;
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<IDependency, Dependency1, di::UniqueScope>();
    container.add<UniquePtrDependant>();
    inst1 = container.createPtr<UniquePtrDependant>();
    BOOST_TEST(inst1->dependency() != nullptr);
    inst1->dependency()->str() = "instance1";
    inst2 = container.createPtr<UniquePtrDependant>();
    BOOST_TEST(inst2->dependency() != nullptr);
    BOOST_TEST(inst1->dependency() != inst2->dependency());
    inst2->dependency()->str() = "instance2";
    BOOST_TEST(inst1->dependency()->str() == "instance1");
    BOOST_TEST(inst2->dependency()->str() == "instance2");
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
  delete inst1;
  delete inst2;
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(CopyUniqueChain)
{
  di::Container container;
  container.add<Dependency1, di::UniqueScope>();
  container.add<CopyDependant>();
  auto inst1 = container.createShared<CopyDependant>();
  inst1->str() = "instance1";
  auto inst2 = container.createShared<CopyDependant>();
  inst2->str() = "instance2";
  BOOST_TEST(inst1->str() == "instance1");
  BOOST_TEST(inst2->str() == "instance2");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(CopySharedChain)
{
  try
  {
    di::Container container;
    container.add<Dependency1, di::SharedScope>();
    container.add<CopyDependant>();
    auto inst = container.createShared<CopyDependant>();
    BOOST_TEST(false);
  }
  catch (...)
  {
    BOOST_TEST(true);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ConstPtrChain)
{
  Dependency1::dtorCalls = 0;
  const IDependency* dep1 = nullptr;
  const IDependency* dep2 = nullptr;
  {
    di::Container container;
    container.add<IDependency, Dependency1>();
    container.add<ConstPtrDependant>();
    auto inst1 = container.createShared<ConstPtrDependant>();
    auto inst2 = container.createShared<ConstPtrDependant>();
    BOOST_TEST(inst1->dependency() != nullptr);
    BOOST_TEST(inst2->dependency() != nullptr);
    BOOST_TEST(inst1->dependency() != inst2->dependency());
    dep1 = inst1->dependency();
    dep2 = inst2->dependency();
  }
  BOOST_TEST(Dependency1::dtorCalls == 0);
  delete dep1;
  delete dep2;
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ConstSharedPtrChain)
{
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<IDependency, Dependency1>();
    container.add<ConstSharedPtrDependant>();
    auto inst1 = container.createShared<ConstSharedPtrDependant>();
    auto inst2 = container.createShared<ConstSharedPtrDependant>();
    BOOST_TEST(inst1->dependency() != nullptr);
    BOOST_TEST(inst2->dependency() != nullptr);
    BOOST_TEST(inst1->dependency() != inst2->dependency());
  }
  BOOST_TEST(Dependency1::dtorCalls == 2);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(ConstUniquePtrChain)
{
  Dependency1::dtorCalls = 0;
  {
    di::Container container;
    container.add<IDependency, Dependency1>();
    container.add<ConstUniquePtrDependant>();
    auto inst1 = container.createShared<ConstUniquePtrDependant>();
    auto inst2 = container.createShared<ConstUniquePtrDependant>();
    BOOST_TEST(inst1->dependency() != nullptr);
    BOOST_TEST(inst2->dependency() != nullptr);
    BOOST_TEST(inst1->dependency() != inst2->dependency());
  }
  BOOST_TEST(Dependency1::dtorCalls == 2);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(InitMethodPtrUnique)
{
  di::Container container;
  container.add<InitDependency, di::UniqueScope, true>();
  auto d1 = container.createShared<InitDependency>();
  BOOST_TEST(d1->counter() == 1);
  auto d2 = container.createShared<InitDependency>();
  BOOST_TEST(d1 != d2);
  BOOST_TEST(d2->counter() == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DisableInitMethodPtrUnique)
{
  di::Container container;
  container.add<InitDependency, di::UniqueScope, false>();
  auto d1 = container.createShared<InitDependency>();
  BOOST_TEST(d1->counter() == 0);
  auto d2 = container.createShared<InitDependency>();
  BOOST_TEST(d1 != d2);
  BOOST_TEST(d2->counter() == 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(InitMethodPtrShared)
{
  di::Container container;
  container.add<InitDependency, di::SharedScope, true>();
  auto d1 = container.createShared<InitDependency>();
  BOOST_TEST(d1->counter() == 1);
  auto d2 = container.createShared<InitDependency>();
  BOOST_TEST(d1 == d2);
  BOOST_TEST(d2->counter() == 1);  
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DisableInitMethodPtrShared)
{
  di::Container container;
  container.add<InitDependency, di::SharedScope, false>();
  auto d1 = container.createShared<InitDependency>();
  BOOST_TEST(d1->counter() == 0);
  auto d2 = container.createShared<InitDependency>();
  BOOST_TEST(d1 == d2);
  BOOST_TEST(d2->counter() == 0);  
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(InitMethodCopy)
{
  di::Container container;
  container.add<InitDependency, di::UniqueScope, true>();
  InitDependency d1 = container.create<InitDependency>();
  BOOST_TEST(d1.counter() == 1);
  InitDependency d2 = container.create<InitDependency>();
  BOOST_TEST(d2.counter() == 1);  
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DisableInitMethodCopy)
{
  di::Container container;
  container.add<InitDependency, di::UniqueScope, false>();
  InitDependency d1 = container.create<InitDependency>();
  BOOST_TEST(d1.counter() == 0);
  InitDependency d2 = container.create<InitDependency>();
  BOOST_TEST(d2.counter() == 0);  
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(InitMethodRef)
{
  di::Container container;
  container.add<InitDependency, di::SharedScope, true>();
  const InitDependency& d1 = container.create<const InitDependency&>();
  BOOST_TEST(d1.counter() == 1);
  const InitDependency& d2 = container.create<const InitDependency&>();
  BOOST_TEST(d1.counter() == 1);
  BOOST_TEST(d2.counter() == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DisableInitMethodRef)
{
  di::Container container;
  container.add<InitDependency, di::SharedScope, false>();
  const InitDependency& d1 = container.create<const InitDependency&>();
  BOOST_TEST(d1.counter() == 0);
  const InitDependency& d2 = container.create<const InitDependency&>();
  BOOST_TEST(d1.counter() == 0);
  BOOST_TEST(d2.counter() == 0);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DoubleDependencySharedImpl)
{
  di::Container container;
  container.add<IDependency,  DoubleDependency, di::SharedImlpScope>();
  container.add<IDependency2, DoubleDependency, di::SharedImlpScope>();
  auto d1 = container.createShared<IDependency>();
  auto d2 = container.createShared<IDependency2>();
  auto dd1 = std::dynamic_pointer_cast<DoubleDependency>(d1);
  auto dd2 = std::dynamic_pointer_cast<DoubleDependency>(d2);
  BOOST_TEST(dd1 != nullptr);
  BOOST_TEST(dd2 != nullptr);
  BOOST_TEST(dd1 == dd2);
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T* getElementPtr(std::vector<IDependency*>& vector)
{
  T* element = dynamic_cast<T*>(vector[0]);
  return element ? element : dynamic_cast<T*>(vector[1]);
};

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(VectorPurePtrUnique)
{
  Dependency1* inst1 = nullptr;
  Dependency2* inst2 = nullptr;
  Dependency1* inst3 = nullptr;
  Dependency2* inst4 = nullptr;
  {
    di::Container container;
    container.addMulti<IDependency, Dependency1>();
    container.addMulti<IDependency, Dependency2>();
    auto vector1 = container.create<std::vector<IDependency*>>();
    BOOST_TEST(vector1.size() == 2);
    inst1 = getElementPtr<Dependency1>(vector1);
    inst2 = getElementPtr<Dependency2>(vector1);
    BOOST_TEST(inst1 != nullptr);
    BOOST_TEST(inst2 != nullptr);
    inst1->str() = "instance1";
    inst2->str() = "instance2";
    auto vector2 = container.create<std::vector<IDependency*>>();
    BOOST_TEST(vector2.size() == 2);
    inst3 = getElementPtr<Dependency1>(vector2);
    inst4 = getElementPtr<Dependency2>(vector2);
    BOOST_TEST(inst3 != nullptr);
    BOOST_TEST(inst4 != nullptr);
    inst3->str() = "instance3";
    inst4->str() = "instance4";
    BOOST_TEST(inst1->str() == "instance1");
    BOOST_TEST(inst2->str() == "instance2");
    BOOST_TEST(inst3->str() == "instance3");
    BOOST_TEST(inst4->str() == "instance4");
  }
  delete inst1;
  delete inst2;
  delete inst3;
  delete inst4;
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
std::shared_ptr<T> getElementSPtr(std::vector<std::shared_ptr<IDependency>>& vector)
{
  std::shared_ptr<T> element = std::dynamic_pointer_cast<T>(vector[0]);
  return element ? element : std::dynamic_pointer_cast<T>(vector[1]);
};

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(VectorSharedPtrUnique)
{
  di::Container container;
  container.addMulti<IDependency, Dependency1>();
  container.addMulti<IDependency, Dependency2>();
  auto vector1 = container.create<std::vector<std::shared_ptr<IDependency>>>();
  BOOST_TEST(vector1.size() == 2);
  auto inst1 = getElementSPtr<Dependency1>(vector1);
  auto inst2 = getElementSPtr<Dependency2>(vector1);
  BOOST_TEST(inst1 != nullptr);
  BOOST_TEST(inst2 != nullptr);
  inst1->str() = "instance1";
  inst2->str() = "instance2";
  auto vector2 = container.create<std::vector<std::shared_ptr<IDependency>>>();
  BOOST_TEST(vector2.size() == 2);
  auto inst3 = getElementSPtr<Dependency1>(vector2);
  auto inst4 = getElementSPtr<Dependency2>(vector2);
  BOOST_TEST(inst3 != nullptr);
  BOOST_TEST(inst4 != nullptr);
  inst3->str() = "instance3";
  inst4->str() = "instance4";
  BOOST_TEST(inst1->str() == "instance1");
  BOOST_TEST(inst2->str() == "instance2");
  BOOST_TEST(inst3->str() == "instance3");
  BOOST_TEST(inst4->str() == "instance4");
}

// -----------------------------------------------------------------------------------------------------------------------------
template <typename T>
T* getElementUPtr(std::vector<std::unique_ptr<IDependency>>& vector)
{
  T* element = dynamic_cast<T*>(vector[0].get());
  return element ? element : dynamic_cast<T*>(vector[1].get());
};

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(VectorUniquePtrUnique)
{
  di::Container container;
  container.addMulti<IDependency, Dependency1>();
  container.addMulti<IDependency, Dependency2>();
  auto vector1 = container.create<std::vector<std::unique_ptr<IDependency>>>();
  BOOST_TEST(vector1.size() == 2);
  auto inst1 = getElementUPtr<Dependency1>(vector1);
  auto inst2 = getElementUPtr<Dependency2>(vector1);
  BOOST_TEST(inst1 != nullptr);
  BOOST_TEST(inst2 != nullptr);
  inst1->str() = "instance1";
  inst2->str() = "instance2";
  auto vector2 = container.create<std::vector<std::unique_ptr<IDependency>>>();
  BOOST_TEST(vector2.size() == 2);
  auto inst3 = getElementUPtr<Dependency1>(vector2);
  auto inst4 = getElementUPtr<Dependency2>(vector2);
  BOOST_TEST(inst3 != nullptr);
  BOOST_TEST(inst4 != nullptr);
  BOOST_TEST(inst3 != inst1);
  BOOST_TEST(inst4 != inst2);
  inst3->str() = "instance3";
  inst4->str() = "instance4";
  BOOST_TEST(inst1->str() == "instance1");
  BOOST_TEST(inst2->str() == "instance2");
  BOOST_TEST(inst3->str() == "instance3");
  BOOST_TEST(inst4->str() == "instance4");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(VectorPurePtrShared)
{
  Dependency1* inst1 = nullptr;
  Dependency2* inst2 = nullptr;
  Dependency1* inst3 = nullptr;
  Dependency2* inst4 = nullptr;
  {
    di::Container container;
    container.addMulti<IDependency, Dependency1, di::SharedScope>();
    container.addMulti<IDependency, Dependency2, di::SharedScope>();
    auto vector1 = container.create<std::vector<IDependency*>>();
    BOOST_TEST(vector1.size() == 2);
    inst1 = getElementPtr<Dependency1>(vector1);
    inst2 = getElementPtr<Dependency2>(vector1);
    BOOST_TEST(inst1 != nullptr);
    BOOST_TEST(inst2 != nullptr);
    inst1->str() = "instance1";
    inst2->str() = "instance2";
    auto vector2 = container.create<std::vector<IDependency*>>();
    BOOST_TEST(vector2.size() == 2);
    inst3 = getElementPtr<Dependency1>(vector2);
    inst4 = getElementPtr<Dependency2>(vector2);
    BOOST_TEST(inst3 == inst1);
    BOOST_TEST(inst4 == inst2);
    inst3->str() = "instance3";
    inst4->str() = "instance4";
    BOOST_TEST(inst1->str() == "instance3");
    BOOST_TEST(inst2->str() == "instance4");
    BOOST_TEST(inst3->str() == "instance3");
    BOOST_TEST(inst4->str() == "instance4");
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(VectorSharedPtrShared)
{
  di::Container container;
  container.addMulti<IDependency, Dependency1, di::SharedScope>();
  container.addMulti<IDependency, Dependency2, di::SharedScope>();
  auto vector1 = container.create<std::vector<std::shared_ptr<IDependency>>>();
  BOOST_TEST(vector1.size() == 2);
  auto inst1 = getElementSPtr<Dependency1>(vector1);
  auto inst2 = getElementSPtr<Dependency2>(vector1);
  BOOST_TEST(inst1 != nullptr);
  BOOST_TEST(inst2 != nullptr);
  inst1->str() = "instance1";
  inst2->str() = "instance2";
  auto vector2 = container.create<std::vector<std::shared_ptr<IDependency>>>();
  BOOST_TEST(vector2.size() == 2);
  auto inst3 = getElementSPtr<Dependency1>(vector2);
  auto inst4 = getElementSPtr<Dependency2>(vector2);
  BOOST_TEST(inst3 != nullptr);
  BOOST_TEST(inst4 != nullptr);
  inst3->str() = "instance3";
  inst4->str() = "instance4";
  BOOST_TEST(inst1->str() == "instance3");
  BOOST_TEST(inst2->str() == "instance4");
  BOOST_TEST(inst3->str() == "instance3");
  BOOST_TEST(inst4->str() == "instance4");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(VectorUniquePtrShared)
{
  di::Container container;
  container.addMulti<IDependency, Dependency1, di::SharedScope>();
  container.addMulti<IDependency, Dependency2, di::SharedScope>();
  auto vector1 = container.create<std::vector<std::unique_ptr<IDependency>>>();
  BOOST_TEST(vector1.size() == 2);
  auto inst1 = getElementUPtr<Dependency1>(vector1);
  auto inst2 = getElementUPtr<Dependency2>(vector1);
  BOOST_TEST(inst1 != nullptr);
  BOOST_TEST(inst2 != nullptr);
  inst1->str() = "instance1";
  inst2->str() = "instance2";
  auto vector2 = container.create<std::vector<std::unique_ptr<IDependency>>>();
  BOOST_TEST(vector2.size() == 2);
  auto inst3 = getElementUPtr<Dependency1>(vector2);
  auto inst4 = getElementUPtr<Dependency2>(vector2);
  BOOST_TEST(inst3 != nullptr);
  BOOST_TEST(inst4 != nullptr);
  inst3->str() = "instance3";
  inst4->str() = "instance4";
  BOOST_TEST(inst1->str() == "instance1");
  BOOST_TEST(inst2->str() == "instance2");
  BOOST_TEST(inst3->str() == "instance3");
  BOOST_TEST(inst4->str() == "instance4");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(VectorDependency)
{
  di::Container container;
  container.addMulti<IDependency, Dependency1, di::SharedScope>();
  container.addMulti<IDependency, Dependency2, di::SharedScope>();
  container.add<IDependency, Dependency3, di::SharedScope>();
  container.add<VectorDependant, di::UniqueScope>();
  auto vector = container.create<std::vector<std::shared_ptr<IDependency>>>();
  vector[0]->str() = "instance1";
  vector[1]->str() = "instance2";
  container.create<std::shared_ptr<IDependency>>();
  container.createPtr<IDependency>()->str() = "instance3";
  auto d = container.createShared<VectorDependant>();
  BOOST_TEST(d->dependencies()[0]->str() == "instance1");
  BOOST_TEST(d->dependencies()[1]->str() == "instance2");
  BOOST_TEST(d->str() == "instance3");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(VectorRefDependency)
{
  di::Container container;
  container.addMulti<IDependency, Dependency1, di::SharedScope>();
  container.addMulti<IDependency, Dependency2, di::SharedScope>();
  container.add<IDependency, Dependency3, di::SharedScope>();
  container.add<VectorRefDependant>();
  auto vector = container.create<std::vector<std::shared_ptr<IDependency>>>();
  vector[0]->str() = "instance1";
  vector[1]->str() = "instance2";
  container.create<std::shared_ptr<IDependency>>();
  container.createPtr<IDependency>()->str() = "instance3";
  auto d = container.createShared<VectorRefDependant>();
  BOOST_TEST(d->dependencies()[0]->str() == "instance1");
  BOOST_TEST(d->dependencies()[1]->str() == "instance2");
  BOOST_TEST(d->str() == "instance3");
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(AddInstance)
{
  di::Container container;
  auto ints1 = std::make_shared<Dependency3>();
  ints1->str() = "instance1";
  container.add<IDependency, Dependency3>(ints1);
  auto inst2 = container.createShared<IDependency>();
  BOOST_TEST(ints1 == inst2);
  auto inst3 = container.createPtr<IDependency>();
  BOOST_TEST(ints1.get() == inst3);
  auto inst4 = container.createUnique<IDependency>();
  BOOST_TEST(ints1.get() != inst4.get());
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DiDependantPtrTest)
{
  di::Container container;
  container.add<DiDependant>();
  auto ints1 = container.createShared<DiDependant>();
  BOOST_TEST(ints1->container() == &container);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DiDependantRefTest)
{
  di::Container container;
  container.add<DiDependantRef>();
  auto ints1 = container.createShared<DiDependantRef>();
  di::Container& ctr = ints1->container();
  BOOST_TEST(&ctr == &container);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(DiDependantRefTest1)
{
  di::Container container;
  container.add<DiDependantRef>();
  auto ints1 = container.createShared<DiDependantRef>();
  di::Container& ctr = ints1->container();
  BOOST_TEST(&ctr == &container);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(FactoryPurePureShared)
{
  di::Container container;
  container.add<FactoryResultPure, di::SharedScope>();
  container.add<FactoryArg1, di::SharedScope>();
  container.add<FactoryArg2, di::SharedScope>();
  auto arg1 = container.createShared<FactoryArg1>();
  auto arg2 = container.createShared<FactoryArg2>();
  auto arg3 = std::make_shared<FactoryArg3>();
  arg1->value = 1;
  arg2->value = 2;
  arg3->value = 3;  
  using Factory = std::function<FactoryResultPure*(FactoryArg3* arg3)>;
  auto factory = container.create<Factory>();
  auto result = factory(arg3.get());
  BOOST_TEST(result->arg1->value == 1);
  BOOST_TEST(result->arg2->value == 2);
  BOOST_TEST(result->arg3->value == 3);
  auto result2 = container.createShared<FactoryResultPure>();
  BOOST_TEST(result2->arg1->value == 1);
  BOOST_TEST(result2->arg2->value == 2);
  BOOST_TEST(result2->arg3->value == 3);
  auto result3 = container.createPtr<FactoryResultPure>();
  BOOST_TEST(result3->arg1->value == 1);
  BOOST_TEST(result3->arg2->value == 2);
  BOOST_TEST(result3->arg3->value == 3);
  try
  {
    container.createUnique<FactoryResultPure>();
    BOOST_TEST(false);
  }
  catch(...)
  {
    BOOST_TEST(true);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(FactoryPureSharedShared)
{
  di::Container container;
  container.add<FactoryResultPure, di::SharedScope>();
  container.add<FactoryArg1, di::SharedScope>();
  container.add<FactoryArg2, di::SharedScope>();
  auto arg1 = container.createShared<FactoryArg1>();
  auto arg2 = container.createShared<FactoryArg2>();
  auto arg3 = std::make_shared<FactoryArg3>();
  arg1->value = 1;
  arg2->value = 2;
  arg3->value = 3;  
  using Factory = std::function<std::shared_ptr<FactoryResultPure>(FactoryArg3* arg3)>;
  auto factory = container.create<Factory>();
  auto result = factory(arg3.get());
  BOOST_TEST(result->arg1->value == 1);
  BOOST_TEST(result->arg2->value == 2);
  BOOST_TEST(result->arg3->value == 3);
  auto result2 = container.createShared<FactoryResultPure>();
  BOOST_TEST(result2->arg1->value == 1);
  BOOST_TEST(result2->arg2->value == 2);
  BOOST_TEST(result2->arg3->value == 3);
  auto result3 = container.createPtr<FactoryResultPure>();
  BOOST_TEST(result3->arg1->value == 1);
  BOOST_TEST(result3->arg2->value == 2);
  BOOST_TEST(result3->arg3->value == 3);
  try
  {
    container.createUnique<FactoryResultPure>();
    BOOST_TEST(false);
  }
  catch(...)
  {
    BOOST_TEST(true);
  }
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(FactoryPureSharedUnique)
{
  di::Container container;
  container.add<FactoryResultPure, di::UniqueScope>();
  container.add<FactoryArg1, di::SharedScope>();
  container.add<FactoryArg2, di::SharedScope>();
  auto arg1 = container.createShared<FactoryArg1>();
  auto arg2 = container.createShared<FactoryArg2>();
  auto arg3 = std::make_shared<FactoryArg3>();
  arg1->value = 1;
  arg2->value = 2;
  arg3->value = 3;  
  using Factory = std::function<std::unique_ptr<FactoryResultPure>(FactoryArg3* arg3)>;
  auto factory = container.create<Factory>();
  auto result = factory(arg3.get());
  BOOST_TEST(result->arg1->value == 1);
  BOOST_TEST(result->arg2->value == 2);
  BOOST_TEST(result->arg3->value == 3);
  try
  {
    container.createShared<FactoryResultPure>();
    BOOST_TEST(false);
  }
  catch(...) {}
  try
  {
    container.createPtr<FactoryResultPure>();
    BOOST_TEST(false);
  }
  catch(...) {}
  try
  {
    container.createUnique<FactoryResultPure>();
    BOOST_TEST(false);
  }
  catch(...) {}
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(FactoryUnique)
{
  di::Container container;
  container.add<FactoryResultUnique, di::SharedScope>();
  container.add<FactoryArg1, di::UniqueScope>();
  container.add<FactoryArg2, di::UniqueScope>();
  auto arg3 = std::make_unique<FactoryArg3>();
  arg3->value = 3;  
  using Factory = std::function<FactoryResultUnique*(std::unique_ptr<FactoryArg3> arg3)>;
  auto factory = container.create<Factory>();
  auto result = factory(std::move(arg3));
  BOOST_TEST(result->arg1->value == 0);
  BOOST_TEST(result->arg2->value == 0);
  BOOST_TEST(result->arg3->value == 3);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(FactoryShared)
{
  di::Container container;
  container.add<FactoryResultShared, di::SharedScope>();
  container.add<FactoryArg1, di::SharedScope>();
  container.add<FactoryArg2, di::SharedScope>();
  auto arg1 = container.createShared<FactoryArg1>();
  auto arg2 = container.createShared<FactoryArg2>();
  auto arg3 = std::make_shared<FactoryArg3>();
  arg1->value = 1;
  arg2->value = 2;
  arg3->value = 3;  
  using Factory = std::function<FactoryResultShared*(std::shared_ptr<FactoryArg3> arg3)>;
  auto factory = container.create<Factory>();
  auto result = factory(arg3);
  BOOST_TEST(result->arg1->value == 1);
  BOOST_TEST(result->arg2->value == 2);
  BOOST_TEST(result->arg3->value == 3);
  auto result2 = container.createShared<FactoryResultShared>();
  BOOST_TEST(result2->arg1->value == 1);
  BOOST_TEST(result2->arg2->value == 2);
  BOOST_TEST(result2->arg3->value == 3);
  auto result3 = container.createPtr<FactoryResultShared>();
  BOOST_TEST(result3->arg1->value == 1);
  BOOST_TEST(result3->arg2->value == 2);
  BOOST_TEST(result3->arg3->value == 3);
  try
  {
    container.createUnique<FactoryResultShared>();
    BOOST_TEST(false);
  }
  catch(...)
  {
    BOOST_TEST(true);
  }
}
struct kek
{
  int boo(double f, std::string qwe) const
  {
    return 0;
  }
};

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(UniqueFactory)
{
  di::Container container;
  int depCounter = 0;
  container.addFactory<IDependency, di::UniqueScope>([&depCounter]() {
    ++depCounter;
    return new Dependency1();
  });
  auto inst1 = container.createShared<IDependency>();
  auto inst2 = container.createShared<IDependency>();
  BOOST_TEST(depCounter == 2);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(SharedFactory)
{
  di::Container container;
  int depCounter = 0;
  container.addFactory<IDependency, di::SharedScope>([&depCounter]() {
    ++depCounter;
    return new Dependency1();
  });
  auto inst1 = container.createShared<IDependency>();
  auto inst2 = container.createShared<IDependency>();
  BOOST_TEST(depCounter == 1);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(FactoryDependency)
{
  di::Container container;
  container.addFactory<IDependency, di::SharedScope>([]() {
    return new Dependency1();
  });
  container.add<SharedPtrDependant>();
  auto inst = container.createShared<SharedPtrDependant>();
  BOOST_TEST(inst != nullptr);
  BOOST_TEST(inst->dependency() != nullptr);
}

// -----------------------------------------------------------------------------------------------------------------------------
BOOST_AUTO_TEST_CASE(FactoryChain)
{
  di::Container container;
  container.addFactory<IDependency, di::SharedScope>([](di::Container*) {
    return new Dependency1();
  });
  container.addFactory<di::SharedScope>([](std::shared_ptr<IDependency> d) {
    return new SharedPtrDependant(d);
  });
  auto inst = container.createShared<SharedPtrDependant>();
  BOOST_TEST(inst != nullptr);
  BOOST_TEST(inst->dependency() != nullptr);
}

BOOST_AUTO_TEST_SUITE_END() // !DiTest

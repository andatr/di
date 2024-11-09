#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "di/di.h"

namespace yaga {

// -----------------------------------------------------------------------------------------------------------------------------
class IDatabase
{
public:
  virtual ~IDatabase() {}
  virtual int getUser() = 0;
  virtual int getContract() = 0;
};

using DatabaseSPtr = std::shared_ptr<IDatabase>;

// -----------------------------------------------------------------------------------------------------------------------------
class MySQLDatabase : public IDatabase
{
public:
  MySQLDatabase() : 
    lastUserId_(10),
    lastContractId_(198)
  {}

  void init() 
  {
    std::cout << "Connected to MySQL database.\n";
  }
  
  int getUser() override
  {
    return lastUserId_++;
  }
  
  int getContract() override
  {
    return lastContractId_++;
  }

private:
  int lastUserId_;
  int lastContractId_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class ILogger
{
public:
  virtual ~ILogger() {}
  virtual void log(const std::string& message) = 0;
};

using LoggerSPtr = std::shared_ptr<ILogger>;

// -----------------------------------------------------------------------------------------------------------------------------
class ConsoleLogger : public ILogger
{
public:
  void log(const std::string& message) override
  {
    std::cout << "[LOG] " << message << "\n";
  }
};

// -----------------------------------------------------------------------------------------------------------------------------
class IService
{
public:
  virtual ~IService() {}
  virtual void doWork() = 0;
};

using ServiceUPtr = std::unique_ptr<IService>;

// -----------------------------------------------------------------------------------------------------------------------------
class Invoice
{
public:
  Invoice(int user, LoggerSPtr logger):
    user_(user),
    logger_(logger)
  {}

  void send()
  {
    logger_->log(std::string("Sending invoice to user ") + std::to_string(user_));
  }

private:
  int user_;
  LoggerSPtr logger_;
};

using InvoiceUPtr = std::unique_ptr<Invoice>;
using InvoiceFactory = std::function<InvoiceUPtr(int)>;

// -----------------------------------------------------------------------------------------------------------------------------
class UserService : public IService
{
public:
  UserService(DatabaseSPtr database, LoggerSPtr logger,
    InvoiceFactory invoiceFactory) :
    database_(database),
    logger_(logger),
    invoiceFactory_(invoiceFactory)
  {}

  void doWork() override
  {
    for (int i = 0; i < 10; ++i) {
      auto user = database_->getUser();
      std::cout << "UserService processing user " << user << ".\n";
      auto invoice = invoiceFactory_(user);
      invoice->send();
      logger_->log(std::string("Processed user ") + std::to_string(user));
    }
  }

private:
  DatabaseSPtr database_;
  LoggerSPtr logger_;
  InvoiceFactory invoiceFactory_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class ContractService : public IService
{
public:
  ContractService(DatabaseSPtr database, LoggerSPtr logger) :
    database_(database),
    logger_(logger)
  {}

  void doWork() override
  {
    for (int i = 0; i < 10; ++i) {
      auto contract = database_->getContract();
      std::cout << "ContractService processing contracts " << contract << ".\n";
      logger_->log(std::string("Processed contract ") + std::to_string(contract));
    }
  }

private:
  DatabaseSPtr database_;
  LoggerSPtr logger_;
};

// -----------------------------------------------------------------------------------------------------------------------------
class Application
{
public:
  Application(std::vector<ServiceUPtr> services) :
    services_(std::move(services))
  {
  }

  void run()
  {
    for (auto& service: services_) {
      service->doWork();
    }
  }

private:
  std::vector<ServiceUPtr> services_;
};

// -----------------------------------------------------------------------------------------------------------------------------
void main()
{
  di::Container container;
  container.add<IDatabase, MySQLDatabase, di::SharedScope, true>();
  container.add<ILogger, ConsoleLogger, di::SharedScope>();
  container.add<Invoice, di::UniqueScope>();
  container.addMulti<IService, UserService, di::UniqueScope>();
  container.addMulti<IService, ContractService, di::UniqueScope>();
  container.add<Application, di::UniqueScope>();

  auto app = container.createUnique<Application>();
  app->run();
}

} // !namespace yaga

// -----------------------------------------------------------------------------------------------------------------------------
int main(int, char**)
{
  yaga::main();
  return 0;
}
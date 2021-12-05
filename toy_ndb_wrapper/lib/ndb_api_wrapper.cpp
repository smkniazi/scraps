#include <ndb_api_wrapper.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <iterator>
#include <NdbApi.hpp>

Ndb_cluster_connection *ndb_connection;
Ndb *ndb_object;

inline bool on_error(const struct NdbError &error, const char *explanation);

struct BasicRow
{
  int attr1, attr2;
};

void hello_world()
{
  std::cout << "Hello World!" << std::endl;
}

int nomain(int argc, char **argv)
{
  if (argc != 2)
  {
    std::cout << "Usage: "
              << "prog <connectstring>" << std::endl;
    return EXIT_FAILURE;
  }

  const char *connectstring = argv[1];
  if (!initialize(connectstring))
  {
    return EXIT_FAILURE;
  }

  some_action();

  return EXIT_SUCCESS;
}

bool do_write(long long key, long long value)
{
  const NdbDictionary::Dictionary *dict = ndb_object->getDictionary();
  const NdbDictionary::Table *table = dict->getTable("test_table");

  if (table == nullptr)
    return on_error(dict->getNdbError(),
                    "Failed to access 'test_db.test_table'");

  // The write will be performed within single transaction
  NdbTransaction *transaction = ndb_object->startTransaction(table);
  if (transaction == nullptr)
    return on_error(ndb_object->getNdbError(), "Failed to start transaction");

  NdbOperation *operation = transaction->getNdbOperation(table);
  if (operation == nullptr)
    return on_error(transaction->getNdbError(),
                    "Failed to start write operation");

  operation->writeTuple();
  operation->equal("ATTR1", key);
  operation->setValue("ATTR2", value);

  if (transaction->execute(NdbTransaction::Commit) != 0)
    return on_error(transaction->getNdbError(),
                    "Failed to execute transaction");

  ndb_object->closeTransaction(transaction);

  return true;
}

bool do_delete(long long key)
{
  const NdbDictionary::Dictionary *dict = ndb_object->getDictionary();
  const NdbDictionary::Table *table = dict->getTable("test_table");

  if (table == nullptr)
    return on_error(dict->getNdbError(),
                    "Failed to access 'test_db.test_table'");

  // The delete operation will be performed within single transaction
  NdbTransaction *transaction = ndb_object->startTransaction(table);
  if (transaction == nullptr)
    return on_error(ndb_object->getNdbError(), "Failed to start transaction");

  NdbOperation *operation = transaction->getNdbOperation(table);
  if (operation == nullptr)
    return on_error(transaction->getNdbError(),
                    "Failed to start delete operation");

  operation->deleteTuple();
  operation->equal("ATTR1", key);

  if (transaction->execute(NdbTransaction::Commit) != 0)
    return on_error(transaction->getNdbError(),
                    "Failed to execute transaction");

  ndb_object->closeTransaction(transaction);

  return true;
}

long long do_read(long long key)
{
  const NdbDictionary::Dictionary *dict = ndb_object->getDictionary();
  const NdbDictionary::Table *table = dict->getTable("test_table");

  if (table == nullptr)
  {

    on_error(dict->getNdbError(),
             "Failed to access 'test_db.test_table'");
    return -1;
  }

  NdbTransaction *transaction = ndb_object->startTransaction(table);
  if (transaction == nullptr)
    return on_error(ndb_object->getNdbError(), "Failed to start transaction");

  NdbOperation *operation = transaction->getNdbOperation(table);
  if (operation == nullptr)
  {
    on_error(transaction->getNdbError(),
             "Failed to start read operation");
    return -1;
  }

  operation->readTuple(NdbOperation::LM_CommittedRead);
  operation->equal("ATTR1", key);
  NdbRecAttr *myRecAttr = operation->getValue("ATTR2", NULL);

  if (transaction->execute(NdbTransaction::Commit) != 0)
  {
    on_error(transaction->getNdbError(),
             "Failed to execute transaction");
    return -1;
  }

  long long retValue = myRecAttr->u_32_value();
  // std::cout << retValue << std::endl;

  ndb_object->closeTransaction(transaction);

  return retValue;
}

bool do_scan()
{
  NdbDictionary::Dictionary *dict = ndb_object->getDictionary();
  const NdbDictionary::Table *table = dict->getTable("test_table");
  if (table == nullptr)
    return on_error(dict->getNdbError(),
                    "Cannot access table 'test_db.test_table'");

  // Prepare record specification,
  // this will allow us later to access rows in the table
  // using our structure BasicRow
  NdbRecord *record;
  NdbDictionary::RecordSpecification record_spec[] = {
      {table->getColumn("ATTR1"), offsetof(BasicRow, attr1), 0, 0, 0},
      {table->getColumn("ATTR2"), offsetof(BasicRow, attr2), 0, 0, 0}};

  record = dict->createRecord(table,
                              record_spec,
                              std::size(record_spec),
                              sizeof(record_spec[0]));
  if (record == nullptr)
    return on_error(dict->getNdbError(), "Failed to create record");

  // All reads will be performed within single transaction
  NdbTransaction *transaction = ndb_object->startTransaction(table);
  if (transaction == nullptr)
    return on_error(ndb_object->getNdbError(), "Failed to start transaction");

  // Note the usage of NdbScanOperation instead of regular NdbOperation
  NdbScanOperation *operation = transaction->scanTable(record);
  if (operation == nullptr)
    return on_error(transaction->getNdbError(),
                    "Failed to start scanTable operation");

  // Note the usage of NoCommit flag, as we are only reading the tuples
  if (transaction->execute(NdbTransaction::NoCommit) != 0)
    return on_error(transaction->getNdbError(),
                    "Failed to execute transaction");

  const BasicRow *row_ptr;
  int rc;
  std::cout << "ATTR1"
            << "\t"
            << "ATTR2" << std::endl;
  // Loop over all read results to print them
  while ((rc = operation->nextResult(reinterpret_cast<const char **>(&row_ptr),
                                     true, false)) == 0)
    std::cout << row_ptr->attr1 << "\t" << row_ptr->attr2
              << std::endl;
  if (rc == -1)
    return on_error(transaction->getNdbError(), "Failed to read tuple");

  operation->close();
  ndb_object->closeTransaction(transaction);
  dict->releaseRecord(record);

  return true;
}

inline bool on_error(const struct NdbError &error,
                     const char *explanation)
{
  // prints error in format:
  // ERROR <NdbErrorCode>: <NdbError message>
  //    explanation what went wrong on higher level (in the example code)
  std::cout << "ERROR " << error.code << ": " << error.message << std::endl;
  std::cout << explanation << std::endl;
  return false;
}

bool initialize(const char *connectstring)
{
  std::cout << "Initializing management server. Connect String: " << connectstring << std::endl;
  ndb_init();
  ndb_connection = new Ndb_cluster_connection(connectstring);
  if (ndb_connection->connect() != 0)
  {
    std::cout << "Cannot connect to cluster management server" << std::endl;
    return false;
  }

  if (ndb_connection->wait_until_ready(30, 0) != 0)
  {
    std::cout << "Cluster was not ready within 30 secs" << std::endl;
    return false;
  }

  ndb_object = new Ndb(ndb_connection, "test_db");
  if (ndb_object->init() != 0)
  {
    on_error(ndb_object->getNdbError(), "Failed to initialize ndb object");
  }

  std::cout << "Connected." << std::endl;
  return true;
}

void shutdown()
{
  ndb_end(0);
  delete ndb_connection;
  std::cout << "NDB connection closed" << std::endl;
}

void some_action()
{
  int records = 100;
  for (int i = 0; i < records; i++)
  {
    do_write(i, i * 2);
  }

  for (int i = 0; i < records; i++)
  {
    do_read(i);
  }

  // do_scan(&ndb_object);
}

#include <ndb_api_wrapper.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <iterator>
#include <NdbApi.hpp>
#include <cstring>

Ndb_cluster_connection *ndb_connection;
Ndb *ndb_object;
char attr2_data[2 + 4096] = {(char)4096 % 256, (char)4096 / 256, 0};

inline bool on_error(const struct NdbError &error, const char *explanation);
int get_byte_array(const NdbRecAttr *attr, char *first_byte, size_t &bytes);

struct BasicRow
{
  int attr1;
  char attr2[2 + 4096];
};

void hello_world()
{
  std::cout << "Hello World!" << std::endl;
}

bool do_write(long long key, char *value)
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

long long do_read(long long key, char *first_byte)
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
  {
    on_error(ndb_object->getNdbError(), "Failed to start transaction");
    return -1;
  }

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

  size_t count;
  get_byte_array(myRecAttr, first_byte, count);
  ndb_object->closeTransaction(transaction);

  return count;
}

/* extracts the length and the start byte of the data stored */
int get_byte_array(const NdbRecAttr *attr, char *first_byte, size_t &bytes)
{
  const NdbDictionary::Column::ArrayType array_type =
      attr->getColumn()->getArrayType();
  const size_t attr_bytes = attr->get_size_in_bytes();
  const char *aRef = attr->aRef();
  std::string result;

  switch (array_type)
  {
  case NdbDictionary::Column::ArrayTypeFixed:
    /*
     No prefix length is stored in aRef. Data starts from aRef's first byte
     data might be padded with blank or null bytes to fill the whole column
     */
    // first_byte = aRef;
    bytes = attr_bytes;
    memcpy(first_byte, aRef, bytes);
    return 0;
  case NdbDictionary::Column::ArrayTypeShortVar:
    /*
     First byte of aRef has the length of data stored
     Data starts from second byte of aRef
     */
    // first_byte = aRef + 1;
    bytes = (size_t)(aRef[0]);
    memcpy(first_byte, aRef+1, bytes);
    return 0;
  case NdbDictionary::Column::ArrayTypeMediumVar:
    /*
     First two bytes of aRef has the length of data stored
     Data starts from third byte of aRef
     */
    // first_byte = aRef + 2;
    bytes = (size_t)(aRef[1]) * 256 + (size_t)(aRef[0]);
    memcpy(first_byte, aRef+2, bytes);
    return 0;
  default:
    first_byte = NULL;
    bytes = 0;
    return -1;
  }
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
  // ndb_end(0);
  delete ndb_connection;
  std::cout << "NDB connection closed" << std::endl;
}
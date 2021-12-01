#include <ndb_api_wrapper.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <iterator>
#include <ndb_api_wrapper.h>

int main(int argc, char **argv)
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

  int iterations = 10000;
  for (int i = 0; i < iterations; i++)
  {
    do_write(i, i);
  }

  for (int i = 0; i < iterations; i++)
  {
    long long read = do_read(i);
    if (read < 0)
    {
      std::cout << "Wrong value read for index : " << i << std::endl;
    }
  }

  return EXIT_SUCCESS;
}

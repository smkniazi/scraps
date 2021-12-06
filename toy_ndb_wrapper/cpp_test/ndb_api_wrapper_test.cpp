#include <ndb_api_wrapper.h>
#include <iostream>
#include <cstdlib>
#include <string>
#include <iterator>
#include <ndb_api_wrapper.h>

void sumArray(const char *data, int start, int end);
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

  char attr2_data[2 + 4096] = {0};
  attr2_data[0] = 4096 % 256;
  attr2_data[1] = 4096 / 256;

  for (int i = 2; i < 4098; i++)
  {
    attr2_data[i] = 1;
  }
  // sumArray(attr2_data, 2, 4098);

  int iterations = 100000;
  for (int i = 0; i < iterations; i++)
  {
    do_write(i, attr2_data);
  }

  for (int i = 0; i < iterations; i++)
  {
    char readData[4096]={};

    long long read = do_read(i, readData);
    if (read < 0)
    {
      std::cout << "Wrong value read for index : " << i << std::endl;
    }
    else
    {
      // sumArray(readData, 0, read);
    }
  }

  return EXIT_SUCCESS;
}

void sumArray(const char *data, int start, int end)
{

  int sum = 0;
  for (int i = start; i < end; i++)
  {
    sum += data[i];
    printf("%d, ", data[i]);
  }
  printf("\nTotal of data is %d \n ", sum);
}

// std::cout<< "Sizeof array is "<< sizeof(attr2_data)<<std::endl;
// std::cout<< "Length of data is  "<< sizeof(attr2_data)<<std::endl;
// printf("hashedChars: ");
// for (long unsigned int i = 0; i < sizeof(attr2_data); i++) {
//   printf("%x", attr2_data[i]);
// }
// printf("\n");
#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
   bool do_write(long long key, char* value);
   long long do_read(long long key, char* first_byte);
   bool do_delete(long long key);
   bool initialize(const char *connectstring);
   void shutdown();
   void hello_world();
#ifdef __cplusplus
}
#endif




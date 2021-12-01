#ifdef __cplusplus
extern "C" {
#endif
#include <stdbool.h>
   bool do_write(long long key, long long value);
   bool do_scan();
   long long do_read(long long key);
   bool do_delete(long long key);
   void some_action();
   bool initialize(const char *connectstring);
   void shutdown();
   void hello_world();
#ifdef __cplusplus
}
#endif




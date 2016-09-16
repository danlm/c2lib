#include <pool.h>
#include <pstring.h>

const char *strings[] = { "John", "Paul", "George", "Ringo" };

main ()
{
  pool pool = global_pool;
  vector v = pvectora (pool, strings, 4);
  printf ("Introducing the Beatles: %s\n", pjoin (pool, v, ", "));
}

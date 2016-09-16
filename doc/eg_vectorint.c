#include <pool.h>
#include <vector.h>
#include <pstring.h>

main ()
{
  pool pool = global_pool;
  vector v = new_vector (pool, int);
  int i, prod = 1;

  for (i = 1; i <= 10; ++i)
    vector_push_back (v, i);

  for (i = 0; i < vector_size (v); ++i)
    {
      int elem;

      vector_get (v, i, elem);
      prod *= elem;
    }

  printf ("product of integers: %s = %d\n",
	  pjoin (pool, pvitostr (pool, v), " * "),
	  prod);
}

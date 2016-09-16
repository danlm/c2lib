#include <pool.h>
#include <vector.h>
#include <pstring.h>

main ()
{
  pool pool = global_pool;
  vector v = pvector (pool,
		      "a", "b", "c", "d", "e",
		      "f", "g", "h", "i", "j", 0);
  const char *X = "X";

  printf ("Original vector contains: %s\n",
	  pjoin (pool, v, ", "));

  vector_erase_range (v, 3, 6);

  printf ("After erasing elements 3-5, vector contains: %s\n",
	  pjoin (pool, v, ", "));

  vector_insert (v, 3, X);
  vector_insert (v, 4, X);
  vector_insert (v, 5, X);

  printf ("After inserting 3 Xs, vector contains: %s\n",
	  pjoin (pool, v, ", "));

  vector_clear (v);
  vector_fill (v, X, 10);

  printf ("After clearing and inserting 10 Xs, vector contains: %s\n",
	  pjoin (pool, v, ", "));
}

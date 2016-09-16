#include <stdio.h>
#include <hash.h>
#include <pstring.h>

main ()
{
  pool pool = global_pool;
  sash h = new_sash (pool);
  char *fruit;
  const char *color;

  sash_insert (h, "banana", "yellow");
  sash_insert (h, "orange", "orange");
  sash_insert (h, "apple", "red");
  sash_insert (h, "kiwi", "green");
  sash_insert (h, "grapefruit", "yellow");
  sash_insert (h, "pear", "green");
  sash_insert (h, "tomato", "red");
  sash_insert (h, "tangerine", "orange");

  for (;;)
    {
      printf ("Please type in the name of a fruit: ");
      fruit = pgetline (pool, stdin, 0);

      if (sash_get (h, fruit, color))
	printf ("The color of that fruit is %s.\n", color);
      else
	printf ("Sorry, I don't know anything about that fruit.\n");
    }
}

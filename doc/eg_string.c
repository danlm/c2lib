#include <assert.h>
#include <pstring.h>

char *given_name = "Richard";
char *family_name = "Jones";
char *email_address = "rich@annexia.org";

main ()
{
  pool pool = global_pool;
  char *email, *s;
  vector v;

  email =
    psprintf (pool, "%s %s <%s>", given_name, family_name, email_address);

  printf ("full email address is: %s\n", email);

  v = pstrcsplit (pool, email, ' ');

  printf ("split email into %d components\n", vector_size (v));

  vector_get (v, 0, s);
  printf ("first component is: %s\n", s);
  assert (strcmp (s, given_name) == 0);

  vector_get (v, 1, s);
  printf ("second component is: %s\n", s);
  assert (strcmp (s, family_name) == 0);

  vector_get (v, 2, s);
  printf ("third component is: %s\n", s);
  s = pstrdup (pool, s);
  s++;
  s[strlen(s)-1] = '\0';
  assert (strcmp (s, email_address) == 0);
}

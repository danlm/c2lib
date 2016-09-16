/* Test pre library.
 * By Richard W.M. Jones <rich@annexia.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * $Id: test_pre.c,v 1.1 2002/10/05 16:42:06 rich Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <pcre.h>

#include "pool.h"
#include "vector.h"
#include "pstring.h"
#include "pre.h"

static int vequals (vector v, int i, const char *str);

int
main ()
{
  pool pool;
  vector v;
  pcre *re1, *re2, *re3, *re4, *re5;
  const char *str;

  pool = new_subpool (global_pool);

  /* Compile some regular expressions. */
  re1 = precomp (pool, "a*b+", 0);
  re2 = precomp (pool, "bu(t+)ery", 0);
  re3 = precomp (pool, "m(a+)rgar(ee+)ne", 0);
  re4 = precomp (pool, "(a|b)(c|d)+", 0);
  re5 = precomp (pool, "(a|b)((c|d)+)", 0);

  /* Matching tests. */
  v = prematch (pool, "", re1, 0);
  assert (v == 0);

  v = prematch (pool, "a", re1, 0);
  assert (v == 0);

  v = prematch (pool, "ab", re1, 0);
  assert (v);
  assert (vector_size (v) == 1);
  assert (vequals (v, 0, "ab"));

  v = prematch (pool, "b", re1, 0);
  assert (v);
  assert (vector_size (v) == 1);
  assert (vequals (v, 0, "b"));

  v = prematch (pool, "xxxaaaaaaaaaabby", re1, 0);
  assert (v);
  assert (vector_size (v) == 1);
  assert (vequals (v, 0, "aaaaaaaaaabb"));

  v = prematch (pool, "This is quite buttery.", re2, 0);
  assert (v);
  assert (vector_size (v) == 2);
  assert (vequals (v, 0, "buttery"));
  assert (vequals (v, 1, "tt"));

  v = prematch (pool, "This is quite buttttttery.", re2, 0);
  assert (v);
  assert (vector_size (v) == 2);
  assert (vequals (v, 0, "buttttttery"));
  assert (vequals (v, 1, "tttttt"));

  v = prematch (pool, "margarene", re3, 0);
  assert (v == 0);

  v = prematch (pool, "margareene", re3, 0);
  assert (v);
  assert (vector_size (v) == 3);
  assert (vequals (v, 0, "margareene"));
  assert (vequals (v, 1, "a"));
  assert (vequals (v, 2, "ee"));

  v = prematch (pool, "maargareeene", re3, 0);
  assert (v);
  assert (vector_size (v) == 3);
  assert (vequals (v, 0, "maargareeene"));
  assert (vequals (v, 1, "aa"));
  assert (vequals (v, 2, "eee"));

  v = prematch (pool, "abcd", re4, 0);
  assert (v);
  assert (vector_size (v) == 3);
  assert (vequals (v, 0, "bcd"));
  assert (vequals (v, 1, "b"));
  assert (vequals (v, 2, "d"));

  v = prematch (pool, "abcd", re5, 0);
  assert (v);
  assert (vector_size (v) == 4);
  assert (vequals (v, 0, "bcd"));
  assert (vequals (v, 1, "b"));
  assert (vequals (v, 2, "cd"));
  assert (vequals (v, 3, "d"));

  /* Substitution tests. */

  str = presubst (pool, "xxxxmaargareeeeneyy", re3, "$1$2", 0);
  assert (strcmp (str, "xxxxaaeeeeyy") == 0);

  str = presubst (pool, "xxxxmaargareeeeneyy", re3, "$2$1", 0);
  assert (strcmp (str, "xxxxeeeeaayy") == 0);

  str = presubst (pool, "xxxxmaargareeeeneyy xxxxmaargareeeeneyy",
		  re3, "$2$1", 0);
  assert (strcmp (str, "xxxxeeeeaayy xxxxmaargareeeeneyy") == 0);

  str = presubst (pool, "abcd", re4, "$2$1", 0);
  assert (strcmp (str, "adb") == 0);

  /* Check all the presubst flags work correctly. */
  str = presubst (pool, "xxxxmaargareeeeneyy", re3, "$1$2",
		  PRESUBST_NO_PLACEHOLDERS);
  assert (strcmp (str, "xxxx$1$2yy") == 0);

  str = presubst (pool, "This is buttery buttery toast.", re2, "tasty",
		  PRESUBST_GLOBAL);
  assert (strcmp (str, "This is tasty tasty toast.") == 0);

  delete_pool (pool);
  exit (0);
}

static int
vequals (vector v, int i, const char *str)
{
  const char *s;

  vector_get (v, i, s);
  return strcmp (s, str) == 0;
}

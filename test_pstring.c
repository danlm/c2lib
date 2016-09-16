/* Test pstring library.
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
 * $Id: test_pstring.c,v 1.11 2003/02/09 16:48:46 rich Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <pcre.h>

#include "pstring.h"
#include "vector.h"

const int i_one = 1, i_two = 2, i_three = 3, i_four = 4,
  i_15 = 15, i_31 = 31, i_63 = 63, i_127 = 127;
const double d_one = 1, d_two = 2, d_three = 3, d_four = 4;
const char *s_one = "one", *s_two = "two", *s_three = "three",
  *s_four = "four";

const char *stra[] = { "one", "two", "three", "four" };

const char *file1[] = { "line1\r\n",
			"line2\r\n",
			"line3\r\n", 0};
const char *file2[] = { "line1\n",
			"line2\n",
			"line3\n", 0 };
const char *file3[] = { "line1\n",
			"line2\n",
			"\n",
			"line3\n", 0 };
const char *file4[] = { "line1\\\n",
			"line2\n",
			"line3\n", 0 };
const char *file5[] = { "line1\n",
			"# a comment\n",
			"line2\n",
			"line3\n", 0 };
const char *res1[] = { "line1", "line2", "line3", 0 };
const char *res2[] = { "line1line2", "line3", 0 };

static int
my_strcmp (const char **p1, const char **p2)
{
  return strcmp (*p1, *p2);
}

int
main ()
{
  pool pool;
  char *s1, *s2;
  vector v1, v2, v3, v4, v5;
  pcre *re;
  int i;
  FILE *fp;
  const char **sp;
  const char *errptr;
  int erroffset;

  pool = new_pool ();

  /* pstrdup */
  s1 = pstrdup (pool, "sheep");
  s2 = pstrdup (pool, s1);

  s1[1] = 'l';

  assert (strcmp (s2, "sheep") == 0);

  /* pstrndup */
  s1 = pstrndup (pool, "sheep", 3);
  s2 = pstrndup (pool, "sheep", 6);

  assert (strcmp (s1, "she") == 0);
  assert (strcmp (s2, "sheep") == 0);

  /* pmemdup */
  s1 = pmemdup (pool, "waves", 6);

  assert (strcmp (s1, "waves") == 0);

  /* pstrsplit */
  v1 = pstrsplit (pool, "one", "--");
  v2 = pstrsplit (pool, "--one--two", "--");
  v3 = pstrsplit (pool, "one--two--three", "--");
  v4 = pstrsplit (pool, "one--two--three--four--", "--");

  v5 = new_vector (pool, char *);
  vector_push_back (v5, s_one);
  assert (vector_compare (v1, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  vector_push_back (v5, s_two);
  assert (vector_compare (v2, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  vector_push_back (v5, s_three);
  assert (vector_compare (v3, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  vector_push_back (v5, s_four);
  assert (vector_compare (v4, v5, (int (*)(const void *,const void *))my_strcmp) == 0);

  /* pstrcsplit */
  v1 = pstrcsplit (pool, "one", ',');
  v2 = pstrcsplit (pool, ",one,two", ',');
  v3 = pstrcsplit (pool, "one,two,three,", ',');
  v4 = pstrcsplit (pool, "one,two,three,four", ',');

  v5 = new_vector (pool, char *);
  vector_push_back (v5, s_one);
  assert (vector_compare (v1, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  vector_push_back (v5, s_two);
  assert (vector_compare (v2, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  vector_push_back (v5, s_three);
  assert (vector_compare (v3, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  vector_push_back (v5, s_four);
  assert (vector_compare (v4, v5, (int (*)(const void *,const void *))my_strcmp) == 0);

  /* pstrresplit */
  re = pcre_compile ("[ \t]+", 0, &errptr, &erroffset, 0);
  assert (re);
  v1 = pstrresplit (pool, "one", re);
  v2 = pstrresplit (pool, " one \ttwo", re);
  v3 = pstrresplit (pool, " one\ttwo\tthree ", re);
  v4 = pstrresplit (pool, "  one two \t three   four  ", re);

  v5 = new_vector (pool, char *);
  vector_push_back (v5, s_one);
  assert (vector_compare (v1, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  vector_push_back (v5, s_two);
  assert (vector_compare (v2, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  vector_push_back (v5, s_three);
  assert (vector_compare (v3, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  vector_push_back (v5, s_four);
  assert (vector_compare (v4, v5, (int (*)(const void *,const void *))my_strcmp) == 0);
  free (re);

  /* pconcat, pjoin */
  v1 = new_vector (pool, char *);
  vector_push_back (v1, s_one);
  vector_push_back (v1, s_two);
  vector_push_back (v1, s_three);
  vector_push_back (v1, s_four);
  s1 = pconcat (pool, v1);
  assert (strcmp (s1, "onetwothreefour") == 0);
  s1 = pjoin (pool, v1, "");
  assert (strcmp (s1, "onetwothreefour") == 0);
  s1 = pjoin (pool, v1, ",");
  assert (strcmp (s1, "one,two,three,four") == 0);
  s1 = pjoin (pool, v1, ", ");
  assert (strcmp (s1, "one, two, three, four") == 0);

  /* pstrresplit2 -> pconcat = original string */
  re = pcre_compile ("[ \t]+", 0, &errptr, &erroffset, 0);
  s1 = "the quick brown fox";
  v1 = pstrresplit2 (pool, s1, re);
  s2 = pconcat (pool, v1);
  assert (strcmp (s1, s2) == 0);

  s1 = " the   quick \tbrown fox";
  v1 = pstrresplit2 (pool, s1, re);
  s2 = pconcat (pool, v1);
  assert (strcmp (s1, s2) == 0);

  s1 = "the quick brown  \tfox\t";
  v1 = pstrresplit2 (pool, s1, re);
  s2 = pconcat (pool, v1);
  assert (strcmp (s1, s2) == 0);

  s1 = "  \tthe quick brown  \tfox\t";
  v1 = pstrresplit2 (pool, s1, re);
  s2 = pconcat (pool, v1);
  assert (strcmp (s1, s2) == 0);
  free (re);

  /* psort */
  v1 = new_vector (pool, char *);
  vector_push_back (v1, s_one);
  vector_push_back (v1, s_two);
  vector_push_back (v1, s_three);
  vector_push_back (v1, s_four);
  psort (v1, my_strcmp);
  s1 = pconcat (pool, v1);
  assert (strcmp (s1, "fouronethreetwo") == 0);

  /* pchomp */
  s1 = pstrdup (pool, "sheep");
  pchomp (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "sheep\r");
  pchomp (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "sheep\r\n");
  pchomp (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "sheep\n");
  pchomp (s1);
  assert (strcmp (s1, "sheep") == 0);

  /* pchrs, pstrs */
  s1 = pchrs (pool, 'x', 20);
  assert (strcmp (s1, "xxxxxxxxxxxxxxxxxxxx") == 0);
  s1 = pchrs (pool, 'x', 0);
  assert (strcmp (s1, "") == 0);
  s1 = pstrs (pool, "x", 20);
  assert (strcmp (s1, "xxxxxxxxxxxxxxxxxxxx") == 0);
  s1 = pstrs (pool, "xyz", 10);
  assert (strcmp (s1, "xyzxyzxyzxyzxyzxyzxyzxyzxyzxyz") == 0);
  s1 = pstrs (pool, "xyz", 0);
  assert (strcmp (s1, "") == 0);
  s1 = pstrs (pool, "", 100);
  assert (strcmp (s1, "") == 0);

  /* psprintf (also implicitly tests pvsprintf) */
  s1 = psprintf (pool, "%d %s %s %s %s",
		 4, "one", "two", "three", "four");
  assert (strcmp (s1, "4 one two three four") == 0);
  for (i = 250; i < 270; ++i)
    {
      s1 = pchrs (pool, 'x', i);
      s2 = psprintf (pool, "%s", s1);
      assert (strcmp (s1, s2) == 0);
    }

  /* pvector, pvectora */
  s1 = pjoin (pool, pvector (pool, "one", "two", "three", "four", 0), ",");
  assert (strcmp (s1, "one,two,three,four") == 0);
  s1 = pjoin (pool, pvectora (pool, stra, 4), ",");
  assert (strcmp (s1, "one,two,three,four") == 0);

  /* pitoa, pdtoa, pxtoa */
  assert (strcmp (pitoa (pool, 1), "1") == 0);
  assert (strcmp (pdtoa (pool, 2.1), "2.100000") == 0);
  assert (strcmp (pxtoa (pool, 15), "f") == 0);

  /* pvitostr, pvdtostr, pvxtostr */
  v1 = new_vector (pool, int);
  vector_push_back (v1, i_one);
  vector_push_back (v1, i_two);
  vector_push_back (v1, i_three);
  vector_push_back (v1, i_four);
  s1 = pjoin (pool, pvitostr (pool, v1), ",");
  assert (strcmp (s1, "1,2,3,4") == 0);

  v1 = new_vector (pool, double);
  vector_push_back (v1, d_one);
  vector_push_back (v1, d_two);
  vector_push_back (v1, d_three);
  vector_push_back (v1, d_four);
  s1 = pjoin (pool, pvdtostr (pool, v1), ",");
  assert (strcmp (s1, "1.000000,2.000000,3.000000,4.000000") == 0);

  v1 = new_vector (pool, unsigned);
  vector_push_back (v1, i_15);
  vector_push_back (v1, i_31);
  vector_push_back (v1, i_63);
  vector_push_back (v1, i_127);
  s1 = pjoin (pool, pvxtostr (pool, v1), ",");
  assert (strcmp (s1, "f,1f,3f,7f") == 0);

  /* pstrcat */
  s1 = pstrdup (pool, "one");
  s1 = pstrcat (pool, s1, ",two");
  assert (strcmp (s1, "one,two") == 0);

  /* pstrncat */
  s1 = pstrdup (pool, "one");
  s1 = pstrncat (pool, s1, ",two,three", 4);
  assert (strcmp (s1, "one,two") == 0);

  /* psubstr */
  s1 = psubstr (pool, "sheep", 1, 2);
  assert (strcmp (s1, "he") == 0);
  s1 = psubstr (pool, "sheep", 1, -1);
  assert (strcmp (s1, "heep") == 0);

  /* pstrupr, pstrlwr */
  s1 = pstrdup (pool, "sheep");
  pstrupr (s1);
  assert (strcmp (s1, "SHEEP") == 0);
  pstrlwr (s1);
  assert (strcmp (s1, "sheep") == 0);

  /* pgetline, pgetlinex, pgetlinec */
#define TEST_PGETL(getfn,input,expect) s1 = pstrdup (pool, ""); fp = tmpfile (); for (sp = input; *sp; ++sp) fputs (*sp, fp); rewind (fp); for (sp = expect; *sp; ++sp) { s1 = getfn (pool, fp, s1); assert (strcmp (s1, *sp) == 0); } assert (getfn (pool, fp, s1) == 0); fclose (fp);

  TEST_PGETL (pgetline, file1, res1);
  TEST_PGETL (pgetline, file2, res1);
  TEST_PGETL (pgetlinec, file3, res1);
  TEST_PGETL (pgetlinec, file4, res2);
  TEST_PGETL (pgetlinec, file5, res1);

  /* ptrim, ptrimfront, ptrimback */
  s1 = pstrdup (pool, "          sheep\t\t\t");
  ptrim (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "sheep   ");
  ptrim (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "          sheep");
  ptrim (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "sheep");
  ptrim (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "");
  ptrim (s1);
  assert (strcmp (s1, "") == 0);

  s1 = pstrdup (pool, "          sheep");
  ptrimfront (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "sheep  ");
  ptrimfront (s1);
  assert (strcmp (s1, "sheep  ") == 0);
  s1 = pstrdup (pool, "sheep");
  ptrimfront (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "");
  ptrimfront (s1);
  assert (strcmp (s1, "") == 0);

  s1 = pstrdup (pool, "          sheep");
  ptrimback (s1);
  assert (strcmp (s1, "          sheep") == 0);
  s1 = pstrdup (pool, "sheep  ");
  ptrimback (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "sheep");
  ptrimback (s1);
  assert (strcmp (s1, "sheep") == 0);
  s1 = pstrdup (pool, "");
  ptrimback (s1);
  assert (strcmp (s1, "") == 0);

  delete_pool (pool);
  exit (0);
}

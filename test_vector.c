/* Test the vector class.
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
 * $Id: test_vector.c,v 1.2 2001/02/16 12:55:54 rich Exp $
 *
 * XXX This test is very incomplete at present.
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#ifdef HAVE_ASSERT_H
#include <assert.h>
#endif

#include <pool.h>
#include <vector.h>
#include <pstring.h>

/* These are the numbers we'll be inserting into the array. */
static int numbers1to9[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };

static void sqr_ptr (int *a, int *r) { *r = *a * *a; }
static int gt20_ptr (int *a) { return *a > 20; }

int
main ()
{
  pool pool = new_pool ();
  vector numbers, squares, squaresgt20;

  /* Create initial vector. */
  numbers = new_vector (pool, int);
  vector_insert_array (numbers, 0, numbers1to9, 9);
  assert (vector_size (numbers) == 9);

  /* Print numbers. */
  printf ("numbers = [ %s ]\n",
	  pjoin (pool, pvitostr (pool, numbers), ", "));

  /* Square each number. */
  squares = vector_map (pool, numbers,
			(void (*) (const void *, void *)) sqr_ptr,
			int);
  assert (vector_size (squares) == 9);
  printf ("squares = [ %s ]\n",
	  pjoin (pool, pvitostr (pool, squares), ", "));

  /* Get squares > 20. */
  squaresgt20 = vector_grep (pool, squares,
			     (int (*) (const void *)) gt20_ptr);
  assert (vector_size (squaresgt20) == 5);
  printf ("squares > 20 = [ %s ]\n",
	  pjoin (pool, pvitostr (pool, squaresgt20), ", "));

  delete_pool (pool);
  exit (0);
}

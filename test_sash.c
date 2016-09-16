/* Test the sash (string hash) class.
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
 * $Id: test_sash.c,v 1.1 2001/02/08 12:51:32 rich Exp $
 */

#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include <pool.h>
#include <vector.h>
#include <pstring.h>
#include <hash.h>

int
main ()
{
  sash h;
  pool pool = new_pool (), pool2;
  const char *v;
  vector keys, values;

  /* Create a string -> string hash. */
  h = new_sash (pool);

  /* Insert some new values. */
  if (sash_insert (h, "tomato", "red") != 0) abort ();
  if (sash_insert (h, "orange", "orange") != 0) abort ();
  if (sash_insert (h, "lemon", "yellow") != 0) abort ();
  if (sash_insert (h, "lime", "green") != 0) abort ();
  if (sash_insert (h, "peach", "orange") != 0) abort ();
  if (sash_insert (h, "apple", "green") != 0) abort ();
  if (sash_insert (h, "raspberry", "red") != 0) abort ();

  /* This should replace existing values. */
  if (sash_insert (h, "apple", "red") == 0) abort ();
  if (sash_insert (h, "peach", "yellow") == 0) abort ();

  /* Retrieve some values. */
  if (sash_get (h, "apple", v) == 0 || strcmp (v, "red") != 0) abort ();
  if (sash_get (h, "tomato", v) == 0 || strcmp (v, "red") != 0) abort ();
  if (sash_get (h, "orange", v) == 0 || strcmp (v, "orange") != 0) abort ();

  /* Copy the sash. */
  h = copy_sash (pool, h);

  /* Erase a key and check that it no longer exists. */
  if (sash_erase (h, "apple") == 0) abort ();
  if (sash_get (h, "apple", v) != 0) abort ();

  /* Get list of keys and values. */
  keys = sash_keys (h);
  values = sash_values (h);

  printf ("keys = [ %s ]\n", pjoin (pool, keys, ", "));
  printf ("values = [ %s ]\n", pjoin (pool, values, ", "));

  /* Copy the sash into another pool, delete the old pool, check all
   * keys and values have been copied across.
   */
  pool2 = new_pool ();
  h = copy_sash (pool2, h);
  delete_pool (pool);

  keys = sash_keys (h);
  values = sash_values (h);

  printf ("keys = [ %s ]\n", pjoin (pool2, keys, ", "));
  printf ("values = [ %s ]\n", pjoin (pool2, values, ", "));

  delete_pool (pool2);
  exit (0);
}

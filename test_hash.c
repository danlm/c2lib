/* Test the hash class.
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
 * $Id: test_hash.c,v 1.1 2001/02/08 12:51:31 rich Exp $
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

/* You can't take the address of non-lvalues in C (ie. you can't
 * evaluate expressions like "&2"). Hence the following constant
 * definitions are necessary.
 */
const int one = 1;
const int two = 2;
const int three = 3;
const int four = 4;
const int five = 5;
const int six = 6;
const int nine = 9;
const int sixteen = 16;
const int twentyfour = 24;
const int twentyfive = 25;
const int thirtyfive = 35;
const int thirtysix = 36;

int
main ()
{
  hash h;
  pool pool = new_pool ();
  int v;
  vector keys, values;

  /* Create a int -> int hash. */
  h = new_hash (pool, int, int);

  /* Insert some new values. */
  if (hash_insert (h, one, one) != 0) abort ();
  if (hash_insert (h, two, four) != 0) abort ();
  if (hash_insert (h, three, nine) != 0) abort ();
  if (hash_insert (h, four, sixteen) != 0) abort ();
  if (hash_insert (h, five, twentyfour) != 0) abort (); /* sic */
  if (hash_insert (h, six, thirtyfive) != 0) abort (); /* sic */

  /* This should replace existing values. */
  if (hash_insert (h, five, twentyfive) == 0) abort ();
  if (hash_insert (h, six, thirtysix) == 0) abort ();

  /* Retrieve some values. */
  if (hash_get (h, one, v) == 0 || v != 1) abort ();
  if (hash_get (h, six, v) == 0 || v != 36) abort ();
  if (hash_get (h, two, v) == 0 || v != 4) abort ();
  if (hash_get (h, five, v) == 0 || v != 25) abort ();

  /* Copy the hash. */
  h = copy_hash (pool, h);

  /* Erase a key and check that it no longer exists. */
  if (hash_erase (h, one) == 0) abort ();
  if (hash_get (h, one, v) != 0) abort ();

  /* Get list of keys and values. */
  keys = hash_keys (h);
  values = hash_values (h);

  printf ("keys = [ %s ]\n",
	  pjoin (pool, pvitostr (pool, keys), ", "));
  printf ("values = [ %s ]\n",
	  pjoin (pool, pvitostr (pool, values), ", "));

  delete_pool (pool);
  exit (0);
}

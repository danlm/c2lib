/* Test the tree class.
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
 * $Id: test_tree.c,v 1.1 2002/09/15 15:08:52 rich Exp $
 *
 * XXX This test is incomplete.
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

#include "pool.h"
#include "tree.h"

int
main ()
{
  pool pool1 = new_subpool (global_pool), pool2;
  tree root, leaf1, leaf2, leaf3, t;
  const char *root_node = "root node";
  const char *leaf1_node = "leaf1 node";
  const char *leaf2_node = "leaf2 node";
  const char *leaf3_node = "leaf3 node";
  char *str;

  /* Create a simple node and test node access functions. */
  root = new_tree (pool1, char *);

  tree_set_data (root, root_node);

  leaf1 = new_tree (pool1, char *);
  tree_set_data (leaf1, leaf1_node);
  leaf2 = new_tree (pool1, char *);
  tree_set_data (leaf2, leaf2_node);
  leaf3 = new_tree (pool1, char *);
  tree_set_data (leaf3, leaf3_node);
  tree_push_back (root, leaf1);
  tree_push_back (root, leaf2);
  tree_push_back (root, leaf3);

  assert (tree_size (root) == 3);

  tree_get (root, 0, t);
  assert (t == leaf1);
  tree_get (root, 1, t);
  assert (t == leaf2);
  tree_get (root, 2, t);
  assert (t == leaf3);

  tree_pop_front (root, t);
  assert (t == leaf1);
  tree_pop_back (root, t);
  assert (t == leaf3);
  tree_pop_front (root, t);
  assert (t == leaf2);

  assert (tree_size (root) == 0);

  tree_insert (root, 0, leaf1);
  tree_insert (root, 1, leaf2);
  tree_insert (root, 2, leaf3);

  tree_get (root, 0, t);
  assert (t == leaf1);
  tree_get (root, 1, t);
  assert (t == leaf2);
  tree_get (root, 2, t);
  assert (t == leaf3);

  tree_replace (root, 0, leaf3);
  tree_replace (root, 1, leaf1);
  tree_replace (root, 2, leaf2);

  tree_get (root, 0, t);
  assert (t == leaf3);
  tree_get (root, 1, t);
  assert (t == leaf1);
  tree_get (root, 2, t);
  assert (t == leaf2);

  tree_erase (root, 0);
  assert (tree_size (root) == 2);

  tree_erase_range (root, 0, 2);
  assert (tree_size (root) == 0);

  tree_insert (root, 0, leaf1);
  tree_insert (root, 1, leaf2);
  tree_insert (root, 2, leaf3);

  tree_clear (root);
  assert (tree_size (root) == 0);

  tree_get_data (root, str);
  assert (strcmp (str, root_node) == 0);

  /* Copy the tree into another pool, delete the original. */
  tree_insert (root, 0, leaf1);
  tree_insert (root, 1, leaf2);
  tree_insert (root, 2, leaf3);

  pool2 = new_subpool (global_pool);
  root = copy_tree (pool2, root);
  delete_pool (pool1);

  assert (tree_size (root) == 3);

  tree_get_data (root, str);
  assert (strcmp (str, root_node) == 0);

  tree_get (root, 0, t);
  tree_get_data (t, str);
  assert (strcmp (str, leaf1_node) == 0);

  tree_get (root, 1, t);
  tree_get_data (t, str);
  assert (strcmp (str, leaf2_node) == 0);

  tree_get (root, 2, t);
  tree_get_data (t, str);
  assert (strcmp (str, leaf3_node) == 0);

  exit (0);
}

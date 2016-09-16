/* Test pools, subpools, memory allocation and so on.
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
 * $Id: test_pool.c,v 1.5 2002/12/07 15:18:26 rich Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <fcntl.h>
#include <errno.h>

/* Implement a simple malloc debugger -- which counts allocations and
 * frees and ensures that all memory allocated by the pool code is
 * evenually freed.
 */

static void  trace_init (void);
static void  trace_finish (void);
static void *trace_malloc (size_t n, const char *filename, int line);
static void *trace_realloc (void *p, size_t n, const char *filename, int line);
static void  trace_free (void *p);

#define malloc(n)    trace_malloc ((n), __FILE__, __LINE__);
#define realloc(p,n) trace_realloc ((p), (n), __FILE__, __LINE__);
#define free(p)      trace_free ((p));

#define NO_GLOBAL_POOL 1

#include "pool.h"
#include "pool.c"

static void test (void);

int
main ()
{
  trace_init ();
  test ();
  trace_finish ();
  exit (0);
}

/* Perform the tests. */
static void
simple_alloc_test ()
{
  pool p;
  int i;

  p = new_pool ();

  for (i = 0; i < 1000; ++i)
    {
      pmalloc (p, 1);
      pmalloc (p, 1);
      prealloc (p, pmalloc (p, 200), 300);
      pmalloc (p, 1000);
      prealloc (p, pmalloc (p, 1000), 1001);
      prealloc (p, pmalloc (p, 900), 901);
      pmalloc (p, 1);
      pmalloc (p, 4);
      pmalloc (p, 8);
      pmalloc (p, 1);
      pmalloc (p, 400);
    }

  delete_pool (p);
}

static void
simple_subpool_test (pool parent, int level)
{
  pool p;
  int i;

  if (level >= 8) return;

  if (parent)
    {
      /* The following two statements must remain together. */
      p = new_subpool (parent);
      prealloc (p, prealloc (p, pmalloc (p, 16), 100),
		200);
      /* End of realloc test. */

      pcalloc (p, 4, 4);
      prealloc (p, 0, 8);
      prealloc (p, pmalloc (p, 4), 8);
      pmalloc (parent, 1);
      prealloc (parent, pmalloc (parent, 8), 8);
      prealloc (parent, pmalloc (parent, 8), 16);
    }
  else
    {
      p = new_pool ();
      prealloc (p, pmalloc (p, 4), 8);
      pmalloc (p, 8);
      pmalloc (p, 1);
    }

  for (i = 0; i < 3; ++i)
    {
      simple_subpool_test (p, level+1);
      pmalloc (p, 1);
      if (parent) pmalloc (parent, 1);
    }

  if (parent == 0 || level == 4)
    delete_pool (p);
}

/* This is a regression test: we had reason to believe this didn't work
 * at one point.
 */
static void
create_delete_test ()
{
  pool p;

  p = new_pool ();
  delete_pool (p);
}

static int cleanup_called = 0;

static void
cleanup_fn (void *v)
{
  cleanup_called ++;
}

static void
cleanup_fn_test ()
{
  pool p, sp1, sp2;

  p = new_pool ();
  pool_register_cleanup_fn (p, cleanup_fn, 0);
  delete_pool (p);
  assert (cleanup_called == 1);

  p = new_pool ();
  sp1 = new_subpool (p);
  sp2 = new_subpool (p);
  pool_register_cleanup_fn (p, cleanup_fn, 0);
  pool_register_cleanup_fn (sp1, cleanup_fn, 0);
  pool_register_cleanup_fn (sp2, cleanup_fn, 0);
  delete_pool (sp1);
  assert (cleanup_called == 2);
  delete_pool (p);
  assert (cleanup_called == 4);
}

static void
cleanup_fd_test ()
{
  int fd;
  pool p;

  p = new_pool ();
  fd = open ("/dev/null", O_RDONLY);
  pool_register_fd (p, fd);
  delete_pool (p);

  assert (close (fd) == -1 && errno == EBADF);
}

static void
cleanup_malloc_test ()
{
  void *m;
  pool p;

  p = new_pool ();
  m = malloc (10);
  pool_register_malloc (p, m);
  delete_pool (p);
}

static void
lots_of_pmalloc_test ()
{
  pool p;
  pool *subpools;
  int i, j, np = 0;

  /* This test kind of simulates what the chat server does, in an
   * attempt to find the putative memory leak.
   */
  p = new_pool ();
  subpools = pcalloc (p, sizeof (struct pool), 100);

  for (i = 0; i < 10000; ++i)
    {
      if (subpools[np] != 0)
	delete_pool (subpools[np]);
      subpools[np] = new_subpool (p);

      for (j = 0; j < 100; ++j)
	{
	  void *ptr = pmalloc (subpools[np], 100);
	  prealloc (subpools[np], ptr, 200);
	}

      np++;
      if (np == 100) np = 0;
    }

  delete_pool (p);
}

static void
test ()
{
  simple_alloc_test ();
  simple_subpool_test (0, 0);
  create_delete_test ();
  cleanup_fn_test ();
  cleanup_fd_test ();
  cleanup_malloc_test ();
  lots_of_pmalloc_test ();
}

/* The tracing code. */
#undef malloc
#undef realloc
#undef free

static int nr_allocations = 0;

static void
trace_init ()
{
}

static void
trace_finish ()
{
  if (nr_allocations > 0)
    {
      fprintf (stderr, "*** error: %d allocation(s) leaked.\n",
	       nr_allocations);
      exit (1);
    }
}

static void *
trace_malloc (size_t n, const char *filename, int line)
{
  nr_allocations++;
  return malloc (n);
}

static void *
trace_realloc (void *p, size_t n, const char *filename, int line)
{
  return realloc (p, n);
}

static void
trace_free (void *p)
{
  nr_allocations--;
  free (p);
}

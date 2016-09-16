/* Test matrix, vector arithmetic.
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
 * $Id: test_matvec.c,v 1.2 2001/11/19 17:10:55 rich Exp $
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

#include "pool.h"
#include "matvec.h"

static inline int
is_equal (float a, float b)
{
  return fabs (a - b) < 1e-6;
}

static void
test_identity_zero ()
{
  pool p = new_subpool (global_pool);
  float *m = new_identity_matrix (p);
  float *v = new_zero_vec (p);

  assert (m[0] == 1 && m[1] == 0 && m[2] == 0 && m[3] == 0 &&
	  m[4] == 0 && m[5] == 1 && m[6] == 0 && m[7] == 0 &&
	  m[8] == 0 && m[9] == 0 && m[10] == 1 && m[11] == 0 &&
	  m[12] == 0 && m[13] == 0 && m[14] == 0 && m[15] == 1);
  assert (v[0] == 0 && v[1] == 0 && v[2] == 0 && v[3] == 1);

  delete_pool (p);
}

static void
test_mag_norm ()
{
  float v[3] = {2, 0, 0}, r[3];

  assert (vec_magnitude (v) == 2);
  assert (vec_magnitude2d (v) == 2);

  vec_normalize (v, r);
  assert (r[0] == 1 && r[1] == 0 && r[2] == 0);

  vec_normalize2d (v, r);
  assert (r[0] == 1 && r[1] == 0);
}

static void
test_dot_product ()
{
  float v1[3] = { 1, 0, 0 }, v2[3] = { 0, 1, 0 }, v3[3] = { 1, 1, 0 };

  /* Dot product of perpendicular vectors is 0. */
  assert (vec_dot_product (v1, v2) == 0);

  /* Dot product of identical normal vectors is 1. */
  assert (vec_dot_product (v1, v1) == 1);

  /* Angle between vectors. */
  assert (vec_angle_between (v1, v1) == 0);
  assert (is_equal (vec_angle_between (v1, v2), M_PI/2));
  assert (is_equal (vec_angle_between (v1, v3), M_PI/4));
}

static void
test_point_line ()
{
  float p[3], v[3], q[3], d;

  /* Line running along the x axis. */
  p[0] = 0; p[1] = 0; p[2] = 0; v[0] = 1; v[1] = 0; v[2] = 0;

  q[0] = 0; q[1] = 0; q[2] = 0;
  assert (point_distance_to_line (q, p, v) == 0);

  q[0] = 1; q[1] = 0; q[2] = 0;
  assert (point_distance_to_line (q, p, v) == 0);

  q[0] = 0; q[1] = 1; q[2] = 0;
  assert (point_distance_to_line (q, p, v) == 1);

  q[0] = 0; q[1] = 0; q[2] = 1;
  assert (point_distance_to_line (q, p, v) == 1);

  q[0] = 2; q[1] = 1; q[2] = 0;
  assert (point_distance_to_line (q, p, v) == 1);

  /* Line running diagonally x-y. */
  p[0] = 2; p[1] = 2; p[2] = 0; v[0] = 1; v[1] = 1; v[2] = 0;

  q[0] = 0; q[1] = 0; q[2] = 0;
  assert (point_distance_to_line (q, p, v) == 0);

  q[0] = 1; q[1] = 0; q[2] = 0;
  d = point_distance_to_line (q, p, v);
  assert (is_equal (d * d, 0.5));
}

static void
test_point_plane ()
{
  float p[3], q[3], r[3], pl[4];

  p[0] = 0; p[1] = 0; p[2] = 0;
  q[0] = 2; q[1] = 0; q[2] = 0;
  r[0] = 2; r[1] = 2; r[2] = 0;
  plane_coefficients (p, q, r, pl);

  assert (pl[3] == 0);
  vec_normalize (pl, pl);
  assert (pl[0] == 0 && pl[1] == 0 && pl[2] == -1);

  assert (point_distance_to_plane (pl, p) == 0);
  assert (point_distance_to_plane (pl, q) == 0);
  assert (point_distance_to_plane (pl, r) == 0);

  p[0] = 5; p[1] = 5; p[2] = -3;
  assert (point_distance_to_plane (pl, p) == 3);
  assert (point_is_inside_plane (pl, p));

  p[0] = 5; p[1] = 5; p[2] = 3;
  assert (point_distance_to_plane (pl, p) == -3);
  assert (!point_is_inside_plane (pl, p));
}

static void
test_point_in_face ()
{
  float points[4][3] = {
    { 0, 0, 0 },
    { 1, 1, 0 },
    { 1, 1, 1 },
    { 0, 0, 1 }
  };
  float p[3];

  p[0] = 0.5; p[1] = 0.5; p[2] = 0.5;
  assert (point_lies_in_face ((float *) points, 4, p));

  p[0] = 0.01; p[1] = 0.01; p[2] = 0.9;
  assert (point_lies_in_face ((float *) points, 4, p));

  p[0] = 0; p[1] = 0; p[2] = 0;
  assert (point_lies_in_face ((float *) points, 4, p));

  p[0] = 1; p[1] = 1; p[2] = 1;
  assert (point_lies_in_face ((float *) points, 4, p));

  p[0] = 1; p[1] = 0; p[2] = 0;
  assert (!point_lies_in_face ((float *) points, 4, p));

  p[0] = 1; p[1] = 1; p[2] = -1;
  assert (!point_lies_in_face ((float *) points, 4, p));

  p[0] = 0.5; p[1] = 0.5; p[2] = 2;
  assert (!point_lies_in_face ((float *) points, 4, p));
}

static void
test_point_to_face ()
{
  float points[4][3] = {
    { 0, 0, 0 },
    { 1, 0, 0 },
    { 1, 1, 0 },
    { 0, 1, 0 }
  };
  float p[3];
  int edge;

  p[0] = 0.5; p[1] = 0.5; p[2] = 0;
  assert (point_distance_to_face ((float *) points, 4, 0, p, &edge) == 0
	  && edge == -1);

  p[0] = 0.5; p[1] = 0.5; p[2] = 1;
  assert (point_distance_to_face ((float *) points, 4, 0, p, &edge) == -1
	  && edge == -1);

  p[0] = 0.5; p[1] = 0.5; p[2] = -1;
  assert (point_distance_to_face ((float *) points, 4, 0, p, &edge) == 1
	  && edge == -1);

  p[0] = -1; p[1] = 0.5; p[2] = 0;
  assert (point_distance_to_face ((float *) points, 4, 0, p, &edge) == -1
	  && edge == 3);
}

int
main ()
{
  test_identity_zero ();
  test_mag_norm ();
  test_dot_product ();
  test_point_line ();
  test_point_plane ();
  test_point_in_face ();
  test_point_to_face ();

  exit (0);
}

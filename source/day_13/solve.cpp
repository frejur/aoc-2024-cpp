#include "day_13.h"

aoc24_13::Results aoc24_13::solve(const Vec2d& vx,
                                  const Vec2d& vy,
                                  const Vec2d& vz)
{
	if (vx.x < 0 || vx.y < 0 || vy.x < 0 || vy.y < 0 || vz.x < 0 || vz.y < 0) {
		throw std::invalid_argument(
		    "Cannot solve, did not expect negative values");
	}

	/* Starting off with two equations:
	   (A)    vx.x * x + vy.x * y = vz.x
	   (B)    vx.y * x + vy.y * y = vz.y

	   Eliminate `x`:
	   1. Multiply equation (A) by `vx.y`:
	      ->                   vx.x * x + vy.x * y = vz.x
	      ->     vx.y * vx.x * x + vx.y * vy.x * y = vx.y * vz.x
	   2. Multiply equation (B) by `-vx.x`:
	      ->                   vx.y * x + vy.y * y = vz.y
	      ->    -vx.x * vx.y * x - vx.x * vy.y * y = -vx.x * vz.y
	   3. Add equations to eliminate `x`:
	      ->        vx.x * vy.y - vx.y * vy.x) * y = vx.x * vz.y - vx.y * vz.x
	   4. Simplify to:                       a * y = c
	*/

	// Start with `a`, which is basically the cross product of the two vectors.
	// (Or the Determinant)
	long long a = vx.x * vy.y - vx.y * vy.x;

	// TODO:   Solve collinear vectors
	// TEMP:   Require unique solution
	// REASON: Not needed for the input data used at the time of writing this...
	if (a == 0) {
		throw std::runtime_error(
		    "Solving for multiple solutions has not been implemented");
	}

	// Continue with `c`
	long long c = vx.x * vz.y - vx.y * vz.x;

	if (c % a != 0) {
		return Results::fail(); // No integer solution for y
	}

	/* Now, solve for `x` using original equations (A) or (B)
	   (A)    vx.x * x = vz.x - vy.x * y
	   (B)    vx.y * x = vz.y - vy.y * y
	*/
	long long y = c / a; // Extract `y`
	long long x;
	if (vx.x != 0) {
		// Use equation (A)
		if ((vz.x - vy.x * y) % vx.x != 0)
			return Results::fail();
		x = (vz.x - vy.x * y) / vx.x;
	} else if (vx.y != 0) {
		// Use equation (B)
		if ((vz.y - vy.y * y) % vx.y != 0)
			return Results::fail();
		x = (vz.y - vy.y * y) / vx.y;
	} else if (vy.x != 0) {
		// Corner case, eq (A) becomes: `vy.x * y = vz.x`
		if (vz.x % vy.x != 0)
			return Results::fail();
		y = 0;
		x = vz.x / vy.x;
	} else if (vy.y != 0) {
		// Corner case, eq (B) becomes: `vy.y * y = vz.y`
		if (vz.y % vy.y != 0)
			return Results::fail();
		y = 0;
		x = vz.y / vy.y;
	} else {
		return Results::fail();
	}

	return {x, y};
}

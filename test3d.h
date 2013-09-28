#ifndef HPP_TEST3D_H
#define HPP_TEST3D_H

#include "viewfrustum.h"
#include "boundingsphere.h"
#include "transform.h"
#include "angle.h"
#include "vector3.h"
#include "plane.h"
#include "random.h"

#include <vector>
#include <iostream>

namespace Hpp
{

namespace Tests
{

inline void test3D(void)
{

	// Basic transformations
	{
		Transform transf(Vector3(0, 0, 0));
		transf.translate(Vector3(0, 1, 2));
		transf.rotateX(Angle(30));
		transf.rotateY(Angle(20));
		transf.scale(Vector3(3, 4, 5));
		transf.rotateZ(Angle(10));
		Vector3 diff = transf.getPosition() - Vector3(2.348, -0.130, 10.487);
		HppAssert(diff.length() < 0.01, "Testing of basic transformations has failed!");
		diff = transf.getPosition() - transf.applyToPosition(Vector3::ZERO);
		HppAssert(diff.length() < 0.01, "Testing of basic transformations has failed!");
	}

	// Translate::addAnotherTransform()
	{
		Transform transf1(Vector3(0, 1, 2));
		transf1.rotateX(Angle(30));
		Transform transf2(Y, Angle(20));
		transf2.scale(Vector3(3, 4, 5));
		transf2.rotateZ(Angle(10));
		Transform transf = transf1.addAnotherTransform(transf2);
		Vector3 diff = transf.getPosition() - Vector3(2.348, -0.130, 10.487);
		HppAssert(diff.length() < 0.01, "Testing of Translate::addAnotherTransform() has failed!");
		diff = transf.getPosition() - transf.applyToPosition(Vector3::ZERO);
		HppAssert(diff.length() < 0.01, "Testing of Translate::addAnotherTransform() has failed!");
	}

	// Translate::getRelativePositionTo()
	{
		Transform transf(Vector3(5, 7, 9));
		transf.rotateX(Angle(20));
		transf.rotateY(Angle(70));
		Vector3 rel_pos(-10, -21, -32);
		Vector3 abs_pos = transf.applyToPosition(rel_pos);
		Vector3 rel_pos_check = transf.getRelativePositionTo(abs_pos);
		Vector3 diff = rel_pos - rel_pos_check;
		HppAssert(diff.length() < 0.01, "Testing of Translate::getRelativePositionTo() has failed!");
	}

	// Simple viewfrustum test
	{
		std::vector< Vector2 > vrts;
		vrts.push_back(Vector2(1, -1));
		vrts.push_back(Vector2(1, 1));
		vrts.push_back(Vector2(-1, 1));
		vrts.push_back(Vector2(-1, -1));
		Viewfrustum vfrust(Vector3::ZERO, Vector3::Y, Vector3::Z, Vector3::X, vrts);
		Boundingsphere bs1(Vector3(0, 10, 0), 1);
		Boundingsphere bs2(Vector3(2.703, 3.227, 4.656), 1);
		Boundingsphere bs3(Vector3(1.403, -1.020, 0.083), 1);
		Boundingsphere bs4(Vector3(-3.875, 4.760, -5.805), 1);
		HppAssert(vfrust.testBoundingvolume(&bs1) != Viewfrustum::OUTSIDE, "Simple viewfrustum test has failed!");
		HppAssert(vfrust.testBoundingvolume(&bs2) == Viewfrustum::OUTSIDE, "Simple viewfrustum test has failed!");
		HppAssert(vfrust.testBoundingvolume(&bs3) == Viewfrustum::OUTSIDE, "Simple viewfrustum test has failed!");
		HppAssert(vfrust.testBoundingvolume(&bs4) != Viewfrustum::OUTSIDE, "Simple viewfrustum test has failed!");
	}

	// Random vectors
	{
		for (size_t i = 0; i < 100; ++ i) {
			Real min_len = random(0, 4);
			Real max_len = random(min_len + 1, 10);
			Vector2 v2a = randomVector2(max_len, min_len);
			Vector3 v3a = randomVector3(max_len, min_len);
			Vector2 v2b = randomVector2(max_len);
			Vector3 v3b = randomVector3(max_len);
			HppAssert(v2a.length() <= max_len + 0.001, "Vector too long!");
			HppAssert(v2a.length() >= min_len - 0.001, "Vector too short!");
			HppAssert(v3a.length() <= max_len + 0.001, "Vector too long!");
			HppAssert(v3a.length() >= min_len - 0.001, "Vector too short!");
			HppAssert(v2b.length() <= max_len + 0.001, "Vector too long!");
			HppAssert(v3b.length() <= max_len + 0.001, "Vector too long!");
		}
	}

	// Planes
	{
		for (size_t i = 0; i < 100; ++ i) {
			Vector3 plane_pos = randomVector3(100);
			Vector3 plane_normal = randomVector3(1, 1);
			Vector3 plane_binormal = plane_normal.perp();
			Vector3 plane_tangent = crossProduct(plane_normal, plane_binormal);

			Plane plane(plane_normal, plane_pos);

			// Test isPositionAtFront()
			HppAssert(plane.isPositionAtFront(plane_pos + plane_normal + plane_binormal * random(-100, 100) + plane_tangent * random(-100, 100)), "Position should be at front of Plane!");
			HppAssert(!plane.isPositionAtFront(plane_pos - plane_normal + plane_binormal * random(-100, 100) + plane_tangent * random(-100, 100)), "Position should be at front of Plane!");

			// Test distanceToPosition()
			Vector3 random_pos_at_plane = plane_pos + plane_binormal * random(-100, 100) + plane_tangent * random(-100, 100);
			Real distance = random(-100, 100);
			HppAssert(fabs(plane.distanceToPosition(random_pos_at_plane + plane_normal * distance) - distance) < 0.01, "Distance calculation has failed!");

			// Test hitsLine()
			Vector3 hitpos = plane_pos + plane_binormal * random(-100, 100) + plane_tangent * random(-100, 100);
			Vector3 ray_begin = randomVector3(100);
			if (fabs(plane.distanceToPosition(ray_begin)) > 0.01) {
				Vector3 ray_dir = (hitpos - ray_begin).normalized();
				Vector3 hitresult_pos = Hpp::randomVector3(0, 1);
				Real hitresult_dir_mult = Hpp::random(0, 1);
				bool hits = plane.hitsLine(ray_begin, ray_dir, &hitresult_pos, &hitresult_dir_mult);
				HppAssert(hits, "Unable to detect hit!");
				HppAssert((hitresult_pos - hitpos).length() < 0.01, "Hitposition calculation has failed!");
				HppAssert(fabs(hitresult_dir_mult - (hitpos - ray_begin).length()) < 0.01, "Ray dir multiplier calculation has failed!");
				// Also try negative direction
				hits = plane.hitsLine(ray_begin, -ray_dir, &hitresult_pos, &hitresult_dir_mult);
				HppAssert(hits, "Unable to detect hit!");
				HppAssert((hitresult_pos - hitpos).length() < 0.01, "Hitposition calculation has failed!");
				HppAssert(fabs(-hitresult_dir_mult - (hitpos - ray_begin).length()) < 0.01, "Ray dir multiplier calculation has failed!");
			}

		}

		Vector3 edge_begin(2.86699, -4.60328, 8.77518);
		Vector3 edge_end(13.6096, 5.23502, 1.11643);
		Plane plane(Vector3(-0.641728, -0.766933, 0), -2.75939);
		Vector3 result_hitpos;
		Real result_dir_mult;
		HppAssert(plane.hitsLine(edge_begin, edge_end - edge_begin, &result_hitpos, &result_dir_mult), "Unable to detect hit!");

	}

}

}

}

#endif

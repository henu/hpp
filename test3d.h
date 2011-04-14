#ifndef HPP_TEST3D_H
#define HPP_TEST3D_H

#include "viewfrustum.h"
#include "boundingsphere.h"
#include "transform.h"
#include "angle.h"
#include "vector3.h"

#include <vector>

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
		transf.rotateX(Angle::fromDegrees(30));
		transf.rotateY(Angle::fromDegrees(20));
		transf.scale(Vector3(3, 4, 5));
		transf.rotateZ(Angle::fromDegrees(10));
		Vector3 diff = transf.getPosition() - Vector3(2.348, -0.130, 10.487);
		HppAssert(diff.length() < 0.01, "Testing of basic transformations has failed!");
		diff = transf.getPosition() - transf.applyToPosition(Vector3::ZERO);
		HppAssert(diff.length() < 0.01, "Testing of basic transformations has failed!");
	}

	// Translate::addAnotherTransform()
	{
		Transform transf1(Vector3(0, 1, 2));
		transf1.rotateX(Angle::fromDegrees(30));
		Transform transf2(Y, Angle::fromDegrees(20));
		transf2.scale(Vector3(3, 4, 5));
		transf2.rotateZ(Angle::fromDegrees(10));
		Transform transf = transf1.addAnotherTransform(transf2);
		Vector3 diff = transf.getPosition() - Vector3(2.348, -0.130, 10.487);
		HppAssert(diff.length() < 0.01, "Testing of basic transformations has failed!");
		diff = transf.getPosition() - transf.applyToPosition(Vector3::ZERO);
		HppAssert(diff.length() < 0.01, "Testing of basic transformations has failed!");
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
		HppAssert(vfrust.testBoundingsphere(bs1) != Viewfrustum::OUTSIDE, "Simple viewfrustum test has failed!");
		HppAssert(vfrust.testBoundingsphere(bs2) == Viewfrustum::OUTSIDE, "Simple viewfrustum test has failed!");
		HppAssert(vfrust.testBoundingsphere(bs3) == Viewfrustum::OUTSIDE, "Simple viewfrustum test has failed!");
		HppAssert(vfrust.testBoundingsphere(bs4) != Viewfrustum::OUTSIDE, "Simple viewfrustum test has failed!");
	}

}

}

}

#endif

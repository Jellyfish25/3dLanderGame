#ifndef _BOX_H_
#define _BOX_H_

#include <assert.h>
#include "vector3.h"
#include "ray.h"
/*
 * Axis-aligned bounding box class, for use with the optimized ray-box
 * intersection test described in:
 *
 *      Amy Williams, Steve Barrus, R. Keith Morley, and Peter Shirley
 *      "An Efficient and Robust Ray-Box Intersection Algorithm"
 *      Journal of graphics tools, 10(1):49-54, 2005
 *
 */

class Box {
  public:
    Box() { }
    Box(const Vector3 &min, const Vector3 &max) {
 //     assert(min < max);
      parameters[0] = min;
      parameters[1] = max;
    }
    // (t0, t1) is the interval for valid hits
    bool intersect(const Ray &, float t0, float t1) const;

    // corners
    Vector3 parameters[2];

	Vector3 min() { return parameters[0]; }
	Vector3 max() { return parameters[1]; }
	bool inside(const Vector3 &p) const {
		return ((p.x() >= parameters[0].x() && p.x() <= parameters[1].x()) &&
		     	(p.y() >= parameters[0].y() && p.y() <= parameters[1].y()) &&
			    (p.z() >= parameters[0].z() && p.z() <= parameters[1].z()));
	}
	bool inside(Vector3 *points, int size) const {
		bool allInside = true;
		for (int i = 0; i < size; i++) {
			if (!inside(points[i])) allInside = false;
			break;
		}
		return allInside;
	}

	// overlap: returns true if this Box overlaps with box and false otherwise.
	 bool overlap(const Box &box) {
		 Vector3 min = parameters[0], max = parameters[1], bMin = box.parameters[0], bMax = box.parameters[1];
		 return !((max.x() < bMin.x() || max.y() < bMin.y() || max.z() < bMin.z()) 
			   || (min.x() > bMax.x() || min.y() > bMax.y() || min.z() > bMax.z()));
	}

	Vector3 center() {
		return ((max() - min()) / 2 + min());
	}
};

#endif // _BOX_H_

#include "Sphere.hpp"
#include "Ray.hpp"
#include <memory>

namespace rt
{

	bool
		Sphere::closestIntersectionModel(const Ray &ray, double maxLambda, RayIntersection& intersection) const
{

	const Vec3d o = ray.origin();
	const Vec3d d = ray.direction();

	const double b = dot(o, d);
	const double c = dot(o, o);

	const double D2 = b * b - c + 1;

	if (D2 < 0){
		return false;
	}

	double lambda = -b - sqrt(D2);

	if (lambda < 0){
		lambda = -b + sqrt(D2);
	}

	if (lambda < 0 || lambda > maxLambda){
		return false;
	}

	const Vec3d uvw(0, 0, 0);

	intersection = RayIntersection(ray, shared_from_this(), lambda, ray.pointOnRay(lambda), uvw);
	return true;
}

BoundingBox Sphere::computeBoundingBox() const
{
  BoundingBox box;
  box.setMin(Vec3d(-1,-1,-1));
  box.setMax(Vec3d(1,1,1));
  return box;
}

} //namespace rt

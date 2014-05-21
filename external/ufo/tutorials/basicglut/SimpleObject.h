#ifndef _SIMPLEOBJECT_H
#define _SIMPLEOBJECT_H

#include <vector>

#include <gmtl/Point.h>
#include <gmtl/Vec.h>

#include <ufo/Steerable.h>

class SimpleObject
: public ufo::Steerable
{
	public:

		SimpleObject();
		virtual ~SimpleObject();

		// Object methods:
		void draw();

		// Steerable methods:
		void steer( const ufo::SteeringDecision &d, const float elapsedTime);
		gmtl::Point3f getPosition();
		gmtl::Vec3f getVelocity();
		gmtl::Quatf getOrientation();

		void print() const;
	private:
		gmtl::Point3f position;
		gmtl::Vec3f velocity;
		gmtl::Quatf orientation;
		
};

class SimpleObjectStore
{
	public:
		/**
		 * Get the ObjectStore Singleton.
		 */
		//static SimpleObjectStore *the();
		/**
		 * The factory function.
		 * In our case, we use the factory method to register the generated Objects,
		 * so we can access them for drawing and updating.
		 */
		static ufo::Steerable *theFactory ( std::vector<std::pair<std::string,std::string> > *params);

		static const std::vector<SimpleObject *> getObjects() ;
	private:
		//static SimpleObjectStore* theSingleton;
		//SimpleObjectStore();
		/**
		 * A vector containing all SimpleObjects created by the FactoryFunction.
		 */
		static std::vector<SimpleObject *> allObjects;
};
#endif /* _SIMPLEOBJECT_H */

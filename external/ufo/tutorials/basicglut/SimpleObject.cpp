#include "SimpleObject.h"

#include <iostream>

#include <gmtl/gmtl.h>

#include <GL/glut.h>
#include <GL/glu.h>
#include <ufo/SteeringDecision.h>

using namespace std;
using namespace ufo;
using namespace gmtl;

	SimpleObject::SimpleObject()
: position(Point3f(0.0,0.0,0.0)), velocity(Vec3f(0.0,0.0,0.0)), orientation(Quatf())
{
}
SimpleObject::~SimpleObject()
{
}

void SimpleObject::draw()
{
	GLUquadricObj *sphere;
	Vec3f v = velocity;
	Vec3f def = Vec3f(0.0,0.0,-1.0);
	Matrix44f rot;

	sphere = gluNewQuadric();
	normalize(v);
	rot = makeRot<Matrix44f>(v,def);

	glPushMatrix();
	//move to position:
	glTranslatef(position[0],position[1],position[2]);
	gluSphere(sphere, 0.03, 10, 10);
	//rotate cone:
	glMultMatrixf(rot.getData());
	//center cone:
	glTranslatef(-0.2,0.0,0.0);
	// base radius, height, slices, stacks
	glutSolidCone(0.1, 0.4, 5, 3);
	gluDeleteQuadric(sphere);
	glPopMatrix();
}

void SimpleObject::steer( const ufo::SteeringDecision &d, const float elapsedTime )
{
	velocity = d.direction;
	// make sure we don't get too fast:
	if (lengthSquared( velocity) > 4.0f)
	{
		// limit speed to 2 units per second
		normalize(velocity);
		velocity *= 2;
	}
	position += velocity * elapsedTime;
	orientation = d.rotation;
}

gmtl::Point3f SimpleObject::getPosition()
{
	return position;
}

gmtl::Vec3f SimpleObject::getVelocity()
{
	return velocity;
}
gmtl::Quatf SimpleObject::getOrientation()
{
	return orientation;
}

void SimpleObject::print() const
{
	cout << "SimpleObject" <<endl;
}

// SimpleObjectStore:

vector<SimpleObject *> SimpleObjectStore::allObjects = vector<SimpleObject *>();

const vector<SimpleObject *> SimpleObjectStore::getObjects()
{
	return allObjects;
}

ufo::Steerable *SimpleObjectStore::theFactory ( std::vector<std::pair<std::string,std::string> > *params)
{
	if ( params->size() != 0 )
	{
		cout << "WARNING: SimpleObject with parameters!\n" << endl;
	}
	SimpleObject *ret = new SimpleObject();
	//register object:
	allObjects.push_back(ret);
	return ret;
}

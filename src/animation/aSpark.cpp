// Spark.cpp: implementation of the ASpark class.
//
//////////////////////////////////////////////////////////////////////

#include "aSpark.h"
#include <math.h>

#ifndef GRAVITY
#define GRAVITY 9.8f
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

ASpark::ASpark()
{
	//coefficients of restitution equals 0.25
	m_COR = 0.25f;
	m_mass = 1.0;
}

ASpark::ASpark(vec3 color): AParticle()
{
	for (int i = 0; i < 3; i++)
		m_color[i] = color[i];
 
	//coefficients of restitution equals 0.25
	m_COR = 0.25f;
}

ASpark::~ASpark()
{

}

//Set attractor position
void ASpark::setAttractor(vec3 position)
{
	m_attractorPos = position;
}

//Set repeller position
void ASpark::setRepeller(vec3 position)
{
	m_repellerPos = position;
}

void ASpark::setWind(vec3 wind)
{
	m_windForce = wind;
}


void ASpark::update(float deltaT, int extForceMode)
{
	m_deltaT = deltaT;
	if (m_state[10] <= 0.0)
	{
		m_alive = false;
		return;
	}

	if (!(extForceMode & EXT_SPARKFORCES_ACTIVE))
		extForceMode = 0;
	
	computeForces(extForceMode);
	
	updateState(deltaT, EULER);

	resolveCollisions();
	
	
}

vec3 getGravity(ASpark* spark) {
	const double G = 66.73;
	const double m1 = 100;
	vec3 pos = vec3(spark->m_state[0], spark->m_state[1], spark->m_state[2]);
	vec3 distance = pos - spark->m_attractorPos;
	double r = distance.Length();
	vec3 d = distance / r;
	return G * m1 * spark->m_mass / (r * r) * d;
}
void ASpark::computeForces(int extForceMode)
{

	// zero out all forces
	m_state[6] = 0.0;
	m_state[7] = 0.0;
	m_state[8] = 0.0;

	// gravity force
	addForce(m_mass*m_gravity);
	
	// wind force
	if (extForceMode & WIND_ACTIVE)
	{
		//TODO: Add your code here
		addForce(m_windForce);
	}

	if (extForceMode & DRAG_ACTIVE)
	{
		//TODO: Add your code here
		vec3 dragForce;
		const double b = 0.5;
		for (int i = 0; i < 3; i++) {
			dragForce[i] = - b * m_state[i + 3];
		}
		addForce(dragForce);
	}

	// attractor force
	if (extForceMode & ATTRACTOR_ACTIVE)
	{
		//TODO: Add your code here
		vec3 attrForce = getGravity(this);
		addForce(attrForce);
	}

	// repeller force
	if (extForceMode & REPELLER_ACTIVE)
	{
		//TODO: Add your code here
		vec3 gravity = getGravity(this);
		vec3 repForce = -gravity;
		addForce(repForce);
	}

	// random force
	if (extForceMode & RANDOM_ACTIVE)
	{
		//TODO: Add your code here
		vec3 randForce;
		for (int i = 0; i < 3; i++) {
			randForce[i] = rand() % 10 + 1;
		}
		randForce[2] = 0;
		addForce(randForce);
	}

}

void ASpark::resolveCollisions()
{
	//TODO: Add your code here that reverses the y value of the spark velocity vector 
	// when the y position value of the spark is < 0
	if (m_state[1] < 0) {
		m_state[4] *= -.8f;
	}
}

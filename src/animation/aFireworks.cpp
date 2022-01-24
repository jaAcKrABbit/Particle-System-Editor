// aFireworks.cpp: implementation of the AFireworks class.
//
//////////////////////////////////////////////////////////////////////

#include "aFireworks.h"
#include <stdlib.h>
#include <math.h>
#include <iostream>


#ifndef RAD
#define PI 3.14159265358979f
#define RAD (PI / 180.0f)
#endif
#ifndef GRAVITY
#define GRAVITY 9.8f
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

AFireworks::AFireworks()
{
    m_deltaT = 0.033f;
	m_rocketMass = 50.0;
	m_sparkMass = 20.0;

	m_attractorPos = vec3(0.0, 500.0, 0.0);
	m_repellerPos = vec3(0.0, 500.0, 0.0);
	m_windForce = vec3(250.0, 0.0, 0.0);

}

AFireworks::~AFireworks()
{

	for (unsigned int i = 0; i < sparks.size(); i++)
	{
		ASpark* pSpark = sparks[i];
		delete pSpark;
	}
	for (unsigned int i = 0; i < rockets.size(); i++)
	{
		ARocket* pRocket = rockets[i];
		delete pRocket;
	}

	rockets.clear();
	sparks.clear();

}

int AFireworks::getNumParticles()
{
	return sparks.size() + rockets.size();
}

/*
 *	fireRocket is called when user presses the space key.
 *  Input:	vec3 pos. launch position of the rocket.
 *			vec3 vel. launch velocity of the rocket.
 *			vec3 color. color[0], color[1] and color[2] are the RGB color of this rocket.
 *
 *  In this function, you want to generate a ARocket object and add it to the rockets vector
 */
void AFireworks::fireRocket(vec3 pos, vec3 vel, vec3 color)
{
	ARocket *newRocket = new ARocket(color);
	const float m = 50;
	const float g = 9.8;
	// Need to compute and set values for initial state of rocket  (including time to live)
	float stateVals[12] = { 0.0 };

	//TODO: Add your code here
	//012 pos
	for (int i = 0; i < 3; i++) {
		stateVals[i] = pos[i];
	}
	//345 vel
	for (int i = 0; i < 3; i++) {
		stateVals[i+3] = vel[i];
	}
	//678 force
	stateVals[6] = 0;
	stateVals[7] = m * GRAVITY;
	stateVals[8] = 0;
	//mass
	stateVals[9] = m;
	//time to live
	stateVals[10] = ROCKET_LIFESPAN;


	newRocket->setState(stateVals);
	newRocket->setAttractor(m_attractorPos);
	newRocket->setRepeller(m_repellerPos);
	newRocket->setWind(m_windForce);
	rockets.push_back(newRocket);
}


/*	explode is called in AFireworks::update() when a rocket reaches its top height. 
 *  It is called ARocket::TOTALEXPLOSIONS times to generate a series of rings of sparks.
 *  Input: vec3 pos. position where a ring of sparks are generated.
 *		   vec3 vel. velocity where a ring of sparks are generated.
 *		   vec3 color. color[0], color[1] and color[2] are the RGB color of the rocket. It will also be the color of the sparks it generate.                       
 *  In this function, you want to generate a number of sparks that are uniformily distributed on a ring at [posx, posy, posz]
 *  then append them to the sparks vector using a push_back function call.
 *  The initial state vector for each spark should accommodate the constraints below:
 *   They should be evenly distribute on a ring on the local XOY plane of the rocket.
 *  
 *  At the time of the explosion:
 *      the number of sparks generated should be based on a random number between 10 and 60.
 *      the position of the sparks is determined by [posx, posy, posz]
 *      the magnitude of the inital velocity of each spark should  be based on a random value between 20 and 40
 *      the direction of the initial velocity of each spark should be uniformly distributed between 0 and 360 degrees 
 *      the total velocity of the spark at the time of its creation is the sum of the rocket velocity and the intial spark velocity
 *  force on every spark is just the gravity.       
 */
void AFireworks::explode(vec3 pos, vec3 vel, vec3 color)
{	
	const float m = 50;
	float stateVals[12] = { 0.0 };
	//10 to 60
	int sparkNumber = rand() % (MAXSPARKS + 1) + 10;
	//20 to 40
	float speed = rand() % 21 + 20;
	
	//vector perpendicular to yz plane
	vec3 temp(0, pos[1], pos[2]);
	vec3 rotateAxis = pos.Cross(temp);

	float angle = 2 * PI / sparkNumber;
	// TODO: Add your code here to randomize the number of sparks and their initial velocity
	// The mass of the rocket is 50.0, it Explodes for 5 deltaT
	for (int i = 0; i < sparkNumber; i++)
	{
		ASpark* newSpark = new ASpark(color);
		
		// TODO: Add your code here
		float alpha = angle * i;
		mat3 rotation;
		rotation.FromAxisAngle(rotateAxis, alpha);
		vec3 sparkVelocity = (rotation * vel).Normalize();
		
		//012 pos
		for (int i = 0; i < 3; i++) {
			stateVals[i] = pos[i];
		}
		//345 vel
		stateVals[3] = speed * sparkVelocity[0];
		stateVals[4] = speed * sparkVelocity[1];
		stateVals[5] = speed * sparkVelocity[2];

		//678 force
		stateVals[6] = 0;
		stateVals[7] = 50 * GRAVITY;
		stateVals[8] = 0;
		//mass
		stateVals[9] = 50;
		//time to live
		stateVals[10] = SPARK_LIFESPAN;

		newSpark->setState(stateVals);
		newSpark->setAttractor(m_attractorPos);
		newSpark->setRepeller(m_repellerPos);
		newSpark->setWind(m_windForce);
		sparks.push_back(newSpark);
	}
}


// One simulation step 
void AFireworks::update(float deltaT, int extForceMode)
{
	//Step 1. Iterate through every ASpark in sparks. If the spark is dead(life is 0), erase it from sparks.
	//        Code is given. It is also an example of erasing an element from a vector.
	ASpark* pSpark;
	int index = 0;
	m_deltaT = deltaT;

	for (unsigned int i = 0; i < sparks.size(); i++)
	{
		pSpark = sparks[index];
		if (!pSpark->m_alive)
		{
			sparks.erase(sparks.begin() + index);
			delete pSpark;
		}
		else index++;
	}


	//Step 2. Iterate through every ARocket in rockets. If the rocket is dead, erase it from rockets.
	//        If the rockets is in explosion mode generate a ring of sparks.
	
	ARocket* pRocket;	
	index = 0;

	// TODO: Add you code here
	for (unsigned int i = 0; i < rockets.size(); i++)
	{
		pRocket = rockets[index];
		if (pRocket->m_mode == EXPLOSION) {
			explode(pRocket->m_Pos, pRocket->m_Vel, pRocket->m_color);
		}
		if (!pRocket->m_alive)
		{
			rockets.erase(rockets.begin() + index);
			delete pRocket;
		}
		else index++;
	}
	//Step 3. Euler steps for valid sparks and rockets.
	//        Code is given.

	for (unsigned int i = 0; i < sparks.size(); i++)
	{
		ASpark* pSpark = sparks[i];
		pSpark->update(m_deltaT, extForceMode);
	}
	
	for (unsigned int i = 0; i < rockets.size(); i++)
	{
		ARocket* pRocket = rockets[i];
		pRocket->update(m_deltaT, extForceMode);
	}
}
#include "actor.hpp"

namespace Core {

	using namespace ei;

	void DynamicActor::Process(float _deltaTime)
	{
		m_position += m_velocity * _deltaTime;

		// Create a delta Quaternion from rotation velocity
		// http://physicsforgames.blogspot.de/2010/02/quaternions.html
		Quaternion deltaRot;
		Vec3 theta = m_angularVelocity * _deltaTime;
		float thetaMagSq = lensq(theta);
		float s;
		if (thetaMagSq * thetaMagSq / 24.0f < 1e-6f)
		{
			deltaRot.r = 1.0f - thetaMagSq / 2.0f;
			s = 1.0f - thetaMagSq / 6.0f;
		}
		else
		{
			float thetaMag = sqrt(thetaMagSq);
			deltaRot.r = cos(thetaMag);
			s = sin(thetaMag) / thetaMag;
		}
		deltaRot.i = theta[0] * s;
		deltaRot.j = theta[1] * s;
		deltaRot.k = theta[2] * s;

		Rotate(deltaRot);
	}
}
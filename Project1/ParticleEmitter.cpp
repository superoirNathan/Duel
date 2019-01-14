//Nathan Alphonse 100523109
//100555192 Janarthan Ravikumar

#include <iostream> // for std::cout
#include <glm\gtc\random.hpp> // for glm::linearRand

#include "AnimationMath.h"
#include "ParticleEmitter.h"

ParticleEmitter::ParticleEmitter()
//	: m_pParticles(nullptr),
//	m_pNumParticles(0),
//	playing(true),
//	interpolateColour(false)
{
    //

    playing = true;
    interpolateColour = false;

    //Physics properties
    velocity0 = glm::vec3(-0.005f, -0.005f, -0.005f);
    velocity1 = glm::vec3(0.005f, 0.005f, 0.05f);
    emitterPosition = glm::vec3(0.f, 0.f, 0.f);
    boxWH = glm::vec3(5.0f);
    boxOn = false;
    //Steer properties                         
    taretgOn = false;
    fleeOn = false;
    magnetOn = false;
    repelOn = false;
    gravityOn = false;
    pathOn = false;

    targetPos = glm::vec3(0.0f);
    fleePos = glm::vec3(0.0f);
    magnetPos = glm::vec3(0.0f);
    repelPos = glm::vec3(0.0f);
    gravity = glm::vec3(0.0f, 1.0f, 0.0f);
    pathEnd = glm::vec3(0.0f);

    targetStrength = 0.0f;
    fleeStrength = 0.0f;
    magnetStrength = 0.0f;
    magnetRadius = 0.0f;
    repelStrength = 0.0f;
    repelRadius = 0.0f;

    // Range Properties
    colour0 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    colour1 = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
    lifeRange = glm::vec2(1.0f, 1.0f);
    sizeRange = glm::vec2(1.0f, 1.0f);
    spawnRange = glm::vec2(50.0f, 50.0f);
    massRange = glm::vec2(0.5f, 0.75f);

    emitterName = "emitter";

    //other
    durationOn = true;
    durationRepeat = false;
    durationOnX_OffY = glm::vec2(0.0f, 0.0f);
    catmullT0 = glm::vec3(0);
    catmullT1 = glm::vec3(0);
    pathPointStrength = 0.0f;
    step = 7;
    showEmitter = true;
    circleSquare = true;

    keyframeController.doesSpeedControl = true;

    keyframeController.addKey(catmullT0);
    keyframeController.addKey(emitterPosition);
    keyframeController.addKey(pathEnd);
    keyframeController.addKey(catmullT1);

    keyframeController.calculateLookupTable(4);

 //   emitterList.push_back(new ParticleEmitter(emitter));

 //   emitterList[0]->initialize(500);
 //   initialize(500);

    initialize(300); //change this to change trail amount
}

ParticleEmitter::~ParticleEmitter()
{
	freeMemory();
}

void ParticleEmitter::initialize(unsigned int numParticles)
{
	freeMemory(); // destroy any existing particles

	if (numParticles > 0)
	{
		m_pParticles = new Particle[numParticles];
		memset(m_pParticles, 0, sizeof(Particle) * numParticles);
		m_pNumParticles = numParticles;
	}
        emitterName = "emitter";

}

void ParticleEmitter::freeMemory()
{
//	if (m_pParticles) // if not a null pointer
//	{
//		delete[] m_pParticles;
//		m_pParticles = nullptr;
//		m_pNumParticles = 0;
//	}
}

void ParticleEmitter::update(float dt)
{
    // if any force is active, calulate forces
    if ((taretgOn) || (fleeOn) || (magnetOn) || (repelOn) || (gravityOn) || (pathOn)) {
        applyForcesToParticleSystem();
    }

    if (pathOn) {
        // All keyframe and tvalue management replaced with a single line of code!
        keyframeController.calculateLookupTable(step);
        interpolatedPosition = keyframeController.update(dt);

        keyframeController.setKey(0, catmullT0);
        keyframeController.setKey(1, emitterPosition);
        keyframeController.setKey(2, pathEnd);
        keyframeController.setKey(3, catmullT1);

//        pointHandles[0].position = emitterPosition;
//        pointHandles[1].position = pathEnd;
    }


    TotalT += dt;

	if (m_pParticles && playing) // make sure memory is initialized and system is playing
	{
		// loop through each particle
		Particle* particle = m_pParticles;
		for (unsigned int i = 0; i < m_pNumParticles; ++i, ++particle)
		{
            if (durationOn) {
                float randomTvalRate = glm::linearRand(0, 1);
                float tempRate = Math::lerp(spawnRange.x, spawnRange.y, randomTvalRate);


                if (particle->life <= 0 && (TotalT >= (1 / tempRate))) // if particle has no life remaining
                {
                    glm::vec3 tempEmitter = glm::vec3(0.f);

                    if (boxOn == true) {
                        randomTvalRate = glm::linearRand(0.f, 1.f);
                        tempEmitter.x = Math::lerp(0.f - boxWH.x, boxWH.x, randomTvalRate);
                        randomTvalRate = glm::linearRand(0.f, 1.f);
                        tempEmitter.y = Math::lerp(0.f - boxWH.y, boxWH.y, randomTvalRate);
                        randomTvalRate = glm::linearRand(0.f, 1.f);
                        tempEmitter.z = Math::lerp(0.f - boxWH.z, boxWH.z, randomTvalRate);

                        //              std::cout << randomTvalRate <<":: "<< tempEmitter.x << " , " << tempEmitter.y << std::endl;
                    }

                    TotalT = 0;
                    // Respawn particle
                    // Note: we are not freeing memory, we are "Recycling" the particles
                    float randomTval = glm::linearRand(0.0f, 1.0f);
                    if (gravityOn) {
                        particle->acceleration = glm::vec3(-9.8) * gravity;
                    }
                    else {
                        particle->acceleration = glm::vec3(0.0f);
                    }

                    particle->colour = Math::lerp(colour0, colour1, randomTval);
                    particle->life = Math::lerp(lifeRange.x, lifeRange.y, randomTval);
                    particle->mass = Math::lerp(massRange.x, massRange.y, randomTval);
                    particle->position = emitterPosition + tempEmitter;
                    particle->size = Math::lerp(sizeRange.x, sizeRange.y, randomTval);
                    particle->velocity = Math::lerp(velocity0, velocity1, randomTval);

                }

            }// if duration end

                // Update physics


                // Update acceleration
                particle->acceleration = particle->force / particle->mass;
                particle->velocity = particle->velocity + (particle->acceleration * dt);
//                particle->position = glm::vec3(parent->transform * glm::vec4(0, 0, 0, 1.0f)) + particle->velocity * dt;
                particle->position = particle->position + particle->velocity * dt;

                // We've applied the force, let's remove it so it does not get applied next frame
                particle->force = glm::vec3(0.0f);

                // Decrease particle life
                particle->life -= dt;

                // Update visual properties
                if (interpolateColour)
                {
                    // calculate t value
                    float tVal = Math::invLerp(particle->life, lifeRange.x, lifeRange.y);
                    particle->colour = Math::lerp(colour0, colour1, tVal);
                }
                if (particle->life <= 0) { // if particle has no life remaining put it below scene
                    particle->position = glm::vec3(-100000.0f);
                }
		}// forloop end

	}// if playing end
}

bool ParticleEmitter::checkAllPartDead() {
    Particle* particle = m_pParticles;
    bool tempCheck = false;
    for (unsigned int i = 0; i < m_pNumParticles; ++i, ++particle)
    {
        if (particle->life <= 0) { // if particle has no life

            tempCheck = true;
        }
    }
    return tempCheck;
}

void ParticleEmitter::draw()
{
	// Draw the emitter position
	// Note: not necessary
//    if (showEmitter) {
//    }
//	Particle* p = m_pParticles;
//	for (int i = 0; i < m_pNumParticles; ++i, ++p)
//	{
//		if (p->life >= 0.0f) // if particle is alive, draw it
//		{
//
//		}
//	}


}

void ParticleEmitter::applyForcesToParticleSystem()
{
    // TODO: implement seeking
    // Loop through each particle in the emitter and apply a seeking for to them

    glm::vec3 seekVec, fleeVec, magVec, repelVec, pathVec;
    glm::vec3 seekDirection, fleeDirection, magnetDirection, repelDirection, pathDirection;
    glm::vec3 seekForce, fleeForce, magnetForce, repelForce, gravityForce, pathForce;


    for (int i = 0; i <= getNumParticles() - 1; i++) {
        if (getParticleLife(i) > 0) {

            //target is on
            if (taretgOn == true) {
                seekVec = targetPos - getParticlePosition(i);
                seekDirection = glm::normalize(seekVec);
                seekForce = seekDirection *(targetStrength);
            }
            // flee is on
            if (fleeOn == true) {
                fleeVec = fleePos - getParticlePosition(i);
                fleeDirection = glm::normalize(-fleeVec);
                fleeForce = fleeDirection *(fleeStrength);
            }
            // Magnet is on
            if (magnetOn == true) {
                magVec = magnetPos - getParticlePosition(i);
                magnetDirection = glm::normalize(magVec);
                if (glm::length(magVec) <= magnetRadius)
                    setParticleVelocity(i, magnetDirection* (abs(getParticleVelocity(i))));
                else
                    magnetForce = (magnetDirection)*((magnetStrength) / (glm::length(magVec) / 100.0f));
            }
            // Repel is on
            if (repelOn == true) {
                repelVec = repelPos - getParticlePosition(i);
                repelDirection = glm::normalize(-repelVec);
                if (glm::length(repelVec) <= repelRadius)
                    setParticleVelocity(i, repelDirection*(abs(getParticleVelocity(i))));
                else
                    repelForce = repelDirection *(repelStrength) / glm::length(-repelVec / 100.0f);
            }
            if (gravityOn == true) {
                gravityForce = gravity;
            }
            if (pathOn) {
                // Seek point between steps
                glm::vec3 p0, p1, p2, p3;
                p0 = catmullT0;
                p1 = emitterPosition;
                p2 = pathEnd;
                p3 = catmullT1;
                //itereate a seek function at all points on spline
                for (float j = 0.0f; j < 1.0f;)
                {
                    //                      glm::vec3 catPoint0 = Math::catmull(p0, p1, p2, p3, i);
                    glm::vec3 catPoint = Math::catmull(p0, p1, p2, p3, j += (1 / step));
                    pathVec = catPoint - getParticlePosition(i);
                    pathDirection = glm::normalize(pathVec);


                    pathForce += pathDirection * pathPointStrength;
                }
                setParticleVelocity(i, pathDirection* (abs(getParticleVelocity(i))));
            }

            //Total Force
            glm::vec3 totalForce = seekForce + fleeForce + magnetForce + repelForce + gravityForce + pathForce;
            applyForceToParticle(i, totalForce);
        }
    }
}




void ParticleEmitter::applyForceToParticle(unsigned int idx, glm::vec3 force)
{
	if (idx >= m_pNumParticles)
	{
		std::cout << "ParticleEmitter::applyForceToParticle ERROR: idx " << idx << "out of range!" << std::endl;
		return;
	}

	m_pParticles[idx].force = force;
}

glm::vec3 ParticleEmitter::getParticlePosition(unsigned int idx)
{
	if (idx >= m_pNumParticles)
	{
		std::cout << "ParticleEmitter::getParticlePosition ERROR: idx " << idx << "out of range!" << std::endl;
		return glm::vec3();
	}

	return m_pParticles[idx].position;
}

glm::vec3 ParticleEmitter::getParticleVelocity(unsigned int idx)
{
    if (idx >= m_pNumParticles)
    {
        std::cout << "ParticleEmitter::getParticlePosition ERROR: idx " << idx << "out of range!" << std::endl;
        return glm::vec3();
    }

    return m_pParticles[idx].position;
}
glm::vec3 ParticleEmitter::getParticleColor(unsigned int idx)
{
    if (idx >= m_pNumParticles)
    {
        std::cout << "ParticleEmitter::getParticlePosition ERROR: idx " << idx << "out of range!" << std::endl;
        return glm::vec3();
    }

    return m_pParticles[idx].colour;
}
void ParticleEmitter::setParticleVelocity(unsigned int idx, glm::vec3 newVeloctiy)
{
    if (idx >= m_pNumParticles)
    {
        std::cout << "ParticleEmitter::getParticlePosition ERROR: idx " << idx << "out of range!" << std::endl;
    }

    m_pParticles[idx].velocity = newVeloctiy;
}
void ParticleEmitter::setParticleAcceleration(unsigned int idx, glm::vec3 addedAcceleration)
{
    if (idx >= m_pNumParticles)
    {
        std::cout << "ParticleEmitter::getParticlePosition ERROR: idx " << idx << "out of range!" << std::endl;
    }

    m_pParticles[idx].acceleration = addedAcceleration;

}
float ParticleEmitter::getParticleLife(unsigned int idx)
{
    if (idx >= m_pNumParticles)
    {
        std::cout << "ParticleEmitter::getParticlePosition ERROR: idx " << idx << "out of range!" << std::endl;
        return float();
    }
    return m_pParticles[idx].life;
}



//Nathan Alphonse 100523109
//100555192 Janarthan Ravikumar

#pragma once

#include <glm/glm.hpp>
#include "KeyframeController.h"

// Params for each particle
// Note: this is a bare minimum particle structure
// Add any properties you want to control here
struct Particle
{
    // Physics properties
    // Position is where the particle currently is
    // Velocity is the rate of change of position
    // Acceleration is the rate of change of velocity
    // p' = p + v*dt
    // current position is the previous position plus the change in position multiplied by the amount of time passed since we last calculated position
    // v' = v + a*dt
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
    glm::vec3 force;
    float mass;

    // Visual Properties
    float size;
    glm::vec4 colour;
    // Other properties... 
    // ie. sprite sheet

    float life;
};

// Emitter is responsible for emitting (creating, dispatching) particles
class ParticleEmitter
{
public:
private:
    unsigned int m_pNumParticles;
    float TotalT;
    Particle* m_pParticles;
public:
    ParticleEmitter();
    ~ParticleEmitter();


    void initialize(unsigned int numParticles);
    void freeMemory();

    void update(float dt);
    void draw();

    void applyForceToParticle(unsigned int idx, glm::vec3 force);

    //setters and getters for particles
    unsigned int getNumParticles() { return m_pNumParticles; }

    glm::vec3 getParticlePosition(unsigned int idx);
    glm::vec3 getParticleVelocity(unsigned int idx);
    glm::vec3 getParticleColor(unsigned int idx);

    void setParticleVelocity(unsigned int idx, glm::vec3 newVelocity);
    void setParticleAcceleration(unsigned int idx, glm::vec3 newVelocity);

    float getParticleLife(unsigned int idx);

    void applyForcesToParticleSystem();

    ///// Playback properties
    bool playing;

    //// Update flags
    bool interpolateColour;

    ///// Initial properties for newly spawned particles

    glm::vec3 emitterPosition;
    // Box emmiter
    glm::vec3 boxWH;
    bool boxOn;
    // Steering Target
    glm::vec3 targetPos;
    glm::vec3 fleePos;
    float targetStrength;
    float fleeStrength;
    bool taretgOn;
    bool fleeOn;
    // Emitter Gravity
    glm::vec3 gravity;
    bool gravityOn;
    // Emitter Magnet and Repel
    glm::vec3 magnetPos;
    glm::vec3 repelPos;
    float magnetRadius;
    float repelRadius;
    float magnetStrength;
    float repelStrength;
    bool magnetOn;
    bool repelOn;
    // Emitter Path
    bool pathOn;
    glm::vec3 pathEnd;

    // Emitter Duration of Effect
    bool durationRepeat;
    bool durationOn;
    glm::vec2 durationOnX_OffY;
    // Emitter Strings
    std::string emitterName;
    // Storing ranges for each property so each particle has some randomness
    glm::vec3 velocity0;
    glm::vec3 velocity1;

    // these properties are single floats, so we can pack the min and max into a vec2, just data!
    glm::vec2 lifeRange;
    glm::vec2 sizeRange;
    glm::vec2 massRange;

    glm::vec2 spawnRange;
    glm::vec4 colour0;
    glm::vec4 colour1;
    // Keyframes
    KeyframeController<glm::vec3> keyframeController; // holds and updates our keyframe animation
    glm::vec3 catmullT0;
    glm::vec3 catmullT1;
    float pathPointStrength;
    float step;
    bool showEmitter;
    //reset value
    float elapsedTime;
    glm::vec3 interpolatedPosition;
    // other
    bool circleSquare;



    // ... what would be a better way of doing this?
    // Make a keyframe controller for each property! this gives you max control!!
    // See the KeyframeController class
    // (this is what full out particle editors do, ie popcorn fx)
};
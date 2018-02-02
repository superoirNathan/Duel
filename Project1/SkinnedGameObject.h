//Nathan Alphonse 100523109
//Janarthan Ravikumar 100555192

#pragma once

#include "GameObject.h"
#include "HTRLoader.h"
#include "KeyframeController.h"

// Forward declare the HTRLoader class
// This is necessary because SkinnedGameObject references HTRLoader and HTRLoader references SkinnedGameObject
class HTRLoader;

class SkinnedGameObject : public GameObject
{
    // Let the HTRLoader class access private and protected member variables
    friend class HTRLoader;

protected:
    int m_pCurrentFrame;
    HTRLoader* m_pHTRAnimation;
    JointDescriptor* m_pJointAnimation; // The animation for this specific joint

    std::shared_ptr<Mesh> m_pBindMesh; // Mesh in t-pose
    glm::mat4 m_pJointToBindMat;
    glm::mat4 m_pJointToInvMat;
    bool m_pJointToInvMatCalutated;
public:
    SkinnedGameObject();
    ~SkinnedGameObject();

    //skin mesh var
    bool m_pPlaying;
    bool m_pDrawMesh;
    bool m_pDrawBindPose;
    bool m_pRootPose;
    bool m_pXrayJoints;
    bool reverse;
    bool seekTarget;

    bool childIk = 0;
    bool parentIK = 0;

    // Keyframes
    KeyframeController<glm::vec3> keyframeController; // holds and updates our keyframe animation
 //   KeyframeController<int> modelKeyController; // holds and updates our keyframe animation

    glm::vec3 catmullT0;
    glm::vec3 catmullT1;
    float step;
    float curvePlaySpeed;

    glm::vec3 interpolatedPosition;


    glm::vec3 targetPos;

    void initializeSkeletonFromHTR(std::string htrFilePath, std::string skinWeightFilePath, std::shared_ptr<Mesh> bindMesh);

    glm::mat4 getJointToBindMatrix();

    //skin deform

    void setXRayJoints(bool xray);
    void setFrameJoints(int frameNumb);
    void checkUpIK();

    void drawMatrix();

    virtual void update(float dt);
};
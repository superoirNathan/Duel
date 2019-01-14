#pragma once
#include <string>
#include <GL/glew.h>
#include <glm\glm.hpp>
#include <vector>

struct MeshFace {
    MeshFace() {
        vertices[0] = 0;
        vertices[1] = 0;
        vertices[2] = 0;

        textureUVs[0] = 0;
        textureUVs[1] = 0;
        textureUVs[2] = 0;

        normals[0] = 0;
        normals[1] = 0;
        normals[2] = 0;


    }
    MeshFace(unsigned int v1, unsigned int v2, unsigned int v3,
        unsigned int t1, unsigned int t2, unsigned int t3,
        unsigned int n1, unsigned int n2, unsigned int n3)
    {

        vertices[0] = v1;
        vertices[1] = v2;
        vertices[2] = v3;

        textureUVs[0] = t1;
        textureUVs[1] = t2;
        textureUVs[2] = t3;

        normals[0] = n1;
        normals[1] = n2;
        normals[2] = n3;

    }

    unsigned int vertices[3];
    unsigned int textureUVs[3];
    unsigned int normals[3];
};

class Mesh {

public:
    Mesh();
    ~Mesh();
    //load a mesh and send it to openGL
    bool loadFromFile(const std::string &file);

    //release data from openGL (VRAM) memory
    void unload();

    unsigned int getNumFaces();
    unsigned int getNumVertices();

    //OpenGl buffers an object
    //Vertex Buffer Object
    //an object that store out vertices and such
	GLuint vboVertices = 0;
	GLuint vboUVs = 0;
	GLuint vboNormals = 0;
	GLuint vboTangents = 0;
	GLuint vboBiTangents = 0;
	GLuint vao = 0;

    // Containers for OBJ data
    std::vector<glm::vec3> objVertices;
    std::vector<glm::vec3> objUVs;
    std::vector<glm::vec3> objNormals;

    float width = 0, height = 0, depth = 0;

    //Geometry Shader Floats
    float geoWidth = 1.0f, geoHeight = 1.0f, geoTime = 1.0f;

    unsigned int numFaces = 0;
    unsigned int numVertices = 0;
    // MeshSkin Var
    void updateMesh(bool updateVertices, bool updateNormals, bool updateUVs, bool calculateNormals);
    ///    //Index / face Data
    std::vector<MeshFace> faceData;

    //Particles
    void updateParticlePos(bool updateVertices, std::vector<glm::vec3*> positions);
    //StaticQuad
    void updateQuad(bool updateVertex, glm::vec3 position);
};


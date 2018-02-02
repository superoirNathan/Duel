#define _CRT_SECURE_NO_WARNINGS

#include "Mesh.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "glm\glm.hpp"

#define BUFFER_OFFSET(i)((char*)0+(i))
Mesh::Mesh() 
  {
}

Mesh::~Mesh() {

}

bool Mesh::loadFromFile(const std::string &file) {
	std::ifstream input;
	input.open(file);

	if (!input) {
		std::cout << "Mesh.loadFromFile: Could not open" << file << std::endl;
		return false;
	}

	std::string inputString;

    ///    //Release data from opengl (VRAM)
    std::vector<float>unpackedVertexData;
    std::vector<float>unpackedTextureData;
    std::vector<float>unpackedNormalData;

    ///    //Uniquw Date
    std::vector<glm::vec3> vertexData;
    std::vector<glm::vec2> textureData;
    std::vector<glm::vec3> normalData;

    ///    //Index / face Data
    faceData.clear();
    // Containers for OBJ data
    objVertices.clear();
    objUVs.clear();
    objNormals.clear();


	while (std::getline(input, inputString))
	{
		if (inputString[0] == '#')
		{
			// This line is a comment, skip
			continue;
		}
		else if (inputString[0] == 'v')
		{
			if (inputString[1] == ' ')
			{
				// This is a vertex data
				glm::vec3 temp;

				std::sscanf(inputString.c_str(), "v %f %f %f", &temp.x, &temp.y, &temp.z);

				vertexData.push_back(temp);
                objVertices.push_back(temp);
			}
			else if (inputString[1] == 't')
			{
				// This is UV data
				glm::vec2 temp;

				std::sscanf(inputString.c_str(), "vt %f %f", &temp.x, &temp.y);

				textureData.push_back(temp);
                objUVs.push_back(glm::vec3(temp.x, temp.y, temp.x));
			}
			else if (inputString[1] == 'n')
			{
				// This is Normal data
				glm::vec3 temp;

				std::sscanf(inputString.c_str(), "vn %f %f %f", &temp.x, &temp.y, &temp.z);

				normalData.push_back(temp);
                objNormals.push_back(temp);
			}
		}
		else if (inputString[0] == 'f')
		{
			// This is face data
			MeshFace temp;

			std::sscanf(inputString.c_str(), "f %u/%u/%u %u/%u/%u %u/%u/%u",
				&temp.vertices[0], &temp.textureUVs[0], &temp.normals[0],
				&temp.vertices[1], &temp.textureUVs[1], &temp.normals[1],
				&temp.vertices[2], &temp.textureUVs[2], &temp.normals[2]);

			faceData.push_back(temp);
		}
		else
		{
			std::cout << "Line not recognized, skipping: " << inputString << std::endl;
			continue;
		}

	}
	input.close();

	//unpack data that we just read from OBJ file
	for (unsigned int i = 0; i < faceData.size(); i++) {
		for (unsigned int j = 0; j < 3; j++) {
			unpackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].x);
			unpackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].y);
			unpackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].z);

			unpackedTextureData.push_back(textureData[faceData[i].textureUVs[j] - 1].x);
			unpackedTextureData.push_back(textureData[faceData[i].textureUVs[j] - 1].y);

			unpackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].x);
			unpackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].y);
			unpackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].z);
		
           
        
        }
	}

	numFaces = faceData.size();
	numVertices = numFaces * 3;

	//Send Data to OpenGL
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vboVertices);
	glGenBuffers(1, &vboUVs);
	glGenBuffers(1, &vboNormals);

	glBindVertexArray(vao);

	glEnableVertexAttribArray(0); //vertexs
	glEnableVertexAttribArray(1); //UVs
	glEnableVertexAttribArray(2); //normals

	//Vertices VBO Setup
	glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*unpackedVertexData.size(),
		&unpackedVertexData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	//VBO UVs Setup
	glBindBuffer(GL_ARRAY_BUFFER, vboUVs);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*unpackedTextureData.size(),
		&unpackedTextureData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));

	//Normals VBO Setup
	glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*unpackedNormalData.size(),
		&unpackedNormalData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	//Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);
	return true;


}


void Mesh::unload() {

	//in reverse order cause practise thing
	glDeleteBuffers(1, &vboNormals);
	glDeleteBuffers(1, &vboUVs);
	glDeleteBuffers(1, &vboVertices);
	glDeleteVertexArrays(1, &vao);

	vboNormals = NULL;
	vboUVs = NULL;
	vao = NULL;
	vao = NULL;

	numFaces = 0;
	numVertices = 0;
}

unsigned int Mesh::getNumFaces() {
	return numFaces;
}

unsigned int Mesh::getNumVertices() {
	return numVertices;
}


void Mesh::updateMesh(bool updateVertices, bool updateNormals, bool updateUVs, bool calculateNormals)
{
   
    numFaces = faceData.size();
    numVertices = numFaces * 3;
    if (objVertices.size() != 0 && objNormals.size() !=0) {

        ///    //Release data from opengl (VRAM)
        std::vector<float>unpackedVertexData;
        std::vector<float>unpackedTextureData;
        std::vector<float>unpackedNormalData;

        //unpack data that we just read from OBJ file
        for (unsigned int i = 0; i < faceData.size(); i++) {
            for (unsigned int j = 0; j < 3; j++) {
                unpackedVertexData.push_back(objVertices[faceData[i].vertices[j] - 1].x);
                unpackedVertexData.push_back(objVertices[faceData[i].vertices[j] - 1].y);
                unpackedVertexData.push_back(objVertices[faceData[i].vertices[j] - 1].z);

                unpackedTextureData.push_back(objUVs[faceData[i].textureUVs[j] - 1].x);
                unpackedTextureData.push_back(objUVs[faceData[i].textureUVs[j] - 1].y);

                unpackedNormalData.push_back(objNormals[faceData[i].normals[j] - 1].x);
                unpackedNormalData.push_back(objNormals[faceData[i].normals[j] - 1].y);
                unpackedNormalData.push_back(objNormals[faceData[i].normals[j] - 1].z);

            }
        }

        //Send Data to OpenGL
        if (vao == 0)
        {
            glGenVertexArrays(1, &vao);
            glGenBuffers(1, &vboVertices);
            glGenBuffers(1, &vboUVs);
            glGenBuffers(1, &vboNormals);

            glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*unpackedVertexData.size(), &unpackedVertexData[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, vboUVs);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*unpackedTextureData.size(), &unpackedTextureData[0], GL_STATIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float)*unpackedNormalData.size(), &unpackedNormalData[0], GL_STATIC_DRAW);
        }

        glBindVertexArray(vao);

        glEnableVertexAttribArray(0); //vertexs
        glEnableVertexAttribArray(1); //UVs
        glEnableVertexAttribArray(2); //normals
                                      // Unpack data
        if (updateVertices) {
            //Vertices VBO Setup
            glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*unpackedVertexData.size(), &unpackedVertexData[0]);
            glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));
        }

        if (updateUVs) {
            //VBO UVs Setup
            glBindBuffer(GL_ARRAY_BUFFER, vboUVs);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*unpackedTextureData.size(), &unpackedTextureData[0]);
            glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, BUFFER_OFFSET(0));
        }
        if (updateNormals || calculateNormals) {
            //Normals VBO Setup
            glBindBuffer(GL_ARRAY_BUFFER, vboNormals);
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*unpackedNormalData.size(), &unpackedNormalData[0]);
            glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));
        }
        //Cleanup
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);
    }

}

//for (int i = 0; i < faceData.size(); i++)
//{
//    MeshFace* face = &faceData[i];
//
//    {
//        unpackedVertexData.push_back(vertexData[face->vertices[0] -1]);
//        unpackedVertexData.push_back(vertexData[face->vertices[1] -1]);
//        unpackedVertexData.push_back(vertexData[face->vertices[2] -1]);
//    }
//
//    if (updateNormals)
//    {
//        normals.push_back(normalData[face->normals[0] - 1]);
//        normals.push_back(normalData[face->normals[1] - 1]);
//        normals.push_back(normalData[face->normals[2] - 1]);
//    }
//
//    // Per-face normal
//    if (calculateNormals)
//    {
//        normals.push_back(glm::normalize(objNormals[face->vertices[0] - 1]));
//        normals.push_back(glm::normalize(objNormals[face->vertices[1] - 1]));
//        normals.push_back(glm::normalize(objNormals[face->vertices[2] - 1]));
//    }
//
//    if (updateUVs)
//    {
//        textureCoordinates.push_back(objUVs[face->textureUVs[0] - 1]);
//        textureCoordinates.push_back(objUVs[face->textureUVs[1] - 1]);
//        textureCoordinates.push_back(objUVs[face->textureUVs[2] - 1]);
//    }
//}
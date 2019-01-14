#define _CRT_SECURE_NO_WARNINGS
#include "Mesh.h"
#include <algorithm>
#include <vector>
#include <fstream>
#include <iostream>
#include "glm\glm.hpp"

#define BUFFER_OFFSET(i)((char*)0+(i))

Mesh::Mesh() {

}

Mesh::~Mesh() {

}

bool Mesh::loadFromFile(const std::string &file) {
	std::ifstream input;
	input.open(file);

	if (!input) {
		std::cout << "Mesh.loadFromFile: Could not open" << file << std::endl;
		system("pause");
		exit(0);
		return false;
	}

	std::string inputString;

	//Unique Date
	std::vector<glm::vec3> vertexData;
	std::vector<glm::vec2> textureData;
	std::vector<glm::vec3> normalData;

	//Release data from opengl (VRAM)
	std::vector<float>unpackedVertexData;
	std::vector<float>unpackedTextureData;
	std::vector<float>unpackedNormalData;
	std::vector<float>tangentData;
	std::vector<float>biTangentData;


	//    //Index / face Data
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
			//            std::cout << "Line not recognized, skipping: " << inputString << std::endl; // we don't listen to this anyways
			continue;
		}
	}
	input.close();

	float minX, maxX, minY, maxY, minZ, maxZ;
	minX = maxX = vertexData[faceData[0].vertices[0] - 1].x;
	minY = maxY = vertexData[faceData[0].vertices[0] - 1].y;
	minZ = maxZ = vertexData[faceData[0].vertices[0] - 1].z;

	//unpack data that we just read from OBJ file
	for (unsigned int i = 0; i < faceData.size(); i++)
	{
		// This is 1 triangle basically
		glm::vec3 vertexA, vertexB, vertexC;
		glm::vec2 uvA, uvB, uvC;

		vertexA = vertexData[faceData[i].vertices[0] - 1];
		vertexB = vertexData[faceData[i].vertices[1] - 1];
		vertexC = vertexData[faceData[i].vertices[2] - 1];

		uvA = textureData[faceData[i].textureUVs[0] - 1];
		uvB = textureData[faceData[i].textureUVs[1] - 1];
		uvC = textureData[faceData[i].textureUVs[2] - 1];

		// Tangent Calculations
		glm::vec3 deltaPosA, deltaPosB;
		glm::vec2 deltaUVA, deltaUVB;

		deltaPosA = vertexB - vertexA;
		deltaPosB = vertexC - vertexA;

		deltaUVA = uvB - uvA;
		deltaUVB = uvC - uvA;

		// Not sure what this really does
		float f = 1.0f / (deltaUVA.x * deltaUVB.y - deltaUVA.y * deltaUVB.x);

		glm::vec3 tangent, biTangent;
		tangent = f * (deltaPosA * deltaUVB.y - deltaPosB * deltaUVA.y);
		biTangent = f * (deltaPosB * deltaUVA.x - deltaPosA * deltaUVB.x);



		for (unsigned int j = 0; j < 3; j++)
		{
			glm::vec3 vert = vertexData[faceData[i].vertices[j] - 1];
			unpackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].x);
			unpackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].y);
			unpackedVertexData.push_back(vertexData[faceData[i].vertices[j] - 1].z);

			maxX = std::fmaxf(vert.x, maxX);
			minX = std::fminf(vert.x, minX);
			maxY = std::fmaxf(vert.y, maxY);
			minY = std::fminf(vert.y, minY);
			maxZ = std::fmaxf(vert.z, maxZ);
			minZ = std::fminf(vert.z, minZ);

			unpackedTextureData.push_back(textureData[faceData[i].textureUVs[j] - 1].x);
			unpackedTextureData.push_back(textureData[faceData[i].textureUVs[j] - 1].y);

			unpackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].x);
			unpackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].y);
			unpackedNormalData.push_back(normalData[faceData[i].normals[j] - 1].z);

			tangentData.push_back(tangent.x);
			tangentData.push_back(tangent.y);
			tangentData.push_back(tangent.z);

			biTangentData.push_back(biTangent.x);
			biTangentData.push_back(biTangent.y);
			biTangentData.push_back(biTangent.z);
		}
	}

	numFaces = faceData.size();
	numVertices = numFaces * 3;

	width = maxX - minX;
	height = maxY - minY;
	depth = maxZ - minZ;

	//Send Data to OpenGL
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vboVertices);
	glGenBuffers(1, &vboUVs);
	glGenBuffers(1, &vboNormals);
	glGenBuffers(1, &vboTangents);
	glGenBuffers(1, &vboBiTangents);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0); //vertexs
	glEnableVertexAttribArray(1); //UVs
	glEnableVertexAttribArray(2); //normals
	glEnableVertexAttribArray(3); //tangents
	glEnableVertexAttribArray(4); //biTangents

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

	//Tangents VBO Setup
	glBindBuffer(GL_ARRAY_BUFFER, vboTangents);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*tangentData.size(),
		&tangentData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	//BiTangents VBO Setup
	glBindBuffer(GL_ARRAY_BUFFER, vboBiTangents);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float)*biTangentData.size(),
		&biTangentData[0], GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));


	//Cleanup
	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);
	return true;

}

void Mesh::unload() {

	//in reverse order cause practise thing
	glDeleteBuffers(1, &vboBiTangents);
	glDeleteBuffers(1, &vboTangents);
	glDeleteBuffers(1, &vboNormals);
	glDeleteBuffers(1, &vboUVs);
	glDeleteBuffers(1, &vboVertices);
	glDeleteVertexArrays(1, &vao);

	vboBiTangents = NULL;
	vboTangents = NULL;
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
	if (objVertices.size() != 0 && objNormals.size() != 0) {

		//Release data from opengl (VRAM)
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
bool sortByHeight(glm::vec3 *lhs, glm::vec3 *rhs) {
	//      turns out that dead particles return nan
	//        if (std::isnan(lhs->y))
	//        *lhs = glm::vec3(-10000.0f);
	return lhs->y < rhs->y;
}


void Mesh::updateParticlePos(bool updateVertices, std::vector<glm::vec3*> positions)
{
	numVertices = positions.size() * 3;
	if (numVertices != 0) {

		std::sort(positions.begin(), positions.end(), sortByHeight);

		///    //Release data from opengl (VRAM)

		std::vector<float>unpackedVertexData;

		//unpack data that we just read from OBJ file
		for (unsigned int i = 0; i < positions.size(); i++) {
			unpackedVertexData.push_back(positions[i]->x);
			unpackedVertexData.push_back(positions[i]->y);
			unpackedVertexData.push_back(positions[i]->z);
		}

		//Send Data to OpenGL
		if (vao == 0) {
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vboVertices);

			glBindVertexArray(vao);
			glEnableVertexAttribArray(0); //vertexs

			glBindBuffer(GL_ARRAY_BUFFER, vboVertices);//Vertices VBO Setup
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*unpackedVertexData.size(), &unpackedVertexData[0], GL_DYNAMIC_DRAW);
		}

		glBindVertexArray(vao);
		glEnableVertexAttribArray(0); //vertexs

		if (updateVertices) {

			//Vertices VBO Setup
			glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*unpackedVertexData.size(), &unpackedVertexData[0]);
			glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));
		}

		//Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);
	}
}

void Mesh::updateQuad(bool updateVertex, glm::vec3 position)
{
	//check to see if position is real
	//   if (std::isnan(position.x) || std::isnan(position.y) || std::isnan(position.z))
	//       numVertices = 0;
	//   else
	numVertices = 3;

	if (numVertices != 0) {

		std::vector<float>unpackedVertexData;

		//unpack data that we just read from OBJ file
		unpackedVertexData.push_back(position.x);
		unpackedVertexData.push_back(position.y);
		unpackedVertexData.push_back(position.z);
		//Send Data to OpenGL
		if (vao == 0) {
			glGenVertexArrays(1, &vao);
			glGenBuffers(1, &vboVertices);

			glBindVertexArray(vao);
			glEnableVertexAttribArray(0); //vertexs

			glBindBuffer(GL_ARRAY_BUFFER, vboVertices);//Vertices VBO Setup
			glBufferData(GL_ARRAY_BUFFER, sizeof(float)*unpackedVertexData.size(), &unpackedVertexData[0], GL_DYNAMIC_DRAW);
		}
		glBindVertexArray(vao);
		glEnableVertexAttribArray(0); //vertexs
		if (updateVertex) {

			//Vertices VBO Setup
			glBindBuffer(GL_ARRAY_BUFFER, vboVertices);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float)*unpackedVertexData.size(), &unpackedVertexData[0]);
			glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));
		}
		//Cleanup
		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);
	}
}

#include "animation.h"


#define BUFFER_OFFSET(i)((char*)0+(i))

// Changes for Tangent Space:
// Pushed back the slot from 3,4 to 5,6 because tang/biTang use 3,4

void Animation::initialize() {
	for (int i = 0; i < keyFrames.size() - 1; i++)
	{
		glBindVertexArray(keyFrames[i]->vao);

		glEnableVertexAttribArray(5); //next frame vert
		glEnableVertexAttribArray(6); //next frame normal

		glBindBuffer(GL_ARRAY_BUFFER, keyFrames[i + 1]->vboVertices);

		glVertexAttribPointer((GLuint)5, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, keyFrames[i + 1]->vboNormals);

		glVertexAttribPointer((GLuint)6, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);
	}
	glBindVertexArray(keyFrames[keyFrames.size() - 1]->vao);

	glEnableVertexAttribArray(5); //next frame vert
	glEnableVertexAttribArray(6); //next frame normal

	glBindBuffer(GL_ARRAY_BUFFER, keyFrames[0]->vboVertices);
	glVertexAttribPointer((GLuint)5, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, keyFrames[0]->vboNormals);
	glVertexAttribPointer((GLuint)6, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);


}

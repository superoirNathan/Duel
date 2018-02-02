#include "animation.h"


#define BUFFER_OFFSET(i)((char*)0+(i))

void Animation::initialize() {
	for (int i = 0; i < keyFrames.size()-1; i++)
	{		
		glBindVertexArray(keyFrames[i]->vao);

		glEnableVertexAttribArray(3); //next frame vert
		glEnableVertexAttribArray(4); //next frame normal

		glBindBuffer(GL_ARRAY_BUFFER, keyFrames[i+1]->vboVertices);

		glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, keyFrames[i + 1]->vboNormals);

		glVertexAttribPointer((GLuint)4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

		glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
		glBindVertexArray(GL_NONE);
	}
	glBindVertexArray(keyFrames[keyFrames.size()-1]->vao);

	glEnableVertexAttribArray(3); //next frame vert
	glEnableVertexAttribArray(4); //next frame normal

	glBindBuffer(GL_ARRAY_BUFFER, keyFrames[0]->vboVertices);

	glVertexAttribPointer((GLuint)3, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, keyFrames[0]->vboNormals);

	glVertexAttribPointer((GLuint)4, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, BUFFER_OFFSET(0));

	glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
	glBindVertexArray(GL_NONE);


}

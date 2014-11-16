#include "libmodel/3ds.h"
#include "Game.hpp"
#include "GameObject.hpp"
#include <iostream>
#include <vector>
#include <pthread.h>
#include <glm/glm.hpp>

using namespace std;
using glm::vec3;

class OpenHandTest : private Game {
public:
	OpenHandTest() : Game() {}
	void start()
	{
		pthread_t renderThread;
		objectLock = PTHREAD_MUTEX_INITIALIZER;
		pthread_create( &renderThread, NULL, runHandler, this);
	}
	
	void createFramebufferShader()
	{	
		GLuint fbvShader = initShader("shaders/fbvs.glsl",GL_VERTEX_SHADER);
		GLuint fbfShader = initShader("shaders/fbfs.glsl",GL_FRAGMENT_SHADER);
		
		fbProgram = glCreateProgram();
		glAttachShader(fbProgram,fbvShader);
		glAttachShader(fbProgram,fbfShader);
		glLinkProgram(fbProgram);

		GLint linked;
		glGetProgramiv(fbProgram,GL_LINK_STATUS,&linked);

		if (linked==GL_FALSE)
		{
			GLint logLen = 0;
			glGetProgramiv(fbProgram, GL_INFO_LOG_LENGTH, &logLen);
			char * log = new char[logLen];
			glGetProgramInfoLog(fbProgram,logLen,NULL,log);
			std::ofstream logFile ("logs/fbprogramLog.txt");
			logFile << log << std::endl;
			logFile.close ();
			delete log;
			std::cout.flush();
			throw std::runtime_error("Failed to link framebuffer program");
		}

		glDeleteShader(fbvShader);
		glDeleteShader(fbfShader);
	}
	
	void render()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int width;
		int height;
		
		SDL_GetWindowSize(window,&width,&height);
		
		glViewport(0,0,width/2, height);
		
		/*GLuint leftFramebuffer = 0;
		glGenFramebuffers(1, &leftFramebuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, leftFramebuffer);
		
		GLuint leftTexture;
		glGenTextures(1, &leftTexture);
		glBindTexture(GL_TEXTURE_2D, leftTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width/2, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, leftTexture, 0);
		glBindTexture(GL_TEXTURE_2D,0);
		
		GLuint leftDepthTexture;
		glGenTextures(1, &leftDepthTexture);
		glBindTexture(GL_TEXTURE_2D, leftDepthTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width/2, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, leftDepthTexture, 0);
		glBindTexture(GL_TEXTURE_2D,0);
		
		// Always check that our framebuffer is ok
		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			return;
		
		GLenum DrawBuffers[1] = {GL_COLOR_ATTACHMENT0};
		glDrawBuffers(1, DrawBuffers);

		//glBindFramebuffer(GL_FRAMEBUFFER, 0);*/

		// view matrix
		glm::mat4 viewMat;
		// projection matrix
		glm::mat4 projectionMat = glm::perspective(1.0472f, width/(float)height, 0.1f, 100.0f);

		glUseProgram(program);
	
		// get locations of uniforms
		GLuint vloc = glGetUniformLocation(program,"view_matrix");
		GLuint mloc = glGetUniformLocation(program,"model_matrix");
		GLuint prloc = glGetUniformLocation(program,"projection_matrix");
		GLuint mvploc = glGetUniformLocation(program, "mvp_matrix");
		GLuint nmloc = glGetUniformLocation(program, "norm_matrix");
		GLuint eploc = glGetUniformLocation(program, "eye_position");
		GLuint enloc = glGetUniformLocation(program, "eye_normal");

		// pass uniforms
		glUniformMatrix4fv(prloc,1,GL_FALSE,&projectionMat[0][0]);

		// create and bind buffers
		GLuint vertexBuffer;
		GLuint normalBuffer;
		
		// enable vertex attributes at indices 0,1
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
	
		// vertex buffer
		glGenBuffers(1,&vertexBuffer);								// generate vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);					// bind vertex buffer
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0); 	// set vertex buffer to index 0
		
		// normal buffer
		glEnableClientState(GL_NORMAL_ARRAY); 						// enable use of normal array
		glGenBuffers(1,&normalBuffer);								// generate normal buffer
		glBindBuffer(GL_ARRAY_BUFFER,normalBuffer);					// bind normal buffer
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0); 	// set normal buffer to index 1
		
		viewMat = 	glm::lookAt(	glm::vec3(-0.3,10,5),
									glm::vec3(0,0,0),
									glm::vec3(0,1,0));

		glUniformMatrix4fv(vloc,1,GL_FALSE,&viewMat[0][0]);
		glUniform4fv(eploc,1,&viewMat[3][0]);
		glUniform4fv(enloc,1,&viewMat[2][0]);
		
		lockObjects();
		
		for (unsigned int i = 0; i<objects.size(); i++)
		{
			const model curModel = objects[i]->getModel();

			// model matrix
			glm::vec3 axis(1.0f,0.0f,0.0f);
			glm::mat4 modelMat = 	glm::translate(	glm::mat4(1.0f),	objects[i]->getPosition()				)*
									glm::rotate(	glm::mat4(1.0f),	-3.14f/2,axis)*//SDL_GetTicks()/10000.0f * 3.14f, axis	)*
									glm::scale(		glm::mat4(1.0f),	glm::vec3(0.1,0.1,0.1)					);

			// modelview projection matrix
			glm::mat4 mvp = projectionMat * viewMat * modelMat;
			// normal matrix (inverse transpose of model matrix)
			glm::mat4 normMat = glm::transpose(glm::inverse(modelMat));

			glUniformMatrix4fv(mloc,1,GL_FALSE,&modelMat[0][0]);
			glUniformMatrix4fv(mvploc,1,GL_FALSE,&mvp[0][0]);
			glUniformMatrix4fv(nmloc,1,GL_FALSE,&normMat[0][0]);
			
			for (unsigned int j = 0; j < curModel.objectc; j++)
			{
				object* curMesh = curModel.objects[j];
				// vertex buffer data
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);	// bind vertex buffer
				glBufferData(GL_ARRAY_BUFFER, curMesh->facec * 3 * sizeof(vec3d), curMesh->expandedVertices, GL_STATIC_DRAW);
				// normal buffer data
				glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);	// bind normal buffer
				glBufferData(GL_ARRAY_BUFFER, curMesh->facec * 3 * sizeof(vec3d), curMesh->normals, GL_STATIC_DRAW);
				// draw triangles with vertex and normal data
				glDrawArrays(GL_TRIANGLES, 0, curMesh->facec * 3);
			}
		}
		
		unlockObjects();
		
		glViewport(width/2,0,width/2,height);
		
		viewMat = 	glm::lookAt(	glm::vec3(0.3,10,5),
									glm::vec3(0,0,0),
									glm::vec3(0,1,0));

		glUniformMatrix4fv(vloc,1,GL_FALSE,&viewMat[0][0]);
		glUniform4fv(eploc,1,&viewMat[3][0]);
		glUniform4fv(enloc,1,&viewMat[2][0]);
		
		lockObjects();
		
		for (unsigned int i = 0; i<objects.size(); i++)
		{
			const model curModel = objects[i]->getModel();

			// model matrix
			glm::vec3 axis(1.0f,0.0f,0.0f);
			glm::mat4 modelMat = 	glm::translate(	glm::mat4(1.0f),	objects[i]->getPosition()				)*
									glm::rotate(	glm::mat4(1.0f),	-3.14f/2,axis)*//SDL_GetTicks()/10000.0f * 3.14f, axis	)*
									glm::scale(		glm::mat4(1.0f),	glm::vec3(0.1,0.1,0.1)					);

			// modelview projection matrix
			glm::mat4 mvp = projectionMat * viewMat * modelMat;
			// normal matrix (inverse transpose of model matrix)
			glm::mat4 normMat = glm::transpose(glm::inverse(modelMat));

			glUniformMatrix4fv(mloc,1,GL_FALSE,&modelMat[0][0]);
			glUniformMatrix4fv(mvploc,1,GL_FALSE,&mvp[0][0]);
			glUniformMatrix4fv(nmloc,1,GL_FALSE,&normMat[0][0]);
			
			for (unsigned int j = 0; j < curModel.objectc; j++)
			{
				object* curMesh = curModel.objects[j];
				// vertex buffer data
				glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);	// bind vertex buffer
				glBufferData(GL_ARRAY_BUFFER, curMesh->facec * 3 * sizeof(vec3d), curMesh->expandedVertices, GL_STATIC_DRAW);
				// normal buffer data
				glBindBuffer(GL_ARRAY_BUFFER, normalBuffer);	// bind normal buffer
				glBufferData(GL_ARRAY_BUFFER, curMesh->facec * 3 * sizeof(vec3d), curMesh->normals, GL_STATIC_DRAW);
				// draw triangles with vertex and normal data
				glDrawArrays(GL_TRIANGLES, 0, curMesh->facec * 3);
			}
		}
		
		unlockObjects();
		
		// free memory
		glDeleteBuffers(1,&vertexBuffer);
		glDeleteBuffers(1,&normalBuffer);
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER,0);
	}
	void addObject(GameObject& curObject)
	{
		//curObject.setWorld(this);
		
		lockObjects();
		objects.push_back(&curObject);
		unlockObjects();
	}
	bool removeObject(GameObject& curObject)
	{
		lockObjects();
		for (vector<GameObject*>::iterator itr = objects.begin(); itr != objects.end(); itr++)
		{
			if ((*itr) == &curObject)
			{
				itr = objects.erase(itr);
				unlockObjects();
				return true;
			}
		}
		unlockObjects();
		return false;
	}
private:
	GLuint fbProgram;
	pthread_mutex_t objectLock;
	vector<GameObject*> objects;
	
	static void* runHandler(void* that)
	{
		cout << "blah" << endl;
		OpenHandTest* o = (OpenHandTest *) that;
		o->init();
		o->createWindow();
		o->createFramebufferShader();		
		o->run();
		exit(1);
	}
	void lockObjects()		{ pthread_mutex_lock(&objectLock); }
	void unlockObjects() 	{ pthread_mutex_unlock(&objectLock); }
};

int main(int argc, char** argv){
	OpenHandTest game;
	game.start();
	
	model m = read3DSFile("models/teapot.3ds");
	GameObject newObj(m);
	
	
	SDL_Delay(1000);
	
	game.addObject(newObj); // please be nice, don't add twice
	float i = 0;
	while (1)
	{
		i+= 0.02f;
		
		newObj.setPosition(vec3(0.0f,sin(i)*3,0.0f));
		
		SDL_Delay(10);
		
		//SDL_Delay(1000);
		
		/*for (int i = 0; i < 100; i++)
		{
			SDL_Delay(10);
			newObj.setPosition(newObj.getPosition()+vec3(0.0f,0.01f,0.0f));
		}
		
		//SDL_Delay(1000);
		
		for (int i = 0; i < 100; i++)
		{
			SDL_Delay(10);
			newObj.setPosition(newObj.getPosition()-vec3(0.0f,0.01f,0.0f));
		}*/
		
	}
	
	game.removeObject(newObj);
	
	SDL_Delay(1000);
	
    return 0;
}

#ifndef GAME_HPP_
#define GAME_HPP_

#include <SDL2/SDL.h>

#ifdef __ANDROID_API__
	#include <GLES2/gl2.h>
#else
	#define GLEW_STATIC
	#include <GL/glew.h>
	#include <GL/gl.h>
#endif

//#include <GL/glu.h>
#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include "libmodel/3ds.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <typeinfo>
#include <stdexcept>
#include <math.h>

class Game
{
protected:
	SDL_Window *window;
	SDL_GLContext context;
	unsigned int vShader;
	unsigned int fShader;
	unsigned int gShader;
	unsigned int program;

	unsigned int initShader(const char * path, unsigned int shaderType);
	std::string readSource(const char * path);
	void setupShaders();

public:
	Game(): window(NULL), context(NULL), vShader(0), fShader(0), gShader(0), program(0) {}
	virtual ~Game() {}
	void init();
	void createWindow(int width, int height, int fps);
	void createWindow ();
	void quit();
	void delay(long ms);
	//void getMouseCoords(GLint *mousePos2, GLdouble *mousePos3);
	SDL_Window * getWindow();
	Uint32 timerCallback(Uint32 interval, void *param);
	void run();
	void windowEvent(SDL_WindowEvent event);
	virtual void render();
};

#endif

#include "Game.hpp"


#ifdef __ANDROID_API__
#include "libmodel/3ds.h"
#include "SDL2/SDL.h"
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <fstream>
#endif

model teapot;

/*void Game::perspectiveY(GLdouble fovY, GLdouble aspect, GLdouble near, GLdouble far)
{
	GLdouble width,height;
	height = tan(fovY/360 * 3.1415926535897932384626433832795);
	width = height * aspect;
	glFrustum(-width,width,-height,height,near,far);
}

void Game::perspectiveX(GLdouble fovX, GLdouble aspect, GLdouble near, GLdouble far)
{
	GLdouble width,height;
	width = tan(fovX/360 * 3.1415926535897932384626433832795);
	height = width / aspect;
	glFrustum(-width,width,-height,height,near,far);
}*/

std::string Game::readSource(const char * path)
{
	std::cout << "Reading source file: " << path << std::endl;

	std::ifstream inputFile;
	inputFile.open(path);
	std::stringstream sStream;
	sStream << inputFile.rdbuf();
	std::string outString = sStream.str();

	std::cout << "Done reading source file: " << path<< std::endl;

	return outString;
}

unsigned int Game::initShader(const char * path, unsigned int shaderType)
{
	std::cout << "Creating shader from file " << path << std::endl;

	std::string sourceString = readSource(path);
	unsigned int newShader = glCreateShader(shaderType);
	std::cout.flush();

	int sourceLength = sourceString.length();
	const char * sourceCString = sourceString.c_str();
	glShaderSource(newShader,1,&sourceCString,&sourceLength);
	glCompileShader(newShader);

	GLint compiled;
	glGetShaderiv(newShader,GL_COMPILE_STATUS,&compiled);

	if (compiled==GL_FALSE)
	{
		GLint logLen = 0;
		glGetShaderiv(newShader, GL_INFO_LOG_LENGTH, &logLen);
		char * log = new char[logLen];
		glGetShaderInfoLog(newShader,logLen,NULL,log);
		std::ofstream logFile ("logs/shaderLog.txt");
		logFile << log << std::endl;
		logFile.close ();
		delete log;
		std::cout.flush();
		throw std::runtime_error("Failed to compile shader: "+std::string(path));
	}

	std::cout << "Shader compiled : " << path << std::endl;

	return newShader;
}



void Game::setupShaders()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);
#ifndef __ANDROID_API__

//	vShader = initShader("vsnorm.glsl",GL_VERTEX_SHADER);
//	gShader = initShader("gsnorm.glsl",GL_GEOMETRY_SHADER);
//	fShader = initShader("fsnorm.glsl",GL_FRAGMENT_SHADER);
	vShader = initShader("shaders/vs.glsl",GL_VERTEX_SHADER);
	fShader = initShader("shaders/fs.glsl",GL_FRAGMENT_SHADER);
#else

	vShader = initShader("shaders/vses.glsl",GL_VERTEX_SHADER);
	fShader = initShader("shaders/fses.glsl",GL_FRAGMENT_SHADER);

#endif
	program = glCreateProgram();
	glAttachShader(program,vShader);
//	glAttachShader(program,gShader);
	glAttachShader(program,fShader);
	glLinkProgram(program);

	GLint linked;
	glGetProgramiv(program,GL_LINK_STATUS,&linked);

	if (linked==GL_FALSE)
	{
		GLint logLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLen);
		char * log = new char[logLen];
		glGetProgramInfoLog(program,logLen,NULL,log);
		std::ofstream logFile ("logs/programLog.txt");
		logFile << log << std::endl;
		logFile.close ();
		delete log;
		std::cout.flush();
		throw std::runtime_error("Failed to link program");
	}

	glDeleteShader(vShader);
//	glDeleteShader(gShader);
	glDeleteShader(fShader);




//	std::cout << vShader << std::endl;
}

void Game::init()
{
	std::cout << "init\n";
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_Init(SDL_INIT_EVERYTHING);
    teapot = read3DSFile("models/teapot.3ds");
//    object* temp = teapot.objects[1];
//    teapot.objects[1] = teapot.objects[0];
//    teapot.objects[0] = temp;
    for (int i = 0; i<teapot.objectc; i++)
    	expandVertices(teapot.objects[i]);
    std::cout << "vec3d size = " << sizeof(vec3d) << std::endl;
    std::cout << "triangle size = " << sizeof(triangle) << std::endl;
}

void Game::createWindow(int width, int height, int fps)
{
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	window = SDL_CreateWindow("SDL Program",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,width,height,SDL_WINDOW_RESIZABLE|SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
    if (SDL_GL_SetSwapInterval(-1)==-1) //TODO change this to -1 for release
    	SDL_GL_SetSwapInterval(1);
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT);
#ifndef __ANDROID_API__
    if (glewInit())
    {
    	throw std::runtime_error("Error initializing glew!");
    }
#endif
    setupShaders();
    //glEnable(GL_POINT_SPRITE_ARB);
    //glEnable(GL_PROGRAM_POINT_SIZE);
    SDL_GL_SwapWindow(window);
//    SDL_AddTimer(1000/fps,timerCallback,this);
}

void Game::createWindow()
{
	#ifdef __ANDROID_API__
	SDL_DisplayMode dmode;
	SDL_GetDesktopDisplayMode(0, &dmode);
	/*struct fb_var_screeninfo fb_var;
	int fd = open("/dev/graphics/fb0",O_RDONLY);
	int i = ioctl(fd,FBIOGET_VSCREENINFO, &fb_var);
	close(fd);
	int width = fb_var.xres;
	int height = fb_var.yres;*/
	int width = dmode.w;
	int height = dmode.h;
	std::cout << "Dimensions are: " << width << "," << height << std::endl;
	std::cout.flush();
	createWindow(width,height,57);
	#else
	createWindow(800,600,57);
	#endif	
}

void Game::quit()
{
	destroy(teapot);
	glDeleteProgram(program);
	SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Game::delay(long ms)
{
    Uint32 startTime = SDL_GetTicks();

    while (SDL_GetTicks()-startTime<2000)
    {SDL_PumpEvents();}
}

/*void Game::getMouseCoords(GLint *mousePos2, GLdouble *mousePos3)
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];
	glGetIntegerv(GL_VIEWPORT,viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX,modelview);
	glGetDoublev(GL_PROJECTION_MATRIX,projection);
	float winX = (float)mousePos2[0];
	float winY = (float)viewport[3]-(float)mousePos2[1];
	GLfloat depth;
	glReadPixels(mousePos2[0],viewport[3]-mousePos2[1],1,1,GL_DEPTH_COMPONENT,GL_FLOAT,&depth);
	gluUnProject( winX, winY, depth, modelview, projection, viewport, mousePos3, mousePos3+1, mousePos3+2);
}*/

SDL_Window * Game::getWindow()
{
	return window;
}

Uint32 Game::timerCallback(Uint32 interval, void *ptr)
{
	SDL_Event event;
	SDL_UserEvent userevent;
	userevent.type = SDL_USEREVENT;
	userevent.code = 0; //draw frame
	userevent.data1 = NULL;
	userevent.data2 = NULL;
	event.type = SDL_USEREVENT;
	event.user = userevent;
	SDL_PushEvent(&event);
	return interval;
}

void Game::run()
{
	SDL_Event event;
    Uint32 startTime = SDL_GetTicks();
    Uint32 prevTime = SDL_GetTicks();
    bool mousedown = false;
    unsigned short fps = 0;
	while (1)
	{
		SDL_PumpEvents();
		while (SDL_PollEvent(&event))
		{
			switch(event.type)
			{
			case SDL_USEREVENT: {
				SDL_UserEvent userevent = event.user;
				if (userevent.code==0)
				{
//					render ();
//					SDL_GL_SwapWindow(window);
				}
				break;
			}
			case SDL_WINDOWEVENT: {
				windowEvent(event.window);
				break;
			}
//			case SDL_KEYDOWN : {
//				break;
//			}
//			case SDL_KEYUP: {
//				break;
//			}
//			case SDL_MOUSEBUTTONDOWN: {
//				SDL_MouseButtonEvent mouse = event.button;
//				if (mouse.button==SDL_BUTTON_LEFT)
//				{
//					mousedown = true;
//				}
//				break;
//			}
//			case SDL_MOUSEBUTTONUP: {
//				SDL_MouseButtonEvent mouse = event.button;
//				if (mouse.button==SDL_BUTTON_LEFT)
//				{
//					mousedown = false;
//				}
//				break;
//			}
			case SDL_QUIT: {
				quit();
				return;
			}
			}
		}
		prevTime = SDL_GetTicks();
//		SDL_Delay(1);
		render ();
		SDL_GL_SwapWindow(window);
		fps++;
		if (SDL_GetTicks()-startTime>1000)
		{
			startTime = SDL_GetTicks();
			std::cout << fps << std::endl;
			fps = 0;
		}
	}
}

void Game::windowEvent(SDL_WindowEvent event)
{
	switch (event.event)
	{
	case SDL_WINDOWEVENT_RESIZED: {
		//TODO fix nasty android hack
		//Here be memory leaks
#ifndef __ANDROID_API__
	    SDL_GL_DeleteContext(context);
		glDeleteProgram(program);
#endif
		context = SDL_GL_CreateContext(window);
		SDL_GL_MakeCurrent(window,context);
		setupShaders();
		render();
	    SDL_GL_SwapWindow(window);
		std::cout << "SDL error is " << std::endl << SDL_GetError() << std::endl;
		std::cout << "Window resized to " << event.data1 << "," << event.data2 << std::endl;
		int width;
		int height;
		SDL_GetWindowSize(window,&width,&height);
		std::cout << "Window is now size " << width << "," << height << std::endl;
//	    glClearColor(0,0,0,1);
		break;
	}
	}
}

void Game::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glFrontFace(GL_CCW);
	int width;
	int height;
	SDL_GetWindowSize(window,&width,&height);
	glm::vec3 axis(1.0f,0.0f,0.0f);
	glm::mat4 model = glm::mat4(1.0f)*glm::rotate(glm::mat4(1.0f),(SDL_GetTicks()/1000.0f) / 3.14f,axis)*glm::scale(glm::mat4(1.0f),glm::vec3(0.1,0.1,0.1));
	glm::mat4 projection = glm::perspective(1.0472f, width/(float)height, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(	glm::vec3(0,0,5),
									glm::vec3(0,0,0),
									glm::vec3(0,1,0));
	glm::mat4 mvp = projection * view * model;
	glm::mat4 normMat = glm::transpose(glm::inverse(model));

	glUseProgram(program);

	GLint curprog;
	glGetIntegerv(GL_CURRENT_PROGRAM,&curprog);

	//get locations of uniforms

	GLuint vloc = glGetUniformLocation(program,"view_matrix");
	GLuint mloc = glGetUniformLocation(program,"model_matrix");
	GLuint prloc = glGetUniformLocation(program,"projection_matrix");
	GLuint mvploc = glGetUniformLocation(program, "mvp_matrix");
	GLuint nmloc = glGetUniformLocation(program, "norm_matrix");
	GLuint eploc = glGetUniformLocation(program, "eye_position");
	GLuint enloc = glGetUniformLocation(program, "eye_normal");

	//create and bind buffers

	GLuint vertexBuffer;
//	GLuint elementBuffer;
	GLuint normalBuffer;
	//enable vertex attributes at indices 0,1

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

//	glBindAttribLocation(program, 0, "gxl3d_Position");
//	glBindAttribLocation(program, 1, "gxl3d_Normal");

	//element buffer

//	glGenBuffers(1,&elementBuffer);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementBuffer);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER,teapot.objects[0]->facec*sizeof(triangle), teapot.objects[0]->faces, GL_STATIC_DRAW);

	//vertex buffer

	for (int i = 0; i<teapot.objectc; i++)
	{
		glGenBuffers(1,&vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER,vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER,teapot.objects[i]->facec*3*sizeof(vec3d),teapot.objects[i]->expandedVertices,GL_STATIC_DRAW);
		glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,(void*)0); //sets previous buffer to index 0

		//normal buffer

		glEnableClientState(GL_NORMAL_ARRAY);
		glGenBuffers(1,&normalBuffer);
		glBindBuffer(GL_ARRAY_BUFFER,normalBuffer);
		glBufferData(GL_ARRAY_BUFFER,teapot.objects[i]->facec*3*sizeof(vec3d),teapot.objects[i]->normals,GL_STATIC_DRAW);
		glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,0,(void*)0); //sets previous buffer to index 1

		//pass uniforms

		glUniformMatrix4fv(vloc,1,GL_FALSE,&view[0][0]);
		glUniformMatrix4fv(mloc,1,GL_FALSE,&model[0][0]);
		glUniformMatrix4fv(prloc,1,GL_FALSE,&projection[0][0]);
		glUniformMatrix4fv(mvploc,1,GL_FALSE,&mvp[0][0]);
		glUniformMatrix4fv(nmloc,1,GL_FALSE,&normMat[0][0]);
		glUniform4fv(eploc,1,&view[3][0]);
		glUniform4fv(enloc,1,&view[2][0]);
	//	std::cout << "expandedVertices = " << teapot.objects[0]->expandedVertices << std::endl;

		//draw teapot


	//	glDrawElements(GL_TRIANGLES,teapot.objects[0]->facec*3,GL_UNSIGNED_SHORT,0);
		glDrawArrays(GL_TRIANGLES, 0, teapot.objects[i]->facec*3);
		glDeleteBuffers(1,&vertexBuffer);
		glDeleteBuffers(1,&normalBuffer);
	}

	//destructors, free memory

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER,0);
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
//	glDeleteBuffers(1,&elementBuffer);
//	glClearColor(0.3,0.0,0.0,1.0);
//	glClear(GL_COLOR_BUFFER_BIT);
//	std::cout << "draw frame" << std::endl;
//	glColor4f(0.0f,0.0f,0.3f,1.0f);
//	GLfloat vertices[] = {	1,	1,	0,
//    						-1,	1,	0,
//    						1,	-1,	0};
//    glEnableClientState (GL_VERTEX_ARRAY);
//    glVertexPointer (3, GL_FLOAT,0, vertices);
//    glDrawArrays (GL_TRIANGLES,0,3);
//    glDisableClientState (GL_VERTEX_ARRAY);
	/*glBegin(GL_TRIANGLES);

	glVertex3f(1.0,1.0,0.0);
	glVertex3f(-1.0,1.0,0);
	glVertex3f(1.0,-1.0,0);
	glEnd();*/
}

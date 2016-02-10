//http://www.youtube.com/user/thecplusplusguy
//normalmapping (bump-mapping) main.cpp
#include <iostream>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_main.h>
//#include 
//#include "GLee.h"
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#endif
#include "camera.h"
#include <vector>
#include <string>
#include <fstream>
//#include "objloader.h"
float angle=0.0;
camera cam;
float lpos;
bool b;
//objloader obj;
//data->vertex->fragment

#define SCREEN_WIDTH (720)
#define SCREEN_HEIGHT (720)

void loadFile(const char* fn,std::string& str)
{
	std::ifstream in(fn);
	if(!in.is_open())
	{
		std::cout << "The file " << fn << " cannot be opened\n";
		return;
	}
	char tmp[300];
	while(!in.eof())
	{
		in.getline(tmp,300);
		str+=tmp;
		str+='\n';
	}
}

unsigned int loadShader(std::string& source,unsigned int mode)
{
	unsigned int id;
	id=glCreateShader(mode);
	
	const char* csource=source.c_str();
	
	glShaderSource(id,1,&csource,NULL);
	glCompileShader(id);
	char error[1000];
	glGetShaderInfoLog(id,1000,NULL,error);
	std::cout << "Compile status: \n" << error << std::endl;
	return id;
}

unsigned int vs,fs,program;

void initShader(const char* vname,const char* fname)
{
	std::string source;
	loadFile(vname,source);
	vs=loadShader(source,GL_VERTEX_SHADER);
	source="";
	loadFile(fname,source);
	fs=loadShader(source,GL_FRAGMENT_SHADER);
	
	program=glCreateProgram();
	glAttachShader(program,vs);
	glAttachShader(program,fs);
	
	glLinkProgram(program);
	glUseProgram(program);
}

void clean()
{
	glDetachShader(program,vs);
	glDetachShader(program,fs);
	glDeleteShader(vs);
	glDeleteShader(fs);
	glDeleteProgram(program);
}

unsigned int loadTexture(const char* filename)
{
	unsigned int num;
	glGenTextures(1,&num);
	SDL_Surface* img=IMG_Load(filename);
	if(img==NULL)
	{
		std::cout << "img was not loaded" << std::endl;
		return -1;
	}
	SDL_PixelFormat form={NULL,32,4,0,0,0,0,0,0,0,0,0xff000000,0x00ff0000,0x0000ff00,0x000000ff,0,255};
	SDL_Surface* img2=SDL_ConvertSurface(img,&form,SDL_SWSURFACE);
	if(img2==NULL)
	{
		std::cout << "img2 was not loaded" << std::endl;
		return -1;		
	}
	glBindTexture(GL_TEXTURE_2D,num);		
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); 
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,img2->w,img2->h,0,GL_RGBA,GL_UNSIGNED_INT_8_8_8_8,img2->pixels);		
	SDL_FreeSurface(img);
	SDL_FreeSurface(img2);
	return num;
}

unsigned int myImg,myImg2, myDisplacement, mySpecularity/*,monkey*/;

void init()
{
	glClearColor(0,0,0,1);
	glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(25,SCREEN_WIDTH/SCREEN_HEIGHT,1,1000);
	glMatrixMode(GL_MODELVIEW);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	initShader("../vertex.vs","../fragment.frag");
	myImg=loadTexture("../testTexture/diffuse.png");
	myImg2=loadTexture("../testTexture/normals.png");
	myDisplacement = loadTexture("../testTexture/displacement.png");
	mySpecularity = loadTexture("../testTexture/specularity.png");
//	monkey=obj.load("monkey.obj");
}

void display()
{
	if(b)
	{
		lpos+=0.2;
		if(lpos>=5.0)
			b=!b;
	}else{
		lpos-=0.2;
		if(lpos<=-5.0)
			b=!b;	
	}
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	cam.Control();
	cam.UpdateCamera();
	angle+=3.0;
	if(angle>=360)
		angle-=360;
	
	
	glUniform3f(glGetUniformLocation(program,"lightPos"),cam.getLocation().x,lpos,cam.getLocation().z);//cam.getLocation().x,cam.getLocation().y,cam.getLocation().z);
	//printf("camx = %f\tcamy = %f\tcamz = %f\n", cam.getLocation().x, cam.getLocation().y, cam.getLocation().z);
	
	glUniform3f(glGetUniformLocation(program,"mambient"),0.2,0.2,0.2);
	glUniform3f(glGetUniformLocation(program,"mdiffuse"),1.0,1.0,1.0);
	glUniform3f(glGetUniformLocation(program,"mspecular"),0.2,0.2,0.2);
	
	glUniform3f(glGetUniformLocation(program,"lambient"),0.2,0.2,0.2);
	glUniform3f(glGetUniformLocation(program,"ldiffuse"),1.0,1.0,1.0);
	glUniform3f(glGetUniformLocation(program,"lspecular"),1.0,1.0,1.0);
		
	glUniform1f(glGetUniformLocation(program,"shininess"),100.0);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,myImg);
	glUniform1i(glGetUniformLocation(program,"img"),0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D,myImg2);
	glUniform1i(glGetUniformLocation(program,"normalmap"),1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D,myDisplacement);
	glUniform1i(glGetUniformLocation(program,"heightmap"),2);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D,mySpecularity);
	glUniform1i(glGetUniformLocation(program,"specularitymap"),3);
	
//	glRotatef(angle,0,1,0);
//	glCallList(monkey);

	double texx = 1.0, texy = 1.0;
	// front
	glBegin(GL_QUADS);
		glNormal3f(0.0,0.0,1.0);
		glTexCoord2f(0,0);		//gl_MultiTexCoord0
		glVertex3f(-1,1,1);	//gl_Vertex
		glTexCoord2f(0,texy);
		glVertex3f(-1,-1,1);
		glTexCoord2f(texx,texy);
		glVertex3f(1,-1,1);
		glTexCoord2f(texx,0);
		glVertex3f(1,1,1);
	glEnd();

	// back
	glBegin(GL_QUADS);
	glNormal3f(0.0,0.0,-1.0);
	glTexCoord2f(0,0);		//gl_MultiTexCoord0
	glVertex3f( 1, 1, -1);	//gl_Vertex
	glTexCoord2f(0,texy);
	glVertex3f( 1, -1, -1);
	glTexCoord2f(texx,texy);
	glVertex3f( -1, -1, -1);
	glTexCoord2f(texx,0);
	glVertex3f( -1, 1, -1);
	glEnd();

	// right
	glBegin(GL_QUADS);
	glNormal3f( 1.0, 0.0, 0.0);
	glTexCoord2f(0,0);		//gl_MultiTexCoord0
	glVertex3f(1, 1, 1);	//gl_Vertex
	glTexCoord2f(0,texy);
	glVertex3f(1, -1, 1);
	glTexCoord2f(texx,texy);
	glVertex3f(1, -1, -1);
	glTexCoord2f(texx,0);
	glVertex3f(1, 1, -1);
	glEnd();

	// left
	glBegin(GL_QUADS);
	glNormal3f( -1.0, 0.0, 0.0);
	glTexCoord2f(0,0);		//gl_MultiTexCoord0
	glVertex3f(-1, 1, -1);	//gl_Vertex
	glTexCoord2f(0,texy);
	glVertex3f(-1, -1, -1);
	glTexCoord2f(texx,texy);
	glVertex3f(-1, -1, 1);
	glTexCoord2f(texx,0);
	glVertex3f(-1, 1, 1);
	glEnd();

	// top
	
	glBegin(GL_QUADS);
	glNormal3f(0.0,1.0,0.0);
	glTexCoord2f(0,0);		//gl_MultiTexCoord0
	glVertex3f( -1, 1, -1);	//gl_Vertex
	glTexCoord2f(0,texy);
	glVertex3f( -1, 1, 1);
	glTexCoord2f(texx,texy);
	glVertex3f( 1, 1, 1);
	glTexCoord2f(texx,0);
	glVertex3f( 1, 1, -1);
	glEnd();

	// bottom
	glBegin(GL_QUADS);
	glNormal3f(0.0,-1.0,0.0);
	glTexCoord2f(0,0);		//gl_MultiTexCoord0
	glVertex3f( 1, -1, -1);	//gl_Vertex
	glTexCoord2f(0,texy);
	glVertex3f( 1, -1, 1);
	glTexCoord2f(texx,texy);
	glVertex3f( -1, -1, 1);
	glTexCoord2f(texx,0);
	glVertex3f( -1, -1, -1);
	glEnd();
	 
}

int main(int argc, char **argv)
{
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_SetVideoMode(SCREEN_WIDTH,SCREEN_HEIGHT,32,SDL_OPENGL);
	Uint32 start;
	SDL_Event event;
	bool running=true;
	init();
	bool b=false;
	while(running)
	{
		start=SDL_GetTicks();
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_QUIT:
					running=false;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym)
					{
						case SDLK_ESCAPE:
							running=false;
							break;
					}
					break;
				case SDL_MOUSEBUTTONDOWN:
					cam.mouseIn(true);
					break;
					
			}
		}
		display();
		SDL_GL_SwapBuffers();
		if(1000.0/30>SDL_GetTicks()-start)
			SDL_Delay(1000.0/30-(SDL_GetTicks()-start));
	}
	clean();
	SDL_Quit();
}

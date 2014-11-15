#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "stl.h"

static void writeVec3d(FILE *f, vec3d v)
{
	float x = (float)v.x;
	float y = (float)v.y;
	float z = (float)v.z;
	fwrite(&x,4,1,f);
	fwrite(&y,4,1,f);
	fwrite(&z,4,1,f);
}

static vec3d parseVec3d(FILE *f)
{
	float x;
	float y;
	float z;
	fread(&x,1,4,f);
	fread(&y,1,4,f);
	fread(&z,1,4,f);
	vec3d retVal = {x,y,z};
	return retVal;
}

static float parseFloat(FILE *f)
{
	float retVal;
	fread(&retVal,1,4,f);
	return retVal;
}

static uint8_t parseInt8(FILE *f)
{
	int retVal = 0;
	fread(&retVal,1,1,f);
	return retVal;
}

static uint16_t parseInt16(FILE *f)
{
	unsigned short retVal = 0;
	fread(&retVal,1,2,f);
	return retVal;
}

static uint32_t parseInt32(FILE *f)
{
	long retVal = 0;
	fread(&retVal,1,4,f);
	return retVal;
}

static model readASCII(FILE *file, long size)
{
	printf("ASCII Files not supported! Sorry :(\n");
	model m = {0};
	return m;
}

static model readBinary(FILE *file,unsigned long size)
{
	model m = {0};
	char name[80];
	fread(name,1,80,file);
	uint32_t triCount = parseInt32(file);
	uint64_t correctSize = 84+triCount*(4*4*3+2);
	int chars = 0;
	while (1)
	{
		if (name[chars]==(char)0||name[chars]==' ')
		{
			break;
		}
		chars++;
	}
	if (size==correctSize)
	{
		printf("Good file!\n");
	}
	else
	{
		printf("Incorrect file size!\n");
		return m;
	}
	uint32_t i;
	object *obj = allocObject(&m);
	allocName(obj,chars);
	strcpy(name,obj->name);
	allocVertices(obj,3*triCount);
	allocNormals(obj,3*triCount);
	allocFaces(obj,triCount);
	allocFaceNormals(obj,triCount);
	for (i=0;i<triCount;i++)
	{
		vec3d norm = parseVec3d(file);
		obj->normals[3*i+0] = norm;
		obj->normals[3*i+1] = norm;
		obj->normals[3*i+2] = norm;
		obj->faceNormals[i] = norm;
		obj->vertices[3*i+0] = parseVec3d(file);
		obj->vertices[3*i+1] = parseVec3d(file);
		obj->vertices[3*i+2] = parseVec3d(file);
		triangle tri = {3*i+0,3*i+1,3*i+2};
		obj->faces[i] = tri;
		unsigned short attribute = parseInt16(file);
		//printf("%f,%f,%f\n",obj->vertices[3*i].x,obj->vertices[3*i].y,obj->vertices[3*i].z);
	}
	return m;
}

void writeSTLFile(FILE *out, model m)
{
	fwrite("model",1,5,out);
	fseek(out,80,SEEK_SET);
	int objectc = m.objectc;
	int i;
	long triCount = 0;
	for (i=0;i<objectc;i++)
	{
		triCount+=m.objects[i]->facec;
	}
	fwrite(&triCount,1,4,out);
	for (i=0;i<objectc;i++)
	{
		object *obj = m.objects[i];
		int j;
		for (j=0;j<obj->facec;j++)
		{
			writeVec3d(out,obj->faceNormals[j]);
			triangle tri = obj->faces[j];
			writeVec3d(out,obj->vertices[tri.a]);
			writeVec3d(out,obj->vertices[tri.b]);
			writeVec3d(out,obj->vertices[tri.c]);
			int i = 0;
			fwrite(&i,1,2,out);
		}
	}
}

model readSTLFile(const char *dir)
{
	FILE *file;
	if ((file = fopen(dir,"rb"))==NULL)
	{
		printf("Error opening file!\n");
		return (model){0};
	}
	fseek(file,0,SEEK_END);
	long size = ftell(file);
	printf("Size is %ld\n",size);
	rewind(file);
	if (size<84)
	{
		printf("Invalid file!\n");
		return (model){0};
	}
	model m={0};
	char asciiTest[6];
	fread(asciiTest,1,6,file);
	rewind(file);
	if (strcmp(asciiTest,"model ")==0)
	{
		printf("ASCII STL file\n");
		m=readASCII(file,size);
	}
	else
	{
		printf("Binary STL file\n");
		m=readBinary(file,size);
	}
	rewind(file);
	fclose(file);
	return m;
}


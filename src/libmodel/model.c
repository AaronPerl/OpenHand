#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "model.h"


short equalsEdge(vec3d a, vec3d b, vec3d c, vec3d d)
{
	return (equals(a,c)&&equals(b,d))||(equals(a,d)&&equals(b,c));
}

void checkBoundaries(model m, long *size, vec3d **badEdges)
{
	vec3d *edges;
	long edgec;
	short *counts;
	int i;
	int k;
	long badCount=0;
	getEdges(m,&edgec,&edges);
	counts = (short*) malloc(sizeof(short)*edgec);
	for (i=0;i<edgec;i++)
	{
		counts[i] = 0;
	}
	for (i=0;i<m.objectc;i++)
	{
		object *obj = m.objects[i];
		vec3d *vertices = obj->vertices;
		int j;
		for (j=0;j<obj->facec;j++)
		{
			triangle face = obj->faces[j];
			vec3d fa = vertices[face.a];
			vec3d fb = vertices[face.b];
			vec3d fc = vertices[face.c];
			int k;
			for (k=0;k<edgec;k++)
			{
				vec3d a = edges[k*2];
				vec3d b = edges[k*2+1];
				if (equalsEdge(fa,fb,a,b))
					counts[k]++;
				if (equalsEdge(fa,fc,a,b))
					counts[k]++;
				if (equalsEdge(fb,fc,a,b))
					counts[k]++;
			}
		}
	}
	for (k=0;k<edgec;k++)
	{
		if (counts[k]<2)
		{
			if (badCount==0)
				*badEdges = (vec3d*)malloc(sizeof(vec3d)*2*(badCount+1));
			else
				*badEdges = (vec3d*)realloc(*badEdges,sizeof(vec3d)*2*(badCount+1));
			(*badEdges)[badCount*2+0]=edges[k*2+0];
			(*badEdges)[badCount*2+1]=edges[k*2+1];
			badCount++;
			printf("Bad edge : %d\n",k);
		}
	}
	*size = badCount;
	free(edges);
	free(counts);
}

void getEdges(model m, long *size, vec3d **edges)
{
	long edgec = 0;
	//vec3d *edges=malloc(sizeof(vec3d)*2);
	int objectc = m.objectc;
	int i;
	*edges = (vec3d*)malloc(sizeof(vec3d)*2);
	for (i=0;i<objectc;i++)
	{
		object *obj = m.objects[i];
		vec3d *vertices = obj->vertices;
		int j;
		for (j=0;j<obj->facec;j++)
		{
			int edge1 = -1;
			int edge2 = -1;
			int edge3 = -1;
			int k = 0;
			triangle face = obj->faces[j];
			vec3d fa = vertices[face.a];
			vec3d fb = vertices[face.b];
			vec3d fc = vertices[face.c];
			for (k=0;k<edgec;k++)
			{
				vec3d a = (*edges)[k*2];
				vec3d b = (*edges)[k*2+1];
				if (equalsEdge(fa,fb,a,b))
					edge1 = k;
				if (equalsEdge(fa,fc,a,b))
					edge2 = k;
				if (equalsEdge(fb,fc,a,b))
					edge3 = k;
				if (edge1>=0&&edge2>=0&&edge3>=0)
					break;
			}
			if (edge1==-1)
			{
				*edges = (vec3d*)realloc(*edges,(edgec+1)*sizeof(vec3d)*2);
				(*edges)[edgec*2] = fa;
				(*edges)[edgec*2+1] = fb;
				edgec++;
			}
			printf("Test 2\n");
			if (edge2==-1)
			{
				*edges = (vec3d*)realloc(*edges,(edgec+1)*sizeof(vec3d)*2);
				(*edges)[edgec*2] = fa;
				(*edges)[edgec*2+1] = fc;
				edgec++;
			}
			if (edge2==-1)
			{
				*edges = (vec3d*)realloc(*edges,(edgec+1)*sizeof(vec3d)*2);
				(*edges)[edgec*2] = fb;
				(*edges)[edgec*2+1] = fc;
				edgec++;
			}
		}
	}
	*size = edgec;
	printf("Edge count is %ld\n",edgec);
}

object *allocObject(model *m)
{
	object *retVal;
	if (m->objectc)
	{
		m->objects = (object**)realloc(m->objects,sizeof(object)*(m->objectc+1));
	}
	else
	{
		m->objects = (object**)malloc(sizeof(object));
	}
	retVal = (object*)malloc(sizeof(object));
	retVal->memFlags = 0;
	retVal->vertexc = 0;
	retVal->facec = 0;
	m->objects[m->objectc] = retVal;
	m->objectc++;
	printf("objectc = %d\n",m->objectc);
	return retVal;
}

void allocName(object *obj, int size)
{
	if (obj->memFlags&1)
	{
		printf("Already alloc'ed\n");
		obj->name = (char*)realloc(obj->name,sizeof(char)*size);
	}
	else
	{
		obj->name = (char*)malloc(sizeof(char)*size);
		obj->memFlags|=1;
	}
}

void allocVertices(object *obj, int size)
{
	if (obj->memFlags&2)
	{
		obj->vertices = (vec3d*)realloc(obj->vertices,sizeof(vec3d)*size);
	}
	else
	{
		obj->vertices = (vec3d*)malloc(sizeof(vec3d)*size);
		obj->memFlags|=2;
	}
	obj->vertexc = size;
}

void allocFaces(object *obj, int size)
{
	if (obj->memFlags&4)
	{
		obj->faces = (triangle*)realloc(obj->faces,sizeof(triangle)*size);
	}
	else
	{
		obj->faces = (triangle*)malloc(sizeof(triangle)*size);
		obj->memFlags|=4;
	}
	obj->facec = size;
}

void allocNormals(object *obj, int size)
{
	if (obj->memFlags&8)
	{
		obj->normals = (vec3d*)realloc(obj->normals,sizeof(vec3d)*size);
	}
	else
	{
		obj->normals = (vec3d*)malloc(sizeof(vec3d)*size);
		obj->memFlags|=8;
	}
}

void allocFaceNormals(object *obj, int size)
{
	if (obj->memFlags&16)
	{
		obj->faceNormals = (vec3d*)realloc(obj->faceNormals,sizeof(vec3d)*size);
	}
	else
	{
		obj->faceNormals = (vec3d*)malloc(sizeof(vec3d)*size);
		obj->memFlags|=16;
	}
}

void allocSmoothGroups(object *obj, int size)
{
	if (obj->memFlags&32)
	{
		obj->smoothGroups = (long*)realloc(obj->smoothGroups,sizeof(long)*size);
	}
	else
	{
		obj->smoothGroups = (long*)malloc(sizeof(long)*size);
		obj->memFlags|=32;
	}
}

void allocExpandedVertices(object *obj, int size)
{
	if (obj->memFlags&64)
	{
		obj->expandedVertices = (vec3d*)realloc(obj->expandedVertices,sizeof(vec3d)*size);
	}
	else
	{
		obj->expandedVertices = (vec3d*)malloc(sizeof(vec3d)*size);
		obj->memFlags|=64;
	}
}

void expandVertices(object *obj)
{
	int i;
	allocExpandedVertices(obj, obj->facec*3);
	for (i=0; i<obj->facec; i++)
	{
		triangle curFace = obj->faces[i];
		obj->expandedVertices[3*i+0] = obj->vertices[curFace.a];
		obj->expandedVertices[3*i+1] = obj->vertices[curFace.b];
		obj->expandedVertices[3*i+2] = obj->vertices[curFace.c];
	}
}

void destroy(model m)
{
	int objc = m.objectc;
	int i;
	for (i=0;i<objc;i++)
	{
		object *curobj = m.objects[i];
		int memFlags = curobj->memFlags;
		if (memFlags>0)
		{
		if (memFlags&1)	
			free(curobj->name);
		if (memFlags&2)	
			free(curobj->vertices);
		if (memFlags&4)	
			free(curobj->faces);
		if (memFlags&8)	
			free(curobj->normals);
		if (memFlags&16)	
			free(curobj->faceNormals);
		if (memFlags&32)	
			free(curobj->smoothGroups);
		if (memFlags&64)
			free(curobj->expandedVertices);
		}
		
		free(curobj);
	}
	if (objc>0)
	{
		free(m.objects);
	}
}

vec3d sum(vec3d addend1, vec3d addend2)
{
	vec3d sum;
	sum.x = addend1.x+addend2.x;
	sum.y = addend1.y+addend2.y;
	sum.z = addend1.z+addend2.z;
	return sum;
}

vec3d diff(vec3d minuend, vec3d subtrahend)
{
	vec3d difference;
	difference.x = minuend.x-subtrahend.x;
	difference.y = minuend.y-subtrahend.y;
	difference.z = minuend.z-subtrahend.z;
	return difference;
} 

vec3d scale(vec3d a,double scalar)
{
	vec3d scaled;
	scaled.x = a.x*scalar;
	scaled.y = a.y*scalar;
	scaled.z = a.z*scalar;
	return scaled;
}

vec3d normalize(vec3d a)
{
	float magnitude = sqrt(a.x*a.x+a.y*a.y+a.z*a.z);
	return scale(a,1/magnitude);
}

short equals(vec3d a, vec3d b)
{
	return (a.x==b.x&&a.y==b.y&&a.z==b.z);
}

vec3d getNormal(vec3d a, vec3d b, vec3d c)
{
	vec3d u = diff(b,a);
	vec3d v = diff(c,a);
	vec3d normal;
	normal.x = (u.y*v.z)-(u.z*v.y);
	normal.y = (u.z*v.x)-(u.x*v.z);
	normal.z = (u.x*v.y)-(u.y*v.x);
	return normalize(normal);
}

#ifndef MODEL_H
#define MODEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct vec3d
{
	float x;
	float y;
	float z;
} vec3d;

typedef struct triangle
{
	uint32_t a;
	uint32_t b;
	uint32_t c;
} triangle;

typedef struct object
{
	int memFlags;		//flags for which fields have been allocated in heap
	int vertexc;
	int facec;
	char *name; 		//alloc-bit 0
	vec3d *vertices; 	//alloc-bit 1
	triangle *faces; 	//alloc-bit 2
	vec3d *normals; 	//alloc-bit 3
	vec3d *faceNormals; 	//alloc-bit 4
	long *smoothGroups; 	//alloc-bit 5
	vec3d *expandedVertices;	//alloc-bit 6
} object;

typedef struct model
{
	int objectc;
	object **objects;
} model;


void checkBoundaries(model m, long *size, vec3d **retVal);
void getEdges(model m, long *size, vec3d **retVal);
object *allocObject(model *m);
void allocName(object *obj, int size);
void allocVertices(object *obj, int size);
void allocFaces(object *obj, int size);
void allocNormals(object *obj, int size);
void allocFaceNormals(object *obj, int size);
void allocSmoothGroups(object *obj, int size);
void allocExpandedVertices(object *obj, int size);
void expandVertices(object *obj);
void destroy(model m);
vec3d sum(vec3d addend1, vec3d addend2);
vec3d diff(vec3d minuend, vec3d subtrahend);
vec3d scale(vec3d a,double scalar);
vec3d normalize(vec3d a);
short equals(vec3d a, vec3d b);
vec3d getNormal(vec3d a, vec3d b, vec3d c);

#ifdef __cplusplus
}
#endif
 
#endif

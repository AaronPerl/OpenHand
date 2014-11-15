#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "3ds.h"

#define MAIN_CHUNK	0x4D4D
#define CHUNK_3D_EDIT	0x3D3D
#define OBJECT_BLOCK	0x4000
#define TRIANGLE_MESH	0x4100
#define VERTICES_LIST	0x4110
#define POINTS_LIST	0x4120
//TODO FACE DESCRIPTION
#define MAPPING_COORDS	0x4140
#define SMOOTH_GROUPS	0x4150
//TODO LIGHT (required?)

//TODO MATERIAL BLOCK

//THE 3DS FORMAT HAS 16 BIT ATOMIC ELEMENTS FOR ENDIANESS

static float parseFloat(FILE *f)
{
	float retVal;
	fread(&retVal,1,4,f);
	return retVal;
}

static unsigned short parseInt16(FILE *f)
{
	unsigned short retVal = 0;
	fread(&retVal,1,2,f);
	return retVal;
}

static long parseInt32(FILE *f)
{
	long retVal = 0;
	fread(&retVal,1,4,f);
	return retVal;
}

static chunk getChunk(FILE *f)
{
	chunk retVal;
	int ID = parseInt16(f);
	long length = parseInt32(f);
	retVal.start = ftell(f);
//	printf("ID is %X and length is %ld\n",ID,length);
	retVal.ID = ID;
	retVal.dataLength = length;
	return retVal;
}

model read3DSFile(const char *dir)
{
	FILE *file;
	model m = {0};
	int size;
	chunk all;
	long totalLength;
	long start=0;
	object *curobj;
	if ((file = fopen(dir,"rb"))==NULL)
	{
		printf("Error opening file!\n");
		return m;
	}
	fseek(file,0,SEEK_END);
	size = ftell(file);
	rewind(file);
//	printf("Size is %d\n",size);
	all = getChunk(file);
	totalLength = all.dataLength;
	m.objectc = 0;
//	printf("Total length is %d\n");
	while (start<totalLength)
	{
		chunk current = getChunk(file);
		long length = current.dataLength;
		switch (current.ID)
		{
		case CHUNK_3D_EDIT: {
			break;}
		case OBJECT_BLOCK: {
			int chars = 0;
			char *name;
			while (1)
			{
				chars++;
				if (fgetc(file)==(char)0)
				{
					break;
				}
			}
			fseek(file,-chars,SEEK_CUR);
			name = (char*) malloc(chars*sizeof(char));
			fgets(name,chars+1,file); //read name
//			printf("	Object %d found\n",m.objectc+1);
//			printf("	Name is %s\n",name);
			curobj = allocObject(&m);
			//printf("%d\n",curobj);
			allocName(curobj,sizeof(char)*(chars+1));
//			printf("objectc = %d\n",m.objectc);
			strcpy(curobj->name,name);
			free(name);
			break;}
		case TRIANGLE_MESH: {
			break;}
		case VERTICES_LIST: {
			int vertexCount = parseInt16(file);
			int i;
			allocVertices(curobj,vertexCount);
//			printf("	Vertex count is %d\n",vertexCount);
			for (i=0;i<vertexCount;i++)
			{
				curobj->vertices[i].x = parseFloat(file);
				curobj->vertices[i].y = parseFloat(file);
				curobj->vertices[i].z = parseFloat(file);
			}
			break;}
		case POINTS_LIST: {
			int faceCount = parseInt16(file);
			int i;
			allocFaces(curobj,faceCount);
//			printf("	Triangle count is %d\n",faceCount);
			for (i=0;i<faceCount;i++)
			{
				curobj->faces[i].a = parseInt16(file);
				curobj->faces[i].b = parseInt16(file);
				curobj->faces[i].c = parseInt16(file);
				fseek(file,2,SEEK_CUR);
			}
			break;}
		/*case MAPPING_COORDS: {
			
			break;}*/
		case SMOOTH_GROUPS: {
			int facec = curobj->facec;
			int i;
			int **groups;
			int sizes[32] = {0};
			int facen;
			int groupn;
			allocSmoothGroups(curobj,facec);
			for (i=0;i<facec;i++)
			{
				curobj->smoothGroups[i] = parseInt32(file);
			}
			allocFaceNormals(curobj,facec);
			allocNormals(curobj,facec*3);
			groups = (int**)malloc(sizeof(int*)*32);


			for (facen=0; facen<facec; facen++)
			{
				vec3d vecs[3]; //calculate face normals
				triangle t = curobj->faces[facen];
				vec3d normal;
				int shift;
				long curGroups;
				vecs[0] = curobj->vertices[t.a];
				vecs[1] = curobj->vertices[t.b];
				vecs[2] = curobj->vertices[t.c];
				vec3d test;
				test.x = 0.0;
				test.y = 1.0;
				test.z = 0.0;
				normal = getNormal(vecs[0],vecs[1],vecs[2]);
//				float temp = normal.x;
//				normal.x = 0.0;
//				normal.y = 1.0;
//				normal.z = 0.0;
				curobj->faceNormals[facen] = normal;
				curGroups = curobj->smoothGroups[facen];
				for (shift=0; shift<32; shift++)
				{
					int mask = 1<<shift;
					if (mask&curGroups)
					{
						sizes[shift]++;
						if (sizes[shift]==1)
							groups[shift] = (int*)malloc(sizeof(int));
						else
							groups[shift]=(int*)realloc(groups[shift],sizeof(int)*sizes[shift]);
						groups[shift][sizes[shift]-1]=facen;
					}
				}
			}
			for (groupn=0; groupn<32; groupn++)
			{
				for (facen = 0; facen<sizes[groupn]; facen++)
				{
					int curface = groups[groupn][facen];
					triangle curtri = curobj->faces[curface];
					vec3d norms[3];
					int j;
					int facen2;
					for (j=0;j<3;j++)
						norms[j] = curobj->normals[curface*3+j];
					for (facen2 = 0; facen2<sizes[groupn]; facen2++)
					{
						int curface2 = groups[groupn][facen2];
						triangle temptri = curobj->faces[curface2];
						vec3d a1 = curobj->vertices[curtri.a];
						vec3d b1 = curobj->vertices[curtri.b];
						vec3d c1 = curobj->vertices[curtri.c];
						vec3d a2 = curobj->vertices[temptri.a];
						vec3d b2 = curobj->vertices[temptri.b];
						vec3d c2 = curobj->vertices[temptri.c];
						if (equals(a1,a2)||equals(a1,b2)||equals(a1,c2))
						{
							norms[0] = sum(norms[0],curobj->faceNormals[curface2]);
						}
						if (equals(b1,a2)||equals(b1,b2)||equals(b1,c2))
						{
							norms[1] = sum(norms[1],curobj->faceNormals[curface2]);
						}
						if (equals(c1,a2)||equals(c1,b2)||equals(c1,c2))
						{
							norms[2] = sum(norms[2],curobj->faceNormals[curface2]);
						}
					}
					for (j=0;j<3;j++)
						curobj->normals[curface*3+j] = norms[j];
				}
			}
//			printf("Test 1\n");
			for (facen = 0; facen<facec; facen++) //check smoothing groups
			{					
				long curGroups = curobj->smoothGroups[facen];
				int normn = facen*3;
				if (curGroups) //normalize summed normals
				{
					curobj->normals[normn+0]=normalize(curobj->normals[normn+0]);
					curobj->normals[normn+1]=normalize(curobj->normals[normn+1]);
					curobj->normals[normn+2]=normalize(curobj->normals[normn+2]);
					//curobj->normals[normn+0] = curobj->faceNormals[facen];
					//curobj->normals[normn+1] = curobj->faceNormals[facen];
					//curobj->normals[normn+2] = curobj->faceNormals[facen]; 
				}
				else //use face normal as vertex normals
				{ 
					//printf("Face %d is not in any smoothing groups\n",facen);
					curobj->normals[normn+0] = curobj->faceNormals[facen];
					curobj->normals[normn+1] = curobj->faceNormals[facen];
					curobj->normals[normn+2] = curobj->faceNormals[facen]; 
				}
			}
			for (i=0;i<32;i++)
			{
				if (sizes[i])
					free(groups[i]);
			}
			free(groups);
//			printf("Test 2\n");
			break;}
		default:{
//			printf("	Skipping chunk %x\n",current.ID);
//			printf("	Currently at %ld\n", ftell(file));
			if (ftell(file)+length-6==totalLength)
			{
//				printf("		Nearing the end\n");
				//length--;
			}
			fseek(file,length-6,SEEK_CUR);
//			printf("	After skip is at %ld\n", ftell(file));
			start = ftell(file);
			break;}
		}
	}
	rewind(file);
	fclose(file);
	return m;
}

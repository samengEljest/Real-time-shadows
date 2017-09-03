#pragma once
#include <vector>
#include <stdio.h>
#include <string>
#include <cstring>
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4.h"
#include <string>
#include <QGLWidget>
#include <QGLFunctions>
#include "node.h"

struct HE_vert;
struct HE_face;

struct HE_edge
{
	HE_vert* vert;   // vertex at the end of the half-edge
    HE_edge* pair;   // oppositely oriented adjacent half-edge 
    HE_face* face;   // face the half-edge borders
    HE_edge* next;   // next half-edge around the face
};

struct HE_vert
{
	float verX;
	float verY;
	float verZ;

	float uvX;
	float uvY;

	float normX;
	float normY;
	float normZ;
	HE_edge* edge;  // one of the half-edges emantating from the vertex

	bool done;

	int neighCout;
	vector <float> neighArray;
};

struct HE_face
{
    HE_edge* edge;  // one of the half-edges bordering the face
};


class ObjectLoader:
	public Node
{

public :
	ObjectLoader(); ///< defoult constructor
	ObjectLoader(float scale1);  ///< constructor

	bool loadOBJ(
         const char * path, 
         std::vector<HE_vert*>& v,
		 std::vector<HE_edge*>& e,
		 std::vector<HE_face*>& f,
		 QGLFunctions funcs); ///< gets vectors, texture position and normals from .obj file

	std::vector< float* > vertices;
	std::vector< float* > uvs;
	std::vector< float* > normals;
	
	GLint getTexHan();  ///< teturns TecHan
	void loadTexture(QString newtexture);  ///< loads the texture for the object
	void update(Matrix4 matrix, std::vector<Carrier> &carrier);  ///< calls drawObject
	void scaleObj(float scale);  ///< scales the object
	void setSun(); ///< sets a bool sun to true to know if the object is a sun
	void bindBuffer(QGLFunctions funcs);///<binds information with the buffer
	void subdivide(QGLFunctions funcs); ///< subdivades the mesh
	void middleVertex(HE_vert *vert1, HE_vert *vert2, HE_vert *newVert); ///< uses un subdivide to calculate the position between to vertexes
	float getBeta(float tmp, int neigh);///< used in subdivide to calculate the beta witch is used when calculate the old vertexes new position
	vector<HE_vert*> ver;
	vector<HE_edge*> edge;
	vector<HE_face*> face;

private:
	int vertexSize;
	int uvSize;
	GLuint texHandle;
	float scale;
	float* drawmMat16;
	GLint texHandelID;
	GLuint vertexID;
	GLuint normalID;
	GLuint uvID;
	bool sun;
	int verIndex;

};
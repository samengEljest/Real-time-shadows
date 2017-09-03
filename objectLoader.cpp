#include "objectLoader.h"
#include <math.h>


ObjectLoader::ObjectLoader()
{}

ObjectLoader::ObjectLoader(float scale1)
{
	scale = scale1;
	sun = false;
	drawmMat16 = new float[16];

}

void ObjectLoader::loadTexture(QString newtexture)
{
	QString filename(newtexture);
	QImage tex = QImage(filename,"PNG");

	QImage texture = QGLWidget::convertToGLFormat(tex);

	int tw = texture.width();
	int th = texture.height();

	//This texture will be number 1 
	glGenTextures(1,&texHandle);
	glBindTexture(GL_TEXTURE_2D, texHandle);

	//Wrap tetxure coordinates for repeating. ex: (1.1,1.2) == (0.1,0.2)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//CLAMP/REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//Texture minification, texture magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	//How the texture will react with the environment. (Lighting etc)
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);//MODULATE/REPLACE

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0 ,GL_RGBA, GL_UNSIGNED_BYTE, (GLvoid*) texture.bits());
}

bool ObjectLoader::loadOBJ(
        const char * path, 
		std::vector<HE_vert*> &v,
		 std::vector<HE_edge*> &e,
		 std::vector<HE_face*> &f,
		 QGLFunctions funcs)
{
        printf("Loading OBJ file %s...\n", path);

        vector<unsigned int> vertexIndices, uvIndices, normalIndices;
        vector<float*> temp_vertices;
        vector<float*> temp_uvs;
        vector<float*> temp_normals;

		vector<float*> out_vertices; 
        vector<float*> out_uvs;
        vector<float*> out_normals;


        FILE * file = fopen(path, "r");
        if( file == NULL )
		{
                printf("Impossible to open the file ! Are you in the right path ?\n");
                return false;
        }

        while( 1 )
		{
                char lineHeader[128];
                // read the first word of the line
                int res = fscanf(file, "%s", lineHeader);
                if (res == EOF)
                        break; // EOF = End Of File. Quit the loop.

                // else : parse lineHeader
                
                if ( strcmp( lineHeader, "v" ) == 0 )
				{
						float* vertex = new float[3];
                        fscanf(file, "%f %f %f\n", &vertex[0], &vertex[1], &vertex[2] );
                        temp_vertices.push_back(vertex);
                }
				else if ( strcmp( lineHeader, "vt" ) == 0 )
				{
                        float* uv = new float[3];
                        fscanf(file, "%f %f\n", &uv[0], &uv[1] );
                       // uv[1] = -uv[1]; // Invert V coordinate since we will only use DDS texture, which are inverted. Remove if you want to use TGA or BMP loaders.
                        temp_uvs.push_back(uv);
                }
				else if ( strcmp( lineHeader, "vn" ) == 0 )
				{
                        float* normal = new float[3];
                        fscanf(file, "%f %f %f\n", &normal[0], &normal[1], &normal[2] );
                        temp_normals.push_back(normal);
                }
				else if ( strcmp( lineHeader, "f" ) == 0 )
				{
                        std::string vertex1, vertex2, vertex3;
                        unsigned int vertexIndex[3], uvIndex[3], normalIndex[3];
                        int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
                        if (matches != 9)
						{
                                printf("File can't be read by our simple parser :-( Try exporting with other options\n");
                                return false;
                        }
                        vertexIndices.push_back(vertexIndex[0]);
                        vertexIndices.push_back(vertexIndex[1]);
                        vertexIndices.push_back(vertexIndex[2]);
                        uvIndices    .push_back(uvIndex[0]);
                        uvIndices    .push_back(uvIndex[1]);
                        uvIndices    .push_back(uvIndex[2]);
                        normalIndices.push_back(normalIndex[0]);
                        normalIndices.push_back(normalIndex[1]);
                        normalIndices.push_back(normalIndex[2]);
                }
				else
				{
                        // Probably a comment, eat up the rest of the line
                        char stupidBuffer[1000];
                        fgets(stupidBuffer, 1000, file);
                }

        }

		 for( unsigned int i=0; i<vertexIndices.size(); i++ )
		 {
			  unsigned int vertexIndex = vertexIndices[i];
			  float* vertex = temp_vertices[ vertexIndex-1 ];
			  vertices.push_back(vertex);
		 }

		 for( unsigned int i=0; i<uvIndices.size(); i++ )
		 {
			  unsigned int uvIndex = uvIndices[i];
			  float* uv = temp_uvs[ uvIndex-1 ];
			  uvs.push_back(uv);
		 }

		 for( unsigned int i=0; i<normalIndices.size(); i++ )
		 {
			  unsigned int normIndex = normalIndices[i];
			  float* normal = temp_normals[ normIndex-1 ];
			  normals.push_back(normal);
		 }

		int vertexCount = 0;
		int normalCount = 0;
		int uvCount = 0;
		for (int i = 0; i < vertices.size();) //creates a new face with vertexes and edges
		{
			HE_edge *edge1 = new HE_edge();
			HE_edge *edge2 = new HE_edge();
			HE_edge *edge3 = new HE_edge();

			HE_vert *vertex1 = new HE_vert();
			HE_vert *vertex2 = new HE_vert();
			HE_vert *vertex3 = new HE_vert();

			HE_face *face = new HE_face();

			face->edge=edge1;

			vertex1->verX = vertices[i][0]*scale;
			vertex1->verY = vertices[i][1]*scale;
			vertex1->verZ = vertices[i][2]*scale;

			vertex1->normX = normals[i][0];
			vertex1->normY = normals[i][1];
			vertex1->normZ = normals[i][2];

			vertex1->uvX = uvs[i][0];
			vertex1->uvY = uvs[i][1];
			vertex1->edge = edge1;
			edge1->vert=vertex1;
			i++;

			//vertex2
			vertex2->verX = vertices[i][0]*scale;
			vertex2->verY = vertices[i][1]*scale;
			vertex2->verZ = vertices[i][2]*scale;

			vertex2->normX = normals[i][0];
			vertex2->normY = normals[i][1];
			vertex2->normZ = normals[i][2];

			vertex2->uvX = uvs[i][0];
			vertex2->uvY = uvs[i][1];
			vertex2->edge = edge2;
			edge2->vert=vertex2;
			i++;

			//vertex3
			vertex3->verX = vertices[i][0]*scale;
			vertex3->verY = vertices[i][1]*scale;
			vertex3->verZ = vertices[i][2]*scale;

			vertex3->normX = normals[i][0];
			vertex3->normY = normals[i][1];
			vertex3->normZ = normals[i][2];

			vertex3->uvX = uvs[i][0];
			vertex3->uvY = uvs[i][1];
			vertex3->edge = edge3;
			edge3->vert=vertex3;
			i++;
			
			edge1->face=face;
			edge1->next=edge2;

			edge2->face=face;
			edge2->next=edge3;

			edge3->face=face;
			edge3->next=edge1;

			e.push_back(edge1);
			e.push_back(edge2);
			e.push_back(edge3);
			
			v.push_back(vertex1);
			v.push_back(vertex2);
			v.push_back(vertex3);
			f.push_back(face);
		}
		for (int i = 0; i < face.size(); i++) //loops throw every face to find pair
		{
			HE_vert *vert1 = face[i]->edge->vert;
			HE_vert *vert2 = face[i]->edge->next->vert;
			HE_vert *vert3 = face[i]->edge->next->next->vert;
			if(face[i]->edge->pair == NULL) //first ege, if it not is paird
			{
				for (int b = 0; b < edge.size(); b++) //loops throw edges
				{
					if( edge[b]->vert->verX == vert2->verX &&
						edge[b]->vert->verY == vert2->verY &&
						edge[b]->vert->verZ == vert2->verZ &&
						edge[b]->next->vert->verX == vert1->verX &&
						edge[b]->next->vert->verY == vert1->verY &&
						edge[b]->next->vert->verZ == vert1->verZ) // checks if we have found the pair
					 {
						  face[i]->edge->pair = edge[b];
						  edge[b]->pair = face[i]->edge;
					 }
				}
			}
			if(face[i]->edge->next->pair == NULL)
			{
				for (int b = 0; b < edge.size(); b++)
				{
					if( edge[b]->vert->verX == vert3->verX &&
						edge[b]->vert->verY == vert3->verY &&
						edge[b]->vert->verZ == vert3->verZ && 
						edge[b]->next->vert->verX == vert2->verX &&
						edge[b]->next->vert->verY == vert2->verY &&
						edge[b]->next->vert->verZ == vert2->verZ)
					{
						 face[i]->edge->next->pair = edge[b];
						 edge[b]->pair = face[i]->edge->next;
					}
				}
			}
			if(face[i]->edge->next->next->pair == NULL)
			{
				for (int b = 0; b < edge.size(); b++)
				{
					 if(edge[b]->vert->verX == vert1->verX &&
						edge[b]->vert->verY == vert1->verY &&
						edge[b]->vert->verZ == vert1->verZ &&
						edge[b]->next->vert->verX == vert3->verX &&
						edge[b]->next->vert->verY == vert3->verY &&
						edge[b]->next->vert->verZ == vert3->verZ)
					 {
						  face[i]->edge->next->next->pair = edge[b];
						  edge[b]->pair = face[i]->edge->next->next;
					 }
				}
			}
		}
		for(int i=0,j=0;i<edge.size();i++)
		{
			if (edge[i]->pair == NULL)
			{
				cout<<endl<<j << " no paire ";
				j++;
			}
		}

		bindBuffer(funcs);
		return true;
}

void ObjectLoader::bindBuffer(QGLFunctions funcs)
{
	float *verBuffer = new float[3000000];
	float *normBuffer = new float[3000000];
	float *uvBuffer = new float[3000000];
	for(int i = 0, j = 0, k = 0, l = 0; k < face.size(); k++) //thows everything into an array that is sent to the buffer
	{
		  HE_vert* vert1 = face[k]->edge->vert;
		  HE_vert* vert2 = face[k]->edge->next->vert;
		  HE_vert* vert3 = face[k]->edge->next->next->vert;

		  verBuffer[i++] = vert1->verX;
		  verBuffer[i++] = vert1->verY;
		  verBuffer[i++] = vert1->verZ;
		  uvBuffer[j++] = vert1->uvX;
		  uvBuffer[j++] = vert1->uvY;
		  normBuffer[l++] = vert1->normX;
		  normBuffer[l++] = vert1->normY;
		  normBuffer[l++] = vert1->normZ;

		  verBuffer[i++] = vert2->verX;
		  verBuffer[i++] = vert2->verY;
		  verBuffer[i++] = vert2->verZ;
		  uvBuffer[j++] = vert2->uvX;
		  uvBuffer[j++] = vert2->uvY;
		  normBuffer[l++] = vert2->normX;
		  normBuffer[l++] = vert2->normY;
		  normBuffer[l++] = vert2->normZ;

		  verBuffer[i++] = vert3->verX;
		  verBuffer[i++] = vert3->verY;
		  verBuffer[i++] = vert3->verZ;
		  uvBuffer[j++] = vert3->uvX;
		  uvBuffer[j++] = vert3->uvY;
		  normBuffer[l++] = vert3->normX;
		  normBuffer[l++] = vert3->normY;
		  normBuffer[l++] = vert3->normZ;
	}


	//cout << ver.size() << " versize\n" << face.size() << " facesize\n" << edge.size() << " edgesize\n";
	funcs.glGenBuffers(1,&vertexID);
	funcs.glBindBuffer(GL_ARRAY_BUFFER, vertexID);
	funcs.glBufferData(GL_ARRAY_BUFFER, ((face.size()*9)*sizeof(float)), verBuffer,GL_STATIC_DRAW);

	funcs.glGenBuffers(1, &uvID);
	funcs.glBindBuffer(GL_ARRAY_BUFFER, uvID);
	funcs.glBufferData(GL_ARRAY_BUFFER, ((face.size()*6)*sizeof(float)), uvBuffer, GL_STATIC_DRAW);

	funcs.glGenBuffers(1, &normalID);
	funcs.glBindBuffer(GL_ARRAY_BUFFER, normalID);
	funcs.glBufferData(GL_ARRAY_BUFFER, ((face.size()*9)*sizeof(float)), normBuffer, GL_STATIC_DRAW);
	
	delete verBuffer;
	delete uvBuffer;
	delete normBuffer;
}

void ObjectLoader::update(Matrix4 matrix, std::vector<Carrier> &carrier)
{
	Carrier tmp = Carrier();

	tmp.boundingBottomY = 1*scale;
	tmp.boundingLeftX = 1*scale;
	tmp.boundingRightX = 1*scale;
	tmp.boundingTopY = 1*scale;

	tmp.vertexID = vertexID;
	tmp.uvID = uvID;
	tmp.normalID = normalID;

	tmp.verSize = face.size()*9;
	tmp.normSize = face.size()*9; 
	tmp.uvSize = face.size()*6;

	tmp.scale = scale;

	tmp.texture = this->texHandle;
	matrix = ~matrix; //maby gone
	tmp.trans = matrix;

	if(sun)
		tmp.sun = true;
	else 
		tmp.sun = false;

	carrier.push_back(tmp);
}

GLint ObjectLoader::getTexHan()
{
	return texHandle;
}

void ObjectLoader::scaleObj(float scale)
{
	Matrix4 M(4,4);
	M[0][0] = scale;
	M[1][1] = scale;
	M[2][2] = scale;
	float* M16 = new float[16];
	M16 = M.getMatrix16();
	glMultMatrixf(M16);
}

void ObjectLoader::setSun()
{
	sun = true;
}


void ObjectLoader::subdivide(QGLFunctions funcs)
{
	for(int i = 0; i < ver.size(); i++) //reset variables in vertecs ver
	{
		ver[i]->done = false;
		ver[i]->neighCout = 0;
		ver[i]->neighArray.clear();
	}
	int loopSize = edge.size();
	for(int i = 0; i < loopSize; i++) //finding how many and the cordinets for all vertexes naighbor vertexis
	{
		if (edge[i]->vert->done == false)
		{
			HE_vert *tmp_vert = new HE_vert;
			HE_edge *tmp_edge = new HE_edge;
			tmp_vert = edge[i]->vert;
			tmp_vert->neighCout++;
			double x,y,z;
			x = tmp_vert->edge->next->vert->verX;
			y = tmp_vert->edge->next->vert->verY;
			z = tmp_vert->edge->next->vert->verZ;

			edge[i]->vert->neighArray.push_back(x);
			edge[i]->vert->neighArray.push_back(y);
			edge[i]->vert->neighArray.push_back(z);

			edge[i]->vert->neighArray.push_back(tmp_vert->edge->next->vert->normX);
			edge[i]->vert->neighArray.push_back(tmp_vert->edge->next->vert->normY);
			edge[i]->vert->neighArray.push_back(tmp_vert->edge->next->vert->normZ);

			edge[i]->vert->neighArray.push_back(tmp_vert->edge->next->vert->uvX);
			edge[i]->vert->neighArray.push_back(tmp_vert->edge->next->vert->uvY);

			tmp_edge = tmp_vert->edge->next->next->pair->next;
			bool loop = true;
			while(loop)
			{
				if(tmp_edge->vert->verX == x && tmp_edge->vert->verY == y && tmp_edge->vert->verZ == z)
				{
					loop = false;
				}
				else
				{
					edge[i]->vert->neighCout++;
					edge[i]->vert->neighArray.push_back(tmp_edge->vert->verX);
					edge[i]->vert->neighArray.push_back(tmp_edge->vert->verY);
					edge[i]->vert->neighArray.push_back(tmp_edge->vert->verZ);

					edge[i]->vert->neighArray.push_back(tmp_edge->vert->normX);
					edge[i]->vert->neighArray.push_back(tmp_edge->vert->normY);
					edge[i]->vert->neighArray.push_back(tmp_edge->vert->normZ);

					edge[i]->vert->neighArray.push_back(tmp_edge->vert->uvX);
					edge[i]->vert->neighArray.push_back(tmp_edge->vert->uvY);
					tmp_edge = tmp_edge->next->pair->next;
				}
			}
			edge[i]->vert->done = true;
		}
	}
	verIndex = ver.size();
	loopSize = edge.size();
	for(int i = 0; i < loopSize; i++) //vreates a new vetex between two old vertexis and two new edges to have new pairs and next
	{
		if(edge[i]->next->pair->next->pair != edge[i])
		{
			HE_vert* middleVert = new HE_vert;
			HE_edge* newEdge = new HE_edge;
			HE_edge* newPaire = new HE_edge;

			middleVertex(edge[i]->vert, edge[i]->next->vert, middleVert);
			
			newEdge->vert = middleVert;
			newPaire->vert = middleVert;

			newEdge->next = edge[i]->next;
			newPaire->next = edge[i]->pair->next;

			edge[i]->next = newEdge;
			newEdge->pair = edge[i]->pair;
			newEdge->pair->next = newPaire;
			newPaire->pair = edge[i];
			newEdge->pair->pair = newEdge;

			edge[i]->pair = newPaire;
			middleVert->edge = newEdge;

			ver.push_back(middleVert);
			edge.push_back(newEdge);
			edge.push_back(newPaire);
		}
	}
	loopSize = face.size();
	for(int i = 0; i < loopSize; i++) //creats four new faces for one old face, and makes creates new edges for it
	{
		HE_face* face1 = new HE_face;
		HE_face* face2 = new HE_face;
		HE_face* face3 = new HE_face;

		HE_edge* edge1 = new HE_edge;
		HE_edge* edge2 = new HE_edge;
		HE_edge* edge4 = new HE_edge;

		HE_edge* edge31 = new HE_edge;
		HE_edge* edge32 = new HE_edge;
		HE_edge* edge34 = new HE_edge;

		face[i]->edge->next->face = face1;
		face1->edge = face[i]->edge->next;

		face[i]->edge->next->next->next->face = face2;
		face2->edge = face[i]->edge->next->next->next;

		edge4->next = face[i]->edge->next->next->next->next->next;

		edge2->vert = face2->edge->next->next->vert;
		face2->edge->next->next = edge2;
		edge2->pair = edge32;
		edge2->face = face2;
		edge32->pair = edge2;
		edge32->vert = face2->edge->vert;
		edge2->next = face2->edge;

		edge1->vert = face1->edge->next->next->vert;
		face1->edge->next->next = edge1;
		edge1->pair = edge31;
		edge1->face = face1;
		edge31->pair = edge1;
		edge31->vert = face1->edge->vert;
		edge1->next = face1->edge;

		edge4->vert = face[i]->edge->next->vert;
		face[i]->edge->next = edge4;
		edge4->pair = edge34;
		edge4->face = face[i];
		edge34->pair = edge4;
		edge34->vert = edge2->vert;

		edge31->next = edge32;
		edge32->next = edge34;
		edge34->next = edge31;
		edge31->face = face3;
		edge32->face = face3;
		edge34->face = face3;
		face3->edge = edge34;

		face2->edge->face = face2;
		face2->edge->next->face = face2;
		face1->edge->face = face1;
		face1->edge->next->face = face1;

		face.push_back(face1);
		face.push_back(face2);
		face.push_back(face3);
		edge.push_back(edge1);
		edge.push_back(edge2);
		edge.push_back(edge4);
		edge.push_back(edge31);
		edge.push_back(edge32);
		edge.push_back(edge34);
	}

	for(int i = verIndex; i < ver.size(); i++) //calculates the position of the new vertexis(middlevertex)
	{
		float vx = 0,vy = 0,vz = 0,nx = 0,ny = 0,nz = 0,ux = 0,uy = 0;
		vx += (ver[i]->edge->next->vert->verX * 3) / 8;
		vy += (ver[i]->edge->next->vert->verY * 3) / 8;
		vz += (ver[i]->edge->next->vert->verZ * 3) / 8;
		nx += (ver[i]->edge->next->vert->normX * 3) / 8;
		ny += (ver[i]->edge->next->vert->normY * 3) / 8;
		nz += (ver[i]->edge->next->vert->normZ * 3) / 8;
		ux += (ver[i]->edge->next->vert->uvX * 3) / 8;
		uy += (ver[i]->edge->next->vert->uvY * 3) / 8;

		vx += ver[i]->edge->next->next->pair->next->pair->next->next->vert->verX / 8;
		vy += ver[i]->edge->next->next->pair->next->pair->next->next->vert->verY / 8;
		vz += ver[i]->edge->next->next->pair->next->pair->next->next->vert->verZ / 8;
		nx += ver[i]->edge->next->next->pair->next->pair->next->next->vert->normX / 8;
		ny += ver[i]->edge->next->next->pair->next->pair->next->next->vert->normY / 8;
		nz += ver[i]->edge->next->next->pair->next->pair->next->next->vert->normZ / 8;
		ux += ver[i]->edge->next->next->pair->next->pair->next->next->vert->uvX / 8;
		uy += ver[i]->edge->next->next->pair->next->pair->next->next->vert->uvY / 8;

		vx += (ver[i]->edge->next->next->pair->next->next->pair->next->next->vert->verX * 3) / 8;
		vy += (ver[i]->edge->next->next->pair->next->next->pair->next->next->vert->verY * 3) / 8;
		vz += (ver[i]->edge->next->next->pair->next->next->pair->next->next->vert->verZ * 3) / 8;
		nx += (ver[i]->edge->next->next->pair->next->next->pair->next->next->vert->normX * 3) / 8;
		ny += (ver[i]->edge->next->next->pair->next->next->pair->next->next->vert->normY * 3) / 8;
		nz += (ver[i]->edge->next->next->pair->next->next->pair->next->next->vert->normZ * 3) / 8;
		ux += (ver[i]->edge->next->next->pair->next->next->pair->next->next->vert->uvX * 3) / 8;
		uy += (ver[i]->edge->next->next->pair->next->next->pair->next->next->vert->uvY * 3) / 8;

		vx += ver[i]->edge->pair->next->pair->next->next->pair->next->next->vert->verX / 8;
		vy += ver[i]->edge->pair->next->pair->next->next->pair->next->next->vert->verY / 8;
		vz += ver[i]->edge->pair->next->pair->next->next->pair->next->next->vert->verZ / 8;
		nx += ver[i]->edge->pair->next->pair->next->next->pair->next->next->vert->normX / 8;
		ny += ver[i]->edge->pair->next->pair->next->next->pair->next->next->vert->normY / 8;
		nz += ver[i]->edge->pair->next->pair->next->next->pair->next->next->vert->normZ / 8;
		ux += ver[i]->edge->pair->next->pair->next->next->pair->next->next->vert->uvX / 8;
		uy += ver[i]->edge->pair->next->pair->next->next->pair->next->next->vert->uvY / 8;

		ver[i]->verX = vx;
		ver[i]->verY = vy;
		ver[i]->verZ = vz;
		ver[i]->normX = nx;
		ver[i]->normY = ny;
		ver[i]->normZ = nz;
		//ver[i]->uvX = ux;
		//ver[i]->uvY = uy;
	}
	
	for(int i = 0; i < verIndex; i++)//calculates the position of the old certexis
	{

		float pi = 3.14159265359;
		float n = ver[i]->neighCout;
		float b = 1.0/n * (5.0/8.0 - (3.0/8.0 + 1.0/4.0 * cos(2.0 * pi/n)) * (3.0/8.0 + 1.0/4.0 * cos(2.0 * pi/n)));

		float vx = ver[i]->neighArray[0];
		float vy = ver[i]->neighArray[1];
		float vz = ver[i]->neighArray[2];

		float nx = ver[i]->neighArray[3];
		float ny = ver[i]->neighArray[4];
		float nz = ver[i]->neighArray[5];

		float ux = ver[i]->neighArray[6];
		float uy = ver[i]->neighArray[7];
		for(int j = 8; j < ver[i]->neighArray.size();)
		{
			vx += ver[i]->neighArray[j++];
			vy += ver[i]->neighArray[j++];
			vz += ver[i]->neighArray[j++];

			nx += ver[i]->neighArray[j++];
			ny += ver[i]->neighArray[j++];
			nz += ver[i]->neighArray[j++];

			ux += ver[i]->neighArray[j++];
			uy += ver[i]->neighArray[j++];
		}
		vx *= b;
		vy *= b;
		vz *= b;

		nx *= b;
		ny *= b;
		nz *= b;

		ux *= b;
		uy *= b;

		vx += (1.0-(n*b)) * ver[i]->verX;
		vy += (1.0-(n*b)) * ver[i]->verY;
		vz += (1.0-(n*b)) * ver[i]->verZ;

		nx += (1.0-(n*b)) * ver[i]->normX;
		ny += (1.0-(n*b)) * ver[i]->normY;
		nz += (1.0-(n*b)) * ver[i]->normZ;

		ux += (1.0-(n*b)) * ver[i]->uvX;
		uy += (1.0-(n*b)) * ver[i]->uvY;

		ver[i]->verX = vx;
		ver[i]->verY = vy;
		ver[i]->verZ = vz;

		//ver[i]->normX = nx;
		//ver[i]->normY = ny;
		//ver[i]->normZ = nz;

		//ver[i]->uvX = ux;
		//ver[i]->uvY = uy;
	}
	bindBuffer(funcs); //binds the new ifo with the buffer
}


void ObjectLoader::middleVertex(HE_vert *vert1, HE_vert *vert2, HE_vert *newVert)
{
	newVert->verX = (vert1->verX + vert2->verX)/2;
	newVert->verY = (vert1->verY + vert2->verY)/2;
	newVert->verZ = (vert1->verZ + vert2->verZ)/2;

	newVert->uvX = (vert1->uvX + vert2->uvX)/2;
	newVert->uvY = (vert1->uvY + vert2->uvY)/2;

	newVert->normX = (vert1->normX + vert2->normX)/2;
	newVert->normY = (vert1->normY + vert2->normY)/2;
	newVert->normZ = (vert1->normZ + vert2->normZ)/2;
}



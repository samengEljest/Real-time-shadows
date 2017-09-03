#pragma once
#include "GlWindow.h"

GlWindow::GlWindow(QWidget* parent) : QGLWidget(parent)
{
	doBeginning();
	this->timer.setInterval(1);
	connect(&this->timer, SIGNAL(timeout()), this, SLOT(updateGL()));
	this->timer.start();
}

void GlWindow::doBeginning()
{
	cameraXPos = 0;
	cameraYPos = 0;
	cameraZPos = 0;
	m_camera = Matrix4(4,4);
	m_id = Matrix4(4,4);
	m_camerapos = Matrix4(4,4);
	matrix = new float[16];
	matrixModView = new float[16];
	carrier=vector<Carrier>();
	objectCount = 0;	

	m_mouseIn = false;
	m_camPitch = 0.0f;
	m_camYaw = 0.0f;
	m_windowWidth = 1080;
	m_windowHeight = 720;
}

GlWindow::~GlWindow()
{}

void GlWindow::wireframe_On()
{
	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK, GL_LINE);
	glEnable(GL_CULL_FACE);
}

void GlWindow::wireframe_Off()
{
	glPolygonMode(GL_FRONT, GL_FILL);
	glPolygonMode(GL_BACK, GL_FILL);
	glDisable(GL_CULL_FACE);
}

void GlWindow::initializeGL()
{
	glClearColor(0.3f, 0.35f, 0.4f, 1);
	
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);
	glEnable(GL_TEXTURE_2D);

	this->funcs.initializeGLFunctions();

	bool compiled, linked, bound;
	compiled = this->program.addShaderFromSourceFile(QGLShader::Vertex,"vertex.vertexshader");
	if (!compiled)
	{
		QString error = this->program.log();
		qErrnoWarning((const char*)error.constData());
	}

	compiled = this->program.addShaderFromSourceFile(QGLShader::Fragment, "fragment.fragmentshader");
	if (!compiled)
	{
		QString error = this->program.log();
		qErrnoWarning((const char*)error.constData());
	}


	linked = this->program.link();

	if (!linked)
	{
		QString error = this->program.log();
		qErrnoWarning((const char*)error.constData());
	}
	else
	{
		this->modelLocation = this->program.uniformLocation("model");
		if (this->modelLocation == -1) qErrnoWarning("Model matrix has been removed because of optimization");

		this->viewLocation = this->program.uniformLocation("view");
		if (this->viewLocation == -1) qErrnoWarning("View matrix has been removed because of optimization");

		this->projectionLocation = this->program.uniformLocation("projection");
		if (this->projectionLocation == -1) qErrnoWarning("Projection matrix has been removed because of optimization");

		this->textureLocation = this->program.uniformLocation("tex");
		if (this->textureLocation == -1) qErrnoWarning("Texture variable has been removed because of optimization");

		this->vertexCoordLocation = this->program.attributeLocation("vertex");
		if (this->vertexCoordLocation == -1) qErrnoWarning("Vertex coordinate has been removed because of optimization");

		this->uvLocation = this->program.attributeLocation("uv");
		if (this->uvLocation == -1) qErrnoWarning("UV coordinate has been removed because of optimization");

		this->normalLocation = this->program.attributeLocation("normal");
		if (this->normalLocation == -1) qErrnoWarning("normal coordinate has been removed because of optimization");

		this->LightLocation = this->program.uniformLocation("LightPosition_worldspace");
		if (this->LightLocation == -1) qErrnoWarning("Light coordinate has been removed because of optimization");

		this->depthBiasLocation = this->program.uniformLocation("DepthBias");
		if (this->depthBiasLocation == -1) qErrnoWarning("depthBias matrix has been removed because of optimization");

		this->First_F = this->program.uniformLocation("is_First_f");
		if (this->First_F == -1) qErrnoWarning("fragmentbool has been removed because of optimization");

		this->First_V = this->program.uniformLocation("is_First_v");
		if (this->First_V == -1) qErrnoWarning("vertexbool has been removed because of optimization");
		
		this->shadowMapLocation = this->program.uniformLocation("shadowMap");
		if (this->shadowMapLocation == -1) qErrnoWarning("shadowMapLocation has been removed because of optimization");
	}

	this->program.enableAttributeArray(this->vertexCoordLocation);
	this->program.enableAttributeArray(this->uvLocation);

	funcs.glEnableVertexAttribArray(vertexCoordLocation);
	funcs.glEnableVertexAttribArray(uvLocation);
	funcs.glEnableVertexAttribArray(normalLocation);

	bound = this->program.bind();
	Q_ASSERT(bound);

	camera = new TransformNode(0,0,0,0,0);

	///////////////////////shadow
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, m_windowWidth, m_windowHeight, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D,0);

	funcs.glGenFramebuffers(1, &FBO);
	funcs.glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	funcs.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D ,shadowMap, 0);
	///////////////////////shadow

	TransformNode* light = new TransformNode(0,0,0.8f,0.0,0);
	ObjectLoader* lightObj = new ObjectLoader(0.2f);
	objects[objectCount] = lightObj;
	objectCount++;
	lightObj->setSun();
	lightObj->loadTexture("light.png");
	lightObj->loadOBJ("cube2.obj", lightObj->ver, lightObj->edge,lightObj->face, funcs);
	camera->addChild(light);
	light->addChild(lightObj);

	TransformNode* sun = new TransformNode(0,0,-8,0.0,0);
	ObjectLoader* sunObj = new ObjectLoader(3);
	objects[objectCount] = sunObj;
	objectCount++;
	sunObj->setSun();
	sunObj->loadTexture("sun.png");
	sunObj->loadOBJ("cube2.obj", sunObj->ver, sunObj->edge,sunObj->face, funcs);
	camera->addChild(sun);
	sun->addChild(sunObj);


	TransformNode* p1 = new TransformNode(0,0,-3,0.5f,0.2f);
	ObjectLoader* p1Obj = new ObjectLoader(0.25);
	objects[objectCount] = p1Obj;
	objectCount++;
	p1Obj->loadTexture("azurothBig.png");
	p1Obj->loadOBJ("sphere.obj", p1Obj->ver, p1Obj->edge, p1Obj->face, funcs);
	sun->addChild(p1);
	p1->addChild(p1Obj);
	
	TransformNode* p2 = new TransformNode(0,0,3,0.5f,0.2f);
	ObjectLoader* p2Obj = new ObjectLoader(0.25);
	objects[objectCount] = p2Obj;
	objectCount++;
	p2Obj->loadTexture("azurothBig.png");
	p2Obj->loadOBJ("sphere.obj", p2Obj->ver, p2Obj->edge, p2Obj->face, funcs);
	sun->addChild(p2);
	p2->addChild(p2Obj);
	

}

void GlWindow::rescaleWindow(int i_newWidth, int i_newHeight) 
{
	m_windowWidth = width();
	m_windowHeight = height();

	resizeGL(m_windowWidth,m_windowHeight);

	///////////////////////shadow
	glGenTextures(1, &shadowMap);
	glBindTexture(GL_TEXTURE_2D, shadowMap);
	glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT16, m_windowWidth, m_windowHeight, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glBindTexture(GL_TEXTURE_2D,0);

	funcs.glGenFramebuffers(1, &FBO);
	funcs.glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	funcs.glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D ,shadowMap, 0);
	///////////////////////shadow
}

void GlWindow::paintGL()
{

	if(m_windowWidth != width() || m_windowHeight != height()) 
	{
		rescaleWindow(width(), height());
	}

	funcs.glBindFramebuffer(GL_FRAMEBUFFER, FBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//camera
	Control(1.0f ,0.1f, m_mouseIn);
	updateCamera();

	Matrix4 trans(4,4);

	funcs.glUniform3f(LightLocation, 0,0,0);
	Matrix4 id(4,4);
	camera->update(id, carrier);

	///////////////first run
	funcs.glUniform1i(First_V,1);
	funcs.glUniform1i(First_F,1);

	for(int i = 0; i < carrier.size(); i++)
	{
		funcs.glUniformMatrix4fv(modelLocation, 1, false, carrier[i].trans.getMatrix16());

		//Verts
		funcs.glEnableVertexAttribArray(this->vertexCoordLocation);
		funcs.glBindBuffer(GL_ARRAY_BUFFER, carrier[i].vertexID);
		funcs.glVertexAttribPointer(vertexCoordLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//UVS
		funcs.glEnableVertexAttribArray(uvLocation);
		funcs.glBindBuffer(GL_ARRAY_BUFFER, carrier[i].uvID);
		funcs.glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);

		//Normals
		funcs.glEnableVertexAttribArray(normalLocation);
		funcs.glBindBuffer(GL_ARRAY_BUFFER, carrier[i].normalID);
		funcs.glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glDrawArrays(GL_TRIANGLES, 0, carrier[i].verSize);
	}

	funcs.glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	///////////////Secound run
	funcs.glUniform1i(First_V,0);
	funcs.glUniform1i(First_F,0);
	Matrix4 depthBias(4,4);
	double bias = 0.4999;

	depthBias[0][0] = bias;
	depthBias[1][1] = bias;
	depthBias[2][2] = bias;
	depthBias[0][3] = bias;
	depthBias[1][3] = bias;
	depthBias[2][3] = bias;

	funcs.glUniformMatrix4fv(depthBiasLocation, 1, true,depthBias.getMatrix16());
	
	funcs.glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D,shadowMap);
	funcs.glUniform1i(shadowMapLocation,0);

	for(int i = 0; i < carrier.size(); i++)
	{
		Matrix4 modelView(4,4);
		modelView = modelView  * carrier[i].trans * m_camera;

		float x = modelView[3][0];
		float y = modelView[3][1];
		float z = modelView[3][2];
		

		program.setUniformValue(textureLocation, 1);
		funcs.glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, carrier[i].texture);

		funcs.glUniformMatrix4fv(modelLocation, 1, false, carrier[i].trans.getMatrix16());

		//vertex
		funcs.glEnableVertexAttribArray(this->vertexCoordLocation);
		funcs.glBindBuffer(GL_ARRAY_BUFFER, carrier[i].vertexID);
		funcs.glVertexAttribPointer(vertexCoordLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);
		//uv
		funcs.glEnableVertexAttribArray(this->uvLocation);
		funcs.glBindBuffer(GL_ARRAY_BUFFER, carrier[i].uvID);
		funcs.glVertexAttribPointer(uvLocation, 2, GL_FLOAT, GL_FALSE, 0, 0);
		//normal
		funcs.glEnableVertexAttribArray(this->normalLocation);
		funcs.glBindBuffer(GL_ARRAY_BUFFER, carrier[i].normalID);
		funcs.glVertexAttribPointer(normalLocation, 3, GL_FLOAT, GL_FALSE, 0, 0);

		//draw
		glDrawArrays(GL_TRIANGLES,0,carrier[i].verSize); 

	}
	carrier.clear();
}


void GlWindow::resizeGL( int width, int height )
{
	m_proj = m_proj.persProj(60.0, 16/10, 0.001, 1000);
	funcs.glUniformMatrix4fv(projectionLocation, 1, false, m_proj.getMatrix16());
	glViewport(0, 0, width, height);
}

void GlWindow::updateCamera()
{
	Matrix4 traTmp(4,4);
	traTmp[3][0] = cameraXPos; 
	traTmp[3][1] = cameraYPos;
	traTmp[3][2] = cameraZPos;

	traTmp = traTmp * traTmp.rotateAxis(rotCamY, "Y");
	traTmp = traTmp * traTmp.rotateAxis(rotCamX, "X");

	m_camera = traTmp;

	funcs.glUniformMatrix4fv(viewLocation, 1, false, m_camera.getMatrix16());
}

void GlWindow::lockCamera()
{
	if (m_camPitch > 90)
	{
		m_camPitch = 90;
	}
	if (m_camPitch < -90)
	{
		m_camPitch = -90;
	}
	if (m_camYaw < 0.0)
	{
		m_camYaw += 360;
	}
	if (m_camYaw > 360)
	{
		m_camYaw -= 360;
	}
}

void GlWindow::Control(float i_moveVel, float i_mouseVel, bool i_mi)
{

	if(i_mi)
	{
		QPoint t_screenRes = mapToGlobal(QPoint(width()/2,height()/2));

		POINT t_mousePos;
		GetCursorPos(&t_mousePos);

		m_camYaw += i_mouseVel * (t_screenRes.rx()-t_mousePos.x);	
		m_camPitch += i_mouseVel * (t_screenRes.ry()-t_mousePos.y);

		lockCamera();
		SetCursorPos(t_screenRes.rx(), t_screenRes.ry());
	}
	rotCamX = m_camPitch;
	rotCamY = m_camYaw;
}

float GlWindow::getFrustumRightX(float deep)
{
	float pi = 3.14159265359;
	float tmp = pi - ((pi/3)*(16/10));
	return -(1.0f / 2.0f) * deep / sin(tmp/2);
}

float GlWindow::getFrustumLeftX(float deep)
{
	float pi = 3.14159265359;
	float tmp = pi - ((pi/3)*(16/10));
	return (1.0f / 2.0f) * deep / sin(tmp/2);
}

float GlWindow::getFrustumTopY(float deep)
{
	float pi = 3.14159265359;
	return -(1.0f / 2.0f) * deep / sin(pi / 3);
}

float GlWindow::getFrustumBottomY(float deep)
{
	float pi = 3.14159265359;
	return (1.0f / 2.0f) * deep / sin(pi / 3);
}



void GlWindow::keyPressEvent(QKeyEvent *event)
{
	float pi = 3.14159265359;
    if(event->key() == Qt::Key_W)
	{
		cameraZPos += cos(rotCamY*pi/180);
		cameraXPos += sin(rotCamY*pi/180);
		cameraYPos -= sin(rotCamX*pi/180);
	}
	else if(event->key() == Qt::Key_A)
	{
		cameraXPos += cos(rotCamY*pi/180);
		cameraZPos -= sin(rotCamY*pi/180);
	}
	else if(event->key() == Qt::Key_S)
	{
		cameraZPos -= cos(rotCamY*pi/180);
		cameraXPos -= sin(rotCamY*pi/180);
		cameraYPos += sin(rotCamX*pi/180);
	}
	else if(event->key() == Qt::Key_D)
	{
		cameraXPos -= cos(rotCamY*pi/180);
		cameraZPos += sin(rotCamY*pi/180);
	}
	else if(event->key() == Qt::Key_Up)
    {
        rotCamX++;
    }
	else if(event->key() == Qt::Key_Down)
    {
        rotCamX--;
    }
	else if(event->key() == Qt::Key_Left)
    {
        rotCamY++;
    }
	else if(event->key() == Qt::Key_Right)
    {
        rotCamY--;
    }
	else if(event->key() == Qt::Key_Space)
    {
        cameraYPos--;
    }
	else if(event->key() == Qt::Key_Control)
    {
        cameraYPos++;
    }
	else if(event->key() == Qt::Key_1)
    {
        RandObject();
    }
}
void GlWindow::keyReleaseEvent(QKeyEvent *event)
{
}

void GlWindow::mousePressEvent(QMouseEvent *event)
 {
	QPoint t_screenRes = mapToGlobal(QPoint(width()/2,height()/2));
	SetCursorPos(t_screenRes.rx(), t_screenRes.ry());

	GetCursorPos(&m_mousePos);
	m_mouseIn = true;
	ShowCursor(false);
 }

 void GlWindow::mouseReleaseEvent(QMouseEvent *event)
 {
	 m_mouseIn = false;
	 ShowCursor(true);
 }

void GlWindow::mouseMoveEvent(QMouseEvent *event)
{
	 /*ShowCursor(true);
	 POINT mousePos;
	 GetCursorPos(&mousePos);// Get the 2D mouse cursor (x,y) position     
	 SetCursorPos(320, 240); // Set the mouse cursor to 300,300

	 float dx = (mousePos.x - 320) / 10.0f;
	 float dy = (mousePos.y - 240) / 10.0f;

	 rotCamX -= dy * 1.0f;
	 rotCamY -= dx * 1.0f;*/
}
void GlWindow::RandObject()
{
	float distance = rand() % 100;
	float orbSpeed = rand() % 20;
	orbSpeed -= 10;
	orbSpeed /= 100;
	float selfSpeed = rand() % 20;
	selfSpeed -= 10;
	selfSpeed /= 100;

	float scale = rand() % 10;
	scale /= 10;

	TransformNode* p2 = new TransformNode(0,0,distance,orbSpeed,selfSpeed);
	ObjectLoader* p2Obj = new ObjectLoader(scale);
	objects[objectCount] = p2Obj;
	objectCount++;
	p2Obj->loadTexture("eatrh.png");
	p2Obj->loadOBJ("sphere.obj", p2Obj->ver, p2Obj->edge, p2Obj->face, funcs);
	camera->addChild(p2);
	p2->addChild(p2Obj);
}

void GlWindow::loopSub()
{
	for(int i = 0; i < objectCount; i++)
	{
		objects[i]->subdivide(funcs);
	}
}
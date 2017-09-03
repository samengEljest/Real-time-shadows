#pragma once
#include <QGLShaderProgram>
#include <QGLFunctions>
#include <QtOpenGL\QGLWidget>
#include <QGLWidget>
#include <QTimer>
#include <QMouseEvent>
#include "ui_lab2.h"
#include <GL/glu.h>

#include "Carrier.h"
#include <math.h>
#include <string>

#include "objectLoader.h"
#include "lightNode.h"
#include "node.h"
#include "transformNode.h"

#include <QKeyEvent>
#include <QMouseEvent>

#include "Matrix3.h"
#include "Matrix4.h"
#include "Vector3.h"
#include "Vector4.h"

/*!
	this is the class that handels the OpenGl window
*/
class GlWindow : public QGLWidget
{
	Q_OBJECT
public:
	GlWindow(QWidget* parent = 0); ///< opens the openGL window with the chosen file, sets the size to the picture
	~GlWindow();
	int objectCount;
	int lightX;
	int lightY;
	int lightZ;
	Matrix4 cameraRotate(float theta,string axis);  ///<  rotates the camera
	double rotCamX; 
	double rotCamY;
	void doBeginning(); ///< initialize stuff
	void updateCamera(); //updates the m_camera matrix

	void newObject(QString texture, const char* newobject); ///< creates a new object

	vector<Carrier> carrier;

public slots:
	void RandObject(); ///< creates a new pyramide
	void loopSub();
	void wireframe_On();
	void wireframe_Off();

private:
	Ui::Lab2Class ui;
	float rotate;
	QTimer timer;

	TransformNode* camera;
	TransformNode* ship;

	ObjectLoader *objects[500];

	double cameraXPos, cameraYPos, cameraZPos;
	QGLShaderProgram program;
	QGLFunctions funcs;

	GLuint m_fbo;
	GLuint m_shadowMap;

	Matrix4 m_camera;
	Matrix4 m_id;
	Matrix4 m_camerapos;
	Matrix4 m_Frustum;
	Matrix4 m_proj;

	float* matrix;
	float* matrixModView;
	float m_Movevel;

	GLuint texture;
	float tmpspeed;

	bool m_mouseIn;
	float m_camPitch;
	float m_camYaw;
	POINT m_mousePos;

	int m_windowWidth;
	int m_windowHeight;

protected:
	void initializeGL(); ///< initialize option for the gl window and sets the scene graph
	void paintGL(); ///< paints the picture on the openGL window
	void resizeGL(int width, int height);///< resize the gl window
	void keyPressEvent(QKeyEvent *); ///< if a key is pressd, do different things
    void keyReleaseEvent(QKeyEvent *); ///< if a key is releasd
	void mouseMoveEvent(QMouseEvent *event); ///< mouse movment
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);

	void Control(float i_moveVel, float i_mouseVel, bool i_mi);
	void lockCamera();
	void rescaleWindow(int i_newWidth, int i_newHeight);

	float getFrustumBottomY(float deep); ///<used to compare id object is inside frustrum
	float getFrustumTopY(float deep); ///<used to compare id object is inside frustrum
	float getFrustumLeftX(float deep); ///<used to compare id object is inside frustrum
	float getFrustumRightX(float deep); ///<used to compare id object is inside frustrum

	GLint vertexCoordLocation;
	GLint uvLocation;
	GLint normalLocation;
	GLint projectionLocation;
	GLint viewLocation;
	GLint modelLocation;
	GLint modelViewLocation;
	GLint textureLocation;
	GLint LightLocation;

	GLint First_F;
	GLint First_V;
	GLint depthLocation;
	GLint depthBiasLocation;
	GLuint shadowMapLocation;
	GLuint shadowMap;

	GLuint frameTextureID;
	GLuint FBO;
	

};

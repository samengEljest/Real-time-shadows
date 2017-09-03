#include "lab2.h"

Lab2::Lab2(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags)
{
	ui.setupUi(this);
	connect(ui.ObjectButton, SIGNAL(clicked()), this, SLOT(randObject()));
	connect(ui.LoopButton, SIGNAL(clicked()), this, SLOT(LoopObject()));
	connect(ui.wireframe_OFF, SIGNAL(clicked()), this, SLOT(wireframeOff()));
	connect(ui.wireframe_ON, SIGNAL(clicked()), this, SLOT(wireframeOn()));

}

Lab2::~Lab2()
{}
void Lab2::randObject()
{
	makeObject();
}

void Lab2::LoopObject()
{
	Division();
}

void Lab2::wireframeOn()
{
	On();
}

void Lab2::wireframeOff()
{
	Off();
}
#include <QtGui/QApplication>
#include "GlWindow.h"
#include "lab2.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	GlWindow w;
	Lab2 w2;
	QObject::connect(&w2, SIGNAL(makeObject()) , &w, SLOT(RandObject()));
	QObject::connect(&w2, SIGNAL(Division()) , &w, SLOT(loopSub()));
	QObject::connect(&w2, SIGNAL(Off()) , &w, SLOT(wireframe_Off()));
	QObject::connect(&w2, SIGNAL(On()) , &w, SLOT(wireframe_On()));
	w.show();
	w2.show(); 
	return a.exec();
}
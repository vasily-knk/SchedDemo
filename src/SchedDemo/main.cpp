#include "stdafx.h"
#include "scheddemo.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SchedDemo w;
	w.show();
	return a.exec();
}

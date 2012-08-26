#include "stdafx.h"
#include "schedscene.h"
#include "scheditem.h"
#include "scheddemo.h"


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

	SchedDemo demo(20, 50, 20, 50);
	demo.show();
	return app.exec();
}

#include "stdafx.h"
#include "schedscene.h"
#include "scheditem.h"
#include "scheddemo.h"


int main(int argc, char *argv[])
{
    /*const size_t N = 7;
    task_t task(N);
    perm_t perm(N);
    sched_t sched(N);

    for (size_t i = 0; i < N; ++i)
    {
        task[i].spans.resize(N);
        std::fill(task[i].spans.begin(), task[i].spans.end(), 60);

        //task[i].proc = 60;//(i + 1) * 40;
        task[i].tweight = (i + 1) * 20;
        task[i].due = 0;

        perm[i] = i;
    }
    std::swap(perm[0], perm[1]);*/

    QApplication app(argc, argv);

	SchedDemo demo(100, 250, 20, 100);
	demo.show();
	


	return app.exec();
    /*QWidget *gridGroupBox = new QWidget();

    QGridLayout *layout = new QGridLayout;
    SchedScene scene (&task, &perm, &sched);

    QGraphicsView *view = new QGraphicsView(&scene);
    view->setMouseTracking(true);

    gridGroupBox->setLayout(layout);

    QWidget *btnBox = new QWidget();
    QVBoxLayout *btnLayout = new QVBoxLayout();
    btnLayout->setAlignment(Qt::AlignTop);

    btnLayout->addWidget(new QPushButton("Hello"));
    btnLayout->addWidget(new QPushButton("World"));
    btnBox->setLayout(btnLayout);

    layout->addWidget(view, 0, 0);
    layout->addWidget(btnBox, 0, 1);


    gridGroupBox->show();
    gridGroupBox->setWindowTitle("Sched");*/

}





/*#include "stdafx.h"
#include "scheddemo.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	SchedDemo w;
	w.show();
	return a.exec();
}
*/
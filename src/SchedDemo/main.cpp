#include "stdafx.h"
#include "schedscene.h"
#include "scheditem.h"

void planes_task(float timespan, task_t &t);


cost_t calculate_cost(const task_t &task, const perm_t &perm)
{
    sched_t sched(task.size());
    perm2sched (task, perm, sched);
    return get_cost(task, sched);
}

perm_t random_solver(const task_t &t, const perm_t &src, size_t n_iters)
{
    perm_t dst (src);
    int counter = 0;
    for (size_t iter = 0; iter < n_iters; ++iter)
    {
        size_t i = rand() % (t.size() - 1);
        size_t j = i + 1 + rand() % (t.size() - i - 1);

        cost_t before = calculate_cost(t, dst);
        std::swap(dst[i], dst[j]);
        cost_t after = calculate_cost(t, dst);

        if (after >= before)
            std::swap(dst[i], dst[j]);
        else
            ++counter;
    }
    //cout << " " << counter << "/" << n_iters << " swaps" << endl;
    return dst;
}


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

    const size_t N = 10;
    const moment_t timespan = 300;

    task_t task(N);
    planes_task(timespan, task);

    perm_t perm(N);
    sched_t sched(N);

    for (size_t i = 0; i < N; ++i)
        perm[i] = i;

    perm = random_solver(task, perm, 10000);

    perm2sched(task, perm, sched);


    QApplication app(argc, argv);

    QWidget *gridGroupBox = new QWidget();

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
    gridGroupBox->setWindowTitle("Sched");

    return app.exec();
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
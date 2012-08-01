#ifndef SCHEDDEMO_H
#define SCHEDDEMO_H

class SchedScene;

class SchedDemo : public QWidget
{
	Q_OBJECT

public:
	SchedDemo(QWidget *parent = 0, Qt::WFlags flags = 0);
	~SchedDemo();

private:
	task_t task_;
	perm_t perm_;
	sched_t sched_;

	SchedScene *scene_;
};

#endif // SCHEDDEMO_H

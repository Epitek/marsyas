#ifndef MEAWS_EXERCISE_DISPATCHER_H
#define MEAWS_EXERCISE_DISPATCHER_H

#include "defs.h"
#include "exerciseIntonation.h"
#include "exerciseControl.h"

#include <QDialog>
#include <QFileDialog>
#include <QImage>
#include <QLabel>
#include <QtGui>

class ExerciseDispatcher : public QDialog {
	Q_OBJECT

public:
	ExerciseDispatcher();
	~ExerciseDispatcher();
	
public slots:
	void open();
	void setArea(QGridLayout *getInstructionArea, QGridLayout *getResultArea);

signals:
	void enableActions(int state);

private:
	bool chooseEvaluation();

	QString exerciseName;
	QGridLayout *instructionArea;
	QGridLayout *resultArea;

	Exercise *evaluation;
};
#endif


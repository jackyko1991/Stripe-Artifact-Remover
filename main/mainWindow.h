#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_stripe_artifact_remover.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QVTKWidget.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QMainWindow* parent = nullptr);
	~MainWindow();

protected:

private slots:

signals:

private:
	Ui::MainWindow ui;

};


#endif
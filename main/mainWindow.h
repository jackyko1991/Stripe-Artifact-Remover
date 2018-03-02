#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_stripe_artifact_remover.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QVTKWidget.h>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"

#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkFourierStripeArtifactImageFilter.h"
#include "itkFFTPadImageFilter.h"
#include <itkImageToVTKImageFilter.h>
#include "vtkImageViewer.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"

#include "vtkSmartPointer.h"

typedef float    PixelType;
const unsigned int      Dimension = 3;
typedef itk::Image< PixelType, Dimension >         ImageType;
typedef itk::ImageToVTKImageFilter<ImageType>       ConnectorType;

class vtkInteractorStyleDicom;

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QMainWindow* parent = nullptr);
	~MainWindow();

protected:

private slots :
	void loadDicom();
	void filter();

signals:

private:
	Ui::MainWindow ui;
	ImageType::Pointer m_originalImageItk;
	ImageType::Pointer m_filteredImageItk;

	vtkImageActor* m_originalActor;
	vtkImageActor* m_filteredActor;

	vtkRenderer* m_originalRender;
	vtkRenderer* m_filteredRender;

	void renderImages();
};


#endif
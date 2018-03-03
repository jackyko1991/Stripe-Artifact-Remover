#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_stripe_artifact_remover.h"

#include <QMainWindow>
#include <QFileDialog>
#include <QVTKWidget.h>
#include <QMessageBox>
#include <QTimer>

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkImageViewer2.h"

#include "itkImage.h"
#include "itkGDCMImageIO.h"
#include "itkGDCMSeriesFileNames.h"
#include "itkImageSeriesReader.h"
#include "itkFourierStripeArtifactImageFilter.h"
#include "itkFFTPadImageFilter.h"
#include "itkMinimumMaximumImageCalculator.h"
#include "itkRescaleIntensityImageFilter.h"

#include <itkImageToVTKImageFilter.h>

#include "vtkImageViewer.h"
#include "vtkImageActor.h"
#include "vtkRenderer.h"
#include "vtkTextProperty.h"
#include "vtkSmartPointer.h"
#include "vtkActor2D.h"
#include "vtkTextMapper.h"

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

	vtkRenderer* m_originalRender;
	vtkRenderer* m_filteredRender;

	vtkImageViewer2* m_originalImageViewer;
	vtkImageViewer2* m_filteredImageViewer;

	vtkInteractorStyleDicom* m_originalStyle;
	vtkInteractorStyleDicom* m_filteredStyle;

	vtkTextProperty* m_sliceTextProp;

	vtkTextMapper* m_originalSliceTextMapper;
	vtkActor2D* m_originalSliceTextActor;

	vtkTextMapper* m_filteredSliceTextMapper;
	vtkActor2D* m_filteredSliceTextActor;

	void renderImages();
};


#endif
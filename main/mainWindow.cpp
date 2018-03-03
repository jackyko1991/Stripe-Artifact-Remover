#include "mainWindow.h"
#include "vtkInteractorStyleDicom.h"

MainWindow::MainWindow(QMainWindow* parent)
{
	ui.setupUi(this);

	m_originalImageItk = ImageType::New();
	m_filteredImageItk = ImageType::New();

	m_originalRender = vtkRenderer::New();
	m_filteredRender = vtkRenderer::New();

	m_originalImageViewer = vtkImageViewer2::New();
	m_filteredImageViewer = vtkImageViewer2::New();

	m_originalImageViewer->SetRenderWindow(ui.originalQvtkWidget->GetRenderWindow());
	m_filteredImageViewer->SetRenderWindow(ui.filteredQvtkWidget->GetRenderWindow());

	m_originalImageViewer->SetupInteractor(ui.originalQvtkWidget->GetInteractor());
	m_filteredImageViewer->SetupInteractor(ui.filteredQvtkWidget->GetInteractor());

	m_originalStyle = vtkInteractorStyleDicom::New();
	m_filteredStyle = vtkInteractorStyleDicom::New();

	m_originalStyle->SetImageViewer(m_originalImageViewer);
	m_filteredStyle->SetImageViewer(m_filteredImageViewer);

	ui.originalQvtkWidget->GetInteractor()->SetInteractorStyle(m_originalStyle);
	ui.filteredQvtkWidget->GetInteractor()->SetInteractorStyle(m_filteredStyle);

	// display text
	m_sliceTextProp = vtkTextProperty::New();
	m_sliceTextProp->SetFontFamilyToCourier();
	m_sliceTextProp->SetFontSize(20);
	m_sliceTextProp->SetVerticalJustificationToBottom();
	m_sliceTextProp->SetJustificationToLeft();

	m_originalSliceTextMapper = vtkTextMapper::New();
	m_originalSliceTextMapper->SetTextProperty(m_sliceTextProp);

	std::string originalMsg = StatusMessage::Format(m_originalImageViewer->GetSliceMin(), m_originalImageViewer->GetSliceMax());
	m_originalSliceTextMapper->SetInput(originalMsg.c_str());
	m_originalStyle->SetStatusMapper(m_originalSliceTextMapper);

	m_originalSliceTextActor = vtkActor2D::New();
	m_originalSliceTextActor->SetMapper(m_originalSliceTextMapper);
	m_originalSliceTextActor->SetPosition(15, 10);

	m_filteredSliceTextMapper = vtkTextMapper::New();
	m_filteredSliceTextMapper->SetTextProperty(m_sliceTextProp);

	std::string filteredMsg = StatusMessage::Format(m_filteredImageViewer->GetSliceMin(), m_filteredImageViewer->GetSliceMax());
	m_filteredSliceTextMapper->SetInput(filteredMsg.c_str());
	m_filteredStyle->SetStatusMapper(m_filteredSliceTextMapper);

	m_filteredSliceTextActor = vtkActor2D::New();
	m_filteredSliceTextActor->SetMapper(m_filteredSliceTextMapper);
	m_filteredSliceTextActor->SetPosition(15, 10);

	m_originalImageViewer->GetRenderer()->AddActor2D(m_originalSliceTextActor);
	m_filteredImageViewer->GetRenderer()->AddActor2D(m_filteredSliceTextActor);


	ui.originalQvtkWidget->update();
	ui.filteredQvtkWidget->update();

	// connect singal slots
	connect(ui.loadPushButton, SIGNAL(clicked()), this, SLOT(loadDicom()));
	connect(ui.filterPushButton, SIGNAL(clicked()), this, SLOT(filter()));
}

MainWindow::~MainWindow()
{
	m_originalRender->Delete();
	m_filteredRender->Delete();
	m_originalStyle->Delete();
	m_filteredStyle->Delete();
	m_originalImageViewer->Delete();
	m_filteredImageViewer->Delete();
	m_sliceTextProp->Delete();

	m_originalSliceTextMapper->Delete();
	m_originalSliceTextActor->Delete();

	m_filteredSliceTextMapper->Delete();
	m_filteredSliceTextActor->Delete();
}

void MainWindow::filter()
{
	QMessageBox mbox;
	mbox.setWindowTitle(tr("Processing Images..."));
	mbox.setText("Please wait until all images are processed...");
	mbox.show();
	mbox.setStandardButtons(0);

	typedef itk::FFTPadImageFilter< ImageType > PadFilterType;
	PadFilterType::Pointer padFilter = PadFilterType::New();
	padFilter->SetInput(m_originalImageItk);
	padFilter->Update();

	std::cout << "fft ok" << std::endl;

	typedef itk::FourierStripeArtifactImageFilter< ImageType > FilterType;
	FilterType::Pointer filter = FilterType::New();

	filter->SetInput(padFilter->GetOutput());

	filter->SetDirection(ui.directionSpinBox->value());
	filter->SetSigma(ui.sigmaSpinBox->value());
	filter->SetStartFrequency(ui.frequencySpinBox->value());
	filter->Update();

	m_filteredImageItk->Graft(filter->GetOutput());
	m_filteredImageItk->SetMetaDataDictionary(filter->GetOutput()->GetMetaDataDictionary());


	m_filteredImageItk->Graft(m_originalImageItk);
	m_filteredImageItk->SetMetaDataDictionary(m_originalImageItk->GetMetaDataDictionary());


	std::cout << "filter image ok" << std::endl;

	mbox.setText("Image filtered!");
	QTimer timer;
	timer.start(5);
	mbox.hide();

	this->renderImages();
}

void MainWindow::renderImages()
{
	QMessageBox mbox;
	mbox.setWindowTitle(tr("Finalizing..."));
	mbox.setText("Finalizing...");
	mbox.show();
	mbox.setStandardButtons(0);

	// convert image from itk to vtk
	itk::RescaleIntensityImageFilter<ImageType, ImageType>::Pointer orginalRescaler = itk::RescaleIntensityImageFilter<ImageType, ImageType>::New();
	orginalRescaler->SetInput(m_originalImageItk);
	orginalRescaler->SetOutputMinimum(0);
	orginalRescaler->SetOutputMaximum(255);
	orginalRescaler->Update();
	m_originalImageItk->Graft(orginalRescaler->GetOutput());

	itk::RescaleIntensityImageFilter<ImageType, ImageType>::Pointer filteredRescaler = itk::RescaleIntensityImageFilter<ImageType, ImageType>::New();
	filteredRescaler->SetInput(m_originalImageItk);
	filteredRescaler->SetOutputMinimum(0);
	filteredRescaler->SetOutputMaximum(255);
	filteredRescaler->Update();
	m_filteredImageItk->Graft(filteredRescaler->GetOutput());

	mbox.setText("Ok!");
	QTimer timer;
	timer.start(5);
	mbox.hide();

	ConnectorType::Pointer originalConnector = ConnectorType::New();
	originalConnector->SetInput(m_originalImageItk);
	originalConnector->Update();

	ConnectorType::Pointer filteredConnector = ConnectorType::New();
	filteredConnector->SetInput(m_originalImageItk);
	filteredConnector->Update();

	m_originalImageViewer->SetInputData(originalConnector->GetOutput());
	m_filteredImageViewer->SetInputData(filteredConnector->GetOutput());

	m_originalStyle->UpdateSliceInfo();
	m_filteredStyle->UpdateSliceInfo();

	itk::MinimumMaximumImageCalculator<ImageType>::Pointer maxFilter = itk::MinimumMaximumImageCalculator<ImageType>::New();
	maxFilter->SetImage(m_originalImageItk);
	maxFilter->Compute();

	m_originalImageViewer->SetColorLevel(maxFilter->GetMaximum()/2);
	m_originalImageViewer->SetColorWindow(maxFilter->GetMaximum());

	m_filteredImageViewer->SetColorLevel(maxFilter->GetMaximum() / 2);
	m_filteredImageViewer->SetColorWindow(maxFilter->GetMaximum());

	m_originalImageViewer->Render();
	m_filteredImageViewer->Render();

	ui.originalQvtkWidget->update();
	ui.filteredQvtkWidget->update();
}

void MainWindow::loadDicom()
{
	QString sourceFile = QFileDialog::getExistingDirectory(this,
		tr("LDicom Directory"), "", QFileDialog::ShowDirsOnly
		| QFileDialog::DontResolveSymlinks);

	if (!sourceFile.isNull())
	{
		QMessageBox mbox;
		mbox.setWindowTitle(tr("Loading Dicom..."));
		mbox.setText("Please wait until all dicom images are loaded...");
		mbox.show();
		mbox.setStandardButtons(0);

		// load dicom images from the folder with itk loader
		typedef itk::ImageSeriesReader< ImageType >        ReaderType;
		ReaderType::Pointer reader = ReaderType::New();

		typedef itk::GDCMImageIO       ImageIOType;
		ImageIOType::Pointer dicomIO = ImageIOType::New();
		reader->SetImageIO(dicomIO);

		typedef itk::GDCMSeriesFileNames NamesGeneratorType;
		NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();
		nameGenerator->SetUseSeriesDetails(true);
		nameGenerator->AddSeriesRestriction("0008|0021");
		nameGenerator->SetDirectory(sourceFile.toStdString().c_str());

		typedef std::vector< std::string >    SeriesIdContainer;
		const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
		SeriesIdContainer::const_iterator seriesItr = seriesUID.begin();
		SeriesIdContainer::const_iterator seriesEnd = seriesUID.end();
		while (seriesItr != seriesEnd)
		{
			std::cout << seriesItr->c_str() << std::endl;
			++seriesItr;
		}

		std::string seriesIdentifier = seriesUID.begin()->c_str();

		typedef std::vector< std::string >   FileNamesContainer;
		FileNamesContainer fileNames;
		fileNames = nameGenerator->GetFileNames(seriesIdentifier);

		reader->SetFileNames(fileNames);

		try
		{
			reader->Update();
		}
		catch (itk::ExceptionObject &ex)
		{
			std::cout << ex << std::endl;
			return;
		}

		m_originalImageItk->Graft(reader->GetOutput());
		m_originalImageItk->SetMetaDataDictionary(reader->GetOutput()->GetMetaDataDictionary());
	
		mbox.setText("All image are loaded!");
		QTimer timer;
		timer.start(5);
		mbox.hide();

		// filter the images
		this->filter();
	}
}
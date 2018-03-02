#include "mainWindow.h"
#include "vtkInteractorStyleDicom.h"

MainWindow::MainWindow(QMainWindow* parent)
{
	ui.setupUi(this);

	m_originalImageItk = ImageType::New();
	m_filteredImageItk = ImageType::New();

	m_originalActor = vtkImageActor::New();
	m_filteredActor = vtkImageActor::New();

	m_originalRender = vtkRenderer::New();
	m_filteredRender = vtkRenderer::New();

	// vtk rendering stuff
	ui.originalQvtkWidget->GetRenderWindow()->AddRenderer(m_originalRender);
	ui.filteredQvtkWidget->GetRenderWindow()->AddRenderer(m_filteredRender);

	vtkSmartPointer<vtkInteractorStyleDicom> style =
		vtkSmartPointer<vtkInteractorStyleDicom>::New();

	ui.originalQvtkWidget->GetInteractor()->SetInteractorStyle(style);
	ui.filteredQvtkWidget->GetInteractor()->SetInteractorStyle(style);


	//ui.originalQvtkWidget->GetInteractor()->SetRenderWindow(renderWindow);

	ui.originalQvtkWidget->update();
	ui.filteredQvtkWidget->update();

	// connect singal slots
	connect(ui.loadPushButton, SIGNAL(clicked()), this, SLOT(loadDicom()));

}

MainWindow::~MainWindow()
{
	m_originalActor->Delete();
	m_filteredActor->Delete();
	m_originalRender->Delete();
	m_filteredRender->Delete();
}

void MainWindow::filter()
{
	typedef itk::FFTPadImageFilter< ImageType > PadFilterType;
	PadFilterType::Pointer padFilter = PadFilterType::New();
	padFilter->SetInput(m_originalImageItk);
	padFilter->Update();

	typedef itk::FourierStripeArtifactImageFilter< ImageType > FilterType;
	FilterType::Pointer filter = FilterType::New();

	filter->SetInput(padFilter->GetOutput());

	filter->SetDirection(ui.directionSpinBox->value());
	filter->SetSigma(ui.sigmaSpinBox->value());
	filter->SetStartFrequency(ui.frequencySpinBox->value());
	filter->Update();

	m_filteredImageItk->Graft(filter->GetOutput());
	m_filteredImageItk->SetMetaDataDictionary(filter->GetOutput()->GetMetaDataDictionary());

	std::cout << "filter image ok" << std::endl;

	this->renderImages();
}

void MainWindow::renderImages()
{
	// convert image from itk to vtk
	ConnectorType::Pointer originalConnector = ConnectorType::New();
	originalConnector->SetInput(m_originalImageItk);
	originalConnector->Update();

	ConnectorType::Pointer filteredConnector = ConnectorType::New();
	filteredConnector->SetInput(m_originalImageItk);
	filteredConnector->Update();

	m_originalActor->SetInputData(originalConnector->GetOutput());
	m_filteredActor->SetInputData(filteredConnector->GetOutput());

	m_originalRender->AddActor(m_originalActor);
	m_originalRender->ResetCamera();
	m_filteredRender->AddActor(m_filteredActor);
	m_filteredRender->ResetCamera();

	ui.originalQvtkWidget->GetInteractor()->Initialize();
	ui.filteredQvtkWidget->GetInteractor()->Initialize();

	ui.originalQvtkWidget->GetInteractor()->Start();
	ui.filteredQvtkWidget->GetInteractor()->Start();

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
	
		std::cout << "read image ok" << std::endl;

		// filter the images
		this->filter();
	}
}
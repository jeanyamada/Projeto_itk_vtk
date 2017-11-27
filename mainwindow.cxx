//Qt
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QFileInfoList>
#include <QDir>
#include <iostream>

//ITK
#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkImageToVTKImageFilter.h"
#include "itkMaskImageFilter.h"

//VTK
#include "vtkImageViewer2.h"
#include "vtkResliceImageViewer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkRenderer.h"

#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkTranslationTransform.h"



using namespace std;

MainWindow::MainWindow(QWidget * parent): QMainWindow(parent),
ui(new Ui::MainWindow) {
        ui -> setupUi(this);

        image_view_edited1 = vtkSmartPointer < vtkResliceImageViewer > ::New();
        image_view_edited2 = vtkSmartPointer < vtkResliceImageViewer > ::New();
        image_view_edited3 = vtkSmartPointer < vtkResliceImageViewer > ::New();
        image_view_original1 = vtkSmartPointer < vtkResliceImageViewer > ::New();
        image_view_original2 = vtkSmartPointer < vtkResliceImageViewer > ::New();
        image_view_original3 = vtkSmartPointer < vtkResliceImageViewer > ::New();
        filter = FilterType::New();
        nifti_io = itk::NiftiImageIO::New();

        //Qt Signal slot
        connect(ui -> actionOpen, SIGNAL(triggered()), this, SLOT(ReadDICOMseq()));
        connect(ui -> actionSave, SIGNAL(triggered()), this, SLOT(SaveDICOMseq()));
        connect(ui -> actionExit, SIGNAL(triggered()), this, SLOT(close()));
        connect(ui -> actionMultiplicar, SIGNAL(triggered()), this, SLOT(ReadMasc()));
    }

    MainWindow::~MainWindow() {
        delete ui;
    }

    int MainWindow::SaveDICOMseq() {

        QString filename = QFileDialog::getSaveFileName(
            this,
            tr("Save Image"),
            QDir::currentPath(),
            tr("Image (*.nii)") );

        typedef itk::ImageFileWriter<InputImageType>  WriterType;

        WriterType::Pointer writer = WriterType::New();

        writer->SetFileName(filename.toStdString().c_str());

        writer->SetInput(imageEdited);
        writer->SetImageIO(nifti_io);

        try
          {
          writer->Update();
          }
        catch (itk::ExceptionObject & e)
          {
          std::cerr << "Exception in file writer " << std::endl;
          std::cerr << e << std::endl;
          return EXIT_FAILURE;
          }
    }

    int MainWindow::ReadDICOMseq() {

        //Replacement of name generator of ITK
        QString fileName = QFileDialog::getOpenFileName(this, ("Open File"),
                                                          "/home",
                                                          ("Images (*.nii)"));

        typedef itk::ImageSeriesReader < InputImageType > ReaderType; //Reader of Image Type

        ReaderType::Pointer reader = ReaderType::New();

        reader -> SetFileName(fileName.toStdString().c_str());

        //Exceptional handling
        try {

            reader -> Update();
            this->imageOriginal = reader->GetOutput();

            reader->SetImageIO(nifti_io);
            renderOriginal();

            return EXIT_SUCCESS;

        } catch (itk::ExceptionObject & e) {
            std::cerr << "exception in file reader " << std::endl;
            std::cerr << e << std::endl;
            return EXIT_FAILURE;
        }

        return EXIT_SUCCESS;
    }

    bool MainWindow::renderOriginal(){

        typedef itk::ImageToVTKImageFilter < InputImageType > ConnectorType;

        ConnectorType::Pointer connector = ConnectorType::New();

        connector -> SetInput(imageOriginal);

        //Exceptional handling
        try {
            connector -> Update();

            //deep copy connector's output to an image else connector will go out of scope
            //and vanish it will cause error while changing slice

            vtkImageData * imageDataVtk = vtkImageData::New();

            imageDataVtk = vtkImageData::New();

            imageDataVtk -> DeepCopy(connector -> GetOutput());


            //set VTK Viewer to QVTKWidget in Qt's UI
            ui -> qVTKOriginal1 -> SetRenderWindow(image_view_original1 -> GetRenderWindow());
            ui -> qVTKOriginal2 -> SetRenderWindow(image_view_original2 -> GetRenderWindow());
            ui -> qVTKOriginal3 -> SetRenderWindow(image_view_original3 -> GetRenderWindow());

            image_view_original1 -> SetupInteractor(ui -> qVTKOriginal1 -> GetRenderWindow() -> GetInteractor());
            image_view_original2 -> SetupInteractor(ui -> qVTKOriginal2 -> GetRenderWindow() -> GetInteractor());
            image_view_original3 -> SetupInteractor(ui -> qVTKOriginal3 -> GetRenderWindow() -> GetInteractor());



            //Set input image to VTK viewer
            image_view_original1 -> SetInputData(imageDataVtk);
            image_view_original2 -> SetInputData(imageDataVtk);
            image_view_original3 -> SetInputData(imageDataVtk);

            ui->horizontalSlider->setMaximum(image_view_original1->GetSliceMax());
            ui->horizontalSlider->setMinimum(image_view_original1->GetSliceMin());

            image_view_original1 -> SetSlice(ui->horizontalSlider->value());
            image_view_original2 -> SetSlice(ui->horizontalSlider->value());
            image_view_original3 -> SetSlice(ui->horizontalSlider->value());

            image_view_original1->SetSliceOrientationToXY();
            image_view_original2->SetSliceOrientationToXZ();
            image_view_original3->SetSliceOrientationToYZ();

            image_view_original1 -> GetRenderer() -> ResetCamera();
            image_view_original1 -> Render();
            image_view_original2 -> GetRenderer() -> ResetCamera();
            image_view_original2 -> Render();
            image_view_original3 -> GetRenderer() -> ResetCamera();
            image_view_original3 -> Render();

            image_view_original1->SetColorLevel(2);
            image_view_original2->SetColorLevel(2);
            image_view_original3->SetColorLevel(2);


            return EXIT_SUCCESS;

        } catch (itk::ExceptionObject & e) {
            std::cerr << "exception in file reader " << std::endl;
            std::cerr << e << std::endl;
            return EXIT_FAILURE;
        }

    }

    int MainWindow::ReadMasc(){
        QString fileName = QFileDialog::getOpenFileName(this, ("Open File"),
                                                           "/home",
                                                           ("Images (*.nii)"));

        typedef itk::ImageSeriesReader < InputImageType > ReaderType; //Reader of Image Type

        ReaderType::Pointer reader = ReaderType::New();

         reader -> SetFileName(fileName.toStdString().c_str());

         //Exceptional handling
         try {
            reader -> Update();
            reader->SetImageIO(nifti_io);
            mask = reader->GetOutput();
            aplicaMascara();
         } catch (itk::ExceptionObject & e) {
             std::cerr << "exception in file reader " << std::endl;
             std::cerr << e << std::endl;
             return EXIT_FAILURE;
         }

    }

    bool MainWindow::aplicaMascara(){



        //float index[3] = {0.0,0.0,0.0};

        //imageOriginal->SetOrigin(index);
        //mask->SetOrigin(index);

        teste();

        std::cout <<"spacing original " <<imageOriginal->GetSpacing() << std::endl;

        std::cout <<"spacing mask " <<mask->GetSpacing() << std::endl;

        std::cout <<"origin original " <<imageOriginal->GetOrigin() << std::endl;

        std::cout <<"origin mask " <<mask->GetOrigin() << std::endl;

        std::cout <<"direção original " <<imageOriginal->GetDirection() << std::endl;

        std::cout <<"direção mask " << mask->GetDirection() << std::endl;

        typedef itk::MaskImageFilter< InputImageType,InputImageType > MaskFilterType;
        MaskFilterType::Pointer maskFilter = MaskFilterType::New();

        maskFilter->SetInput(imageOriginal);
        maskFilter->SetMaskImage(mask);

        //Exceptional handling
        try {
            maskFilter->Update();
            imageEdited = maskFilter->GetOutput();

            renderEdited();
        } catch (itk::ExceptionObject & e) {
            std::cerr << "exception in file reader " << std::endl;
            std::cerr << e << std::endl;
            return EXIT_FAILURE;
        }


    }

    void MainWindow::on_horizontalSlider_sliderMoved(int position) {

        if(position > image_view_original1->GetSliceMin() && position <image_view_original1->GetSliceMax()){
            image_view_original1 -> SetSlice(position);
            image_view_original1 -> Render();
        }
        if(position > image_view_original2->GetSliceMin() && position < image_view_original2->GetSliceMax()){
            image_view_original2 -> SetSlice(position);
            image_view_original2 -> Render();
        }

        if(position > image_view_original3->GetSliceMin() && position < image_view_original3->GetSliceMax()){
            image_view_original3 -> SetSlice(position);
            image_view_original3 -> Render();
        }

        if(position > image_view_edited1->GetSliceMin() && position < image_view_edited1->GetSliceMax()){
            image_view_edited1 -> SetSlice(position);
            image_view_edited1 -> Render();
        }

        if(position > image_view_edited2->GetSliceMin() && position < image_view_edited2->GetSliceMax()){
            image_view_edited2 -> SetSlice(position);
            image_view_edited2 -> Render();
        }

        if(position > image_view_edited3->GetSliceMin() && position < image_view_edited3->GetSliceMax()){
            image_view_edited3 -> SetSlice(position);
            image_view_edited3 -> Render();
        }

        ui -> centralWidget -> repaint();
    }

    bool MainWindow::teste(){
        typedef itk::ResampleImageFilter< InputImageType,InputImageType>  FilterTypeResample;
        FilterTypeResample::Pointer resampler = FilterTypeResample::New();

        resampler->SetReferenceImage(imageOriginal);

        resampler->UseReferenceImageOn();

        /*
        resampler->SetOutputSpacing(imageOriginal->GetSpacing());

        resampler->SetOutputOrigin(imageOriginal->GetOrigin());

        */
        resampler->SetSize(imageOriginal->GetLargestPossibleRegion().GetSize());

        /*
        InputImageType::IndexType start;
        start[0] = 0;
        start[1] = 0;

        resampler->SetOutputStartIndex(start);

        resampler->SetOutputDirection(imageOriginal->GetDirection());

        */

        resampler->SetNumberOfThreads();
        resampler->SetInput(mask);

        typedef itk::LinearInterpolateImageFunction<
                                          InputImageType,
                                         double>     InterpolatorType;


        InterpolatorType::Pointer interpolator = InterpolatorType::New();

        typedef itk::TranslationTransform< double, 3 >  TransformType;

        TransformType::Pointer transform = TransformType::New();

        transform->SetIdentity();


        resampler->SetInterpolator(interpolator);
        resampler->SetTransform(transform);



        //Exceptional handling
        try {
            resampler->Update();
            mask = resampler->GetOutput();
        } catch (itk::ExceptionObject & e) {
            std::cerr << "exception in file reader " << std::endl;
            std::cerr << e << std::endl;
            return EXIT_FAILURE;
        }

    }

    int MainWindow::aplicar(){

        filter -> SetInput(imageOriginal);

        try {

            filter-> Update();
            imageEdited = filter->GetOutput();
            renderEdited();

            return EXIT_SUCCESS;

        } catch (itk::ExceptionObject & e) {
            std::cerr << "exception in file reader " << std::endl;
            std::cerr << e << std::endl;
            return EXIT_FAILURE;
        }

    }

    bool MainWindow::renderEdited(){

        typedef itk::ImageToVTKImageFilter < InputImageType > ConnectorType;
        ConnectorType::Pointer connector = ConnectorType::New();

        connector -> SetInput(imageEdited);
        //Exceptional handling
        try {
            connector -> Update();

            //deep copy connector's output to an image else connector will go out of scope
            //and vanish it will cause error while changing slice
            vtkImageData * image = vtkImageData::New();
            image -> DeepCopy(connector -> GetOutput());

            ui -> qVTKEdited1 -> SetRenderWindow(image_view_edited1 -> GetRenderWindow());
            ui -> qVTKEdited2-> SetRenderWindow(image_view_edited2 -> GetRenderWindow());
            ui -> qVTKEdited3 -> SetRenderWindow(image_view_edited3 -> GetRenderWindow());

            image_view_edited1 -> SetupInteractor(ui -> qVTKEdited1-> GetRenderWindow() -> GetInteractor());
            image_view_edited2 -> SetupInteractor(ui -> qVTKEdited2 -> GetRenderWindow() -> GetInteractor());
            image_view_edited3 -> SetupInteractor(ui -> qVTKEdited3 -> GetRenderWindow() -> GetInteractor());

            //Set input image to VTK viewer
            image_view_edited1 -> SetInputData(image);
            image_view_edited2 -> SetInputData(image);
            image_view_edited3 -> SetInputData(image);

            image_view_edited1->SetSliceOrientationToXY();
            image_view_edited2->SetSliceOrientationToXZ();
            image_view_edited3->SetSliceOrientationToYZ();

            image_view_edited1 -> GetRenderer() -> ResetCamera();
            image_view_edited1 -> Render();
            image_view_edited2 -> GetRenderer() -> ResetCamera();
            image_view_edited2 -> Render();
            image_view_edited3 -> GetRenderer() -> ResetCamera();
            image_view_edited3 -> Render();


            image_view_edited1->SetColorLevel(2);
            image_view_edited2->SetColorLevel(2);
            image_view_edited3->SetColorLevel(2);



            return EXIT_SUCCESS;

        } catch (itk::ExceptionObject & e) {
            std::cerr << "exception in file reader " << std::endl;
            std::cerr << e << std::endl;
            return EXIT_FAILURE;
        }
    }

    void MainWindow::on_pushButton_clicked()
    {
        aplicar();
    }

    void MainWindow::on_pushButton_Mascara_clicked()
    {
        ReadMasc();
    }

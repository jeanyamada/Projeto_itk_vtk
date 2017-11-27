#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "vtkSmartPointer.h"
#include "itkImage.h"
#include "itkImageSeriesReader.h"
#include "itkImageSeriesWriter.h"
#include "ImageFilter.h"
#include <itkNiftiImageIO.h>



typedef signed short InputPixelType; //Pixel Type
typedef itk::Image < InputPixelType, 3> InputImageType; //Image Type
typedef itk::ImageFilter <InputImageType> FilterType;


class vtkImageViewer2;
class vtkResliceImageViewer;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
   
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
   
private slots:
    int ReadDICOMseq();
    int ReadMasc();
    void on_horizontalSlider_sliderMoved(int position);
    int SaveDICOMseq();
    int aplicar();
    bool renderOriginal();
    bool renderEdited();
    bool aplicaMascara();
    bool teste();

    void on_pushButton_clicked();


    void on_horizontalSlider_sliderPressed();

    void on_pushButton_Mascara_clicked();


private:
    vtkSmartPointer<vtkResliceImageViewer> image_view_edited1;
    vtkSmartPointer<vtkResliceImageViewer> image_view_edited2;
    vtkSmartPointer<vtkResliceImageViewer> image_view_edited3;
    vtkSmartPointer<vtkResliceImageViewer> image_view_original1;
    vtkSmartPointer<vtkResliceImageViewer> image_view_original2;
    vtkSmartPointer<vtkResliceImageViewer> image_view_original3;
    InputImageType::Pointer imageOriginal;
    InputImageType::Pointer imageEdited;
    InputImageType::Pointer mask;
    FilterType::Pointer filter;
    itk::NiftiImageIO::Pointer nifti_io;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H

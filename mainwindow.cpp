#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>


#include <iostream>

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionOpen_triggered(void){
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/", tr("Image Files (*.*)"));

    im1.load( fileName );

    //im1 = im1.convertToFormat(QImage::Format_Grayscale8);
    //im1.invertPixels( QImage::InvertRgb );

    pix1 = QPixmap::fromImage( im1 );
    sahne1.addPixmap( pix1 );

    ui->graphicsView->setScene( &sahne1 );

}

void MainWindow::on_pushButton_clicked()
{

    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), "/", tr("Image Files (*.*)"));


    Mat src1, dst1;

    //VideoCapture cap(fileName.toStdString());

    cap.open( fileName.toStdString() );

    if( !cap.isOpened()){
         cout << "Cannot open the video file" << endl;
         return;
    }

    double count = cap.get(CV_CAP_PROP_FRAME_COUNT); //get the frame count
    cap.set(CV_CAP_PROP_POS_FRAMES,15000); //Set index to last frame
    namedWindow("MyVideo",CV_WINDOW_AUTOSIZE);
    ui->horizontalSlider->setMaximum((int)count);



    Mat frame;
    bool success = cap.read(frame);
    if (!success){
      cout << "Cannot read  frame " << endl;

    }

    int ksize = frame.size().width/40;
    if( ksize%2 == 0)
        ksize++;

    cv::GaussianBlur(frame, frame, Size(ksize,ksize), 1, 1 );


   im1 = conv.Mat2QImage(frame);


    pix1 = QPixmap::fromImage(im1);
    sahne1.addPixmap( pix1 );

    ui->graphicsView->setScene( &sahne1 );




}

void MainWindow::on_horizontalSlider_valueChanged(int value)
{




    Mat frame, Ref_frame, frame_gray, Ref_frame_gray;

    cap.set(CV_CAP_PROP_POS_FRAMES,15000);
    bool success = cap.read(Ref_frame);
    if (!success){
      cout << "Cannot read  frame " << endl;

    }



    cap.set(CV_CAP_PROP_POS_FRAMES,(double)value);
    success = cap.read(frame);
    if (!success){
      cout << "Cannot read  frame " << endl;

    }
    //imshow("MyVideo", frame);

    cvtColor( frame, frame_gray, COLOR_BGR2GRAY );
    cvtColor( Ref_frame, Ref_frame_gray, COLOR_BGR2GRAY);

    int ksize = frame_gray.size().width/40;
    if( ksize%2 == 0)
        ksize++;



    cv::GaussianBlur(frame_gray, frame_gray, Size(ksize,ksize), 1, 1 );
    cv::GaussianBlur(Ref_frame, Ref_frame, Size(ksize,ksize), 1, 1 );


    Mat f1, f2, f3;

//    f1 = Ref_frame_gray - frame_gray;
//    f2 = cv::abs( f1 );

    double threshold_value = ui->horizontalSlider_threshold->value();

//    cv::threshold(f2, f3, threshold_value, 255, THRESH_BINARY);


    cv::threshold(frame_gray, f1, threshold_value, 255, THRESH_BINARY);

    f3 = ~f1;


    int morph_size = 5;
    Mat element = getStructuringElement( MORPH_ELLIPSE, Size( 2*morph_size + 1, 2*morph_size+1 ), Point( morph_size, morph_size ) );
      morphologyEx( f3, f3, MORPH_OPEN, element );
      morphologyEx(f3,f3,MORPH_CLOSE,element);


    Moments mu = moments( f3 );
    Point centroid = Point(  (int) (mu.m10/mu.m00), (int) (mu.m01/mu.m00) );
    circle( f3, centroid, 15, Scalar(255,0,0),1);






   im2 = conv.Mat2QImage(f3);


    pix2 = QPixmap::fromImage(im2);
    sahne2.clear();
    sahne2.addPixmap( pix2 );



    ui->graphicsView_2->setScene( &sahne2 );


    im1 = conv.Mat2QImage(frame_gray);


     pix1 = QPixmap::fromImage(im1);
     sahne1.clear();
     sahne1.addPixmap( pix1 );

     ui->graphicsView->setScene( &sahne1 );

}

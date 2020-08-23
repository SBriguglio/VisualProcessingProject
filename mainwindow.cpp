#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    file_path_left = "";
    file_path_right = "";
    scene_left = new QGraphicsScene(this);
    scene_right = new QGraphicsScene(this);
    connect(this->ui->graphicsView_image_left, SIGNAL(sendPixel(QPoint)), this, SLOT(receivePixelLeft(QPoint)));
    connect(this->ui->graphicsView_image_right, SIGNAL(sendPixel(QPoint)), this, SLOT(receivePixelRight(QPoint)));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_OpenLeft_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open the file");
    QPixmap imageLeft;
    if(!imageLeft.load(file_name)){
        QMessageBox::warning(this, "..", "file not open");
        return;
    }
    file_path_left = file_name;
    ui->label_filepath_left->setText("Left Image: " + file_path_left);
    scene_left->addPixmap(imageLeft.scaledToWidth(ui->graphicsView_image_left->width(), Qt::SmoothTransformation));
    ui->graphicsView_image_left->setScene(scene_left);
    ui->graphicsView_image_left->setSceneRect(0,0,imageLeft.width(), imageLeft.height());
    //ui->graphicsView_image_left->fitInView(ui->graphicsView_image_left->sceneRect(),Qt::KeepAspectRatio);
}

void MainWindow::on_pushButton_OpenRight_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open the file");
    QPixmap imageRight;
    if(!imageRight.load(file_name)){
        QMessageBox::warning(this, "..", "file not open");
        return;
    }
    file_path_right = file_name;
    ui->label_filepath_right->setText("Right Image: " + file_path_right);
    scene_right->addPixmap(imageRight.scaledToWidth(ui->graphicsView_image_right->width(), Qt::SmoothTransformation));
    scene_right->setSceneRect(imageRight.rect());
    ui->graphicsView_image_right->setScene(scene_right);
    ui->graphicsView_image_right->setSceneRect(0,0,imageRight.width(), imageRight.height());
    //ui->graphicsView_image_right->fitInView(ui->graphicsView_image_right->sceneRect(),Qt::KeepAspectRatio);
}

void MainWindow::on_pushButton_clicked() //should read on_pushButton_match_pixels_clicked
{
    if(file_path_left == "" || file_path_right == ""){
        QMessageBox::warning(this, "Error", "You must select two image files before pixels can be matched.");
        return;
    }
    QString question = "Match pixels by first selecting a pixel in the left image "
                       "and then selecting the matching pixel in the right image. "
                       "Be sure to choose the pixels very carefully and to choose "
                       "the same number of pixels in each image. Any inconsistancies "
                       "can greatly reduce accuracy and matching may need to be redone. "
                       "You must choose atleast 10 pairs of pixels in order to continue.\n"
                       "Press \"STOP\" to stop matching.\n\n"
                       "Click \"Yes\" to continue.";
    if(QMessageBox::question(this, "Pixel Matching", question)==QMessageBox::Yes){
        ui->pushButton_stop->setEnabled(true);
        ui->graphicsView_image_left->setInteractive(true);
        ui->graphicsView_image_right->setInteractive(true);
        ui->pushButton_OpenLeft->setVisible(false);
        ui->pushButton_OpenRight->setVisible(false);
    } else return;

}

void MainWindow::on_pushButton_stop_clicked()
{
    ui->pushButton_stop->setEnabled(false);
    ui->graphicsView_image_left->setInteractive(false);
    ui->graphicsView_image_right->setInteractive(false);
    if((ui->graphicsView_image_left->getSize()<10) || (ui->graphicsView_image_right->getSize()<10)){
        QMessageBox::warning(this, "Error", "You must match at least 10 pixels in each image!");
        ui->pushButton_stop->setEnabled(true);
        ui->graphicsView_image_left->setInteractive(true);
        ui->graphicsView_image_right->setInteractive(true);
    }
    if((ui->graphicsView_image_left->getSize()!=ui->graphicsView_image_right->getSize())){
        QMessageBox::warning(this, "Error", "Mismatch in the number of pixels selected for each image. "
                                            "Please select another pixel or restart the program");
        ui->pushButton_stop->setEnabled(true);
        ui->graphicsView_image_left->setInteractive(true);
        ui->graphicsView_image_right->setInteractive(true);
    } else {
        ui->pushButton_EpipolarMode->setEnabled(true);
    }
}

void MainWindow::receivePixelRight(QPoint pixel)
{
    //this->ui->statusbar->showMessage(QString("RIGHT X: %1 Y: %2").arg(pixel.x()).arg(pixel.y()));
    this->ui->statusbar->showMessage(QString("RIGHT X: %1 Y: %2  ||  Left: %3 pixels Right: %4 pixels")
                                     .arg(pixel.x())
                                     .arg(pixel.y())
                                     .arg(this->ui->graphicsView_image_left->getSize())
                                     .arg(this->ui->graphicsView_image_right->getSize()));
}

void MainWindow::drawEpipolarLineLeft(QPoint pixelFromRight)
{
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(255,127,0));

    float height = this->ui->graphicsView_image_left->sceneRect().height();
    float width = this->ui->graphicsView_image_left->sceneRect().width();
    float maxLength = sqrt(pow(height, 2)+pow(width, 2));
    qDebug() << "height: " << height;
    qDebug() << "w: " << width;
    float px = pixelFromRight.x();
    float py = pixelFromRight.y();
    qDebug() << "Right pixel selected: (" << px <<", " << py <<" )";
    qDebug() << "I";

    cv::Mat fM = this->ui->graphicsView_image_right->getcvFundamentalMatrix();
    cv::Mat p(3,1,CV_64FC1);

    p.at<double>(0,0) = px;
    p.at<double>(1,0) = py;
    p.at<double>(2,0) = 1;

    cv::Mat line = fM*p;

    //std::cerr << "fM " << endl << " " << fM << endl << endl;
    //std::cerr << "p " << endl << " " << p << endl << endl;
    //std::cerr << "line" << endl << " " << line << endl << endl;

    float a = line.at<double>(0,0);
    float b = line.at<double>(1,0);
    float c = line.at<double>(2,0);
    qDebug() << "I" << a << b << c;

    float x0 = 0;
    float y0 = (0-c)/a;
    float x1 = (((-1)*height*b)-c)/a;
    float y1 = height;

    QPointF basepoint = this->ui->graphicsView_image_left->mapToScene(x0, y0);
    QPointF endpoint = this->ui->graphicsView_image_left->mapToScene(x1, y1);
    //scene_left->addLine(x0,y0,x1,y1,pen);
    QLineF sLine(basepoint, endpoint+QPointF(1,0));

    sLine.setLength(maxLength);
    scene_left->addLine(sLine, pen);
}

void MainWindow::drawEpipolarLineRight(QPoint pixelFromLeft)
{
    QPen pen;
    pen.setWidth(1);
    pen.setColor(QColor(255,127,0));
    float height = this->ui->graphicsView_image_right->sceneRect().height();
    float width = this->ui->graphicsView_image_right->sceneRect().width();
    float maxLength = sqrt(pow(height, 2)+pow(width, 2));

    qDebug() << "height (left): " << height;

    float px = pixelFromLeft.x();
    float py = pixelFromLeft.y();
    qDebug() << "Left pixel selected: (" << px <<", " << py <<" )";
    cv::Mat fM = this->ui->graphicsView_image_left->getcvFundamentalMatrix();
    cv::Mat p(3,1,CV_64FC1);

    p.at<double>(0,0) = px;
    p.at<double>(1,0) = py;
    p.at<double>(2,0) = 1;

    cv::Mat line = fM*p;

    //std::cerr << "fM " << endl << " " << fM << endl << endl;
    //std::cerr << "p " << endl << " " << p << endl << endl;
    //std::cerr << "line" << endl << " " << line << endl << endl;

    double a = line.at<double>(0,0);
    double b = line.at<double>(1,0);
    double c = line.at<double>(2,0);

    qDebug() << "I" << a << b << c;

    float x0 = 0;
    float y0 = (0-c)/a;
    float x1 = (((-1)*height*b)-c)/a;
    float y1 = height;

    QPointF basepoint = this->ui->graphicsView_image_right->mapToScene(x0, y0);
    QPointF endpoint = this->ui->graphicsView_image_right->mapToScene(x1, y1);
    //scene_left->addLine(x0,y0,x1,y1,pen);
    QLineF sLine(basepoint, endpoint+QPointF(1,0));

    sLine.setLength(maxLength);
    scene_right->addLine(sLine, pen);
}

void MainWindow::receivePixelLeft(QPoint pixel)
{
    //this->ui->statusbar->showMessage(QString("LEFT X: %1 Y: %2").arg(pixel.x()).arg(pixel.y()));
    this->ui->statusbar->showMessage(QString("LEFT X: %1 Y: %2  ||  Left: %3 pixels Right: %4 pixels")
                                     .arg(pixel.x())
                                     .arg(pixel.y())
                                     .arg(this->ui->graphicsView_image_left->getSize())
                                     .arg(this->ui->graphicsView_image_right->getSize()));
}


void MainWindow::on_pushButton_EpipolarMode_clicked()
{
    this->ui->pushButton->setEnabled(false);
    this->ui->pushButton->setVisible(false);
    this->ui->pushButton_stop->setEnabled(false);
    this->ui->pushButton_stop->setVisible(false);
    pixelChooser *a = this->ui->graphicsView_image_left;
    pixelChooser *b = this->ui->graphicsView_image_right;
    a->fillFundamentalMatrix(b);  //set fundamental matrix a-to-b
    b->fillFundamentalMatrix(a);  //set fundamental matrix b-to-a

    this->ui->pushButton_EpipolarMode->setEnabled(false);
    this->ui->statusbar->showMessage("Epipolar Mode Enabled. Select pixels to draw the resulting epipolar line.");

    //disconnet old slots
    disconnect(this->ui->graphicsView_image_left, SIGNAL(sendPixel(QPoint)), this, SLOT(receivePixelLeft(QPoint)));
    disconnect(this->ui->graphicsView_image_right, SIGNAL(sendPixel(QPoint)), this, SLOT(receivePixelRight(QPoint)));
    //connect new slots
    connect(this->ui->graphicsView_image_left, SIGNAL(sendPixel(QPoint)), this, SLOT(drawEpipolarLineRight(QPoint)));
    connect(this->ui->graphicsView_image_right, SIGNAL(sendPixel(QPoint)), this, SLOT(drawEpipolarLineLeft(QPoint)));

    this->ui->graphicsView_image_left->setInteractive(true);
    this->ui->graphicsView_image_right->setInteractive(true);


}

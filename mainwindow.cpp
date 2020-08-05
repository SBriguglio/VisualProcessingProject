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
    scene_left->addPixmap(imageLeft);
    ui->graphicsView_image_left->setScene(scene_left);
    ui->graphicsView_image_left->fitInView(ui->graphicsView_image_left->sceneRect(),Qt::KeepAspectRatio);
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
    scene_right->addPixmap(imageRight);
    scene_right->setSceneRect(imageRight.rect());
    ui->graphicsView_image_right->setScene(scene_right);
    ui->graphicsView_image_right->fitInView(ui->graphicsView_image_right->sceneRect(),Qt::KeepAspectRatio);
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

void MainWindow::receivePixelLeft(QPoint pixel)
{
    //this->ui->statusbar->showMessage(QString("LEFT X: %1 Y: %2").arg(pixel.x()).arg(pixel.y()));
    this->ui->statusbar->showMessage(QString("LEFT X: %1 Y: %2  ||  Left: %3 pixels Right: %4 pixels")
                                     .arg(pixel.x())
                                     .arg(pixel.y())
                                     .arg(this->ui->graphicsView_image_left->getSize())
                                     .arg(this->ui->graphicsView_image_right->getSize()));
}





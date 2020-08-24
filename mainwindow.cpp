#include "mainwindow.h"
#include "ui_mainwindow.h"

float getAverage(QPoint, QImage*);
float getSD(QPoint, QImage*);
float znccScore(QPoint, QPoint, QImage*, QImage*);

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
    ui->graphicsView_image_left->picture = imageLeft;
    file_path_left = file_name;
    ui->label_filepath_left->setText("Left Image: " + file_path_left);
    scene_left->addPixmap(imageLeft);
    //scene_left->setSceneRect(imageLeft.rect());
    ui->graphicsView_image_left->setScene(scene_left);
    //ui->graphicsView_image_left->setSceneRect(0, 0, imageLeft.width(), imageLeft.height());
    ui->graphicsView_image_left->fitInView(0, 0, imageLeft.width(), imageLeft.height(), Qt::IgnoreAspectRatio);
    //ui->graphicsView_image_left->setAlignment(Qt::AlignCenter);
}

void MainWindow::on_pushButton_OpenRight_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Open the file");
    QPixmap imageRight;
    if(!imageRight.load(file_name)){
        QMessageBox::warning(this, "..", "file not open");
        return;
    }
    ui->graphicsView_image_right->picture = imageRight;
    file_path_right = file_name;
    ui->label_filepath_right->setText("Right Image: " + file_path_right);
    scene_right->addPixmap(imageRight.scaledToWidth(ui->graphicsView_image_right->width(), Qt::SmoothTransformation));
    //scene_right->setSceneRect(imageRight.rect());
    ui->graphicsView_image_right->setScene(scene_right);
    //ui->graphicsView_image_right->setSceneRect(0,0,imageRight.width(), imageRight.height());
    ui->graphicsView_image_right->fitInView(ui->graphicsView_image_right->sceneRect(),Qt::IgnoreAspectRatio);
    //ui->graphicsView_image_right->setAlignment(Qt::AlignCenter);
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
        ui->graphicsView_image_left->setAddbit(false);
        ui->graphicsView_image_right->setAddbit(false);
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
    pen.setWidth(2);
    pen.setColor(QColor(255,127,0));

    float height = this->ui->graphicsView_image_left->sceneRect().height();
    float width = this->ui->graphicsView_image_left->sceneRect().width();
    //float maxLength = sqrt(pow(600, 2)+pow(800, 2));

    float px = pixelFromRight.x();
    float py = pixelFromRight.y();

    qDebug() << "Right pixel selected: (" << px <<", " << py <<" )";

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
    float y0 = (0-c)/b;
    float x1 = (((-1)*height*b)-c)/a;
    float y1 = height;

    //check x,y within bounds
    if(y0<0 || y0>600){
        ++x0;
        while((y0 < 0 || y0 > 600) && x0 < 800){
            y0 = ((a*(-1)*x0)-c)/b;
            ++x0;
        }
        --x0;
    }
    if(x1<0 || x1>800){
        --y1;
        while((x1 < 0 || x1 > 800) && y1 > 0){
            x1 = ((b*(-1)*y1)-c)/a;
            --y1;
        }
        ++y1;
    }

    QPointF basepoint = QPointF(x0, y0);
    QPointF endpoint = QPointF(x1, y1);
    QLineF sLine(basepoint, endpoint+QPointF(1,0));

    //sLine.setLength(maxLength);
    scene_left->addLine(sLine, pen);
}

void MainWindow::drawEpipolarLineRight(QPoint pixelFromLeft)
{
    QPen pen;
    pen.setWidth(2);
    pen.setColor(QColor(255,127,255));

    float height = this->ui->graphicsView_image_right->sceneRect().height();
    float width = this->ui->graphicsView_image_right->sceneRect().width();
    //float maxLength = sqrt(pow(600, 2)+pow(800, 2));

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
    float y0 = (0-c)/b;
    float x1 = (((-1)*height*b)-c)/a;
    float y1 = height;

    //check x,y within bounds
    if(y0<0 || y0>600){
        ++x0;
        while((y0 < 0 || y0 > 600) && x0 < 800){
            y0 = ((a*(-1)*x0)-c)/b;
            ++x0;
        }
        --x0;
    }
    if(x1<0 || x1>800){
        --y1;
        while((x1 < 0 || x1 > 800) && y1 > 0){
            x1 = ((b*(-1)*y1)-c)/a;
            --y1;
        }
        ++y1;
    }

    QPointF basepoint = QPointF(x0, y0);
    QPointF endpoint = QPointF(x1, y1);
    QLineF sLine(basepoint, endpoint+QPointF(1,0));

    //sLine.setLength(maxLength);
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
    this->ui->pushButton_pmatching->setEnabled(true);
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

void MainWindow::on_pushButton_pmatching_clicked()
{
    disconnect(this->ui->graphicsView_image_left, SIGNAL(sendPixel(QPoint)), this, SLOT(drawEpipolarLineRight(QPoint)));
    disconnect(this->ui->graphicsView_image_right, SIGNAL(sendPixel(QPoint)), this, SLOT(drawEpipolarLineLeft(QPoint)));

    this->ui->pushButton_EpipolarMode->setVisible(false);
    this->ui->pushButton_pmatching->setEnabled(false);

    scene_left->clear();
    scene_right->clear();

    QPixmap imageLeft = ui->graphicsView_image_left->picture;
    QPixmap imageRight = ui->graphicsView_image_right->picture;

    QImage imagel =(imageLeft.toImage().convertToFormat(QImage::Format_Indexed8));
    QImage imager = (imageRight.toImage().convertToFormat(QImage::Format_Indexed8));

    ui->graphicsView_image_left->pic = imagel;
    ui->graphicsView_image_right->pic = imager;

    scene_left->addPixmap(imageLeft.scaledToWidth(ui->graphicsView_image_left->width(), Qt::SmoothTransformation));
    scene_left->setSceneRect(imageLeft.rect());
    ui->graphicsView_image_left->setScene(scene_left);
    ui->graphicsView_image_left->setSceneRect(0,0,imageLeft.width(), imageLeft.height());

    scene_right->addPixmap(imageRight.scaledToWidth(ui->graphicsView_image_right->width(), Qt::SmoothTransformation));
    scene_right->setSceneRect(imageRight.rect());
    ui->graphicsView_image_right->setScene(scene_right);
    ui->graphicsView_image_right->setSceneRect(0,0,imageRight.width(), imageRight.height());

    connect(this->ui->graphicsView_image_left, SIGNAL(sendPixel(QPoint)), this, SLOT(matchPixelModeRight(QPoint)));
    connect(this->ui->graphicsView_image_right, SIGNAL(sendPixel(QPoint)), this, SLOT(matchPixelModeLeft(QPoint)));

}

void MainWindow::matchPixelModeLeft(QPoint pixel){
    QPen pen;
    pen.setWidth(3);
    pen.setColor(QColor(255,127,50));

    float height = this->ui->graphicsView_image_left->sceneRect().height();
    float width = this->ui->graphicsView_image_left->sceneRect().width();

    float px = pixel.x();
    float py = pixel.y();

    qDebug() << "Right pixel selected: (" << px <<", " << py <<" )";

    cv::Mat fM = this->ui->graphicsView_image_right->getcvFundamentalMatrix();
    cv::Mat p(3,1,CV_64FC1);

    p.at<double>(0,0) = px;
    p.at<double>(1,0) = py;
    p.at<double>(2,0) = 1;

    cv::Mat line = fM*p;

    double a = line.at<double>(0,0);
    double b = line.at<double>(1,0);
    double c = line.at<double>(2,0);
    qDebug() << "I" << a << b << c;

    float x0 = 0;
    float y0 = (0-c)/b;
    float x1 = (((-1)*height*b)-c)/a;
    float y1 = height;

    //check x,y within bounds
    if(y0<0 || y0>600){
        ++x0;
        while((y0 < 0 || y0 > 600) && x0 < 800){
            y0 = ((a*(-1)*x0)-c)/b;
            ++x0;
        }
        --x0;
    }
    if(x1<0 || x1>800){
        --y1;
        while((x1 < 0 || x1 > 800) && y1 > 0){
            x1 = ((b*(-1)*y1)-c)/a;
            --y1;
        }
        ++y1;
    }

    QPointF basepoint = QPointF(x0, y0);
    QPointF endpoint = QPointF(x1, y1);

    //while loop to follow the line starting at x0, y0, CHECK FOR PADDING BY MAKING SURE 3<x0<797 and 3<x0<597
    float xp = x0;
    float yp = y0;
    //put xp, yp between new bounds
    if(yp<3 || yp>597){
        ++xp;
        while((yp < 3 || yp > 597) && xp < 797){
            yp = ((a*(-1)*xp)-c)/b;
            ++xp;
        }
        --xp;
    }
    //now map the ZNCC scores by stepping along the line
    float d = sqrt(pow((x1-x0),2)+pow((y1-y0), 2));
    float cx = x0-x1;
    float d2 = d/1000;
    int step = 1;
    float x_max = xp;
    float y_max = yp;
    float score = 0;
    float ZNCC_max = 0;
    QPoint t;
    do{
        t = QPoint(xp, yp);
        qDebug() << "t" << t;
        score = znccScore(pixel, t, &this->ui->graphicsView_image_right->pic, &this->ui->graphicsView_image_left->pic);
        if(score > ZNCC_max){
            ZNCC_max = score;
            x_max = xp;
            y_max = yp;
            qDebug() << "ZNCC: " << ZNCC_max;
        }
        //set xp and yp to point 1 step down the line
        xp = x0 - (((step++)*d2*cx)/d);
        yp = ((a*(-1)*xp)-c)/b;
        if(xp > 796 || xp < 4 || yp > 596 || yp < 4 || step>996) break;
    }while((xp >3 && xp <797 && yp > 3 && yp <597));

    scene_left->addEllipse(x_max, y_max, 2, 2, pen, QBrush(Qt::red));
}

void MainWindow::matchPixelModeRight(QPoint pixel){
    QPen pen;
    pen.setWidth(3);
    pen.setColor(QColor(150,238,50));

    float height = this->ui->graphicsView_image_right->sceneRect().height();

    float px = pixel.x();
    float py = pixel.y();

    qDebug() << "Left pixel selected: (" << px <<", " << py <<" )";

    cv::Mat fM = this->ui->graphicsView_image_left->getcvFundamentalMatrix();
    cv::Mat p(3,1,CV_64FC1);

    p.at<double>(0,0) = px;
    p.at<double>(1,0) = py;
    p.at<double>(2,0) = 1;

    cv::Mat line = fM*p;

    double a = line.at<double>(0,0);
    double b = line.at<double>(1,0);
    double c = line.at<double>(2,0);
    qDebug() << "I" << a << b << c;

    float x0 = 0;
    float y0 = (0-c)/b;
    float x1 = (((-1)*height*b)-c)/a;
    float y1 = height;

    //check x,y within bounds
    if(y0<0 || y0>600){
        ++x0;
        while((y0 < 0 || y0 > 600) && x0 < 800){
            y0 = ((a*(-1)*x0)-c)/b;
            ++x0;
        }
        --x0;
    }
    if(x1<0 || x1>800){
        --y1;
        while((x1 < 0 || x1 > 800) && y1 > 0){
            x1 = ((b*(-1)*y1)-c)/a;
            --y1;
        }
        ++y1;
    }

    //while loop to follow the line starting at x0, y0, CHECK FOR PADDING BY MAKING SURE 3<x0<797 and 3<x0<597
    float xp = x0;
    float yp = y0;
    //put xp, yp between new bounds
    if(yp<3 || yp>597){
        ++xp;
        while((yp < 3 || yp > 597) && xp < 797){
            yp = ((a*(-1)*xp)-c)/b;
            ++xp;
        }
        --xp;
    }
    //now map the ZNCC scores by stepping along the line
    float d = sqrt(pow((x1-x0),2)+pow((y1-y0), 2));
    float cx = x0-x1;
    float d2 = d/1000;
    int step = 1;
    float x_max = xp;
    float y_max = yp;
    float score = 0;
    float ZNCC_max = 0;
    QPoint t;
    do{
        t = QPoint(xp, yp);
        qDebug() << "t: " << t;
        score = znccScore(pixel, t, &this->ui->graphicsView_image_left->pic, &this->ui->graphicsView_image_right->pic);
        if(score > ZNCC_max){
            ZNCC_max = score;
            x_max = xp;
            y_max = yp;
            qDebug() << "ZNCC: " << ZNCC_max;
        }
        //set xp and yp to point 1 step down the line
        xp = x0 - (((step++)*d2*cx)/d);
        yp = ((a*(-1)*xp)-c)/b;
        if (xp > 796 || xp < 4 || yp > 596 || yp < 4 || step>996) break;
    }while((xp >3 && xp <=796 && yp > 3 && yp <=596));

    scene_right->addEllipse(x_max, y_max, 2, 2, pen, QBrush(Qt::red));
}

float znccScore(QPoint px_src, QPoint px_dest, QImage *pic_src, QImage *pic_dest){ //should really be moved to pixelchooser class
    //give score of any two pixels
    //7x7 window only
    //get stdDev of src
    //get avg of source

    //check px_src and px_dest have enough padding
    if     (px_src.x()<3 || px_src.x()>= 797 ||
            px_src.y()<3 || px_src.y()>= 597 ||
            px_dest.x()<3 || px_dest.x()>= 797 ||
            px_dest.y()<3 || px_dest.y()>= 597){
        qDebug() << "[!!] Warning: Not enough padding";
        return -1;
    }

    float sd_src = getSD(px_src, pic_src);
    float sd_dest = getSD(px_dest, pic_dest);
    float avg_src = getAverage(px_src, pic_src);
    float avg_dest = getAverage(px_dest, pic_dest);

    float score = 0;
    for(int i = -3; i<=3 ; i++){
        for(int j = -3; j<=3; j++){
            score += (pic_src->pixelIndex(i+px_src.x(), j+px_src.y())-avg_src) * (pic_dest->pixelIndex(i+px_dest.x(), j+px_dest.x())-avg_dest);
        }
    }
    return score/(pow((2*7+1),2)*sd_src*sd_dest);
}

float getAverage(QPoint px, QImage *img){ //7x7 window only
    float avg = 0;
    for(int i = px.x()-3; i<=px.x()+3 ; i++){
        for(int j = px.y()-3; j<=px.y()+3; j++){
            avg += img->pixelIndex(i,j);
        }
    }
    return avg / (pow(7,2));
}

float getSD(QPoint px, QImage *img){ //7x7 window only
    float sd = 0;
    float avg = getAverage(px, img);
    for(int i = px.x()-3; i<=px.x()+3 ; i++){
        for(int j = px.y()-3; j<=px.y()+3; j++){
            sd += pow((img->pixelIndex(i,j)-avg),2);
        }
    }
    return sqrt(sd)/7;
}




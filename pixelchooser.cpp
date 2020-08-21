#include "pixelchooser.h"
#include "vispro.h"
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>


pixelChooser::pixelChooser(QWidget *parent) : QGraphicsView(parent),
    _Size(0)
{
    this->setInteractive(false);
}

void pixelChooser::initVecX()
{
    this->_VecX = ud_vector(1, this->getSize());
}

void pixelChooser::initVecY()
{
    this->_VecY = ud_vector(1, this->getSize());
}

void pixelChooser::initSVDMatrix()
{
    this->_svdMatrix = ud_matrix(1, this->getSize(), 1, 9);
}

void pixelChooser::initFundamentalMatrix()
{
    this->_fundamentMatrix = ud_matrix(1,3,1,3);
}

void pixelChooser::fillSVD(pixelChooser *a, pixelChooser *b)
{


    if(a->getSize() != b->getSize()){
        QMessageBox::warning(this, "Error", "Number of pixel elements does not match.");
        return;
    }
    int rows = a->getSize();
    float ua, ub, va, vb;
    a->setSVDMatrix(ud_matrix(1, rows, 1, 9));
    float **svd = a->getSVDMatrix();
    for(int i=1; i<=rows; i++){
        ua = a->getArrayX(i-1);
        ub = b->getArrayX(i-1);
        va = a->getArrayY(i-1);
        vb = b->getArrayY(i-1);
        svd[i][1] = ua * ub;
        svd[i][2] = ua * vb;
        svd[i][3] = ua;
        svd[i][4] = va * ub;
        svd[i][5] = va * vb;
        svd[i][6] = va;
        svd[i][7] = ub;
        svd[i][8] = vb;
        svd[i][9] = 1;
    }
    b->setSVDMatrix(svd);
    qDebug() << "svdMatrix filled";
    printMatrix(b->_svdMatrix, rows, 9);
}

void pixelChooser::SVD(pixelChooser *a, pixelChooser *b)
{
    int m = a->getSize();
    int n = 9;
    a->_eigenMatrix = ud_matrix(1,9,1,9);
    b->_eigenMatrix = a->_eigenMatrix;
    float **eigenMatrix = a->_eigenMatrix;

    a->_eigenValues = ud_vector(1,9);
    b->_eigenValues = a->_eigenValues;
    float *eigenValues = a->_eigenValues;

    float **svdMatrix = a->getSVDMatrix();
    svdRoutine(svdMatrix, m, n, eigenValues, eigenMatrix);
    qDebug() << "SVD Complete";

    a->_minEigenvector = getEigenVecMin(eigenMatrix, eigenValues);
    b->_minEigenvector = a->_minEigenvector;
    qDebug() << "minEigenvector Found";
    printVector(b->_minEigenvector, 9);

    a->_fundamentMatrix = eigenToCal(a->_minEigenvector);

    vector<cv::Point2f> lpoints(10);
    vector<cv::Point2f> rpoints(10);
    lpoints[0] = cv::Point2f(287, 220);
    lpoints[1] = cv::Point2f(520, 186);
    lpoints[2] = cv::Point2f(699, 140);
    lpoints[3] = cv::Point2f(435, 348);
    lpoints[4] = cv::Point2f(558, 329);
    lpoints[5] = cv::Point2f(635, 298);
    lpoints[6] = cv::Point2f(354, 539);
    lpoints[7] = cv::Point2f(596, 449);
    lpoints[8] = cv::Point2f(721, 389);
    lpoints[9] = cv::Point2f(821, 394);

    rpoints[0] = cv::Point2f(286, 209);
    rpoints[1] = cv::Point2f(486, 232);
    rpoints[2] = cv::Point2f(695, 234);
    rpoints[3] = cv::Point2f(321, 345);
    rpoints[4] = cv::Point2f(428, 365);
    rpoints[5] = cv::Point2f(534, 371);
    rpoints[6] = cv::Point2f(171, 465);
    rpoints[7] = cv::Point2f(383, 480);
    rpoints[8] = cv::Point2f(537, 481);
    rpoints[9] = cv::Point2f(639, 530);

    auto fundamental_matrix = cv::findFundamentalMat(lpoints, rpoints);
    qDebug() << "HERE IT GOES!! <3";
    for(int i=0; i< fundamental_matrix.rows; i++){
        const double* fM = fundamental_matrix.ptr<double>(i);
        for(int j=0; j<fundamental_matrix.cols; j++){
            qDebug() << " " << fM[j] << " ";
        }
    }

    b->_fundamentMatrix = a->_fundamentMatrix;
    qDebug() << "Fundamental Matrix completed";
    printMatrix(b->_fundamentMatrix, 3, 3);
}

void pixelChooser::setLine(QPoint pixel)
{
    getEpiline(getLine(), getFundamentalMatrix(), pixel);
}

void pixelChooser::mousePressEvent(QMouseEvent *event)
{
    if(this->isInteractive()){
        this->addCoordinate(event->x(), event->y());
        //this->printCoordinates();
        emit sendPixel(event->pos());
    }
}





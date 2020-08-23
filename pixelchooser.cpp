#include "pixelchooser.h"
#include "vispro.h"
#include <QMessageBox>
#include <QPaintEvent>
#include <QPainter>
#include <QGraphicsScene>

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
{   //deprecated
    this->_fundamentMatrix = ud_matrix(1,3,1,3);
}

void pixelChooser::fillFundamentalMatrix(pixelChooser *b){
    pixelChooser *a = this;
    int size = a->getSize();
    if(b->getSize()<size) size=b->getSize(); //really this should result in an error, refactor later
    //after points have been selected
    //create two vectors
    vector<cv::Point2i> Apoints(size);
    vector<cv::Point2i> Bpoints(size);
    for(int i=0; i<size; i++){
        Apoints[i] = cv::Point2i(a->getArrayX(i), a->getArrayY(i));
        Bpoints[i] = cv::Point2i(b->getArrayX(i), b->getArrayY(i));
    }
    this->_cvFundamentalMatrix = cv::findFundamentalMat(Apoints, Bpoints, cv::FM_LMEDS);

    for(int i=0; i< this->_cvFundamentalMatrix.rows; i++){ //test print
            const double* fM = this->_cvFundamentalMatrix.ptr<double>(i);
            QString out = QString("");
            for(int j=0; j<this->_cvFundamentalMatrix.cols; j++){
                out.append(QString(QString::number(fM[j])).append(" "));
            }
            qDebug() << "[ " << out << " ]";
        }
}

void pixelChooser::fillSVD(pixelChooser *a, pixelChooser *b)
{ //DEPRECATED WITH svdRoutine()


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
        QPointF scenePoint = this->mapToScene(event->pos());
        this->addCoordinate(scenePoint.x(), scenePoint.y());

        //this->printCoordinates();
        emit sendPixel(event->pos());
    }
}

/*
void pixelChooser::mousePressEvent(QGraphicsSceneMouseEvent *event){
    if(this->isInteractive()){
        QPointF scenePoint = event->scenePos();
        //this->addCoordinate(scenePoint.x(), scenePoint.y());
        qDebug() << "ScenePoint: " << scenePoint;
        //this->printCoordinates();
        emit sendPixel(QPoint(scenePoint.x(), scenePoint.y()));
    }
}
*/







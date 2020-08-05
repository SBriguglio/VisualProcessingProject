#include "pixelchooser.h"
#include "vispro.h"
#include <QMessageBox>

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

}

void pixelChooser::mousePressEvent(QMouseEvent *event)
{
    if(this->isInteractive()){
        this->addCoordinate(event->x(), event->y());
        //this->printCoordinates();
        emit sendPixel(event->pos());
    }
}


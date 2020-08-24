#ifndef PIXELCHOOSER_H
#define PIXELCHOOSER_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QPen>
#include <QColor>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>


class pixelChooser : public QGraphicsView
{
    Q_OBJECT
public:
    explicit pixelChooser(QWidget *parent = nullptr);

    void setSize(int);
    void incrementSize() {++_Size;}
    int getSize() const {return _Size;}

    void initVecX();
    float *getVecX() const {return _VecX;}

    void initVecY();
    float *getVecY() const {return _VecY;}

    void insertArrayX(int i, float n) {_ArrayX[i] = n;}
    float getArrayX(int i) const {return _ArrayX[i];}

    void insertArrayY(int i, float n) {_ArrayY[i] = n;}
    float getArrayY(int i) const {return _ArrayY[i];}

    void addCoordinate(float x, float y) {if(_Size<100){insertArrayX(_Size, x); insertArrayY(_Size, y); incrementSize();}}
    QPoint getCoordinate(int i) {if(i<100){return QPoint(_ArrayX[i], _ArrayY[i]);}return QPoint(-1000,-1000);}

    void initSVDMatrix();
    void setSVDMatrix(float **m) {_svdMatrix = m;}
    float** getSVDMatrix() const {return _svdMatrix;}

    void initFundamentalMatrix();
    void setFundamentalMatrix(float **m) {_fundamentMatrix = m;}
    void fillFundamentalMatrix(pixelChooser *b);
    cv::Mat getcvFundamentalMatrix() const {return _cvFundamentalMatrix;}
    float** getFundamentalMatrix() const {return _fundamentMatrix;}

    void printCoordinates(){for(int i=0; i<_Size; i++){qDebug() << _ArrayX[i] << ", " << _ArrayY[i] << "\n";} qDebug() << "mSize = " << _Size << "\n";}

    void fillSVD(pixelChooser *a, pixelChooser *b);

    void SVD(pixelChooser *a, pixelChooser *b);

    void setLine(QPoint pixel);
    float *getLine() const {return _line;}

    void setAddbit(bool b){addbit=b;}
    bool getAddbit() const {return addbit;}

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void sendPixel(QPoint pixel);

public:
    QPixmap picture;
    QImage pic;

private:
    QGraphicsScene *scene;
    float _ArrayX[100];
    float _ArrayY[100];
    float *_VecX;
    float *_VecY;
    float **_svdMatrix;
    float **_fundamentMatrix;
    float **_eigenMatrix;
    float *_eigenValues;
    float *_minEigenvector;
    int _Size;
    float *_line;
    QPen _pen;
    cv::Mat _cvFundamentalMatrix;
    bool addbit = true;
};

#endif // PIXELCHOOSER_H

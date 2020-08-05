#ifndef PIXELCHOOSER_H
#define PIXELCHOOSER_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>

class pixelChooser : public QGraphicsView
{
    Q_OBJECT
public:
    explicit pixelChooser(QWidget *parent = nullptr);

    void setSize(int);
    void incrementSize() {++mSize;}
    int getSize() const {return mSize;}

    void initVecX(int, int);
    float *getVecX() const {return mVecX;}

    void initVecY(int, int);
    float *getVecY() const {return mVecY;}

    void insertArrayX(int i, float n) {mArrayX[i] = n;}
    float getArrayX(int i) const {return mArrayX[i];}

    void insertArrayY(int i, float n) {mArrayY[i] = n;}
    float getArrayY(int i) const {return mArrayY[i];}

    void addCoordinate(float x, float y) {if(mSize<100){insertArrayX(mSize, x); insertArrayY(mSize, y); incrementSize();}}
    QPoint getCoordinate(int i) {if(i<100){return QPoint(mArrayX[i], mArrayY[i]);}return QPoint(-1000,-1000);}

    void printCoordinates(){for(int i=0; i<mSize; i++){qDebug() << mArrayX[i] << ", " << mArrayY[i] << "\n";} qDebug() << "mSize = " << mSize << "\n";}

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void sendPixel(QPoint pixel);

private:
    float mArrayX[100];
    float mArrayY[100];
    float *mVecX;
    float *mVecY;
    int mSize;
};

#endif // PIXELCHOOSER_H

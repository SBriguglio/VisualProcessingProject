#include "pixelchooser.h"
#include "vispro.h"

pixelChooser::pixelChooser(QWidget *parent) : QGraphicsView(parent),
    mSize(0)
{
    this->setInteractive(false);
}

void pixelChooser::mousePressEvent(QMouseEvent *event)
{
    if(this->isInteractive()){
        this->addCoordinate(event->x(), event->y());
        //this->printCoordinates();
        emit sendPixel(event->pos());
    }
}


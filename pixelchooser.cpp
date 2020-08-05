#include "pixelchooser.h"

pixelChooser::pixelChooser(QWidget *parent) : QGraphicsView(parent)
{

}

void pixelChooser::mousePressEvent(QMouseEvent *event)
{
    emit sendPixel(event->pos());
}


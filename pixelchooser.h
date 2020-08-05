#ifndef PIXELCHOOSER_H
#define PIXELCHOOSER_H

#include <QWidget>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QGraphicsSceneMouseEvent>

class pixelChooser : public QGraphicsView
{
    Q_OBJECT
public:
    explicit pixelChooser(QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;

signals:
    void sendPixel(QPoint pixel);

private:

};

#endif // PIXELCHOOSER_H

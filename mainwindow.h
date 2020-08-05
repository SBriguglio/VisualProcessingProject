#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QGraphicsScene>
#include <QString>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <QImage>
#include <QPixmap>
#include <QImageReader>
#include <QImageWriter>
#include <QPicture>
#include <QGraphicsView>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();   

private slots:
    void on_pushButton_OpenLeft_clicked();

    void on_pushButton_OpenRight_clicked();

    void on_pushButton_clicked();

    void on_pushButton_stop_clicked();

    void receivePixelLeft(QPoint pixel);

    void receivePixelRight(QPoint pixel);

protected:
    /*TODO:
     *  make two arrays/lists/vectors to store QPoints or X,Y coordinates
     *  change slots receiving points to add points to the above arrays
     *  implement the same algorithm to find the fundamental matrix, should be able to use SVD Routine here
     *  find eigenvectors corresponding to the minimum eigenvalues
     *  reshape the eigenvector to be a fundamental matrix (3x3 i believe but double check)
     *  see whiteboard for furthersteps
     *  draw epipolar lines with method in Anotherone project above
     */
private:
    Ui::MainWindow *ui;
    QString file_path_left;
    QString file_path_right;
    QGraphicsScene *scene_left;
    QGraphicsScene *scene_right;


};
#endif // MAINWINDOW_H

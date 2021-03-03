#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QImage>
#include <QPainter>
#include <QFileDialog>
#include <QInputDialog>
#include <QMenuBar>
#include <QWheelEvent>
#include <QKeyEvent>

using std::min;
using std::max;
using Qt::Key_Left;
using Qt::Key_Right;
using Qt::Key_Up;
using Qt::Key_Down;

const QSize qs(800, 800);
const double four_pi = 4 * 3.141592;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *event);
    void wheelEvent(QWheelEvent *event);
    void keyPressEvent(QKeyEvent *event);

public slots:
    void doClick(QString btnPress);

private:
    Ui::MainWindow *ui;
    QMenuBar *mb;
    QImage og;
    double rotation;
    QRgb avg;
    int dy, dx;
};
#endif // MAINWINDOW_H

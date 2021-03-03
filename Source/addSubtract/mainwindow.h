#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPaintEvent>
#include <QImage>
#include <QPainter>
#include <QFileDialog>
#include <QMenuBar>
#include <QScrollArea>
#include <QBoxLayout>
#include <QLabel>

using std::max;
using std::min;

const QSize qs(800, 800);

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void doClick();

private:
    Ui::MainWindow *ui;
    QMenuBar *mb;
    QScrollArea *qsa;
    QLabel *v;
};
#endif // MAINWINDOW_H

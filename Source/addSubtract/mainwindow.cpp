#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qsa = new QScrollArea(this);
    QVBoxLayout *layout = new QVBoxLayout(qsa);
    qsa->setWidgetResizable(true);
    v = new QLabel(qsa);
    v->setLayout(layout);
    setCentralWidget(qsa);
    statusBar()->hide();
    mb = new QMenuBar(this);
    QMenu *menu = mb->addMenu("File");
    QAction *action = menu->addAction("Load Image");
    connect(action, &QAction::triggered, this, [=]() { this->doClick(); });
    qsa->setWidget(v);
    resize(qs);
    mb->setGeometry(0, 0, qs.width(), 25);
}

MainWindow::~MainWindow() {
    hide();
    delete v;
    delete qsa;
    delete mb;
    delete ui;
}

void MainWindow::doClick() {
    // Prompt user for images, check if they are valid, draw to screen.
    QString fileName = QFileDialog::getOpenFileName(this, tr("Select First Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
    if (fileName == "")
        return;
    QImage a = QImage(fileName);
    fileName = QFileDialog::getOpenFileName(this, tr("Select Second Image"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
    if (fileName == "")
        return;
    QImage b = QImage(fileName);
    int w = max(a.width(), b.width()), h = max(a.height(), b.height());
    QImage qi = QImage(2 * w, 3 * h, QImage::Format_ARGB32_Premultiplied);
    qi.fill(0xFF000000);
    QPainter qp;
    qp.begin(&qi);
    qp.drawImage(0, 0, a);
    qp.drawImage(w, 0, b);
    qp.drawImage(0, h, a);
    qp.drawImage(w, h, a);
    qp.drawImage(0, 2 * h, b);
    qp.drawImage(w, 2 * h, b);
    qp.end();
    // In order, given images a and b, calculate a+b, a-b, b+a, b-a.
    for (int i = 0; i < min(a.width(), b.width()); ++i)
        for (int j = 0; j < min(a.height(), b.height()); ++j) {
            QColor qca = a.pixelColor(i, j), qcb = b.pixelColor(i, j);
            int red = min(255, qca.red() + qcb.red());
            int green = min(255, qca.green() + qcb.green());
            int blue = min(255, qca.blue() + qcb.blue());
            qi.setPixelColor(i, j + h, QColor(red, green, blue));

            red = max(0, qca.red() - qcb.red());
            green = max(0, qca.green() - qcb.green());
            blue = max(0, qca.blue() - qcb.blue());
            qi.setPixelColor(i + w, j + h, QColor(red, green, blue));

            red = min(255, qcb.red() + qca.red());
            green = min(255, qcb.green() + qca.green());
            blue = min(255, qcb.blue() + qca.blue());
            qi.setPixelColor(i, j + 2 * h, QColor(red, green, blue));

            red = max(0, qcb.red() - qca.red());
            green = max(0, qcb.green() - qca.green());
            blue = max(0, qcb.blue() - qca.blue());
            qi.setPixelColor(i + w, j + 2 * h, QColor(red, green, blue));
        }
    // Draw to screen.
    v->setPixmap(QPixmap::fromImage(qi));
    mb->setGeometry(0, 0, qi.width(), 25);
}

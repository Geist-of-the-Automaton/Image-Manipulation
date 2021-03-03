#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Setup app components.
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
    // Open file and check for validity.
    QString fileName = QFileDialog::getOpenFileName(this, tr("Import"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
    if (fileName == "")
        return;
    QImage og = QImage(fileName);
    QImage boarder = QImage(og.width(), og.height(), QImage::Format_ARGB32_Premultiplied);
    QImage skeleton = QImage(og.width(), og.height(), QImage::Format_ARGB32_Premultiplied);
    QImage raised = QImage(og.width(), og.height(), QImage::Format_ARGB32_Premultiplied);
    boarder.fill(0xFF000000);
    skeleton.fill(0xFF000000);
    raised.fill(0xFF000000);
    // Create a dynamic matrix to process the distance transformation.
    int **checkMap = new int *[og.width()];
    for (int i = 0; i < og.width(); ++i)
        checkMap[i] = new int [og.height()];
    // Grassfire transform from top left. Check for boarder.
    for (int i = 0; i < og.width(); ++i)
        for (int j = 0; j < og.height(); ++j) {
            if (og.pixel(i, j) == 0xFF000000)
                checkMap[i][j] = 0;
            else {
                int n = j == 0 ? 0 : checkMap[i][j - 1];
                int w = i == 0 ? 0 : checkMap[i - 1][j];
                checkMap[i][j] = min(n, w) + 1;
                if (checkMap[i][j] == 1)
                    boarder.setPixel(i, j, 0xFFFFFFFF);
            }
        }
    // Grassfire transform from bottom right. Check for boarder and max distance value to scale the visual output of the distance transform.
    double max = 1.0;
    for (int i = og.width() - 1; i >= 0; --i)
        for (int j = og.height() - 1; j >= 0; --j) {
            if (og.pixel(i, j) == 0xFF000000)
                checkMap[i][j] = 0;
            else {
                int s = j == og.height() - 1 ? 0 : checkMap[i][j + 1];
                int e = i == og.width() - 1 ? 0 : checkMap[i + 1][j];
                int val = min(s, e) + 1;
                checkMap[i][j] = min(checkMap[i][j], val);
                if (checkMap[i][j] > max)
                    max = static_cast<int>(checkMap[i][j]);
                if (checkMap[i][j] == 1)
                    boarder.setPixel(i, j, 0xFFFFFFFF);
            }
        }
    // Attempt to find the skeleton from the distance transform's ridgeline.
    for (int i = 1; i < og.width() - 1; ++i)
        for (int j = 1; j < og.height() - 1; ++j)
            if (og.pixel(i, j) == 0xFFFFFFFF) {
                int gcnt = 0, ecnt = 0;
                for (int m = i - 1; m <= i + 1; ++m)
                    for (int n = j - 1; n <= j + 1; ++n) {
                        if (checkMap[m][n] > checkMap[i][j])
                            ++gcnt;
                        if (checkMap[m][n] == checkMap[i][j] && (i != m || j != n))
                            ++ecnt;
                    }
                if (gcnt == 1 || (gcnt == 0 && ecnt > 0))
                    skeleton.setPixel(i, j, 0xFFFFFFFF);
            }
    // Scale the visual output of the distance transform.
    for (int i = 0; i < raised.width(); ++i)
        for (int j = 0; j < raised.height(); ++j) {
            int val = static_cast<int>(static_cast<double>(255 * checkMap[i][j]) / max);
            QColor qc(val, val, val, 255);
            raised.setPixel(i, j, qc.rgba());
        }
    // Clean up.
    for (int i = 0; i < og.width(); ++i)
        delete [] checkMap[i];
    delete [] checkMap;
    // Draw to screen.
    QImage qi = QImage(og.width() * 2, og.height() * 2, QImage::Format_ARGB32_Premultiplied);
    qi.fill(0xFF000000);
    QPainter qp;
    qp.begin(&qi);
    qp.drawImage(0, 0, og);
    qp.drawImage(og.width(), 0, raised);
    qp.drawImage(0, og.height(), boarder);
    qp.drawImage(og.width(), og.height(), skeleton);
    qp.end();
    v->setPixmap(QPixmap::fromImage(qi));
    mb->setGeometry(0, 0, qi.width(), 25);
}

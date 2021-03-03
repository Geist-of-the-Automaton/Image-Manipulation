#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    // Setup app components.
    ui->setupUi(this);
    statusBar()->hide();
    mb = new QMenuBar(this);
    QMenu *menu = mb->addMenu("File");
    QAction *action = menu->addAction("Load Image");
    connect(action, &QAction::triggered, this, [=]() { this->doClick(action->text()); });
    action = menu->addAction("Set Rotation");
    connect(action, &QAction::triggered, this, [=]() { this->doClick(action->text()); });
    action = menu->addAction("Set X Offset");
    connect(action, &QAction::triggered, this, [=]() { this->doClick(action->text()); });
    action = menu->addAction("Set Y Offset");
    connect(action, &QAction::triggered, this, [=]() { this->doClick(action->text()); });
    mb->setGeometry(0, 0, qs.width(), 25);
    og = QImage(qs, QImage::Format_ARGB32_Premultiplied);
    rotation = 0.0;
    avg = 0xFFFFFFFF;
    dy = 0;
    dx = 0;
}

MainWindow::~MainWindow() {
    hide();
    delete mb;
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter qp(this);
    // Calculate the location of the corners.
    QPoint pts[] = {QPoint(0,0), QPoint(og.width() - 1, 0), QPoint(0, og.height() - 1), QPoint(og.width() - 1, og.height() - 1)};
    int minX = INT_MAX, minY = INT_MAX, maxX = 0, maxY = 0;
    int midX1 = og.width() / 2, midY1 = og.height() / 2;
    for (int i = 0; i < 4; ++i) {
        pts->setX(cos(rotation) * (pts[i].x() - midX1) + sin(rotation) * (pts[i].y() - midY1));
        pts->setY(-sin(rotation) * (pts[i].x() - midX1) + cos(rotation) * (pts[i].y() - midY1));
        minX = min(pts[i].x(), minX);
        minY = min(pts[i].y(), minY);
        maxX = max(pts[i].x(), maxX);
        maxY = max(pts[i].y(), maxY);
    }
    // Create a bounding image and fill the background with the average color.
    QImage qi(dx + abs(og.width() * cos(rotation)) + abs(og.height() * sin(rotation)), dy + abs(og.width() * sin(rotation)) + abs(og.height() * cos(rotation)), QImage::Format_ARGB32_Premultiplied);
    qi.fill(avg);
    // Resize the window if needed.
    if (size() != QSize(qi.width(), qi.height() + 25)) {
        resize(qi.width(), qi.height() + 25);
        mb->setGeometry(0, 0, qi.width(), 25);
    }
    // Calculate the pixels of the new image back to the original one.
    int midX2 = (qi.width() - (dx + 1)) / 2, midY2 = (qi.height() - (dy + 1)) / 2;
    for (int i = -midX2; i <= midX2; ++i)
        for (int j = -midY2; j <= midY2; ++j) {
            QPoint pt;
            pt.setX(cos(-rotation) * i + sin(-rotation) * j);
            pt.setY(-sin(-rotation) * i + cos(-rotation) * j);
            if (i + midX2 >= 0 && i + midX2 < qi.width() && j + midY2 >= 0 && j + midY2 < qi.height() && pt.x() + midX1 >= 0 && pt.x() + midX1 < og.width() && pt.y() + midY1 >= 0 && pt.y() + midY1 < og.height())
                qi.setPixel(dx + i + midX2, dy + j + midY2, og.pixel(pt.x() + midX1, pt.y() + midY1));
        }
    // Draw to screen.
    qp.drawImage(0, 0, og);
    qp.drawImage(0, 25, qi);
}

void MainWindow::doClick(QString btnPress) {
    if (btnPress == "Load Image") {
        // Open image and get average color.
        QString fileName = QFileDialog::getOpenFileName(this, tr("Import"), "/", tr("Image Files (*.png *.jpg *.bmp)"));
        if (fileName == "")
            return;
        og = QImage(fileName);
        unsigned long ar = 0, ag = 0, ab = 0;
        for (int i = 0; i < og.width(); ++i)
            for (int j = 0; j < og.height(); ++j) {
                QColor qc = og.pixelColor(i, j);
                ar += qc.red();
                ag += qc.green();
                ab += qc.blue();
            }
        unsigned long pixels = og.width() * og.height();
        ar /= pixels;
        ag /= pixels;
        ab /= pixels;
        avg = QColor(ar, ag, ab).rgba();
        rotation = 0.0;
        repaint();
    }
    else if (btnPress == "Set Rotation") {
        // Prompt user for rotation input and check if the input window gave a proper response.
        bool ok = false;
        double ret = QInputDialog::getDouble(this, "Rotation", "Enter a rotation", rotation, -four_pi, four_pi, 5, &ok );
        if (ok && ret != rotation) {
            rotation = ret;
            repaint();
        }
    }
    else if (btnPress == "Set X Offset") {
        // Prompt user for x offset input and check if the input window gave a proper response.
        bool ok = false;
        int ret = QInputDialog::getInt(this, "X Offset", "Enter a X offset", dx, 0, INT_MAX, 1, &ok );
        if (ok && ret != dx) {
            dx = ret;
            repaint();
        }
    }
    else if (btnPress == "Set Y Offset") {
        // Prompt user for y offset input and check if the input window gave a proper response.
        bool ok = false;
        int ret = QInputDialog::getInt(this, "Y Offset", "Enter a Y offset", dy, 0, INT_MAX, 1, &ok );
        if (ok && ret != dy) {
            dy = ret;
            repaint();
        }
    }
}

void MainWindow::wheelEvent(QWheelEvent *event) {
    // Rotate the image by 1/100th of a radian in the direction of the wheel spin.
    int dy = event->angleDelta().y();
    rotation += dy > 0 ? 0.01 : -0.01;
    if (rotation == four_pi / 2 || rotation == -four_pi / 2)
        rotation = 0.0;
    repaint();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    // Process the arrow key presses as modifications to the offsets.
    switch (event->key()) {
    case Key_Down:
        if (dy < INT_MAX)
            ++dy;
        break;
    case Key_Up:
        if (dy > 0)
            --dy;
        break;
    case Key_Right:
        if (dx < INT_MAX)
            ++dx;
        break;
    case Key_Left:
        if (dx > 0)
            --dx;
        break;
    default:
        return;
    }
    repaint();
}

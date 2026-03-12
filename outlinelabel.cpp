#include "outlinelabel.h"
#include <QPainter>
#include <QPainterPath>

OutlineLabel::OutlineLabel(QWidget* parent)
    : QLabel(parent), textColor(Qt::white), outlineColor(Qt::black)
{}

void OutlineLabel::setTextColor(const QColor &color) {
    textColor = color;
    update(); // triggers repaint
}

void OutlineLabel::setOutlineColor(const QColor &color) {
    outlineColor = color;
    update();
}

void OutlineLabel::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QFont font = this->font();
    QString textStr = text();

    QPainterPath path;
    path.addText(0, font.pointSize(), font, textStr);

    // Center text in the label
    QRectF bounds = path.boundingRect();
    qreal x = (width() - bounds.width()) / 2 - bounds.left();
    qreal y = (height() + bounds.height()) / 2 - bounds.bottom();
    QTransform transform;
    transform.translate(x, y);
    path = transform.map(path);

    // Draw outline
    painter.setPen(QPen(outlineColor, 5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    painter.setBrush(Qt::NoBrush);
    painter.drawPath(path);

    // Draw text fill
    painter.setPen(Qt::NoPen);
    painter.setBrush(textColor);
    painter.drawPath(path);
}

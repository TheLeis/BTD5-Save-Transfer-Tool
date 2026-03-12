#ifndef OUTLINELABEL_H
#define OUTLINELABEL_H

#include <QLabel>
#include <QColor>

class OutlineLabel : public QLabel
{
    Q_OBJECT

public:
    explicit OutlineLabel(QWidget* parent = nullptr);

    void setTextColor(const QColor& color);
    void setOutlineColor(const QColor& color);

protected:
    void paintEvent(QPaintEvent* event) override;

private:
    QColor textColor;
    QColor outlineColor;
};

#endif // OUTLINELABEL_H

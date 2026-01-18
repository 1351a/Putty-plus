#ifndef QLISTWIDGETPRO_H
#define QLISTWIDGETPRO_H

#include <QListWidget>
#include <QDebug>

class QListWidgetPro : public QListWidget
{
    Q_OBJECT
public:
    explicit QListWidgetPro(QWidget *parent = nullptr);

signals:
    void requestAllowMouseClick(bool &allow);

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    // Widget *MainWidget;
};

#endif // QLISTWIDGETPRO_H

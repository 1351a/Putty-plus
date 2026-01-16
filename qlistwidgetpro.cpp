#include "qlistwidgetpro.h"
#include <QEvent>
#include <QMouseEvent>

QListWidgetPro::QListWidgetPro(QWidget *parent)
    : QListWidget(parent)
{

    this->viewport()->installEventFilter(this);
}

bool QListWidgetPro::eventFilter(QObject *watched, QEvent *event)
{

    if(event->type() == QEvent::MouseMove||event->type() == QEvent::MouseButtonRelease)
        return true;   //禁止滑动
    if (event->type()==QEvent::MouseButtonPress||event->type()==QEvent::MouseButtonDblClick)
    {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        QListWidgetItem *item = this->itemAt(mouseEvent->pos());
        if (item)
        {
            bool allow=true;
            emit requestAllowMouseClick(allow); //判断当前是否允许选择
            if(allow) return true;
        }
    }

    return QListWidget::eventFilter(watched, event);
}

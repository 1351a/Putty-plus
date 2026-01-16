#include "qspinboxpro.h"

QSpinBoxPro::QSpinBoxPro(QWidget *parent)
    : QSpinBox(parent)
{
    // setRange(0, 99);
    clear();                 // 初始为空
    connect(this,&QSpinBoxPro::valueChanged,this,[=]{
        if(m_isEmpty) clear();
    });

}

bool QSpinBoxPro::isEmpty() const
{
    return m_isEmpty;
}

QValidator::State QSpinBoxPro::validate(QString &text, int &pos) const
{
    // qDebug()<<"QSpinBoxPro pos:"<<pos<<text;
    Q_UNUSED(pos);

    if (text.isEmpty()) {
        m_isEmpty = true;

        return QValidator::Acceptable;
    }
   m_isEmpty = false;
    if (text == "-"&&minimum()<0) {  //
        // m_isEmpty = false;
        return QValidator::Intermediate;
    }

    bool ok = false;
    int v = text.toInt(&ok);

    if (!ok)
        return QValidator::Invalid;

    if (v < minimum() || v > maximum())
        return QValidator::Invalid;

    // m_isEmpty = false;
    return QValidator::Acceptable;
}

int QSpinBoxPro::valueFromText(const QString &text) const
{
    // qDebug()<<"valueFromText"<<text;
    if (text.isEmpty()) {
        m_isEmpty = true;
        return minimum();   // 占位，不会真正用
    }
    m_isEmpty = false;
    return text.toInt();
}

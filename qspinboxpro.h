#ifndef QSPINBOXPRO_H
#define QSPINBOXPRO_H

#include <QSpinBox>

class QSpinBoxPro : public QSpinBox
{
    Q_OBJECT
public:
    explicit QSpinBoxPro(QWidget *parent = nullptr);
    bool isEmpty() const;

signals:
    void showEmpty();  //显示空

protected:
    QValidator::State validate(QString &text, int &pos) const override;
    int valueFromText(const QString &text) const override;

private:
    mutable bool m_isEmpty = true;
};



#endif

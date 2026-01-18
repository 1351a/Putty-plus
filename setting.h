#ifndef SETTING_H
#define SETTING_H

#include <QWidget>
#include <QApplication>

namespace Ui {
class Setting;
}

class Setting : public QWidget
{
    Q_OBJECT

public:
    explicit Setting(QWidget *parent = nullptr);
    ~Setting();
    QString getPuttyPath(); //获取putty程序路径
    QString getLanguage(bool isAbsPath);  //获取语言 如果isAbsPath为真返回带路径文件名 否返回文件名
    void setPuttyPath(const QString &path);    //设置putty.exe路径
    void setLanguage(const QString &language);  //设置语言

signals:
    void saveSetting(); //保存设置信号

private:
    Ui::Setting *ui;
    QString Putty_Path;     //putty.exe路径
    QString Language;   //语言
    const QString Program_Path=QCoreApplication::applicationDirPath();  //程序运行时的路径
    const QString Internal_Putty_Path=Program_Path+"/res/putty_V0.73.exe";    //putty.exe内部路径
    const QString Language_File_Path=Program_Path+"/Language";     //语言文件夹路径
    QStringList Language_List;     //拥有的语言   [0]文件名
    void updateLanguageList();      //更新拥有的语言
    void inputPath();
    void updateEditPuttyPath();  //更新ui->editPuttyPath
};

#endif // SETTING_H

#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include "usermanage.h"
#include "setting.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

private:
    Ui::Widget *ui;
    Setting *setting;
    UserManage *usermanage;
    // QString Putty_Path=QCoreApplication::applicationDirPath()+"/res/putty_V0.73.exe";     //putty.exe路径
    QStringList ConnTypeNameList={
        "SSH",
        "Telnet",
        "Raw",
        "Rlogin",
        "Serial"
    };
    QMap<QString,int> itemRow;
    void updateList();  //更新LoginSite
    void addListItem(QString text,int img_id);  //更新item
    void selectSSH(const QString hostName);   //选择SSH服务器
    void setLoginParmsEdit(bool isEdit);    //真表示允许编辑
    bool isUserDataModified(const QString &hostName);   //是否用户更改输入框 返回真表示更改 返回假表示无更改
    bool isSaveUser();      //用户是否需要保存 返回真表示需要
    void saveUser(const QString &hostName);    //保存用户
    void removeUser(const QString &hostName);    //删除用户
    void loginServer();  //登录服务器
    bool isReselectionAllowed();    //用户选择(是，否，取消)后是否运行选择item 真表示用户没有选择，不可选择item
    void protoFrontend();   //用户选择协议时，前端界面变为对应，如果当前协议不支持保存密码则禁用密码
};
#endif // WIDGET_H

#ifndef USERMANAGE_H
#define USERMANAGE_H

#include <QWidget>
#include <QJsonObject>
#include <QApplication>

struct UData
{
    int connType=0;
    QString userName;
    QString address;
    QString password;
    int port=22;

    UData() = default;
    UData(int connTyper,    //连接类型 0 ssh,1 tenent,2 raw,3 rlogin
          const QString &userNamer,
          const QString &addressr,
          const QString &passwordr,
          int portr = 22)
        : connType(connTyper), userName(userNamer), address(addressr), password(passwordr), port(portr) {}

    bool operator==(const UData &other) const {
        return connType == other.connType &&
               userName == other.userName &&
               address == other.address &&
               password == other.password &&
               port == other.port;
    }
};
class UserManage : public QWidget
{
    Q_OBJECT
public:
    explicit UserManage(QWidget *parent = nullptr);
    // void setUser(const QString &userName,QString address,QString password="",int port=22,QString hostName=""); //设置用户文件 用户名,ip,密码,端口,主机名
    QString setUser(const int connType,
                    const QString &userName,
                    const QString &address,
                    const QString &password = "",
                    int port = 22,
                    const QString &hostName = "",
                    bool cover=true);
    void removeUser(const QString &hostName); //删除用户
    QStringList userList();              //用户列表
    UData userData(const QString &userName);    //获取用户数据
    void saveSetting(const QString &puttyPath); //保存设置
    QString getSetting();   //获取设置
    void saveLanguage(const QString &language);     //保存设置语言
    QString getLanguage();   //获取语言

signals:

private:
    // QVector<UData> User_List;
    QString File_Path=QCoreApplication::applicationDirPath()+"/configuration.json";     //配置文件路径
    QString language;   //当前语言
    QMap<QString,UData> User_List;  //用户列表
    QJsonObject User_Data_Object;   //配置文件用户数据
    QJsonObject Root_Object;    //配置文件根
    void readUserInfo();    //读取用户文件
};

#endif // USERMANAGE_H

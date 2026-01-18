#define Current_Version "1.0"

#include "usermanage.h"
#include <QJsonDocument>
#include <QApplication>
#include <QFile>

UserManage::UserManage(QWidget *parent)
    : QWidget{parent}
{
    readUserInfo();
}
void UserManage::readUserInfo(){
    User_List.clear();
    QFile file(File_Path);
    if(!file.open(QIODevice::ReadOnly)) {
        qCritical() << "文件打开失败：" << file.errorString();
        return;
        // return QStringList();
    }
    // 解析JSON
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if(doc.isNull()){
        qWarning() << "JSON解析失败！";
        return;
        // return QStringList();
    }
    // 获取根对象
    Root_Object = doc.object();
    language=Root_Object["LANGUAGE"].toString();
    User_Data_Object=Root_Object["USERDATA"].toObject();
    for(QString &hostName:User_Data_Object.keys()){
        if(hostName.isEmpty()) continue;

        // qDebug()<<"hostName: "<<hostName;
        QJsonObject userObject;
        int connType;
        QString userName;
        QString address;
        QString password;
        int port;
        userObject=User_Data_Object[hostName].toObject();
        connType=userObject["connType"].toInt();
        userName=userObject["username"].toString();
        address=userObject["address"].toString();
        password=userObject["password"].toString();
        port=userObject["port"].toInt();
        // qDebug()<<"json  "<<hostName;
        User_List[hostName]={
            connType,
            userName,
            address,
            password,
            port
        };
    }
}
QString UserManage::setUser(const int connType,const QString &userName,const QString &address,const QString &password,int port,const QString &hostName,bool cover){
    // if(userName.isEmpty()&&address.isEmpty()&&password.isEmpty()&&hostName.isEmpty()) return QString();
    //重命名************************* ---->简直就是依托史

    QString newHostName(hostName);
    // if(hostName=="新建站点"||hostName=="临时站点"||hostName=="New Session"||hostName=="Temporary Session") newHostName+=" (1)";
    if(hostName.isEmpty()){ //如果用户没有指定会话名
        if(userName.isEmpty()||address.isEmpty()) newHostName="session";    //如果主机名或端口为空 会话命名为session
        else newHostName=userName+"@"+address;
    }

    if(User_Data_Object.contains(newHostName)&&!cover){
        newHostName=[](QString hostName,const QJsonObject &object){
            int hostLengh=hostName.length();
            for(int i=hostLengh-1;i>=0;i--){
                if(hostName[i]==' ') hostName.chop(1);
                else break;
            }
            hostLengh=hostName.length();
            // qDebug()<<"hhh  "<<hostName[hostLengh-1];
            if(object.contains(hostName)&&hostName[hostLengh-1]==')'){
                for(int i=hostLengh-2;i>=0;i--){
                    // qDebug()<<"ccc  "<<hostName[i];
                    if(((hostName[i]>'9'||hostName[i]<'1')&&hostName[i]!='(')||(hostName[i]=='('&&i==hostLengh-2)) break;
                    if(hostName[i]=='('){
                        if(!i){
                            hostName.clear();
                            break;
                        }
                        else{
                            if(hostName[i-1]==' ') hostName.remove(i-1,hostLengh-i+1);
                            break;
                        }
                        // qDebug()<<"re  "<<hostName<<" i: "<<i<<" L: "<<hostLengh-1-i;
                    }
                }
            }
            // qDebug()<<"0: "<<hostName;
            QString fHostName(hostName);
            hostLengh=hostName.length();
            if(hostLengh-1>=0&&hostName[hostLengh-1]!=' ')
                hostName+=" ";
            for(int i=2;object.contains(fHostName)||i==2;i++){
                QString rhostName=hostName+"("+QString().number(i)+")";
                if(!object.contains(rhostName)){
                    hostName=rhostName;
                    break;
                }
            }
            qDebug()<<"1: "<<hostName;
            return hostName;
        }(newHostName,User_Data_Object);
    }

    //***************************

    QJsonObject userObject;
    userObject["connType"]=connType;
    userObject["username"]=userName;
    userObject["address"]=address;
    userObject["password"]=password;
    userObject["port"]=port;
    User_Data_Object[newHostName]=userObject;
    Root_Object["USERDATA"]=User_Data_Object;
    Root_Object["VERSION"]=Current_Version;
    QFile file(File_Path);
    if(file.open(QIODevice::WriteOnly)){
        QJsonDocument doc(Root_Object);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "JSON文件写入成功";
    }
    return newHostName;
}
void UserManage::removeUser(const QString &hostName){
    if(!User_Data_Object.contains(hostName)) return;
    User_Data_Object.remove(hostName);
    Root_Object["USERDATA"]=User_Data_Object;
    Root_Object["VERSION"]=Current_Version;
    QFile file(File_Path);
    if(file.open(QIODevice::WriteOnly)){
        QJsonDocument doc(Root_Object);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "JSON文件删除成功";
    }
}
QStringList UserManage::userList(){
    readUserInfo();
    return User_List.keys();
}
UData UserManage::userData(const QString &userName){
    auto copy_User_List=User_List;
    if(copy_User_List.isEmpty()) return UData();
    return copy_User_List[userName];
}
void UserManage::saveSetting(const QString &puttyPath){
    Root_Object["PUTTY_PAYH"]=puttyPath;
    QFile file(File_Path);
    if(file.open(QIODevice::WriteOnly)){
        QJsonDocument doc(Root_Object);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "JSON文件写入成功";
    }
}
void UserManage::saveLanguage(const QString &language){
    Root_Object["LANGUAGE"]=language;
    QFile file(File_Path);
    if(file.open(QIODevice::WriteOnly)){
        QJsonDocument doc(Root_Object);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
        qDebug() << "JSON文件写入成功";
    }
}
QString UserManage::getSetting(){
    return Root_Object["PUTTY_PAYH"].toString();
}
QString UserManage::getLanguage(){
    return Root_Object["LANGUAGE"].toString();
}

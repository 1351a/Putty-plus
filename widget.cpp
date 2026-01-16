#include "widget.h"
#include "ui_widget.h"
#include <usermanage.h>
#include <QFont>
#include <QMessageBox>
#include <QProcess>
#include <QFile>
#include <QTranslator>
#include "qspinboxpro.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->label_1->setFocus();    //默认焦点

    // qDebug()<<"Putty_Path"<<Putty_Path;
    // QSpinBoxPro *qspinboxpro=new QSpinBoxPro(this);
    // qspinboxpro->setRange(-100,100);
    // QProcess* process = new QProcess(this);
    // QStringList a={
    //     "-ssh",
    //     "-l",
    //     "root",
    //     "-pw",
    //     "dfrobot",
    //     "-P",
    //     "22",
    //     "10.1.2.3"
    // };
    // process->start(QCoreApplication::applicationDirPath()+"/res/putty_V0.73.exe",a);

    // 禁用拖动选择和拖动操作
    // ui->LoginSite->setSelectionMode(QAbstractItemView::SingleSelection);  // 只允许单项选择
    // ui->LoginSite->setDragEnabled(false);  // 禁用拖动操作
    // ui->LoginSite->setDragDropMode(QAbstractItemView::NoDragDrop);  // 禁用拖放


    ui->connType->setEditable(false);
    setLoginParmsEdit(true);
    ui->connType->addItems(ConnTypeNameList);
    usermanage=new UserManage(this);
    setting =new Setting(this);

    //*****************设置语言*****************
    setting->setPuttyPath(usermanage->getSetting());
    setting->setLanguage(usermanage->getLanguage());
    QTranslator *translator=new QTranslator(this);
    translator->load(setting->getLanguage(true));
    qApp->installTranslator(translator);
    ui->retranslateUi(this);
    //*****************************************

    qDebug()<<"PUTTY_PAYH"<<usermanage->getSetting();
    updateList();

    connect(ui->LoginSite,&QListWidget::currentTextChanged,this,&Widget::selectSSH);   //选择
    connect(ui->pushButtonEdit,&QPushButton::clicked,this,[=]{  //编辑
        setLoginParmsEdit(true);
        ui->stackedButton->setCurrentIndex(1);
    });
    connect(ui->pushButtonCancel,&QPushButton::clicked,this,[=]{    //取消
        if(ui->LoginSite->currentItem())
            selectSSH(ui->LoginSite->currentItem()->text());
    });
    connect(ui->pushButtonSave,&QPushButton::clicked,this,[=]{  //保存
        if(ui->LoginSite->currentItem())
            saveUser(ui->LoginSite->currentItem()->text());
    });
    connect(ui->pushButtonRemove,&QPushButton::clicked,this,[=]{    //删除
        if(ui->LoginSite->currentItem())
            removeUser(ui->LoginSite->currentItem()->text());
    });
    //***************设置登录按钮是否启用***********************
    connect(ui->editAddress,&QLineEdit::textChanged,this,[=](const QString &addres){
        if(ui->connType->currentText()!="Serial")
            ui->pushButtonLogIn->setEnabled(!addres.isEmpty()&&!ui->editPort->isEmpty());
    });
    connect(ui->editSerial,&QLineEdit::textChanged,this,[=](const QString &addres){
        if(ui->connType->currentText()=="Serial")
            ui->pushButtonLogIn->setEnabled(!addres.isEmpty()&&!ui->editPort->isEmpty());
    });
    connect(ui->editPort,&QSpinBoxPro::valueChanged,this,[=]{
        if(ui->connType->currentText()=="Serial")
            ui->pushButtonLogIn->setEnabled(!ui->editSerial->text().isEmpty()&&!ui->editPort->isEmpty());
        else
            ui->pushButtonLogIn->setEnabled(!ui->editAddress->text().isEmpty()&&!ui->editPort->isEmpty());
    });
    //*******************************************************
    ui->pushButtonLogIn->setEnabled(false); //登录
    connect(ui->pushButtonLogIn,&QPushButton::clicked,this,[=]{     //登录
        if(ui->LoginSite->currentItem()){
            // if(ui->editAddress->text().isEmpty()&&ui->connType->currentText()=="") return;
            if(ui->editSerial->text().isEmpty()&&ui->connType->currentText()=="Serial") return;
            if(ui->editAddress->text().isEmpty()&&ui->connType->currentText()!="Serial") return;
            isReselectionAllowed();
            loginServer();
        }
    });
    connect(ui->pushButtonClose,&QPushButton::clicked,qApp,&QApplication::quit);    //关闭
    connect(ui->LoginSite,&QListWidgetPro::requestAllowMouseClick,this,[=](bool &allow){   //判断是否允许选择 假表示允许
        allow=isReselectionAllowed();
    });
    connect(ui->connType,&QComboBox::currentTextChanged,this,&Widget::protoFrontend);   //用户选择协议时，前端界面变为对应
    connect(ui->pushButtonSet,&QPushButton::clicked,this,[=]{
        setting->show();
    });
    connect(setting,&Setting::saveSetting,this,[=]{     //保存设置
        usermanage->saveSetting(setting->getPuttyPath());
        usermanage->saveLanguage(setting->getLanguage(false));
    });
    // ui->LoginSite->setFocusPolicy(Qt::NoFocus);   // 完全不显示焦点框
    // usermanage->setUser(0,"root","127.0.0.1","1234");
    // usermanage->setUser(0,"root","192.168.1.1","Asd34",2145);
    updateList();
    // qDebug()<<usermanage->userList();
    ui->LoginSite->setCurrentRow(0);

    // ui->editPort->setMinimum(-1);
    // ui->editPort->setSpecialValueText(" ");
    // ui->editPort->setValue(-1);
}

Widget::~Widget()
{
    delete ui;
}
void Widget::updateList(){
    ui->LoginSite->clear();
    itemRow={
        {tr("Temporary Session"),0},
        {tr("New Session"),1}
    };
    // itemRow.clear();
    // itemRow["临时站点"]=0;
    // itemRow["新建站点"]=1;
    addListItem(tr("Temporary Session"),3);
    addListItem(tr("New Session"),2);
    int index=1;
    for(QString &userName:usermanage->userList()){
        if(!userName.isEmpty()){
            itemRow[userName]=++index;
            addListItem(userName,1);
        }
    }
}
void Widget::addListItem(QString text,int img_id){
    ui->LoginSite->addItem(text);
    int row=ui->LoginSite->count()-1;
    QSize size(0,30);
    ui->LoginSite->item(row)->setSizeHint(size);
    ui->LoginSite->setIconSize(QSize(25, 25));
    QIcon icon("://res/computer1.png");
    if(img_id==2) icon.addFile("://res/computer2.png");
    if(img_id==3) icon.addFile("://res/computer3.png");
    ui->LoginSite->item(row)->setIcon(icon);
    QFont font;
    font=ui->LoginSite->item(row)->font();
    font.setPointSize(9);
    ui->LoginSite->item(row)->setFont(font);
}
void Widget::selectSSH(const QString hostName){
    ui->label_1->setFocus();    //默认焦点
    if(hostName.isEmpty()) return;
    ui->stackedButton->setCurrentIndex(0);
    int connType;
    QString userName;
    QString address;
    QString password;
    int port;
    int count;
    count=ui->LoginSite->currentRow();  //获取当前选择的站点

    ui->editHostName->setEnabled(true);
    if(count==0||count==1){     //如果是临时站点或新建站点
        // ui->pushButtonLogIn->setEnabled(false);
        ui->editHostName->setText("");
        ui->connType->setCurrentIndex(0);
        ui->editUserName->setText("");
        ui->editPassword->setText("");
        ui->editPort->setValue(22);
        ui->editAddress->setText("");
        ui->editSerial->setText("COM1");
        ui->pushButtonRemove->setVisible(false);
        if(count==0){   //是临时站点
            ui->editHostName->setText(tr("<Temporary Session>"));
            ui->editHostName->setEnabled(false);
            ui->pushButtonEdit->setVisible(false);
            setLoginParmsEdit(true);
        }
        if(count==1){   //是新建站点
            ui->stackedButton->setCurrentIndex(1);
            ui->pushButtonCancel->setVisible(false);
            setLoginParmsEdit(true);
        }
        return;
    }

    setLoginParmsEdit(false);
    ui->pushButtonCancel->setVisible(true);
    ui->pushButtonEdit->setVisible(true);
    ui->pushButtonRemove->setVisible(true);

    connType=usermanage->userData(hostName).connType;
    userName=usermanage->userData(hostName).userName;
    address=usermanage->userData(hostName).address;
    password=usermanage->userData(hostName).password;
    port=usermanage->userData(hostName).port;
    ui->editHostName->setText(hostName);
    ui->connType->setCurrentIndex(connType);
    ui->editUserName->setText(userName);
    ui->editPassword->setText(password);
    if(port!=-1)    //如果端口不为空
        ui->editPort->setValue(port);
    else{   //如果端口为空
        ui->editPort->setValue(0);
        ui->editPort->clear();
    }
    if(connType==4){    //如果是串口协议
        ui->editAddress->clear();
        ui->editSerial->setText(address);
    }
    else{
        ui->editAddress->setText(address);
        ui->editSerial->setText("COM1");
    }
    protoFrontend();    //如果当前协议不支持保存密码则禁用密码
    // ui->pushButtonLogIn->setEnabled(!address.isEmpty());    //设置登录按钮是否启用
}
void Widget::saveUser(const QString &hostName){
    int count;
    count=ui->LoginSite->currentRow();  //获取当前选择的站点

    if(count==0) return;    //如果是临时站点退出
    int connType;
    QString rHostName=ui->editHostName->text();
    QString userName;
    QString address;
    QString password;
    int port;
    connType=ui->connType->currentIndex();
    userName=ui->editUserName->text();
    if(connType==4)     //如果是串口协议
        address=ui->editSerial->text();
    else
        address=ui->editAddress->text();
    password=ui->editPassword->text();
    port=ui->editPort->value();

    if(ui->editPort->isEmpty()) //如果端口是空
        port=-1;
    // if(address.isEmpty()) return;
    bool cover=false;   //是否覆盖
    if(count!=1){   //如果不是新建站点
        if(rHostName!=hostName) usermanage->removeUser(hostName);   //如果站点名改变先删除在新建
        else cover=true;    //如果没有改变则覆盖
        ui->stackedButton->setCurrentIndex(0);
    }
    else
    {
        rHostName=ui->editHostName->text();
        setLoginParmsEdit(false);
    }

    QString newHostName;
    newHostName=usermanage->setUser(connType,userName,address,password,port,rHostName,cover);
    updateList();
    ui->LoginSite->setCurrentRow(itemRow[newHostName]);
}
void Widget::removeUser(const QString &hostName){
    int count;
    count=ui->LoginSite->currentRow();  //获取当前选择的站点

    if(hostName.isEmpty()||count==0||count==1) return;
    usermanage->removeUser(hostName);
    int row=itemRow[hostName];
    updateList();
    if(row==ui->LoginSite->count())
        row--;
    ui->LoginSite->setCurrentRow(row);
}
bool Widget::isUserDataModified(const QString &hostName){
    int count;
    count=ui->LoginSite->currentRow();  //获取当前选择的站点

    UData udata(ui->connType->currentIndex(),    //输入框的用户数据
                ui->editUserName->text(),
                ui->editAddress->text(),
                ui->editPassword->text(),
                ui->editPort->value()
                );
    if(ui->editPort->isEmpty()) //如果端口为空
        udata.port=-1;
    if(udata.connType==4)       //如果是串口协议
        udata.address=ui->editSerial->text();
    UData udataM=usermanage->userData(hostName);
    if(count==1) udataM=UData();
    if(udata==udataM){
        QString eHostName=ui->editHostName->text();
        //用户有输入数据***********************
        if(count==1&&eHostName.isEmpty()) return true;
        if(hostName==eHostName) return true;
        //***********************************

        // qDebug()<<"无更改isUserDataModified: "<<hostName;
    }
    // qDebug()<<"有更改isUserDataModified: "<<hostName;
    // if(udata.userName!=usermanage->userData(hostName).userName) qDebug()<<"userName"<<udata.userName<<usermanage->userData(hostName).userName;
    // if(udata.address!=usermanage->userData(hostName).address) qDebug()<<"address"<<udata.address<<usermanage->userData(hostName).address;
    // if(udata.connType!=usermanage->userData(hostName).connType) qDebug()<<"connType";
    // if(udata.password!=usermanage->userData(hostName).password) qDebug()<<"password";
    // if(udata.port!=usermanage->userData(hostName).port) qDebug()<<"port";
    return false;   //用户无输入数据
}
bool Widget::isSaveUser(){
    qDebug()<<ui->LoginSite->currentItem()->text();
    // static int lastCount_=0;    //上次item的项
    // int lastCount=lastCount_;
    // lastCount_=ui->LoginSite->currentRow();
    // if(lastCount==0||lastCount==ui->LoginSite->count()) return false;
    // if(ui->LoginSite->currentItem())
    //     if(isUserDataModified(ui->LoginSite->item(lastCount)->text())) return false;
    // return true;

    int currentRow = ui->LoginSite->currentRow();
    if(currentRow == 0 || currentRow == ui->LoginSite->count()) return false;

    if(ui->LoginSite->currentItem()) {
        QString hostName = ui->LoginSite->currentItem()->text();
        if(isUserDataModified(hostName)) {
            return false;  // 数据未修改，可以点击
        }
    }
    return true;  // 数据已修改，不允许点击
}
void Widget::setLoginParmsEdit(bool isEdit){
    ui->editHostName->setReadOnly(!isEdit);
    ui->connType->setEnabled(isEdit);
    ui->editUserName->setReadOnly(!isEdit);
    ui->editPassword->setReadOnly(!isEdit);
    ui->editPort->setReadOnly(!isEdit);
    ui->editAddress->setReadOnly(!isEdit);
    ui->editSerial->setReadOnly(!isEdit);
    if(isEdit){ //如果允许编辑
        ui->editHostName->setStyleSheet("");
        ui->editUserName->setStyleSheet("");
        ui->editPassword->setStyleSheet("");
        ui->editPort->setStyleSheet("");
        ui->editAddress->setStyleSheet("");
        ui->editSerial->setStyleSheet("");
    }
    else{   //如果不允许编辑
        ui->editHostName->setStyleSheet("background: #f3f3f3;");
        ui->editUserName->setStyleSheet("background: #f3f3f3;");
        ui->editPassword->setStyleSheet("background: #f3f3f3;");
        ui->editPort->setStyleSheet("background: #f3f3f3;");
        ui->editAddress->setStyleSheet("background: #f3f3f3;");
        ui->editSerial->setStyleSheet("background: #f3f3f3;");
    }
}
void Widget::loginServer(){
    if(!QFile::exists(setting->getPuttyPath())){
        QMessageBox::warning(
            this,
            tr("Warning"),
            tr("PuTTY executable not found!")
            );

        return;
    }
    qDebug()<<"aaay";
    UData udata(
        ui->connType->currentIndex(),
        ui->editUserName->text(),
        ui->editAddress->text(),
        ui->editPassword->text(),
        ui->editPort->value()
        );
    if(udata.connType<0||udata.connType>4){
        qDebug()<<"登录退出"<<udata.address;
        return;
    }
    qDebug()<<"yyy";
    QProcess* process = new QProcess(this);
    // QProcess process(this);
    QStringList connTypeName={
        "-ssh",
        "-telnet",
        "-raw",
        "-rlogin",
        "serial"

    };
    // QStringList startupParams;  //启动参数
    // startupParams<<connTypeName[udata.connType];
    // if(!udata.userName.isEmpty()) startupParams<<"-l"<<udata.userName;
    // if(!udata.password.isEmpty()&&(udata.connType==0||udata.connType==4)) startupParams<<"-pw"<<udata.password;
    // startupParams<<"-P"<<QString().number(udata.port)<<udata.address;
    QString puttyStartupParams;  //putty启动参数
    puttyStartupParams=" "+connTypeName[udata.connType]+" ";
    if(!udata.userName.isEmpty())
        puttyStartupParams+="-l "+udata.userName+" ";
    if(!udata.password.isEmpty()&&(udata.connType==0||udata.connType==4))
        puttyStartupParams+="-pw "+udata.password+" ";
    puttyStartupParams+="-P "+QString().number(udata.port)+" "+udata.address;

    QString puttyName;  //程序名
    QString path;   //程序路径
    QString puttyPath;
    puttyPath=setting->getPuttyPath();
    path=setting->getPuttyPath();
    for(int i=setting->getPuttyPath().length()-1;i>=0;i--){
        if(puttyPath[i]=='/') break;
        puttyName+=puttyPath[i];
        path.chop(1);
    }
    std::reverse(puttyName.begin(), puttyName.end()); //反转

    qDebug()<<path<<puttyName<<puttyStartupParams;
    QStringList cmdStartupParams;  //cmd启动参数
    cmdStartupParams << "/c"
         << "cd /d "+path+" && "+puttyName+puttyStartupParams;

    qDebug()<<"登录";
    connect(process,&QProcess::finished,this,[=]{
        qDebug()<<"putty程序退出";
        disconnect(process,0,0,0);
        delete process;
    });

    // process->start(setting->getPuttyPath(),startupParams);
    process->start("cmd",cmdStartupParams);

}
bool Widget::isReselectionAllowed(){
    bool allow;
    allow=isSaveUser();
    if(allow){
        QMessageBox msgBox(QMessageBox::Question,
                            tr("Confirmation"),
                           "<font color='#003399' style='font-size:12pt;'>"+tr("You are editing a session. Do you want to save the changes?")+"</font>",
                           QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        msgBox.setStyleSheet(R"(
            QMessageBox {
                background: white;  /* 设置对话框的背景为白色 */
                border-bottom: 40px solid #f2f2f2;
            }
            QMessageBox QPushButton {
                font-size: 10pt;
            }
            )");
        msgBox.button(QMessageBox::Yes)->setText(tr("Yes (Y)"));
        msgBox.button(QMessageBox::No)->setText(tr("No (N)"));
        msgBox.button(QMessageBox::Cancel)->setText(tr("Cancel"));
        int reply=msgBox.exec();
        qDebug()<<"reply"<<reply;
        if(reply==QMessageBox::Yes){
            ui->pushButtonSave->click();
            allow=false;
        }
        if(reply==QMessageBox::No){
            allow=false;
        }
    }
    return allow;
}
void Widget::protoFrontend(){
    static QString _connTypeItemName="SSH";
    int port;
    port=ui->editPort->value();
    if(ui->editPort->isEmpty())
        port=-1;
    QString connTypeItemName;
    connTypeItemName=ui->connType->currentText();
    // int portList[]={22,23,-1,513,9600};

    if(connTypeItemName=="Serial"){
        ui->editPort->setMaximum(INT_MAX);
        ui->stackedWidgetAddress->setCurrentIndex(1);
        ui->label_4->setText(tr("Speed"));

    }
    else{
        ui->editPort->setMaximum(65535);
        ui->stackedWidgetAddress->setCurrentIndex(0);
        ui->label_4->setText(tr("Port"));
    }
    if(connTypeItemName=="Serial"||connTypeItemName=="SSH"){
        ui->editPassword->setEnabled(true);
        ui->label_6->setText(tr("Password"));
    }
    else{
        ui->editPassword->setEnabled(false);
        ui->label_6->setText(tr("Password (N/A)"));
    }
    if((_connTypeItemName=="SSH"&&port==22)||
        (_connTypeItemName=="Telnet"&&port==23)||
        (_connTypeItemName=="Rlogin"&&port==513)||
        (_connTypeItemName=="Raw"&&port==-1)||
        (_connTypeItemName=="Serial"&&port==9600)){
        qDebug()<<connTypeItemName;
        if(connTypeItemName=="SSH")
            ui->editPort->setValue(22);
        if(connTypeItemName=="Telnet")
            ui->editPort->setValue(23);
        if(connTypeItemName=="Rlogin")
            ui->editPort->setValue(513);
        if(connTypeItemName=="Raw"){    //RAW协议默认端口为空
            ui->editPort->setValue(0);
            ui->editPort->clear();
            // if(ui->editSerial->text().isEmpty())
        }
        if(connTypeItemName=="Serial")
             ui->editPort->setValue(9600);
    }
    _connTypeItemName=connTypeItemName;

}

#include "setting.h"
#include "ui_setting.h"
#include <QFileDialog>
#include <QTranslator>
#include <QDir>

Setting::Setting(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Setting)
{
    ui->setupUi(this);
    setWindowFlags(Qt::Window);
    setWindowModality(Qt::ApplicationModal); // 关键

    updateLanguageList();
    ui->editPuttyPath->addItem("内部");
    if(!Putty_Path.isEmpty()) ui->editPuttyPath->addItem(Putty_Path);
    ui->editPuttyPath->addItem("...");

    connect(ui->editPuttyPath,&QComboBox::currentTextChanged,this,[=](const QString &path){
        if(path=="...")  inputPath();
    });
    connect(ui->pushButtonCancel,&QPushButton::clicked,this,[=]{    //取消
        if(ui->editPuttyPath->currentIndex()==0)
            Putty_Path.clear();
        for(int i=0;i<=ui->editLanguage->count();i++){
            if(ui->editLanguage->itemText(i)==Language){
                ui->editLanguage->setCurrentIndex(i);
                break;
            }
        }
        updateEditPuttyPath();
        close();
    });
    connect(ui->pushButtonSave,&QPushButton::clicked,this,[=]{      //保存
        if(ui->editPuttyPath->currentIndex()==1)
            Putty_Path=ui->editPuttyPath->itemText(1);
        else
            Putty_Path.clear();
        Language=ui->editLanguage->currentText();
        emit saveSetting();
        close();
    });

}

Setting::~Setting()
{
    delete ui;
}
void Setting::inputPath(){
    QString fileName = QFileDialog::getOpenFileName(
        this,                  // parent
        "选择putty程序", // caption
        QCoreApplication::applicationDirPath(),
        "putty程序 (*.exe);" // filter
        );
    // ui->editPuttyPath->setCurrentIndex(0);
    if (fileName.isEmpty()){
        ui->editPuttyPath->setCurrentIndex(0);
        return;
    }
    // updateEditPuttyPath();
    ui->editPuttyPath->setItemText(1,fileName);
    if(ui->editPuttyPath->count()<3) ui->editPuttyPath->addItem("...");
    ui->editPuttyPath->setCurrentIndex(1);
}
QString Setting::getPuttyPath(){
    if(Putty_Path.isEmpty()) return Internal_Putty_Path;
    return Putty_Path;
}
QString Setting::getLanguage(bool isAbsPath){
    if(isAbsPath) return Language_File_Path+"/"+Language;
    return Language;
}
void Setting::setPuttyPath(const QString &path){
    Putty_Path=path;
    updateEditPuttyPath();
}
void Setting::updateEditPuttyPath(){
    ui->editPuttyPath->clear();
    if(Putty_Path==Internal_Putty_Path) Putty_Path.clear();
    if(Putty_Path.isEmpty()){
        ui->editPuttyPath->addItems({
            tr("Internal"),
            "..."
        });
    }
    else{
        ui->editPuttyPath->addItems({
            tr("Internal"),
            Putty_Path,
            "..."
        });
        ui->editPuttyPath->setCurrentIndex(1);
    }
}
void Setting::updateLanguageList(){
    QDir dir(Language_File_Path);
    Language_List={"English"};
    Language_List+=dir.entryList(QStringList() << "*.qm", QDir::Files);
    ui->editLanguage->addItems(Language_List);
}
void Setting::setLanguage(const QString &language){
    int currentIndex=-1;
    for(int i=0;i<=ui->editLanguage->count();i++){
        if(ui->editLanguage->itemText(i)==language){
            currentIndex=i;
            break;
        }
    }
    if(currentIndex==-1)
        currentIndex=0;
    ui->editLanguage->setCurrentIndex(currentIndex);
    Language=language;
    QTranslator *translator=new QTranslator(this);     //设置语言
    translator->load(getLanguage(true));
    qApp->installTranslator(translator);
    ui->retranslateUi(this);

    // emit saveSetting();
}


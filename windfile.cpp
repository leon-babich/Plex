#include "windfile.h"
#include "ui_windfile.h"
#include <QDebug>

WindFile::WindFile(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindFile)
{
    ui->setupUi(this);

    m_pathWrite = "RecordData.bin";
    m_pathRead = "WriteData.bin";
    pathWriteTar = "RecordTargets.txt";
    isWriteData = false;
    isReadData = false;
    m_isSetRead = false;
    m_isChangedPathRecData = false;
    m_typeRec = WRITE_REC;

    model = new QFileSystemModel(this);
    model->setFilter(QDir::AllEntries);
    model->setRootPath("");
    ui->listView->setModel(model);
}

WindFile::~WindFile()
{
    delete ui;
}

QString WindFile::getPathWrite()
{
    m_isChangedPathRecData = false;
    return m_pathWrite;
}

QString WindFile::getPathRead()
{
    m_isChangedPathRecData = false;
    return m_pathRead;
}

void WindFile::setIsSetRead(TypeRec type)
{
    m_typeRec = type;
}

void WindFile::setPathRead(QString path)
{
    m_pathRead = path;
}

bool WindFile::getIsChangedPath()
{
    return m_isChangedPathRecData;
}

void WindFile::on_butSelect_clicked()
{
    if(m_typeRec == READ_REC) {
        isReadData = true;
        m_pathRead = ui->lineEdit->text();
    }
    else if(m_typeRec == WRITE_REC) {
        isWriteData = true;
        m_pathWrite = ui->lineEdit->text();
    }
    else if(m_typeRec == WRITE_REC_TAR) {
        emit sigIsWriteTar();
        pathWriteTar = ui->lineEdit->text();
    }

    this->close();
    m_isChangedPathRecData = true;
}

void WindFile::on_listView_doubleClicked(const QModelIndex &index)
{
    QListView *listView = (QListView*)sender();
    QFileInfo fileInfo = model->fileInfo(index);
    m_index = index;

    if(fileInfo.fileName() == "..") {
        QDir dir = fileInfo.dir();
        dir.cdUp();
        listView->setRootIndex(model->index(dir.absolutePath()));
    }
    else if(fileInfo.fileName() == ".") {
        listView->setRootIndex(model->index(""));
    }
    else if(fileInfo.isDir()) {
        listView->setRootIndex(index);
    }

//    if(m_typeRec == READ_REC || (m_typeRec != READ_REC && fileInfo.isFile())) {
//        ui->lineEdit->setText(fileInfo.absoluteFilePath());
//    }
//    else {
//        ui->lineEdit->setText(fileInfo.absoluteFilePath() + "/RecordData.bin");
//    }

    QString strPath = fileInfo.absoluteFilePath();

    if(m_typeRec == WRITE_REC && !fileInfo.isFile()) {
        strPath.append("/RecordData.bin");
    }
    else if(m_typeRec == WRITE_REC_TAR && !fileInfo.isFile()) {
        strPath.append("/RecordTargets.txt");
    }

    ui->lineEdit->setText(strPath);
}

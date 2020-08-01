#ifndef WINDFILE_H
#define WINDFILE_H

#include <QDialog>
#include <QFileSystemModel>
#include <QDir>

namespace Ui {
class WindFile;
}

enum TypeRec { READ_REC, WRITE_REC, WRITE_REC_TAR };

class WindFile : public QDialog
{
    Q_OBJECT

public:
    explicit WindFile(QWidget *parent = 0);
    ~WindFile();
    QString getPathWrite();
    QString getPathRead();
    bool getIsChangedPath();
    void setIsSetRead(TypeRec type);
    void setPathRead(QString path);

private slots:
    void on_butSelect_clicked();
    void on_listView_doubleClicked(const QModelIndex &index);

signals:
    void sigIsWriteTar();

public:
    bool isWriteData;
    bool isReadData;
    QString m_pathWrite;
    QString m_pathRead;
    QString pathWriteTar;

private:
    bool m_isChangedPathRecData;
    bool m_isSetRead;

    Ui::WindFile *ui;
    QFileSystemModel *model;
    QModelIndex m_index;
    TypeRec m_typeRec;
};

#endif // WINDFILE_H

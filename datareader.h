#ifndef DATACOLLECTOR_H
#define DATACOLLECTOR_H

#include <QObject>
#include <QDebug>
#include <QTime>
#include <QLibrary>
#include <QThread>
#include <QMutex>
#include <QFile>

const int LENGTHARRAY = 40*1000*1000;
const int WIDTHARRAY = 8;
const int S = 40;

enum ModeReading { BOARD, DISK };
enum TypeExeption { WRONGDATA, OUTRANGE };

class ExeptionRange : public std::exception
{
public:
    ExeptionRange(const char* str, QString inf, QString func)
        : exception(str) { this->type = WRONGDATA; numElement = 0; infomation = inf; nameFunc = func; }
    ExeptionRange(const char* str, int n, QString func)
        : exception(str) { this->type = OUTRANGE; numElement = n; nameFunc = func; }
    int getNumElemtnt() { return numElement; }
    bool getType() { return type; }
    QString getInformation() { return infomation; }
    QString getFunc() { return nameFunc; }
private:
    int numElement;
    QString infomation;
    QString nameFunc;
    TypeExeption type = WRONGDATA;
};

struct BoardData
{
    short int dataArray[LENGTHARRAY][WIDTHARRAY];                //промежуточный массив данных для передачи в обработку
    short int fileDataArray[WIDTHARRAY][LENGTHARRAY/S];          //исходный урезанный массив данных считанный из файла
};

typedef int (*INIT_BRD) ();
typedef int (*START_BRD) (unsigned long long sizeArray, char* array);
typedef int (*STATUS_BRD) ();
typedef int (*CLEAR_BRD) ();
typedef int (*CLOSE_BRD) ();

class DataReader : public QObject
{
    Q_OBJECT
public:
    explicit DataReader(QObject *parent = nullptr);
    ~DataReader();
    void setRunning(bool running);
    int getTimeRead() const;
    int getTimeCopy();
    void loadBRD();
    void closingBRD();
    void setMode(ModeReading mode);
    ModeReading getMode() const;
    bool getIsBoard();
    void updataFile();
    BoardData* getBoardData();
    void setPath(QString path);
    bool startFile();
    void finishFile();
    bool isEmptyFile();
    bool getIsStartFile();

public slots:
    void runRead();

signals:
    void sigFinished();
    void sigSendMessage(QString strMessage);
    void sigDisplayErrorMessage(QString strMessage);
    void sigDataReady();
    void sigStopHandle();
    void sigTime(int time);

private:
    short int m_dataArray[WIDTHARRAY][LENGTHARRAY];                //исходный массив данных
    bool m_isRunning;
    bool m_isWorking;
    int m_timeReading;
    int timeCoping;
    QLibrary *libBRD;
    INIT_BRD initBRD;
    START_BRD startBRD;
    STATUS_BRD statusBRD;
    CLEAR_BRD clearBRD;
    CLOSE_BRD closeBRD;
    bool m_isBoard;
    bool m_isStartBRD;
    bool m_isWorkFile;
    bool m_isEmptyFile;
    bool m_isFinishFile;
    unsigned long long m_sizeArray;
    QMutex *m_pMutex;
    ModeReading m_modeReding;
    QFile *m_fileOut;
    BoardData m_boardData;
    QString m_path;
    QTime time;

private:
    void copyArray();          //копирование массива данных в промежуточный массив
    void readRandom();         //чтение рандомных чисел (для теста)
    void readBoard();          //чтение данных из платы АЦП и заполнение массива
    void readRecord();         //чтение данных из файла и заполение урезанного массива
    void setIsEmptyFile(bool isEmpty);
    void savePath();
};

#endif // DATACOLLECTOR_H

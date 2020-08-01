#include "datareader.h"

DataReader::DataReader(QObject *parent) : QObject(parent)
{
    m_isRunning = true;
//    m_isRunning = false;
    m_isWorking = false;
    m_timeReading = 0;
    timeCoping = 0;
    m_isBoard = false;
    m_isStartBRD = false;
    m_isFinishFile = false;
    m_isEmptyFile = true;
    m_isWorkFile = false;

    m_pMutex = new QMutex;
    libBRD = new QLibrary;
    m_sizeArray = (LENGTHARRAY*WIDTHARRAY) * sizeof(short int);
    m_modeReding = BOARD;

    m_fileOut = new QFile;
    m_path = "writeData.bin";
    m_fileOut->setFileName(m_path);
}

DataReader::~DataReader()
{
}

void DataReader::setRunning(bool running)
{
    m_isWorking = running;
//    m_isRunning = running;
}

int DataReader::getTimeRead() const
{
    return m_timeReading;
}

void DataReader::loadBRD()
{
    m_modeReding = BOARD;

    QString brdPath = "A0BRD.dll";
    QString strMessage;
    QString strMessageConnect;
    QString strMessageLoading = "";
    QString strMessageInit = "";
    QString strMessageStart = "";
    QString strMessageStatus = "";
    QString strMessageClear = "";
    QString strMessageClose = "";

    m_isBoard = true;
    libBRD->setFileName(brdPath);
    libBRD->load();

    bool isLoad = true;

    if(!libBRD->isLoaded())
    {
        strMessageLoading = " Library 'AOBRD' not loaded!";
        m_isBoard = false;
        isLoad = false;
    }

    initBRD = (INIT_BRD)libBRD->resolve("InitBoard");
    startBRD = (START_BRD)libBRD->resolve("StartBoard");
    statusBRD = (STATUS_BRD)libBRD->resolve("GetStatus");
    clearBRD = (CLEAR_BRD)libBRD->resolve("ClearBoard");
    closeBRD = (CLOSE_BRD)libBRD->resolve("ClearBoard");

    if(!initBRD)
    {
        strMessageInit = "  'Init_BRD' not found!";
        m_isBoard = false;
    }
    else
    {
        int resultInit = initBRD();

        if(resultInit < 0)
        {
            m_isBoard = false;
            QString strInit;
            strInit.setNum(resultInit);
            strMessageInit = "  'Init_BRD' returned: '" + strInit + "'.";
        }
    }

    if(!startBRD)
    {
        strMessageStart = "  'Start_BRD' not found!";
        m_isBoard = false;
    }

    if(!statusBRD)
    {
        strMessageStatus = "  'Status_BRD' not found!";
        m_isBoard = false;
    }

    if(!clearBRD) {
        strMessageClear = "  'Clear_BRD' not found!";
        m_isBoard = false;
    }

//    if(!closeBRD)
//    {
//        strMessageClose="  'Close_BRD' not found!";
//        m_isBoard = false;
//    }

    if(m_isBoard)
    {
        strMessageConnect = "The board connected.";
    }
    else
    {
        strMessageConnect = "<FONT COLOR=RED>The board not connected:";
    }

    if(isLoad) {
        strMessage = strMessageConnect + strMessageLoading + strMessageInit + strMessageStart + strMessageStatus + strMessageClose;
    }
    else {
        strMessage = strMessageConnect + strMessageLoading;
    }

    emit sigSendMessage(strMessage);
}

void DataReader::runRead()
{
    m_timeReading = time.msecsTo(QTime::currentTime());

    while (m_isRunning)
    {
        if(m_isWorking) {
            try {
                switch (m_modeReding)
                {
                case BOARD:
                    readBoard();
                    break;
                case DISK:
                    readRecord();
                    break;
                default:
                    break;
                }
            }
            catch (ExeptionRange ex) {
                QString message;

                if(ex.getType() == WRONGDATA) {
                    message = ex.what() + ex.getInformation() + "\n" + ex.getFunc();
                }
                else if(ex.getType() == OUTRANGE) {
                    message = ex.what() + QString::number(ex.getNumElemtnt()) + "\n" + ex.getFunc();
                }

                emit sigDisplayErrorMessage(message);
            }
            catch (const std::exception& ex) {
                QString message = ex.what();
                emit sigDisplayErrorMessage(message);
            }
        }
        else {
            QThread::msleep(500);
        }
    }

    emit sigFinished();
}

void DataReader::closingBRD()
{
    if(m_isBoard)
    {
        int resultClear = clearBRD();

        if(resultClear < 0)
        {
            emit sigSendMessage("Failed closing board");
        }
        else {
            emit sigSendMessage("Board close");
        }

        m_isBoard = false;
    }
}

void DataReader::copyArray()
{
    m_pMutex->lock();

    memcpy(m_boardData.dataArray, m_dataArray, (LENGTHARRAY*WIDTHARRAY)*sizeof(short int));

    m_pMutex->unlock();
    emit sigDataReady();

    m_timeReading = time.msecsTo(QTime::currentTime());
    time = QTime::currentTime();
    emit sigTime(m_timeReading);
}

void DataReader::setMode(ModeReading mode)
{
    if(mode==DISK && m_isStartBRD) {
        closingBRD();
    }
    if(mode==BOARD && m_isWorkFile) {
        finishFile();
    }

    m_modeReding = mode;
}

ModeReading DataReader::getMode() const
{
    return m_modeReding;
}

void DataReader::readRandom()
{
    short int randNum;
    int length = LENGTHARRAY;
    int width = WIDTHARRAY;

    for(int j = 0; j < width; j++)
    {
        for(int i = 0; i < length; i++)
        {
            randNum = qrand() % 4050;

            m_dataArray[j][i] = randNum;

            if(j == width / 2)
            {
                m_dataArray[j][i] = 5000;
            }
        }
    }

    copyArray();
}

void DataReader::readBoard()
{
    if(m_isBoard)
    {
        if(!m_isStartBRD)
        {
            int resultStartBRD;
            QString strMessage;
            QString strResult;

            resultStartBRD=startBRD(m_sizeArray,(char*)m_dataArray);
            strResult.setNum(resultStartBRD);
            strMessage = "StartBRD runned with " + strResult;
            m_isStartBRD = true;

            emit sigSendMessage(strMessage);
        }

        QString strMessage;
        QString strStatus;
        int resultStatus = statusBRD();

        strStatus.setNum(resultStatus);

        if(resultStatus == 0) {
            copyArray();
            QThread::msleep(200);
        }
        else if(resultStatus < 0)
        {
            strMessage = "StatusBRD returned " + strStatus;
            emit sigSendMessage(strMessage);
        }

        QThread::msleep(10);
    }
}

bool DataReader::getIsBoard()
{
    return m_isBoard;
}

void DataReader::readRecord()
{
    if(!startFile()) {
        return;
    }

    if(m_isEmptyFile) {
        setIsEmptyFile(false);
    }

    if(!m_fileOut->atEnd())
    {
        m_fileOut->read(reinterpret_cast<char *>(m_boardData.fileDataArray), ((LENGTHARRAY/S)*WIDTHARRAY) * sizeof(short int));

        m_timeReading = time.msecsTo(QTime::currentTime());
        time = QTime::currentTime();
        emit sigTime(m_timeReading);
    }
    else
    {
        if(!m_isFinishFile)
        {
            emit sigStopHandle();
            emit sigSendMessage("File finished.");
        }

        m_isFinishFile = true;
    }

    QThread::msleep(950);
    emit sigDataReady();
}

void DataReader::setIsEmptyFile(bool isEmpty)
{
    m_isEmptyFile = isEmpty;
}

void DataReader::savePath()
{
    try {
        QFile fileOut("settings.txt");

        if(!fileOut.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            throw ExeptionRange("Fail opening file: ", fileOut.fileName(), "DataReader::savePath()");
            return;
        }

        QString str;
        QString strChannels;
        int n = 0;

        while(!fileOut.atEnd()) {
            str = fileOut.readLine();
            str.remove("\r");
            str.remove("\n");
            n++;
            strChannels = n == 2 ? strChannels + str : strChannels;
        }

        fileOut.close();

        QFile fileIn("settings.txt");
        QTextStream stream(&fileIn);

        if(!fileIn.open(QIODevice::WriteOnly))
        {
            throw ExeptionRange("Fail opening file: ", fileIn.fileName(), "DataReader::savePath()");
            return;
        }

        stream << m_path;
        if(n >= 2) stream << endl << strChannels;
        fileIn.close();
    }
    catch (ExeptionRange ex) {
        QString message;

        if(ex.getType() == WRONGDATA) {
            message = ex.what() + ex.getInformation() + "\n" + ex.getFunc();
        }
        else if(ex.getType() == OUTRANGE) {
            message = ex.what() + QString::number(ex.getNumElemtnt()) + "\n" + ex.getFunc();
        }

        emit sigDisplayErrorMessage(message);
    }
    catch (const std::exception& ex) {
        QString message = ex.what();
        emit sigDisplayErrorMessage(message);
    }
}

void DataReader::updataFile()
{
    m_fileOut->seek(0);
}

BoardData* DataReader::getBoardData()
{
    return &m_boardData;
}

void DataReader::setPath(QString path)
{
    if(m_fileOut->isOpen()) {
        m_fileOut->close();
    }

    m_path = path;
    m_fileOut->setFileName(m_path);
    setIsEmptyFile(true);
    savePath();
}

bool DataReader::startFile()
{
    try {
        if(!m_fileOut->isOpen())
        {
            if(!m_fileOut->open(QIODevice::ReadOnly))
            {
                throw ExeptionRange("Fail opening file: ", m_fileOut->fileName(), "DataReader::startFile()");
//                emit sigSendMessage("<FONT COLOR=RED>Fail opening file: " + m_path);
                return false;
            }

            m_isWorkFile = true;
            m_isFinishFile = false;
        }
    }
    catch (ExeptionRange ex) {
        QString message;

        if(ex.getType() == WRONGDATA) {
            message = ex.what() + ex.getInformation() + "\n" + ex.getFunc();
        }
        else if(ex.getType() == OUTRANGE) {
            message = ex.what() + QString::number(ex.getNumElemtnt()) + "\n" + ex.getFunc();
        }

        emit sigDisplayErrorMessage(message);
    }
    catch (const std::exception& ex) {
        QString message = ex.what();
        emit sigDisplayErrorMessage(message);
    }

    return true;
}

void DataReader::finishFile()
{
    m_fileOut->close();
    setIsEmptyFile(true);
    m_isWorkFile = false;
}

bool DataReader::isEmptyFile()
{
    return m_isEmptyFile;
}

bool DataReader::getIsStartFile()
{
    return m_isWorkFile;
}

int DataReader::getTimeCopy()
{
    return timeCoping;
}

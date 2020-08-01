#ifndef COMPORT_H
#define COMPORT_H

#include <QObject>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QDataStream>
#include <windows.h>
#include <QThread>
#include <QDebug>
#include <QTimer>

#include "Mghdlc.h"
#include "spsprotocol.h"

struct SettingsPort
{
    QString namePort;
    QString nameMicro;
    qint32 baudRate;
    QSerialPort::DataBits dataBits;
    QSerialPort::Parity parity;
    QSerialPort::StopBits stopBits;
    MGSL_PARAMS paramsMicrogate;
};

enum ModeDataTransfer { NONE, TEST_MES, PRIM_PLOT, AZIMUTH_REF };

namespace microgate {
    QString getAvailablePorts(QVector<QString> &vecPorts);
}

class Port: public QObject
{
    Q_OBJECT
public:
    explicit Port(QObject *parent = nullptr);
    void setSettings(SettingsPort &m_settings);
    bool openCOMPort();
    bool closeCOMPort();
    bool openMGPort();
    bool closeMGPort();
    void setRunning(bool running);
    bool getIsConnection();

private:
    void displayBlockData();
    bool writeMGPort();
    bool writeCOMPort();
    bool sendData();
    void sendToPort();
    void sendBlockPrimaryPlots();

public slots:
    void runPort();
    void readMGPort();
//    void sendBlockData(bool isNorth,qreal az,qreal dist);
    void slotReceiveBlockPrimaryPlots(QList<QPair<float,float>*> list);
    void slotSendAziuthReference(int count);
//    void slotAppearData(bool isNorth,qreal az,qreal dist);
    Sps::ReportID getReportID(unsigned short hdr);

private slots:
    void readCOMPort();
    void slotCheckConnection();

signals:
    void sigSendMessage(QString strMessage);
    void sigFinished();
//    void sigTarget(bool isNorth,qreal az, qreal dist);
    void sigSendedData();
    void sigReceivedData();
    void sigSetConnection(bool connection);

private:
    QSerialPort *m_port;
    QSerialPort::BaudRate baudRate;
    SettingsPort m_settings;
    bool m_isOpenedCOM;
    bool m_isConnectedMG;
    bool m_isRunning;
    bool m_isConnectedCOM;
    bool m_isConnected;
    bool m_isAllowErrorReading;
    QList<QPair<float,float>*> m_poinerListTarget;
    bool m_checkConnection;
    int m_countReceivedTestMessages;

    PHANDLE m_handle;
    HANDLE m_hand;
    QByteArray m_blockToSend;
    QByteArray m_receivedBlock;
    int m_sizeData;
    qint64 totalSize;
    OVERLAPPED lapped;
    PMGSL_RECEIVE_REQUEST requestMG;
    Sps::TestMessage testMessage;
    Sps::AzimuthReferenceMessage azimuthMessage;
    Sps::PrimaryPlotMessage plotMessage;
    Sps::ReportID reportID;

    ModeDataTransfer m_modeTransfer;

public:
    QTimer *m_timerStopConnection;
};

#endif // COMPORT_H

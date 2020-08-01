#include "port.h"

Port::Port(QObject *parent) : QObject(parent)
{
    m_port = new QSerialPort;

    m_hand = nullptr;
    m_handle = &m_hand;
    m_isConnectedMG = false;
    m_isOpenedCOM = false;
    m_isRunning = false;
    m_isConnectedCOM = false;
    m_isConnected = false;
    m_isAllowErrorReading = true;
    m_sizeData = sizeof(short int)*2;
    totalSize = 0;
    lapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    requestMG = reinterpret_cast<MGSL_RECEIVE_REQUEST*>(malloc(sizeof(MGSL_RECEIVE_REQUEST) + m_sizeData));
    plotMessage.bits.height = 0;
    reportID = Sps::PRIMARY_PLOT;
    testMessage = Sps::TestMessage(0);
    m_modeTransfer = NONE;
    m_countReceivedTestMessages = 0;

    m_timerStopConnection = new QTimer;
    connect(m_timerStopConnection, &QTimer::timeout, this, &Port::slotCheckConnection, Qt::DirectConnection);
}

void Port::setSettings(SettingsPort &settings)
{
    m_settings = settings;
}

bool Port::openCOMPort()
{
    m_port->setPortName(m_settings.namePort);

    if(!m_port->open(QIODevice::ReadWrite))
    {
        if(m_port->isOpen()) {
            emit sigSendMessage("<FONT COLOR=RED>Port was already opened");
        }
        else {
            emit sigSendMessage("<FONT COLOR=RED>Port not opened");
        }

        return false;
    }
    else
    {
        emit sigSendMessage("Port '" + m_settings.namePort + "' opened");
        m_isOpenedCOM = true;

        bool isBaud = m_port->setBaudRate(m_settings.baudRate);
        bool isBits = m_port->setDataBits(m_settings.dataBits);
        bool isParity = m_port->setParity(m_settings.parity);
        bool isStop = m_port->setStopBits(m_settings.stopBits);
        bool isFlow = m_port->setFlowControl(QSerialPort::NoFlowControl);

        if(!isBaud||!isBits||!isParity||!isStop || !isFlow) {
            emit sigSendMessage("<FONT COLOR=RED>The settings could not be set");
        }
        else {
            emit sigSendMessage("The settings set. Baud rate: "+QString::number(m_settings.baudRate));
            emit sigSendMessage("Data bits: "+QString::number(m_settings.dataBits));
            emit sigSendMessage("Parity: "+QString::number(m_settings.parity));
            emit sigSendMessage("Stop bits: "+QString::number(m_settings.stopBits));
        }
    }

    return true;
}

bool Port::closeCOMPort()
{
    if(m_port->isOpen()) {
        m_port->close();
        m_isOpenedCOM = false;
        emit sigSendMessage("Port '" + m_settings.namePort + "' closed");
        disconnect(m_port,&QSerialPort::readyRead,this,&Port::readCOMPort);
        return true;
    }
    else {
        emit sigSendMessage("<FONT COLOR=RED>Port was not open");
        return false;
    }
}

bool Port::openMGPort()
{
    QByteArray nameByte = m_settings.nameMicro.toLocal8Bit();
    char* name = nameByte.data();
    ULONG rc;

    QString strName = name;
    if(strName == "None") {
        return false;
    }

    rc = MgslOpenByName(name, m_handle);

    if(rc != ERROR_SUCCESS)
    {
        if(strName != "None") emit sigSendMessage("<FONT COLOR=RED>Error opening port '" + m_settings.nameMicro + "'");
        return false;
    }
    else
    {
        emit sigSendMessage("Port '" + m_settings.nameMicro + "' opened");
        m_isConnectedMG = true;
        rc = MgslSetParams(m_hand,&m_settings.paramsMicrogate);

        if(rc!=ERROR_SUCCESS) {
            emit sigSendMessage("<FONT COLOR=RED>Error settings parametrs of port");
            return false;
        }
        else {
            emit sigSendMessage("The settings set. Baud rate: "+QString::number(m_settings.paramsMicrogate.DataRate));
            emit sigSendMessage("Data bits: "+QString::number(m_settings.paramsMicrogate.DataBits));
            emit sigSendMessage("Parity: "+QString::number(m_settings.paramsMicrogate.Parity));
            emit sigSendMessage("Stop bits: "+QString::number(m_settings.paramsMicrogate.StopBits));
        }

        auto is_hdlc = int(MGSL_MODE_HDLC == m_settings.paramsMicrogate.Mode);

        if (is_hdlc) {
            rc = MgslSetIdleMode(m_hand, HDLC_TXIDLE_ONES);
            if (rc != NO_ERROR) {
                emit sigSendMessage("<FONT COLOR=RED>Error settings parametrs Id mode");
                return false;
            }
        }

        // set blocked mode for MgslRead
        rc = MgslSetOption(m_hand, MGSL_OPT_RX_POLL, is_hdlc ^ 1);
        if (rc != NO_ERROR) {
            emit sigSendMessage("<FONT COLOR=RED>Error seting of option");
            return false;
        }

        // set MgslRead to return only error free data
        rc = MgslSetOption(m_hand, MGSL_OPT_RX_ERROR_MASK, is_hdlc);
        if (rc != NO_ERROR) {
            emit sigSendMessage("<FONT COLOR=RED>Error seting of option 2");
            return false;
        }

        // set blocked mode for MgslWrite and MgslWaitAllSent
        rc = MgslSetOption(m_hand, MGSL_OPT_TX_POLL, 0);
        if (rc != NO_ERROR) {
            emit sigSendMessage("<FONT COLOR=RED>Error seting of option 3");
            return false;
        }

        rc = MgslEnableReceiver(m_hand, TRUE);
        if (rc != NO_ERROR) {
            emit sigSendMessage("<FONT COLOR=RED>Error seting of enable receiver");
            return false;
        }
        rc = MgslEnableTransmitter(m_hand, TRUE);
        if (rc != NO_ERROR) {
             emit sigSendMessage("<FONT COLOR=RED>Error seting of enable transmitter");
            return false;
        }

        return true;
    }
}

bool Port::closeMGPort()
{
    ULONG rc;

    rc = MgslClose(m_hand);

    if(rc != ERROR_SUCCESS) {
        emit sigSendMessage("<FONT COLOR=RED>Error closing port '"+m_settings.nameMicro+"'");
        return false;
    }
    else {
        emit sigSendMessage("Port '"+m_settings.nameMicro+"' closed");
        m_hand=nullptr;
        m_isConnectedMG=false;
        return true;
    }
}

namespace microgate {
    QString getAvailablePorts(QVector<QString> &vecPorts)
    {
        unsigned long count = 0;
        MGSL_PORT *ports;

        ULONG rc = MgslEnumeratePorts(nullptr, 0, &count);

        if (rc != ERROR_SUCCESS) {
            return "<FONT COLOR=RED>Error opening 'EnumeratePorts";
        }

        ports = (MGSL_PORT*)malloc(count*sizeof(MGSL_PORT));

        rc = MgslEnumeratePorts(ports, count * sizeof(MGSL_PORT), &count);
        if (rc != ERROR_SUCCESS) {
            return "<FONT COLOR=RED>Error finding available ports";
        }

        for (auto i = 0; i < int(count); ++i) {
            QString portName = QString(ports[i].DeviceName);
            vecPorts.push_back(portName);
        }

        free(ports);
        return "";
    }
}

void Port::setRunning(bool running)
{
    m_isRunning = running;
}

bool Port::getIsConnection()
{
    if(m_isOpenedCOM || m_isConnectedMG) {
        return true;
    }
    else {
        return false;
    }
}

void Port::runPort()
{
    if(m_isOpenedCOM && !m_isConnectedCOM) {
        connect(m_port,&QSerialPort::readyRead,this,&Port::readCOMPort);
        m_isConnectedCOM = true;
    }

    while (m_isRunning)
    {
        if(m_modeTransfer != NONE) {
            sendToPort();
        }

        readMGPort();
    }

    emit sigFinished();
}

bool Port::writeCOMPort()
{
    int count = m_port->write(m_blockToSend);

    if(count) {
        emit sigSendedData();
        return true;
    }
    else
    {
        emit sigSendMessage("<FONT COLOR=RED>The data cann't send");
        return false;
    }
}

bool Port::sendData()
{
    bool isSent;

    if(m_isOpenedCOM) {
        isSent = writeCOMPort();
    }
    else if(m_isConnectedMG) {
        isSent = writeMGPort();
    }

    return isSent;
}

bool Port::writeMGPort()
{
    int totalCount = 0;

    while(totalCount < m_blockToSend.size()) {
        auto width = qMin((m_blockToSend.size() - totalCount), 4096);
        int count = MgslWrite(m_hand, (unsigned char*)m_blockToSend.data()+totalCount, width);

        if(count != 0) {
            emit sigSendedData();
        }
        else {
            emit sigSendMessage("<FONT COLOR=RED>The data cann't send");
            return false;
        }

        totalCount += count;
    }

    MgslWaitAllSent(m_hand);
    return true;
}

void Port::readCOMPort()
{
    m_receivedBlock.append(m_port->readAll());

//    m_blockToSend=m_receivedBlock;
//    m_port->write(m_blockToSend);

    if(m_receivedBlock.size() >= m_sizeData)
    {
        emit sigReceivedData();
        displayBlockData();
        m_receivedBlock.clear();
    }
}

void Port::slotCheckConnection()
{
    if(!m_isConnected) {
        return;
    }

    if(!m_checkConnection) {
        m_isConnected = false;
        emit sigSetConnection(false);
    }

    m_checkConnection = false;
}

void Port::readMGPort()
{
    if(!m_isConnectedMG) {
        return;
    }

    QString strMessage;
    int count;
    int status;
    QVector<unsigned char> buf;
    buf.resize(m_sizeData);

    count = MgslReadWithStatus(m_hand, buf.data(), m_sizeData, &status);

    if(status == RxStatus_OK)
    {
        if(!count) {
            QThread::msleep(1);
            return;
        }

        m_receivedBlock.append(reinterpret_cast<char*>(buf.data()),m_sizeData);
        displayBlockData();
        emit sigReceivedData();
        m_isAllowErrorReading = true;
    }
    else if((status == RxStatus_CrcError)&&m_isAllowErrorReading) {
        strMessage = "<FONT COLOR=RED>Failed reading MG: CRC error";
        m_isAllowErrorReading = false;
    }
    else if((status == RxStatus_Abort)&&m_isAllowErrorReading) {
        strMessage = "<FONT COLOR=RED>Failed reading MG: receive idle or abort pattern";
        m_isAllowErrorReading = false;
    }
    else if((status == RxStatus_ShortFrame)&&m_isAllowErrorReading) {
        strMessage = "<FONT COLOR=RED>Failed reading MG: malformed or short frame";
        m_isAllowErrorReading = false;
    }
    else if((status == RxStatus_Cancel)&&m_isAllowErrorReading) {
        //strMessage = "<FONT COLOR=RED>Failed reading MG: no data or indication available";
        m_isAllowErrorReading = false;
    }
    else if(m_isAllowErrorReading){
        strMessage = "<FONT COLOR=RED>Failed reading MG: error code "+QString::number(status);
        m_isAllowErrorReading = false;
    }
}

void Port::sendToPort()
{
    if((!m_isConnectedMG && !m_isOpenedCOM) || !m_isConnected) {
        return;
    }

    bool isSent = false;

    m_blockToSend.clear();
    QDataStream in(&m_blockToSend,QIODevice::WriteOnly);

    if(m_modeTransfer == TEST_MES) {
        in.writeRawData((char*)&testMessage,sizeof (testMessage));
        isSent = sendData();

        if(isSent) {
//            emit sigSendMessage("Sent <FONT COLOR=GRAY>Test Message");
        }
    }
    else if(m_modeTransfer == PRIM_PLOT) {
        sendBlockPrimaryPlots();
    }
    else if(m_modeTransfer == AZIMUTH_REF) {
        in.writeRawData((char*)&azimuthMessage,sizeof (azimuthMessage));
        isSent = sendData();

        if(isSent) {
            emit sigSendMessage("Sent <FONT COLOR=Blue>Azimuth Reference Message");
        }
    }

    m_modeTransfer = NONE;
}

void Port::sendBlockPrimaryPlots()
{
    int n = 0;

    for(QList<QPair<float,float>*>::iterator iter = m_poinerListTarget.begin(); iter != m_poinerListTarget.end(); iter++) {
        m_blockToSend.clear();
        QDataStream in(&m_blockToSend,QIODevice::WriteOnly);

        if((*iter)->second > 300) {
            return;
        }

        plotMessage.bits.azimuth = (*iter)->first/0.088;
        plotMessage.bits.range = (*iter)->second/0.11575;

        in.writeRawData((char*)&plotMessage, sizeof (plotMessage));

        float isSent = sendData();
        n = isSent ? n + 1 : n;
        delete *iter;
        m_poinerListTarget.removeOne(*iter);
    }

    if(n != 0) {
        emit sigSendMessage("Sent <FONT COLOR=CYAN>Primary Plot Message " + QString::number(n) + " targets");
    }
}

void Port::slotReceiveBlockPrimaryPlots(QList<QPair<float, float> *> list)
{
    if((!m_isConnectedMG && !m_isOpenedCOM) || !m_isConnected)
    {
        for(QList<QPair<float,float>*>::iterator iter = list.begin(); iter != list.end(); iter++) {
            delete *iter;
        }

        return;
    }

    if(list.isEmpty()) {
        return;
    }

    m_poinerListTarget = list;
    m_modeTransfer = PRIM_PLOT;
}

void Port::slotSendAziuthReference(int count)
{
    if(!m_isConnectedMG) {
        return;
    }

    m_modeTransfer = AZIMUTH_REF;
    emit sigSendMessage("Received <FONT COLOR=GRAY>Test message " + QString::number(m_countReceivedTestMessages));
    m_countReceivedTestMessages = 0;

    Q_UNUSED(count);
}

void Port::displayBlockData()
{    
    QDataStream out(&m_receivedBlock, QIODevice::ReadOnly);

    char* ptr = m_receivedBlock.data();
    const unsigned short* bytes = reinterpret_cast<const unsigned short*>(ptr);
    reportID = getReportID(*bytes);

    switch (reportID) {
    case Sps::TEST:
//        emit sigSendMessage("Received <FONT COLOR=GRAY>Test message");
        m_countReceivedTestMessages++;

        out.readRawData((char*)&testMessage, sizeof (testMessage));
        testMessage.bits.RT = 4;
        m_modeTransfer = TEST_MES;

        if(!m_isConnected) {
            emit sigSetConnection(true);
            m_isConnected = true;
        }

        m_checkConnection = true;
        break;
    case Sps::AZIMUTH_REF:
        out.readRawData((char*)&azimuthMessage, sizeof (azimuthMessage));
        emit sigSendMessage("Received - azimuth");
        break;
    case Sps::PRIMARY_PLOT:
        if(m_receivedBlock.size() >= 8)
        {
            out.readRawData((char*)&plotMessage, sizeof (plotMessage));
            emit sigSendMessage("Received - plot: " + QString::number(plotMessage.bits.azimuth) + " range: "+
                                QString::number(plotMessage.bits.range));
        }

        break;
    default:
        break;
    }
    m_receivedBlock.clear();
}

Sps::ReportID Port::getReportID(unsigned short hdr)
{
    Sps::ReportID rid = Sps::ReportID(0x0F & (hdr >> 12));
    return rid;
}

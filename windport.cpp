#include "windport.h"
#include "ui_windport.h"

WindowPort::WindowPort(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindowSettings)
{
    ui->setupUi(this);

    m_isMicroGate=false;
    m_comPort=new Port;
    m_isOpenPort=false;
    m_isMicroGate=true;
    m_isReadRecord=false;
    m_isWriteRecord=false;

    m_windFile=new WindFile;

    QStringList listName;
    QVector<QString> vecName;
    QString message=microgate::getAvailablePorts(vecName);

    if(vecName.isEmpty()) {
        listName<<"None";
    }
    else {
        foreach (QString str, vecName) {
            listName<<str;
        }
    }

    ui->comboBoxPort->addItems(listName);
    emit sigSendMessage(message);

    m_settingsPort.nameMicro="None";
    m_settingsPort.baudRate=QSerialPort::Baud115200;
    m_settingsPort.dataBits=QSerialPort::Data8;
    m_settingsPort.parity=QSerialPort::EvenParity;
    m_settingsPort.stopBits=QSerialPort::OneStop;

    ui->comboBox->setCurrentIndex(1);
    ui->comboBoxBaud->setCurrentIndex(7);
    ui->comboBoxByte->setCurrentIndex(3);
    ui->comboBoxParity->setCurrentIndex(1);
    m_comPort->setSettings(m_settingsPort);

    QPixmap pix(":/images/Img/vspc-icon-256.png");
    QPixmap scalePix=pix.scaled(pix.width()/3,pix.height()/2,Qt::KeepAspectRatio);
    ui->labelIcon->setPixmap(scalePix);

    ui->comboBoxAz->setCurrentIndex(0);
    ui->comboBoxNor->setCurrentIndex(1);
    ui->comboBoxTar->setCurrentIndex(2);
    ui->comboBoxTr->setCurrentIndex(3);

    ui->spinBoxAz->setMaximum(10000);
    ui->spinBoxAz->setSingleStep(10);
    ui->spinBoxNor->setMaximum(10000);
    ui->spinBoxNor->setSingleStep(10);
    ui->spinBoxTar->setMaximum(10000);
    ui->spinBoxTar->setSingleStep(10);
    ui->spinBoxTr->setMaximum(10000);
    ui->spinBoxTr->setSingleStep(10);

    ui->spinBoxAz->setValue(4000);
    ui->spinBoxNor->setValue(4000);
    ui->spinBoxTar->setValue(3970);
    ui->spinBoxTr->setValue(4000);

    for(int i=0;i<8;i++) {
        m_settingsChan.valueThreshold[i]=4000;
    }

    m_settingsChan.nameChannel.insert(0,"ACD");
    m_settingsChan.nameChannel.insert(1,"ARP");
    m_settingsChan.nameChannel.insert(2,"Vidio");
    m_settingsChan.nameChannel.insert(3,"Trig");
    m_settingsChan.nameChannel.insert(4,"");
    m_settingsChan.nameChannel.insert(5,"");
    m_settingsChan.nameChannel.insert(6,"");
    m_settingsChan.nameChannel.insert(7,"");

    QPixmap pix2(":/images/Img/w256ConnectorPlug.png");
    QPixmap scalePix2=pix2.scaled(pix.width()/3,pix.height()/2,Qt::KeepAspectRatio);
    ui->labelIcon_2->setPixmap(scalePix2);

    ui->butOk->setFixedSize(100,30);
}

WindowPort::~WindowPort()
{
    delete ui;
}

void WindowPort::setSettingsPort()
{
    if(m_isMicroGate)
    {
        m_settingsPort.nameMicro=ui->comboBoxPort->currentText();
    }
    else
    {
        m_settingsPort.namePort=ui->comboBoxPort->currentText();
    }

    int baud=ui->comboBoxBaud->currentText().toInt();
    m_settingsPort.baudRate=(QSerialPort::BaudRate)baud;

    int bits=ui->comboBoxByte->currentText().toInt();
    m_settingsPort.dataBits=(QSerialPort::DataBits)bits;

    QString strValue=ui->comboBoxParity->currentText();

    if(strValue=="NoParity")
        m_settingsPort.parity=QSerialPort::NoParity;
    else if(strValue=="EvenParity")
        m_settingsPort.parity=QSerialPort::EvenParity;
    else if(strValue=="OddParity")
        m_settingsPort.parity=QSerialPort::OddParity;
    else if(strValue=="SpaceParity")
        m_settingsPort.parity=QSerialPort::SpaceParity;
    else if(strValue=="MarkParity")
        m_settingsPort.parity=QSerialPort::MarkParity;
    else if(strValue=="UnknownParity")
        m_settingsPort.parity=QSerialPort::UnknownParity;
    else
        m_settingsPort.parity=QSerialPort::EvenParity;

    strValue=ui->comboBoxBits->currentText();

    if(strValue=="OneStop") {
        m_settingsPort.stopBits=QSerialPort::OneStop;
        m_settingsPort.paramsMicrogate.StopBits=0;
    }
    else if(strValue=="OneAndHalfStop") {
        m_settingsPort.stopBits=QSerialPort::OneAndHalfStop;
    }
    else if(strValue=="TwoStop") {
        m_settingsPort.stopBits=QSerialPort::TwoStop;
    }
    else if(strValue=="UnknownStopBits") {
        m_settingsPort.stopBits=QSerialPort::UnknownStopBits;
    }
    else {
        m_settingsPort.stopBits=QSerialPort::OneStop;
    }

    m_settingsPort.paramsMicrogate.Mode = MGSL_MODE_ASYNC;
    m_settingsPort.paramsMicrogate.Loopback = 0;
    m_settingsPort.paramsMicrogate.Flags = 0;

    m_settingsPort.paramsMicrogate.DataRate=ui->comboBoxBaud->currentText().toUInt();
    m_settingsPort.paramsMicrogate.DataBits=ui->comboBoxByte->currentText().toUInt();
    m_settingsPort.paramsMicrogate.StopBits=m_settingsPort.stopBits;
    int parity=ui->comboBoxParity->currentIndex();
    m_settingsPort.paramsMicrogate.Parity=parity+4;
}

bool WindowPort::getIsMicrogate() const
{
    return m_isMicroGate;
}

void WindowPort::connectPort()
{
    bool isSuccess;

    setSettingsPort();
    m_comPort->setSettings(m_settingsPort);

    if(m_isMicroGate)
    {
        if(!m_isOpenPort)
        {
            isSuccess=m_comPort->openMGPort();
        }
        else
        {
            isSuccess=m_comPort->closeMGPort();
        }
    }
    else
    {
        if(!m_isOpenPort)
        {
            isSuccess=m_comPort->openCOMPort();
        }
        else
        {
            isSuccess=m_comPort->closeCOMPort();
        }
    }

    if(isSuccess)
    {
        if(m_isOpenPort==true) m_isOpenPort=false;
        else m_isOpenPort=true;
    }
}

void WindowPort::on_comboBox_activated(int index)
{
    bool isSuccess;

    if(m_isOpenPort) {
        isSuccess=m_comPort->closeCOMPort();

        if(isSuccess) {
            m_isOpenPort=false;
        }
    }

    m_isMicroGate=index;
    ui->comboBoxPort->clear();

    QStringList listName;

    if(m_isMicroGate)
    {
        QVector<QString> vecName;

        QString message=microgate::getAvailablePorts(vecName);

        if(vecName.isEmpty()) {
            listName<<"None";
        }
        else {
            foreach (QString str, vecName) {
                listName<<str;
            }
        }

        ui->comboBoxPort->addItems(listName);
        emit sigSendMessage(message);
    }
    else
    {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            ui->comboBoxPort->addItem(info.portName());
        }

        if(QSerialPortInfo::availablePorts().isEmpty()) {
            ui->comboBoxPort->addItem("None");
        }
    }
}

void WindowPort::on_comboBoxPort_activated(int index)
{
    QString message;

    if(m_isOpenPort) {
        message=m_comPort->closeCOMPort();

        if(message=="Port closed") {
            m_isOpenPort=false;
        }
    }
}

SettingsChannel WindowPort::getSettingsChannel()
{
    clearNamesChannel();

    QString strValue=ui->comboBoxAz->currentText();
    strValue.remove(0,8);
    m_settingsChan.azimuth=(strValue.toInt()-1);
    m_settingsChan.nameChannel[m_settingsChan.azimuth]="ACD";

    strValue=ui->comboBoxNor->currentText();
    strValue.remove(0,8);
    m_settingsChan.north=(strValue.toInt()-1);
    m_settingsChan.nameChannel[m_settingsChan.north]="ARP";

    strValue=ui->comboBoxTar->currentText();
    strValue.remove(0,8);
    m_settingsChan.target=(strValue.toInt()-1);
    m_settingsChan.nameChannel[m_settingsChan.target]="Vidio";

    strValue=ui->comboBoxTr->currentText();
    strValue.remove(0,8);
    m_settingsChan.trigger=(strValue.toInt()-1);
    m_settingsChan.nameChannel[m_settingsChan.trigger]="Trig";

    for(int i=0;i<8;i++)
    {
        if((i==m_settingsChan.azimuth)||(i==m_settingsChan.north)||(i==m_settingsChan.target)||(i==m_settingsChan.trigger))
        {
            if(i==m_settingsChan.azimuth) m_settingsChan.valueThreshold[i]=ui->spinBoxAz->value();
            else if(i==m_settingsChan.north) m_settingsChan.valueThreshold[i]=ui->spinBoxNor->value();
            else if(i==m_settingsChan.target) m_settingsChan.valueThreshold[i]=ui->spinBoxTar->value();
            else if(i==m_settingsChan.trigger) m_settingsChan.valueThreshold[i]=ui->spinBoxTr->value();
        }
    }

    return m_settingsChan;
}

void WindowPort::changeThreshold(int num, int value)
{
    m_settingsChan.valueThreshold[num]+=value;

    if((num==m_settingsChan.azimuth)||(num==m_settingsChan.north)||(num==m_settingsChan.target)||(num==m_settingsChan.trigger))
    {
        if(num==m_settingsChan.azimuth) ui->spinBoxAz->setValue(m_settingsChan.valueThreshold[num]);
        else if(num==m_settingsChan.north) ui->spinBoxNor->setValue(m_settingsChan.valueThreshold[num]);
        else if(num==m_settingsChan.target) ui->spinBoxTar->setValue(m_settingsChan.valueThreshold[num]);
        else if(num==m_settingsChan.trigger) ui->spinBoxTr->setValue(m_settingsChan.valueThreshold[num]);
    }
}

NameChannel WindowPort::getNameChan(int num)
{
    //QString name;
    NameChannel name;

//    if(num==m_settingsChan.azimuth) name="Az";
//    else if(num==m_settingsChan.north) name="Nor";
//    else if(num==m_settingsChan.target) name="Targ";
//    else if(num==m_settingsChan.trigger) name="Trig";
//    else name="";
    if(num==m_settingsChan.azimuth) name=ACD;
    else if(num==m_settingsChan.north) name=ARP;
    else if(num==m_settingsChan.target) name=VIDIO;
    else if(num==m_settingsChan.trigger) name=TRIGGER;
    else name=OTHER;

    return name;
}

int WindowPort::getThreshold(int num)
{
    return m_settingsChan.valueThreshold[num];
}

QString WindowPort::getPathRead() const
{
    return m_windFile->getPathRead();
}

QString WindowPort::getPathWrite() const
{
    return m_windFile->getPathWrite();
}

bool WindowPort::getIsWrite() const
{
    return m_isWriteRecord;
}

bool WindowPort::getIsRead() const
{
    return m_isReadRecord;
}

void WindowPort::on_butPathWrite_clicked()
{
    m_windFile->setIsSetRead(false);
    m_windFile->setWindowTitle("File path");
    m_windFile->setModal(true);
    m_windFile->exec();

    if(m_windFile->m_isWrite==true) {
        m_isWriteRecord=true;
        ui->comboBoxCreateRecord->setCurrentIndex(1);
    }
}

void WindowPort::on_butPathRead_clicked()
{
    m_windFile->setIsSetRead(true);
    m_windFile->setWindowTitle("File path");
    m_windFile->setModal(true);
    m_windFile->exec();

    if(m_windFile->m_isRead==true) {
        m_isReadRecord=true;
        ui->comboBoxMode->setCurrentIndex(1);
    }
}

void WindowPort::on_comboBoxMode_activated(const QString &arg1)
{
    if(arg1=="File")
    {
        m_isReadRecord=true;
    }
    else if(arg1=="Board")
    {
        m_isReadRecord=false;
    }
}

void WindowPort::on_comboBoxCreateRecord_activated(const QString &arg1)
{
    if(arg1=="None")
    {
        m_isWriteRecord=true;
    }
    else if(arg1=="Write")
    {
        m_isWriteRecord=false;
    }
}

void WindowPort::clearNamesChannel()
{
    m_settingsChan.nameChannel[0]="";
    m_settingsChan.nameChannel[1]="";
    m_settingsChan.nameChannel[2]="";
    m_settingsChan.nameChannel[3]="";
    m_settingsChan.nameChannel[4]="";
    m_settingsChan.nameChannel[5]="";
    m_settingsChan.nameChannel[6]="";
    m_settingsChan.nameChannel[7]="";
}

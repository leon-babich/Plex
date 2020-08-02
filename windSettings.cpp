#include "windSettings.h"
#include "ui_windport.h"

WindowSettings::WindowSettings(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WindowSettings)
{
    ui->setupUi(this);

    m_isMicroGate = false;
    m_port = new Port;
    m_isOpenPort = false;
    m_isMicroGate = true;
    m_isReadRecord = false;
    m_isWriteRecord = false;
    m_isCreateMask = false;
    m_sizeMask = 0;
    m_isChangeMask = false;
    m_numMask = 0;
    m_isClearOldData = false;
    m_isProtectCheckSig = true;

    m_windFile = new WindFile;

    QStringList listName;
    QVector<QString> vecName;
    QString message = microgate::getAvailablePorts(vecName);
    emit sigNumFoundPorts(vecName.size());

    if(vecName.isEmpty()) {
        listName << "None";
    }
    else {
        foreach (QString str, vecName) {
            listName << str;
        }
    }

    ui->comboBoxPort->addItems(listName);
    emit sigSendMessage(message);

    m_settingsPort.nameMicro = "None";
    m_settingsPort.baudRate = QSerialPort::Baud115200;
    m_settingsPort.dataBits = QSerialPort::Data8;
    m_settingsPort.parity = QSerialPort::EvenParity;
    m_settingsPort.stopBits = QSerialPort::OneStop;

    ui->comboBox->setCurrentIndex(1);
    ui->comboBoxBaud->setCurrentIndex(7);
    ui->comboBoxByte->setCurrentIndex(3);
    ui->comboBoxParity->setCurrentIndex(1);
    m_port->setSettings(m_settingsPort);

    QPixmap pix(":/images/Img/vspc-icon-256.png");
    QPixmap scalePix = pix.scaled(pix.width()/3, pix.height()/2, Qt::KeepAspectRatio);
    ui->labelIcon->setPixmap(scalePix);

    ui->comboBoxAz->setCurrentIndex(0);
    ui->comboBoxNor->setCurrentIndex(1);
    ui->comboBoxTar->setCurrentIndex(2);
    ui->comboBoxTr->setCurrentIndex(3);

    ui->spinBoxAz->setMaximum(20000);
    ui->spinBoxAz->setSingleStep(10);
    ui->spinBoxNor->setMaximum(20000);
    ui->spinBoxNor->setSingleStep(10);
    ui->spinBoxTar->setMaximum(20000);
    ui->spinBoxTar->setSingleStep(10);
    ui->spinBoxTr->setMaximum(20000);
    ui->spinBoxTr->setSingleStep(10);

    ui->spinBoxAz->setValue(4000);
    ui->spinBoxNor->setValue(4000);
    ui->spinBoxTar->setValue(3970);
    ui->spinBoxTr->setValue(4000);

    ui->spinBoxAz->setEnabled(false);
    ui->spinBoxNor->setEnabled(false);
    ui->spinBoxTar->setEnabled(false);
    ui->spinBoxTr->setEnabled(false);

    ui->spinCoefficACP->setMaximum(100);
    ui->spinCoefficARP->setMaximum(100);
    ui->spinCoefficVideo->setMaximum(100);
    ui->spinCoefficTrigger->setMaximum(100);
    ui->spinCoefficACP->setSingleStep(10);
    ui->spinCoefficARP->setSingleStep(10);
    ui->spinCoefficVideo->setSingleStep(1);
    ui->spinCoefficTrigger->setSingleStep(10);

    ui->spinCoefficACP->setValue(50);
    ui->spinCoefficARP->setValue(50);
    ui->spinCoefficVideo->setValue(50);
    ui->spinCoefficTrigger->setValue(50);

    ui->spinBoxFiltreNoise->setMaximum(100);
    ui->spinBoxFiltreNoise->setValue(100);
    ui->spinBoxStAngle->setMaximum(359);
    ui->spinBoxStAngle->setValue(0);

    ui->butAutoACP->setCheckable(true);
    ui->butAutoARP->setCheckable(true);
    ui->butAutoVidio->setCheckable(true);
    ui->butAutoTrig->setCheckable(true);
    ui->butReduceS->setCheckable(true);
    ui->butReduceR->setCheckable(true);

    ui->butAutoACP->setChecked(true);
    ui->butAutoARP->setChecked(true);
    ui->butAutoVidio->setChecked(true);
    ui->butAutoTrig->setChecked(true);
    ui->butReduceR->setChecked(true);

    QPixmap pix2(":/images/Img/w256ConnectorPlug.png");
    QPixmap scalePix2 = pix2.scaled(pix.width()/3,pix.height()/2, Qt::KeepAspectRatio);
    ui->labelIcon_2->setPixmap(scalePix2);

    ui->butOk->setFixedSize(100,30);

    ui->labAz->hide();
    ui->labAz_2->hide();
    ui->linAz->hide();
    ui->linAz_2->hide();
    ui->labRang->hide();
    ui->labRang_2->hide();
    ui->linRang->hide();
    ui->linRang_2->hide();
    ui->butCancelMask->hide();
    ui->butDeleteMask->hide();
    ui->textEditMask->hide();

    ui->labR1->hide();
    ui->labR2->hide();
    ui->labS1->hide();
    ui->labS2->hide();
    ui->labS3->hide();
    ui->labVidR->hide();
    ui->labTrigR->hide();
    ui->comboBoxR1->hide();
    ui->comboBoxR2->hide();
    ui->comboBoxS1->hide();
    ui->comboBoxS2->hide();
    ui->comboBoxS3->hide();
    ui->comboBoxTarR->hide();
    ui->comboBoxTrR->hide();
    ui->comboBoxR1->setCurrentIndex(0);
    ui->comboBoxR2->setCurrentIndex(1);
    ui->comboBoxS1->setCurrentIndex(2);
    ui->comboBoxS2->setCurrentIndex(3);
    ui->comboBoxS3->setCurrentIndex(4);
    ui->comboBoxTarR->setCurrentIndex(5);
    ui->comboBoxTrR->setCurrentIndex(6);

    m_timerCountTimeSettings = new QTimer;
    connect(m_timerCountTimeSettings,&QTimer::timeout,this,&WindowSettings::slotSetIsClearOldData);

    connect(m_windFile,&WindFile::sigIsWriteTar,this,&WindowSettings::slotIsWriteTar);
}

WindowSettings::~WindowSettings()
{
    delete ui;
}

void WindowSettings::setSettingsPort()
{
    if(m_isMicroGate) {
        m_settingsPort.nameMicro = ui->comboBoxPort->currentText();
    }
    else {
        m_settingsPort.namePort = ui->comboBoxPort->currentText();
    }

    int baud = ui->comboBoxBaud->currentText().toInt();
    m_settingsPort.baudRate = (QSerialPort::BaudRate)baud;

    int bits = ui->comboBoxByte->currentText().toInt();
    m_settingsPort.dataBits = (QSerialPort::DataBits)bits;

    QString strValue = ui->comboBoxParity->currentText();

    if(strValue == "NoParity")
        m_settingsPort.parity = QSerialPort::NoParity;
    else if(strValue == "EvenParity")
        m_settingsPort.parity = QSerialPort::EvenParity;
    else if(strValue == "OddParity")
        m_settingsPort.parity = QSerialPort::OddParity;
    else if(strValue == "SpaceParity")
        m_settingsPort.parity = QSerialPort::SpaceParity;
    else if(strValue == "MarkParity")
        m_settingsPort.parity = QSerialPort::MarkParity;
    else if(strValue == "UnknownParity")
        m_settingsPort.parity = QSerialPort::UnknownParity;
    else
        m_settingsPort.parity = QSerialPort::EvenParity;

    strValue = ui->comboBoxBits->currentText();

    if(strValue == "OneStop") {
        m_settingsPort.stopBits = QSerialPort::OneStop;
        m_settingsPort.paramsMicrogate.StopBits=0;
    }
    else if(strValue == "OneAndHalfStop") {
        m_settingsPort.stopBits = QSerialPort::OneAndHalfStop;
    }
    else if(strValue == "TwoStop") {
        m_settingsPort.stopBits = QSerialPort::TwoStop;
    }
    else if(strValue == "UnknownStopBits") {
        m_settingsPort.stopBits = QSerialPort::UnknownStopBits;
    }
    else {
        m_settingsPort.stopBits = QSerialPort::OneStop;
    }

    m_settingsPort.paramsMicrogate.Mode = MGSL_MODE_ASYNC;
    m_settingsPort.paramsMicrogate.Loopback = 0;
    m_settingsPort.paramsMicrogate.Flags = 0;

    m_settingsPort.paramsMicrogate.DataRate=ui->comboBoxBaud->currentText().toUInt();
    m_settingsPort.paramsMicrogate.DataBits=ui->comboBoxByte->currentText().toUInt();
    m_settingsPort.paramsMicrogate.StopBits=m_settingsPort.stopBits;
    int parity=ui->comboBoxParity->currentIndex();
    m_settingsPort.paramsMicrogate.Parity = parity + 4;
}

bool WindowSettings::getIsMicrogate() const
{
    return m_isMicroGate;
}

void WindowSettings::connectPort()
{
    bool isSuccess;

    setSettingsPort();
    m_port->setSettings(m_settingsPort);

    if(m_isMicroGate) {
        if(!m_isOpenPort) {
            isSuccess = m_port->openMGPort();
        }
        else {
            isSuccess = m_port->closeMGPort();
        }
    }
    else {
        if(!m_isOpenPort) {
            isSuccess = m_port->openCOMPort();
        }
        else {
            isSuccess = m_port->closeCOMPort();
        }
    }

    if(isSuccess) {
        if(m_isOpenPort == true) m_isOpenPort = false;
        else m_isOpenPort = true;

        if(!m_port->m_timerStopConnection->isActive()) {
            m_port->m_timerStopConnection->start(2000);
        }
    }
}

void WindowSettings::on_comboBox_activated(int index)
{
    bool isSuccess;

    if(m_isOpenPort) {
        isSuccess = m_port->closeCOMPort();

        if(isSuccess) {
            m_isOpenPort = false;
        }
    }

    m_isMicroGate = index;
    ui->comboBoxPort->clear();

    QStringList listName;

    if(m_isMicroGate) {
        QVector<QString> vecName;
        QString message = microgate::getAvailablePorts(vecName);
        emit sigNumFoundPorts(vecName.size());

        if(vecName.isEmpty()) {
            listName<<"None";
        }
        else {
            foreach (QString str, vecName) {
                listName << str;
            }
        }

        ui->comboBoxPort->addItems(listName);
        emit sigSendMessage(message);
    }
    else {
        foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
            ui->comboBoxPort->addItem(info.portName());
        }

        if(QSerialPortInfo::availablePorts().isEmpty()) {
            ui->comboBoxPort->addItem("None");
        }
    }
}

void WindowSettings::on_comboBoxPort_activated(int index)
{
    QString message;

    if(m_isOpenPort) {
        message = m_port->closeCOMPort();

        if(message == "Port closed") {
            m_isOpenPort = false;
        }
    }
}

void WindowSettings::changeSettingsChannel()
{
    clearNamesChannel();

    QString strValue = ui->comboBoxAz->currentText();
    strValue.remove(0,8);
    m_pSettingsChannel->azimuth = strValue.toInt() - 1;
    m_pSettingsChannel->nameChannel[m_pSettingsChannel->azimuth] = "ACD";

    strValue = ui->comboBoxNor->currentText();
    strValue.remove(0,8);
    m_pSettingsChannel->north = strValue.toInt() - 1;
    m_pSettingsChannel->nameChannel[m_pSettingsChannel->north] = "ARP";

    strValue = ui->comboBoxTar->currentText();
    strValue.remove(0,8);
    m_pSettingsChannel->target = strValue.toInt() - 1;
    m_pSettingsChannel->nameChannel[m_pSettingsChannel->target] = "Vidio";

    strValue = ui->comboBoxTr->currentText();
    strValue.remove(0,8);
    m_pSettingsChannel->trigger = strValue.toInt() - 1;
    m_pSettingsChannel->nameChannel[m_pSettingsChannel->trigger] = "Trig";

    if(ui->comboBoxTypeRadar->currentIndex()==1) {
        strValue = ui->comboBoxR1->currentText();
        strValue.remove(0,8);
        m_pSettingsChannel->numberR18Channel[R1] = strValue.toInt() - 1;

        strValue = ui->comboBoxR2->currentText();
        strValue.remove(0,8);
        m_pSettingsChannel->numberR18Channel[R2] = strValue.toInt() - 1;

        strValue = ui->comboBoxS1->currentText();
        strValue.remove(0,8);
        m_pSettingsChannel->numberR18Channel[S1] = strValue.toInt() - 1;

        strValue = ui->comboBoxS2->currentText();
        strValue.remove(0,8);
        m_pSettingsChannel->numberR18Channel[S2] = strValue.toInt() - 1;

        strValue = ui->comboBoxS3->currentText();
        strValue.remove(0,8);
        m_pSettingsChannel->numberR18Channel[S3] = strValue.toInt() - 1;

        strValue = ui->comboBoxTarR->currentText();
        strValue.remove(0,8);
        m_pSettingsChannel->numberR18Channel[VIDEO_R] = strValue.toInt() - 1;

        strValue = ui->comboBoxTrR->currentText();
        strValue.remove(0,8);
        m_pSettingsChannel->numberR18Channel[TRIGGER_R] = strValue.toInt() - 1;
    }

    for(int i = 0;i < 8; i++)
    {
        if((i == m_pSettingsChannel->azimuth) || (i == m_pSettingsChannel->north) || (i == m_pSettingsChannel->target) ||
                (i == m_pSettingsChannel->trigger))
        {
            if(i == m_pSettingsChannel->azimuth && !pAutoValue[0]) m_pSettingsChannel->valueThreshold[i] = ui->spinBoxAz->value();
            else if(i == m_pSettingsChannel->north && !pAutoValue[1]) m_pSettingsChannel->valueThreshold[i] = ui->spinBoxNor->value();
            else if(i == m_pSettingsChannel->target && !pAutoValue[2]) m_pSettingsChannel->valueThreshold[i] = ui->spinBoxTar->value();
            else if(i == m_pSettingsChannel->trigger && !pAutoValue[3]) m_pSettingsChannel->valueThreshold[i] = ui->spinBoxTr->value();
        }
    }

    m_pSettingsChannel->filter[0] = ui->spinCoefficACP->value();
    m_pSettingsChannel->filter[1] = ui->spinCoefficARP->value();
    m_pSettingsChannel->filter[2] = ui->spinCoefficVideo->value();
    m_pSettingsChannel->filter[3] = ui->spinCoefficTrigger->value();

    saveChannels();
}

void WindowSettings::saveChannels()
{
    QFile fileOut("settings.txt");
    QTextStream stream(&fileOut);
    QString strPath;

    if(fileOut.exists()) {
        if(!fileOut.open(QIODevice::ReadOnly)) {
            sigSendMessage("<FONT COLOR=RED>Fail opening settings file.");
            return;
        }

        QString str;
        int n = 0;
        stream >> strPath;

//    while(!fileOut.atEnd()) {
//        str = fileOut.readLine();
//        n++;
//        strPath = n == 1 ? strPath + str : strPath;
//    }

        fileOut.close();
    }
    else {
        strPath = "WriteData.bin";
    }

    QFile fileIn("settings.txt");
//    QTextStream stream(&fileIn);
    stream.setDevice(&fileIn);

    if(!fileIn.open(QIODevice::WriteOnly | QIODevice::Text)) {
        sigSendMessage("<FONT COLOR=RED>Fail opening settings file.");
        return;
    }

    QString strChannels = QString::number(m_pSettingsChannel->numberR18Channel[R1])+" "+QString::number(m_pSettingsChannel->numberR18Channel[R2])
            +" "+QString::number(m_pSettingsChannel->numberR18Channel[S1])+" "+QString::number(m_pSettingsChannel->numberR18Channel[S2])
            +" "+QString::number(m_pSettingsChannel->numberR18Channel[S3])+" "+QString::number(m_pSettingsChannel->numberR18Channel[VIDEO_R])+" "+
            QString::number(m_pSettingsChannel->numberR18Channel[TRIGGER_R]);
    /*if(n >= 1)*/ stream << strPath << endl << strChannels;
    fileIn.close();
}

QString WindowSettings::getPathRead() const
{
    return m_windFile->getPathRead();
}

QString WindowSettings::getPathWrite() const
{
    return m_windFile->getPathWrite();
}

QString WindowSettings::getPathWriteTar() const
{
    return m_windFile->pathWriteTar;
}

bool WindowSettings::getIsWriteRec() const
{
    return m_isWriteRecord;
}

bool WindowSettings::getIsReadRec() const
{
    return m_isReadRecord;
}

bool WindowSettings::getIsWritingTar() const
{
    return ui->comboBoxTarRec->currentIndex();
}

void WindowSettings::setSettingsChannel(SettingsChannel *settings)
{
    m_pSettingsChannel = settings;
}

void WindowSettings::setMode(int mode)
{
    ui->comboBoxMode->setCurrentIndex(mode);
    m_isReadRecord = mode;
}

void WindowSettings::setTypeRadar()
{
    int t = ui->comboBoxTypeRadar->currentIndex();
    if(t == 0) {
        ui->comboBoxTypeRadar->setCurrentIndex(1);
    }
    else if(t == 1) {
        ui->comboBoxTypeRadar->setCurrentIndex(0);
    }
}

void WindowSettings::setIsClearOldData(bool clearing)
{
    m_isClearOldData = clearing;

    if(!clearing && m_timerCountTimeSettings->isActive()) {
        m_timerCountTimeSettings->stop();
    }
}

void WindowSettings::setNumberChannels(int number[])
{
    ui->comboBoxR1->setCurrentIndex(number[R1]);
    ui->comboBoxR2->setCurrentIndex(number[R2]);
    ui->comboBoxS1->setCurrentIndex(number[S1]);
    ui->comboBoxS2->setCurrentIndex(number[S2]);
    ui->comboBoxS3->setCurrentIndex(number[S3]);
    ui->comboBoxTarR->setCurrentIndex(number[VIDEO_R]);
    ui->comboBoxTrR->setCurrentIndex(number[TRIGGER_R]);
}

bool WindowSettings::getIsChangedPath()
{
    return m_windFile->getIsChangedPath();
}

bool WindowSettings::getIsClearOldData()
{
    return m_isClearOldData;
}

bool WindowSettings::getIsProtectCheckSig()
{
    bool isProtecting = (ui->comboBoxProtect->currentIndex() + 1) % 2;
    return isProtecting;
}

int WindowSettings::getStartAngle()
{
    return ui->spinBoxStAngle->value();
}

int WindowSettings::getNoiseFilter()
{
    return ui->spinBoxFiltreNoise->value();
}

TypeRadar WindowSettings::getTypeRadar()
{
    return (TypeRadar)ui->comboBoxTypeRadar->currentIndex();
}

QList<RadarMask *> &WindowSettings::getListMask()
{
    return m_listMask;
}

bool WindowSettings::getIsReducedSignalS()
{
    return ui->butReduceS->isChecked();
}

bool WindowSettings::getIsReducedSignalR()
{
    return ui->butReduceR->isChecked();
}

void WindowSettings::checkCurrentWindow()
{
    int index = ui->tabWidget->currentIndex();

    if(index == 1) {
        m_timerCountTimeSettings->start(2500);
    }
}

void WindowSettings::on_butPathWrite_clicked()
{
    m_windFile->setIsSetRead(WRITE_REC);
    m_windFile->setWindowTitle("File path for writing");
    m_windFile->setModal(true);
    m_windFile->exec();

    if(m_windFile->isWriteData == true) {
        m_isWriteRecord = true;
        ui->comboBoxCreateRecord->setCurrentIndex(1);
    }
}

void WindowSettings::on_butPathRead_clicked()
{
    m_windFile->setIsSetRead(READ_REC);
    m_windFile->setWindowTitle("File path for reading");
    m_windFile->setModal(true);
    m_windFile->exec();

    if(m_windFile->isReadData == true) {
        m_isReadRecord = true;
        ui->comboBoxMode->setCurrentIndex(1);
        emit sigSetComBoxMode(1);
    }
}

void WindowSettings::on_butPathTarRec_clicked()
{
    m_windFile->setIsSetRead(WRITE_REC_TAR);
    m_windFile->setWindowTitle("File path for writing of targets");
    m_windFile->setModal(true);
    m_windFile->exec();
}

void WindowSettings::on_comboBoxMode_activated(const QString &arg1)
{
    if(arg1 == "File")
    {
        m_isReadRecord = true;
        emit sigSetComBoxMode(1);
    }
    else if(arg1 == "Board")
    {
        m_isReadRecord = false;
        emit sigSetComBoxMode(0);
    }
}

void WindowSettings::on_comboBoxCreateRecord_activated(const QString &arg1)
{
    if(arg1 == "None") {
        m_isWriteRecord = true;
    }
    else if(arg1 == "Write") {
        m_isWriteRecord = false;
    }
}

void WindowSettings::clearNamesChannel()
{
    m_pSettingsChannel->nameChannel[0] = "";
    m_pSettingsChannel->nameChannel[1] = "";
    m_pSettingsChannel->nameChannel[2] = "";
    m_pSettingsChannel->nameChannel[3] = "";
    m_pSettingsChannel->nameChannel[4] = "";
    m_pSettingsChannel->nameChannel[5] = "";
    m_pSettingsChannel->nameChannel[6] = "";
    m_pSettingsChannel->nameChannel[7] = "";
}

void WindowSettings::on_butAutoACP_clicked(bool checked)
{
    pAutoValue[0] = checked;
    ui->spinBoxAz->setEnabled(!checked);
    ui->spinCoefficACP->setEnabled(checked);
}

void WindowSettings::on_butAutoARP_clicked(bool checked)
{
    pAutoValue[1] = checked;
    ui->spinBoxNor->setEnabled(!checked);
    ui->spinCoefficARP->setEnabled(checked);
}

void WindowSettings::on_butAutoVidio_clicked(bool checked)
{
    pAutoValue[2] = checked;
    ui->spinBoxTar->setEnabled(!checked);
    ui->spinCoefficVideo->setEnabled(checked);
}

void WindowSettings::on_butAutoTrig_clicked(bool checked)
{
    pAutoValue[3] = checked;
    ui->spinBoxTr->setEnabled(!checked);
    ui->spinCoefficTrigger->setEnabled(checked);
}

void WindowSettings::on_butAddMask_clicked()
{
    if(!m_isCreateMask && !m_isChangeMask) {
        ui->labAz->show();
        ui->labAz_2->show();
        ui->linAz->show();
        ui->linAz_2->show();
        ui->labRang->show();
        ui->labRang_2->show();
        ui->linRang->show();
        ui->linRang_2->show();
        ui->butCancelMask->show();
        ui->butAddMask->setText("Add");
        m_isCreateMask = true;
    }
    else if(m_isCreateMask){
        float az[2];
        float rang[2];

        az[0] = ui->linAz->text().toInt();
        az[1] = ui->linAz_2->text().toInt();
        rang[0] = ui->linRang->text().toInt();
        rang[1] = ui->linRang_2->text().toInt();

        RadarMask *radarMask = new RadarMask;

        for(int i=0; i<2; i++) {
            radarMask->az[i] = az[i];
            radarMask->rang[i] = rang[i];
        }

        m_listMask.append(radarMask);

        emit sigAddMask();

        m_sizeMask++;
        QListWidgetItem *item = new QListWidgetItem("Radar mask " + QString::number(m_sizeMask), ui->listMask);

        ui->labAz->hide();
        ui->labAz_2->hide();
        ui->linAz->hide();
        ui->linAz_2->hide();
        ui->labRang->hide();
        ui->labRang_2->hide();
        ui->linRang->hide();
        ui->linRang_2->hide();
        ui->butCancelMask->hide();
        ui->butAddMask->setText("New");
        ui->linAz->clear();
        ui->linAz_2->clear();
        ui->linRang->clear();
        ui->linRang_2->clear();
        m_isCreateMask = false;
    }
    else {
        int i = 0;

        for(QList<RadarMask*>::iterator iter=m_listMask.begin(); iter!=m_listMask.end(); iter++) {
            if(i == m_numMask) {
                (*iter)->az[0] = ui->linAz->text().toInt();
                (*iter)->az[1] = ui->linAz_2->text().toInt();
                (*iter)->rang[0] = ui->linRang->text().toInt();
                (*iter)->rang[1] = ui->linRang_2->text().toInt();
                break;
            }

            i++;
        }

        ui->labAz->hide();
        ui->labAz_2->hide();
        ui->linAz->hide();
        ui->linAz_2->hide();
        ui->labRang->hide();
        ui->labRang_2->hide();
        ui->linRang->hide();
        ui->linRang_2->hide();
        ui->butCancelMask->hide();
        ui->butAddMask->setText("New");
        ui->linAz->clear();
        ui->linAz_2->clear();
        ui->linRang->clear();
        ui->linRang_2->clear();
        m_isChangeMask = false;

        emit sigAddMask();
    }
}

void WindowSettings::on_butCancelMask_clicked()
{
    ui->labAz->hide();
    ui->labAz_2->hide();
    ui->linAz->hide();
    ui->linAz_2->hide();
    ui->labRang->hide();
    ui->labRang_2->hide();
    ui->linRang->hide();
    ui->linRang_2->hide();
    ui->butCancelMask->hide();
    ui->butAddMask->setText("New");
    m_isCreateMask = false;
    ui->linAz->clear();
    ui->linAz_2->clear();
    ui->linRang->clear();
    ui->linRang_2->clear();
}

void WindowSettings::on_listMask_doubleClicked(const QModelIndex &index)
{
    int i = 0;

    for(QList<RadarMask*>::iterator iter=m_listMask.begin(); iter!=m_listMask.end(); iter++) {
        if(i == index.row()) {
            ui->linAz->setText(QString::number((*iter)->az[0]));
            ui->linAz_2->setText(QString::number((*iter)->az[1]));
            ui->linRang->setText(QString::number((*iter)->rang[0]));
            ui->linRang_2->setText(QString::number((*iter)->rang[1]));
            ui->butDeleteMask->hide();
            ui->labAz->show();
            ui->labAz_2->show();
            ui->linAz->show();
            ui->linAz_2->show();
            ui->labRang->show();
            ui->labRang_2->show();
            ui->linRang->show();
            ui->linRang_2->show();
            ui->butCancelMask->show();
            ui->butAddMask->setText("Change");
            m_isChangeMask = true;
            m_numMask = i;
            break;
        }

        i++;
    }
}

void WindowSettings::on_listMask_clicked(const QModelIndex &index)
{
    int i = 0;

    for(QList<RadarMask*>::iterator iter=m_listMask.begin(); iter!=m_listMask.end(); iter++) {
        if(i == index.row()) {
            m_numMask = i;
            ui->butDeleteMask->show();
            break;
        }

        i++;
    }
}

void WindowSettings::on_butDeleteMask_clicked()
{
    int i = 0;

    for(QList<RadarMask*>::iterator iter=m_listMask.begin(); iter!=m_listMask.end(); iter++) {
        if(i == m_numMask) {
            m_listMask.removeOne(*iter);
            delete *iter;
            delete ui->listMask->takeItem(i);
            emit sigAddMask();
            break;
        }

        i++;
    }
}

void WindowSettings::on_comboBoxButLay_activated(int index)
{
    emit sigSetButLayout((ModeLocationButtons)index);
}

void WindowSettings::on_tabWidget_currentChanged(int index)
{
    if(index == 1) {
        m_timerCountTimeSettings->start(2500);
    }
    else if(m_timerCountTimeSettings->isActive()){
        m_timerCountTimeSettings->stop();
    }
}

void WindowSettings::slotSetIsClearOldData()
{
    setIsClearOldData(true);
    m_timerCountTimeSettings->stop();
}

void WindowSettings::slotIsWriteTar()
{
    ui->comboBoxTarRec->setCurrentIndex(1);
}

void WindowSettings::on_comboBoxTypeRadar_activated(int index)
{
    if(index) {
        ui->labACP->hide();
        ui->labARP->hide();
        ui->labVid->hide();
        ui->labTrig->hide();
        ui->comboBoxAz->hide();
        ui->comboBoxNor->hide();
        ui->comboBoxTar->hide();
        ui->comboBoxTr->hide();
        ui->spinBoxAz->hide();
        ui->spinBoxNor->hide();
        ui->spinBoxTar->hide();
        ui->spinBoxTr->hide();
        ui->spinCoefficACP->hide();
        ui->spinCoefficARP->hide();
        ui->spinCoefficVideo->hide();
        ui->spinCoefficTrigger->hide();
        ui->butAutoACP->hide();
        ui->butAutoARP->hide();
        ui->butAutoVidio->hide();
        ui->butAutoTrig->hide();

        ui->labR1->show();
        ui->labR2->show();
        ui->labS1->show();
        ui->labS2->show();
        ui->labS3->show();
        ui->labVidR->show();
        ui->labTrigR->show();
        ui->comboBoxR1->show();
        ui->comboBoxR2->show();
        ui->comboBoxS1->show();
        ui->comboBoxS2->show();
        ui->comboBoxS3->show();
        ui->comboBoxTarR->show();
        ui->comboBoxTrR->show();
    }
    else {
        ui->labACP->show();
        ui->labARP->show();
        ui->labVid->show();
        ui->labTrig->show();
        ui->comboBoxAz->show();
        ui->comboBoxNor->show();
        ui->comboBoxTar->show();
        ui->comboBoxTr->show();
        ui->spinBoxAz->show();
        ui->spinBoxNor->show();
        ui->spinBoxTar->show();
        ui->spinBoxTr->show();
        ui->spinCoefficACP->show();
        ui->spinCoefficARP->show();
        ui->spinCoefficVideo->show();
        ui->spinCoefficTrigger->show();
        ui->butAutoACP->show();
        ui->butAutoARP->show();
        ui->butAutoVidio->show();
        ui->butAutoTrig->show();

        ui->labR1->hide();
        ui->labR2->hide();
        ui->labS1->hide();
        ui->labS2->hide();
        ui->labS3->hide();
        ui->labVidR->hide();
        ui->labTrigR->hide();
        ui->comboBoxR1->hide();
        ui->comboBoxR2->hide();
        ui->comboBoxS1->hide();
        ui->comboBoxS2->hide();
        ui->comboBoxS3->hide();
        ui->comboBoxTarR->hide();
        ui->comboBoxTrR->hide();
    }
}

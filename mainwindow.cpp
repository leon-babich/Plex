#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowTitle("Plex handler");

    m_windSettings = new WindowSettings;

    m_sizeInd = 20;                                                                      //размеры кнопок
    m_sizeBut = 50;
    ui->widgetGraph;

    ui->labIndicator->setFixedSize(m_sizeInd,m_sizeInd);
    ui->labIndicator_2->setFixedSize(m_sizeInd,m_sizeInd);
    ui->labIndicator_3->setFixedSize(m_sizeInd,m_sizeInd);
    ui->butSettingsPort->setFixedSize(m_sizeBut,m_sizeBut);
    ui->buttonStartBoard->setFixedSize(60,30);
    ui->butConnectPort->setFixedSize(60,30);
    ui->butPath->setFixedSize(40,30);

    paintIndicator_1(false);
    paintIndicator_2(false);
    paintIndicator_3(false);
    paintButSettings();
    paintButBoard(false);
    paintButPort(false);

    timerButBoard = new QTimer;                                                            //таймеры
    connect(timerButBoard,&QTimer::timeout,this,&MainWindow::slotSwitchButBoard);

    timerButPort = new QTimer;
    connect(timerButPort,&QTimer::timeout,this,&MainWindow::slotSwitchButBoard);

    timerIndicator2 = new QTimer;
    connect(timerIndicator2,&QTimer::timeout,this,&MainWindow::slotSwitchIndicator_2);

    timerIndicator3 = new QTimer;
    connect(timerIndicator3,&QTimer::timeout,this,&MainWindow::slotSwitchIndicator_3);

    m_timerGoBackBoxMode = new QTimer;
    connect(m_timerGoBackBoxMode,&QTimer::timeout,this,&MainWindow::slotGoBackBoxMode);

    ui->buttonStop->setEnabled(false);

    isConnectedImage = true;
    isShowTime = false;
    isShowMessage = true;
    m_isBoard = false;
    m_isPort = false;
    m_isFile = false;
    m_isIndicator_2 = false;
    m_isIndicator_3 = false;
    m_isFullScreen = false;
    m_chooseTab = 1;
    m_isActiveButtons = true;
    m_isMessagePort = false;
    m_isOpenWindowMessage = false;
    m_isFistARP = true;

    m_dataReader = new DataReader;                                                                //класс обработчика
    m_dataProcessor = new DataProcessor;
    m_dataProcessor->setBoardData(m_dataReader->getBoardData());
    m_dataProcessor->setModeScreen(ui->widgetGraph->getModeScreen());
    m_windSettings->setSettingsChannel(m_dataProcessor->getSettingsChannel());
//    ui->widgetGraph->setSetingsChannel(m_dataProcessor->getLinkSettingsChannel());               //настройки каналов в графическом окне
    m_windSettings->pAutoValue = m_dataProcessor->autoValue;
    ui->widgetGraph->radar->setTimeBlockTarg(m_dataProcessor->getTimeBlockTarg());
    ui->widgetGraph->radar->setArrayTargets(m_dataProcessor->getArrayTargets());
    for(int i=0; i<PROC_CHAN; i++) ui->widgetGraph->setVecSin(m_dataProcessor->getVecSin(i), i);
    for(int i=0; i<PROC_CHAN; i++) ui->widgetGraph->setVecSinReal(m_dataProcessor->getVecSinReal(i), i);
    readSettingsFile();
    ui->spinBoxTarg->setMaximum(200);
    ui->spinBoxTarg->setValue(50);

    m_nameChannel = m_dataProcessor->getNameChannel();
    ui->widgetGraph->resizeGraphVec(m_dataProcessor->getVecLine());
//    ui->widgetGraph->resizeGraphVecR18(m_dataProcessor->getVecLineR18());
    ui->widgetGraph->setArrayTargets(m_dataProcessor->getArrayTargets());
    ui->widgetGraph->setListMask(m_windSettings->getListMask());
    m_dataProcessor->setListMask(m_windSettings->getListMask());
    ui->widgetGraph->setNumbersTPSChannel(m_dataProcessor->getNumbersTPSChannels());
    ui->widgetGraph->setNumbersR18Channel(m_dataProcessor->getNumbersR18Channels());
    ui->widgetGraph->radar->setPathFileTarg(m_windSettings->getPathWriteTar());

    m_thread_1 = new QThread;                                                                       //потоки
    connect(m_thread_1,&QThread::started,m_dataReader,&DataReader::runRead);
    connect(m_dataReader,&DataReader::sigFinished,m_thread_1,&QThread::terminate);
    m_dataReader->moveToThread(m_thread_1);

    m_thread_2 = new QThread;
    connect(m_thread_2,&QThread::started,m_dataProcessor,&DataProcessor::runProcessing);
    connect(m_dataProcessor,&DataProcessor::signalFinishProcessing,m_thread_2,&QThread::terminate);
    m_dataProcessor->moveToThread(m_thread_2);

    m_thread_3 = new QThread;
    connect(m_thread_3,&QThread::started,m_windSettings->m_port,&Port::runPort);
    connect(m_windSettings->m_port,&Port::sigFinished,m_thread_3,&QThread::terminate);
    m_windSettings->m_port->moveToThread(m_thread_3);

//    keyFile = new QShortcut(this);                                                                //клавиатура
//    keyFile->setKey(Qt::Key_F);
//    connect(keyFile,&QShortcut::activated,this,&MainWindow::slotSetModeKey_F);

    keySwitchBack = new QShortcut(this);
    keySwitchBack->setKey(Qt::Key_Left);
    connect(keySwitchBack,&QShortcut::activated,this,&MainWindow::slotSwitchBack);

    keySwitchFoward = new QShortcut(this);
    keySwitchFoward->setKey(Qt::Key_Right);
    connect(keySwitchFoward,&QShortcut::activated,this,&MainWindow::slotSwitchFoward);

    connect(m_dataReader,&DataReader::sigDataReady,this,&MainWindow::slotSetReadyProcessor,Qt::DirectConnection);
    connect(m_dataReader,&DataReader::sigSendMessage,this,&MainWindow::slotShowMessage);
    connect(m_dataReader,&DataReader::sigDisplayErrorMessage,this,&MainWindow::slotDisplayErrorMessage);
    connect(m_dataReader,&DataReader::sigStopHandle,this,&MainWindow::slotStopRecord);
    connect(m_dataReader,&DataReader::sigTime,ui->widgetGraph,&GlGraph::slotSetTimeBoard);

    connect(m_dataProcessor,&DataProcessor::sigSendMessage,this,&MainWindow::slotShowMessage);
    connect(m_dataProcessor,&DataProcessor::sigDisplayErrorMessage,this,&MainWindow::slotDisplayErrorMessage);
    connect(m_dataProcessor,&DataProcessor::sigRotationCounting,this,&MainWindow::slotShowRotationCounting,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigCountPack,this,&MainWindow::slotShowCurrentCount,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigCountPRF,this,&MainWindow::slotShowPRF,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigBlockTarget,ui->widgetGraph->radar,&Radar::slotAddBlockTarget,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigFoundARP,ui->widgetGraph->radar,&Radar::slotCheckNorthCursor);
    connect(m_dataProcessor,&DataProcessor::sigCurrentAzimuth,ui->widgetGraph->radar,&Radar::slotSetSpeedCursor);
    connect(m_dataProcessor,&DataProcessor::sigPacketFinish,ui->widgetGraph,&GlGraph::slotSetLine,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigSendAngles,ui->widgetGraph,&GlGraph::slotReceiveAngles,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigSendCountAngle,ui->widgetGraph,&GlGraph::slotSetCountAngleR18,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigSetSinchData,ui->widgetGraph,&GlGraph::slotSetSinchData,Qt::DirectConnection);
//    connect(m_dataProcessor,&DataProcessor::sigSendTopsSin,ui->widgetGraph,&GlGraph::slotSetVecSin,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigSetSizeSin,ui->widgetGraph,&GlGraph::slotSetSizeSin,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigTimeRotation,this,&MainWindow::slotShowNorth);
    connect(m_dataProcessor,&DataProcessor::sigBlockDetectTarget,m_windSettings->m_port,&Port::slotReceiveBlockPrimaryPlots,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigFoundARP,m_windSettings->m_port,&Port::slotSendAziuthReference,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigSetChanges,this,&MainWindow::slotSetChangedValue);
    connect(m_dataProcessor,&DataProcessor::sigValueSignal,this,&MainWindow::slotShowValueSignals,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigShowImpuls,this,&MainWindow::slotShowImpuls,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigTime,ui->widgetGraph,&GlGraph::slotSetTimeProcessor);
    connect(m_dataProcessor,&DataProcessor::sigCorrectARP,ui->widgetGraph->radar,&Radar::slotSetCorrectARP,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigSendCountAngle,ui->widgetGraph->radar,&Radar::slotSetCountAngleR18,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigSendCountAngle,ui->widgetGraph->radar,&Radar::slotSetSpeedCursorR18,Qt::DirectConnection);
    connect(m_dataProcessor,&DataProcessor::sigSendPoints,ui->widgetGraph,&GlGraph::slotSetPoints,Qt::DirectConnection); //тест

    connect(m_windSettings,&WindowSettings::sigSendMessage,this,&MainWindow::slotShowMessage);
    connect(m_windSettings,&WindowSettings::sigSetComBoxMode,this,&MainWindow::slotSetComBoxMode);
    connect(m_windSettings,&WindowSettings::sigNumFoundPorts,this,&MainWindow::slotNumFoundPorts);
    connect(m_windSettings,&WindowSettings::sigAddMask,ui->widgetGraph,&GlGraph::slotUpdata);
    connect(m_windSettings,&WindowSettings::sigSetButLayout,ui->widgetGraph,&GlGraph::slotSetButLayout);

    connect(m_windSettings->m_port,&Port::sigSendMessage,this,&MainWindow::slotShowMessage);
    connect(m_windSettings->m_port,&Port::sigSendedData,this,&MainWindow::slotSwitchIndicator_3);
    connect(m_windSettings->m_port,&Port::sigSetConnection,this,&MainWindow::slotSwitchIndicator_4);

    connect(ui->widgetGraph,&GlGraph::sigDisplayErrorMessage,this,&MainWindow::slotDisplayErrorMessage);
    connect(ui->widgetGraph,&GlGraph::sigSetChannel,this,&MainWindow::slotSetChannel,Qt::DirectConnection);

    emit m_dataProcessor->sigSetChanges();
    m_thread_1->start();
    m_thread_2->start();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *ev)                     //событие изменения размера окна
{
    ModeLocationButtons mode = (float)ev->size().width()/ev->size().height() > 1.6 ? SIDE : BOTTOM;
    ui->widgetGraph->slotSetButLayout(mode);
//    Q_UNUSED(ev);
}

void MainWindow::keyPressEvent(QKeyEvent *e)
{
    if(e->key() == Qt::Key_Escape) {
        if(isFullScreen()) {
            this->showNormal();
        }
        else {
            this->showFullScreen();
        }
    }
    if(e->key() == Qt::Key_C) {
        m_windSettings->setTypeRadar();
        m_dataProcessor->setTypeRadar(m_windSettings->getTypeRadar());
        ui->widgetGraph->radar->setTypeRadar(m_windSettings->getTypeRadar());
        ui->widgetGraph->setTypeRadar(m_windSettings->getTypeRadar());
    }
    if(e->key() == Qt::Key_W) {
        ui->widgetGraph->radar->slotSetModeMoving();
    }
    if(e->key() == Qt::Key_F) {
        setModeKey_F();
    }
    if(e->key() == Qt::Key_A) {
        setModeKey_F();
        m_windSettings->setTypeRadar();
        m_dataProcessor->setTypeRadar(m_windSettings->getTypeRadar());
        ui->widgetGraph->radar->setTypeRadar(m_windSettings->getTypeRadar());
        ui->widgetGraph->setTypeRadar(m_windSettings->getTypeRadar());
        ui->widgetGraph->switchOnAnalog();
    }
}

void MainWindow::paintIndicator_1(bool isConnect)                      //отрисовка индикаторов микрогейта...
{
    QColor colorCycle = (isConnect?Qt::green:QColor(150,150,150));

    ui->labIndicator->setPixmap(QPixmap::fromImage(paintIndicators(colorCycle)));
}

void MainWindow::paintIndicator_2(bool isConnect)
{
    QColor colorCycle = (isConnect?QColor(255,120,0):QColor(150,150,150));

    ui->labIndicator_2->setPixmap(QPixmap::fromImage(paintIndicators(colorCycle)));
}

void MainWindow::paintIndicator_3(bool isConnect)
{
    QColor colorCycle = (isConnect?Qt::red:QColor(150,150,150));

    ui->labIndicator_3->setPixmap(QPixmap::fromImage(paintIndicators(colorCycle)));
}

QImage MainWindow::paintIndicators(QColor &color)
{
    QRect rect(ui->labIndicator->contentsRect());
    QPainter painter;
    QImage image(rect.size(),QImage::Format_ARGB32_Premultiplied);

    image.fill(QColor(255, 255, 255, 0));
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing,true);
    painter.setBrush(QBrush(color));
    painter.drawEllipse(0,0,m_sizeInd,m_sizeInd);
    painter.end();

    return image;
}

bool MainWindow::activationBoard()
{
    if(!m_dataReader->getIsBoard()) {
        m_dataReader->loadBRD();
        paintButBoard(true);
        timerButBoard->start(200);

        if(m_dataReader->getIsBoard()) {
            m_isBoard = true;
            return true;
        }
    }
    else {
        m_dataReader->closingBRD();
        paintButBoard(false);
        timerButBoard->start(100);

        if(!m_dataReader->getIsBoard()) {
            m_isBoard = false;
            return true;
        }
    }

    return false;
}

bool MainWindow::activationPort()
{
    if(!m_isPort)
    {
        m_windSettings->m_port->setRunning(true);
        m_thread_3->start();
        m_windSettings->connectPort();

        if(m_windSettings->m_port->getIsConnection()) {
            paintIndicator_1(true);
        }
        else {
//            ui->widgetGraph->setIsConnectPort(false);
//            ui->textEditMessage->append("<FONT COLOR=RED>The port is not connected");
            return false;
        }

        m_isPort = true;
        paintButPort(true);
        timerButPort->start(200);
    }
    else
    {
        m_windSettings->m_port->setRunning(false);
        m_windSettings->connectPort();
        m_isPort = false;
        paintButPort(false);
        timerButPort->start(200);
        paintIndicator_1(false);
        slotSwitchIndicator_4(false);
    }

    return true;
}

void MainWindow::setActiveButtons(bool active)
{
    ui->buttonStart->setEnabled(active);
    ui->buttonStop->setEnabled(!active);
    m_isActiveButtons = active;
}

void MainWindow::readSettingsFile()
{
    QFile fileOut("settings.txt");

    if(!fileOut.exists()) return;

    if(!fileOut.open(QIODevice::ReadOnly)) {
        ui->textEditMessage->append("<FONT COLOR=RED>Fail opening settings file");
        return;
    }

    QString path;
    QString str;
    QString sym;
    int n = 0;
    int s;
    int channels[7] = { 0, 0, 0, 0, 0, 0, 0 };

    while(!fileOut.atEnd()) {
        str = fileOut.readLine();
        if(str[str.size()-1] == "\n") {
            str.remove(str.size()-1, 1);
        }
        n++;
        switch (n) {
        case 1:
            path = str;
            path.remove("\r");
            m_dataReader->setPath(path);
            m_windSettings->m_windFile->setPathRead(path);
            break;
        case 2:
            str.remove(" ");
            s = str.size() <= 7 ? str.size() : 7;

            for(int i=0; i<s; i++) {
                sym = str[i];
                channels[i] = sym.toInt(0,10);
//                qDebug << isdigit(str[i]);
            }

            m_dataProcessor->setChannelsR18(channels);
            m_windSettings->setNumberChannels(channels);
            break;
        default:
            break;
        }
    }

    fileOut.close();
}

void MainWindow::slotSetChannel(short num, bool isActive)
{
    m_dataProcessor->setPaintChan(num, isActive);
    ui->widgetGraph->setPaintChan(num, isActive);
}

void MainWindow::slotGoBackBoxMode()
{
    m_timerGoBackBoxMode->stop();
    ui->comboBoxMode->setCurrentIndex(m_dataReader->getMode());
}

void MainWindow::slotSetComBoxMode(int index)
{
    ui->comboBoxMode->setCurrentIndex(index);
}

void MainWindow::slotNumFoundPorts(int n)
{
    ui->widgetGraph->setIsConnectPort(n);
}

void MainWindow::paintButSettings()
{
    //QPixmap pix(":/images/Img/vspc-icon-256.png");
    QPixmap pix(":/images/Img/gear_2.jpg");
    QBitmap bm = pix.createMaskFromColor(QColor(255,255,255));
    pix.setMask(bm);
    QBitmap bm2 = pix.createMaskFromColor(QColor(254,254,254));
    pix.setMask(bm2);
    ui->butSettingsPort->setIcon(pix);
    ui->butSettingsPort->setIconSize(QSize(m_sizeBut,m_sizeBut));
}

void MainWindow::paintButBoard(bool isConnect)
{
    QRect rect(ui->buttonStartBoard->contentsRect());
    QRect rect2;
    int rad=rect.height()-2;
    int xPos=(isConnect?rad:0);
    QColor color=(isConnect?Qt::darkGreen:Qt::darkGray);

    rect2.setWidth(rad*2);
    rect2.setHeight(rad);

    QPainter painter;
    QImage image(rect2.size(),QImage::Format_ARGB32_Premultiplied);

    image.fill(QColor(255, 255, 255,0));
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing,true);

    QLinearGradient gradient1(rad,0,rad,rad);

    gradient1.setColorAt(0.0,Qt::white);
    gradient1.setColorAt(0.5,color);
    painter.setBrush(gradient1);
    painter.drawEllipse(xPos,0,rad,rad);
    painter.end();

    ui->buttonStartBoard->setIcon(QPixmap::fromImage(image));
    ui->buttonStartBoard->setIconSize(QSize(rad*2,rad));
}

void MainWindow::paintButPort(bool isConnect)
{
    QRect rect(ui->butConnectPort->contentsRect());
    QRect rect2;
    int rad = rect.height() - 2;
    int xPos = (isConnect ? rad : 0);
    QColor color = (isConnect ? QColor(0, 147, 0) : Qt::darkGray);

    rect2.setWidth(rad * 2);
    rect2.setHeight(rad);

    QPainter painter;
    QImage image(rect2.size(),QImage::Format_ARGB32_Premultiplied);

    image.fill(QColor(255, 255, 255,0));
    painter.begin(&image);
    painter.setRenderHint(QPainter::Antialiasing,true);

    QLinearGradient gradient1(rad,0,rad,rad);

    gradient1.setColorAt(0.0,Qt::white);
    gradient1.setColorAt(0.5,color);
    painter.setBrush(gradient1);
    painter.drawEllipse(xPos,0,rad,rad);
    painter.end();

    ui->butConnectPort->setIcon(QPixmap::fromImage(image));
    ui->butConnectPort->setIconSize(QSize(rad*2,rad));
}

void MainWindow::slotSwitchIndicator_2()
{
    if(!m_isIndicator_2) {
        paintIndicator_2(true);
        m_isIndicator_2 = true;
        timerIndicator2->start(100);
    }
    else {
        paintIndicator_2(false);
        m_isIndicator_2 = false;
        timerIndicator2->stop();
    }
}

void MainWindow::slotSwitchIndicator_3()
{
    if(!m_isIndicator_3) {
        paintIndicator_3(true);
        m_isIndicator_3 = true;
        timerIndicator3->start(100);
    }
    else {
        paintIndicator_3(false);
        m_isIndicator_3 = false;
        timerIndicator3->stop();
    }
}

void MainWindow::slotSwitchIndicator_4(bool connection)
{
    if(connection) {
        paintIndicator_2(true);
    }
    else {
        paintIndicator_2(false);
    }
}

void MainWindow::slotShowNorth(int time, int az)
{
    qreal realTime = (qreal)time/1000;
    ui->widgetGraph->setData(realTime, ROTARION);

    QString strAz = az != 4096 ? "<FONT COLOR=RED> " + QString::number(az) : "<FONT COLOR=GREEN> " + QString::number(az);
    if(!m_isFistARP) {
         ui->textEditMessage->append("Azimuth:" + strAz);
    }
    else {
        m_isFistARP = false;
    }
}

void MainWindow::slotSwitchButBoard()
{
    if(m_isBoard) {
        paintButBoard(true);
    }
    else {
        paintButBoard(false);
    }

    timerButBoard->stop();
}

void MainWindow::slotSwitchButPort()
{
    if(m_isPort) {
        paintButPort(true);
    }
    else {
        paintButPort(false);
    }

    timerButPort->stop();
}

void MainWindow::setModeKey_F()
{
    if(!m_isFile) {
        m_dataReader->setMode(DISK);
        m_dataProcessor->setMode(DISK);
        m_windSettings->setMode(DISK);
        ui->comboBoxMode->setCurrentIndex(DISK);
        m_dataProcessor->clearData();
        ui->widgetGraph->radar->clearRadar();
        m_isFile = true;
    }
    else {
        m_dataReader->setMode(BOARD);
        m_dataProcessor->setMode(BOARD);
        m_windSettings->setMode(BOARD);
        ui->comboBoxMode->setCurrentIndex(BOARD);
        m_dataProcessor->clearData();
        ui->widgetGraph->radar->clearRadar();
        m_isFile = false;
    }
}

void MainWindow::slotShowCurrentCount(int channel[])
{
//    ui->lcdNumberACP->display(channel[m_dataProcessor->getSettingsChannel().azimuth]);
//    ui->lcdNumberARP->display(channel[m_dataProcessor->getSettingsChannel().north]);
//    ui->lcdNumberVid->display(channel[m_dataProcessor->getSettingsChannel().target]);
//    ui->lcdNumberTrig->display(channel[m_dataProcessor->getSettingsChannel().trigger]);

    ui->widgetGraph->setData(channel[m_dataProcessor->getNumbersTPSChannels()[ACP]], AZIMUTH);
}

void MainWindow::slotShowPRF(int count)
{
//    ui->lcdNumberPRF->display(count);
    ui->widgetGraph->setData(count, PRF);
}

void MainWindow::slotSetChangedValue()
{
//    ui->lcdThreshACP->display(m_dataProcessor->getSettingsChannel().valueThreshold[m_dataProcessor->getSettingsChannel().azimuth]);
//    ui->lcdThreshARP->display(m_dataProcessor->getSettingsChannel().valueThreshold[m_dataProcessor->getSettingsChannel().north]);
//    ui->lcdThreshVidio->display(m_dataProcessor->getSettingsChannel().valueThreshold[m_dataProcessor->getSettingsChannel().target]);
//    ui->lcdThreshTrig->display(m_dataProcessor->getSettingsChannel().valueThreshold[m_dataProcessor->getSettingsChannel().trigger]);
}

void MainWindow::slotShowValueSignals(int maxValue[], int maxNoise[])
{
//    ui->lcdValueACP->display(maxValue[m_dataProcessor->getSettingsChannel().azimuth]);
//    ui->lcdValueARP->display(maxValue[m_dataProcessor->getSettingsChannel().north]);
//    ui->lcdValueVid->display(maxValue[m_dataProcessor->getSettingsChannel().target]);
//    ui->lcdValueTrig->display(maxValue[m_dataProcessor->getSettingsChannel().trigger]);

    ui->widgetGraph->setMaxVideo(maxValue[m_dataProcessor->getSettingsChannel()->target] * 0.8);
}

void MainWindow::slotShowRotationCounting(int channel[])
{
//    ui->channelLcd_1->display(channel[m_dataProcessor->getSettingsChannel().azimuth]);
//    ui->channelLcd_2->display(channel[m_dataProcessor->getSettingsChannel().north]);
//    ui->channelLcd_3->display(channel[m_dataProcessor->getSettingsChannel().target]);
//    ui->channelLcd_4->display(channel[m_dataProcessor->getSettingsChannel().trigger]);

//    ui->widgetShedule->setData(channel[m_dataProcessor->getSettingsChannel().azimuth], AZIMUTH);
    ui->widgetGraph->setData(channel[m_dataProcessor->getNumbersTPSChannels()[ARP]], NORTH);
    ui->widgetGraph->setData(channel[m_dataProcessor->getNumbersTPSChannels()[VIDEO]], TARGET);
    ui->widgetGraph->setData(channel[m_dataProcessor->getNumbersTPSChannels()[TRIGGER]], TRIG);
}

void MainWindow::slotSetReadyProcessor()
{
    m_dataProcessor->setIsReadyData(true);
}

void MainWindow::slotShowMessage(QString strMessage)
{
    if(isShowMessage)
    {
        ui->textEditMessage->append(strMessage);
    }
}

void MainWindow::slotDisplayErrorMessage(QString strMessage)
{

    int ret = 0;

    if(!m_isOpenWindowMessage) {
        m_isOpenWindowMessage = true;
        ret = QMessageBox::warning(this, "Out of range", strMessage+".\nDo you want to stop the program?", QMessageBox::Ok | QMessageBox::Cancel);
    }

    if(ret == QMessageBox::Ok) {
        m_dataReader->setRunning(false);
        m_dataProcessor->setRunning(false);
        ui->widgetGraph->runImage(false);

        if(m_dataReader->getMode() == BOARD) {
            m_dataProcessor->clearData();
        }

        setActiveButtons(true);
        m_isOpenWindowMessage = false;
    }
}

void MainWindow::on_buttonStart_clicked()                                              //кнопка старт
{
    if(m_dataReader->getMode() == BOARD) {
        if(!m_isBoard) {
            if(!activationBoard()) return;
        }

        ui->widgetGraph->radar->clearRadar();
    }
    else if(m_dataReader->getMode() == DISK) {
        if(!m_dataReader->getIsStartFile()) {
            if(!m_dataReader->startFile()) return;
        }

        if(m_dataReader->isEmptyFile()) {
            ui->widgetGraph->radar->clearRadar();
        }
    }

    if(!m_isMessagePort) {
        if(!m_isPort) activationPort();
        m_isMessagePort = true;
    }

    ui->widgetGraph->radar->setNewTimePath();
    m_dataReader->setRunning(true);
    m_dataProcessor->setRunning(true);
    ui->widgetGraph->runImage(true);
    //    m_thread_1->start();
    //    m_thread_2->start();

    setActiveButtons(false);
}

void MainWindow::on_buttonStop_clicked()                                              //кнопка стоп
{
    m_dataReader->setRunning(false);
    m_dataProcessor->setRunning(false);
    ui->widgetGraph->runImage(false);

    if(m_dataReader->getMode() == BOARD) {
        m_dataProcessor->clearData();
    }

    setActiveButtons(true);
}

void MainWindow::on_buttonStartBoard_clicked()                                        //кнопка старт платы
{
    if(m_isActiveButtons) {
        activationBoard();
    }
}

void MainWindow::on_butConnectPort_clicked()                                          //кнопка старт порта
{
    if(m_isActiveButtons) {
        activationPort();
    }
}

void MainWindow::on_butSettingsPort_clicked()                                         //кнопка настройки
{
    if(!m_isActiveButtons) {
        return;
    }

    m_windSettings->checkCurrentWindow();
    m_windSettings->setWindowTitle("COM port settings");
    m_windSettings->setModal(true);
    m_windSettings->exec();
//    ui->widgetGraph->setSetingsChannel(m_dataProcessor->getLinkSettingsChannel());          //настройки каналов в графическом окне
    m_windSettings->changeSettingsChannel();
    m_dataProcessor->setIsProtectCorrectARP(m_windSettings->getIsProtectCheckSig());
    m_dataProcessor->setStartAzimuth(m_windSettings->getStartAngle());
    ui->widgetGraph->radar->setStartAngle(m_windSettings->getStartAngle());
    ui->widgetGraph->setNoiseFilterTarg(m_windSettings->getNoiseFilter());
    m_dataProcessor->setTypeRadar(m_windSettings->getTypeRadar());
    m_dataProcessor->setIsReduceSigSR18(m_windSettings->getIsReducedSignalS());
    m_dataProcessor->setIsReduceSigRR18(m_windSettings->getIsReducedSignalR());
    ui->widgetGraph->radar->setTypeRadar(m_windSettings->getTypeRadar());
    ui->widgetGraph->setTypeRadar(m_windSettings->getTypeRadar());
    ui->widgetGraph->radar->setWritingTarg(m_windSettings->getIsWritingTar());
    emit m_dataProcessor->sigSetChanges();

    if(m_windSettings->getIsChangedPath()) {
        m_dataReader->setPath(m_windSettings->getPathRead());
        m_dataProcessor->setPath(m_windSettings->getPathWrite());
        m_dataProcessor->clearData();
    }

    bool isChangedMode = m_dataReader->getMode() == (ModeReading)m_windSettings->getIsReadRec() ? false : true;

    if(isChangedMode) {
        m_dataReader->setMode((ModeReading)m_windSettings->getIsReadRec());
        m_dataProcessor->setMode((ModeReading)m_windSettings->getIsReadRec());
        m_dataProcessor->clearData();
        ui->widgetGraph->radar->clearRadar();
    }

    if(m_windSettings->getIsClearOldData()) {
        m_dataProcessor->clearData();
        ui->widgetGraph->radar->clearRadar();
    }

    m_dataProcessor->setWriting(m_windSettings->getIsWriteRec());
    ui->widgetGraph->radar->setPathFileTarg(m_windSettings->getPathWriteTar());
    m_windSettings->setIsClearOldData(false);
}

void MainWindow::on_butClear_clicked()                                                    //кнопка очистки окна сообщений
{
    ui->textEditMessage->clear();
}

void MainWindow::on_butPath_clicked()                                                     //кнопка путь к файлу
{
    if(!m_isActiveButtons) {
        return;
    }

    m_windSettings->m_windFile->setIsSetRead(READ_REC);
    m_windSettings->m_windFile->setWindowTitle("File path for reading");
    m_windSettings->m_windFile->setModal(true);
    m_windSettings->m_windFile->exec();

    if(m_windSettings->getIsChangedPath()) {
        m_dataReader->setPath(m_windSettings->getPathRead());
        m_dataReader->setMode(DISK);
        m_dataProcessor->setMode(DISK);
        ui->comboBoxMode->setCurrentIndex(DISK);
        m_windSettings->setMode(DISK);
        m_dataProcessor->clearData();
    }
}

void MainWindow::on_comboBoxMode_activated(int index)
{
    if(!m_isActiveButtons) {
        m_timerGoBackBoxMode->start(250);
        return;
    }

    bool isChanged = m_dataReader->getMode() != index ? true : false;

    if(isChanged) {
        m_dataReader->setMode((ModeReading)index);
        m_dataProcessor->setMode((ModeReading)index);
        m_windSettings->setMode(index);

        m_dataProcessor->clearData();
        ui->widgetGraph->radar->clearRadar();
    }
}

void MainWindow::slotSwitchBack()
{
    ui->widgetGraph->switchModeScreen(false);
}

void MainWindow::slotSwitchFoward()
{
    ui->widgetGraph->switchModeScreen(true);
}

void MainWindow::slotShowImpuls(int *impuls)
{
//    ui->lcdMinImpulsACP->display(impuls[m_dataProcessor->getSettingsChannel().azimuth]);
//    ui->lcdMinImpulsARP->display(impuls[m_dataProcessor->getSettingsChannel().north]);
//    ui->lcdMinImpulsVid->display(impuls[m_dataProcessor->getSettingsChannel().target]);
//    ui->lcdMinImpulsTr->display(impuls[m_dataProcessor->getSettingsChannel().trigger]);
    ui->widgetGraph->setData(impuls[m_dataProcessor->getNumbersTPSChannels()[TRIGGER]], IMPULS);
}

void MainWindow::slotStopRecord()
{
    m_dataReader->setRunning(false);
    m_dataProcessor->setRunning(false);
    ui->widgetGraph->runImage(false);
    m_dataReader->finishFile();
    m_dataProcessor->clearData();
    setActiveButtons(true);
}

void MainWindow::on_spinBoxTarg_valueChanged(int arg1)
{
    m_dataProcessor->setNumTarg(arg1);
}

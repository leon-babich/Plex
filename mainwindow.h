#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QGraphicsScene>
#include <QThread>
#include <QTimer>
#include <QPainter>
#include <QPalette>
#include <QBitmap>
#include <QDebug>
#include <QShortcut>
#include <QWheelEvent>

#include "dataprocessor.h"
#include "datareader.h"
#include "port.h"
#include "windSettings.h"
#include "glgraph.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *e);

private slots:
    void slotShowMessage(QString strMessage);
    void slotDisplayErrorMessage(QString strMessage);
    void slotShowRotationCounting(int channel[]);
    void slotSetReadyProcessor();
    void slotSwitchIndicator_2();
    void slotSwitchIndicator_3();
    void slotSwitchIndicator_4(bool connection);
    void slotShowNorth(int time, int az);
    void slotSwitchButBoard();
    void slotSwitchButPort();
    void slotShowCurrentCount(int channel[]);
    void slotShowPRF(int count);
    void slotSetChangedValue();
    void slotShowValueSignals(int maxValue[], int maxNoise[]);
    void slotSwitchBack();
    void slotSwitchFoward();
    void slotShowImpuls(int *impuls);
    void slotStopRecord();
    void slotSetChannel(short num, bool isActive);
    void slotGoBackBoxMode();
    void slotSetComBoxMode(int index);
    void slotNumFoundPorts(int n);

    void on_buttonStart_clicked();
    void on_buttonStop_clicked();
    void on_buttonStartBoard_clicked();
    void on_butConnectPort_clicked();
    void on_butSettingsPort_clicked();
    void on_butClear_clicked();
    void on_butPath_clicked();
    void on_comboBoxMode_activated(int index);
    void on_spinBoxTarg_valueChanged(int arg1);

private:
    void setModeKey_F();
    void paintIndicator_1(bool isConnect);
    void paintIndicator_2(bool isConnect);
    void paintIndicator_3(bool isConnect);
    void paintIndicator_4(bool isConnect);
    void paintButSettings();
    void paintButBoard(bool isConnect);
    void paintButPort(bool isConnect);
    //QImage MainWindow::paintIndicators(QColor &color);
    QImage paintIndicators(QColor &color);
    bool activationBoard();
    bool activationPort();
    void setActiveButtons(bool active);
    void readSettingsFile();

private:
    Ui::MainWindow *ui;
    DataProcessor *m_dataProcessor;
    DataReader *m_dataReader;
    QGraphicsScene *m_scene;
    QGraphicsScene *m_scene_2;
    QGraphicsScene *m_scene_3;
    QThread *m_thread_1;
    QThread *m_thread_2;
    QThread *m_thread_3;
    bool isConnectedImage;
    bool isShowTime;
    bool isShowMessage;
    bool m_isBoard;
    bool m_isPort;
    bool m_isFile;
    bool m_isIndicator_1;
    bool m_isIndicator_2;
    bool m_isIndicator_3;
    bool m_isFullScreen;
    int m_sizeInd;
    int m_sizeBut;
    int m_widthView_2;
    int m_heightView_2;
    int m_widthView_3;
    int m_heightView_3;
    int m_chooseTab;
    bool m_isActiveButtons;
    bool m_isMessagePort;
    bool m_isOpenWindowMessage;
    bool m_isFistARP;

    Cursor m_cursor;
    QTimer *timerShowTime;
    QTimer *timerIndicator2;
    QTimer *timerIndicator3;
    QTimer *timerButBoard;
    QTimer *timerButPort;
    QTimer *m_timerGoBackBoxMode;
    WindowSettings *m_windSettings;
    QShortcut *keySwitchBack;
    QShortcut *keySwitchFoward;
    const QMap<int,QString> *m_nameChannel;
};

#endif // MAINWINDOW_H

#ifndef WINDOWSETTINGS_H
#define WINDOWSETTINGS_H

#include <QDialog>
#include <port.h>
#include <QTimer>

#include "dataprocessor.h"
#include "windfile.h"
#include "glgraph.h"

namespace Ui {
class WindowSettings;
}

class WindowSettings : public QDialog
{
    Q_OBJECT

public:
    explicit WindowSettings(QWidget *parent = 0);
    ~WindowSettings();
    void checkCurrentWindow();
    void connectPort();
    void changeSettingsChannel();
    void saveChannels();
    void setSettingsPort();
    void setSettingsChannel(SettingsChannel *settings);
    void setMode(int mode);
    void setTypeRadar();
    void setIsClearOldData(bool clearing);
    void setNumberChannels(int number[7]);
    bool getIsMicrogate() const;
    QString getPathRead() const;
    QString getPathWrite() const;
    QString getPathWriteTar() const;
    bool getIsWriteRec() const;
    bool getIsReadRec() const;
    bool getIsWritingTar() const;
    bool getIsChangedPath();
    bool getIsClearOldData();
    bool getIsProtectCheckSig();
    int getStartAngle();
    int getNoiseFilter();
    TypeRadar getTypeRadar();
    QList<RadarMask*>& getListMask();
    bool getIsReducedSignalS();
    bool getIsReducedSignalR();

private slots:
    void slotSetIsClearOldData();
    void slotIsWriteTar();

    void on_comboBox_activated(int index);
    void on_comboBoxPort_activated(int index);
    void on_butPathWrite_clicked();
    void on_butPathRead_clicked();
    void on_comboBoxMode_activated(const QString &arg1);
    void on_comboBoxCreateRecord_activated(const QString &arg1);
    void on_butAutoACP_clicked(bool checked);
    void on_butAutoARP_clicked(bool checked);
    void on_butAutoVidio_clicked(bool checked);
    void on_butAutoTrig_clicked(bool checked);
    void on_butAddMask_clicked();
    void on_butCancelMask_clicked();
    void on_listMask_doubleClicked(const QModelIndex &index);
    void on_listMask_clicked(const QModelIndex &index);
    void on_butDeleteMask_clicked();
    void on_comboBoxButLay_activated(int index);
    void on_tabWidget_currentChanged(int index);
    void on_comboBoxTypeRadar_activated(int index);
    void on_butPathTarRec_clicked();

signals:
    void sigSendMessage(QString strMessage);
    void sigSetComBoxMode(int index);
    void sigAddMask();
    void sigSetButLayout(ModeLocationButtons mode);
    void sigNumFoundPorts(int n);

public:
    Port *m_port;
    bool *pAutoValue;
    WindFile *m_windFile;

private:
    Ui::WindowSettings *ui;
    SettingsPort m_settingsPort;
    SettingsChannel *m_pSettingsChannel;
    bool m_isMicroGate;
    bool m_isOpenPort;
    bool m_isWriteRecord;
    bool m_isReadRecord;
    ModeReading m_modeRead;
    bool m_isCreateMask;
    bool m_isChangeMask;
    int m_sizeMask;
    QList<RadarMask*> m_listMask;
    int m_numMask;
    bool m_isClearOldData;
    bool m_isProtectCheckSig;
    QTimer *m_timerCountTimeSettings;

private:
    void clearNamesChannel();
};

#endif // WINDOWSETTINGS_H

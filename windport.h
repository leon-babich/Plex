#ifndef WINDOWSETTINGS_H
#define WINDOWSETTINGS_H

#include <QDialog>
#include <port.h>

#include "dataprocessor.h"
#include "windfile.h"

namespace Ui {
class WindowSettings;
}

class WindowPort : public QDialog
{
    Q_OBJECT

public:
    explicit WindowPort(QWidget *parent = 0);
    ~WindowPort();
    void setSettingsPort();
    bool getIsMicrogate() const;
    void connectPort();
    SettingsChannel getSettingsChannel();
    void changeThreshold(int num,int value);
    NameChannel getNameChan(int num);
    int getThreshold(int num);
    QString getPathRead() const;
    QString getPathWrite() const;
    bool getIsWrite() const;
    bool getIsRead() const;

private slots:
    void on_comboBox_activated(int index);
    void on_comboBoxPort_activated(int index);
    void on_butPathWrite_clicked();
    void on_butPathRead_clicked();
    void on_comboBoxMode_activated(const QString &arg1);
    void on_comboBoxCreateRecord_activated(const QString &arg1);

signals:
    void sigSendMessage(QString strMessage);

public:
    Port *m_comPort;

private:
    Ui::WindowSettings *ui;
    SettingsPort m_settingsPort;
    SettingsChannel m_settingsChan;
    bool m_isMicroGate;
    bool m_isOpenPort;
    bool m_isWriteRecord;
    bool m_isReadRecord;
    WindFile *m_windFile;
    ModeReading m_mode;

private:
    void clearNamesChannel();
};

#endif // WINDOWSETTINGS_H

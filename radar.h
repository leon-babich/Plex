#ifndef RADAR_H
#define RADAR_H

#include <QObject>
#include <QDebug>
#include <QTimer>

#include "dataprocessor.h"

enum ModeMovingRadar { SMOOTHLY, INDIGREES };
const int N_TAB = 4;

struct FindingTarget
{
    const int N = 4;
    QPair<float,float> list[3];
    bool isVisible;
    bool isTarget;
    int turn;
};

struct TabTarget
{
    unsigned int num;
    float az;
    float range;
    QString time;
    bool isEnd;
    unsigned int rotation;
};

const float ANG_NORM = 0.53;
const int AZ_NORM = 345;
const int ANGL_NORM = 31;
const int SEC = 1000;

struct Cursor
{
    const int AZIMUTH = AZ_NORM;
    const int ANGLE = ANGL_NORM;
    const float S_ANG = ANG_NORM;
    const qreal FACT_DIF_AZ = 0.05;
    int speed = 14;
    float sAngNorm = S_ANG;
    float sAng = S_ANG;
    int middleAz /*= AZ_NORM*/;
    int usualAz /*= AZ_NORM*/;
    int countAz = 0;
    int bufAz = 0;
    int countRepeat = 0;
    int lastDif = 0;
    float timeRotation = 10 * SEC;

    QTimer *timerMove;
    QTimer *timerCorrect; 
};

struct CursorR18
{
    const int N_CADR = 25;
    const float M_ANG = 2.4;
    float spAng;
    int spTimer;
    int targAng;
    bool isStart = false;
    float bufStAng;
    float midSpAng;
    int cMid;
    int cExt;
    float remExtSpAng;
    QTimer *timerMove;
};

class Radar : public QObject
{
    Q_OBJECT
public:
    explicit Radar(QObject *parent = nullptr);
    void clearRadar();
    void clearArrayTargetsImg();
    float getDAng();
    void setMoving(bool moving);
    void setStartAngle(int angle);
    void setTimeBlockTarg(QTime* time);
    void setArrayTargets(vector<short> *array);
    void setTypeRadar(TypeRadar typeRadar);
    void setWritingTarg(bool writing);
    void setPathFileTarg(QString path);
    void setNewTimePath();

private:
    void updataTargets(int angle);
    void correctSpeedCursor(int difference);
    void updataCursor();
    void eraseListDetTarg(int rotation);
    void writingTargets(int n);
    void updataArrTargets(int ang);
    void setAngle(int ang);

public slots:
    void slotMove();
    void slotMoveR18();
    void slotCheckNorthCursor(int turn);
    void slotAddBlockTarget(vector<Target*> block);
    void slotSetSpeedCursor(int rotation, int az);
    void slotSetSpeedCursorR18(float ang);
    void slotSetCorrectARP(bool correct);
    void slotSetCountAngleR18(float angle);
    void slotSetModeMoving();

private slots:
    void slotBlinkCursor();
    void slotStopCorrect();

signals:
    void sigUpdata();
    void sigChangedTabTar(int n);

private:
    bool m_isMovingCursor;
    bool m_isMoving;
    bool m_isCorrectARP;
    bool m_isWriteTargets;
    bool m_isPrintedTitleRecTar;
    int m_numTarg;
    int m_startAngle;
    int m_azimuth;
    float m_countAngR18;
    int m_countArrTar;
    int m_remAng;
    int m_remPosEndTarg;

    QString m_pathRecTar;
    QString m_totalPathRecTar;
    TypeRadar m_typeRadar;
    ModeMovingRadar m_modeMoving;

    QTime *m_timeBlockTarg;           //Время выполнения блока целей
    QTimer *m_timerMarkARP;

public:
    float m_angle;
    QList<FindingTarget*> m_listTargets;
    QVector<TabTarget*> m_listTabTargets[N_TAB];
    bool m_isMarkARP;
    float m_realAngle;
    vector<short> *vecTargets;
    vector<short> m_arrayTargetsImg;
    int m_posArrTarg;
    int sizeArrTar;
    int maxVideo;
    float xPosRadar;
    float yPosRadar;

    float m_difValue = 0;
    int m_numberRotation;
    int m_tabRotation;
    bool m_isAllTabTar;
    int m_dif = 0;
    int m_timeDif = 0;

    float vertexesTar[2000 * WIDCUR * 2];
    float vertexesTarCol[2000 * WIDCUR * 3];

    Cursor m_cursor;
    CursorR18 m_cursR18;
};

#endif // RADAR_H

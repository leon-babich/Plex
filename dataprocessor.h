#ifndef GRAPHICTHREAD_H
#define GRAPHICTHREAD_H

#include <QObject>
#include <QVector>
#include <QDebug>
#include <QTime>
#include <QFile>
#include <QMutex>
#include <QThread>
#include <QMessageBox>
#include <vector>
#include <map>
#include <math.h>
#include <exception>
#include <algorithm>

using std::vector;
using std::map;
using std::move;
using std::exception;

#include "datareader.h"

const int LENSH = 100000;
const short WIDSH = 600;
const short N_PART = 9;
const int LENCUR = 10000;
const int WIDCUR = 2000;
const int CHAN = 8;
const int PROC_CHAN = 7;
const int S_SIN = 15;
const int N_ANALOG = 4;
const int N_SIN = 3;
const int N_MIN_ROT = 5;
const qreal P = 3.14159265;
const int N_A = 50;

enum NameChannel { ACP, ARP, VIDEO, TRIGGER, OTHER };
enum NameChannelR18 { R1, R2, S1, S2, S3, VIDEO_R, TRIGGER_R, R, S1P, S2P, S3P, S3_S2, S2_S1, S1_S3 };
enum ModeScreen { RADAR, TABLE_TARG, GRAPH, GRAPH_CURVE, PROCESSING };
enum TypeRadar { TPS_63, R_18 };
enum TypeArr { DATA_ARRAY, PROC_ARRAY_R, PROC_ARRAY };

struct SettingsChannel
{
    int azimuth;
    int north;
    int target;
    int trigger;
    int numberTPSChannel[4];
    int numberR18Channel[14];
    int valueThreshold[8];
    QMap<int,QString> nameChannel;
    qreal filter[4];
};

struct MaxSignal
{
    const int N = 10;
    int n = 0;
    const qreal factor1 = 0.992;
    const qreal factor2 = 0.97;
    int countMax[8][10];
    int interimMax[8];
    int constantMax[8];
    bool isRecount[8];
    bool isFull = false;
};

struct WidthSignal
{
    const int N = 10;
    int count[8][10];
    int maxWidth[8];
    int midWidth[8];
    bool isCounting[8];
    bool isFull[8];
    int n[8];
};

struct Target
{
    float az[3];
    float rang[3];
    bool isTarget;
    short value;
    short space;
};

struct PointSin
{
    float x, y;
    bool wron;
    int count;
    int chan;
};

struct FindTarget
{
    const int NUMEMT = 12000;
    short numTarg = 50;
    const float D_RANGE = 0.5;
    QVector<Target*> bufferRange;
    vector<Target*> buffer;
    QList<QPair<float,float>*> bufferDetected;
};

struct RadarMask
{
    float az[2];
    float rang[2];
};

struct PicSinusoida
{
    short maxSinM1[N_SIN];
    short minSinM1[N_SIN];
    short maxSinM2[N_SIN];
    short minSinM2[N_SIN];
    int nMed = 0;
    short maxSinP[N_ANALOG];
    short minSinP[N_ANALOG];
    vector<short> bufMax[N_SIN];
    vector<short> bufMin[N_SIN];
    bool isFind[N_SIN];
    bool isAllFind = false;
    int nBlock = 0;
    const float DIF = 1.1;
};

struct MicroSinusoida
{
    float tMax;
    float tMin;
    unsigned int pMax;
    unsigned int pMin;
    bool sinch;

};

const short MINV = -32000;
const short MAXV = 32000;
const int N_SAMPLE_MSIN = 50;

struct ModulatedSinusoida
{
    QVector<MicroSinusoida> bufSin;
    vector<float> listSin;
    vector<float> listSinReg;
    vector<float> listSinReal;
    int lenArc;
    bool isFindVertex = false;
    int remain = 0;
    int pos = 0;
    int nBlockReg = 0;
    int nReg = 0;
    int lastReg = 0;
    unsigned int nSin;
    bool totSinch;
    int nSinch;
    float top;
    bool isRise;
    unsigned int range;
    float memVal;
    float x0 = 0, x1 = 0;
};

struct CountSinusoida
{
    vector<float> listLenArc;
    int totPer = 0;
    int countPer = 0;
    int countDev = 0;
    int memDev = 0;
    int lenArc = 0;
    int len[3];
    bool isCountLen[3];
    bool isStart = true;
    float w[3];
    float t[3];
    const float N = 800;
};

struct SectorSins
{
    int nam;
    int namS;
    int nSect;
    int nSect2;
    int angle;
    int angle2;
};

const short int NUM = 8;

class DataProcessor : public QObject
{
    Q_OBJECT
public:
    explicit DataProcessor(QObject *parent = nullptr);
    ~DataProcessor();
    void updataCycle();
    void nullifyCountingExcesses();
    bool startFile();
    void clearData();

    void setRunning(bool running);
    void setDisplay(bool display);
    void setWriting(bool writing);
    void setValueThresholds(int channels[8]);
    void setIsReadyData(bool ready);
    void setBoardData(BoardData *data);
    void setMode(ModeReading mode);
    void setPaintChan(int num,bool isPaint);
    void setPath(QString path);
    void setStop();
    void setRecountTarget();
    void setNumTarg(int num);
    void setModeScreen(ModeScreen *mode);
    void setListMask(QList<RadarMask*> &list);
    void setStartAzimuth(int angle);
    void setIsProtectCorrectARP(bool protecting);
    void setTypeRadar(TypeRadar typeRadar);
    void setIsReduceSigSR18(bool reduce);
    void setIsReduceSigRR18(bool reduce);
    void setChannelsR18(int channels[7]);

    vector<short>* getArrayTargets();
    int getTimeRun() const;
    int getTimeClear() const;
    QTime* getTimeBlockTarg();
    const QMap<int,QString>* getNameChannel();
    const QVector<QVector<short int>>* getVecLine() const;
    const QVector<QVector<short int>>* getVecLineR18() const;
    vector<float>* getVecSin(int nVec);
    vector<float>* getVecSinReal(int nVec);
    const int* getThresholdChannel();
    bool* getIsPaint();
    SettingsChannel* getSettingsChannel();
    const int* getNumbersTPSChannels();
    const int* getNumbersR18Channels();

private:
    void decreaseBlock();                                     //урезание массива данных до оптимального размера
    void setReadyBlock();                                     //получение урезанного массива данных при чтении из файла
    void handler();                                           //обработка данных
    void fileWriter();                                        //запись данных в файл
    void pushPointToLine(short int value,short int num);
    void clearArrayTargets();
    void handlerTPS63();
    void handlerR18();
    void countSignalR18();
    bool correlationA(int nArr, int i, float f);
    bool correlationP(int nArr, int i, double f);
    void correlationT();
    void showDinamicArray(float *array, int s);

public slots:
    void runProcessing();

private slots:
    void updateThreshold();
    void updateCountValue();
    void recountThreshold();
    void recountTarget();
    void helpCountWidth();
    void helpClearImpuls();
    void detectTarget(short valueTarget);
    void pushTarget(Target *target);
    void pushDetectTarget(float az, float rang);
    void sendBlockTarget();
    void sendBlockDetectTarget();

signals:
    void sigDataProcessed(int start,int width);
    void signalFinishProcessing();
    void sigSendMessage(QString strMessage);
    void sigDisplayErrorMessage(QString strMessage);
    void sigReadyData();
    void sigStartThread();
    void sigRotationCounting(int excesses[]);
    void sigFoundTarger(bool isNorth, qreal az, qreal dist);
    void sigValueTarget(Target* target, bool isTarget);
    void sigBlockTarget(vector<Target*> block);
    void sigBlockDetectTarget(QList<QPair<float,float>*> list);
    void sigFoundARP(int turn);
    void sigTimeRotation(int time, int az);
    void sigPacketFinish(QVector<QVector<short int>> &vecLine);
    void sigSendAngles(vector<float> &angles);
    void sigSetSinchData(bool sinch[N_SIN]);
    void sigCountPack(int excesses[]);
    void sigCountPRF(int count);
    void sigSetChanges();
    void sigCurrentAzimuth(int rotatation, int az);
    void sigValueSignal(int max[], int noise[]);
    void sigShowImpuls(int *impuls);
    void sigTime(int time);
    void sigCorrectARP(bool correct);
//    void sigSendTopsSin(vector<float>* vec[PROC_CHAN], bool isCorrectData);
    void sigSendCountAngle(float angle);
    void sigSendPoints(QVector<PointSin> &vec);
    void sigSetSizeSin(int n);

private:
    QVector<QVector<short>> m_vecLine;
    QVector<QVector<short>> m_vecLineR18;
    QVector<short int> m_buf[CHAN + PROC_CHAN];
    vector<short> m_arrayTargets;
    vector<float> m_vecAngle;
    vector<float> m_vecRealAngle;
    QVector<PointSin> m_vecPoint;
    QList<RadarMask*> *m_listMask;
    BoardData *pBoardData;
    SectorSins sectors[3];
    short int m_cutDataArray[WIDTHARRAY][LENGTHARRAY/S];
    short arrTest[LENGTHARRAY/S];
    short arrTestSwing[LENGTHARRAY/S];
    float m_procesDataArray[PROC_CHAN][LENGTHARRAY/S];

    bool m_isRunning;
    bool m_isWorking;
    bool m_isWriting;
    bool m_isdisplay;
    bool m_isReadyData;
    bool m_isPage;
    bool m_isTrigger;
    bool m_isPaint[CHAN + PROC_CHAN];
    bool m_isStart;
    bool m_isCorrectARP;
    bool m_isProtectCheckSig;
    bool m_isReduceSigSR18;
    bool m_isReduceSigRR18;
//    bool m_isCorrelation = true;

    int m_lengthStringTarget;
    int m_widthStringTarget;
    int m_timeRun;
    int m_timeClear;
    int m_countAzimuth;
    int m_lastInd;
    int m_colDist;
    int m_sizeBuf;
    int m_indVecLine;
    int m_sizeLine;
    int m_timeNorth;
    int m_timeNorthCurrent;
    int m_countExcess[8];
    int m_countN[8];
    int m_countNoise[8];
    int value[4];
    int m_countPRF;
    int m_isCountRPR;
    short int m_numPaintChan;
    int m_curentAzimuth;
    int m_rotation;
    int m_timeReading;
    int m_startAngle;
    float m_countAngleR18;
    int m_sectorAngle;
    int countCorel = 0;
    int countCorel2 = 0;

    SettingsChannel m_settings;
    ModeReading modeRead;
    MaxSignal m_maxSignal;
    WidthSignal m_width;
    FindTarget m_target;
    ModeScreen *m_pointModeScreen;
    TypeRadar m_typeRadar;
    PicSinusoida m_pSin;
    ModulatedSinusoida m_mSin[PROC_CHAN];
    CountSinusoida m_cSin;

    QString m_path;
    QFile *m_fileIn;
    QMutex *m_pMutex;
    QTime *m_timerNorth;
    QTime *m_timeBlockTarg;

public:
    bool autoValue[4];
};

#endif // GRAPHICTHREAD_H

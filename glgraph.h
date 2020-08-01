#ifndef GLGRAPH_H
#define GLGRAPH_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QVector>
#include <QPair>
#include <map>
#include <QOpenGLWidget>
#include <QWheelEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QDebug>
#include <vector>

#include "radar.h"

const int STEP = 100;

struct Scale
{
    float factScale;
    float xShiftScale;
    float yShiftScale;
    float dxShift;
    float dyShift;
    QPoint posCurs;
};

struct GlRgb
{
    unsigned char r, g, b, a;
};

struct Color
{
    float r, g, b;
};

const int SUB = 400;
struct VertexLine
{
    short x1 = 0;
    short y1 = SUB;
    short x2 = 0;
    short y2 = 0;
};

struct Inscription
{
    short pos;
    float posDigit;
    float inscription;
    bool isActive;
    bool isVisible;
    short rang;
};

enum ModeLocationButtons { BOTTOM, SIDE, WITHOUT };
enum TypeButtonGraph { ACP_BUT, ARP_BUT, VIDEO_BUT, TRIGGER_BUT, SWITCH_BUT, CH1, CH2, CH3, CH4, CH5, CH6, CH7, CH8,
                       R1_BUT, R2_BUT, S1_BUT, S2_BUT, S3_BUT, VIDEO_R_BUT, TRIGGER_R_BUT, R_BUT,
                       S1P_BUT, S2P_BUT, S3P_BUT, S3_S2_BUT, S2_S1_BUT, S1_S3_BUT, MODE_GRAPH_BUT, SIN_BUT, SIN2_BUT, SCALE_BUT };
enum TypeButtonRadar { RADAR_BUT, TAB_BUT, GRAPH_BUT, GRAPH2_BUT, LED_BUT, EMPTY_BUT, LOCATION_BUT };
enum TypeData { PRF, IMPULS, ROTARION, AZIMUTH, NORTH, TARGET, TRIG };
enum TypeTrigger { R0, XMIT };
enum ModeGraph { LINE_MODE, POINT_MODE };
enum TypeSinisoida { MODELING, MODEL };

struct ButtonGraph
{
    TypeButtonGraph type;
    QRectF location;
    QRectF locationSc;
    bool isOn;
    float color[3];
};

struct GraphSlider : public ButtonGraph
{
    QRectF placeSlider;
    QRectF positionSlider;
    QRectF butZoomIn;
    QRectF butZoomOut;
    QRectF placeSliderSc;
    QRectF positionSliderSc;
    QRectF butZoomInSc;
    QRectF butZoomOutSc;
    float posSlider;
    int numButZoom = 1;
    QTimer *timerClick;
};

struct ButScroll
{
    QRectF rectCom;
    QRectF rectArrow1;
    QRectF rectArrow2;
    QRectF rectStrip;
    QRectF rectSlid;
    float normHeight;
    float posSlid;
    bool isScroll = false;
    int nArr = 1;
    float yShould = 0;
    QTimer *timer1;
    QTimer *timer2;
};

struct ButtonRadar
{
    TypeButtonRadar type;
    QString tite;
    QRectF location;
    QRectF locationSc;
    bool choose;
};

struct ImageScreen
{
    float F_H = 0.12f;
    float F_HC = 0.05f;
    float F_W = 0.07f;
    float F_WC = 0.0f;
    float fW = 0.06f;
    float fH = 0.12f;
    float fW_2 = 0.06f;
    float widthField;
    float heightField;
    float widthField_2;
    float widthWind;
    float heightWind;
    float globWidthWind;
    float globHeightWind;
    float globWidthField;
    float fScX;
    float fScY;
};

struct DataTable
{
    int PRF;
    int widthImpTrig;
    float timeRotation;
    int ACP;
    int ARP;
    int video;
    int trigger;
    TypeTrigger typeTrigger;
};

struct ParamTabTar
{
    const int N1 = 27;
    const int N2 = 24;
    float width;
    float height, height2;
    float x, x2, y;
    int numStSl = 0, numStSl2 = 0;
    bool isEndSl = true;
};

struct AnalogGraph
{
    float x1, y1, x2, y2;
    float xg1, yg1, xg2, yg2;
    bool isNormalSize;
    float xInf, yInf;
    bool isDrawInf;
    float factScr;
    unsigned int sizeVec;
    bool isActive;
    bool isClick;
    bool isHighlightLine[PROC_CHAN];
};

class GlGraph : public QOpenGLWidget
{
    Q_OBJECT
public:
    explicit GlGraph(QWidget *parent = 0);
    ~GlGraph();
    void mousePressEvent(QMouseEvent *ev);
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void wheelEvent(QWheelEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *ev);
    ModeScreen* getModeScreen();
    void setPaintChan(int num,bool isPaint);
    void setThreshold(const int *threshold);
    void resizeGraphVec(const QVector<QVector<short>> *vec);
    void setData(float data, TypeData type);
    void setListMask(QList<RadarMask*> &list);
    void setArrayTargets(vector<short> *array);
    void setNoiseFilterTarg(int value);
    void setTypeRadar(TypeRadar typeRadar);
    void setNumbersTPSChannel(const int* numbersChannel);
    void setNumbersR18Channel(const int* numbersChannel);
    void setVecSin(vector<float> *vec, int nVec);
    void setVecSinReal(vector<float> *vec, int nVec);
    void setIsConnectPort(int n);
    void setMaxVideo(int value);
    void switchModeScreen(bool up);
    void switchOnAnalog();
    void runImage(bool running);
    void placeButtons();

private:
    void drawRadar();
    void drawGraphick();
    void drawCurveTargets();
    void drawTableTargets();
    void drawIndicator();
    void drawFrame(float x, float y, float w, float h);
    void drawCoordinateGrid();
    void drawGraphickTableSignals();
    void drawRadarTableInfo();
    void drawRadarTestCursorTable();
    void drawIndicatorTypeRadar();
    void drawAnalogGraph();
    void drawAnalogGraphSin();

    void drawGraphicInscription();
    void drawGraphicCurveInscription();
    void createAtributsBut();
    void createTitlesGraph();
    void createTitlesCurve();
    void recountGraphicTitles();
    void createAnalogWindows();
    void setSizeAnalogWindows();

    void placeSlider();
    void createSizeWindImages();
    void createButtons();
    void createButton(TypeButtonRadar type, QString title);
    void resizeButton(TypeButtonRadar type);
    void drawButtons();
    void drawButton(TypeButtonRadar type);
    void createGraphButtons();
    void createGraphButton(float x, float y, float w, float h, TypeButtonGraph name, float col[3]);
    void resizeGraphButton(TypeButtonGraph name);
    void drawGraphButton(TypeButtonGraph name);
    void drawGraphButtons();
    void recountScaleSizeButtons();

    void activeButton(TypeButtonGraph type);
    void drawline(const QVector<short int> &vec, Color c, int n);
    void drawAnalogLine(const QVector<short> &vec, Color c);
    void drawAnalogSin(const vector<float> &vec, Color c, int num);
//    void drawAnalogSin(short vec[N_SIN][30], int n, Color c);
    void drawTableSting(float wTab, float hTab, QString title, QString strNumber, int num, int totalNumStr, QPainter &painter);
    void drawTargets(float xCent, float yCent);
    void fullTarget(QVector<float> &vecPoints, float f, float s);
    void drawDesignationRadar(float xCent, float yCent, float radius);
    void drawRadarMask(float xCentr, float yCentr);
    Color countColor(short value);
    void paintMask(float xCent, float yCent, float az1, float az2, float rang1, float rang2);
    void paintCursor(float x, float y, float rad, float stAng);
    void printString(float x, float y, const char* s);
    void printTablString(float x, float y, float widthTable, QString title, QString strNumber);
    void drawTableAngles();
    void paintContourTabTar(int x, int y, bool isAllTarg);
    void paintScrolTabTar(int n);
    void paintSymbolPort();
    void helpScroll(int nArr);
    void helpScale(int numButZoom);
    void paintAnalogInf();

    void checkBoardGraphick();
    void checkBoardCurve();
    void scaleGraph();
    void scaleCurve();
    void zoomGraph(float factorScale);
    void zoomGraphCurve(float factorScale);
    void recountGraphick();

    void cutString(QString &str, int w);
    void makeRasterFont();
    void drawCircle(float xCen, float yCen, float radius);
    void drawBlendCircle(float xCen, float yCen, float radius, float posAng);
    void paintCircle(float xCen, float yCen, float radius);
    void paintBlendCircle(float xCen, float yCen, float radius, float posAng);
    void paintEllipse(float xC, float yC, float rx, float ry);
    void paintRotateEllipse(float xC, float yC, float rx, float ry, float angle);

public slots:
    void slotSetLine(QVector<QVector<short int>> &vecLine);
    void slotReceiveAngles(vector<float> &angles);
    void slotUpdata();
    void slotSetTimeBoard(int time);
    void slotSetTimeProcessor(qreal time);
    void slotSetButLayout(ModeLocationButtons mode);
    void slotSetSinchData(bool sinch[N_SIN]);
//    void slotSetVecSin(vector<float>* vec[PROC_CHAN], bool isCorrectData);
    void slotSetCountAngleR18(float angle);
    void slotSetPoints(QVector<PointSin> &vec);
    void slotSetSizeSin(int n);

private slots:
    void slotUpdataImgGraph();
    void slotUpdataImgRadar();
    void slotChangedTabTar(int n);
    void slotRunScroll();
    void slotRunStepScroll();
    void slotButZoom();

signals:
    void sigSetChannel(short num, bool isActive);
    void sigDisplayErrorMessage(QString strMessage);

protected:
    virtual void initializedGL();
    virtual void resizeGL(int w, int h);
    virtual void paintGL();

private:
    bool m_chan[CHAN + PROC_CHAN];
    bool m_isUpdate;
    bool m_isMoveMouse;
    bool m_isAllChannelButtons;
    bool m_isInfTable;
    bool m_isTabTargRadar;
    bool m_isSliderScale;
    Color m_colorFond;
    Scale m_scale;
    Scale m_scaleCurve;
    DataTable m_dataTable;
    ParamTabTar m_tabTar;
    ImageScreen m_curveSc;
    ImageScreen m_graghSc;
    ModeScreen m_modeScreen;
    TypeButtonRadar m_choosedBut;
    ModeLocationButtons m_modeLocBut;
    GraphSlider m_butScale;
    ButScroll m_butScroll;
    ButScroll m_butScroll2;
    TypeRadar m_typeRadar;
    ModeGraph m_modeGraph;
    AnalogGraph m_analogGraph;
    AnalogGraph m_analogGraphSin;
    VertexLine vertexes[CHAN + PROC_CHAN][LENSH];
    QTimer *m_timerImgGraph;
    QTimer *m_timerImgRadar;
    QTime m_timePaint;
    const char* TITLEBUT[18];
    float vertexesTar[2000 * WIDCUR * 2];
    float vertexesTarCol[2000 * WIDCUR * 3];
    GLuint fontOffset;
    QVector<QVector<short>> m_vecLine;
    QVector<QVector<short>> m_vecLineInter;
    QVector<Inscription> m_listGraphicY;
    QVector<Inscription> m_listGraphicX;
    QVector<Inscription> m_listGraphicCurveY;
    QVector<Inscription> m_listGraphicCurveX;
    QVector<ButtonGraph> m_listButGraph;
    QVector<ButtonRadar> m_listBut;
    QVector<Color> m_colorsBut_1;
    QVector<Color> m_colorsBut_2;
//    vector<vector<short>> *m_pointerArrayTargets;
    vector<short> *m_imageTargets;
    vector<float> m_vecAngles;
//    vector<float> m_vecSin[PROC_CHAN];
    vector<float> *m_pVecSin[PROC_CHAN];
    vector<float> *m_pVecSinReal[PROC_CHAN];
    QList<RadarMask*> *m_listMask;
    QList<QPair<float, float>> listMarks;
    QVector<PointSin> m_vecPoint;
    const int *m_numbersTPSChannel;
    const int *m_numbersR18Channel;
    float m_scFactX;
    float m_scFactY;
    const int *m_threshold;
    int m_widthShedule;
    int m_heightShedule;
    int m_widthScreen;
    int m_heightScreen;
//    float m_fIncrease;
//    float m_fIncreaseY;
//    float m_rangeIncreaseX;
//    float m_rangeIncreaseY;
    float m_shiftButton;
    float m_factSides;
    int m_posUpdata;
    int m_sizeLine;
    int m_sizeLineR18;
    int m_sizeVec;
    int m_sizeVecR18;
    int m_sizeLineInt;
    int m_sizeLineIntR18;
    char m_numPart;
    float m_yPosScreen;
    float m_xPosScreen;
    int m_widthButGraph;
    int m_heightButGraph;
    int m_widthBut;
    int m_heightBut;
    float m_factSidesReal;
    float m_scFactRadar;
    float m_widthTabInf;
    float m_widthTabInfRadar;
    int m_timeBoard;
    int m_timeProcessor;
    float m_fact;
    int m_startAngle;
    int m_filterTarg;
    bool m_isNormGraph;
    bool m_isSinch[3];
    int m_poinPartSin;
    int m_sizeSin;
    int m_nVecSin[PROC_CHAN];
    float m_countAngle;
    bool m_isConnectPort;
    int m_numPorts;
    int m_maxVideo;

public:
    Radar *radar;
};

#endif // GLGRAPHICK_H

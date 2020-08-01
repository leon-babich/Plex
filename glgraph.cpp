#include "glgraph.h"

GlGraph::GlGraph(QWidget *parent) :
    QOpenGLWidget(parent)
{
    this->setMouseTracking(true);
    m_widthShedule = LENSH/N_PART;
    m_heightShedule = WIDSH;
//    m_fIncrease = 0.08;
//    m_fIncreaseY = 0.05;
    m_widthScreen = 1000;
    m_heightScreen = 700;
    m_fact = m_widthShedule/m_widthScreen;
//    m_rangeIncreaseX = m_widthShedule/m_fact * m_fIncrease;
//    m_rangeIncreaseY = m_heightShedule * m_fIncreaseY;
//    m_heightScreen = m_heightShedule + m_rangeIncreaseY;
    m_factSides = m_widthScreen/m_heightScreen;
    m_modeLocBut = BOTTOM;
    m_modeGraph = LINE_MODE;
    createSizeWindImages();
    m_scFactX = 1.0;
    m_scFactY = 1.0;
    m_posUpdata = 0;
    m_sizeVec = 0;
    m_sizeVecR18 = 0;
    m_sizeLine = 0;
    m_sizeLineR18 = 0;
    m_numPart = N_PART;
    m_sizeLineInt = 0;
    m_sizeLineIntR18 = 0;
    m_scale.factScale = 1.0;
    m_scale.dxShift = 0;
    m_scale.dyShift = 0;
    m_scale.xShiftScale = 0.0;
    m_scale.yShiftScale = 0.0;
    m_scaleCurve.factScale = 1.0;
    m_scaleCurve.dxShift = 0;
    m_scaleCurve.dyShift = 0;
    m_scaleCurve.xShiftScale = 0.0;
    m_scaleCurve.yShiftScale = 0.0;
//    m_widthTabTar = m_widthScreen/2 - 50;
//    m_heightTabTar = m_heightScreen - 50;
    m_tabTar.width = m_widthScreen/2 - 50;
    m_tabTar.height = m_heightScreen - 40;
    m_tabTar.height2 = m_heightScreen - 100;
    m_tabTar.x = 5;
    m_tabTar.x2 = m_widthScreen/2 - 20;
    m_tabTar.y = 5;
    m_widthTabInf = 230;
    m_widthTabInfRadar = 150;
    m_yPosScreen = m_heightShedule/2;
    m_xPosScreen = m_widthShedule/2;
    m_widthButGraph = m_widthScreen/45;
    m_heightButGraph = m_heightScreen/35;
    m_scFactRadar = 1.0;
    m_timeBoard = 0;
    m_startAngle = 0;
    m_timeProcessor = 0;
    m_filterTarg = 100;
    m_countAngle = 0.0;
    m_maxVideo = 3000;
    m_sizeSin = S_SIN;
    m_modeScreen = RADAR;
    m_choosedBut = RADAR_BUT;
    m_typeRadar = TPS_63;
    m_isUpdate = true;
    m_isMoveMouse = false;
    m_isInfTable = false;
    m_isAllChannelButtons = true;
    m_isTabTargRadar = false;
    m_isSliderScale = false;
    m_isNormGraph = true;
    m_isConnectPort = true;
    m_numPorts = 0;
    radar = new Radar();
    m_dataTable = { 0, 0, 0.0, 0, 0, 0, 0, R0 };
    m_colorFond = { 0.1, 0.1, 0.1 };

    for(int i=0; i<3; i++) {
        m_isSinch[i] = false;
    }

    for(int i = 0; i < CHAN + PROC_CHAN; i++) {
        m_chan[i] = false;
    }

    createAnalogWindows();
    createAtributsBut();
    createTitlesGraph();
    createTitlesCurve();
    createButtons();
    createGraphButtons();
    zoomGraph(2.0);

    m_timerImgGraph = new QTimer;
    connect(m_timerImgGraph, &QTimer::timeout, this, &GlGraph::slotUpdataImgGraph);

    m_timerImgRadar = new QTimer;
    connect(m_timerImgRadar, &QTimer::timeout, this, &GlGraph::slotUpdataImgRadar);

    m_butScroll.timer1 = new QTimer;
    connect(m_butScroll.timer1, &QTimer::timeout, this, &GlGraph::slotRunScroll);
    m_butScroll.timer2 = new QTimer;
    connect(m_butScroll.timer2, &QTimer::timeout, this, &GlGraph::slotRunStepScroll);

    m_butScale.timerClick = new QTimer;
    connect(m_butScale.timerClick, &QTimer::timeout, this, &GlGraph::slotButZoom);

    connect(radar,&Radar::sigUpdata,this,&GlGraph::slotUpdata);
    connect(radar,&Radar::sigChangedTabTar,this,&GlGraph::slotChangedTabTar);
}

GlGraph::~GlGraph()
{
}

void GlGraph::mousePressEvent(QMouseEvent *ev)
{
    m_scale.posCurs = ev->pos();

    if(m_modeScreen == GRAPH_CURVE) {
        m_scaleCurve.posCurs = ev->pos();
    }

    float xPos = ev->pos().x() * m_scFactX;
    float yPos = ev->pos().y() * m_scFactY;

    if(ev->button() == Qt::LeftButton) {
        QRectF locRad = m_listBut[RADAR_BUT].locationSc;
        QRectF locTab = m_listBut[TAB_BUT].locationSc;
        QRectF locGr = m_listBut[GRAPH_BUT].locationSc;
        QRectF locCur = m_listBut[GRAPH2_BUT].locationSc;
        QRectF locPro = m_listBut[LED_BUT].locationSc;

        if(xPos>locRad.x() && xPos<(locRad.x()+locRad.width()) && yPos<locRad.y() && yPos>(locRad.y()-locRad.height())) {
            m_modeScreen = RADAR;
            m_choosedBut = RADAR_BUT;
            update();
        }
        else if(xPos>locTab.x() && xPos<(locTab.x()+locTab.width()) && yPos<locTab.y() && yPos>(locTab.y()-locTab.height())) {
            m_modeScreen = TABLE_TARG;
            m_choosedBut = TAB_BUT;
            update();
        }
        else if(xPos>locGr.x() && xPos<(locGr.x()+locGr.width()) && yPos<locGr.y() && yPos>(locGr.y()-locGr.height())) {
            m_modeScreen = GRAPH;
            m_choosedBut = GRAPH_BUT;
            update();
        }
        else if(xPos>locCur.x() && xPos<(locCur.x()+locCur.width()) && yPos<locCur.y() && yPos>(locCur.y()-locCur.height())) {
            m_scFactRadar = 0.4;
            m_modeScreen = GRAPH_CURVE;
            m_choosedBut = GRAPH2_BUT;
            update();
        }
        else if(xPos>locPro.x() && xPos<(locPro.x()+locPro.width()) && yPos<locPro.y() && yPos>(locPro.y()-locPro.height())) {;
            m_modeScreen = PROCESSING;
            m_choosedBut = LED_BUT;
            update();
        }

        if(m_modeScreen == GRAPH) {
            int s, f;

            if(m_typeRadar == TPS_63) {
                s = !m_isAllChannelButtons ? 0 : CH1;
                f = !m_isAllChannelButtons ? SWITCH_BUT : R1_BUT;
            }
            else if(m_typeRadar == R_18) {
                s = !m_isAllChannelButtons ? R1_BUT : CH1;
                f = !m_isAllChannelButtons ? MODE_GRAPH_BUT : R1_BUT;
            }

            for(int i=s; i<f; i++) {
                QRectF rect = m_listButGraph[i].locationSc;

                if(xPos > rect.x() && xPos < (rect.x()+rect.width()) && yPos < rect.y() && yPos > (rect.y()-rect.height())) {
                    activeButton((TypeButtonGraph)i);
                    return;
                }
            }

            QRectF locSw = m_listButGraph[SWITCH_BUT].locationSc;
            if(xPos > locSw.x() && xPos < (locSw.x()+locSw.width()) && yPos < locSw.y() && yPos>(locSw.y()-locSw.height())) {
                activeButton(SWITCH_BUT);
                return;
            }
            QRectF locW = m_listButGraph[MODE_GRAPH_BUT].locationSc;
            if(xPos > locW.x() && xPos < (locW.x()+locW.width()) && yPos < locW.y() && yPos>(locW.y()-locW.height())) {
                activeButton(MODE_GRAPH_BUT);
                return;
            }
            QRectF locSl = m_butScale.positionSliderSc;
            if(xPos > locSl.x() && xPos < (locSl.x()+locSl.width()) && yPos < locSl.y()-
                    m_butScale.posSlider && yPos > (locSl.y()-m_butScale.posSlider-locSl.height())) {
                m_isSliderScale = true;
                return;
            }
            QRectF locZIn = m_butScale.butZoomInSc;
            if(xPos>locZIn.x() && xPos<(locZIn.x()+locZIn.width()) && yPos>locZIn.y() && yPos<(locZIn.y()+locZIn.height())) {
                m_butScale.numButZoom = 2;
                m_butScale.timerClick->start(50);
                return;
            }
            QRectF locZOut = m_butScale.butZoomOutSc;
            if(xPos>locZOut.x() && xPos<(locZOut.x()+locZOut.width()) && yPos>locZOut.y() && yPos<(locZOut.y()+locZOut.height())) {
                m_butScale.numButZoom = 1;
                m_butScale.timerClick->start(50);
                return;
            }
            QRectF locSin = m_listButGraph[SIN_BUT].locationSc;
            if(m_analogGraphSin.isActive && xPos > locSin.x() && xPos < (locSin.x()+locSin.width()) && yPos < locSin.y() &&
                    yPos>(locSin.y()-locSin.height())) {
                m_analogGraphSin.isClick = true;
                if(m_analogGraphSin.isDrawInf) {
                    m_analogGraphSin.isDrawInf = false;
                    m_analogGraphSin.xInf = 0;
                    m_analogGraphSin.yInf = 0;
                }
                update();
                return;
            }
            QRectF locSin2 = m_listButGraph[SIN2_BUT].locationSc;
            if(m_analogGraphSin.isActive && xPos > locSin2.x() && xPos < (locSin2.x()+locSin2.width()) && yPos < locSin2.y() &&
                    yPos>(locSin2.y()-locSin2.height())) {
                m_analogGraph.isClick = true;
//                if(m_analogGraphSin.isDrawInf) {
//                    m_analogGraphSin.isDrawInf = false;
//                    m_analogGraphSin.xInf = 0;
//                    m_analogGraphSin.yInf = 0;
//                }
                update();
                return;
            }
        }
        else if(m_modeScreen == TABLE_TARG) {
            QRectF locScr = m_butScroll.rectSlid;
            if(xPos>locScr.x() && xPos<(locScr.x()+locScr.width()) && yPos>locScr.y() && yPos<(locScr.y()+locScr.height())) {
                m_butScroll.isScroll = true;
                m_tabTar.isEndSl = false;
                m_butScroll.yShould = yPos - m_butScroll.rectSlid.y();
                return;
            }
            QRectF locAr1 = m_butScroll.rectArrow1;
            if(xPos>locAr1.x() && xPos<(locAr1.x()+locAr1.width()) && yPos>locAr1.y() && yPos<(locAr1.y()+locAr1.height())) {
                helpScroll(1);
                m_butScroll.nArr = 1;
                m_butScroll.timer1->start(600);
                return;
            }
            QRectF locAr2 = m_butScroll.rectArrow2;
            if(xPos>locAr2.x() && xPos<(locAr2.x()+locAr2.width()) && yPos>locAr2.y() && yPos<(locAr2.y()+locAr2.height())) {
                helpScroll(2);
                m_butScroll.nArr = 2;
                m_butScroll.timer1->start(600);
                return;
            }
        }
    }
    else if(ev->button() == Qt::RightButton) {
        m_isUpdate = (m_isUpdate + 1) % 2;
    }
}

void GlGraph::mouseMoveEvent(QMouseEvent *ev)
{
    float xP = ev->pos().x(), yP = ev->pos().y();

    if(ev->buttons() == Qt::LeftButton) {
        float dx = xP - m_scale.posCurs.x();
        float dy = yP - m_scale.posCurs.y();

        if(m_modeScreen == GRAPH) {
            if(m_isSliderScale) {
                float y = ev->pos().y();
                QRectF place = m_butScale.placeSlider;
                y = y < place.y() - place.height() ? place.y() - place.height() : y;
                y = y > place.y() ? place.y() : y;
                float piece = (place.height())/50;
                float valueScale = (place.y() - y)/piece * m_scFactY + 1.0;
                zoomGraph(valueScale);
            }
            else if(m_analogGraphSin.isClick) {
                float xB = xP * m_scFactX, yB = yP * m_scFactY;
                xB = xB >= 0 ? xB : 0;
                float yBoard = m_graghSc.heightWind;
                yB = yB <= yBoard ? yB : yBoard;

                m_analogGraph.x1 = xB;
                m_analogGraph.y1 += (yB - m_analogGraphSin.y1)/2;
                m_analogGraphSin.y2 = m_analogGraph.y1;
                m_analogGraphSin.x1 = xB;
                m_analogGraphSin.y1 = yB;
                m_analogGraphSin.factScr = m_analogGraphSin.sizeVec/(m_analogGraphSin.xg2 - m_analogGraphSin.xg1);

                setSizeAnalogWindows();

                QRectF &rect = m_listButGraph[SIN_BUT].location;
                float w = rect.width(), h = rect.height();
                rect.setRect(m_analogGraphSin.x1, m_analogGraphSin.y1, w, h);
                resizeGraphButton(SIN_BUT);

                QRectF &rect2 = m_listButGraph[SIN2_BUT].location;
                w = rect2.width(); h = rect2.height();
                rect2.setRect(m_analogGraph.x1 + (m_analogGraph.x2 - m_analogGraph.x1)/2 - w/2, m_analogGraph.y1 + 0.3*h/2 * m_scFactY, w, h);
                resizeGraphButton(SIN2_BUT);
            }
            else if(m_analogGraph.isClick) {
                float xB = xP * m_scFactX, yB = yP * m_scFactY;
                xB = xB >= 0 ? xB : 0;
                float yBoard = m_analogGraphSin.y1, yBoard2 = m_analogGraph.y2;
                yB = yB <= yBoard ? yB : yBoard;
                yB = yB >= yBoard2 ? yB : yBoard2;

                m_analogGraph.y1 = yB;
                m_analogGraphSin.y2 = yB;
                m_analogGraphSin.factScr = m_analogGraphSin.sizeVec/(m_analogGraphSin.xg2 - m_analogGraphSin.xg1);

                setSizeAnalogWindows();

                QRectF &rect2 = m_listButGraph[SIN2_BUT].location;
                float w = rect2.width(); float h = rect2.height();
                rect2.setRect(m_analogGraph.x1 + (m_analogGraph.x2 - m_analogGraph.x1)/2 - w/2, m_analogGraph.y1 + 0.3*h/2 * m_scFactY, w, h);
                resizeGraphButton(SIN2_BUT);
            }
            else {
                m_scale.dxShift += dx * 8/m_scale.factScale;
                m_scale.dyShift += dy/2.5/m_scale.factScale;
                m_scale.posCurs = ev->pos();
                m_isMoveMouse = true;
            }
        }
        else if(m_modeScreen == GRAPH_CURVE) {
            float fx = 1;
            float fy = 5;
            m_scaleCurve.dxShift += dx * fx/ m_scFactX;
            m_scaleCurve.dyShift += dy * fy/ m_scFactY;
            m_scaleCurve.posCurs = ev->pos();
            m_isMoveMouse = true;
        }
        else if(m_modeScreen == TABLE_TARG && m_butScroll.isScroll) {
            float y = ev->pos().y() * m_scFactY;
            QRectF &strip = m_butScroll.rectStrip;
            QRectF &slid = m_butScroll.rectSlid;
            float yShould = m_butScroll.yShould;
            float xS = slid.x(), yS = slid.y(), wS = slid.width(), hS = slid.height();
            float yDown = strip.y() + strip.height() - (hS - yShould);
            float yUp = strip.y() + yShould;
            y = y < yDown ? y : yDown;
            y = y > yUp ? y : yUp;
            yS = y - yShould;
            m_butScroll.rectSlid.setRect(xS, yS, wS, hS);
            m_tabTar.numStSl = (yS - strip.y())/strip.height() * radar->m_listTabTargets[1].size();
        }
    }
    else if(m_modeScreen == GRAPH && m_typeRadar == R_18 && !m_isAllChannelButtons) {
        float xPsc = xP * m_scFactX;
        float yPsc = yP * m_scFactY;
        float x1 = m_analogGraphSin.x1, x2 = m_analogGraphSin.x2, y1 = m_analogGraphSin.y1, y2 = m_analogGraphSin.y2;

        if(xPsc>x1 && xPsc<x2 && yPsc<y1 && yPsc>y2) {
            m_analogGraphSin.isDrawInf = true;
            m_analogGraphSin.xInf = xPsc;
            m_analogGraphSin.yInf = yPsc;
        }
        else {
            m_analogGraphSin.isDrawInf = false;
        }
    }

    update();
}

void GlGraph::mouseReleaseEvent(QMouseEvent *ev)
{
    if(m_modeScreen == GRAPH) {
        if(m_isSliderScale) m_isSliderScale = false;
        if(m_butScale.timerClick->isActive()) m_butScale.timerClick->stop();
        if(m_analogGraphSin.isActive && m_analogGraphSin.isClick) m_analogGraphSin.isClick = false;
        if(m_analogGraphSin.isActive && m_analogGraph.isClick) m_analogGraph.isClick = false;
    }
    else if(m_modeScreen == TABLE_TARG) {
        if(m_butScroll.isScroll) {
            m_butScroll.isScroll = false;

            int ySl = m_butScroll.rectSlid.y() + m_butScroll.rectSlid.height();
            int yStr = m_butScroll.rectStrip.y() + m_butScroll.rectStrip.height();

            if(ySl == yStr) m_tabTar.isEndSl = true;
        }

        if(m_butScroll.timer1->isActive()) {
            m_butScroll.timer1->stop();
        }
        if(m_butScroll.timer2->isActive()) {
            m_butScroll.timer2->stop();
        }
    }
}

void GlGraph::wheelEvent(QWheelEvent *event)
{
    float factorScale = event->delta() > 0 ? 1.1 : 1/1.1;

    if(m_modeScreen == GRAPH) {
        float valueScale = m_scale.factScale * factorScale;
        zoomGraph(valueScale);
    }
    else if(m_modeScreen == GRAPH_CURVE) {
        float valueScale = m_scaleCurve.factScale * factorScale;
        zoomGraphCurve(valueScale);
    }
}

void GlGraph::mouseDoubleClickEvent(QMouseEvent *ev)
{
    float x = ev->pos().x() * m_scFactX;
    float y = ev->pos().y() * m_scFactY;

    if((x>=m_analogGraph.x1 && x<=m_analogGraph.x2 && y>=m_analogGraph.y2 && y<=m_analogGraph.y1) || !m_analogGraph.isNormalSize) {
        m_analogGraph.isNormalSize = (m_analogGraph.isNormalSize + 1) % 2;
    }
    else {
        m_modeGraph = (ModeGraph)((m_modeGraph + 1) % 2);
    }

    update();
}

ModeScreen *GlGraph::getModeScreen()
{
    return &m_modeScreen;
}

void GlGraph::initializedGL()
{
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void GlGraph::resizeGL(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(0, 0, (GLint)w, (GLint)h);
    glOrtho(0, m_widthScreen, m_heightScreen, 0, -1, 1);
    m_scFactX = m_widthScreen/(qreal)w;
    m_scFactY = m_heightScreen/(qreal)h;
    m_factSidesReal = (float)m_widthScreen/m_heightScreen * h/w;
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);
    makeRasterFont();
    recountScaleSizeButtons();
    recountGraphicTitles();
}

void GlGraph::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);
    glClearColor(m_colorFond.r, m_colorFond.g, m_colorFond.b, 1.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    try {
        if(m_modeScreen == RADAR) {
            drawRadar();
        }
        else if(m_modeScreen == TABLE_TARG) {
            drawTableTargets();
        }
        else if(m_modeScreen == GRAPH) {
            drawGraphick();
        }
        else if(m_modeScreen == GRAPH_CURVE) {
            drawCurveTargets();
        }
        else if(m_modeScreen == PROCESSING) {
            drawIndicator();
        }

        if(m_modeLocBut != WITHOUT) {
            drawButtons();
        }

        if(m_modeScreen != TABLE_TARG) drawIndicatorTypeRadar();
    }
    catch (ExeptionRange ex) {
        QString message;
        if(ex.getType() == WRONGDATA) {
            message = ex.what() + ex.getInformation() + "\n" + ex.getFunc();
        }
        else if(ex.getType() == OUTRANGE) {
            message = ex.what() + QString::number(ex.getNumElemtnt()) + "\n" + ex.getFunc();
        }
        emit sigDisplayErrorMessage(message);
    }
    catch (const std::exception& ex) {
            QString message = ex.what();
            emit sigDisplayErrorMessage(message);
    }
}

void GlGraph::drawRadar()
{
//    m_timePaint.restart();
    float fScale = m_factSides/m_factSidesReal;
    float xCentr = m_widthScreen * 0.46;
    float yCentr = m_heightScreen * 0.48;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0 * m_factSidesReal, 1.0, 1.0);
    glTranslatef(-xCentr * (1 - fScale), 0.0, 0.0);

    float rad = (float)m_heightScreen/9 * 4/6;
    QVector<float> radius;
    for(int i=0; i<6; i++) {
        float newRad = rad * (i+1);
        radius.push_back(newRad);
    }

    const qreal P = 3.14159265;
    float angle = (radar->m_angle - 90) * P/180;
    float xCurs = xCentr + radius[5] * cos(angle);
    float yCurs = yCentr + radius[5] * sin(angle);

    paintCursor(xCentr, yCentr, radius[5], angle);
    drawDesignationRadar(xCentr, yCentr - 1, radius[5]);

    glColor3f(0.0f, 0.3f, 0.0f);
    glBegin(GL_LINES);
        glVertex2f(xCentr, yCentr);
        glVertex2f(xCurs, yCurs);
    glEnd();

//    float realAngle = (m_radar->m_realAngle - 90) * P/180;
//    float xCurs_real = xCentr + radius[5] * cos(realAngle);
//    float yCurs_real = yCentr + radius[5] * sin(realAngle);
//    glColor3f(0.5f, 0.0f, 0.0f);
//    glBegin(GL_LINES);
//        glVertex2f(xCentr, yCentr);
//        glVertex2f(xCurs_real, yCurs_real);
//    glEnd();
    radar->xPosRadar = xCentr;
    radar->yPosRadar = yCentr;
    drawTargets(xCentr, yCentr);

    float xP;
    float yP;
    glColor3f(0.0f, 0.0f, 1.0f);
    glPointSize(4);
    glBegin(GL_POINTS);
        for(int i=0; i<radius.size(); i++) {
            xP = xCentr + radius[i] * cos(angle);
            yP = yCentr + radius[i] * sin(angle);
            glVertex2f(xP, yP);
        }
    glEnd();

    if(radar->m_isMarkARP) {
        glPointSize(15);
        glBegin(GL_POINTS);
            glVertex2f(xCentr + radius[5] * cos(angle), yCentr + radius[5] * sin(angle));
        glEnd();
    }

    for(int i=0; i<radius.size(); i++) {
        drawBlendCircle(xCentr, yCentr, radius[i], angle);
    }

    glColor3f(0.5f, 0.5, 0.5f);
    drawCircle(xCentr, yCentr, radius[5]);

    float ang, xEnd, yEnd;
    float c = 0.1;
    const float INK = 0.3/12;
    glColor3f(0.0f, c, 0.0f);

    glBegin(GL_LINES);
    int a = (int)radar->m_angle - (int)radar->m_angle % 30 + 30;
    for(int i=0; i<360; i += 30, a += 30) {
        ang = (a - 90) * P/180;
        xEnd = xCentr + radius[5] * cos(ang);
        yEnd = yCentr + radius[5] * sin(ang);
        glVertex2f(xCentr, yCentr);
        glVertex2f(xEnd, yEnd);
        c += INK;
        glColor3f(0.0f, c, 0.0f);
    }
    glEnd();


    drawRadarMask(xCentr, yCentr);
    drawRadarTableInfo();
    paintSymbolPort();

    if(m_isTabTargRadar) {
        drawTableTargets();
    }

    if(m_typeRadar == R_18) {
        glColor3f(0.0f, 0.8f, 0.0f);
        float x = m_widthScreen - 50, y = m_heightScreen - 100;
        float d = 70 * m_scFactX;
        printString(x - 2*d, y, "Angle: ");
        QString title = QString::number(m_countAngle);
        printString(x - d, y, title.toStdString().c_str());
    }
//    float time = m_timePaint.elapsed();
//    qDebug()<<time;
//    drawRadarTestCursorTable();
}

void GlGraph::drawGraphick()
{
    scaleGraph();

    if(m_typeRadar == TPS_63) {
        for(int i=0; i<CHAN; i++) {
            if(m_chan[i]) {
                m_isNormGraph ? drawline(m_vecLineInter[i], m_colorsBut_1[i], i) : drawline(m_vecLine[i], m_colorsBut_1[i], i);
            }
        }
    }
    else if(m_typeRadar == R_18 && m_isAllChannelButtons) {
        for(int i=0; i<CHAN; i++) {
            if(m_chan[i]) {
                m_isNormGraph ? drawline(m_vecLineInter[i], m_colorsBut_1[i], i) : drawline(m_vecLine[i], m_colorsBut_1[i], i);
            }
        }
    }
    else if(m_typeRadar == R_18 && !m_isAllChannelButtons) {
        for(int i=0; i<CHAN; i++) {
            if(m_chan[i]) {
                int c = 0;
                for(int j=0; j<=CHAN; j++) {
                    if(i == m_numbersR18Channel[j]) {
                        c = j;
                    }
                }

                m_isNormGraph ? drawline(m_vecLineInter[i], m_colorsBut_2[c], i) : drawline(m_vecLine[i], m_colorsBut_2[c], i);
            }
        }
    }

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);

    glColor3f(0.2f, 0.2f, 0.2f);
    glRectf(0, 0, m_graghSc.widthField, m_graghSc.heightWind);
    glRectf(0, m_graghSc.heightWind, m_graghSc.widthField + m_graghSc.widthWind, m_graghSc.heightWind + m_graghSc.heightField);
    glColor3f(0.5, 0.5, 0.5);

    drawFrame(m_graghSc.widthField, 0, m_graghSc.widthField + m_graghSc.widthWind, m_graghSc.heightWind);
    drawCoordinateGrid();
    drawGraphicInscription();
    if(!m_analogGraphSin.isActive) drawGraphickTableSignals();
    drawGraphButtons();

    if(m_typeRadar == R_18 && !m_isAllChannelButtons) {
        drawAnalogGraph();
        drawTableAngles();
        drawAnalogGraphSin();
    }
}

void GlGraph::drawCurveTargets()
{
    short l = LENCUR;
    short w = WIDCUR;
    scaleCurve();

    Color c;
    glBegin(GL_POINTS);
        for(int i=0; i<w; i++) {
            for(int j=0; j<l; j++) {
                if((*m_imageTargets)[i * l + j] > 500) {
                    c = countColor((*m_imageTargets)[i * l + j]);
                    glColor3f(c.r, c.g, c.b);
                    glVertex2f(i, l - j);
                }
            }
        }
    glEnd();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);
    glColor3f(0.2f, 0.2f, 0.2f);
    glRectf(0, 0, m_curveSc.widthField, m_curveSc.heightWind);
    glRectf(0, m_curveSc.heightWind, m_curveSc.widthField + m_curveSc.widthWind, m_curveSc.heightWind + m_curveSc.heightField);
    glColor3f(0.5, 0.5, 0.5);
    drawFrame(m_curveSc.widthField, 0, m_curveSc.widthField + m_curveSc.widthWind, m_curveSc.heightWind);
    drawGraphicCurveInscription();
}

void GlGraph::drawTableTargets()
{
    paintContourTabTar(m_tabTar.x, m_tabTar.y, false);
    paintContourTabTar(m_tabTar.x2, m_tabTar.y, true);
}

void GlGraph::drawIndicator()
{
    float xShift = m_widthScreen - (m_widthScreen/4 * m_scFactX/m_factSides);
    float step = m_heightScreen/30;
    float yShift = (m_heightScreen - step * 6);
    float yShift2 = step/5;
    float w = m_widthScreen * m_scFactX/8;
    float h = m_heightScreen * m_scFactY/40;
    float xShift3 = xShift + w * 0.2;
    int partBoard = (float)m_timeBoard/1200 * 100;
    int partProc = (float)m_timeProcessor/400 * 100;
    float wPartBoard = w * partBoard/100;
    float wPartProc = w * partProc/100;

    glColor3f(0.5f, 0.5f, 0.5f);
    printString(xShift, yShift, "Board");
    printString(xShift, yShift + step * 2, "Processing");

    glColor3f(0.0f, 1.0f, 0.0f);
    glRectf(xShift, yShift + step + yShift2, xShift + wPartBoard, yShift + step - h);
    glRectf(xShift, yShift + step * 3 + yShift2, xShift + wPartProc, yShift + step * 3 - h);

    glColor3f(1.0f, 0.0f, 0.0f);
    glRectf(xShift + wPartBoard, yShift + step + yShift2, xShift + w, yShift + step - h);
    glRectf(xShift + wPartProc, yShift + step * 3 + yShift2, xShift + w, yShift + step * 3 - h);

    glColor3f(0.5f, 0.5f, 0.5f);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(xShift, yShift + step + yShift2, xShift + w, yShift + step - h);
    glRectf(xShift, yShift + step * 3 + yShift2, xShift + w, yShift + step * 3 - h);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    QString strPartBoard = QString::number((float)m_timeBoard/1000) + " sec";
    printString(xShift3, yShift + step, strPartBoard.toStdString().c_str());

    QString strPartProc = QString::number((float)m_timeProcessor/1000) + " sec";
    printString(xShift3, yShift + step * 3, strPartProc.toStdString().c_str());
}

void GlGraph::drawFrame(float x, float y, float w, float h)
{
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(x, y, w, h);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void GlGraph::drawCoordinateGrid()
{
    glLineStipple(3, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glColor3f(0.5, 0.5, 0.5);

    glBegin(GL_LINES);
       for(int i=0; i<m_listGraphicY.size(); i++) {
           if(m_listGraphicY[i].isActive && m_listGraphicY[i].isVisible) {
               glVertex2f(m_graghSc.widthField, m_listGraphicY[i].posDigit);
               glVertex2f(m_graghSc.widthField + m_graghSc.widthWind, m_listGraphicY[i].posDigit);
//               glVertex2f(m_rangeIncreaseX, m_listGraphicY[i].posDigit);
//               glVertex2f(m_widthScreen, m_listGraphicY[i].posDigit);
           }
       }

       for(int i=0; i<m_listGraphicX.size(); i++) {
           if(m_listGraphicX[i].isActive && m_listGraphicX[i].isVisible) {
               glVertex2f(m_listGraphicX[i].posDigit + m_graghSc.widthField, 0);
               glVertex2f(m_listGraphicX[i].posDigit + m_graghSc.widthField, m_graghSc.heightWind);
//               glVertex2f(m_listGraphicX[i].posDigit + m_rangeIncreaseX, 0);
//               glVertex2f(m_listGraphicX[i].posDigit + m_rangeIncreaseX, m_heightShedule);
           }
       }
    glEnd();
    glDisable(GL_LINE_STIPPLE);
}

void GlGraph::drawGraphickTableSignals()
{
    float widthShift = m_modeLocBut == SIDE ? m_widthScreen - (m_widthTabInf + 85) * m_scFactX : m_widthScreen - (m_widthTabInf + 15) * m_scFactX;
    float stepShift = m_heightScreen/20;
    float horShift = m_widthTabInf/20 * m_scFactX;
    float verShift = stepShift/5;
    int num = 7;
    float heightShift = m_modeScreen == RADAR ? 0 : m_graghSc.heightWind - (stepShift * num + verShift);
    float hACP = heightShift + stepShift * 4;
    float hARP = heightShift + stepShift * 5;
    float hVid = heightShift + stepShift * 6;
    float hTr = heightShift + stepShift * 7;

    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glRectf(widthShift - horShift, heightShift - stepShift/2, widthShift + m_widthTabInf * m_scFactX + horShift, heightShift + stepShift * num + verShift);

    glColor3f(0.5f, 0.5f, 0.5f);
    printTablString(widthShift, heightShift, m_widthTabInf, "PRF", QString::number(m_dataTable.PRF));

    QString strTypeTrigger = m_dataTable.typeTrigger == XMIT ? "XMIT" : "R0";
    printTablString(widthShift, heightShift + stepShift, m_widthTabInf, "TYPE OF TRIGGER", strTypeTrigger);
    printTablString(widthShift, heightShift + stepShift * 2, m_widthTabInf, "WIDTH OF TRIGGER", QString::number(m_dataTable.widthImpTrig));
    printTablString(widthShift, heightShift + stepShift * 3, m_widthTabInf, "TIME ROTATION", QString::number(m_dataTable.timeRotation));
    printTablString(widthShift, hACP, m_widthTabInf, "ACP", QString::number(m_dataTable.ACP));
    printTablString(widthShift, hARP, m_widthTabInf, "ARP", QString::number(m_dataTable.ARP));
    printTablString(widthShift, hVid, m_widthTabInf, "VIDEO", QString::number(m_dataTable.video));
    printTablString(widthShift, hTr, m_widthTabInf, "TRIGGER", QString::number(m_dataTable.trigger));

    glBegin(GL_LINE_LOOP);
        glVertex2f(widthShift - horShift, heightShift - stepShift/2);
        glVertex2f(widthShift + m_widthTabInf * m_scFactX + horShift, heightShift - stepShift/2);
        glVertex2f(widthShift + m_widthTabInf * m_scFactX + horShift, heightShift + stepShift * num + verShift);
        glVertex2f(widthShift - horShift, heightShift + stepShift * num + verShift);
    glEnd();

    float w = m_widthTabInf * m_scFactX/10;
    float h = stepShift/3;
    float wSh =  m_widthTabInf * m_scFactX/3;
    float hSh =  stepShift/10;
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
    glRectf(widthShift + wSh, hACP + hSh, widthShift + wSh + w, hACP - h);
    glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
    glRectf(widthShift + wSh, hARP + hSh, widthShift + wSh + w, hARP - h);
    glColor4f(1.0f, 1.0f, 0.0f, 0.5f);
    glRectf(widthShift + wSh, hVid + hSh, widthShift + wSh + w, hVid - h);
    glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
    glRectf(widthShift + wSh, hTr + hSh, widthShift + wSh + w, hTr - h);
}

void GlGraph::drawRadarTableInfo()
{
    float widthShift = 0;
    float heightShift = 15 * m_scFactY;
    float stepShift = m_heightScreen/30;
    float horShift = m_widthTabInfRadar/20 * m_scFactX;
    float verShift = stepShift/5;
    int n = 2;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);

    glColor4f(0.0f, 0.0f, 0.0f, 0.3f);
    glRectf(widthShift - horShift, heightShift - stepShift/2,
            widthShift + m_widthTabInfRadar * m_scFactX + horShift, heightShift + stepShift * n + verShift);

    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(widthShift - horShift, heightShift - stepShift/2);
        glVertex2f(widthShift + m_widthTabInfRadar * m_scFactX + horShift, heightShift - stepShift/2);
        glVertex2f(widthShift + m_widthTabInfRadar * m_scFactX + horShift, heightShift + stepShift * n + verShift);
        glVertex2f(widthShift - horShift, heightShift + stepShift * n + verShift);
    glEnd();

    QString strTypeTrigger = m_dataTable.typeTrigger == XMIT ? "XMIT" : "R0";
    printTablString(widthShift, heightShift, m_widthTabInfRadar, "PRF", QString::number(m_dataTable.PRF));
    printTablString(widthShift, heightShift + stepShift, m_widthTabInfRadar, "TRIGGER", strTypeTrigger);
    printTablString(widthShift, heightShift + stepShift * 2, m_widthTabInfRadar, "ROTATION", QString::number(m_dataTable.timeRotation));
}

void GlGraph::drawRadarTestCursorTable()
{
    float widthShift = 0;
    float heightShift = m_heightScreen/10 * 4;
    float stepShift = m_heightScreen/20;

    glColor3f(0.5f, 0.5f, 0.5f);
    printTablString(widthShift, heightShift, m_widthTabInf, "timeRotation", QString::number(radar->m_cursor.timeRotation));
    printTablString(widthShift, heightShift + stepShift, m_widthTabInf, "dAngNorm", QString::number(radar->m_cursor.sAngNorm));
    printTablString(widthShift, heightShift + stepShift * 2, m_widthTabInf, "dAng", QString::number(radar->m_cursor.sAng));
    printTablString(widthShift, heightShift + stepShift * 3, m_widthTabInf, "middleAz", QString::number(radar->m_cursor.middleAz));
    printTablString(widthShift, heightShift + stepShift * 4, m_widthTabInf, "usualAz", QString::number(radar->m_cursor.usualAz));
    printTablString(widthShift, heightShift + stepShift * 5, m_widthTabInf, "timeDif", QString::number((float)radar->m_timeDif/1000));
    printTablString(widthShift, heightShift + stepShift * 6, m_widthTabInf, "delay", QString::number(radar->m_difValue));
    printTablString(widthShift, heightShift + stepShift * 7, m_widthTabInf, "difference", QString::number(radar->m_dif));
    //    printTablString(widthShift, heightShift + stepShift * 7, m_widthTabInf, "rotation", QString::number(m_radar->m_numberRotation));
}

void GlGraph::drawIndicatorTypeRadar()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);

    float widthShift = m_widthScreen - 90 * m_scFactX;
    float heightShift = 40 * m_scFactY;
    float stepShift = 40 * m_scFactY;
    const char *title1 = "TPS 63";
    const char *title2 = "R 18";

    m_typeRadar == TPS_63 ? glColor3f(1.0f, 0.0f, 0.0f) : glColor3f(0.5f, 0.5f, 0.5f);
    printString(widthShift, heightShift, title1);

    m_typeRadar == TPS_63 ? glColor3f(0.5f, 0.5f, 0.5f) : glColor3f(1.0f, 0.0f, 0.0f);
    printString(widthShift, heightShift + stepShift, title2);;
}

void GlGraph::drawAnalogGraph()
{
    float sc = m_analogGraph.isNormalSize ? 1.0 : 1.6;
    float x1 = m_analogGraph.x1, x2 = m_analogGraph.x2, y1 = m_analogGraph.y1, y2 = m_analogGraph.y2;
    float xg1 = m_analogGraph.xg1, xg2 = m_analogGraph.xg2, yg1 = m_analogGraph.yg1, yg2 = m_analogGraph.yg2;
    float height = y1 - y2, width = xg2 - xg1, widthL = 5;
    float yCent = yg2 + (yg1 - yg2)/2;
    float dx = x1 * (1 - 1/sc);
    float dy = y1 * (1 - 1/sc);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(sc, sc, 1.0);
    glTranslatef(-dx, -dy, 0.0);

    glColor3f(1.0f, 1.0f, 1.0f);
//    glRectf(x1, y1, x2, y2);

    glLineStipple(1, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glColor3f(0.5, 0.5, 0.5);
    float p = (yg1-yg2)/4;
    glBegin(GL_LINES);
        glVertex2f(xg1, yCent);
        glVertex2f(xg2, yCent);
    glEnd();

    glLineStipple(5, 0xAAAA);
    glBegin(GL_LINES);
        glVertex2f(xg1, yg2);
        glVertex2f(xg2, yg2);
        glVertex2f(xg2, yg1);
        glVertex2f(xg2, yg2);

        glVertex2f(xg1, yg2);
        glVertex2f(xg2, yg2);
        glVertex2f(xg1, yg2+p);
        glVertex2f(xg2, yg2+p);
        glVertex2f(xg1, yg1-p);
        glVertex2f(xg2, yg1-p);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    glBegin(GL_LINES);
        glVertex2f(xg1, yg1);
        glVertex2f(xg1, yg2);
        glVertex2f(xg1, yg1);
        glVertex2f(xg2, yg1);
    glEnd();

    float ySh = 5 * m_scFactY;
    printString(xg1 - 12*m_scFactX, yCent + ySh, "0");
    printString(xg1 - 32*m_scFactX, yCent - p, "0.5");
    printString(xg1 - 32*m_scFactX, yCent + p, "0.5");
    printString(xg1 + (xg2-xg1)/2 - 5*m_scFactX, yg1 + 3*ySh, "P");
    printString(xg2 - 15*m_scFactX, yg1 + 3*ySh, "2P");

    /*float sy = y2;
        float sw = 20;
        float sh = 20;
        for(int i=0; i<3; i++) {
            int t=11;
            glColor3f(m_colorsBut_2[i+t].r, m_colorsBut_2[i+t].g, m_colorsBut_2[i+t].b);
            glRectf(x2, sy, x2+sw, sy+sh);
            glColor3f(0.1, 0.1, 0.1);
            if(m_isSinch[i]) {
                glBegin(GL_LINES);
                    glVertex2f(x2+sw/2, sy);
                    glVertex2f(x2+sw/2, sy+sh);
                    glVertex2f(x2+sw/2, sy);
                    glVertex2f(x2+sw*0.3, sy+sh*0.3);
                    glVertex2f(x2+sw/2, sy);
                    glVertex2f(x2+sw*0.7, sy+sh*0.3);
                glEnd();
            }
            else {
                glBegin(GL_LINES);
                    glVertex2f(x2+sw/2, sy);
                    glVertex2f(x2+sw/2, sy+sh);
                    glVertex2f(x2+sw/2, sy+sh);
                    glVertex2f(x2+sw*0.3, sy+sh-sh*0.3);
                    glVertex2f(x2+sw/2, sy+sh);
                    glVertex2f(x2+sw*0.7, sy+sh-sh*0.3);
                glEnd();
            }
            sy += 2*sh;
        }*/

    float fact = m_widthShedule/width;
    float factY = 0.65;
    glScalef(1.0/fact, 1.0, 1.0);

    float dxGr = x1 * fact;
    float dyGr = y2 + height/2;
    glTranslatef(dxGr + (m_analogGraph.xg1-m_analogGraph.x1)*fact, dyGr + (m_analogGraph.yg1-m_analogGraph.y1)/2*factY, 0.0);


    int numChan = CHAN + PROC_CHAN;
    for(int i=CHAN; i<numChan; i++) {
        if(m_chan[i]) {
            m_isNormGraph ? drawAnalogLine(m_vecLineInter[i], m_colorsBut_2[i-1]) : drawAnalogLine(m_vecLine[i], m_colorsBut_2[i-1]);
        }
    }
}

void GlGraph::drawAnalogGraphSin()
{
    float sc = m_analogGraphSin.isNormalSize ? 1.0 : 1.6;
    float x1 = m_analogGraphSin.x1, x2 = m_analogGraphSin.x2, y1 = m_analogGraphSin.y1, y2 = m_analogGraphSin.y2;
    float xg1 = m_analogGraphSin.xg1, xg2 = m_analogGraphSin.xg2, yg1 = m_analogGraphSin.yg1, yg2 = m_analogGraphSin.yg2;
    float height = y1 - y2, width = x2 - x1, widthL = 5;
    float yCent = yg2 + (yg1 - yg2)/2;
    float dx = x1 * (1 - 1/sc);
    float dy = y1 * (1 - 1/sc);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(sc, sc, 1.0);
    glTranslatef(-dx, -dy, 0.0);

    glColor3f(1.0f, 1.0f, 1.0f);
//    glRectf(x1, y1, x2, y2);

    glLineStipple(1, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glColor3f(0.5, 0.5, 0.5);
    float p = (yg1-yg2)/4;
    glBegin(GL_LINES);
        glVertex2f(xg1, yCent);
        glVertex2f(xg2, yCent);
    glEnd();

    glLineStipple(5, 0xAAAA);
    glBegin(GL_LINES);
        glVertex2f(xg1, yg2);
        glVertex2f(xg2, yg2);
        glVertex2f(xg2, yg1);
        glVertex2f(xg2, yg2);

        glVertex2f(xg1, yg2);
        glVertex2f(xg2, yg2);
        glVertex2f(xg1, yg2+p);
        glVertex2f(xg2, yg2+p);
        glVertex2f(xg1, yg1-p);
        glVertex2f(xg2, yg1-p);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    glBegin(GL_LINES);
        glVertex2f(xg1, yg1);
        glVertex2f(xg1, yg2);
        glVertex2f(xg1, yg1);
        glVertex2f(xg2, yg1);
    glEnd();

    float ySh = 5 * m_scFactY;
    printString(xg1 - 12*m_scFactX, yCent + ySh, "0");
    printString(xg1 - 32*m_scFactX, yCent - p, "0.5");
    printString(xg1 - 32*m_scFactX, yCent + p, "0.5");
    printString(xg1 + (xg2-xg1)/2 - 5*m_scFactX, yg1 + 3*ySh, "P");
    printString(xg2 - 15*m_scFactX, yg1 + 3*ySh, "2P");

//    float yPos = y2;
//    QString str;
//    QString str2;
//    for(int i=0; i<PROC_CHAN; i++) {
//        int n = i + R + 1;
//        yPos += 20*m_scFactY;
//        str = QString::number(m_pVecSin[i]->size());
//        str2 = QString::number(m_chan[n]);
//        printString(x1 - 50*m_scFactX, yPos, str.toStdString().c_str());
//        printString(x1 - 150*m_scFactX, yPos, str2.toStdString().c_str());
//    }

//    float xPos = 10;
//    for(int i=0, s=m_pVecSin[4]->size(); i<s; i++) {
//        str = QString::number((*m_pVecSin[5])[i], 'f', 1);
//        xPos += 60*m_scFactY;
//        printString(xPos, m_heightScreen*0.8, str.toStdString().c_str());
//    }

    float fact = m_analogGraphSin.factScr;
//    float factY = 1/(m_analogGraphSin.yg1 - m_analogGraphSin.yg2)*2;
    float factY = 0.65;
    glScalef(1.0/fact, 1.0, 1.0);

    float dxGr = x1 * fact;
    float dyGr = y2 + height/2;
    glTranslatef(dxGr + (m_analogGraphSin.xg1-m_analogGraphSin.x1)*fact, dyGr + (m_analogGraphSin.yg1-m_analogGraphSin.y1)/2*factY, 0.0);

    for(int i=0; i<PROC_CHAN; i++) {
        int n = i + R + 1;

        if(n >= CHAN + PROC_CHAN) throw ExeptionRange("Out or range: ", n, "GlGraph::drawAnalogGraphSin() - m_chan");

        if(m_chan[n]) {
            drawAnalogSin(*m_pVecSin[i], m_colorsBut_2[i+7], i);
        }
    }

//    if(m_analogGraphSin.isDrawInf) paintAnalogInf();

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);
    drawGraphButton(m_listButGraph[SIN_BUT].type);
    drawGraphButton(m_listButGraph[SIN2_BUT].type);
}

void GlGraph::drawGraphicInscription()
{
//    short indentation = 5;
//    short xPos = m_rangeIncreaseX/8;
//    short xPos2 = m_rangeIncreaseX - m_rangeIncreaseX/15;

    float shY1 = 5 * m_scFactY;
    float shY2;
    float xPos = 0.05 * m_graghSc.widthField;
    float xPos2 = 0.9 * m_graghSc.widthField;
    QString title;

    glColor3f(0.6, 0.6, 0.6);
    for(int i=0; i<m_listGraphicY.size(); i++) {
        title = QString::number(m_listGraphicY[i].inscription) + "K";
        shY2 = m_listGraphicY[i].pos != 0 ? 0 : 5;

        if(m_listGraphicY[i].isActive && m_listGraphicY[i].isVisible) {
            printString(xPos, m_listGraphicY[i].posDigit + shY1 + shY2, title.toStdString().c_str());
        }
    }

    glLineWidth(3);
    glBegin(GL_LINES);
    for(int i=0; i<m_listGraphicY.size(); i++) {
        if(m_listGraphicY[i].isActive && m_listGraphicY[i].isVisible) {
            glVertex2f(xPos2, m_listGraphicY[i].posDigit);
            glVertex2f(/*m_rangeIncreaseX*/m_graghSc.widthField-1, m_listGraphicY[i].posDigit);
        }
    }
    glEnd();

//    short yPos = m_heightShedule + m_heightShedule/20;
//    short xIndentation = m_rangeIncreaseX;
//    float yPos2 = m_heightShedule + m_heightShedule/60;
    float yPos = m_graghSc.heightWind + 25 * m_scFactY;
    float shX1 = m_graghSc.widthField;
    float shX2 = 10 * m_scFactX;
    float yPos2 = m_graghSc.heightWind + 8 * m_scFactY;

    glColor3f(0.6f, 0.6f, 0.6f);
    for(int i=0; i<m_listGraphicX.size(); i++) {
        if(m_listGraphicX[i].isActive && m_listGraphicX[i].isVisible) {
            title = QString::number(m_listGraphicX[i].inscription) + "s";
            printString(m_listGraphicX[i].posDigit + shX1 - shX2, yPos, title.toStdString().c_str());
        }
    }

    glBegin(GL_LINES);
    for(int i=0; i<m_listGraphicX.size(); i++) {
        if(m_listGraphicX[i].isActive && m_listGraphicX[i].isVisible) {
            glVertex2f(m_listGraphicX[i].posDigit + shX1, /*m_heightShedule*/m_graghSc.heightWind);
            glVertex2f(m_listGraphicX[i].posDigit + shX1, yPos2);
        }
    }
    glEnd();
    glLineWidth(1);
}

void GlGraph::drawGraphicCurveInscription()
{
    float fX = m_scFactX;
    float fY = m_scFactY;
    int shY1 = 5 * fY;
    int shY2;
    int shY3 = 15 * fY;
    float xPos = 0.1 * m_graghSc.widthField;
    float xPos2 = 0.9 * m_graghSc.widthField;
    QString title;

    glColor3f(0.6, 0.6, 0.6);
    for(int i=1; i<m_listGraphicCurveY.size(); i++) {
        title = QString::number(m_listGraphicCurveY[i].inscription);
        shY2 = m_listGraphicCurveY[i].pos != 0 ? 0 : 10 * fY;

        if(m_listGraphicCurveY[i].isActive && m_listGraphicCurveY[i].isVisible) {
            printString(xPos, m_listGraphicCurveY[i].posDigit + shY1 + shY2, title.toStdString().c_str());
            printString(xPos, m_listGraphicCurveY[i].posDigit + shY1 + shY2 + shY3, "km");
        }
    }

    glLineWidth(3);
    glBegin(GL_LINES);
    for(int i=1; i<m_listGraphicCurveY.size(); i++) {
        if(m_listGraphicCurveY[i].isActive && m_listGraphicCurveY[i].isVisible) {
            glVertex2f(xPos2, m_listGraphicCurveY[i].posDigit);
            glVertex2f(m_curveSc.widthField-1, m_listGraphicCurveY[i].posDigit);
        }
    }
    glEnd();

    short yPos = m_curveSc.heightWind + 25 * fY;
    short xIndentation = m_curveSc.widthField;
    short xShift = 0;
    float yPos2 = m_curveSc.heightWind + 8 * fY;

    glColor3f(0.6f, 0.6f, 0.6f);
    for(int i=0; i<m_listGraphicCurveX.size(); i++) {
        if(m_listGraphicCurveX[i].isActive && m_listGraphicCurveX[i].isVisible) {
            xShift = m_listGraphicCurveX[i].inscription < 100 ? 10*fX : 15*fX;
            xShift = m_listGraphicCurveX[i].inscription == 360 ? 30*fX : xShift;
            printString(m_listGraphicCurveX[i].posDigit + xIndentation - xShift, yPos,
                        QString::number(m_listGraphicCurveX[i].inscription).toStdString().c_str());
        }
    }

    glBegin(GL_LINES);
    for(int i=1; i<m_listGraphicCurveX.size(); i++) {
        if(m_listGraphicCurveX[i].isActive && m_listGraphicCurveX[i].isVisible) {
            glVertex2f(m_listGraphicCurveX[i].posDigit + xIndentation, m_curveSc.heightWind);
            glVertex2f(m_listGraphicCurveX[i].posDigit + xIndentation, yPos2);
        }
    }
    glEnd();
    glLineWidth(1);
}

void GlGraph::createAtributsBut()
{
    m_colorsBut_1.append({ 0.0f, 1.0f, 0.0f });
    m_colorsBut_1.append({ 1.0f, 0.0f, 0.0f });
    m_colorsBut_1.append({ 1.0f, 1.0f, 0.0f });
    m_colorsBut_1.append({ 0.0f, 1.0f, 1.0f });
    m_colorsBut_1.append({ 0.0f, 0.0f, 1.0f });
    m_colorsBut_1.append({ 0.0f, 1.0f, 0.5f });
    m_colorsBut_1.append({ 0.6f, 0.2f, 1.0f });
    m_colorsBut_1.append({ 1.0f, 0.5f, 0.0f });

    m_colorsBut_2.append({ 1.0f, 0.0f, 0.0f });
    m_colorsBut_2.append({ 1.0f, 0.6f, 0.0f });
    m_colorsBut_2.append({ 0.6f, 0.2f, 1.0f });
    m_colorsBut_2.append({ 0.6f, 0.6f, 1.0f });
    m_colorsBut_2.append({ 1.0f, 0.2f, 1.0f });
    m_colorsBut_2.append({ 1.0f, 1.0f, 0.0f });
    m_colorsBut_2.append({ 0.0f, 1.0f, 1.0f });

    m_colorsBut_2.append({ 1.0f, 0.0f, 0.0f });
    m_colorsBut_2.append({ 0.6f, 0.2f, 1.0f });
    m_colorsBut_2.append({ 0.6f, 0.6f, 1.0f });
    m_colorsBut_2.append({ 1.0f, 0.2f, 1.0f });

    m_colorsBut_2.append({ 0.0f, 1.0f, 0.0f });
    m_colorsBut_2.append({ 0.5f, 1.0f, 0.7f });
    m_colorsBut_2.append({ 0.0f, 1.0f, 1.0f });

    TITLEBUT[0] = "A";
    TITLEBUT[1] = "N";
    TITLEBUT[2] = "V";
    TITLEBUT[3] = "T";
    TITLEBUT[4] = "R1";
    TITLEBUT[5] = "R2";
    TITLEBUT[6] = "S1";
    TITLEBUT[7] = "S2";
    TITLEBUT[8] = "S3";
    TITLEBUT[9] = "VD";
    TITLEBUT[10] = "TR";
    TITLEBUT[11] = "R";
    TITLEBUT[12] = "S1";
    TITLEBUT[13] = "S2";
    TITLEBUT[14] = "S3";
    TITLEBUT[15] = "120*";
    TITLEBUT[16] = "240*";
    TITLEBUT[17] = "0*";
}

void GlGraph::createTitlesGraph()
{
    const int Y = 600;
    float dy = m_graghSc.heightWind/Y;
    const int X = m_widthScreen;
    float dx = m_graghSc.widthWind/X;

    for(float i=0; i<=Y; i+=12.5) {
        Inscription designation;
        designation.pos = i * dy;
        designation.posDigit = i * dy;
//        designation.inscription = (300 - i) * STEP;
        designation.inscription = (300 - i)/10;

        if((int)i % 50 == 0) {
            designation.rang = 1;
            designation.isActive = true;
            designation.isVisible = true;
        }
        else if((int)i % 25 == 0) {
            designation.rang = 2;
            designation.isActive = false;
            designation.isVisible = false;
        }
        else {
            designation.rang = 3;
            designation.isActive = false;
            designation.isVisible = false;
        }

        m_listGraphicY.append(designation);
    }

    for(float i=0; i<X; i+=50) {
        Inscription designation;
        designation.pos = i * dx;
        designation.posDigit = i * dx;
//        designation.inscription = i;
        designation.inscription = i/1000;

        if((int)i % 200 == 0) {
            designation.rang = 1;
            designation.isActive = true;
            designation.isVisible = true;
        }
        else if((int)i % 100 == 0){
            designation.rang = 2;
            designation.isActive = false;
            designation.isVisible = false;
        }
        else {
            designation.rang = 3;
            designation.isActive = false;
            designation.isVisible = false;
        }

        m_listGraphicX.append(designation);
    }
}

void GlGraph::createTitlesCurve()
{
    const int RANGE = 1500;
    float r = (float)m_curveSc.heightWind/RANGE;
    const int AZ = 360;
    float a = m_curveSc.widthWind/AZ;

    for(float i = 0; i <= RANGE; i += 50) {
        Inscription designation;
        designation.pos = (RANGE - i) * r;
        designation.posDigit = (RANGE - i) * r;
        designation.inscription = i;

        if((int)i % 300 == 0) {
            designation.rang = 1;
            designation.isActive = true;
            designation.isVisible = true;
        }
        else if((int)i % 150 == 0) {
            designation.rang = 2;
            designation.isActive = false;
            designation.isVisible = false;
        }
        else {
            designation.rang = 3;
            designation.isActive = false;
            designation.isVisible = false;
        }

        m_listGraphicCurveY.append(designation);
    }

    for(float i=0; i<=AZ; i+=15) {
        Inscription designation;
        designation.pos = i * a;
        designation.posDigit = i * a;
        designation.inscription = i;

        if((int)i % 90 == 0) {
            designation.rang = 1;
            designation.isActive = true;
            designation.isVisible = true;
        }
        else if((int)i % 30 == 0){
            designation.rang = 2;
            designation.isActive = false;
            designation.isVisible = false;
        }
        else {
            designation.rang = 3;
            designation.isActive = false;
            designation.isVisible = false;
        }

        m_listGraphicCurveX.append(designation);
    }
}

void GlGraph::recountGraphicTitles()
{
    float widthOldCur = m_curveSc.widthWind;
    float heightOldCur = m_curveSc.heightWind;
    float widthOldGr = m_graghSc.widthWind;
    float heightOldGr = m_graghSc.heightWind;

    m_curveSc.widthField = m_widthScreen * m_curveSc.fW * m_scFactX;
    m_curveSc.heightField = m_heightScreen * m_curveSc.fH * m_scFactY;
    m_curveSc.widthField_2 = m_widthScreen * m_curveSc.fW_2 * m_scFactX;
    m_curveSc.widthWind = m_widthScreen - m_curveSc.widthField - m_curveSc.widthField_2;
    m_curveSc.heightWind = m_heightScreen - m_curveSc.heightField;
    m_curveSc.fScX = m_curveSc.globWidthWind/m_curveSc.widthWind;
    m_curveSc.fScY = m_curveSc.globHeightWind/m_curveSc.heightWind;
    m_curveSc.globWidthField  = m_curveSc.fScX * m_curveSc.widthField;

    m_graghSc.widthField = m_widthScreen * m_graghSc.fW * m_scFactX;
    m_graghSc.heightField = m_heightScreen * m_graghSc.fH * m_scFactY;
    m_graghSc.widthField_2 = m_widthScreen * m_graghSc.fW_2 * m_scFactX;
    m_graghSc.widthWind = m_widthScreen - m_graghSc.widthField - m_graghSc.widthField_2;
    m_graghSc.heightWind = m_heightScreen - m_graghSc.heightField;
    m_graghSc.fScX = m_graghSc.globWidthWind/m_graghSc.widthWind;
    m_graghSc.fScY = m_graghSc.globHeightWind/m_graghSc.heightWind;
    m_graghSc.globWidthField  = m_graghSc.fScX * m_graghSc.widthField;

    m_analogGraph.x2 = m_graghSc.widthField + m_graghSc.widthWind;
    m_analogGraph.factScr = m_analogGraph.sizeVec/(m_analogGraph.x2 - m_analogGraph.x1);
    m_analogGraphSin.x2 = m_graghSc.widthField + m_graghSc.widthWind;
    m_analogGraphSin.factScr = m_analogGraphSin.sizeVec/(m_analogGraphSin.x2 - m_analogGraphSin.x1);
    setSizeAnalogWindows();

    float dx = widthOldCur/m_curveSc.widthWind;
    float dy = heightOldCur/m_curveSc.heightWind;

    for(float i=0, s=m_listGraphicCurveY.size(); i<s; i++) {
        m_listGraphicCurveY[i].pos /= dy;
        m_listGraphicCurveY[i].posDigit /= dy;
    }

    for(float i=0, s=m_listGraphicCurveX.size(); i<s; i++) {
        m_listGraphicCurveX[i].pos /= dx;
        m_listGraphicCurveX[i].posDigit /= dx;
    }

    dx = widthOldGr/m_graghSc.widthWind;
    dy = heightOldGr/m_graghSc.heightWind;

    for(float i=0, s=m_listGraphicY.size(); i<s; i++) {
        m_listGraphicY[i].pos /= dy;
        m_listGraphicY[i].posDigit /= dy;
    }

    for(float i=0, s=m_listGraphicX.size(); i<s; i++) {
        m_listGraphicX[i].pos /= dx;
        m_listGraphicX[i].posDigit /= dx;
    }

    update();
}

void GlGraph::createAnalogWindows()
{
    float width = 0.5 * m_widthScreen;
    float height = 0.2 * m_heightScreen;

    m_analogGraph = { 0, 0, 0, 0, 0, 0, 0, 0,true, 0, 0, false, 1, 750, false, false };
    m_analogGraphSin = { 0, 0, 0, 0, 0, 0, 0, 0, true, 0, 0, false, 1, 750, false, false };

    m_analogGraph.x1 = 0.45 * m_widthScreen;
    m_analogGraph.y2 = 0.18 * m_heightScreen;
    m_analogGraph.y1 = m_analogGraph.y2 + height;
    m_analogGraph.x2 = m_analogGraph.x1 + width;

    m_analogGraphSin.x1 = m_analogGraph.x1;
    m_analogGraphSin.y1 = m_analogGraph.y1 + height;
    m_analogGraphSin.x2 = m_analogGraphSin.x1 + width;
    m_analogGraphSin.y2 = m_analogGraphSin.y1 - height;

    setSizeAnalogWindows();
}

void GlGraph::setSizeAnalogWindows()
{
    m_analogGraph.xg1 = m_analogGraph.x1 + 35 * m_scFactX;
    m_analogGraph.yg1 = m_analogGraph.y1 - 18 * m_scFactY;
    m_analogGraph.xg2 = m_analogGraph.x2 - 5 * m_scFactX;
    m_analogGraph.yg2 = m_analogGraph.y2 + 10 * m_scFactY;

    m_analogGraphSin.xg1 = m_analogGraphSin.x1 + 35 * m_scFactX;
    m_analogGraphSin.yg1 = m_analogGraphSin.y1 - 18 * m_scFactY;
    m_analogGraphSin.xg2 = m_analogGraphSin.x2 - 5 * m_scFactX;
    m_analogGraphSin.yg2 = m_analogGraphSin.y2 + 10 * m_scFactY;
}

void GlGraph::placeSlider()
{
    QRectF place = m_butScale.placeSlider;
    float upBoard = (place.height() - m_butScale.positionSlider.height()) * m_scFactY;
    float piece = (place.height())/50;
    float yPos = place.y() - (m_scale.factScale - 1.0) * piece;

    m_butScale.posSlider = m_butScale.positionSlider.y() - yPos;
    m_butScale.posSlider = m_butScale.posSlider > upBoard ? upBoard  : m_butScale.posSlider;
}

void GlGraph::createSizeWindImages()
{
    m_curveSc.fH = m_modeLocBut == BOTTOM ? m_curveSc.F_H : m_curveSc.F_HC;
    m_curveSc.widthField = m_widthScreen * m_curveSc.fW * m_scFactX;
    m_curveSc.heightField = m_widthScreen * m_curveSc.fH * m_scFactY;
    m_curveSc.fW_2 = m_modeLocBut == SIDE ? m_curveSc.F_W : m_curveSc.F_WC;
    m_curveSc.widthField_2 = m_widthScreen * m_curveSc.fW_2 * m_scFactX;
    m_curveSc.widthWind = m_widthScreen - m_curveSc.widthField - m_curveSc.widthField_2;
    m_curveSc.heightWind = m_heightScreen - m_curveSc.heightField;
    m_curveSc.globWidthWind = WIDCUR;
    m_curveSc.globHeightWind = LENCUR;
    m_curveSc.fScX = m_curveSc.globWidthWind/m_curveSc.widthWind;
    m_curveSc.fScY = m_curveSc.globHeightWind/m_curveSc.heightWind;
    m_curveSc.globWidthField  = m_curveSc.fScX * m_curveSc.widthField;

    m_graghSc.fH = m_modeLocBut == BOTTOM ? m_graghSc.F_H : m_graghSc.F_HC;
    m_graghSc.widthField = m_widthScreen * m_graghSc.fW * m_scFactX;
    m_graghSc.heightField = m_widthScreen * m_graghSc.fH * m_scFactY;
    m_graghSc.fW_2 = m_modeLocBut == SIDE ? m_graghSc.F_W : m_graghSc.F_WC;
    m_graghSc.widthField_2 = m_widthScreen * m_graghSc.fW_2 * m_scFactX;
    m_graghSc.widthWind = m_widthScreen - m_graghSc.widthField - m_graghSc.widthField_2;
    m_graghSc.heightWind = m_heightScreen - m_graghSc.heightField;
    m_graghSc.globWidthWind = WIDCUR;
    m_graghSc.globHeightWind = LENCUR;
    m_graghSc.fScX = m_graghSc.globWidthWind/m_graghSc.widthWind;
    m_graghSc.fScY = m_graghSc.globHeightWind/m_graghSc.heightWind;
    m_graghSc.globWidthField  = m_graghSc.fScX * m_graghSc.widthField;
}

void GlGraph::createButtons()
{
    createButton(RADAR_BUT, "RADAR");
    createButton(TAB_BUT, "TARGET");
    createButton(GRAPH_BUT, "GRAPH");
    createButton(GRAPH2_BUT, "CURVE");
    createButton(LED_BUT, "IND-R");

    placeButtons();
}

void GlGraph::createButton(TypeButtonRadar type, QString title)
{
    ButtonRadar butGraph;

    butGraph.type = type;
    butGraph.tite = title;
    butGraph.location.setRect(0, 0, m_widthBut, m_heightBut);
    butGraph.choose = false;
    m_listBut.append(butGraph);
}

void GlGraph::resizeButton(TypeButtonRadar type)
{
    const float xFsc = 1;
    const float yFsc = 1;
    QRectF but = m_listBut[type].location;
    float xPos = m_modeLocBut == BOTTOM ? but.x() : m_widthScreen - (m_widthScreen - but.x())/xFsc * m_scFactX;
    float yPos = m_modeLocBut == BOTTOM ? m_heightScreen - (m_heightScreen - but.y())/yFsc * m_scFactY : but.y();
    float width = m_modeLocBut == BOTTOM ? but.width() : but.width()/xFsc * m_scFactX;
    float height = m_modeLocBut == BOTTOM ? but.height()/yFsc * m_scFactY : but.height();

    m_listBut[type].locationSc.setRect(xPos, yPos, width, height);
}

void GlGraph::drawButtons()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);

    for(int i=0; i<m_listBut.size(); i++) {
        drawButton(m_listBut[i].type);
    }
}

void GlGraph::drawButton(TypeButtonRadar type)
{
    QRectF but = m_listBut[type].locationSc;
    float xPos = but.x();
    float yPos = but.y();
    float width = but.width();
    float height = but.height();

    type != m_choosedBut ? glColor4f(0.4f, 0.4f, 0.4f, 0.3f) : glColor3f(0.5f, 0.5f, 0.0f);
    glRectf(xPos, yPos, xPos + width, yPos - height);

    glColor4f(0.8f, 0.8f, 0.8f, 0.5f);
    glLineWidth(2);
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glRectf(xPos, yPos, xPos + width, yPos - height);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glLineWidth(1);

    float wStr = m_listBut[type].tite.size() * width/8.5;
    float xPosStr = (width - wStr)/2;
    float yPosStr = height/3;

    glColor3f(0.8f, 0.8f, 0.8f);
    printString(xPos + xPosStr, yPos - yPosStr, m_listBut[type].tite.toStdString().c_str());
}

void GlGraph::createGraphButtons()
{
    float xPos = m_widthScreen/5;
    float yPos = m_heightScreen/8;
    float width = m_widthButGraph;
    float height = m_heightButGraph;
    float distance = m_widthScreen/20;
    float xPosCur = xPos;
    float yPosCur = yPos;

    for(int i=0; i<4; i++) {
        float colCh[3] = { m_colorsBut_1[i].r, m_colorsBut_1[i].g, m_colorsBut_1[i].b };
        xPosCur = xPos + i * (width + distance);
        createGraphButton(xPosCur, yPosCur, width, height, (TypeButtonGraph)i, colCh);
    }

    float colSw[3] = { 0.0, 0.5, 1.0 };
    xPosCur = xPos;
    yPosCur = yPos/2;
    createGraphButton(xPosCur, yPosCur, width * 2, height, SWITCH_BUT, colSw);

    yPosCur = yPos;

    for(int i=0; i<8; i++) {
        float colCh[3] = { m_colorsBut_1[i].r, m_colorsBut_1[i].g, m_colorsBut_1[i].b };
        xPosCur = xPos + i * (width + distance);
        createGraphButton(xPosCur, yPosCur, width, height, (TypeButtonGraph)(i + CH1), colCh);
    }

    float width3 = width * 1.4;
    for(int i=0; i<7; i++) {
        float colCh[3] = { m_colorsBut_2[i].r, m_colorsBut_2[i].g, m_colorsBut_2[i].b };

        xPosCur = xPos + i * (width3 + distance);
        createGraphButton(xPosCur, yPosCur, width3, height, (TypeButtonGraph)(i + R1_BUT), colCh);
    }

    float width2 = width * 2;
    yPosCur = m_analogGraph.y2;

    for(int i=7; i<14; i++) {
        float colCh[3] = { m_colorsBut_2[i].r, m_colorsBut_2[i].g, m_colorsBut_2[i].b };

        xPosCur = m_analogGraph.x1 + (i-7) * (width2 + width2/10);
        createGraphButton(xPosCur, yPosCur, width2, height, (TypeButtonGraph)(i + R1_BUT), colCh);
    }

    float colMode[3] = { 0.0, 0.8, 0.0 };
    xPosCur = xPos + 2 * width + 2 * distance;
    yPosCur = yPos/2;
    createGraphButton(xPosCur, yPosCur, width * 2, height, MODE_GRAPH_BUT, colMode);

    float colSin[3] = { 0.5, 0.5, 0.5 };
    xPosCur = m_analogGraphSin.x1;
    yPosCur = m_analogGraphSin.y1;
    createGraphButton(xPosCur + 1*m_scFactX, yPosCur - 1*m_scFactY, 0.6*width, 0.6*height, SIN_BUT, colSin);

    float colSin2[3] = { 1.0, 0.5, 0.5 };
    xPosCur = m_analogGraph.x1 + (m_analogGraph.x2 - m_analogGraph.x1)/2 - width/2;
    xPosCur = 0.7*m_widthScreen;
    yPosCur = m_analogGraph.y1 + 0.3*height/2 * m_scFactY;
    createGraphButton(xPosCur, yPosCur, width, 0.3*height, SIN2_BUT, colSin2);

    float x = m_widthScreen/10; float y = m_heightScreen/2; float w = width; float h = height * 10;
    m_butScale.type = SCALE_BUT;
    m_butScale.location.setRect(x, y, w, h);
    m_butScale.placeSlider.setRect(x, y - 0.16*h, 0.33*w, 0.66*h);
    m_butScale.positionSlider.setRect(x, y - 0.16*h, w, 0.06*h);
    m_butScale.butZoomIn.setRect(x, y - h, w, 0.12*h);
    m_butScale.butZoomOut.setRect(x, y - 0.12*h, w, 0.12*h);

    x = m_tabTar.x2 + m_tabTar.width; y = m_tabTar.y; w = 12; h = m_tabTar.height;
    float hAr = w; float hSc = h - 2 * hAr;
    m_butScroll.rectCom.setRect(x, y, w, h);
    m_butScroll.rectArrow1.setRect(x, y, w, hAr);
    m_butScroll.rectStrip.setRect(x, y+hAr, w, hSc);
    m_butScroll.rectSlid.setRect(x, y+hAr, w, hSc);
    m_butScroll.rectArrow2.setRect(x, y+hAr+hSc, w, hAr);
    m_butScroll.posSlid = 0;
    m_butScroll.normHeight = hSc;

    x = m_tabTar.x + m_tabTar.width; y = m_tabTar.y; w = 12; h = m_tabTar.height2;
    hAr = w; hSc = h - 2 * hAr;
    m_butScroll2.rectCom.setRect(x, y, w, h);
    m_butScroll2.rectArrow1.setRect(x, y, w, hAr);
    m_butScroll2.rectStrip.setRect(x, y+hAr, w, hSc);
    m_butScroll2.rectSlid.setRect(x, y+hAr, w, hSc);
    m_butScroll2.rectArrow2.setRect(x, y+hAr+hSc, w, hAr);
    m_butScroll2.posSlid = 0;
    m_butScroll2.normHeight = hSc;
}

void GlGraph::createGraphButton(float x, float y, float w, float h, TypeButtonGraph name, float col[3])
{
    ButtonGraph but;
    but.type = name;
    but.location.setRect(x, y, w, h);
    but.isOn = false;

    for(int i=0; i<3; i++) {
       but.color[i] = col[i];
    }

    m_listButGraph.append(but);
}

void GlGraph::resizeGraphButton(TypeButtonGraph type)
{
    if(type <= TRIGGER_R_BUT || type == MODE_GRAPH_BUT) {
        QRectF but = m_listButGraph[type].location;
        float xPos = but.x();
        float yPos = but.y();
        float width = but.width() * m_scFactX;
        float height = but.height() * m_scFactY;
        m_listButGraph[type].locationSc.setRect(xPos, yPos, width, height);
    }
    else if(type >= R_BUT && type <= S1_S3_BUT) {
        QRectF but = m_listButGraph[type].location;
        float xPos = but.x();
        float yPos = but.y();
        float width = but.width();
        float height = but.height();
        m_listButGraph[type].locationSc.setRect(xPos, yPos, width, height);
    }
    else if(type == SCALE_BUT) {
        QRectF but = m_butScale.location;
        float xPos = but.x();
        float yPos = but.y() * m_scFactY;
        float width = but.width() * m_scFactX;
        float height = but.height() * m_scFactY;
        m_butScale.locationSc.setRect(xPos, yPos, width, height);

        QRectF place = m_butScale.placeSlider;
        xPos = place.x();
        yPos = place.y() * m_scFactY;
        width = place.width() * m_scFactX;
        height =place.height() * m_scFactY;
        m_butScale.placeSliderSc.setRect(xPos, yPos, width, height);

        place = m_butScale.positionSlider;
        xPos = place.x();
        yPos = place.y() * m_scFactY;
        width = place.width() * m_scFactX;
        height = place.height() * m_scFactY;
        m_butScale.positionSliderSc.setRect(xPos, yPos, width, height);

        place = m_butScale.butZoomIn;
        xPos = place.x();
        yPos = place.y() * m_scFactY;
        width = place.width() * m_scFactX;
        height = place.height() * m_scFactY;
        m_butScale.butZoomInSc.setRect(xPos, yPos, width, height);

        place = m_butScale.butZoomOut;
        xPos = place.x();
        yPos = place.y() * m_scFactY;
        width = place.width() * m_scFactX;
        height = place.height() * m_scFactY;
        m_butScale.butZoomOutSc.setRect(xPos, yPos, width, height);
    }
    else if(type == SIN_BUT) {
        QRectF but = m_listButGraph[type].location;
        float xPos = but.x(), yPos = but.y(), width = but.width() * m_scFactX, height = but.height() * m_scFactY;

        m_listButGraph[type].locationSc.setRect(xPos, yPos, width, height);
    }
    else if(type == SIN2_BUT) {
        QRectF but = m_listButGraph[type].location;
        float xPos = but.x(), yPos = but.y();
        float width = but.width() * m_scFactX, height = but.height() * m_scFactY;

        m_listButGraph[type].locationSc.setRect(xPos, yPos, width, height);
    }
}

void GlGraph::drawGraphButton(TypeButtonGraph type)
{
    QRectF but = m_listButGraph[type].locationSc;
    float x1 = but.x(), y1 = but.y(), x2 = but.x() + but.width(), y2 = but.y() - but.height();
    int k = m_listButGraph[type].isOn ? 1 : 2;
    float xPosStr, yPosStr;

    if(type <= TRIGGER_BUT) {
        glColor3f(m_listButGraph[type].color[0]/k, m_listButGraph[type].color[1]/k, m_listButGraph[type].color[2]/k);
        glRectf(x1, y1, x2, y2);

        xPosStr = but.width()/3.5;
        yPosStr = but.height()/4.5;
        glColor3f(0.8f, 0.8f, 0.8f);
        printString(x1 + xPosStr, y1 - yPosStr, TITLEBUT[type]);
    }
    else if(type == SWITCH_BUT) {
        glColor3f(m_listButGraph[type].color[0]/k, m_listButGraph[type].color[1]/k, m_listButGraph[type].color[2]/k);
        glRectf(x1 + but.width()/2, y1, x2, y2);

        int k2 = m_listButGraph[type].isOn ? 2 : 1;
        glColor3f(m_listButGraph[type].color[0]/k2, m_listButGraph[type].color[1]/k2, m_listButGraph[type].color[2]/k2);
        glRectf(x1, y1, x1 + but.width()/2, y2);

        xPosStr = but.width()/7;
        yPosStr = but.height()/4.5;
        float xPosStr2 = but.width()/7 + but.width()/2;
        glColor3f(0.1f, 0.1f, 0.1f);
        printString(x1 + xPosStr, y1 - yPosStr, "A");
        printString(x1 + xPosStr2, y1 - yPosStr, "T");
    }
    else if(type >= CH1 && type <= CH8) {
        glColor3f(0.0f/k, 0.8f/k, 0.0f/k);
        glRectf(x1, y1, x2, y2);

        xPosStr = but.width()/3.5;
        yPosStr = but.height()/4.5;
        QString title = QString::number((int)m_listButGraph[type - SWITCH_BUT].type);
        glColor3f(0.8f, 0.8f, 0.8f);
        printString(x1 + xPosStr, y1 - yPosStr, title.toStdString().c_str());

        glColor3f(m_listButGraph[type].color[0]/k, m_listButGraph[type].color[1]/k, m_listButGraph[type].color[2]/k);
        glBegin(GL_POLYGON);
            glVertex2f(x1, y1 + but.height() * 0.3);
            glVertex2f(x2, y1 + but.height() * 0.3);
            glVertex2f((x1 + x2)/2, y1 + but.height() * 0.8);
            glVertex2f(x1, y1 + but.height() * 0.3);
        glEnd();
        glBegin(GL_POLYGON);
            glVertex2f(x1, y2 - but.height() * 0.3);
            glVertex2f(x2, y2 - but.height() * 0.3);
            glVertex2f((x1 + x2)/2, y2 - but.height() * 0.8);
            glVertex2f(x1, y2 - but.height() * 0.3);
        glEnd();
    }
    else if(type >= R1_BUT && type <= S1_S3_BUT) {
        glColor3f(m_listButGraph[type].color[0]/k, m_listButGraph[type].color[1]/k, m_listButGraph[type].color[2]/k);
        glRectf(x1, y1, x2, y2);
        glColor3f(0.5, 0.5, 0.5);
        drawFrame(x1, y1, x2, y2);

        xPosStr = type < S3_S2_BUT ? but.width()/5.0 : but.width()/10.0;
        yPosStr = but.height()/4.5;
        glColor3f(0.8f, 0.8f, 0.8f);
        printString(x1 + xPosStr, y1 - yPosStr, TITLEBUT[type - (R1_BUT - SWITCH_BUT)]);
    }
    else if(type == MODE_GRAPH_BUT) {
        glColor3f(m_listButGraph[type].color[0]/k, m_listButGraph[type].color[1]/k, m_listButGraph[type].color[2]/k);
        glRectf(x1, y1, x2, y2);

        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_LINES);
            glVertex2f(x1, y1 - but.height() * 0.5);
            glVertex2f(x2, y1 - but.height() * 0.5);
        glEnd();
        glBegin(GL_POLYGON);
            glVertex2f(x1, y1 - but.height() * 0.5);
            glVertex2f(x1 + but.width() * 0.3, y1 - but.height() * 0.3);
            glVertex2f(x1 + but.width() * 0.3, y1 - but.height() * 0.7);
        glEnd();
        glBegin(GL_POLYGON);
            glVertex2f(x2, y1 - but.height() * 0.5);
            glVertex2f(x2 - but.width() * 0.3, y1 - but.height() * 0.3);
            glVertex2f(x2 - but.width() * 0.3, y1 - but.height() * 0.7);
        glEnd();

        glColor3f(0.5, 0.5, 0.5);
        drawFrame(x1, y1, x2, y2);
    }
    else if(type == SIN_BUT) {
        float w = but.width(), h = but.height();
        glColor3f(0.8f, 0.8f, 0.8f);
        glBegin(GL_POLYGON);
            glVertex2f(x1, y1);
            glVertex2f(x1, y1-1.2*h);
            glVertex2f(x1+1.2*w, y1);
        glEnd();
    }
    else if(type == SIN2_BUT) {
        glRectf(x1, y1, x2, y2);
        glColor3f(0.0f, 0.0f, 0.0f);
        float fx = 5 * m_scFactX, fy = 4 * m_scFactY;
        glRectf(x1+(x2-x1)/2-fx, y1-fy, x1+(x2-x1)/2+fx, y2+fy);
    }
}

void GlGraph::drawGraphButtons()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);
    int s, f;

    if(m_typeRadar == TPS_63) {
        s = !m_isAllChannelButtons ? 0 : CH1;
        f = !m_isAllChannelButtons ? SWITCH_BUT : R1_BUT;
    }
    else if(m_typeRadar == R_18) {
        s = !m_isAllChannelButtons ? R1_BUT : SWITCH_BUT;
        f = !m_isAllChannelButtons ? MODE_GRAPH_BUT : R1_BUT;
    }

    drawGraphButton(m_listButGraph[SWITCH_BUT].type);
    drawGraphButton(m_listButGraph[MODE_GRAPH_BUT].type);
//    if(m_analogGraphSin.isActive) drawGraphButton(m_listButGraph[SIN_BUT].type);

    for(int i=s; i<f; i++) {
        drawGraphButton(m_listButGraph[i].type);
    }

    QRectF but = m_butScale.locationSc;
    float x1 = but.x(); float y1 = but.y(); float x2 = but.x() + but.width(); float y2 = but.y() - but.height();
    QRectF place = m_butScale.placeSliderSc;
    QRectF pos = m_butScale.positionSliderSc;
    QRectF butZoomIn = m_butScale.butZoomInSc;
    QRectF butZoomOut = m_butScale.butZoomOutSc;
    glColor4f(0.5f, 0.5f, 0.5f, 0.2f);
    glRectf(x1, y1, x2, y2);
    glColor3f(0.5f, 0.5f, 0.5f);
    glRectf(place.x() + but.width()/3, place.y(), place.x() + but.width()/3 + place.width(), place.y() - place.height());
    glColor3f(0.0f, 1.0f, 0.0f);
    glRectf(butZoomIn.x(), butZoomIn.y(), butZoomIn.x()+butZoomIn.width(), butZoomIn.y()+butZoomIn.height());
    glRectf(butZoomOut.x(), butZoomOut.y(), butZoomOut.x()+butZoomOut.width(), butZoomOut.y()+butZoomOut.height());
    glRectf(pos.x(), pos.y() - m_butScale.posSlider, pos.x() + pos.width(), pos.y() - m_butScale.posSlider - pos.height());
    glColor3f(0.0f, 0.0f, 0.0f);
    glRectf(x1 + but.width()/10, y2 + but.height()/20, x2 - but.width()/10, y2 + but.height()/15);
    glRectf(x1 + but.width() * 0.42, y2 + but.height()/50, x2 - but.width() * 0.42, y2 + but.height()/10);
    glRectf(x1 + but.width()/10, y1 - but.height()/20, x2 - but.width()/10, y1 - but.height()/15);
}

void GlGraph::recountScaleSizeButtons()
{
    for(int i=0; i<m_listBut.size(); i++) {
        resizeButton(m_listBut[i].type);
    }

    for(int i=0; i<m_listButGraph.size(); i++) {
        resizeGraphButton(m_listButGraph[i].type);
    }

    resizeGraphButton(SCALE_BUT);
}

void GlGraph::activeButton(TypeButtonGraph type)
{
    int numberChannel = 0;
    int numChanR18 = m_numbersR18Channel[type - R1_BUT];
    m_listButGraph[type].isOn = (m_listButGraph[type].isOn + 1) % 2;

    if(type < SWITCH_BUT) {
        numberChannel = m_numbersTPSChannel[int(type)];
        m_listButGraph[CH1 + numberChannel].isOn = m_listButGraph[type].isOn;
    }
    else if(type >= CH1 && type <= CH8) {
        if(m_typeRadar == TPS_63) {
            for(int i=0; i<4; i++) {
                if((type - CH1) == m_numbersTPSChannel[i]) {
                    m_listButGraph[i].isOn = m_listButGraph[type].isOn;
                    break;
                }
            }
        }
        else if(m_typeRadar == R_18) {
            int n = type - CH1;
            for(int i=0; i<8; i++) {
                if(n == m_numbersR18Channel[i]) {
                    m_listButGraph[i + R1_BUT].isOn = m_listButGraph[type].isOn;
                    break;
                }
            }
        }
    }
    else if(type >= R1_BUT && type <= TRIGGER_R_BUT) {
        m_listButGraph[CH1 + numChanR18].isOn = m_listButGraph[type].isOn;
    }

    if(type >= CH1 && type <= CH8) {
        int channel = type < SWITCH_BUT ? type : type - CH1;
        emit sigSetChannel(channel, m_listButGraph[type].isOn);
    }
    else if(type < SWITCH_BUT) {
        emit sigSetChannel(numberChannel, m_listButGraph[type].isOn);
    }
    else if(type == SWITCH_BUT) {
        m_isAllChannelButtons = (m_isAllChannelButtons + 1) % 2;

        if(m_typeRadar == R_18 && !m_isAllChannelButtons) {
            m_analogGraph.isActive = true;
            m_analogGraphSin.isActive = true;
        }
        else {
            m_analogGraph.isActive = false;
            m_analogGraphSin.isActive = false;
        }
    }
    else if(type >= R1_BUT && type <= S1_S3_BUT) {
        emit sigSetChannel(numChanR18, m_listButGraph[type].isOn);
    }
    else if(type == MODE_GRAPH_BUT) {
        m_isNormGraph = (m_isNormGraph + 1) % 2;
        recountGraphick();
    }

    update();
}

void GlGraph::drawline(const QVector<short> &vec, Color c, int n)
{
    if(n >= CHAN + PROC_CHAN) return;
//    const int S = LENSH/N_PART;
//    GLshort vertexes[S];
//    memcpy(vertexes, vec.data(), sizeof(short) * S);

//    glEnableClientState(GL_VERTEX_ARRAY);
//    glVertexPointer(2, GL_SHORT, 0, vertexes[n]);
//    glColor3f(c.r, c.g, c.b);
//    glDrawArrays(GL_LINES,0,LENSH);
////    glDrawArrays(GL_POINTS,0,LENSH);
//    glDisableClientState(GL_VERTEX_ARRAY);

    int value;
    const int s = m_isNormGraph ? m_sizeLineInt : m_sizeLine;
    GLenum mode = m_modeGraph == LINE_MODE ? GL_LINES : GL_POINTS;
    float sh = m_graghSc.widthField * m_fact;

    glColor3f(c.r, c.g, c.b);
    glBegin(mode);
        for(int i = 0; i < s; i++) {
            value = vec[i]/STEP;
            if(m_modeGraph == LINE_MODE) glVertex2f(i + sh, 300);
            glVertex2f(i + sh, 300 - value);
//            if(m_modeGraph == LINE_MODE) glVertex2f(i + m_rangeIncreaseX, 300);
//            glVertex2f(i + m_rangeIncreaseX, 300 - value);
        }
    glEnd();
}

void GlGraph::drawAnalogLine(const QVector<short> &vec, Color c)
{
    int value;
    int s = m_isNormGraph ? m_sizeLineInt : m_sizeLine;
    GLenum mode = GL_POINTS;
    float yCent = 0;
    float fact = (m_analogGraph.yg1 - m_analogGraph.yg2)/2/1000;
    glPointSize(1);

    glColor3f(c.r, c.g, c.b);
    glBegin(mode);
        for(int i = 0; i < s; i++) {
            value = vec[i]*fact;
            glVertex2f(i, yCent - value);
        }
    glEnd();
}

void GlGraph::drawAnalogSin(const vector<float> &vec, Color c, int num)
{
    int value = 0;
//    int s = vec.size();
//    int s = (*m_pVecSin[num]).size();
    float yCent = 0;
    float fact = (m_analogGraphSin.yg1 - m_analogGraphSin.yg2)/2;
    if(m_analogGraphSin.isHighlightLine[num]) glLineWidth(4);

    int s = m_pVecSinReal[num]->size();
    glColor3f(c.r, c.g, c.b);
    glPointSize(1);
    glBegin(GL_POINTS);
        for(int i=0; i<s; i++) {
            if(vec[i]>1.0 || vec[i]<-1.0 || vec[i]==0.0) continue;
            if(i>m_analogGraphSin.sizeVec) break;
            value = (*m_pVecSinReal[num])[i] * fact;
            glVertex2f(i, yCent - value);
        }
    glEnd();
    glPointSize(1);
    glLineWidth(1);

    s = vec.size();
    glColor3f(c.r, c.g, c.b);
    glBegin(GL_LINE_STRIP);
        for(int i=0; i<s; i++) {
            if(vec[i]>1.0 || vec[i]<-1.0 || vec[i]==0.0) continue;
            if(i>m_analogGraphSin.sizeVec) break;
            value = vec[i] * fact;
            glVertex2f(i, yCent - value);
        }
    glEnd();
    glLineWidth(1);


//    glColor3f(1.0, 0.0, 0.0);
//    glColor3f(c.r, c.g, c.b);
//    glPointSize(6);
//    glBegin(GL_POINTS);
//        for(int i=0, s=m_vecPoint.size(); i<s; i++) {
//            if(m_vecPoint[i].chan == num) {
//                value = m_vecPoint[i].y * fact;
//                glVertex2f(m_vecPoint[i].x, yCent - value);
//            }
//        }
//    glEnd();
//    glPointSize(1);

//    glPointSize(4);
//    glBegin(GL_POINTS);
//    for(int i=0, s=m_vecPoint.size(); i<s; i++) {
//        if(m_vecPoint[i].chan == num) {
//            m_vecPoint[i].wron ? glColor3f(1.0, 0.0, 0.0) : glColor3f(0.0, 1.0, 0.0);
//            value = m_vecPoint[i].y * fact;
//            glVertex2f(m_vecPoint[i].x, yCent - value);
//        }
//    }
//    glEnd();
//    glPointSize(1);

//    glColor3f(1.0, 1.0, 1.0);
//    for(int i=0, s=m_vecPoint.size(); i<s; i++) {
//        if(m_vecPoint[i].chan == num) {
//            value = m_vecPoint[i].y * fact;
//            printString(m_vecPoint[i].x, yCent - value, QString::number(m_vecPoint[i].y, 'f', 3).toStdString().c_str());
//        }
//    }
}

void GlGraph::drawTableSting(float wTab, float hTab, QString title, QString strNumber, int num, int totalNumStr, QPainter &painter)
{
    float width = wTab;
    float height = hTab;
    float shiftTwoStr = width/10 * 9;
    float heightStr = height/(totalNumStr + 1);
    float shiftStr = width/10;
    float widthPlaceWrite = width - shiftStr * 2;
    int lengthStr = strNumber.size() * 8;
    int numberSym = widthPlaceWrite/8 - 3;
    int dif = title.size() - numberSym;

    if(numberSym < title.size()) {
        title.remove(title.size() - dif, dif);
    }
    painter.drawText(shiftStr, heightStr * num, title);
    painter.drawText(shiftTwoStr - lengthStr, heightStr * num, strNumber);
}

void GlGraph::printTablString(float x, float y, float widthTable, QString title, QString strNumber)
{
    int lenTitle = strNumber.size() * 10 * m_scFactX;

    printString(x, y, title.toStdString().c_str());
    printString(x + widthTable * m_scFactX - lenTitle, y, strNumber.toStdString().c_str());
}

void GlGraph::drawTableAngles()
{
    if(m_vecAngles.empty()) return;

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);

    const int N = 10;
    float angl_S3_2[N];
    float angl_S2_1[N];
    float angl_S1_3[N];
    int n = m_vecAngles[0];
    int c = n/N;
    QString title;
    float x = m_widthScreen - 400 * m_scFactX, y = 20 * m_scFactY;
    float st = 15 * m_scFactY;
    float d = 100 * m_scFactX;

    glColor3f(0.0f, 1.0f, 0.0f);
    printString(x, y, "S3_S2");
    glColor3f(0.5f, 1.0f, 0.7f);
    printString(x + d, y, "S2_S1");
    glColor3f(0.0f, 1.0f, 1.0f);
    printString(x + 2*d, y, "S1_S3");
    y += st;
    glColor3f(0.0f, 1.0f, 0.0f);

    for(int i=0; i<N; i++) {
        y += st;
        angl_S3_2[i] = m_vecAngles[i*c+1];
        title = QString::number(angl_S3_2[i]);
        printString(x, y, title.toStdString().c_str());
        angl_S2_1[i] = m_vecAngles[i*c+1+n];
        title = QString::number(angl_S2_1[i]);
        printString(x + d , y, title.toStdString().c_str());
        angl_S1_3[i] = m_vecAngles[i*c+1+2*n];
        title = QString::number(angl_S1_3[i]);
        printString(x + 2*d, y, title.toStdString().c_str());
    }

    glColor3f(0.0f, 1.0f, 0.0f);
    printString(x - 2*d, y, "Total: ");
    title = QString::number(m_countAngle);
    printString(x - d, y, title.toStdString().c_str());
}

void GlGraph::paintContourTabTar(int x, int y, bool isAllTarg)
{
    float widthShift = x;
    float heightShift = y;
    float width = m_tabTar.width;
    float stepShift = m_heightScreen/30;
    float shiftStr = 0.02 * width;
    float shiftStr2 = 0.14 * width;
    float shiftStr3 = 0.45 * width;
    float shiftStr4 = 0.75 * width;
    float heightStart = heightShift + stepShift + stepShift/5;
//    float height = isTitle ? m_tabTar.height : (radar->m_listTabTargets[0].size() + 2) * stepShift;
    float height = 0;

    if(isAllTarg) height = m_tabTar.height;
    else {
        int s = radar->m_listTabTargets[0].size();
        s = s < m_tabTar.N2 ? s : m_tabTar.N2-1;
        height = (s + 2) * stepShift;
    }

    !isAllTarg ? glColor4f(0.0f, 0.0f, 0.0f, 0.3f) : glColor4f(0.0f, 0.5f, 0.0f, 0.3f);
    glRectf(widthShift, heightShift, widthShift + width, heightShift + height);

    glColor3f(0.5f, 0.5f, 0.5f);

    printString(widthShift + shiftStr2, heightShift + 0.8*stepShift, "Time");
    printString(widthShift + shiftStr3, heightShift + 0.8*stepShift, "AZ, deg");
    printString(widthShift + shiftStr4, heightShift + 0.8*stepShift, "RANG, km");
//    if(isTitle) {
//    }

    glBegin(GL_LINES);
//        glVertex2f(widthShift, heightShift);
//        glVertex2f(widthShift, heightLine);
//        glVertex2f(widthShift + width, heightShift);
//        glVertex2f(widthShift + width, heightLine);
        glVertex2f(widthShift, heightShift);
        glVertex2f(widthShift + width, heightShift);
//        glVertex2f(widthShift, heightLine);
//        glVertex2f(widthShift + width, heightLine);
//        if(isTitle) {
//        }
        glVertex2f(widthShift, heightStart);
        glVertex2f(widthShift + width, heightStart);
        glVertex2f(widthShift + shiftStr2 - width/20, heightShift);
        glVertex2f(widthShift + shiftStr2 - width/20, heightStart);
        glVertex2f(widthShift + shiftStr3 - width/20, heightShift);
        glVertex2f(widthShift + shiftStr3 - width/20, heightStart);
        glVertex2f(widthShift + shiftStr4 - width/20, heightShift);
        glVertex2f(widthShift + shiftStr4 - width/20, heightStart);
    glEnd();

    glLineStipple(3, 0xAAAA);
    glEnable(GL_LINE_STIPPLE);
    glBegin(GL_LINES);
        glVertex2f(widthShift + shiftStr2 - width/20, heightShift);
        glVertex2f(widthShift + shiftStr2 - width/20, height);
        glVertex2f(widthShift + shiftStr3 - width/20, heightShift);
        glVertex2f(widthShift + shiftStr3 - width/20, height);
        glVertex2f(widthShift + shiftStr4 - width/20, heightShift);
        glVertex2f(widthShift + shiftStr4 - width/20, height);
    glEnd();
    glDisable(GL_LINE_STIPPLE);

    float h;

    if(isAllTarg) {
        int s = radar->m_listTabTargets[1].size();

        for(int i=m_tabTar.numStSl, n=0; i<s; i++, n++) {
            if(n >= m_tabTar.N1) break;
            h = heightShift + stepShift * (n + 2);
            printString(widthShift + shiftStr, h, QString::number(radar->m_listTabTargets[1][i]->num).toStdString().c_str());
            printString(widthShift + shiftStr2, h, radar->m_listTabTargets[1][i]->time.toStdString().c_str());
            printString(widthShift + shiftStr3, h, QString::number(radar->m_listTabTargets[1][i]->az, 'f', 1).toStdString().c_str());
            printString(widthShift + shiftStr4, h, QString::number(radar->m_listTabTargets[1][i]->range, 'f', 1).toStdString().c_str());

            if(radar->m_listTabTargets[1][i]->isEnd) {
                glBegin(GL_LINES);
                    glVertex2f(widthShift, h + 0.2*stepShift);
                    glVertex2f(widthShift + width, h + 0.2*stepShift);
                glEnd();
            }
        }

        paintScrolTabTar(1);
    }
    else {
        int s = radar->m_listTabTargets[0].size();

        for(int i=m_tabTar.numStSl2, n=0; i<s; i++, n++) {
            if(n >= m_tabTar.N2) break;
            h = heightShift + stepShift * (n + 2);
            printString(widthShift + shiftStr, h, QString::number(radar->m_listTabTargets[0][i]->num).toStdString().c_str());
            printString(widthShift + shiftStr2, h, radar->m_listTabTargets[0][i]->time.toStdString().c_str());
            printString(widthShift + shiftStr3, h, QString::number(radar->m_listTabTargets[0][i]->az, 'f', 1).toStdString().c_str());
            printString(widthShift + shiftStr4, h, QString::number(radar->m_listTabTargets[0][i]->range, 'f', 1).toStdString().c_str());
        }
        if(s >= m_tabTar.N2) paintScrolTabTar(2);
    }
}

void GlGraph::paintScrolTabTar(int n)
{
    QRectF &arr1 = n == 1 ? m_butScroll.rectArrow1 : m_butScroll2.rectArrow1;
    QRectF &strip = n == 1 ? m_butScroll.rectStrip : m_butScroll2.rectStrip;
    QRectF &arr2 = n == 1 ? m_butScroll.rectArrow2 : m_butScroll2.rectArrow2;
    QRectF &slid = n == 1 ? m_butScroll.rectSlid : m_butScroll2.rectSlid;

    glColor3f(0.5f, 0.5f, 0.5f);
    drawFrame(arr1.x(), arr1.y(), arr1.x() + arr1.width(), arr1.y() + arr1.height());
    drawFrame(strip.x(), strip.y(), strip.x() + strip.width(), strip.y() + strip.height());
    drawFrame(arr2.x(), arr2.y(), arr2.x() + arr2.width(), arr2.y() + arr2.height());
    n == 1 ? glColor3f(0.0f, 0.3f, 1.0f) : glColor3f(0.5f, 0.5f, 0.5f);
    float sh0 = 2;
    glRectf(slid.x()+sh0, slid.y(), slid.x() + slid.width()-sh0, slid.y() + slid.height());

    float shHor = 2;
    float shVer = 4;
    glBegin(GL_POLYGON);
        glVertex2f(arr1.x() + arr1.width()/2, arr1.y() + shVer);
        glVertex2f(arr1.x() + shHor, arr1.y() + arr1.height() - shVer);
        glVertex2f(arr1.x() + arr1.width() - shHor, arr1.y() + arr1.height() - shVer);
    glEnd();
    glBegin(GL_POLYGON);
        glVertex2f(arr2.x() + arr2.width()/2, arr2.y() + arr2.height() - shVer);
        glVertex2f(arr2.x() + shHor, arr2.y() + shVer);
        glVertex2f(arr2.x() + arr2.width() - shHor, arr2.y() + shVer);
    glEnd();
}

void GlGraph::paintSymbolPort()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);

    float xPosStr = 10;
    float yPosStr = 80;
    float xPos = xPosStr + 70 * m_scFactX;
    float yPos = yPosStr + 3 * m_scFactY;
    int n = m_numPorts;

    printString(xPosStr, yPosStr, "Ports");

    if(m_numPorts >= 1) {
        glColor3f(0.0f, 1.0f, 0.0f);
        glRectf(xPos, yPos, xPos + 15 * m_scFactX, yPos - 15 * m_scFactY);
        glColor3f(0.5f, 0.5f, 1.0f);
        printString(xPos + 3.5 * m_scFactX, yPosStr, QString::number(m_numPorts).toStdString().c_str());
    }
    else {
        glColor3f(0.0f, 0.8f, 1.0f);
        glRectf(xPos, yPos, xPos + 15 * m_scFactX, yPos - 15 * m_scFactY);

        glColor3f(1.0f, 0.0f, 0.0f);
        glLineWidth(3);
        glBegin(GL_LINES);
            glVertex2f(xPos, yPos);
            glVertex2f(xPos + 15 * m_scFactX, yPos - 15 * m_scFactY);
            glVertex2f(xPos, yPos - 15 * m_scFactY);
            glVertex2f(xPos + 15 * m_scFactX, yPos);
        glEnd();
        glLineWidth(1);
    }
}

void GlGraph::helpScroll(int nArr)
{
    float yS = m_butScroll.rectSlid.y(), hS = m_butScroll.rectSlid.height();
    int s = radar->m_listTabTargets[1].size();
    float st = (m_butScroll.rectStrip.height() - hS)/(s - m_tabTar.N1);

    if(nArr == 1) {
        if(m_tabTar.numStSl == 0) return;
        m_tabTar.numStSl--;
        yS -= st;
        if(s > m_tabTar.N1) m_tabTar.isEndSl = false;
    }
    else if(nArr == 2){
        if(m_tabTar.numStSl == s - m_tabTar.N1) return;
        m_tabTar.numStSl++;
        yS += st;
        if(s > m_tabTar.N1 && m_tabTar.numStSl >= (s - m_tabTar.N1)) m_tabTar.isEndSl = true;
    }

    m_butScroll.rectSlid.setY(yS);
    m_butScroll.rectSlid.setHeight(hS);
    update();
}

void GlGraph::helpScale(int numButZoom)
{
    float val = numButZoom == 1 ? -0.1*m_scale.factScale : 0.1*m_scale.factScale;
    float valueScale = m_scale.factScale += val;
    zoomGraph(valueScale);
}

void GlGraph::paintAnalogInf()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(1.0, 1.0, 1.0);

    float x = m_analogGraphSin.xInf+20*m_scFactX, xR = x-40*m_scFactX, y = m_analogGraphSin.yInf, yR = y-45*m_scFactY;
    int n = -1;
    int el = (m_analogGraphSin.xInf - m_analogGraphSin.x1) * m_analogGraphSin.factScr;
    float half = (m_analogGraphSin.y1 - m_analogGraphSin.y2)/2;
    float val = ((m_analogGraphSin.yInf - m_analogGraphSin.y2) - half) * (-1)/half;
    float w = 105*m_scFactX, h = 80*m_scFactY;

//    for(int i=1; i<=PROC_CHAN; i++) {
////        if(el<0 || el>=(*m_pVecSin[i-1]).size()) throw ExeptionRange("Out or range: ", el, "GlGraph::paintAnalogInf() - m_vecSin");
//        if(el<0 || el>=(*m_pVecSin[i-1]).size()) return;

//        if(m_chan[R+i] && fabs(val-(*m_pVecSin[i-1])[el]) < 0.08) {
//            n = i-1;
//            break;
//        }
//    }

//    for(int i=0; i<PROC_CHAN; i++) {
//        m_analogGraphSin.isHighlightLine[i] = false;
//    }

//    if(n == -1) return;

    int s = (*m_pVecSin[n]).size();
    float st = 15 * m_scFactY;
    QString str;
//    switch (n) {
//    case 0: str = "R:"; break;
//    case 1: str = "S1:"; break;
//    case 2: str = "S2:"; break;
//    case 3: str = "S3:"; break;
//    case 4: str = "120:"; break;
//    case 5: str = "240:"; break;
//    case 6: str = "0:"; break;
//    default:
//        break;
//    }
    glColor4f(0.0f, 0.8f, 0.0f, 0.1f);
    glRectf(xR, yR, xR+w, yR+h);
    glColor3f(0.8f, 0.8f, 0.8f);
    drawFrame(xR, yR, xR+w, yR+h);
    str = QString::number(m_analogGraphSin.sizeVec);
    printString(x, y, str.toStdString().c_str());

//    printString(x-40*m_scFactX, y-2*st, str.toStdString().c_str());
//    str = QString::number((*m_pVecSin[n])[el], 'f', 3);
//    printString(x+15*m_scFactX, y, str.toStdString().c_str());
//    if((el-1)<s) str = QString::number((*m_pVecSin[n])[el-1], 'f', 3);
//    printString(x, y-st, str.toStdString().c_str());
//    if((el-2)<s) str = QString::number((*m_pVecSin[n])[el-2], 'f', 3);
//    printString(x, y-2*st, str.toStdString().c_str());
//    if((el+1)<s) str = QString::number((*m_pVecSin[n])[el+1], 'f', 3);
//    printString(x, y+st, str.toStdString().c_str());
//    if((el+2)<s) str = QString::number((*m_pVecSin[n])[el+2], 'f', 3);
//    printString(x, y+2*st, str.toStdString().c_str());

//    m_analogGraphSin.isHighlightLine[n] = true;
}

void GlGraph::drawTargets(float xCent, float yCent)
{
    short l = LENCUR;
    short w = WIDCUR;
    float x1, y1, x2, y2, x3, y3, x, y;
    float az, range;
    float valSin, valCos;
    float factScale = 1.055;
    float factAz = 1.03;
//    int minValueTarg = 16000/100 * (100 - m_filterTarg);
    int minValue = 500;
    int maxValue = m_maxVideo;

    glColor3f(1.0f, 1.0f, 0.0f);
    glPointSize(1);

    /*float fAz = factAz * 360/4096;
    int count = 0;
//    int iSt = radar->m_angle > 30 ? (radar->m_angle - 30) * 4096/360 : 0;
    int iSt = 0;
//    int iFin = radar->m_angle * 4096/360;
//    int iFin = radar->m_angle / fAz;
    int iFin = w;

    for(int i=iSt; i<iFin; i++) {
        az = (float)(*m_imageTargets)[i * l] * fAz;
//        if(az>radar->m_angle) break;
        valSin = sin(az * P/180)/factScale;
        valCos = cos(az * P/180)/factScale;

        for(int j=0, k=0, c=0; j<2000; j++) {
            if((*m_imageTargets)[i*l+j] > minValue) {
                range = j * 0.15;
                x = xCent + range * valSin;
                y = yCent - range * valCos;
                float col = (float)(*m_imageTargets)[i*l+j]/maxValue;
                k = count * 2;
                c = count * 3;
                vertexesTarCol[c] = col;
                vertexesTarCol[c+1] = col;
                vertexesTarCol[c+2] = 0.0;
                vertexesTar[k] = x;
                vertexesTar[k+1] = y;
//                radar->vertexesTarCol[c] = col;
//                radar->vertexesTarCol[c+1] = col;
//                radar->vertexesTarCol[c+2] = 0.0;
//                radar->vertexesTar[k] = x;
//                radar->vertexesTar[k+1] = y;
                count++;
            }
        }
    }

    glEnableClientState(GL_COLOR_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glColorPointer(3, GL_FLOAT, 0, vertexesTarCol);
    glVertexPointer(2, GL_FLOAT, 0, vertexesTar);
//    glColorPointer(3, GL_FLOAT, 0, radar->vertexesTarCol);
//    glVertexPointer(2, GL_FLOAT, 0, radar->vertexesTar);
    glDrawArrays(GL_POINTS, 0, count);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);*/

    foreach (auto target, radar->m_listTargets) {
        if(target->isVisible) {
            x1 = xCent + target->list[0].second * sin(target->list[0].first * P/180)/factScale;
            y1 = yCent - target->list[0].second * cos(target->list[0].first * P/180)/factScale;
            x2 = xCent + target->list[1].second * sin(target->list[1].first * P/180)/factScale;
            y2 = yCent - target->list[1].second * cos(target->list[1].first * P/180)/factScale;
            x3 = xCent + target->list[2].second * sin(target->list[2].first * P/180)/factScale;
            y3 = yCent - target->list[2].second * cos(target->list[2].first * P/180)/factScale;

            float difAng = radar->m_angle > target->list[0].first ? radar->m_angle - target->list[0].first :
                    360 - target->list[0].first + radar->m_angle;
            float col = 1 - difAng * 0.002;
//            float col = 1 - difAng * 0.002;

            if(!target->isTarget) {
//                glColor3f(col, col, 0.0f);
                glColor3f(1.0f, 1.0f, 0.0f);
//                glLineWidth(2);
                glPointSize(1);
            }
            else {
                float colOr = 0.5 - difAng * 0.001;
                glColor3f(col, colOr, 0.0f);
                glPointSize(2);
            }

            QVector<float> listMarks;
            fullTarget(listMarks, target->list[0].first, target->list[1].first);
            float range = target->list[0].second;

            glBegin(GL_POINTS);
                for(int i=0; i<listMarks.size(); i++) {
                    float x = xCent + range * sin(listMarks[i] * P/180)/factScale;
                    float y = yCent - range * cos(listMarks[i] * P/180)/factScale;

                    glVertex2f(x, y);
                }
            glEnd();
        }
    }
}

void GlGraph::fullTarget(QVector<float> &vecPoints, float f, float l)
{
    const float INTERVAL = 0.2;
    int s = (l - f)/INTERVAL + 1;
    vecPoints.resize(s);

    for(int i=0; i<s-1; i++) {
        vecPoints[i] = f + INTERVAL * i;
    }

    vecPoints[s - 1] = l;
}

void GlGraph::drawDesignationRadar(float xCent, float yCent, float radius)
{
    float factStretch = 1 + m_scFactY/10;

    glColor3f(0.4f, 0.4f, 0.4f);

    for(int i=50; i<350; i+=50) {
        printString(xCent + 3, yCent - (i - 15)/factStretch, QString::number(i).toStdString().c_str());
    }

    const qreal P = 3.14159265;
    float angle;
    float xPos;
    float yPos;
    float xShift;
    float yShift;

    for(int i=0; i<360; i+=30) {
        angle = (i - 90) * P/180;
        xPos = xCent + radius * cos(angle);
        yPos = yCent + radius * sin(angle);
        xShift = i > 180 ? 35 * m_scFactY : -10 * m_scFactY;
        yShift = (i>90 && i<270) ? 18 * m_scFactY : -2;
        xShift = i==0 || i==180 ? 1 * m_scFactY : xShift;
        yShift = i==0 ? -6 * m_scFactY : yShift;
        printString(xPos - xShift, yPos + yShift, QString::number(i).toStdString().c_str());
    }
}

void GlGraph::drawRadarMask(float xCentr, float yCentr)
{
    glColor4f(0.0f, 1.0f, 1.0f, 0.15f);
    for(QList<RadarMask*>::iterator iter = m_listMask->begin(); iter!=m_listMask->end(); iter++) {
        paintMask(xCentr, yCentr, (*iter)->az[0], (*iter)->az[1], (*iter)->rang[0], (*iter)->rang[1]);
    }
}

Color GlGraph::countColor(short value)
{
    const short MAXVAL = 32767;
    const short PIECE = MAXVAL/3;
    Color c;

    if(value < 500) {
        c.g = 0.0;
        c.b = 0.0;
        c.r = 0.0;
    }
    else if(value > PIECE * 2) {
        c.g = 0.0;
        c.b = 1.0;
        c.r = (value - PIECE * 2)/PIECE;
    }
    else if(value > PIECE) {
        c.r = 0.0;
        c.g = 1.0;
        c.b = (value - PIECE * 2)/PIECE;
    }
    else if(value < PIECE) {
        c.b = 0.0;
        c.r = 1.0;
        c.g = (value - PIECE * 2)/PIECE;
    }

    return c;
}

void GlGraph::paintMask(float xCent, float yCent, float az1, float az2, float rang1, float rang2)
{
    float factScale = 1.07;
    const int N = 30;
    float startAng = az1;
    float angle = az2 - az1;
    float ang = (startAng - 90) * P/180;
    float incAng = angle * P/(180 * N);
    float radius = rang2/factScale;
    float radius2 = rang1/factScale;

    glBegin(GL_QUAD_STRIP);
        for(int i = 0; i < N; i++, ang += incAng) {
           glVertex2f(xCent + radius * cos(ang), yCent + radius * sin(ang));
           glVertex2f(xCent + radius2 * cos(ang), yCent + radius2 * sin(ang));
        }

        if(angle == 360) {
            ang = (startAng - 90) * P/180;
            glVertex2f(xCent + radius * cos(ang), yCent + radius * sin(ang));
            glVertex2f(xCent + radius2 * cos(ang), yCent + radius2 * sin(ang));
        }
        glEnd();
}

void GlGraph::paintCursor(float x, float y, float rad, float stAng)
{
    const int N = 50;
    float angle = 360;
    float ang = stAng;
    float incAng = angle * P/(180 * N);
    float col = 0.05;
    float inkCol = 0.25/N;
    glColor3f(0.0f, col, 0.0f);

    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(x, y);
        glVertex2f(x + rad * cos(ang), y + rad * sin(ang));

        for(int i=0; i<N; i++, ang+=incAng) {
           glVertex2f(x + rad * cos(ang), y + rad * sin(ang));
           col += inkCol;
           glColor3f(0.0f, col, 0.0f);
        }
        glVertex2f(x + rad * cos(ang), y + rad * sin(ang));
    glEnd();
}

void GlGraph::cutString(QString &str, int w)
{
    int numberSym = w/8 - 3;
    int dif = str.size() - numberSym;

    if(numberSym < str.size()) {
        str.remove(str.size() - dif, dif);
    }
}

void GlGraph::makeRasterFont()
{
    GLubyte space[] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };

    GLubyte letters[][13] = {
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x36, 0x36, 0x36, 0x36},
        {0x00, 0x00, 0x00, 0x66, 0x66, 0xff, 0x66, 0x66, 0xff, 0x66, 0x66, 0x00, 0x00},
        {0x00, 0x00, 0x18, 0x7e, 0xff, 0x1b, 0x1f, 0x7e, 0xf8, 0xd8, 0xff, 0x7e, 0x18},
        {0x00, 0x00, 0x0e, 0x1b, 0xdb, 0x6e, 0x30, 0x18, 0x0c, 0x76, 0xdb, 0xd8, 0x70},
        {0x00, 0x00, 0x7f, 0xc6, 0xcf, 0xd8, 0x70, 0x70, 0xd8, 0xcc, 0xcc, 0x6c, 0x38},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x1c, 0x0c, 0x0e},
        {0x00, 0x00, 0x0c, 0x18, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c},
        {0x00, 0x00, 0x30, 0x18, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x18, 0x30},
        {0x00, 0x00, 0x00, 0x00, 0x99, 0x5a, 0x3c, 0xff, 0x3c, 0x5a, 0x99, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0xff, 0xff, 0x18, 0x18, 0x18, 0x00, 0x00},
        {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x60, 0x60, 0x30, 0x30, 0x18, 0x18, 0x0c, 0x0c, 0x06, 0x06, 0x03, 0x03},
        {0x00, 0x00, 0x3c, 0x66, 0xc3, 0xe3, 0xf3, 0xdb, 0xcf, 0xc7, 0xc3, 0x66, 0x3c},
        {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78, 0x38, 0x18},
        {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0xe7, 0x7e},
        {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0x07, 0x03, 0x03, 0xe7, 0x7e},
        {0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0xff, 0xcc, 0x6c, 0x3c, 0x1c, 0x0c},
        {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
        {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x03, 0x03, 0xff},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e},
        {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x03, 0x7f, 0xe7, 0xc3, 0xc3, 0xe7, 0x7e},
        {0x00, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x38, 0x38, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x30, 0x18, 0x1c, 0x1c, 0x00, 0x00, 0x1c, 0x1c, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x06, 0x0c, 0x18, 0x30, 0x60, 0xc0, 0x60, 0x30, 0x18, 0x0c, 0x06},
        {0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x60, 0x30, 0x18, 0x0c, 0x06, 0x03, 0x06, 0x0c, 0x18, 0x30, 0x60},
        {0x00, 0x00, 0x18, 0x00, 0x00, 0x18, 0x18, 0x0c, 0x06, 0x03, 0xc3, 0xc3, 0x7e},
        {0x00, 0x00, 0x3f, 0x60, 0xcf, 0xdb, 0xd3, 0xdd, 0xc3, 0x7e, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0x66, 0x3c, 0x18},
        {0x00, 0x00, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
        {0x00, 0x00, 0x7e, 0xe7, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
        {0x00, 0x00, 0xfc, 0xce, 0xc7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc7, 0xce, 0xfc},
        {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xc0, 0xff},
        {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfc, 0xc0, 0xc0, 0xc0, 0xff},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xcf, 0xc0, 0xc0, 0xc0, 0xc0, 0xe7, 0x7e},
        {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xff, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
        {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7e},
        {0x00, 0x00, 0x7c, 0xee, 0xc6, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06},
        {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xe0, 0xf0, 0xd8, 0xcc, 0xc6, 0xc3},
        {0x00, 0x00, 0xff, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
        {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xdb, 0xff, 0xff, 0xe7, 0xc3},
        {0x00, 0x00, 0xc7, 0xc7, 0xcf, 0xcf, 0xdf, 0xdb, 0xfb, 0xf3, 0xf3, 0xe3, 0xe3},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xe7, 0x7e},
        {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
        {0x00, 0x00, 0x3f, 0x6e, 0xdf, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0x66, 0x3c},
        {0x00, 0x00, 0xc3, 0xc6, 0xcc, 0xd8, 0xf0, 0xfe, 0xc7, 0xc3, 0xc3, 0xc7, 0xfe},
        {0x00, 0x00, 0x7e, 0xe7, 0x03, 0x03, 0x07, 0x7e, 0xe0, 0xc0, 0xc0, 0xe7, 0x7e},
        {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0xff},
        {0x00, 0x00, 0x7e, 0xe7, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
        {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
        {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xff, 0xdb, 0xdb, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3},
        {0x00, 0x00, 0xc3, 0x66, 0x66, 0x3c, 0x3c, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
        {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3},
        {0x00, 0x00, 0xff, 0xc0, 0xc0, 0x60, 0x30, 0x7e, 0x0c, 0x06, 0x03, 0x03, 0xff},
        {0x00, 0x00, 0x3c, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3c},
        {0x00, 0x03, 0x03, 0x06, 0x06, 0x0c, 0x0c, 0x18, 0x18, 0x30, 0x30, 0x60, 0x60},
        {0x00, 0x00, 0x3c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x3c},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18},
        {0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x38, 0x30, 0x70},
        {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0x7f, 0x03, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0},
        {0x00, 0x00, 0x7e, 0xc3, 0xc0, 0xc0, 0xc0, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x03, 0x03, 0x03, 0x03, 0x03},
        {0x00, 0x00, 0x7f, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x30, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x33, 0x1e},
        {0x7e, 0xc3, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0x7e, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0xc0, 0xc0, 0xc0, 0xc0},
        {0x00, 0x00, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x18, 0x00},
        {0x38, 0x6c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x0c, 0x00},
        {0x00, 0x00, 0xc6, 0xcc, 0xf8, 0xf0, 0xd8, 0xcc, 0xc6, 0xc0, 0xc0, 0xc0, 0xc0},
        {0x00, 0x00, 0x7e, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x78},
        {0x00, 0x00, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xdb, 0xfe, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xfc, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x7c, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x7c, 0x00, 0x00, 0x00, 0x00},
        {0xc0, 0xc0, 0xc0, 0xfe, 0xc3, 0xc3, 0xc3, 0xc3, 0xfe, 0x00, 0x00, 0x00, 0x00},
        {0x03, 0x03, 0x03, 0x7f, 0xc3, 0xc3, 0xc3, 0xc3, 0x7f, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xe0, 0xfe, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xfe, 0x03, 0x03, 0x7e, 0xc0, 0xc0, 0x7f, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x1c, 0x36, 0x30, 0x30, 0x30, 0x30, 0xfc, 0x30, 0x30, 0x30, 0x00},
        {0x00, 0x00, 0x7e, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x18, 0x3c, 0x3c, 0x66, 0x66, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc3, 0xe7, 0xff, 0xdb, 0xc3, 0xc3, 0xc3, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xc3, 0x66, 0x3c, 0x18, 0x3c, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00},
        {0xc0, 0x60, 0x60, 0x30, 0x18, 0x3c, 0x66, 0x66, 0xc3, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0xff, 0x60, 0x30, 0x18, 0x0c, 0x06, 0xff, 0x00, 0x00, 0x00, 0x00},
        {0x00, 0x00, 0x0f, 0x18, 0x18, 0x18, 0x38, 0xf0, 0x38, 0x18, 0x18, 0x18, 0x0f},
        {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
        {0x00, 0x00, 0xf0, 0x18, 0x18, 0x18, 0x1c, 0x0f, 0x1c, 0x18, 0x18, 0x18, 0xf0},
        {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x8f, 0xf1, 0x60, 0x00, 0x00, 0x00}
    };

    GLuint i, j;
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    fontOffset = glGenLists(128);

    for(i=32; i<127; i++) {
        glNewList(fontOffset + i, GL_COMPILE);
        glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, letters[i - 32]);
        glEndList();
    }

    glNewList(fontOffset + ' ', GL_COMPILE);
    glBitmap(8, 13, 0.0, 2.0, 10.0, 0.0, space);
    glEndList();
}

void GlGraph::printString(float x, float y, const char *s)
{
    glRasterPos2f(x, y);

    glPushAttrib(GL_LIST_BIT);
    glListBase(fontOffset);
    glCallLists(strlen(s), GL_UNSIGNED_BYTE, (GLubyte*) s);
    glPopAttrib();
}

void GlGraph::setPaintChan(int num, bool isPaint)
{
    if(num >= CHAN + PROC_CHAN) {
        return;
    }

    m_chan[num] = isPaint;
}

void GlGraph::setThreshold(const int *threshold)
{
    m_threshold = threshold;
}

void GlGraph::resizeGraphVec(const QVector<QVector<short>> *vec)
{
    m_vecLine.resize(vec->size());
    m_vecLineInter.resize(vec->size());
    m_sizeLine = LENSH;
    m_sizeVec = vec->size();
    m_sizeLineInt = LENSH/N_PART;

    for(int i = 0; i < m_sizeVec; i++) {
        m_vecLine[i].resize(m_sizeLine);
        m_vecLineInter[i].resize(m_sizeLineInt);
    }

//    for(int i=0; i<PROC_CHAN; i++) {
//        m_vecSin[i].resize(m_sizeSin * 50, 0.5);
//        m_nVecSin[i] = 0;
//    }

    m_analogGraph.sizeVec = m_vecLine[0].size();
    m_analogGraph.factScr = m_analogGraph.sizeVec/(m_analogGraph.x2 - m_analogGraph.x1);
    m_analogGraphSin.factScr = m_analogGraphSin.sizeVec/(m_analogGraphSin.x2 - m_analogGraphSin.x1);
}

void GlGraph::runImage(bool running)
{
    if(running) {
        int n = 1;
        m_timerImgGraph->start(1000/m_numPart/n);
        m_timerImgRadar->start(1000/60);
    }
    else {
        m_timerImgGraph->stop();
        m_timerImgRadar->stop();
    }

    radar->setMoving(running);
}

void GlGraph::setData(float data, TypeData type)
{
    int choice = type;

    switch (choice) {
    case PRF:
        m_dataTable.PRF = data; break;
    case IMPULS:
        m_dataTable.widthImpTrig = data;
        if(data > 10) m_dataTable.typeTrigger = XMIT;
        else m_dataTable.typeTrigger = R0;
        break;
    case ROTARION:
        m_dataTable.timeRotation = data; radar->m_cursor.timeRotation = data * 1000; break;
    case AZIMUTH:
        m_dataTable.ACP = data; break;
    case NORTH:
        m_dataTable.ARP = data; break;
    case TARGET:
        m_dataTable.video = data; break;
    case TRIG:
        m_dataTable.trigger = data; break;
    default:
        break;
    }
}

void GlGraph::setListMask(QList<RadarMask *> &list)
{
    m_listMask = &list;
}

void GlGraph::placeButtons()
{
    int numberButtons = m_listBut.size();
    float xPos = 0; float yPos = 0;

    if(m_modeLocBut == SIDE) {
        m_widthBut = m_widthScreen/15;
        m_heightBut = m_heightScreen/numberButtons;
        xPos = m_widthScreen - m_widthBut;
    }
    else if(m_modeLocBut == BOTTOM){
        m_widthBut = m_widthScreen/numberButtons;
        m_heightBut = m_heightScreen/15;
        yPos = m_heightScreen - 2;
    }

    for(int i=0; i<m_listBut.size(); i++) {
        m_modeLocBut == BOTTOM ? xPos = m_widthBut * i : yPos = m_heightBut * (i + 1);
        m_listBut[i].location.setRect(xPos, yPos, m_widthBut, m_heightBut);
    }

    recountScaleSizeButtons();
    update();
}

void GlGraph::setArrayTargets(vector<short> *array)
{
    m_imageTargets = array;
    radar->vecTargets = array;
}

void GlGraph::setNoiseFilterTarg(int value)
{
    m_filterTarg = value;
}

void GlGraph::setTypeRadar(TypeRadar typeRadar)
{
    m_typeRadar = typeRadar;
    update();
}

void GlGraph::setNumbersTPSChannel(const int *numbersChannel)
{
    m_numbersTPSChannel = numbersChannel;
}

void GlGraph::setNumbersR18Channel(const int *numbersChannel)
{
    m_numbersR18Channel = numbersChannel;
}

void GlGraph::setVecSin(vector<float> *vec, int nVec)
{
    if(nVec<0 || nVec>=PROC_CHAN) return;
    m_pVecSin[nVec] = vec;
}

void GlGraph::setVecSinReal(vector<float> *vec, int nVec)
{
    if(nVec<0 || nVec>=PROC_CHAN) return;
    m_pVecSinReal[nVec] = vec;
}

void GlGraph::setIsConnectPort(int n)
{
    m_isConnectPort = false;
    m_numPorts = n;
}

void GlGraph::setMaxVideo(int value)
{
    m_maxVideo = value;
    radar->maxVideo = value;
}

void GlGraph::switchModeScreen(bool up)
{
    if(up) {
        m_modeScreen = (ModeScreen)(m_modeScreen + 1);
        m_choosedBut = (TypeButtonRadar)(m_choosedBut + 1);
        m_modeScreen = m_modeScreen > 5 ? (ModeScreen)0 : m_modeScreen;
        m_choosedBut = m_choosedBut > 5 ? (TypeButtonRadar)0 : m_choosedBut;
    }
    else {
        m_modeScreen = (ModeScreen)(m_modeScreen - 1);
        m_choosedBut = (TypeButtonRadar)(m_choosedBut - 1);
        m_modeScreen = m_modeScreen < 0 ? (ModeScreen)5 : m_modeScreen;
        m_choosedBut = m_choosedBut < 0 ? (TypeButtonRadar)5 : m_choosedBut;
    }

    update();
}

void GlGraph::switchOnAnalog()
{
    m_modeScreen = GRAPH;
    m_choosedBut = GRAPH_BUT;
    m_isAllChannelButtons = false;
    m_analogGraph.isActive = true;
    m_analogGraphSin.isActive = true;
    m_listButGraph[SWITCH_BUT].isOn = true;
    emit sigSetChannel(S1_BUT-6, true);
    emit sigSetChannel(S2_BUT-6, true);
    emit sigSetChannel(S3_BUT-6, true);
    m_listButGraph[S1P_BUT].isOn = true;
    m_listButGraph[S2P_BUT].isOn = true;
    m_listButGraph[S3P_BUT].isOn = true;
}

void GlGraph::slotUpdata()
{
    update();
}

void GlGraph::slotSetTimeBoard(int time)
{
    m_timeBoard = time;
}

void GlGraph::slotSetTimeProcessor(qreal time)
{
    m_timeProcessor = time;
}

void GlGraph::slotSetButLayout(ModeLocationButtons mode)
{
    m_modeLocBut = mode;
    m_curveSc.fH = m_modeLocBut == BOTTOM ? m_curveSc.F_H : m_curveSc.F_HC;
    m_curveSc.fW_2 = m_modeLocBut == SIDE ? m_curveSc.F_W : m_curveSc.F_WC;
    m_graghSc.fH = m_modeLocBut == BOTTOM ? m_graghSc.F_H : m_graghSc.F_HC;
    m_graghSc.fW_2 = m_modeLocBut == SIDE ? m_graghSc.F_W : m_graghSc.F_WC;

    recountGraphicTitles();
    placeButtons();
}

void GlGraph::slotSetSinchData(bool sinch[N_SIN])
{
    for(int i=0; i<3; i++) {
        m_isSinch[i] = sinch[i];
    }
}

//void GlGraph::slotSetVecSin(vector<float>* vec[PROC_CHAN], bool isCorectData)
//{
//    if(!m_isUpdate) {
//        return;
//    }

//    if(m_nVecSin[4] == 0) {
//        for(int i=0; i<PROC_CHAN; i++) {
//            int s = m_vecSin[i].size();

//            for(int j=0; j<s; j++) {
//                m_vecSin[i][j] = 0.0;
//            }

//            m_nVecSin[i] = 0.0;
//        }
//        m_vecPoint.clear();
//    }

//    if(isCorectData) {
//        for(int i=0; i<PROC_CHAN; i++) {
//            int sCop = (*vec[i]).size() <= 50 ? (*vec[i]).size() : 50;
//            if(sCop > (*vec[i]).size()) throw ExeptionRange("Out or range: ", sCop, "GlGraph::slotSetVecSin() -  vec");

//            memcpy(m_vecSin[i].data() + m_nVecSin[i], (*vec[i]).data(), sCop * sizeof(float));
//            m_nVecSin[i] += sCop;
//            m_nVecSin[i] = m_nVecSin[i] < (m_sizeSin - 1) * N_SAMPLE_MSIN ? m_nVecSin[i] : 0;
//        }
//    }
//    else {
//        const int N = 50;
//        vector<float> temp(N, 3.0);

//        for(int i=0; i<PROC_CHAN; i++) {
//            memcpy(m_vecSin[i].data() + m_nVecSin[i], temp.data(), N * sizeof(float));
//            m_nVecSin[i] += N;
//            m_nVecSin[i] = m_nVecSin[i] < (m_sizeSin - 1) * N_SAMPLE_MSIN ? m_nVecSin[i] : 0;
//        }
//    }
//}

void GlGraph::slotSetCountAngleR18(float angle)
{
    m_countAngle = angle;
}

void GlGraph::slotSetPoints(QVector<PointSin> &vec)
{
    for(int i=0, j=m_vecPoint.size(), s=vec.size(); i<s; i++, j++) {
        m_vecPoint.push_back(vec[i]);
        if(j >= m_vecPoint.size()) throw ExeptionRange("Out or range: ", j, "GlGraph::slotSetCountAngleR18 -  vec");
        m_vecPoint[j].x += m_nVecSin[1];
    }
}

void GlGraph::slotSetSizeSin(int n)
{
    m_analogGraphSin.sizeVec = n;
    m_analogGraphSin.factScr = m_analogGraphSin.sizeVec/(m_analogGraphSin.x2 - m_analogGraphSin.x1);
}

void GlGraph::slotSetLine(QVector<QVector<short int>> &vecLine)
{
    if(!m_isUpdate) {
        return;
    }

//    int W = CHAN + PROC_CHAN;
//    int L = LENSH;
//    for(int i=0; i<W; i++) {
//        for(int j=0; j<L; j++) {
//            vertexes[i][j].x1 = j + m_rangeIncreaseX;
//            vertexes[i][j].x2 = j + m_rangeIncreaseX;
//            vertexes[i][j].y2 = SUB - vecLine[i][j]/STEP;
//        }
//    }

    for(int i = 0; i < m_sizeVec; i++) {
        if(m_chan[i] == true) {
            memcpy(m_vecLine[i].data(), vecLine[i].data(), sizeof(short) * m_sizeLine);
        }
    }

    update();
}

void GlGraph::slotReceiveAngles(vector<float> &angles)
{
    int s = angles.size();

    if(m_vecAngles.size() < s) {
        m_vecAngles.clear();
        m_vecAngles.resize(s);
    }

    memcpy(m_vecAngles.data(), angles.data(), sizeof(float) * s);
}

void GlGraph::slotUpdataImgGraph()
{
    if(!m_isUpdate) {
        return;
    }

    for(int i = 0; i < m_sizeVec; i++) {
        if(m_chan[i] == true) {
            memcpy(m_vecLineInter[i].data(), m_vecLine[i].data() + m_posUpdata, m_sizeLineInt * sizeof(short));
        }
    }

    m_posUpdata += m_sizeLine/m_numPart;
    m_posUpdata = m_sizeLine - m_posUpdata > m_sizeLine/100 ? m_posUpdata : 0;
    if(m_modeScreen == GRAPH) update();
}

void GlGraph::slotUpdataImgRadar()
{
    if(m_modeScreen == RADAR) update();
}

void GlGraph::slotChangedTabTar(int n)
{
    if(n == 1) {
        int s = radar->m_listTabTargets[1].size();

        if(s > m_tabTar.N1) {
            float x = m_butScroll.rectSlid.x();
            float y = m_butScroll.rectSlid.y();
            float w = m_butScroll.rectSlid.width();
            float h = m_butScroll.rectSlid.height();
            float ySt = m_butScroll.rectArrow1.y() + m_butScroll.rectArrow1.height();
            h = m_butScroll.normHeight * m_tabTar.N1/s;

            if(m_tabTar.isEndSl) {
                y = ySt + m_butScroll.normHeight - h;
                m_tabTar.numStSl = s - m_tabTar.N1;
            }

            m_butScroll.rectSlid.setRect(x, y, w, h);
            m_tabTar.numStSl2 = 0;
        }
    }
    else {
        int s = radar->m_listTabTargets[0].size();

        if(s > m_tabTar.N2) {
            float x = m_butScroll2.rectSlid.x();
            float y = m_butScroll2.rectSlid.y();
            float w = m_butScroll2.rectSlid.width();
            float h = m_butScroll2.rectSlid.height();
            float ySt = m_butScroll2.rectArrow1.y() + m_butScroll2.rectArrow1.height();
            h = m_butScroll2.normHeight * m_tabTar.N2/s;
            y = ySt + m_butScroll2.normHeight - h;
            m_tabTar.numStSl2 = s - m_tabTar.N2;
            m_butScroll2.rectSlid.setRect(x, y, w, h);
        }
    }
}

void GlGraph::slotRunScroll()
{
    m_butScroll.timer1->stop();
    m_butScroll.timer2->start(50);
}

void GlGraph::slotRunStepScroll()
{
    helpScroll(m_butScroll.nArr);
}

void GlGraph::slotButZoom()
{
    helpScale(m_butScale.numButZoom);
}

void GlGraph::checkBoardGraphick()
{
    float leftEdge = -m_scale.xShiftScale + m_scale.dxShift;
    if(leftEdge > 0) {
        m_scale.dxShift = m_scale.xShiftScale;
    }

    float upEdge = -m_scale.yShiftScale + m_scale.dyShift;
    if(upEdge > 0) {
        m_scale.dyShift = m_scale.yShiftScale;
    }

    float rightEdge = -m_scale.xShiftScale + m_scale.dxShift + m_widthShedule * m_scale.factScale;
    if(rightEdge < m_widthShedule) {
        m_scale.dxShift = m_widthShedule + m_scale.xShiftScale - m_widthShedule * m_scale.factScale;
    }

    float downEdge = -m_scale.yShiftScale + m_scale.dyShift + m_heightShedule * m_scale.factScale;
    if(downEdge < m_heightShedule) {
        m_scale.dyShift = m_heightShedule + m_scale.yShiftScale - m_heightShedule * m_scale.factScale;
    }
}

void GlGraph::checkBoardCurve()
{
    float leftEdge = -m_scaleCurve.xShiftScale + m_scaleCurve.dxShift;
    if(leftEdge > 0) {
        m_scaleCurve.dxShift = m_scaleCurve.xShiftScale;
    }

    float upEdge = -m_scaleCurve.yShiftScale + m_scaleCurve.dyShift;
    if(upEdge > 0) {
        m_scaleCurve.dyShift = m_scaleCurve.yShiftScale;
    }

    float maxRight = m_curveSc.globWidthField + m_curveSc.globWidthWind;
    float rightEdge = -m_scaleCurve.xShiftScale + m_scaleCurve.dxShift + maxRight * m_scaleCurve.factScale;
    if(rightEdge < maxRight) {
        m_scaleCurve.dxShift = maxRight + m_scaleCurve.xShiftScale - maxRight * m_scaleCurve.factScale;
    }

    float maxDown = m_curveSc.globHeightWind;
    float downEdge = -m_scaleCurve.yShiftScale + m_scaleCurve.dyShift + m_curveSc.globHeightWind * m_scaleCurve.factScale;
    if(downEdge < m_heightShedule) {
        m_scaleCurve.dyShift = maxDown + m_scaleCurve.yShiftScale - maxDown * m_scaleCurve.factScale;
    }
}

void GlGraph::scaleGraph()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float dw = m_graghSc.widthWind/m_widthShedule;
    float dh = m_graghSc.heightWind/m_heightShedule;
    glScalef(dw * m_scale.factScale, dh * m_scale.factScale, 1.0);
//    glScalef(m_scale.factScale/m_fact, m_scale.factScale, 1);

//    short shiftX = m_rangeIncreaseX;
    float shiftX = m_graghSc.widthField * m_fact;
    float shiftY = 0;
    int w = LENSH/N_PART;
    int h = WIDSH;
    float xShift = shiftX * (1 - 1/m_scale.factScale);
    float yShift = shiftY * (1 - 1/m_scale.factScale);
    float xShiftSc = w/m_scale.factScale * (m_scale.factScale - 1)/2;
    float yShiftSc = h/m_scale.factScale * (m_scale.factScale - 1)/2;
    float dx = xShift + xShiftSc - m_scale.dxShift;
    float dy = yShift + yShiftSc - m_scale.dyShift;

    dx = -dx <= -xShift ? dx : xShift;
    dx = -dx >= -m_scale.xShiftScale * 2/m_scale.factScale ? dx : m_scale.xShiftScale * 2/m_scale.factScale;
    dy = -dy <= -yShift ? dy : -yShift;
    dy = -dy >= -m_scale.yShiftScale * 2/m_scale.factScale ? dy : m_scale.yShiftScale * 2/m_scale.factScale;
    glTranslatef(-dx, -dy, 0.0);

    if(m_isMoveMouse || m_isSliderScale) {
        m_xPosScreen = (m_widthScreen/2/m_scale.factScale + dx/m_fact);
        m_yPosScreen = m_heightShedule/2/m_scale.factScale + dy;

        float up = m_yPosScreen - m_heightShedule/m_scale.factScale/2;
        float down = m_yPosScreen + m_heightShedule/m_scale.factScale/2;
        float left = m_xPosScreen - m_widthScreen/m_scale.factScale/2;
        float right = m_xPosScreen + m_widthScreen/m_scale.factScale/2;

        for(int i=0, s=m_listGraphicY.size(); i<s; i++) {
            if(m_listGraphicY[i].pos < up || m_listGraphicY[i].pos > down) {
                m_listGraphicY[i].isVisible = false;
            }
            else {
                m_listGraphicY[i].isVisible = true;
                m_listGraphicY[i].posDigit = m_listGraphicY[i].pos * m_scale.factScale - dy * m_scale.factScale;
                if(m_listGraphicY[i].posDigit > m_graghSc.heightWind) m_listGraphicY[i].isVisible = false;
            }
        }

        for(int i=0, s=m_listGraphicX.size(); i<s; i++) {
            if(m_listGraphicX[i].pos < left || m_listGraphicX[i].pos > right) {
                m_listGraphicX[i].isVisible = false;
            }
            else {
                m_listGraphicX[i].isVisible = true;
                m_listGraphicX[i].posDigit = m_listGraphicX[i].pos * m_scale.factScale - dx/m_fact * m_scale.factScale;
            }
        }

        m_isMoveMouse = false;
    }

}

void GlGraph::scaleCurve()
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    int w = WIDCUR;
    int l = LENCUR;
    float dw = m_curveSc.widthWind/w;
    float dh = m_curveSc.heightWind/l;
    glScalef(dw * m_scaleCurve.factScale, dh * m_scaleCurve.factScale, 1.0);

    float dxField = m_curveSc.widthField/dw;
    float dyField = 0.0;
    float dxScale = w/m_scaleCurve.factScale * (m_scaleCurve.factScale - 1)/2;
    float dyScale = l/m_scaleCurve.factScale * (m_scaleCurve.factScale - 1)/2;
    float widFeild = w/m_curveSc.widthWind * m_curveSc.widthField;
    float dxF = widFeild * (1 - 1/m_scaleCurve.factScale);
    float dyF = 0;
    float dx = dxField - dxScale - dxF + m_scaleCurve.dxShift;
    float dy = dyField - dyScale - dyF + m_scaleCurve.dyShift;
    float horEdge = dxField/m_scaleCurve.factScale;
    float horIncrease = (m_curveSc.globWidthWind * m_scaleCurve.factScale - m_curveSc.globWidthWind)/m_scaleCurve.factScale;
    float verEdge = dyField/m_scaleCurve.factScale;
    float verIncrease = (m_curveSc.globHeightWind * m_scaleCurve.factScale - m_curveSc.globHeightWind)/m_scaleCurve.factScale;

    dx = dx > horEdge ? horEdge : dx;
    dx = dx < horEdge - horIncrease ? horEdge - horIncrease : dx;
    dy = dy > dyField/m_scaleCurve.factScale ? dyField/m_scaleCurve.factScale : dy;
    dy = dy < verEdge - verIncrease ? verEdge - verIncrease : dy;
    glTranslatef(dx, dy, 0.0);

    if(m_isMoveMouse) {
        float xCentPosScr = m_curveSc.globWidthWind/2 + m_scaleCurve.dxShift;
        float yCentPosScr = m_curveSc.globHeightWind/2 + m_scaleCurve.dyShift;
        float up = yCentPosScr - m_curveSc.globHeightWind/m_scaleCurve.factScale/2;
        float down = yCentPosScr + m_curveSc.globHeightWind/m_scaleCurve.factScale/2;
        float left = xCentPosScr - m_curveSc.globWidthWind/m_scaleCurve.factScale/2;
        float right = xCentPosScr + m_curveSc.globWidthWind/m_scaleCurve.factScale/2;

        for(int i=0; i<m_listGraphicCurveY.size(); i++) {
            if(m_listGraphicCurveY[i].pos < up || m_listGraphicCurveY[i].pos > down) {
                m_listGraphicCurveY[i].posDigit = m_listGraphicCurveY[i].pos * m_scaleCurve.factScale + dy/m_curveSc.fScY * m_scaleCurve.factScale;
//                m_listGraphicCurveY[i].isVisible = false;
            }
            else {
//                m_listGraphicCurveY[i].isVisible = true;
                m_listGraphicCurveY[i].posDigit = m_listGraphicCurveY[i].pos * m_scaleCurve.factScale + dy/m_curveSc.fScY * m_scaleCurve.factScale;
//                if(m_listGraphicCurveY[i].posDigit > m_curveSc.heightWind) m_listGraphicCurveY[i].isVisible = false;
            }
        }

        for(int i=0; i<m_listGraphicCurveX.size(); i++) {
            if(m_listGraphicCurveX[i].pos < left || m_listGraphicCurveX[i].pos > right) {
                m_listGraphicX[i].isVisible = false;
            }
            else {
                m_listGraphicCurveX[i].isVisible = true;
            }

            m_listGraphicCurveX[i].posDigit = m_listGraphicCurveX[i].pos * m_scaleCurve.factScale +
                    (dx+dxF)/m_curveSc.fScX * m_scaleCurve.factScale;
        }

        m_isMoveMouse = false;
    }
}

void GlGraph::zoomGraph(float factorScale)
{
    float factTemp = m_scale.factScale;
    m_scale.factScale = factorScale;
    m_scale.factScale = m_scale.factScale > 1.0 ? m_scale.factScale : 1.0;
    m_scale.xShiftScale = (m_widthShedule * m_scale.factScale - m_widthShedule)/2;
    m_scale.yShiftScale = (m_heightShedule * m_scale.factScale - m_heightShedule)/2;

    if(factorScale > factTemp) {
        if(m_scale.factScale > 6) {
            for(int i=0; i<m_listGraphicY.size(); i++) {
                if(m_listGraphicY[i].rang == 3) {
                     m_listGraphicY[i].isActive = true;
                }
            }
            for(int i=0; i<m_listGraphicX.size(); i++) {
                if(m_listGraphicX[i].rang == 3) {
                       m_listGraphicX[i].isActive = true;
                }
            }
        }
        else if(m_scale.factScale > 3) {
             for(int i=0; i<m_listGraphicY.size(); i++) {
                 if(m_listGraphicY[i].rang == 2) {
                        m_listGraphicY[i].isActive = true;
                 }
             }
             for(int i=0; i<m_listGraphicX.size(); i++) {
                 if(m_listGraphicX[i].rang == 2) {
                        m_listGraphicX[i].isActive = true;
                 }
             }
        }
    }
    else {
        if(m_scale.factScale < 3) {
            for(int i=0; i<m_listGraphicY.size(); i++) {
                if(m_listGraphicY[i].rang == 2 || m_listGraphicY[i].rang == 3) {
                     m_listGraphicY[i].isActive = false;
                 }
             }
             for(int i=0; i<m_listGraphicX.size(); i++) {
                if(m_listGraphicX[i].rang == 2 || m_listGraphicX[i].rang == 3) {
                     m_listGraphicX[i].isActive = false;
                 }
             }
        }
        else if(m_scale.factScale < 6 && m_scale.factScale > 5.5) {
            for(int i=0; i<m_listGraphicY.size(); i++) {
                if(m_listGraphicY[i].rang == 3) {
                     m_listGraphicY[i].isActive = false;
                 }
            }
            for(int i=0; i<m_listGraphicX.size(); i++) {
                if(m_listGraphicX[i].rang == 3) {
                     m_listGraphicX[i].isActive = false;
                 }
            }
        }
    }

    if(factorScale < m_scale.factScale) {
        checkBoardGraphick();
    }

    placeSlider();
    m_isMoveMouse = true;
    update();
}

void GlGraph::zoomGraphCurve(float factorScale)
{
//    float factTemp = m_scaleCurve.factScale;
    m_scaleCurve.factScale = factorScale;
    m_scaleCurve.factScale = m_scaleCurve.factScale > 1.0 ? m_scaleCurve.factScale : 1.0;
    m_scaleCurve.xShiftScale = (m_curveSc.widthWind * m_scaleCurve.factScale - m_curveSc.widthWind)/2;
    m_scaleCurve.yShiftScale = (m_curveSc.heightWind * m_scaleCurve.factScale - m_curveSc.heightWind)/2;

    /*if(factorScale > factTemp) {
        if(m_scale.factScale > 6) {
            for(int i=0; i<m_listGraphicY.size(); i++) {
                if(m_listGraphicY[i].rang == 3) {
                     m_listGraphicY[i].isActive = true;
                }
            }
            for(int i=0; i<m_listGraphicX.size(); i++) {
                if(m_listGraphicX[i].rang == 3) {
                       m_listGraphicX[i].isActive = true;
                }
            }
        }
        else if(m_scale.factScale > 3) {
             for(int i=0; i<m_listGraphicY.size(); i++) {
                 if(m_listGraphicY[i].rang == 2) {
                        m_listGraphicY[i].isActive = true;
                 }
             }
             for(int i=0; i<m_listGraphicX.size(); i++) {
                 if(m_listGraphicX[i].rang == 2) {
                        m_listGraphicX[i].isActive = true;
                 }
             }
        }
    }
    else {
        if(m_scale.factScale < 3) {
            for(int i=0; i<m_listGraphicY.size(); i++) {
                if(m_listGraphicY[i].rang == 2 || m_listGraphicY[i].rang == 3) {
                     m_listGraphicY[i].isActive = false;
                 }
             }
             for(int i=0; i<m_listGraphicX.size(); i++) {
                if(m_listGraphicX[i].rang == 2 || m_listGraphicX[i].rang == 3) {
                     m_listGraphicX[i].isActive = false;
                 }
             }
        }
        else if(m_scale.factScale < 6 && m_scale.factScale > 5.5) {
            for(int i=0; i<m_listGraphicY.size(); i++) {
                if(m_listGraphicY[i].rang == 3) {
                     m_listGraphicY[i].isActive = false;
                 }
            }
            for(int i=0; i<m_listGraphicX.size(); i++) {
                if(m_listGraphicX[i].rang == 3) {
                     m_listGraphicX[i].isActive = false;
                 }
            }
        }
    }*/

//    if(factorScale < m_scale.factScale) {
//        checkBoard();
//    }

//    m_isMoveMouse = true;
    if(factorScale < m_scaleCurve.factScale) {
        checkBoardCurve();
    }

    m_isMoveMouse = true;
    update();
}

void GlGraph::recountGraphick()
{
    if(m_isNormGraph) {
        m_widthShedule = LENSH/N_PART;
        m_fact = m_widthShedule/1000;
    }
    else {
        m_widthShedule = LENSH;
        m_fact = m_widthShedule/1000;
    }
}

void GlGraph::drawCircle(float xCen, float yCen, float radius)
{
    const qreal P = 3.14159265;
    const int N = 50;
    float ang = 0 * P/180;
    float incAng = 359 * P/(180 * N);

    glBegin(GL_LINE_LOOP);
        glVertex2f(xCen + radius * cos(ang), yCen + radius * sin(ang));

        for(int i = 0; i < N; i++, ang +=incAng) {
           glVertex2f(xCen + radius * cos(ang), yCen + radius * sin(ang));
        }
    glEnd();
}

void GlGraph::drawBlendCircle(float xCen, float yCen, float radius, float posAng)
{
    const qreal P = 3.14159265;
    const int N = 50;
    float ang = posAng;
    float incAng = 360 * P/(180 * N);
    float c = 0.1;
    const float INK = 0.2/N;
//    glColor3f(0.0f, c, 0.0f);

    glBegin(GL_LINE_LOOP);
        glVertex2f(xCen + radius * cos(ang), yCen + radius * sin(ang));

        for(int i = 0; i < N; i++, ang +=incAng) {
           c += INK;
//           glColor3f(0.0f, c, 0.0f);
           glColor3f(c, c, c);
           glVertex2f(xCen + radius * cos(ang), yCen + radius * sin(ang));
        }
    glEnd();
}

void GlGraph::paintCircle(float xCen, float yCen, float radius)
{
    const qreal P = 3.14159265;
    const int N = 50;
    float ang = 0 * P/180;
    float incAng = 359 * P/(180 * N);

    glBegin(GL_POLYGON);
        glVertex2f(xCen + radius * cos(ang), yCen + radius * sin(ang));

        for(int i = 0; i < N; i++, ang += incAng) {
           glVertex2f(xCen + radius * cos(ang), yCen + radius * sin(ang));
        }
    glEnd();
}

void GlGraph::paintBlendCircle(float xCen, float yCen, float radius, float posAng)
{
    const qreal P = 3.14159265;
    const int N = 50;
//    float ang = 0 * P/180;
    float ang = posAng;
    float incAng = 359 * P/(180 * N);
    float c = 0.05;
    const float INK = 0.25/N;
    glColor3f(0.0f, c, 0.0f);

    glBegin(GL_POLYGON);
        glVertex2f(xCen + radius * cos(ang), yCen + radius * sin(ang));

        for(int i = 0; i < N; i++, ang += incAng) {
            c += INK;
            glColor3f(0.0f, c, 0.0f);
           glVertex2f(xCen + radius * cos(ang), yCen + radius * sin(ang));
        }
    glEnd();
}

void GlGraph::paintEllipse(float xC, float yC, float rx, float ry)
{
    const qreal P = 3.14159265;
    const float step = float(2 * P) / 360;

        glBegin(GL_TRIANGLE_FAN);

        glVertex2f(xC, yC);
        for (float angle = 0; angle <= float(2 * P); angle += step)
        {
            float a = (fabsf(angle - float(2 * P)) < 0.00001f) ? 0.f : angle;
            const float dx = rx * cosf(a);
            const float dy = ry * sinf(a);
            glVertex2f(dx + xC, dy + yC);
        }
        glEnd();
}

void GlGraph::paintRotateEllipse(float xC, float yC, float rx, float ry, float angle)
{
    const qreal P = 3.14159265;
    const float step = float(2 * P) / 360;
    const float ang = angle * P / 180;

    glBegin(GL_TRIANGLE_FAN);

    glVertex2f(xC, yC);

        for (float angle = 0; angle <= float(2 * P); angle += step)
        {
            float a = (fabsf(angle - float(2 * P)) < 0.00001f) ? 0.f : angle;
            const float dx = rx * cosf(a);
            const float dy = ry * sinf(a);
            const float dxP = dx * cos(ang) - dy * sin(ang);
            const float dyP = dx * sin(ang) + dy * cos(ang);
            glVertex2f(xC + dxP, yC + dyP);
        }

    glEnd();
}

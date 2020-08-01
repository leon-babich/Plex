#include "radar.h"

Radar::Radar(QObject *parent) : QObject(parent)
{
    m_isMovingCursor = false;
    m_isMoving = false;
    m_numberRotation = 0;
    m_tabRotation = 1;
    m_isMarkARP = false;
    m_numTarg = 0;
    m_isCorrectARP = true;
    m_isAllTabTar = false;
    m_isWriteTargets = false;
    m_isPrintedTitleRecTar = false;
    m_startAngle = 0;
//    m_angle = m_startAngle;
    setAngle(m_startAngle);
    m_azimuth = 0;
    m_realAngle = 0;
    m_cursR18.spTimer = 800/m_cursR18.N_CADR;
    m_cursR18.targAng = 0;
    m_posArrTarg = 0;
    m_remAng = 0;
    m_remPosEndTarg = 0;
    maxVideo = 3000;
    m_modeMoving = SMOOTHLY;
//    m_modeMoving = INDIGREES;

    m_cursor.timerMove = new QTimer;
    connect(m_cursor.timerMove,&QTimer::timeout,this,&Radar::slotMove);

    m_cursR18.timerMove = new QTimer;
    connect(m_cursR18.timerMove,&QTimer::timeout,this,&Radar::slotMoveR18);

    m_cursor.timerCorrect = new QTimer;
    connect(m_cursor.timerCorrect,&QTimer::timeout,this,&Radar::slotStopCorrect);

    m_timerMarkARP = new QTimer;
    connect(m_timerMarkARP,&QTimer::timeout,this,&Radar::slotBlinkCursor);

    clearRadar();
    clearArrayTargetsImg();
}

void Radar::setMoving(bool moving)
{
    m_isMoving = moving;

    if(m_isMoving) {
        if(m_isMovingCursor) {
            m_cursor.timerMove->start(m_cursor.speed);
        }

//        if(!m_cursR18.timerMove->isActive()) {
//            m_cursR18.timerMove->start(m_cursR18.spTimer);
//        }
    }
    else {
        m_cursor.timerMove->stop();
//        m_cursR18.timerMove->stop();

        if(m_cursor.timerCorrect->isActive()) {
            m_cursor.sAng = m_cursor.sAngNorm;
            m_cursor.timerCorrect->stop();
        }
    }
}

void Radar::setStartAngle(int angle)
{
    m_startAngle = angle;
}

void Radar::setTimeBlockTarg(QTime *time)
{
    m_timeBlockTarg = time;
}

void Radar::setArrayTargets(vector<short> *array)
{
    vecTargets = array;
}

void Radar::setTypeRadar(TypeRadar typeRadar)
{
    m_typeRadar = typeRadar;
}

void Radar::setWritingTarg(bool writing)
{
    m_isWriteTargets = writing;
}

void Radar::setPathFileTarg(QString path)
{
//    if(m_fileIn->isOpen()) {
//        m_fileIn->close();
//    }

    m_pathRecTar = path;
    m_totalPathRecTar = path;
    //    m_fileIn->setFileName(m_path);
}

void Radar::setNewTimePath()
{
    QDate data = QDate::currentDate();
    QTime time = QTime::currentTime();
    QString path = m_pathRecTar;
    QString nameTime;
    path.remove(".txt");
    nameTime.append("-[");
    nameTime.append(data.toString("dd_MM_yyyy"));
    nameTime.append("][");
    nameTime.append(time.toString("hh_mm"));
    nameTime.append("]");
    m_totalPathRecTar = path + nameTime;
    m_totalPathRecTar.append(".txt");
}

void Radar::clearRadar()
{
    setAngle(m_startAngle);
    if(!m_listTargets.isEmpty()) m_listTargets.clear();

    m_isMovingCursor = false;
    m_numberRotation = 0;
    m_tabRotation = 1;
    m_isCorrectARP = true;
    m_isPrintedTitleRecTar = false;

    updataCursor();
}

void Radar::clearArrayTargetsImg()
{
    const int T = LENCUR * WIDCUR;

    if(m_arrayTargetsImg.empty()) {
        m_arrayTargetsImg.resize(T);
    }

//    for(int i=0; i<T; i++) {
//        m_arrayTargetsImg[i] = 0;
//    }
}

float Radar::getDAng()
{
    return m_cursor.sAng;
}

void Radar::slotMove()
{
    if(!m_isMovingCursor) {
        return;
    }

    m_angle += m_cursor.sAng;

    if(m_angle >= 360) {
//        m_angle = 0;
        setAngle(0);
        m_numberRotation++;
    }

    if(((int)m_angle % 5) == 0 && (int)m_angle != m_remAng) {
        updataTargets(m_angle);
//        updataArrTargets((int)m_angle);
        m_remAng = (int)m_angle;
    }

//    emit sigUpdata();
}

void Radar::slotMoveR18()
{
    m_angle += m_cursR18.spAng;

    if(m_angle >= 360) {
//        m_angle = 0;
        setAngle(0);
        m_numberRotation++;
    }

    if(m_angle >= m_cursR18.targAng) {
//        m_timerMoveR18->stop();
    }
    emit sigUpdata();
}

void Radar::slotCheckNorthCursor(int rotation)
{
//    m_tabRotation++;
//    if(m_tabRotation > N_TAB) {
//        m_tabRotation = 1;
//        m_isAllTabTar = true;
//    }

    eraseListDetTarg(m_tabRotation-1);

    if(!m_isCorrectARP) {
        return;
    }

    m_azimuth = 0;

    if(m_isMovingCursor) {
        int delay = m_cursor.usualAz < 400 ? 10 : 40;
        int difference = (rotation * 360 + m_startAngle) - (m_numberRotation * 360 + m_angle + delay);
        m_dif = difference;

        if(abs(difference) > 90) {
//            m_angle = m_startAngle;
            setAngle(m_startAngle);
            m_numberRotation = rotation;
        }
    }
    if(!m_isMovingCursor) {
        m_isMovingCursor = true;
        if(m_modeMoving == SMOOTHLY) m_cursor.timerMove->start(m_cursor.speed);
    }

    m_isMarkARP = true;
    m_timerMarkARP->start(500);

//    if(!m_listTabTargets[0].isEmpty()) {
//        m_listTabTargets[0][m_listTabTargets[0].size() - 1]->isEnd = true;
//        m_numTarg = 0;
//    }
}

void Radar::slotAddBlockTarget(vector<Target *> block)
{
    QTime time = QTime::currentTime();
    QString strTime;
    strTime = time.toString("hh:mm:ss");
    int n = 0;

    foreach (Target *target, block) {
        if((target->rang[0] > 300) || (target->az[0] == -1)) {
            continue;
        }

        FindingTarget *targetRadar = new FindingTarget;

        for(int i = 0; i < 3; i++) {
            targetRadar->list[i].first = target->az[i] + m_startAngle;
            targetRadar->list[i].first = targetRadar->list[i].first < 360 ? targetRadar->list[i].first : targetRadar->list[i].first - 360;
            targetRadar->list[i].second = target->rang[i];
        }

        targetRadar->isVisible = false;
        targetRadar->isTarget = target->isTarget;
        targetRadar->turn = m_numberRotation;

        m_listTargets.push_back(targetRadar);

        if(target->isTarget) {
            TabTarget *detTarg = new TabTarget();
            detTarg->num = ++m_numTarg;
            detTarg->az = target->az[0];
            detTarg->range = target->rang[0];
            detTarg->time = strTime;
            detTarg->isEnd = false;
            detTarg->rotation = m_tabRotation;
//            if(m_tabRotation < 1 || m_tabRotation > N_TAB) m_tabRotation = 1;
//            if(m_listTabTargets[m_tabRotation-1].size() < S) {
//            }
            m_listTabTargets[0].append(detTarg);
            n++;

            if(m_numTarg >= 999) m_numTarg = 0;
        }

        delete target;
    }

    if(n != 0) emit sigChangedTabTar(2);

    if(m_isWriteTargets) {
        writingTargets(n);
    }

    int timeTarg = m_timeBlockTarg->elapsed();
}

void Radar::slotSetSpeedCursor(int rotation, int az)
{
    m_cursor.bufAz += az;
    m_cursor.countAz++;
    m_azimuth += az;
    m_realAngle = (float)m_azimuth/4096 * 360;

    if(m_cursor.countAz % 3 == 0) {
        m_cursor.middleAz = m_cursor.bufAz/m_cursor.countAz;
        float maxDif = m_cursor.middleAz * m_cursor.FACT_DIF_AZ;

        if(fabsf(m_cursor.usualAz - m_cursor.middleAz) > maxDif) {
            m_cursor.usualAz = m_cursor.middleAz;
            m_cursor.sAngNorm = (float)m_cursor.usualAz * m_cursor.S_ANG / m_cursor.AZIMUTH;
            m_cursor.sAng = m_cursor.sAngNorm;
        }
    }

    if(m_cursor.countAz % 2 == 0) {
        int delay = m_cursor.usualAz < 400 ? 10 : 40;
        float difference = rotation * 360 + m_startAngle + m_realAngle - (m_numberRotation * 360 + delay + m_angle);
        correctSpeedCursor(difference);
        m_dif = difference;
    }

    if(m_cursor.countAz > 6) {
        m_cursor.countAz = 0;
        m_cursor.bufAz = 0;
    }
}

void Radar::slotSetSpeedCursorR18(float ang)
{
    if(!m_isMovingCursor) return;

    int vAng = ang > m_countAngR18 ? ang - m_countAngR18 : 360 - m_countAngR18 + ang;
    m_cursor.bufAz += vAng;
    m_cursor.countAz++;
    m_countAngR18 = ang;
    m_realAngle = ang;

    if(m_cursor.countAz % 3 == 0) {
        m_cursor.middleAz = m_cursor.bufAz/m_cursor.countAz;
        float maxDif = m_cursor.middleAz * m_cursor.FACT_DIF_AZ;

        if(fabsf(m_cursor.usualAz - m_cursor.middleAz) > maxDif) {
            m_cursor.usualAz = m_cursor.middleAz;
            m_cursor.sAngNorm = (float)m_cursor.usualAz * m_cursor.S_ANG / m_cursor.ANGLE;
            m_cursor.sAng = m_cursor.sAngNorm;
        }
    }

//    if(m_cursor.countAz % 2 == 0) {
//        int delay = m_cursor.usualAz < 40 ? 10 : 40;
//        float difference = m_startAngle + m_realAngle - (delay + m_angle);
//        correctSpeedCursor(difference);
//        m_dif = difference;
//    }

    if(m_cursor.countAz > 6) {
        m_cursor.countAz = 0;
        m_cursor.bufAz = 0;
    }
}

void Radar::slotSetCorrectARP(bool correct)
{
    m_isCorrectARP = correct;

    if(!m_isCorrectARP) {
        setMoving(false);
    }
    else {
        clearRadar();
        setMoving(true);
    }
}

void Radar::slotSetCountAngleR18(float angle)
{
    /*if(!m_cursR18.isStart) {
        if(angle < 30) m_cursR18.isStart = true;
        else return;
    }

    if((angle - m_angle) > 45) {
        m_angle = angle;
    }
    float spAng;

    if(angle > m_cursR18.targAng) {
        spAng = (angle - m_cursR18.targAng) / m_cursR18.N_CADR;
    }
    else {
        int range = 360 - m_cursR18.targAng + angle;
        spAng = range / m_cursR18.N_CADR;
    }

//    if(spAng == 0) return;
    m_cursR18.bufStAng += spAng;
    if(++m_cursR18.cMid >= 3) {
        m_cursR18.midSpAng = m_cursR18.bufStAng / 3;
        m_cursR18.cMid = 0;
        m_cursR18.bufStAng = 0;
    }

    m_cursR18.targAng = angle;

//    if(spAng == 0 || m_cursR18.midSpAng/spAng > 1.4) {
//        qDebug()<<"Worked protect";
//        return;
//    }
//    if(m_cursR18.midSpAng != 0 && spAng/m_cursR18.midSpAng > 1.4) {
//        qDebug()<<"Worked protect 2  "<<m_cursR18.midSpAng;
//        return;
//    }

//    if(m_cursR18.spAng!=0 && spAng/m_cursR18.spAng > 1.2) {
////        qDebug()<<"Worked protect";
//        if(++m_cursR18.cExt >= 2) {
////            qDebug()<<"Worked protect 2";
//            m_cursR18.spAng = m_cursR18.remExtSpAng;
//            m_cursR18.cExt = 0;
//        }

//        m_cursR18.remExtSpAng = spAng;
//    }
//    else {
//        m_cursR18.cExt = 0;
//        m_cursR18.spAng = spAng;
//    }

    m_cursR18.spAng = spAng;
    m_cursR18.spAng = m_cursR18.spAng <= m_cursR18.M_ANG ? m_cursR18.spAng : m_cursR18.M_ANG;
    m_cursR18.spAng = m_cursR18.spAng >= 0 ? m_cursR18.spAng : 0;*/

    if(!m_isMovingCursor) return;
    if(m_modeMoving == INDIGREES) {
        setAngle(angle);
        emit sigUpdata();
    }
}

void Radar::slotSetModeMoving()
{
    m_modeMoving = (ModeMovingRadar)((m_modeMoving + 1) % 2);

    if(m_modeMoving == SMOOTHLY) {
        if(m_isMovingCursor) {
            m_cursor.timerMove->start(m_cursor.speed);
        }
    }
    else if(m_modeMoving == INDIGREES) {
        m_cursor.timerMove->stop();
        m_cursor.timerCorrect->stop();
    }
}

void Radar::slotBlinkCursor()
{
    m_isMarkARP = false;
    m_timerMarkARP->stop();
}

void Radar::slotStopCorrect()
{
    m_cursor.sAng = m_cursor.sAngNorm;
    m_cursor.timerCorrect->stop();
}

void Radar::updataTargets(int azimuth)
{
    int az;

    foreach (FindingTarget* target, m_listTargets) {
        az = target->list[0].first;

        if(az <= azimuth) {
            if(target->turn != m_numberRotation) {
                m_listTargets.removeOne(target);
            }
            else {
                target->isVisible = true;
            }
        }
    }
}

void Radar::correctSpeedCursor(int difference)
{
    if(m_cursor.timerCorrect->isActive() || (difference > 0 && difference < 15)) {
        return;
    }

    float difValue = difference > 30 ? m_cursor.sAngNorm/10 : m_cursor.sAngNorm/20;
    difValue = difference < 0 ? difValue * -2 : difValue;

    m_difValue = difValue;
    m_cursor.sAng = m_cursor.sAng + difValue;
    m_timeDif = 1800;
    m_cursor.timerCorrect->start(abs(m_timeDif));
}

void Radar::updataCursor()
{
    m_cursor.speed = 14;
    m_cursor.sAngNorm = ANG_NORM;
    m_cursor.sAng = ANG_NORM;
    m_cursor.countAz = 0;
    m_cursor.bufAz = 0;
    m_cursor.countRepeat = 0;
    m_cursor.lastDif = 0;
    m_cursor.timeRotation = 10 * SEC;

    if(m_typeRadar == TPS_63) {
        m_cursor.middleAz = AZ_NORM;
        m_cursor.usualAz = AZ_NORM;
    }
    else if(m_typeRadar == R_18) {
        m_cursor.usualAz = ANGL_NORM;
        m_cursor.middleAz = ANGL_NORM;
    }

    m_cursR18.isStart = false;
    m_cursR18.targAng = 0;
    m_cursR18.bufStAng = 0;
    m_cursR18.midSpAng = 0;
    m_cursR18.cMid = 0;
    m_cursR18.spAng = 0;
    m_cursR18.cExt = 0;
}

void Radar::eraseListDetTarg(int rotation)
{
//    if(rotation < 1 || rotation > N_TAB) return;
//    m_listTabTargets[rotation-1].clear();

    if(!m_listTabTargets[1].isEmpty()) {
        m_listTabTargets[1][m_listTabTargets[1].size() - 1]->isEnd = true;
//        m_numTarg = 0;
    }

    for(int i=0; i<m_listTabTargets[0].size(); i++) {
        m_listTabTargets[1].push_back(move(m_listTabTargets[0][i]));
    }

    emit sigChangedTabTar(1);

    m_listTabTargets[0].clear();

    if(m_listTabTargets[1].size() >= 1000) {
        QVector<TabTarget*>::iterator itSt = m_listTabTargets[1].begin();
        QVector<TabTarget*>::iterator itFin = m_listTabTargets[1].begin();

        for(; itFin<m_listTabTargets[1].end(); itFin++) {
            if((*itFin)->isEnd) {
                break;
            }
        }

        m_listTabTargets[1].erase(itSt, itFin);
    }
}

void Radar::writingTargets(int n)
{
    QFile fileIn(m_totalPathRecTar);

    if(!fileIn.open(QIODevice::WriteOnly | QIODevice::Append))
    {
//        emit sigSendMessage("<FONT COLOR=RED>Fail opening file targets: " + m_pathRecTar);
        qDebug()<<"Eror opening file";
        return;
    }

    QString str;
    QString strSp1;
    QString strSp2;
    int r = m_tabRotation - 1;
    if(r < 0 || r >= N_TAB) return;
    int i = m_listTabTargets[r].size() - n;
    if(i < 0) return;

    QTextStream stream(&fileIn);

    if(!m_isPrintedTitleRecTar) {
        str = "____________________________________";
        stream << str << endl;

        str = "N   | Time     | Az, deg | Rang, km";
        stream << str << endl;
        str = "____|__________|_________|_________";
        stream << str << endl;
        m_isPrintedTitleRecTar = true;
    }

    for(int s=m_listTabTargets[r].size(); i<s; i++) {
        strSp1 = m_listTabTargets[r][i]->num >= 10 ? "  | " : "   | ";
        strSp1 = m_listTabTargets[r][i]->num >= 100 ? " | " : strSp1;
        strSp2 = m_listTabTargets[r][i]->az >= 10.0 ? "    | " : "     | ";
        strSp2 = m_listTabTargets[r][i]->az >= 100.0 ? "   | " : strSp2;
        str = QString::number(m_listTabTargets[r][i]->num) + strSp1 + m_listTabTargets[r][i]->time + " | " +
                QString::number(m_listTabTargets[r][i]->az, 'f', 1) + strSp2 + QString::number(m_listTabTargets[r][i]->range, 'f', 1);
        stream << str << endl;
    }
}

void Radar::updataArrTargets(int ang)
{
    short l = LENCUR;
    short w = WIDCUR;
    float x, y;
    float az, range;
    float valSin, valCos;
    float factScale = 1.055;
    float factAz = 1.03;
//    int minValueTarg = 16000/100 * (100 - m_filterTarg);
    int minValue = 500;
    int maxValue = maxVideo;

    float fAz = factAz * 360/4096;
    float fAz2 = 360/4096;
//    int count = 0;
//    int iSt = ang >= 5 ? (ang - 5) * 4096/360 : 0;
    int iSt = 0;
//    int iFin = ang * 4096/360;
//    qDebug()<<iFin;
//    int iFin = radar->m_angle / fAz;
    int iFin = w;
//    for(int i=0; i<w; i++) {
//        if((int)((*vecTargets)[i * l] * fAz) == (ang-5)) {
//            iSt = i;
//        }
//        else if((int)((*vecTargets)[i * l] * fAz) == ang) {
//            iFin = i;
//            break;
//        }
//    }
//    qDebug()<<iSt<<" "<<iFin;

    for(int i=iSt; i<iFin; i++) {
        az = (float)(*vecTargets)[i * l] * fAz;
//        if(az>radar->m_angle) break;
        valSin = sin(az * P/180)/factScale;
        valCos = cos(az * P/180)/factScale;

        for(int j=0, k=0, c=0; j<2000; j++) {
            if((*vecTargets)[i*l+j] > minValue) {
                range = j * 0.15;
                x = xPosRadar + range * valSin;
                y = yPosRadar - range * valCos;
                float col = (float)(*vecTargets)[i*l+j]/maxValue;
                k = m_countArrTar * 2;
                c = m_countArrTar * 3;
                vertexesTarCol[c] = col;
                vertexesTarCol[c+1] = col;
                vertexesTarCol[c+2] = 0.0;
                vertexesTar[k] = x;
                vertexesTar[k+1] = y;
                m_countArrTar++;
            }
        }
    }
}

void Radar::setAngle(int ang)
{
    if(ang == 0) sizeArrTar = m_countArrTar;
    m_angle = ang;
    m_countArrTar = ang * 4096/360;
}

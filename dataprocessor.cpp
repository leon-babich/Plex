#include "dataprocessor.h"

DataProcessor::DataProcessor(QObject *parent) : QObject(parent)
{
    m_isRunning = true;
//    m_isRunning = false;
    m_isWorking = false;
    m_pMutex = new QMutex;
    modeRead = BOARD;
    m_typeRadar = TPS_63;
    m_isdisplay = true;
    m_isWriting = false;
    m_timeRun = 0;
    m_timeClear = 0;
    m_sizeLine = LENSH;
    m_sizeBuf = LENGTHARRAY/S/LENSH;
    m_numPaintChan = 0;
    m_timeReading = 0;
    m_isCorrectARP = true;
    m_startAngle = 0;
    m_isProtectCheckSig = true;
    m_isReduceSigSR18 = false;
    m_isReduceSigRR18 = true;

//    for(int i=0; i<WIDCUR; i++) {
//        m_arrayAzTarg[i] = 0;
//    }

    m_vecLine.resize(CHAN + PROC_CHAN);
    for(int i = 0; i < CHAN + PROC_CHAN; i++) {
        m_vecLine[i].resize(LENSH);
    }

    for(int i = 0; i < CHAN + PROC_CHAN; i++) {
        m_isPaint[i] = false;
    }

    for(int i = 0; i < 4; i++) {
        autoValue[i] = true;
    }

    m_settings.filter[0] = 50.0;
    m_settings.filter[1] = 50.0;
    m_settings.filter[2] = 50.0;
    m_settings.filter[3] = 50.0;

    m_fileIn = new QFile;
    m_path = "WriteData.bin";
//    m_fileIn->setFileName(m_path);

    m_timerNorth = new QTime;
    m_timerNorth->start();

    m_timeBlockTarg = new QTime;
    m_timeBlockTarg->start();

    m_settings.nameChannel.insert(0, "ACP");
    m_settings.nameChannel.insert(1, "ARP");
    m_settings.nameChannel.insert(2, "Video");
    m_settings.nameChannel.insert(3, "Trigger");
    m_settings.nameChannel.insert(4, "");
    m_settings.nameChannel.insert(5, "");
    m_settings.nameChannel.insert(6, "");
    m_settings.nameChannel.insert(7, "");

    m_settings.azimuth = 0;
    m_settings.north = 1;
    m_settings.target = 2;
    m_settings.trigger = 3;

    for(int i=0; i<4; i++) {
        m_settings.numberTPSChannel[i] = i;
    }

    for(int i=0; i<7; i++) {
        m_settings.numberR18Channel[i] = i;
    }
    for(int i=7; i<14; i++) {
        m_settings.numberR18Channel[i] = i + 1;
    }

    sectors[0].nam = 4;
    sectors[0].namS = 1;
    sectors[0].nSect = 3;
    sectors[0].nSect2 = 6;
    sectors[0].angle = 120;
    sectors[0].angle2 = 300;

    sectors[1].nam = 5;
    sectors[1].namS = 2;
    sectors[1].nSect = 5;
    sectors[1].nSect2 = 2;
    sectors[1].angle = 240;
    sectors[1].angle2 = 60;

    sectors[2].nam = 6;
    sectors[2].namS = 3;
    sectors[2].nSect = 1;
    sectors[2].nSect2 = 4;
    sectors[2].angle = 0;
    sectors[2].angle2 = 180;

    clearData();
}

DataProcessor::~DataProcessor()
{

}

void DataProcessor::runProcessing()
{
    while(m_isRunning)
    {
        if(m_isWorking && m_isReadyData)
        {
            QTime time;
            time.start();

            try {
                if(modeRead == DISK)
                {
                    setReadyBlock();
                }
                else if(modeRead == BOARD)
                {
                    decreaseBlock();
                }

                handler();

                if(m_isWriting)
                {
                    fileWriter();
                }
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

            m_timeRun = time.elapsed();
            m_isReadyData = false;
            emit sigTime(m_timeRun);
        }
        else if(m_isWorking) {
            QThread::msleep(500);
        }
    }

    emit signalFinishProcessing();
}

void DataProcessor::decreaseBlock()
{
    int length = LENGTHARRAY/S;
    int width = WIDTHARRAY;

    for(int j = 0; j < width; j++)
    {
        for(int i = 0; i < length; i++)
        {
            m_cutDataArray[j][i] = pBoardData->dataArray[i*S][j];
        }
    }
}

void DataProcessor::setReadyBlock()
{
    memcpy(m_cutDataArray, pBoardData->fileDataArray, ((LENGTHARRAY/S) * WIDTHARRAY) * sizeof(short int));
}

void DataProcessor::handler()
{
    if(m_typeRadar == TPS_63) {
        handlerTPS63();
    }
    else if(m_typeRadar == R_18) {
        handlerR18();
    }
}

void DataProcessor::detectTarget(short valueTarget)
{
    if(valueTarget > m_settings.valueThreshold[m_settings.target])                                                                        //есть сигнал
    {
        float azimuth = static_cast<float>(m_countAzimuth) * 360/4096;                //новая цель
        float range = m_colDist * 0.15;

//        if(range<200) {
//            return;
//        }

        foreach (Target *target, m_target.bufferRange) {
            if(fabs(target->rang[2] - range) < m_target.D_RANGE) {
                target->value++;
                target->space = 0;

                if(azimuth > target->az[2]) {
                    target->az[2] = azimuth;
                    target->rang[2] = range;
                }
                return;
            }
        }

        Target *target = new Target;
        target->rang[2] = target->rang[1] = target->rang[0] = range;
        target->az[2] = target->az[1] = target->az[0] = azimuth;
        target->value = 1;
        target->space = 0;
        m_target.bufferRange.append(target);
    }
    else {                                                                              //не сигнал
        foreach (Target* target, m_target.bufferRange) {
            target->space++;

            if(target->space > m_target.NUMEMT) {
                target->isTarget = false;

                if((target->value > m_target.numTarg)) {                                //предел длины цели
                    bool isMask = false;                                                //радарная маска

                    for(QList<RadarMask*>::iterator iter = m_listMask->begin(); iter!=m_listMask->end(); iter++) {
                        int lowAz = (*iter)->az[0];
                        int upAz = (*iter)->az[1];
                        float lowRang = (*iter)->rang[0];
                        float upRang = (*iter)->rang[1];

                        if(target->az[0]>lowAz && target->az[0]<upAz && target->rang[0]>lowRang &&
                                target->rang[0]<upRang) {
                            isMask = true;
                            break;
                        }
                    }

                    if(!isMask) {
                        target->isTarget = true;
                        pushDetectTarget(target->az[0], target->rang[0]);
                    }
                }

                target->az[1] = (target->az[0] + target->az[2])/2;
                target->rang[1] = (target->rang[0] + target->rang[2])/2;
                pushTarget(target);
                m_target.bufferRange.removeOne(target);
            }
        }
    }

    m_colDist = (m_colDist + 1) % LENCUR;
}

void DataProcessor::pushTarget(Target *target)
{
//    m_target.buffer.append(target);
    m_target.buffer.push_back(target);
}

void DataProcessor::pushDetectTarget(float az, float rang)
{
    QPair<float, float> *detectTarget = new QPair<float, float>(az, rang);
    m_target.bufferDetected.append(detectTarget);
}

void DataProcessor::sendBlockTarget()
{
    m_timeBlockTarg->restart();
    emit sigBlockTarget(m_target.buffer);
    m_target.buffer.clear();
}

void DataProcessor::sendBlockDetectTarget()
{
    emit sigBlockDetectTarget(m_target.bufferDetected);
    m_target.bufferDetected.clear();
}

void DataProcessor::pushPointToLine(short value, short num)
{
    m_buf[num].push_back(value);

    if(m_buf[num].size() >= m_sizeBuf) {
        short int top = 0;

        for(int i = 0; i < m_buf[num].size(); i++) {
            if(qAbs(m_buf[num][i]) > qAbs(top)) {
                top = m_buf[num][i];
            }
        }

        if(m_indVecLine < LENSH) {
            m_vecLine[num][m_indVecLine] = top;

            if(num == m_numPaintChan) {
                m_indVecLine++;
            }
        }

        m_buf[num].clear();
    }
}

void DataProcessor::clearArrayTargets()
{
    const int T = LENCUR * WIDCUR;

    if(m_arrayTargets.empty()) {
        m_arrayTargets.resize(T);
    }

    for(int i=0; i<T; i++) {
        m_arrayTargets[i] = 0;
    }
}

void DataProcessor::handlerTPS63()
{
    int l = LENGTHARRAY/S;
    int w = WIDTHARRAY;
    int imgLen = LENCUR;
    int imgWid = WIDCUR;
    int n = 0;
    bool isCountN = false;
    m_isCountRPR = true;

    for(int i = 0; i < l; i++)
    {
        for(int j = 0; j < w; j++)
        {
            if(m_cutDataArray[j][i] > m_settings.valueThreshold[j])
            {
                if(m_countN[j] == 0) {                                                 //повтор пороговых значений
                    if(m_width.isCounting[j] && !m_width.isFull[j] && (m_width.count[j][m_width.n[j]] != 0)) {
                        m_width.n[j]++;

                        if(m_width.n[j] == m_width.N) m_width.isFull[j] = true;
                    }

                    m_countExcess[j]++;

                    if(j == m_settings.trigger) {
                        m_countPRF++;
                    }

                    if(j == m_settings.north) {                                         //север ("ARP")
                        recountThreshold();

                        m_timeNorth = m_timerNorth->elapsed() - m_timeNorthCurrent;
                        m_timeNorthCurrent = m_timerNorth->elapsed();

                        if(m_isStart) {
                            m_rotation++;

                            if(m_isProtectCheckSig) {
                                if((m_timeNorth < 2000) && m_isCorrectARP) {
                                    m_isCorrectARP = false;
                                    emit sigSendMessage("<FONT COLOR=RED>Uncorrect the signal ARP");
    //                                    emit sigCorrectARP(false);
                                }
                                else if(!m_isCorrectARP && (m_timeNorth >= 2000)) {
                                    m_isCorrectARP = true;
                                    clearData();
                                    emit sigCorrectARP(true);
                                }
                            }
                        }
                        else {
                            m_isStart = true;
                        }

                        if(!m_isCorrectARP) {
                            continue;
                        }
                        emit sigFoundTarger(true,0,0);

                        m_isTrigger = false;
                        m_isPage = true;
                        m_countAzimuth = 0;

                        emit sigValueSignal(m_maxSignal.interimMax,m_countNoise);

                        m_lastInd = 0;

                        emit sigFoundARP(m_rotation);
                        if(m_isCorrectARP) emit sigTimeRotation(m_timeNorth, m_countExcess[m_settings.azimuth]);
                        emit sigRotationCounting(m_countExcess);
                        nullifyCountingExcesses();
                        updateCountValue();
                    }

                    if(j == m_settings.trigger) {                                      //триггер("trigger")
                        m_colDist = 0;

                        if(m_isPage) {
                            if(m_isTrigger) {
                                m_lastInd = (m_lastInd/imgLen + 1) * imgLen;

                                if(m_lastInd >= imgWid * imgLen) {
                                     m_isPage = false;
                                     m_isTrigger = false;
                                     m_lastInd = 0;
                                }
                            }
                            else {
                                m_isTrigger = true;
                            }

                            m_arrayTargets[m_lastInd] = m_countAzimuth;
//                            m_arrayAzTarg[m_lastInd/imgLen] = m_curentAzimuth;
                        }
                    }

                    if(j == m_settings.azimuth) {                                      //азимут ("ACP")
                        m_countAzimuth++;
                        m_curentAzimuth++;
                    }
                }

                if(j == m_settings.target) isCountN = true;

                m_countN[j]++;                                                         //счетчик пороговых значений

                if((m_width.isCounting[j] == true) && !m_width.isFull[j]) {
                    m_width.count[j][m_width.n[j]]++;
                }
            }
            else {
                if(j == m_settings.target) {
                    if(isCountN) n++;

                    if(n > 10) {
                        m_countN[j] = 0;
                        n = 0;
                        isCountN = false;
                    }
                }
                else {
                    if(m_countN[j] != 0) m_countN[j] = 0;
                }
            }

            if((m_maxSignal.n < m_maxSignal.N) && (m_maxSignal.countMax[j][m_maxSignal.n] < m_cutDataArray[j][i])) {
                m_maxSignal.countMax[j][m_maxSignal.n] = m_cutDataArray[j][i];         //массив с максимальными значениями сигналов
            }

            if(j == m_settings.target) {
                detectTarget(m_cutDataArray[j][i]);                                    //Обнаружение цели
            }

            if(m_isPaint[j] == true && *m_pointModeScreen == GRAPH) {
                pushPointToLine(m_cutDataArray[j][i], j);                              //массив для отрисовки
            }

            if(j == m_settings.target && m_isTrigger) {
                m_lastInd++;

                if(m_lastInd >= imgWid * imgLen) {                                      //Заполнение картинки данных
                     m_isPage = false;
                     m_isTrigger = false;
                     m_lastInd = 0;
                }
                else if(m_lastInd % imgLen == 0) {
                    m_isTrigger = false;
                }

                if(m_isTrigger) {
                    int col = m_lastInd % imgLen;
                    int raw = m_lastInd/imgLen;
                    m_arrayTargets[raw * imgLen + col] = m_cutDataArray[j][i];
                }
            }
        }
    }

    recountTarget();
    m_maxSignal.n++;
    helpCountWidth();

    sendBlockDetectTarget();
    sendBlockTarget();
    emit sigShowImpuls(m_width.midWidth);
    helpClearImpuls();
    emit sigCurrentAzimuth(m_rotation, m_curentAzimuth);
    emit sigCountPack(m_countExcess);
    emit sigCountPRF(m_countPRF);
    emit sigPacketFinish(m_vecLine);
    m_indVecLine = 0;
    m_countPRF = 0;
    m_curentAzimuth = 0;
}

void DataProcessor::handlerR18()
{
    int l = LENGTHARRAY/S;
    int w = WIDTHARRAY;

    countSignalR18();

    for(int i = 0; i < l; i++)
    {
        for(int j = 0; j < w; j++)
        {
            if(m_isPaint[j] == true && *m_pointModeScreen == GRAPH) {
                pushPointToLine(m_cutDataArray[j][i], j);
            }
        }

        for(int k = 0; k < PROC_CHAN; k++) {
            if(m_isPaint[w + k] == true && *m_pointModeScreen == GRAPH) {
                pushPointToLine((int)(m_procesDataArray[k][i]*1000), w + k);
            }
        }
    }

    emit sigPacketFinish(m_vecLine);
    m_indVecLine = 0;
}

void DataProcessor::countSignalR18()
{
    int l = LENGTHARRAY/S;
    const int A = N_ANALOG;
    const int PR = PROC_CHAN;
    int s[A] = { m_settings.numberR18Channel[R1], m_settings.numberR18Channel[S1], m_settings.numberR18Channel[S2], m_settings.numberR18Channel[S3] };
    int sR2 = m_settings.numberR18Channel[R2];
    int maxS[A] = { MINV, MINV, MINV, MINV };
    int minS[A] = { MAXV, MAXV, MAXV, MAXV };

    const int RAN = 3000;

    for(int i=0; i<PROC_CHAN; i++) {
        m_mSin[i].nSin = 0;
        m_mSin[i].top = 0.0;
        m_mSin[i].isRise = true;
        m_mSin[i].range = 0;
    }
    countCorel=0; countCorel2=0;

    int sTest = 10000;
    bool isEmptyData = true;
    for(int i=0; i < sTest; i+=1) {
        if(abs(m_cutDataArray[s[0]][i]) > 500) {
            isEmptyData = false;
        }
    }
    if(isEmptyData) {
//        emit sigSendTopsSin(m_pVecSin, !isEmptyData);
        return;
    }

    int fCor = 40;
    for(int i=0; i < l; i++)
    {
        for(int j=0; j<A; j++) {
            if(j == 0) {
                if(!m_isReduceSigRR18) {
                    m_procesDataArray[j][i] = m_cutDataArray[s[j]][i] - m_cutDataArray[sR2][i];

                    if(m_procesDataArray[j][i] > m_pSin.maxSinP[j] && correlationP(j, i, fCor)) {
                        m_pSin.maxSinP[j] = m_procesDataArray[j][i];
                    }
                    else if(m_procesDataArray[j][i] < m_pSin.minSinP[j] && correlationP(j, i, fCor)) {
                        m_pSin.minSinP[j] = m_procesDataArray[j][i];
                    }
                }
                else {
                    if(m_cutDataArray[s[j]][i] > m_pSin.maxSinP[j] && correlationA(j, i, fCor)) {
                        m_pSin.maxSinP[j] = m_cutDataArray[s[j]][i];
                    }
                    else if(m_cutDataArray[s[j]][i] < m_pSin.minSinP[j] && correlationA(j, i, fCor)) {
                        m_pSin.minSinP[j] = m_cutDataArray[s[j]][i];
                    }
                }
            }
            else {
                if(m_cutDataArray[s[j]][i] > maxS[j] && correlationA(j, i, fCor)) {
                    maxS[j] = m_cutDataArray[s[j]][i];
                }
                else if(m_cutDataArray[s[j]][i] < minS[j] && correlationA(j, i, fCor)) {
                    minS[j] = m_cutDataArray[s[j]][i];
                }
            }
        }
    }

    int lenPer[3] = { 0, 0, 0 };
    bool isLenPer[3] = { false, false, false };

    for(int i=0; i<N_SIN; i++) {
        if(m_pSin.isFind[i]) {
            m_pSin.bufMax[i].push_back(maxS[i+1]);
            m_pSin.bufMin[i].push_back(minS[i+1]);

            int s = m_pSin.bufMax[i].size();
            if(s >= N_MIN_ROT && (m_pSin.bufMax[i][s-1]<m_pSin.bufMax[i][s-2]) &&
                    (m_pSin.bufMax[i][s-2]!=0 && (abs((float)m_pSin.bufMax[i][0])/abs(m_pSin.bufMax[i][s-2]) < m_pSin.DIF))) {
                short dataMax = m_pSin.bufMax[i][s-2];
                short dataMin = m_pSin.bufMin[i][m_pSin.bufMin[i].size()-2];
                m_pSin.bufMax[i].clear();
                m_pSin.bufMax[i].reserve(50);
                m_pSin.bufMax[i].push_back(dataMax);
                m_pSin.bufMin[i].clear();
                m_pSin.bufMin[i].reserve(50);
                m_pSin.bufMin[i].push_back(dataMin);

                if(m_pSin.nMed == 0) {
                    m_pSin.maxSinM1[i] = dataMax;
                    m_pSin.minSinM1[i] = dataMin;
                    m_pSin.nMed++;
                }
                else {
                    m_pSin.maxSinM2[i] = dataMax;
                    m_pSin.minSinM2[i] = dataMin;
                    m_pSin.nMed = 0;
                    m_pSin.maxSinP[i+1] = (m_pSin.maxSinM1[i] + m_pSin.maxSinM2[i])/2;
                    m_pSin.minSinP[i+1] = (m_pSin.minSinM1[i] + m_pSin.minSinM2[i])/2;
                }

                m_mSin[i+1].lenArc = m_mSin[i+1].remain + m_mSin[i+1].listSinReal.size() - m_mSin[i+1].pos;
                m_mSin[i+1].remain = 0;
                m_mSin[i+1].pos = m_mSin[i+1].listSinReal.size();

                m_cSin.len[i] = m_mSin[i+1].lenArc;
                m_cSin.isCountLen[i] = true;

                //1
                /*if((m_cSin.lenArc==0 || ((float)m_mSin[i+1].lenArc/m_cSin.lenArc < 1.3 && (float)m_mSin[i+1].lenArc/m_cSin.lenArc > 0.7))) {
                    m_cSin.countDev = 0;
                    m_cSin.totPer += m_mSin[i+1].lenArc;
                    m_cSin.countPer++;

                    int len = m_cSin.totPer/m_cSin.countPer;
                    if(!(m_cSin.lenArc/len < 1.1 && m_cSin.lenArc/len > 0.9)) {
                        m_cSin.lenArc = len;
                    }

                    if(m_cSin.countPer > 10) {
                        m_cSin.totPer = 0;
                        m_cSin.countPer = 0;
                    }
                }
                else if((m_cSin.memDev==0 || ((float)m_mSin[i+1].lenArc/m_cSin.memDev < 1.5 && (float)m_mSin[i+1].lenArc/m_cSin.memDev > 0.5))) {
                    m_cSin.memDev = m_mSin[i+1].lenArc;
                    m_cSin.countDev++;

                    if(m_cSin.countDev >= 4) {
                        m_cSin.lenArc = m_mSin[i+1].lenArc;
                        m_cSin.totPer = 0;
                        m_cSin.countPer = 0;
                    }
                }

                qDebug()<<i+1<<" - "<<m_mSin[i+1].lenArc<<" Mid: "<<m_cSin.lenArc;*/
                //////
            }
        }
        else {
            m_pSin.bufMax[i].push_back(maxS[i+1]);
            m_pSin.bufMin[i].push_back(minS[i+1]);

            int s = m_pSin.bufMax[i].size();
            if(s >= 5) {
                for(int j=1; j<s-1; j++) {
                    if(m_pSin.bufMax[i][j]>m_pSin.bufMax[i][j-1] && m_pSin.bufMax[i][j]>m_pSin.bufMax[i][j+1]) {
                        short dataMax = m_pSin.bufMax[i][j];
                        short dataMin = m_pSin.bufMin[i][m_pSin.bufMin[i].size()-2];
                        m_pSin.isFind[i] = true;
                        m_mSin[i+1].isFindVertex = true;
                        m_mSin[i+1].remain = 0;

                        if(m_pSin.isFind[0] && m_pSin.isFind[1] && m_pSin.isFind[2]) m_pSin.isAllFind = true;
                        m_pSin.bufMax[i].clear();
                        m_pSin.bufMax[i].reserve(50);
                        m_pSin.bufMin[i].clear();
                        m_pSin.bufMin[i].reserve(50);
                        m_pSin.bufMax[i].push_back(dataMax);
                        m_pSin.bufMin[i].push_back(dataMin);
                        m_pSin.maxSinM1[i] = dataMax;
                        m_pSin.minSinM1[i] = dataMin;
                        m_pSin.nMed++;
                        m_pSin.maxSinP[i+1] = dataMax;
                        m_pSin.minSinP[i+1] = dataMin;
                    }
                }
            }
        }
    }

    //2
    if((m_cSin.isCountLen[0] && m_cSin.isCountLen[1] && m_cSin.isCountLen[2]) || m_cSin.isStart) {
        if(!(!m_cSin.isCountLen[0] && !m_cSin.isCountLen[1] && !m_cSin.isCountLen[2])) {
            int choseLen = m_cSin.isCountLen[0] ? m_cSin.len[0] : m_cSin.len[1];
            choseLen = m_cSin.isCountLen[1] ? choseLen : m_cSin.len[2];
            if(choseLen == 0) return;

            for(int i=0; i<3; i++) {
                if(m_cSin.len[i] < choseLen && m_cSin.isCountLen[i]) {
                    choseLen = m_cSin.len[i];
                }
                m_cSin.isCountLen[i] = false;
            }

            m_cSin.totPer += choseLen;
            m_cSin.countPer++;

            int len = m_cSin.totPer/m_cSin.countPer;
            if(!(m_cSin.lenArc/len < 1.1 && m_cSin.lenArc/len > 0.9)) {
                m_cSin.lenArc = len;
            }
//            qDebug()<<" Mid: "<<m_cSin.lenArc;

            if(m_cSin.countPer > 10) {
                m_cSin.totPer = 0;
                m_cSin.countPer = 0;
            }

            if((m_cSin.isCountLen[0] && m_cSin.isCountLen[1] && m_cSin.isCountLen[2]) && m_cSin.isStart) m_cSin.isStart = false;
        }
    }
    /////

    int ampl[PROC_CHAN] = { 0, 0, 0, 0 };
    int offsetNul[A];
    float fact[PROC_CHAN];

    for(int i=0; i<A; i++) {
        if(m_pSin.maxSinP[i] >= 0 && m_pSin.minSinP[i] < 0) {
            ampl[i] = m_pSin.maxSinP[i] + abs(m_pSin.minSinP[i]);
        }
        else if(m_pSin.maxSinP[i] > 0 && m_pSin.minSinP[i] >= 0) {
            ampl[i] = m_pSin.maxSinP[i] - m_pSin.minSinP[i];
        }
        else if(m_pSin.maxSinP[i] <= 0 && m_pSin.minSinP[i] < 0) {
            ampl[i] = abs(m_pSin.minSinP[i] - m_pSin.maxSinP[i]);
        }

        offsetNul[i] = m_pSin.maxSinP[i] - ampl[i]/2;
        fact[i] = ampl[i]/2;
    }

    int stAngle = m_countAngleR18;
    int stSect = m_sectorAngle;
    bool stTotSinch[3] = { m_mSin[4].totSinch, m_mSin[5].totSinch, m_mSin[6].totSinch };

    float f = 1.765;

    for(int i=0; i<l; i++) {
        for(int j=0; j<A; j++) {
            if(!m_isReduceSigRR18) {
                if(j == 0) {
                    m_procesDataArray[j][i] = (m_procesDataArray[j][i] - offsetNul[j])/fact[j];
                }
                else {
                    m_procesDataArray[j][i] = (m_cutDataArray[s[j]][i] - offsetNul[j])/fact[j];
                }
            }
            else {
                m_procesDataArray[j][i] = (m_cutDataArray[s[j]][i] - offsetNul[j])/fact[j];
            }
        }

        m_procesDataArray[4][i] = (m_procesDataArray[3][i] - m_procesDataArray[2][i])/f;
        m_procesDataArray[5][i] = (m_procesDataArray[2][i] - m_procesDataArray[1][i])/f;
        m_procesDataArray[6][i] = (m_procesDataArray[1][i] - m_procesDataArray[3][i])/f;

        double fd = 0.001;

        for(int j=0; j<PR; j++) {
            if(m_mSin[j].isRise) {
                if(m_mSin[j].top < m_procesDataArray[j][i] && correlationP(j, i, fd)) {
                    m_mSin[j].top = m_procesDataArray[j][i];
                    m_mSin[j].range = 0;
                }
                else {
                    m_mSin[j].range++;

                    if(m_mSin[j].range > RAN) {
                        if(m_mSin[j].nSin >= m_mSin[j].bufSin.size() || m_mSin[j].nSin < 0)
                            throw ExeptionRange("Out or range: ", m_mSin[j].nSin, "DataProcessor::countSignalR18() - m_msin[j].bufSin");
                        m_mSin[j].bufSin[m_mSin[j].nSin].tMax = move(m_mSin[j].top);
                        m_mSin[j].bufSin[m_mSin[j].nSin].pMax = i;
                        m_mSin[j].isRise = false;
                        m_mSin[j].range = 0;
                        m_mSin[j].bufSin[m_mSin[j].nSin].sinch = (m_procesDataArray[0][i] > 0) ? true : false;
                        if(m_mSin[j].bufSin[m_mSin[j].nSin].sinch != m_mSin[j].totSinch && m_mSin[j].nSin != 0) {
                            m_mSin[j].nSinch++;
//                            float val1 = m_msin[j].bufSin[m_msin[j].nSin].sinch ? m_msin[j].bufSin[m_msin[j].nSin].tMax : m_msin[j].bufSin[m_msin[j].nSin].tMin;
//                            float val2 = m_msin[j].bufSin[m_msin[j].nSin-1].sinch ? m_msin[j].bufSin[m_msin[j].nSin-1].tMax : m_msin[j].bufSin[m_msin[j].nSin-1].tMin;
                            if(m_mSin[j].nSinch >= 10 /*&& fabs(val1-val2) < 0.2*/) {
                                m_mSin[j].totSinch = m_mSin[j].bufSin[m_mSin[j].nSin].sinch;
                                m_mSin[j].nSinch = 0;

                                bool isS = !m_isReduceSigSR18 ? (j>=A) : (j>=1 && j<A);
                                int c = !m_isReduceSigSR18 ? 4 : 1;
                                int c2 = !m_isReduceSigSR18 ? 0 : 3;

                                if(isS) {
                                    if(m_mSin[j].totSinch == true) {
                                        m_countAngleR18 = sectors[j-c].angle;
                                        m_sectorAngle = sectors[j-c].nSect;
                                    }
                                    else {
                                        m_countAngleR18 = sectors[j-c].angle2;
                                        m_sectorAngle = sectors[j-c].nSect2;
                                    }

                                    if(j==(6-c2) && m_pSin.isAllFind && m_mSin[j].totSinch == true) {
                                        if(m_rotation!=0) {
                                            int n = 0;
                                            for(int i=0; i<PROC_CHAN; i++) {
                                                if(m_mSin[i].listSinReg.size() > n) n = m_mSin[i].listSinReg.size();
                                            }
                                            sigSetSizeSin(n);
                                        }

                                        emit sigFoundARP(m_rotation);
                                        m_rotation++;

                                        for(int k=0; k<PROC_CHAN; k++) {
                                            m_mSin[k].remain = m_mSin[k].listSinReal.size() - m_mSin[k].pos;
                                            m_mSin[k].pos = 0;
                                            m_mSin[k].listSinReg.clear();
                                            m_mSin[k].listSinReal.clear();
                                        }
                                    }
                                }
                            }
                        }
                        else {
                            m_mSin[j].nSinch = 0;
                        }
                        m_mSin[j].nSin++;

                        if(m_mSin[j].nSin >= m_mSin[j].bufSin.size()) {
                            if(m_pSin.isAllFind) {
                                MicroSinusoida sin;
                                m_mSin[j].bufSin.push_back(sin);
                            }
                            else {
                                m_mSin[j].nSin--;
                            }
                        }
                    }
                }
            }
            else {
                if(m_procesDataArray[j][i] < m_mSin[j].top && correlationP(j, i, fd)) {
                    m_mSin[j].top = m_procesDataArray[j][i];
                    m_mSin[j].range = 0;
                }
                else {
                    m_mSin[j].range++;

                    if(m_mSin[j].range > RAN) {
                        if(m_mSin[j].nSin >= m_mSin[j].bufSin.size() || m_mSin[j].nSin < 0)
                            throw ExeptionRange("Out or range: ", m_mSin[j].nSin, "DataProcessor::countSignalR18() - m_msin[j].bufSin");
                        m_mSin[j].bufSin[m_mSin[j].nSin].tMin = move(m_mSin[j].top);
                        m_mSin[j].bufSin[m_mSin[j].nSin].pMin = i;
                        m_mSin[j].isRise = true;
                        m_mSin[j].range = 0;
                    }
                }
            }
        }
    }

    correlationT();

    /*for(int i=1; i<PROC_CHAN; i++) {
        for(int j=0, s=m_mSin[i].listSinReg.size()-1; j<s; j++) {
            bool isS = !m_isReduceSigSR18 ? (j>=A) : (j>=1 && j<A);
            int c = !m_isReduceSigSR18 ? 4 : 1;
            int c2 = !m_isReduceSigSR18 ? 0 : 3;

            if(isS) {
                if(m_mSin[i].listSinReg[j]<=0 && m_mSin[i].listSinReg[j+1]>0) {
                    m_countAngleR18 = sectors[j-c].angle;
                    m_sectorAngle = sectors[j-c].nSect;

                    if(j==(6-c2) && m_pSin.isAllFind && m_mSin[j].totSinch == true) {
                        if(m_rotation!=0) {
                            int n = 0;
                            for(int i=0; i<PROC_CHAN; i++) {
                                if(m_mSin[i].listSinReg.size() > n) n = m_mSin[i].listSinReg.size();
                            }
                            sigSetSizeSin(n);
                        }

                        emit sigFoundARP(m_rotation);
                        m_rotation++;

                        for(int k=0; k<PROC_CHAN; k++) {
                            m_mSin[k].remain = m_mSin[k].listSinReal.size() - m_mSin[k].pos;
                            m_mSin[k].pos = 0;
                            m_mSin[k].listSinReg.clear();
                            m_mSin[k].listSinReal.clear();
                            qDebug()<<"find";
                        }
                    }
                    break;
                }
                else if(m_mSin[i].listSinReg[j]>=0 && m_mSin[i].listSinReg[j+1]<0){
                    m_countAngleR18 = sectors[j-c].angle2;
                    m_sectorAngle = sectors[j-c].nSect2;
                    break;
                }
            }
        }
    }*/

//    if(m_countAngleR18 == 300 && stAngle == 0) {
//        m_countAngleR18 = stAngle;
//        m_sectorAngle = stSect;
//        for(int i=0; i<3; i++) {
//            m_mSin[i+4].totSinch = stTotSinch[i];
//        }
//    }

    int sectRed = m_sectorAngle < 4 ? m_sectorAngle : m_sectorAngle - 3;
    sectRed = sectRed < 4 && sectRed > 0 ? sectRed : 1;
    int nSig = !m_isReduceSigSR18 ? 4 - sectRed + 3 : 4 - sectRed;
    float totAngle = m_countAngleR18/* + asin(fabs(m_mSin[nSig].listSin[m_mSin[nSig].listSin.size()-1])) * 180/P*/;

    bool isSinch[N_SIN] = { m_mSin[1].totSinch, m_mSin[2].totSinch, m_mSin[3].totSinch };

//    const int NA = 60;
//    float arr[NA];
//    for(int i=0; i<NA; i++) {
//        arr[i] = 0;
//    }
//    int t = 1;
//    int sV = m_msin[t].bufSin.size();
//    if(sV > NA || sV < 0) throw ExeptionRange("Out or range the vector 'testOut' ", sV, NA);
//    for(int i=0; i<sV; i++) {
//        arr[i] = m_msin[t].bufSin[i].tMax;
//    }

    emit sigSendCountAngle(totAngle);
    emit sigSetSinchData(isSinch);
//    emit sigSendAngles(m_vecAngle);
//    emit sigSendTopsSin(m_pVecSin, !isEmptyData);
    m_pSin.nBlock++;
//    qDebug()<<countCorel<<" "<<countCorel2<<" "<<m_pSin.nBlock;
}

bool DataProcessor::correlationA(int nArr, int i, float f)
{
    if(i<=0) return false;

    return !(abs(m_cutDataArray[nArr][i]-m_cutDataArray[nArr][i-1]) > f &&
            abs(m_cutDataArray[nArr][i]-m_cutDataArray[nArr][i+1]) > f);
}

bool DataProcessor::correlationP(int nArr, int i, double f)
{
    int s = LENGTHARRAY/S;
    bool is = true;

    if(i >= 1 && (s-i) >= 2) {

        if(fabs(m_procesDataArray[nArr][i]-m_procesDataArray[nArr][i-1]) > f &&
                fabs(m_procesDataArray[nArr][i]-m_procesDataArray[nArr][i+1]) > f) {
            is = false;
        }
    }
    else {
        is = false;
    }

    return is;
}

void DataProcessor::correlationT()
{
    //Debug array 'listSin'
    const int ARR = 50; float arr[ARR]; for(int i=0; i<ARR; i++) arr[i] = 10;
    const int ARR2 = 50; float arr2[ARR2]; for(int i=0; i<ARR2; i++) arr2[i] = 10;
    int sig = 1; int arrX=0;
    ///////

    if(!m_mSin[1].isFindVertex) return;

    float f = 0.05;
    PointSin point;
    m_vecPoint.clear();
    bool cond[PROC_CHAN];
    int l[PROC_CHAN];
    int countL[PROC_CHAN];
    float average[PROC_CHAN];
    l[0] = 0;
    countL[0] = 0;
    cond[0] = false;
//    qDebug()<<m_mSin[1].nSin<<" "<<m_mSin[2].nSin<<" "<<m_mSin[3].nSin;

    for(int i=0; i<4; i++) {
        cond[i] = false;
        l[i] = 0;
        countL[i] = 0;
        average[i] = 0;

        int s = m_mSin[i].nSin;
//        s = s
        m_mSin[i].listSin.resize(s);

        if(!m_mSin[i].isFindVertex) {
            for(int j=0; j<s; j++) {
                m_mSin[i].listSin[j] = 0;
            }
        }
        else {
            for(int j=0; j<s; j++) {
                m_mSin[i].listSin[j] = m_mSin[i].totSinch ? m_mSin[i].bufSin[j].tMax : m_mSin[i].bufSin[j].tMin;
            }
        }

        m_mSin[i].listSinReal.reserve(m_mSin[i].listSinReal.size() + s);
        vector<float>::iterator itEnd = m_mSin[i].listSin.begin() + m_mSin[i].nSin;
        m_mSin[i].listSinReal.insert(m_mSin[i].listSinReal.end(), m_mSin[i].listSin.begin(), itEnd);

        for(int j=0; j<s; j++) {
            if(cond[i] == true && (j-l[i]) > 5) {
                cond[i] = false;
                l[i] = 0;
            }

            float lastVal = j!=0 ? (cond[i] ? (l[i]>0 ? m_mSin[i].listSin[l[i]-1] : m_mSin[i].memVal) : m_mSin[i].listSin[j-1]) : m_mSin[i].memVal;

            if(fabs(m_mSin[i].listSin[j]-lastVal) > f) {
                if(cond[i] == false) {
                    point.x = j; point.y = m_mSin[i].listSin[j]; point.chan = i; point.wron = true; m_vecPoint.push_back(point);
                    cond[i] = true;
                    l[i] = j;
                }
            }
            else if(cond[i] && fabs(m_mSin[i].listSin[j])>0.1) {
                int lon = j - l[i];

                if(lon < 4 && lon > 0) {
                    for(int k=l[i]; k<j; k++) {
                        float ageVal = l[i] > 0 ? m_mSin[i].listSin[l[i]-1] : m_mSin[i].memVal;
                        m_mSin[i].listSin[k] = (ageVal + m_mSin[i].listSin[j])/2;
                        point.x = k; point.y = m_mSin[i].listSin[k]; point.chan = i; point.wron = false; m_vecPoint.push_back(point);
                    }
                }

                cond[i] = false;
            }
            average[i] += fabs(m_mSin[i].listSin[j]);
        }

        //Debug array 'listSin'
        if(i == sig) {
            int x=0, x2=0;
            for(int j=0, s=m_mSin[sig].bufSin.size(); j<s; j++) { arr[x++] = m_mSin[sig].bufSin[j].tMax; x = x<ARR-1 ? x : 0; }
            foreach (float dig, m_mSin[sig].listSin) { arr2[x++] = dig; x = x<ARR2 ? x : 0; }
//            foreach (float dig, m_mSin[sig].listSinReg) { arr2[x2++]=dig; x2=x2<ARR2?x2 : 0;}

            if(m_pSin.nBlock >= 6) {
                sig = sig;
            }
        }
        /////////

        if(s > 0) m_mSin[i].memVal = m_mSin[i].listSin[s-1];

        if(i>=1 && i<4) {
            //Sinusoid regression
            int sVecReg = m_mSin[i].listSinReg.size();
            if(sVecReg == 0) {
//            m_cSin.lenArc = 480;
//            float per = m_cSin.lenArc * 2;
                float per = 475 * 2;
                switch (i) {
                case 1:
                    per = 480 * 2; break;
                case 2:
                    per = 550 * 2; break;
                case 3:
                    per = 470 * 2; break;
                default:
                    break;
                }
//            float per = 550 * 2;
                per = per != 0 ? per : 1;
                m_cSin.w[i-1] = m_cSin.N/per;
                m_cSin.t[i-1] = 0;

                float z;
                QVector<float> vecZ;
                for(float d=0; d<360; d++) {
                    z = 0;
                    for(int j=0; j<s; j++) {
                        float xGr = 360/(float)m_cSin.N * (j+sVecReg);
                        float y = sin((m_cSin.w[i-1] * xGr + d) * P/180);
                        z += (m_mSin[i].listSin[j] - y) * (m_mSin[i].listSin[j] - y);
                    }

                    vecZ.push_back(z);
                }

                z = 1000000;
                int count = 0;
                int deg = 0;
                foreach (float deviation, vecZ) {
                    if(deviation < z) {
                        z = deviation;
                        deg = count;
                    }

                    count++;
                }

                m_cSin.t[i-1] = deg;
            }

            for(int j=0; j<s; j++) {
                float xGr = 360/(float)m_cSin.N * (j+sVecReg);
                m_mSin[i].listSin[j] = sin((m_cSin.w[i-1] * xGr + m_cSin.t[i-1]) * P/180);
            }
            //////
        }

        m_mSin[i].lastReg = m_mSin[i].listSinReg.size();
        m_mSin[i].listSinReg.reserve(m_mSin[i].listSinReg.size() + s);
        m_mSin[i].listSinReg.insert(m_mSin[i].listSinReg.end(), m_mSin[i].listSin.begin(), itEnd);
    }

    /*float fp = 1.765;
    int s[3] = { m_mSin[1].listSin.size(), m_mSin[2].listSin.size(), m_mSin[3].listSin.size() };

    for(int i=4; i<PROC_CHAN; i++) {
        m_mSin[i].listSin.resize(s);
    }

    int ms = s[0];
    int x1 = 0, x2 = 0;
    ms = ms >= s[1] ? ms : s[1];
    ms = ms >= s[2] ? ms : s[2];

    for(int i=0; i<ms; i++) {
        m_mSin[i].listSin.resize(s);
        m_mSin[4].listSin[i] = (m_mSin[3].listSin[i] - m_mSin[2].listSin[i])/fp;
        m_mSin[5].listSin[i] = (m_mSin[2].listSin[i] - m_mSin[1].listSin[i])/fp;
        m_mSin[6].listSin[i] = (m_mSin[1].listSin[i] - m_mSin[3].listSin[i])/fp;
    }

    for(int i=4; i<PROC_CHAN; i++) {
        m_mSin[i].lastReg = m_mSin[i].listSinReg.size();
        m_mSin[i].listSinReg.reserve(m_mSin[i].listSinReg.size() + m_mSin[i].listSin.size());
        m_mSin[i].listSinReg.insert(m_mSin[i].listSinReg.end(), m_mSin[i].listSin.begin(), itEnd);
    }*/

    //Debug array 'listSin'
    int x = 0;
    foreach (float dig, m_mSin[sig].listSin) { arr2[x++] = dig; x = x<ARR2 ? x : 0; }
    //////

//    emit sigSendPoints(m_vecPoint);
}

void DataProcessor::showDinamicArray(float *array, int s)
{
//    const int NA = (const int)s;
//    float arr[NA];
//    for(int i=0; i<NA; i++) {
//        arr[i] = 500;
//    }
//    int t = 1;
//    int siz = 0;
}

void DataProcessor::updateThreshold()
{
    for(int i = 0; i < 8; i++) {
        m_settings.valueThreshold[i] = 4000;
    }
}

void DataProcessor::updateCountValue()
{
    for(int i = 0; i < 8; i++) {
        for(int j = 0; j < m_maxSignal.N; j++) {
            m_maxSignal.countMax[i][j] = 0;
        }

        m_maxSignal.interimMax[i] = 0;
    }

    m_maxSignal.n = 0;
}

void DataProcessor::recountThreshold()
{
    int sum;
    bool isNeedSend = false;

    if(m_maxSignal.n > m_maxSignal.N) m_maxSignal.n = m_maxSignal.N;

    for(int i = 0; i < 8; i++) {
        sum = 0;

        for(int j = 0; j < m_maxSignal.n; j++) {
            if(m_maxSignal.interimMax[i] < m_maxSignal.countMax[i][j]) {
                m_maxSignal.interimMax[i] = m_maxSignal.countMax[i][j];
            }
        }

        if((m_maxSignal.constantMax[i] < m_maxSignal.interimMax[i] * m_maxSignal.factor1) ||
                (m_maxSignal.constantMax[i] > m_maxSignal.interimMax[i]/m_maxSignal.factor2)) {
            m_maxSignal.constantMax[i] = m_maxSignal.interimMax[i];
            m_maxSignal.isRecount[i] = true;
        }
    }

    if((autoValue[0] == true) && m_maxSignal.isRecount[m_settings.azimuth]) {
        m_settings.valueThreshold[m_settings.azimuth] = m_maxSignal.constantMax[m_settings.azimuth] * m_settings.filter[ACP]/100;
        m_maxSignal.isRecount[m_settings.azimuth] = false;
        isNeedSend = true;
    }
    if((autoValue[1] == true) && m_isStart && m_maxSignal.isRecount[m_settings.north]) {
        m_settings.valueThreshold[m_settings.north] = m_maxSignal.constantMax[m_settings.north] * m_settings.filter[ARP]/100;
        m_maxSignal.isRecount[m_settings.north] = false;
        isNeedSend = true;
    }
    if((autoValue[3] == true) && m_maxSignal.isRecount[m_settings.trigger]) {
        m_settings.valueThreshold[m_settings.trigger] = m_maxSignal.constantMax[m_settings.trigger] * m_settings.filter[TRIGGER]/100;
        m_maxSignal.isRecount[m_settings.trigger] = false;
        isNeedSend = true;
    }

    if(isNeedSend) {
        emit sigSetChanges();
    }
}

void DataProcessor::recountTarget()
{
    if((autoValue[2] == true) && m_maxSignal.isRecount[m_settings.target]) {
        m_settings.valueThreshold[m_settings.target] = m_maxSignal.constantMax[m_settings.target] * m_settings.filter[VIDEO]/100;
        m_maxSignal.isRecount[m_settings.target] = false;
        emit sigSetChanges();
    }
}

void DataProcessor::helpCountWidth()
{
    for(int i = 0; i < 8; i++) {
        if(m_width.isCounting[i] == true) {
            int sum = 0;

            for(int j = 0; j < m_width.n[i]; j++) {
                sum += m_width.count[i][j];
            }

            int n = m_width.n[i] != 0 ? m_width.n[i] : 1;
            m_width.midWidth[i] = sum/n;
        }
    }
}

void DataProcessor::helpClearImpuls()
{
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < m_width.N; j++) {
            m_width.count[i][j] = 0;
        }

        m_width.midWidth[i] = 0;
        m_width.isCounting[i] = false;
        m_width.isFull[i] = false;
        m_width.n[i] = 0;
    }

    m_width.isCounting[m_settings.azimuth] = true;
    m_width.isCounting[m_settings.north] = true;
    m_width.isCounting[m_settings.target] = true;
    m_width.isCounting[m_settings.trigger] = true;
}

int DataProcessor::getTimeRun() const
{
    return m_timeRun;
}

int DataProcessor::getTimeClear() const
{
    return m_timeClear;
}

QTime *DataProcessor::getTimeBlockTarg()
{
    return m_timeBlockTarg;
}

void DataProcessor::setRunning(bool running)
{
//    m_isRunning = running;
    m_isWorking = running;
}

void DataProcessor::setDisplay(bool display)
{
    m_isdisplay = display;
}

void DataProcessor::setWriting(bool writing)
{
    m_isWriting = writing;
}

void DataProcessor::setValueThresholds(int channels[])
{
    for(int i = 0; i < 8; i++)
    {
        m_settings.valueThreshold[i] = channels[i];
    }
}

void DataProcessor::nullifyCountingExcesses()
{
    for(int i = 0; i < 8; i++)
    {
        m_countExcess[i] = 0;
    }
}

void DataProcessor::setIsReadyData(bool ready)
{
    m_isReadyData = ready;
}

void DataProcessor::setBoardData(BoardData *data)
{
    pBoardData = data;
}

void DataProcessor::setMode(ModeReading mode)
{
    modeRead = mode;
}

const QVector<QVector<short int>> *DataProcessor::getVecLine() const
{
    return &m_vecLine;
}

const QVector<QVector<short> > *DataProcessor::getVecLineR18() const
{
    return &m_vecLineR18;
}

vector<float> *DataProcessor::getVecSin(int nVec)
{
    if(nVec<0 || nVec>=PROC_CHAN) return nullptr;
    return &m_mSin[nVec].listSinReg;
}

vector<float> *DataProcessor::getVecSinReal(int nVec)
{
    if(nVec<0 || nVec>=PROC_CHAN) return nullptr;
    return &m_mSin[nVec].listSinReal;
}

SettingsChannel *DataProcessor::getSettingsChannel()
{
    return &m_settings;
}

const int *DataProcessor::getNumbersTPSChannels()
{
    m_settings.numberTPSChannel[0] = m_settings.azimuth;
    m_settings.numberTPSChannel[1] = m_settings.north;
    m_settings.numberTPSChannel[2] = m_settings.target;
    m_settings.numberTPSChannel[3] = m_settings.trigger;

    return m_settings.numberTPSChannel;
}

const int *DataProcessor::getNumbersR18Channels()
{
    return m_settings.numberR18Channel;
}

void DataProcessor::setPaintChan(int num, bool isPaint)
{
    if(num >= CHAN + PROC_CHAN) {
        return;
    }

    m_isPaint[num] = isPaint;

    if(isPaint) {
        if(num > m_numPaintChan) {
            m_numPaintChan = num;
        }
    }
    else {
        m_numPaintChan = 0;

        for(int i = 0; i < CHAN + PROC_CHAN; i++) {
            if(m_isPaint[i]) {
                m_numPaintChan = i;
            }
        }
    }
}

void DataProcessor::setPath(QString path)
{
//    if(m_fileIn->isOpen()) {
//        m_fileIn->close();
//    }

    m_path = path;
//    m_fileIn->setFileName(m_path);
//    startFile();
}

bool DataProcessor::startFile()
{
    if(!m_fileIn->isOpen())
    {
        if(!m_fileIn->open(QIODevice::WriteOnly|QIODevice::Append))
        {
            emit sigSendMessage("<FONT COLOR=RED>Fail opening file: " + m_path);
            return false;
        }
    }

    return true;
}

const int *DataProcessor::getThresholdChannel()
{
    return m_settings.valueThreshold;
}

const QMap<int, QString> *DataProcessor::getNameChannel()
{
    return &m_settings.nameChannel;
}

void DataProcessor::updataCycle()
{
    m_rotation = 0;
    m_isStart = false;

    updateCountValue();

    emit sigSetChanges();
}

bool *DataProcessor::getIsPaint()
{
    return m_isPaint;
}

void DataProcessor::setRecountTarget()
{
    m_maxSignal.isRecount[m_settings.target] = true;
}

//void DataProcessor::setScaleImg(int length)
//{
//    m_sizeLine = length;
//    m_sizeBuf = LENGTHARRAY/S/m_sizeLine;
//}

void DataProcessor::setNumTarg(int num)
{
    m_target.numTarg = num;
}

void DataProcessor::setModeScreen(ModeScreen *mode)
{
    m_pointModeScreen = mode;
}

void DataProcessor::clearData()
{
    m_countAzimuth = 0;
    m_isReadyData = false;
    m_lastInd = 0;
    m_colDist = 0;
    m_isPage = false;
    m_isTrigger = false;
    m_indVecLine = 0;
    m_countPRF = 0;
    m_timeNorth = 0;
    m_timeNorthCurrent = 0;
    m_curentAzimuth = 0;
    m_isCorrectARP = true;
    m_countAngleR18 = 0.0;
    m_sectorAngle = 0;

    for(int i=0; i<PROC_CHAN; i++) {
        m_mSin[i].bufSin.clear();
        m_mSin[i].bufSin.resize(50);
        m_mSin[i].listSin.clear();
        m_mSin[i].listSin.resize(50);
        m_mSin[i].totSinch = false;
        m_mSin[i].nSinch = 0;
        m_mSin[i].memVal = 0;
    }

    for(int i=0; i<N_ANALOG; i++) {
        m_pSin.maxSinP[i] = MINV;
        m_pSin.minSinP[i] = MAXV;
    }
    for(int i=0; i<N_SIN; i++) {
        m_pSin.maxSinM1[i] = MINV;
        m_pSin.minSinM1[i] = MAXV;
        m_pSin.maxSinM2[i] = MINV;
        m_pSin.minSinM2[i] = MAXV;
        m_pSin.bufMax[i].clear();
        m_pSin.bufMin[i].clear();
        m_pSin.isFind[i] = false;
        m_mSin[i+4].isFindVertex = true;
    }
    m_pSin.isAllFind = false;
    m_pSin.nBlock = 0;
    m_pSin.nMed = 0;

    for(int i = 0; i < 8; i++) {
        m_countN[i] = 0;
        m_maxSignal.constantMax[i] = 0;
        m_countNoise[i] = 0;
    }

    int length = LENGTHARRAY/S;
    int width = WIDTHARRAY;

    for(int j = 0; j < width; j++)
    {
        for(int i = 0; i < length; i++)
        {
            m_cutDataArray[j][i] = 0;
        }
    }
    for(int j = 0; j < PROC_CHAN; j++)
    {
        for(int i = 0; i < length; i++)
        {
            m_procesDataArray[j][i] = 0;
        }
    }

    updataCycle();
    updateThreshold();
    nullifyCountingExcesses();
    helpClearImpuls();
    clearArrayTargets();

    foreach (Target *target, m_target.bufferRange) {
        delete target;
    }
    m_target.bufferRange.clear();
}

void DataProcessor::setListMask(QList<RadarMask *> &list)
{
    m_listMask = &list;
}

void DataProcessor::setStartAzimuth(int angle)
{
    if(angle < 0 || angle > 360) {
        return;
    }

    m_startAngle = angle;
}

vector<short> *DataProcessor::getArrayTargets()
{
    return &m_arrayTargets;
}

void DataProcessor::setIsProtectCorrectARP(bool protecting)
{
    m_isProtectCheckSig = protecting;
}

void DataProcessor::setTypeRadar(TypeRadar typeRadar)
{
    m_typeRadar = typeRadar;
}

void DataProcessor::setIsReduceSigSR18(bool reduce)
{
    m_isReduceSigSR18 = reduce;
}

void DataProcessor::setIsReduceSigRR18(bool reduce)
{
    m_isReduceSigRR18 = reduce;
}

void DataProcessor::setChannelsR18(int channels[7])
{
    for(int i=0; i<7; i++) {
        m_settings.numberR18Channel[i] = channels[i];
    }
}

void DataProcessor::fileWriter()
{
    QFile fileIn(m_path);

    if(!fileIn.open(QIODevice::WriteOnly|QIODevice::Append))
    {
        throw ExeptionRange("Fail opening file: ", fileIn.fileName(), "DataProcessor::fileWriter()");
//        emit sigSendMessage("<FONT COLOR=RED>Fail opening file: " + m_path);
        return;
    }

    fileIn.write(reinterpret_cast<const char *>(m_cutDataArray),
                  ((LENGTHARRAY/S)*WIDTHARRAY)*sizeof(short int));
}


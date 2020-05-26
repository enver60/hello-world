﻿#include "HikvisionLogin.h"
#include "STh.h"
#include "mainResources.h"	
#include "externFunctions.h"
#include "externData.h"
#include "WebformWorker.h"
#include "Connector.h"
#include "BasicAuth.h"
#include "FTPAuth.h"
#include "SSHAuth.h"
#include <memory>
#include "FileUpdater.h"
#include "IPCAuth.h"
#include <qjsonobject.h>
#include "RTSP.h"
#include "MainStarter.h"

#ifndef WIN32
	#define _cdecl
	#include <errno.h>
	int GetLastError(){
		return errno;
	}
#endif


extern "C"
{
	void _cdecl __vcrt_initialize_telemetry_provider() {}
	void _cdecl __telemetry_main_invoke_trigger() {}
	void _cdecl __telemetry_main_return_trigger() {}
	void _cdecl __vcrt_uninitialize_telemetry_provider() {}
};

unsigned char tl(unsigned char d)
{
	if (d >= 192 && d <= 223)
	{
		return (unsigned char)(d + 32);
	}
	else
	{
		return tolower(d);
	};
}

std::string toLowerStr(const char *str)
{
	if (str != NULL) {
		int tsz = strlen(str);
		char *strr = new char[tsz + 1];
		//ZeroMemory(strr, tsz);
		strr[0] = 0;

		for (int i = 0; i < tsz; i++)
		{
			strr[i] = tl(str[i]);
		};

		memset(strr + tsz, '\0', 1);

		std::string tstr = std::string(strr);
		delete[]strr;
		return tstr;
	}
	else return "";
}

char* strstri(const char *_Str, const char *_SubStr)
{
	if(_Str != NULL)
	{
        const std::string &_lowStr = toLowerStr(_Str);
        const std::string &_lowSubStr = toLowerStr(_SubStr);
		const char *resChar = strstr(_lowStr.c_str(), _lowSubStr.c_str());
        if(resChar == 0) return NULL;
        else {
            return (char*)(_Str + (resChar - _lowStr.c_str()));
        }
	};
    return 0;
}

bool gGlobalTrackLocked = false;
char *_findFirst(const char *str, char *delim)
{
	int sz = strlen(str);
	int dsz = strlen(delim);
	for(int i = 0; i < sz; ++i)
	{
		for(int j = 0; j < dsz; ++j)
		{
			if(str[i] == delim[j]) return (char *)(str + i);
		};
	};

	return NULL;
}

char *_findLast(char *str, char *delim)
{
	int sz = strlen(str);
	int dsz = strlen(delim);
	int savedPosition = 0;
	for(int i = 0; i < sz; ++i)
	{
		for(int j = 0; j < dsz; ++j)
		{
			if(str[i] == delim[j]) savedPosition = i;
		};
	};

	return (char *)(str + savedPosition);
}

char *getCodePage(const char *str)
{
	char cdpg[32] = {0};
	char *ptr1 = strstri(str, "charset=");

	if (ptr1 != NULL)
	{
		char *temp3 = _findFirst((char *)(ptr1 + 8), " \"'\n\r");
		if (temp3 != NULL)
		{
			int ln = (int)(temp3 - ptr1 - 8);
			if (ln > 16) return "WTF?";
			strncpy(cdpg, (char *)(ptr1 + 8), (ln > 32) ? 32 : ln);
			if (strstri(cdpg, "%s") != NULL) return "UTF-8";
			return cdpg;
		}
		else
		{
			stt->doEmitionRedFoundData("[GetCodePage] [" + QString(temp3).mid(0, 16) + "]");
			return "NULL";
		};
	}

	ptr1 = strstri(str, "<meta ");
    if(ptr1 != NULL)
    {
        char *ptr2 = strstri(ptr1 + 6, "charset=");
        if(ptr2 != NULL)
        {
            char *temp4 = _findFirst((char *)(ptr2 + 6), " \"'>\n\r");
			if(temp4 != NULL)
			{
                int ln = (int)(temp4 - ptr2 - 8);
				if(ln > 16) return "WTF?";
                strncpy(cdpg, (char *)(ptr2 + 8), (ln > 32) ? 32 : ln );
				if(strstri(cdpg, "%s") != NULL) return "UTF-8";
				return cdpg;
			}
			else
			{
                stt->doEmitionRedFoundData("[GetCodePage] [" + QString(ptr2).mid(0, 16) + "]");
                return "NULL";
			};
        }

        ptr2 = strstri(ptr1 + 6, "charset = ");
        if(ptr2 != NULL)
        {
            char *temp4 = _findFirst((char *)(ptr2 + 10), " \"'>\n\r");
			if(temp4 != NULL)
			{
                int ln = (int)(temp4 - ptr2 - 10);
				if(ln > 16) return "WTF?";
                strncpy(cdpg, (char *)(ptr2 + 10), (ln > 32) ? 32 : ln );
				if(strstri(cdpg, "%s") != NULL) return "UTF-8";
				return cdpg;
			}
			else
			{
                stt->doEmitionRedFoundData("[GetCodePage] [" + QString(ptr2).mid(0, 16) + "]");
                return "NULL";
			};
        }

        ptr2 = strstri(ptr1 + 6, "charset =");
        if(ptr2 != NULL)
        {
            char *temp4 = _findFirst((char *)(ptr2 + 9), " \"'>\n\r");
			if(temp4 != NULL)
			{
                int ln = (int)(temp4 - ptr2 - 9);
				if(ln > 16) return "WTF?";
                strncpy(cdpg, (char *)(ptr2 + 9), (ln > 32) ? 32 : ln );
				if(strstri(cdpg, "%s") != NULL) return "UTF-8";
				return cdpg;
			}
			else
			{
                stt->doEmitionRedFoundData("[GetCodePage] [" + QString(ptr2).mid(0, 16) + "]");
                return "NULL";
			};
		}
		else
		{
			if(strstri(str, "charset=") != NULL)
			{
				char *temp2 = strstri(str, "charset=");
                char *temp3 = _findFirst((char *)(temp2 + 8), " \"'>\n\r");
				if(temp3 != NULL)
				{
                    int ln = (int)(temp3 - temp2 - 8);
					if(ln > 16) return "WTF?";
                    strncpy(cdpg, (char *)(temp2 + 8), (ln > 32) ? 32 : ln );
					if(strstri(cdpg, "%s") != NULL) return "UTF-8";
					return cdpg;
				}
				else
				{
					stt->doEmitionRedFoundData("[GetCodePage] [" + QString(temp3).mid(0, 16) + "]");
                    return "NULL";
				}
			}
			else return "NULL";
		};	
	}
	else return "NULL";
}

bool isNegative(const std::string *buff, const char *ip, int port, const char *cp)
{
	QTextCodec *codec;
	QString codedStr;

	if (strstri(cp, "shift_jis") != NULL)
	{
		codec = QTextCodec::codecForName("Shift-JIS");
		codedStr = codec->toUnicode(buff->c_str());
	}
	else if (strstri(cp, "utf") != NULL)
	{
		codec = QTextCodec::codecForName("UTF-8");
		codedStr = codec->toUnicode(buff->c_str());
	}
	else if (strstri(cp, "cp") != NULL || strstri(cp, "windows") != NULL)
	{
		codec = QTextCodec::codecForName("Windows-1251");
		codedStr = codec->toUnicode(buff->c_str());
	}
	else if (strstri(cp, "gb") != NULL)
	{
		codec = QTextCodec::codecForName("GB2312");
		codedStr = codec->toUnicode(buff->c_str());
	}
	else codedStr = QString(buff->c_str());

	for (auto negEntry : FileUpdater::negativeVector) {
		FileUpdater::cv.wait(FileUpdater::lk, []{return FileUpdater::ready; });
		if (!globalScanFlag) return true;

		if (Utils::ustrstr(std::string(codedStr.toLocal8Bit().data()), negEntry) != -1) {
			if (gNegDebugMode)
			{
				QTextCodec *nCodec = QTextCodec::codecForName("Windows-1251");
				stt->doEmitionDebugFoundData("[<a href=\"" + QString(ip) +
					"/\"><font color=\"#0084ff\">" + QString(ip) +
					"</font></a>" + "]\tNegative hit: \"" + nCodec->toUnicode(negEntry.c_str()).toHtmlEscaped() + "\"");
			}

			++filtered;
			return true;
		}
	}

	//if (buff->size() < 200) {
	//	if (buff->find("rtsp") == -1) {
	//		std::size_t ptr1 = buff->find("\r\n\r\n");
	//		if (ptr1 != -1) {
	//			int sz = buff->size();
	//			int nSz = buff->substr(ptr1, sz).size() - 4;
	//			if (nSz < 100) {
	//				if (gNegDebugMode)
	//				{
	//					QTextCodec *nCodec = QTextCodec::codecForName("Windows-1251");
	//					stt->doEmitionDebugFoundData("[<a href=\"" + QString(ip) +
	//						"/\"><font color=\"#0084ff\">" + QString(ip) +
	//						"</font></a>]\tNegative hit: Size:" + QString::number(nSz));
	//				}

	//				++filtered;
	//				return true;
	//			}
	//		}
	//	}
	//}

	return false;
}
int globalSearchPrnt(const std::string *buff)
{
	if(Utils::ustrstr(buff, "en/_top.htm") != -1 || Utils::ustrstr(buff, "cannon http server") != -1
		|| Utils::ustrstr(buff, "konica minolta") != -1 || Utils::ustrstr(buff, "/eng/home_frm.htm") != -1
		|| Utils::ustrstr(buff, "networkScanner webserver") != -1 || Utils::ustrstr(buff, "/eng/htm/top.htm") != -1
		|| Utils::ustrstr(buff, "pages/t_ixdmy.htm") != -1
		|| Utils::ustrstr(buff, "/web/guest/") != -1 || Utils::ustrstr(buff, "printerInfo") != -1
		|| Utils::ustrstr(buff, "hp photosmart") != -1
		|| Utils::ustrstr(buff, "menu and") != -1
		|| Utils::ustrstr(buff, "hewlett packard") != -1
		|| Utils::ustrstr(buff, "laserjet") != -1 || Utils::ustrstr(buff, "supplies summary") != -1
		|| Utils::ustrstr(buff, "seiko epson") != -1 || Utils::ustrstr(buff, "ink_y.png") != -1
		|| Utils::ustrstr(buff, "epsonnet") != -1 || Utils::ustrstr(buff, "printer name") != -1
		) 
		{
			//if(gNegDebugMode) stt->doEmitionDebugFoundData("Printer detected.");	

			if (gNegDebugMode)
			{
				/*QTextCodec *nCodec = QTextCodec::codecForName("Windows-1251");*/
				stt->doEmitionDebugFoundData("Printer ignored");
			}
			return -1;
	};

    return 0;
}
int sharedDetector(const char * ip, int port, const std::string *buffcpy, const char *cp) {

	if (buffcpy->size() == 0) {
		if (port == 9000) {
			if (HikVis::checkSAFARI(ip, port))												return 6; //Safari CCTV
			else if (HikVis::checkHikk(ip, port))											return 4; //Hikkvision iVMS
			else if (HikVis::checkRVI(ip, port))											return 5; //RVI
			else
			{
				if (gNegDebugMode)
				{
					stt->doEmitionDebugFoundData("Safari CCTV check failed - ignoring [<a href=\"" + QString(ip) +
						"/\"><font color=\"#0084ff\">" + QString(ip) + "</font></a>]");
				}
				return -1;
			}
		}
		else if(port == 8000) {
			if (HikVis::checkHikk(ip, port))												return 4; //Hikkvision iVMS
			else if (HikVis::checkRVI(ip, port))											return 5; //RVI
			//else if (HikVis::checkSAFARI(ip, port))											return 6; //Safari CCTV
			else
			{
				if (gNegDebugMode)
				{
					stt->doEmitionDebugFoundData("Hikkvision iVMS check failed - ignoring [<a href=\"" + QString(ip) +
						"/\"><font color=\"#0084ff\">" + QString(ip) + "</font></a>]");
				}
				return -1;
			}
		}
		else if (port == 37777) {
			if (HikVis::checkRVI(ip, port))													return 5; //RVI
			else if(HikVis::checkHikk(ip, port))											return 4; //Hikkvision iVMS
			else if (HikVis::checkSAFARI(ip, port))											return 6; //Safari CCTV
			else
			{
				if (gNegDebugMode)
				{
					stt->doEmitionDebugFoundData("RVI check failed - ignoring [<a href=\"" + QString(ip) +
						"/\"><font color=\"#0084ff\">" + QString(ip) + "</font></a>]");
				}
				return -1;
			}
		} else {
			//stt->doEmitionDebugFoundData("[<a href=\"" + QString(ip) + ":" + QString::number(port) +
			//	"/\"><font color=\"#0084ff\">" + QString(ip) + ":" + QString::number(port) +
			//	"</font></a>]\tNegative hit: Size: 0");
			//return -1;
		}
	}

	int isDig = Utils::isDigest(buffcpy);
	if (isDig != -1)																	return 2; //Auth

	if (Utils::ustrstr(buffcpy, "netwave ip camera") != -1)								return 11;
	if (Utils::ustrstr(buffcpy, "live view / - axis") != -1)							return 12;
	if (Utils::ustrstr(buffcpy, "vilar ipcamera") != -1)								return 13;
	if (Utils::ustrstr(buffcpy, "window.location = \"rdr.cgi\"") != -1)					return 14;
	if (Utils::ustrstr(buffcpy, "httpfileserver") != -1)								return 15;
    if(Utils::ustrstr(buffcpy, "real-time ip camera monitoring system") != -1
            || Utils::ustrstr(buffcpy, "server push mode") != -1
        )																				return 17; //Real-time IP Camera Monitoring System
    if(Utils::ustrstr(buffcpy, "linksys.com") != -1 
		&& Utils::ustrstr(buffcpy, "tm05") != -1)										return 18; //linksys.com cameras
    if(Utils::ustrstr(buffcpy, "reecam ip camera") != -1)								return 19; //reecam cameras
    if(Utils::ustrstr(buffcpy, "/view/viewer_index.shtml") != -1)						return 20; //axis cameras
    if(Utils::ustrstr(buffcpy, "bridge eyeon") != -1)									return 21; //Bridge Eyeon
    if(Utils::ustrstr(buffcpy, "ip camera control webpage") != -1 
		&& Utils::ustrstr(buffcpy, "/main/cs_motion.asp") != -1)						return 22; //ip camera control
    if(Utils::ustrstr(buffcpy, "network camera") != -1 
		&& Utils::ustrstr(buffcpy, "/live/index2.html") != -1)							return 23; //network camera BB-SC384
    if(Utils::ustrstr(buffcpy, "network camera") != -1 
		&& Utils::ustrstr(buffcpy, "/viewer/live/en/live.html") != -1)					return 24; //Network Camera VB-M40
    if(Utils::ustrstr(buffcpy, "panasonic ") != -1 
		&& Utils::ustrstr(buffcpy, ":60002/snapshotjpeg") != -1)						return 25; //Panasonic wtfidonteven-camera
    if(Utils::ustrstr(buffcpy, "sony network camera") != -1 
		&& Utils::ustrstr(buffcpy, "/command/inquiry.cgi?") != -1)						return 26; //Sony Network Camera
    if(Utils::ustrstr(buffcpy, "network camera") != -1 
		&& Utils::ustrstr(buffcpy, "src=\"webs.cgi?") != -1)							return 27; //UA Network Camera
    if(Utils::ustrstr(buffcpy, "network camera") != -1 
		&& Utils::ustrstr(buffcpy, "/viewer/live/index.html") != -1)					return 28; //Network Camera VB-M40
    if(Utils::ustrstr(buffcpy, "lg smart ip device") != -1)								return 29; //LG Smart IP Device Camera
    if(Utils::ustrstr(buffcpy, "/view/viewer_index.shtml") != -1)						return 20; //axis cameras
    if(Utils::ustrstr(buffcpy, "nas") != -1 
		&& Utils::ustrstr(buffcpy, "/cgi-bin/data/viostor-220/viostor/viostor.cgi") != -1)	return 30; //NAX
    if(Utils::ustrstr(buffcpy, "ip camera") != -1 
		&& Utils::ustrstr(buffcpy, "check_user.cgi") != -1)								return 31; //axis cameras
    if(Utils::ustrstr(buffcpy, "ws(\"user\");") != -1 
		&& Utils::ustrstr(buffcpy, "src=\"/tool.js") != -1
            && Utils::ustrstr(buffcpy, "<b class=\"xb1\"></b>") != -1)                  return 32; //web ip cam
    if(Utils::ustrstr(buffcpy, "geovision") != -1
            && (Utils::ustrstr(buffcpy, "ip camera") != -1
                || Utils::ustrstr(buffcpy, "ssi.cgi/login.htm") != -1)
				)																		return 33; //GEO web ip cam

    if(Utils::ustrstr(buffcpy, "hikvision-webs") != -1
		|| (
		(Utils::ustrstr(buffcpy, "hikvision digital") != -1 || Utils::ustrstr(buffcpy, "doc/page/login.asp") != -1)
		&& Utils::ustrstr(buffcpy, "dvrdvs-webs") != -1 || Utils::ustrstr(buffcpy, "app-webs") != -1 || Utils::ustrstr(buffcpy, "DNVRS-Webs") != -1)
		|| 
		(Utils::ustrstr(buffcpy, "lapassword") != -1 
		&& Utils::ustrstr(buffcpy, "lausername") != -1 
		&& Utils::ustrstr(buffcpy, "dologin()") != -1)
		)																				return 34; //hikvision cam
    if((Utils::ustrstr(buffcpy, "easy cam") != -1 
		&& Utils::ustrstr(buffcpy, "easy life") != -1)
        || (Utils::ustrstr(buffcpy, "ipcamera") != -1 
		&& Utils::ustrstr(buffcpy, "/tool.js") != -1)
		)																				return 35; //EasyCam
    if(Utils::ustrstr(buffcpy, "/config/cam_portal.cgi") != -1 
		|| Utils::ustrstr(buffcpy, "/config/easy_index.cgi") != -1)						return 36; //Panasonic Cam
    if(Utils::ustrstr(buffcpy, "panasonic") != -1 
		&& Utils::ustrstr(buffcpy, "/view/getuid.cgi") != -1)							return 37; //Panasonic Cam WJ-HD180
    if(Utils::ustrstr(buffcpy, "ipcam client") != -1 && (
		Utils::ustrstr(buffcpy, "plugins.xpi") != -1 || 
		Utils::ustrstr(buffcpy, "IPCWebComponents") != -1)
		&& Utils::ustrstr(buffcpy, "js/upfile.js") != -1)								return 38; //Foscam
    if(Utils::ustrstr(buffcpy, "ip surveillance") != -1 
		&& Utils::ustrstr(buffcpy, "customer login") != -1)								return 39; //EagleEye
    if(Utils::ustrstr(buffcpy, "network camera") != -1 
		&& Utils::ustrstr(buffcpy, "/admin/index.shtml?") != -1)						return 40; //Network Camera VB-C300
    if(Utils::ustrstr(buffcpy, "sq-webcam") != -1 
		&& Utils::ustrstr(buffcpy, "liveview.html") != -1)								return 41; //AVIOSYS-camera
    if(Utils::ustrstr(buffcpy, "nw_camera") != -1 
		&& Utils::ustrstr(buffcpy, "/cgi-bin/getuid") != -1)							return 42; //NW_camera
    if(Utils::ustrstr(buffcpy, "micros") != -1 
		&& Utils::ustrstr(buffcpy, "/gui/gui_outer_frame.shtml") != -1)					return 43; //NW_camera
    if(Utils::ustrstr(buffcpy, "lapassword") != -1
        && Utils::ustrstr(buffcpy, "lausername") != -1
        && Utils::ustrstr(buffcpy, "g_ologin.dologin()") != -1
        )																				return 44; //hikvision cam 2
    if(Utils::ustrstr(buffcpy, "panasonic") != -1 
		&& Utils::ustrstr(buffcpy, "/config/index.cgi") != -1)							return 45; //Panasonic Cam BB-HG???
    if(Utils::ustrstr(buffcpy, "/ui/") != -1 
		&& Utils::ustrstr(buffcpy, "sencha-touch") != -1)								return 46; //BUFFALO disk
    if(Utils::ustrstr(buffcpy, "digital video server") != -1 
		&& Utils::ustrstr(buffcpy, "gui.css") != -1)									return 47; //Digital Video Server
        if(Utils::ustrstr(buffcpy, "/ipcamerasetup.zip") != -1 
			&& Utils::ustrstr(buffcpy, "download player") != -1
        && Utils::ustrstr(buffcpy, "ipcam") != -1
		)																				return 48; //ipCam
    if(Utils::ustrstr(buffcpy, "dvr") != -1 
		&& Utils::ustrstr(buffcpy, "ieorforefox") != -1
        && Utils::ustrstr(buffcpy, "sofari") != -1
		)																				return 49; //IEORFOREFOX
	if (Utils::ustrstr(buffcpy, "seyeon") != -1
		&& (Utils::ustrstr(buffcpy, "/app/multi/single.asp") != -1
		|| Utils::ustrstr(buffcpy, "/app/live/sim/single.asp") != -1)
		)																				return 50; //Network Video System
	if (Utils::ustrstr(buffcpy, "MASPRO DENKOH") != -1)									return 51; //MASPRO
	if (Utils::ustrstr(buffcpy, "webcamXP") != -1
		&& Utils::ustrstr(buffcpy, "a valid username/password") != -1
		)																				return 52; //Webcamxp5
	if (Utils::ustrstr(buffcpy, "NetSuveillance") != -1
		&& Utils::ustrstr(buffcpy, "l_bgm.gif") != -1
		)																				return 53; //Jassun (http://176.32.180.42/Login.htm)
	if (Utils::ustrstr(buffcpy, "WEB SERVICE") != -1
		&& Utils::ustrstr(buffcpy, "jsmain/liveview.js") != -1
		)																				return 54; //Beward (http://46.146.243.88:88/login.asp)
	if (Utils::ustrstr(buffcpy, "get_status.cgi") != -1
		&& Utils::ustrstr(buffcpy, "str_device+") != -1
		)																				return 55; //QCam (http://1.177.123.118:8080/)
	if (Utils::ustrstr(buffcpy, "EagleEyes") != -1
		&& (Utils::ustrstr(buffcpy, "/Login.cgi?rnd=") != -1
		|| Utils::ustrstr(buffcpy, "mobile480.htm") != -1)
		)																				return 56; //EaglesEye (http://203.190.113.34:88/nobody/mobile480.htm)
	if (Utils::ustrstr(buffcpy, "dvr_remember") != -1
		&& Utils::ustrstr(buffcpy, "login_chk_usr_pwd") != -1
		)																				return 57; //Network video client (http://203.190.113.54:60001/)
	if (Utils::ustrstr(buffcpy, "QlikView") != -1)										return 58; //QLikView (http://203.96.113.183/qlikview/login.htm)
	if (Utils::ustrstr(buffcpy, "RTSP/1.") != -1)										return 59; //RTSP (http://121.72.55.19:554/ Hisilicon Ipcam)
	if (Utils::ustrstr(buffcpy, "ACTi Corporation") != -1)								return 60; //ACTi (http://87.197.30.20/cgi-bin/videoconfiguration.cgi)
	if (Utils::ustrstr(buffcpy, "airos_logo") != -1)									return 61; //AirOS (http://103.5.73.114/login.cgi?uri=/)
	if (Utils::ustrstr(buffcpy, "NetSuveillanceWebCookie") != -1 && 
		Utils::ustrstr(buffcpy, "resizeL") != -1)										return 62; //XMSECU (http://121.72.179.76/Login.htm)
	
	//if (Utils::ustrstr(buffcpy, "ShareCenter") != -1)									return 58; //ShareCenter (http://49.50.207.6/)

	if (Utils::ustrstr(buffcpy, "nas - ") != -1
		&& Utils::ustrstr(buffcpy, "login.html?pg=index.html") != -1)					return 1; //https NAS (https://90.224.187.151/)

    if(((Utils::ustrstr(buffcpy, "220") != -1) && (port == 21)) ||
        Utils::ustrstr(buffcpy, "220 diskStation ftp server ready") != -1 ||
        Utils::ustrstr(buffcpy, "220 ftp server ready") != -1
		|| Utils::ustrstr(buffcpy, "500 'get': command not understood") != -1
        )                                                                               return 3; // 3 - FTP

	if (Utils::ustrstr(buffcpy, "camera") != -1 ||
		Utils::ustrstr(buffcpy, "webcamxp") != -1 ||
		Utils::ustrstr(buffcpy, "ipcam") != -1 ||
		Utils::ustrstr(buffcpy, "smart ip") != -1 ||
		Utils::ustrstr(buffcpy, "sanpshot_icon") != -1 ||
		Utils::ustrstr(buffcpy, "snapshot_icon") != -1
		)																				return 0;

    if((Utils::ustrstr(buffcpy, "hfs /") != -1			||
		Utils::ustrstr(buffcpy, "httpfileserver") != -1 ||
		Utils::ustrstr(buffcpy, "index of") != -1		||
        Utils::ustrstr(buffcpy, "$lock extended") != -1
		)
        && Utils::ustrstr(buffcpy, "customer") == -1
        && Utils::ustrstr(buffcpy, "purchase") == -1
        && Utils::ustrstr(buffcpy, "contac") == -1
        && Utils::ustrstr(buffcpy, "company") == -1
        )																				return 1;

	if (isNegative(buffcpy, ip, port, cp))												return -1;
	//if (globalSearchPrnt(buffcpy) == -1)												return -1;

    return 1;
}
// 500 < 1600
//int _mainFinderFirst(const std::string *buffcpy, int port, const char *ip, const char *cp)
//{
//    int flag = sharedDetector(ip, port, buffcpy, cp);
//	if (flag != -2) return flag;
//
//	return 0;
//}
////> 1600
//int _mainFinderSecond(const std::string *buffcpy, int port, const char *ip, const char *cp)
//{
//    int flag = sharedDetector(ip, port, buffcpy, cp);
//    if(flag != -2) return flag;
//
//	return 3; //Suspicious
//}



int firstStage(const std::string *buffcpy, int port, const char *ip, const char *cp, int sz)
{
	if (buffcpy->size() == 0 && sz != 0) {
		stt->doEmitionYellowFoundData("Strange behavior: 0 bytes. " + QString(ip));
		return -1;
	}
	int flag = sharedDetector(ip, port, buffcpy, cp);
	if (flag != -2) return flag;

	return 1;
}
int contentFilter(const std::string *buff, int port, const char *ip, const char *cp, int sz)
{
	//int res = 0;
	//if (sz <= 500)										res = _mainFinderFirst(buff, 1, port, ip, cp);
	//else if ((sz > 500 && sz <= 3500) || sz > 180000)	res = _mainFinderFirst(buff, 0, port, ip, cp);
	//else if (sz > 3500 && sz <= 180000)					res = _mainFinderSecond(buff, port, ip, cp);

	//return res;

	if (sz < 180000) {
		return firstStage(buff, port, ip, cp, sz);
	}
	else {
		return 1;
	}
}

void fillGlobalLogData(const char *ip, int port, const char *sz, const char *title,
                       const char *login, const char *pass, char *comment, char *cdpg, char *clss)
{
	if(trackerOK == true)
	{
		while(gGlobalTrackLocked == true) Sleep(10);
		gGlobalTrackLocked = true;
		
		QJsonObject jsonData; 

			if(gMode == 0 || gMode == -1)
			{
				if(strlen(ip) > 0) jsonData.insert("ip_addr", QJsonValue(QString(ip)) );
				else jsonData.insert("ip_addr", QJsonValue(QString("")) );

                jsonData.insert("hostname", QJsonValue(QString("")) );
			}
			else
			{
				jsonData.insert("ip_addr", QJsonValue(QString("")) );
				jsonData.insert("hostname", QJsonValue(QString(ip)) );
			};

			jsonData.insert("port", QJsonValue(QString::number(port)) );
			jsonData.insert("recv", QJsonValue(QString(sz)));
			QString tt = QString(base64_encode((const unsigned char *)title, strlen(title)).c_str());
			if(strlen(title) == 0) jsonData.insert("title", QJsonValue(QString("NULL")) );
			else jsonData.insert("title", QJsonValue(QString(base64_encode((const unsigned char *)title, strlen(title)).c_str())) );
			if(strlen(login) > 0) jsonData.insert("login", QJsonValue(QString(login)) );
			else jsonData.insert("login", QJsonValue(QString("")) );
			if(strlen(pass) > 0) jsonData.insert("pass", QJsonValue(QString(pass)) );
			else jsonData.insert("pass", QJsonValue(QString("")) );
			if(strlen(comment) > 0) jsonData.insert("other", QJsonValue(QString(comment)) );
			else jsonData.insert("other", QJsonValue(QString("")) );
			if(strlen(cdpg) > 0) jsonData.insert("encoding", QJsonValue(QString(cdpg)) );
			else jsonData.insert("encoding", QJsonValue(QString("")) );
			if(strlen(clss) > 0) jsonData.insert("Class", QJsonValue(QString(clss)) );
			else jsonData.insert("Class", QJsonValue(QString("")) );

		jsonArr->push_front(jsonData);
		gGlobalTrackLocked = false;
	};
}

int __checkFileExistence(int flag)
{
	char fileName[64] = {0};

	if (flag == -22)									sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE5 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());
	else if (flag == 0 || flag == 15 || flag == -10)	sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE1 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());
	else if (flag == 3)									sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE2 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());
	else if (flag == 16)								sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE4 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());
	else if(flag >= 17 || flag == 11 || flag == 12 
		|| flag == 13 || flag == 14 || flag == 1)		sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE3 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());

	FILE *f = fopen(fileName, "r");
	if(f == NULL) return true;
	else 
	{
		fclose(f);
		return false;
	};
}
bool ftsCameras = true;
bool ftsOther = true;
bool ftsSSH = true;
bool ftsFTP = true;
bool ftsBA = true;
std::atomic<bool> fOpened(false);
void fputsf(char *text, int flag)
{
	char fileName[256] = { 0 };

	if(flag == 0 || flag == 15 || flag == -10) 
	{
		if (ftsCameras) ftsCameras		= __checkFileExistence(flag);
		sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE1 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());
	}
	else if(flag == 1) 
	{
		if(ftsOther) ftsOther			= __checkFileExistence(flag);
		sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE2 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());
	}
	else if(flag == -22) 
	{
		if(ftsSSH) ftsSSH				= __checkFileExistence(flag);
		sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE5 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());
	}
	else if(flag == 3) 
	{
		if(ftsFTP) ftsFTP				= __checkFileExistence(flag);
		sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE4 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());
	}
	else if(flag >= 17 || flag == 11 || flag == 12 
		|| flag == 13 || flag == 14 || flag == 2
		) 
	{
		if(ftsBA) ftsBA					= __checkFileExistence(flag);
		sprintf(fileName, "./" DIR_NAME "%s_%s/" TYPE3 ".html", Utils::getStartDate().c_str(), Utils::getCurrentTarget().c_str());
	}
	else stt->doEmitionRedFoundData("Unknown flag [FLAG: " + QString::number(flag) + "]");

	FILE *file = fopen(fileName, "a");

	if(file != NULL)
	{
		time_t rtime;
		time(&rtime);
		if(horLineFlag == false) 
		{
			horLineFlag = true;
			char delimiter[128] = {0};
			char cdate[32] = {0};
			strcpy (cdate, "[");
			strcat (cdate, ctime (&rtime));
			memset (cdate + strlen(cdate) - 1, '\0', 1);
			strcat (cdate, "] ");
			strcpy(delimiter, "<hr><center><h5><font color=\"#a1a1a1\">");
			strcat(delimiter, cdate);
			strcat(delimiter, "</font></h5></center><hr>");
			fputs (delimiter, file);
		};

		++saved;
		char *string = new char[strlen(text) + 512];

		if(flag != -22) 
		{
			strcpy (string, "<div id=\"ipd\" style=\"color:#707070;text-decoration: none;\">");
			
			char cdate[32] = {0};
			strcat (cdate, "[");
			strcat (cdate, ctime (&rtime));
			memset (cdate + strlen(cdate) - 1, '\0', 1);
			strcat (cdate, "] ");
			strcat (string, cdate);
			strcat (string, text);
			strcat (string, "</div>");
		}
		else
		{
			strcpy (string, "<div id=\"ipd\" style=\"color:#707070;\">");
			
			char cdate[32] = {0};
			strcat (cdate, "[");
			strcat (cdate, ctime (&rtime));
			memset (cdate + strlen(cdate) - 1, '\0', 1);
			strcat (cdate, "] ");
			strcat (string, cdate);
			strcat (string, text);
			strcat (string, "</div>");
		};

		if (flag == 0 && ftsCameras)
		{
			char tmsg[1024] = {0};
			ftsCameras = false;
			strcpy(tmsg, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>" TYPE1 "</title>");
			//strcat(tmsg, msg);
			strcat(tmsg, HTTP_FILE_STYLE);
			fputs (tmsg, file);
			fputs(HTTP_FILE_HEADER, file);
		};
		if(flag == 1 && ftsOther)	
		{
			char tmsg[1024] = {0};
			ftsOther = false;
			strcpy(tmsg, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>" TYPE2 "</title>");
			//strcat(tmsg, msg);
			strcat(tmsg, HTTP_FILE_STYLE);
			fputs (tmsg, file);
			fputs(HTTP_FILE_HEADER, file);
		};
		if(flag == -22 && ftsSSH)	
		{
			char tmsg[1024] = {0};
			ftsOther = false;
			strcpy(tmsg, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>" TYPE5 "</title>");
			//strcat(tmsg, msg);
			strcat(tmsg, HTTP_FILE_STYLE);
			fputs (tmsg, file);
			fputs(HTTP_FILE_HEADER, file);
		};
		if(flag == 3 && ftsFTP)
		{
			char tmsg[1024] = {0};
			ftsFTP = false;
			strcpy(tmsg, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>" TYPE4 "</title>");
			//strcat(tmsg, msg);
			strcat(tmsg, HTTP_FILE_STYLE);
			fputs (tmsg, file);
			fputs(HTTP_FILE_HEADER, file);
		};
		if((flag >= 17 || flag == 11 || flag == 12 || flag == 13 || flag == 14 || flag == 2) && ftsBA) 
		{
			char tmsg[1024] = {0};
			ftsBA = false;
			strcpy(tmsg, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" /><title>" TYPE3 "</title>");
			//strcat(tmsg, msg);
			strcat(tmsg, HTTP_FILE_STYLE);
			fputs (tmsg, file);
			fputs(HTTP_FILE_HEADER, file);
		};
 
		while(fOpened) {
			Sleep((rand() % 10 + 60));
		};
		fOpened = true;
		fputs (string, file);
		fclose (file);
		fOpened = false;

		delete []string;
	}
	else
	{
		stt->doEmitionRedFoundData("Cannot open file [Flag: " + QString::number(flag) + " Err:" + QString::number(GetLastError()) + "]");
		MainStarter::createResultFiles();
	};
}

#define REDIRECT_PIC "<img src='data:image/gif;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAICAIAAAB/FOjAAAAABGdBTUEAAK/INwWK6QAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAA7SURBVHjaYmIgEbAA8f///4lUzcjIyILMAxE4lMINZUEWAxE4DIazmcjxA6YxRGkg0ussxJsNAQABBgDDuAwgjmnSVwAAAABJRU5ErkJggg=='/>"
#define PEKO_PIC "<a href='[PEKO]'><img src='data:image/gif;base64,iVBORw0KGgoAAAANSUhEUgAAABAAAAAICAIAAAB/FOjAAAAABGdBTUEAAK/INwWK6QAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAABASURBVHjaYmAgETAC8Yd1fESqFgj6BNLw//9/Yo1nZGSBsPg/SgHJTwLPiXISSTYQpe4/BIHNZSHJBnIAQIABAEhuFAfSpDW+AAAAAElFTkSuQmCC'/></a>"

void putInFile(int flag, const char *ip, int port, int size, const char *finalstr, char *cp)
{
	char log[4096] = {0}, msg[512] = {0};
		
	QTextCodec *codec;
	sprintf(msg, "<a href=\"%s/\"><span style=\"color: #a1a1a1;\">%s</span></a>",
		ip, ip);

	QString resMes(msg);
	QString strf;
	if(strstri(cp, "shift_jis") != NULL)
	{
		codec = QTextCodec::codecForName("Shift-JIS");
		strf = codec->toUnicode(finalstr);
	}
	else if(strstri(cp, "utf") != NULL)
	{
		codec = QTextCodec::codecForName("UTF-8");
		strf = codec->toUnicode(finalstr);
	}
	else if (strstri(cp, "cp") != NULL || strstri(cp, "windows") != NULL)
	{
		codec = QTextCodec::codecForName("Windows-1251");
		strf = codec->toUnicode(finalstr);
	}
	else if (strstri(cp, "gb") != NULL)
	{
		codec = QTextCodec::codecForName("GB2312");
		strf = codec->toUnicode(finalstr);
	}
	else strf = QString(finalstr);

	if(flag != 6 && flag != 5 && flag != 4)
	{
		strcat(msg, " <font color=\"#0084ff\">: </font><font color=\"#ff9600\">");
		int sz = strf.size();
		strncat(msg, QString::fromLocal8Bit(finalstr).toHtmlEscaped().toLocal8Bit().data(), (sz < 128 ? sz : 128));
		strcat(msg, "</font>");
		resMes += " <font color=\"#0084ff\">: </font><font color=\"#ff9600\">" + strf.toHtmlEscaped() + "</font>";
	};

	resMes.replace("[PK]", PEKO_PIC);
	stt->doEmitionFoundData(resMes.replace("[R]", REDIRECT_PIC));

	sprintf(log, "<span id=\"hostSpan\"><a href=\"%s\"/><font color=MediumSeaGreen>%s</font></a>;</span> <span id=\"recvSpan\">Received: <font color=SteelBlue>%d</font>",
        ip, ip, size);
	
	//Generic camera
	if(flag == 0 || flag == 15 || flag == -10)
	{
		fillGlobalLogData(ip, port, std::to_string(size).c_str(), finalstr, "", "", "", cp,  TYPE1 );
	}
	//Other
	else if(flag == 1)
	{
		fillGlobalLogData(ip, port, std::to_string(size).c_str(), finalstr, "", "", "", cp,  TYPE2 );
	}
	//Special camera (Hikk, RVI, Safari, etc)
	if(flag != 6 && flag != 5 && flag != 4)
	{
		strcat(log, ";</span> T: <font color=GoldenRod>");

		strncat(log, QString::fromLocal8Bit(finalstr).toHtmlEscaped().toLocal8Bit().data(), 100);
		strcat(log, "</font>");
	};
	strcat(log, "\n");

    fputsf (log, flag);

	//ZeroMemory(msg, strlen(msg));
	msg[0] = 0;
}
//void _specFillerWF(const char *ip, int port, const char *finalstr, const char *login, const char *pass, int flag)
//{
//	char log[512] = { 0 };
//
//	++PieBA;
//
//	if (strlen(login) > 0 || strlen(pass) > 0)
//	{
//		sprintf(log, "[WF]:<span id=\"hostSpan\"><a href=\"%s\"><font color=darkturquoise>%s (%s:%s)</font></a></span> T: <font color=GoldenRod>%s</font>\n",
//			ip, ip, login, pass, finalstr);
//	}
//
//	stt->doEmitionFoundData(QString::fromLocal8Bit(log));
//
//	fputsf(log, flag);
//}

void _specFillerCustom(const char *ip, int port, const char *finalstr, const char *login, const char *pass, int flag, const char *classString) {
	char log[512] = { 0 };

	++PieBA;

	if (strlen(login) > 0 || strlen(pass) > 0)
	{
		sprintf(log, "<font color=orangered>%s</font>: <span id=\"hostSpan\"><a href=\"%s\"><font color=darkcyan>%s (%s:%s)</font></a></span> T: <font color=GoldenRod>%s</font>\n",
			classString, ip, ip, login, pass, finalstr);
	}

	stt->doEmitionFoundData(QString::fromLocal8Bit(log));

	fputsf(log, flag);
}
void _specFillerBA(const char *ip, int port, const char *finalstr, const char *login, const char *pass, int flag)
{
 /*   char log[512] = {0};
	
	++PieBA;

	if (strlen(login) > 0 || strlen(pass) > 0)
	{
		sprintf(log, "[BA]:<span id=\"hostSpan\"><a href=\"%s:%s@%s\"><font color=MediumSeaGreen>%s:%s@%s</font></a></span> T: <font color=GoldenRod>%s</font>\n",
			login, pass, ip, login, pass, ip, finalstr);
	}

	stt->doEmitionFoundData(QString::fromLocal8Bit(log));

	fputsf(log, flag);*/

	char log[512] = { 0 };
	int offset = 0;

	if (strstri(ip, "https://") != NULL) {
		offset = 8;
	}
	else if (strstri(ip, "http://") != NULL) {
		offset = 7;
	}

	if (strlen(login) > 0 || strlen(pass) > 0)
    {
		if (8 == offset) {
			sprintf(log, "[BA]: <span id=\"hostSpan\"><a href=\"https://%s:%s@%s\"><font color=floralwhite>%s:%s@%s</font></a></span> T: <font color=GoldenRod>%s</font>\n",
				login, pass, ip + offset, login, pass, ip + offset, finalstr);
		}
		else {
			sprintf(log, "[BA]: <span id=\"hostSpan\"><a href=\"http://%s:%s@%s\"><font color=floralwhite>%s:%s@%s</font></a></span> T: <font color=GoldenRod>%s</font>\n",
				login, pass, ip + offset, login, pass, ip + offset, finalstr);
		}
    } else {
		if (8 == offset) {
			sprintf(log, "[BA]: <span id=\"hostSpan\"><a href=\"https://%s\"><font color=floralwhite>%s</font></a></span> T: <font color=GoldenRod>%s</font>\n",
				ip + offset, ip + offset, finalstr);
		}
		else {
			sprintf(log, "[BA]: <span id=\"hostSpan\"><a href=\"http://%s\"><font color=floralwhite>%s</font></a></span> T: <font color=GoldenRod>%s</font>\n",
				ip + offset, ip + offset, finalstr);
		}
    }

	stt->doEmitionFoundData(QString::fromLocal8Bit(log));

	fputsf(log, flag);
}

void _specFillerRSTP(const char *ip, int port, const char *finalstr, const char *login, const char *pass, int flag)
{
	char log[512] = { 0 };

	++PieBA;

	if (strlen(login) > 0 || strlen(pass) > 0)
	{
			sprintf(log, "[RSTP]: <span id=\"hostSpan\"><a href=\"%s11\"><font color=\"#736AFF\">%s11 (%s:%s)</font></a></span> T: <font color=#F0E68C>%s</font>\n",
				ip, ip, login, pass, finalstr);
	}
	else {
			sprintf(log, "[RSTP]: <span id=\"hostSpan\"><a href=\"%s11\"><font color=\"#736AFF\">%s11</font></a></span> T: <font color=#F0E68C>%s</font>\n",
				ip, ip, finalstr);
	}

	stt->doEmitionFoundDataCustom(QString::fromLocal8Bit(log), "3090C7");

	fputsf(log, flag);
}

//void _specFillerWF(const char *ip, int port, char *finalstr, char *login, char *pass, int flag)
//{
//	char log[512] = {0};
//	
//	++PieWF;
//
//    sprintf(log, "[WF]:<span id=\"hostSpan\"><a href=\"%s:%s\"><font color=MediumSeaGreen>%s:%s</font></a></span> T: <font color=GoldenRod>%s</font> Pass: <font color=SteelBlue>%s:%s</font>\n",
//            ip, port, ip, port, finalstr, login, pass);
//
//	stt->doEmitionFoundData(QString::fromLocal8Bit(log));
//
//    fputsf (log , flag, "Web Form");
//}
//void _getFormVal(char *data, char *result, char *key, char *path = NULL)
//{
//	char parVal[256] = {0};
//	int psz = 0;
//	char *pkeyResult1 = strstr(data, ">");
//	if(pkeyResult1 != NULL)
//	{
//		psz = pkeyResult1 - data + 1;
//		strncpy(parVal, data, (psz < 256 ? psz : 256));
//	}
//	else
//	{
//		strncpy(parVal, data, 256);		
//	};
//
//	int sz = 0;
//	char parVal2[256] = {0};
//
//	char startPath[256] = {0};
//	if(strcmp(key, "action") == 0)
//	{
//		if(strstr(path, "./") == NULL)
//		{
//			char *ptrP1 = _findLast(path, "/");
//			if(ptrP1 != path)
//			{
//				int pSz = ptrP1 -path;
//				strncpy(startPath, path, pSz);
//			};
//		};
//	};
//
//	char *keyResult1 = strstri(parVal, key);
//
//	if(keyResult1 != NULL)
//	{
//		char *pkeyResult2 = _findFirst(keyResult1, " >");
//		if(pkeyResult2 != NULL)
//		{
//			int psz2 = pkeyResult2 - keyResult1;
//			strncpy(parVal2, keyResult1, (psz2 < 256 ? psz2 : 256));
//
//			char *keyResult2 = _findFirst(parVal2, "'\"");
//			if(keyResult2 != NULL)
//			{
//				char *keyResult3 = _findFirst(keyResult2 + 1, "'\"> ");
//				if(keyResult3 != NULL)
//				{
//					sz = keyResult3 - keyResult2 - 1;
//					char tempRes[256] = {0};
//					if(strstr(keyResult2, "./") != NULL) 
//					{
//						strcpy(result, startPath);
//						strncpy(tempRes, keyResult2 + 2, sz - 1);
//						if(tempRes[0] != '/') strcat(result, "/");
//						strcat(result, tempRes);
//					}
//					else if(strstr(keyResult2, "/") == NULL)
//					{
//						if(strcmp(key, "action") == 0)
//						{
//							strcpy(result, startPath);
//							strncpy(tempRes, keyResult2 + 1, sz);
//							if(tempRes[0] != '/') strcat(result, "/");
//							strcat(result, tempRes);
//						}
//						else
//						{
//							strncpy(result, keyResult2 + 1, sz);							
//						};
//					}
//					else
//					{
//						strncpy(result, keyResult2 + 1, sz);
//					};
//				};
//			}
//			else
//			{
//				keyResult2 = _findFirst(parVal2, "=");
//				if(keyResult2 != NULL)
//				{
//					char *keyResult3 = _findFirst(keyResult2, "'\"> ");
//					if(keyResult3 != NULL )
//					{
//						sz = keyResult3 - keyResult2 - 1;
//						strncpy(result, keyResult2 + 1, sz);
//						char tempRes[256] = {0};
//						if(strstr(keyResult2, "./") != NULL) 
//						{
//							strcpy(result, startPath);
//							strncpy(tempRes, keyResult2 + 2, sz - 1);
//							if(tempRes[0] != '/') strcat(result, "/");
//							strcat(result, tempRes);
//						}
//						else if(strstr(keyResult2, "/") == NULL)
//						{
//							if(strcmp(key, "action") == 0)
//							{
//								strcpy(result, startPath);
//								strncpy(tempRes, keyResult2 + 1, sz);
//								if(tempRes[0] != '/') strcat(result, "/");
//								strcat(result, tempRes);
//							}
//							else
//							{
//								strncpy(result, keyResult2 + 1, sz);
//							};
//						}
//						else
//						{
//							strncpy(result, keyResult2 + 1, sz);
//						};
//					}
//					else
//					{
//						strcpy(result, startPath);
//						strcat(result, keyResult2 + 1);
//					};
//				}
//			};
//
//		}
//		else
//		{
//			stt->doEmitionFoundData("[WF]: GetParam - Cannot retrieve field.");				
//		};
//	};
//}
//
//static const std::string arrUser[] = {"user", "usr", "username", "login", "lgn", "account", "acc", "param1", "param3", "id", "A1", "uname", "mail", "name"};
//std::vector<std::string> vecUser (arrUser, arrUser + sizeof(arrUser) / sizeof(arrUser[0]) );
//static const std::string arrPass[] = {"pass", "pw", "password", "code", "param2", "param4", "secret", "login_p", "A2", "admin_pw", "pws", "secretkey"};
//std::vector<std::string> vecPass (arrPass, arrPass + sizeof(arrPass) / sizeof(arrPass[0]) );
//
//char *_getAttribute(const char *str, char *attrib)
//{
//	if(strstri(str, attrib) != NULL)
//	{
//		char res[1024] = {0};
//		char *ptrStart = strstri(str, attrib);
//		char *ptrEnd = _findFirst(ptrStart, "\r\n");
//		if(ptrEnd != NULL)
//		{
//			int szAt = strlen(attrib);
//			int sz = ptrEnd - ptrStart - szAt;
//
//			if(sz != 0 && sz < 1024) strncpy(res, ptrStart + szAt, sz);
//			else return "";
//
//			return res;
//		}
//		else return "";
//	}
//	else return "";
//}
//
//void _getInputVal(std::vector<std::string> inputVec, char *buff, char *key)
//{
//	char *pos = NULL;
//	char field[256] = {0};
//	if(strcmp(key, "USER") == 0)
//	{
//		for(int i = 0; i < inputVec.size(); ++i)
//		{
//			ZeroMemory(field, 256);
//			_getFormVal((char*)inputVec[i].data(), field, "name=");
//			for(int j = 0; j < vecUser.size(); ++j)
//			{
//				pos = strstri(field, vecUser[j].data());
//				if(pos != NULL)
//				{
//					strncpy(buff, field, 256);
//					return;
//				};
//			};
//		};
//	}
//	else
//	{
//		for(int i = 0; i < inputVec.size(); ++i)
//		{
//			ZeroMemory(field, 256);
//			_getFormVal((char*)inputVec[i].data(), field, "name=");
//			for(int j = 0; j < vecPass.size(); ++j)
//			{
//				pos = strstri(field, vecPass[j].data());
//				if(pos != NULL)
//				{
//					strncpy(buff, field, 256);
//					return;
//				};
//			};
//		};
//	};
//}
//
//void _specWFBrute(const char *ip, int port, const char *buff, int flag, char *path, char *comment, char *tclass, char *cp, int size, char *title)
//{
//	if(strstr(buff, "VER_CODE") != NULL || strstri(buff, "captcha") != NULL)
//	{
//		if(gNegDebugMode)
//		{
//			stt->doEmitionDebugFoundData("[<a href=\"" + QString(ip) + ":" + QString::number(port) + "\"><font color=\"#0084ff\">" + QString(ip) + ":" + QString::number(port) + "</font></a>" + "] Ignoring: Captcha detected.");
//		};
//		return;
//    };
//
//	char methodVal[128] = {0};
//	char actionVal[512] = {0};
//	char userVal[128] = {0};
//	char passVal[128] = {0};
//	char frmBlock[4096] = {0};
//	char *fBlock = strstri(buff, "<form ");
//	char formVal[128] = {0};
//	int fbsz = 0;
//
//	std::vector<std::string> inputVec;
//	if(fBlock != NULL)
//	{
//		char *fBlock2 = strstri(fBlock, ">");
//		int szfb2 = fBlock2 - fBlock;
//		strncpy(formVal, fBlock, (szfb2 < 128 ? szfb2 : 128));
//		char *frmBlockEnd = strstri(fBlock, "</form>");
//		if(frmBlockEnd != NULL)
//		{
//			fbsz = frmBlockEnd - fBlock;
//			strncpy(frmBlock, fBlock, (fbsz < 4096 ? fbsz : 4096));
//		}
//		else
//		{
//			strncpy(frmBlock, fBlock, 4096);			
//		};
//
//		_getFormVal(frmBlock, methodVal, "method");
//		_getFormVal(frmBlock, actionVal, "action", path);
//		if(actionVal[0] == '.')
//		{
//			char tmpBuff[512] = {0};
//			char *tempPtr1 = _findLast(path, "/");
//			int sz = tempPtr1 - path;
//			if(sz > 0)
//			{
//				strncpy(tmpBuff, path, sz);
//				strncat(tmpBuff, actionVal + 1, strlen(actionVal) - 1);
//				ZeroMemory(actionVal, sizeof(actionVal));
//				strcpy(actionVal, tmpBuff);
//			};
//		};
//
//		char *inptPtr1 = strstri(frmBlock, "<input ");
//		int insz = 0;
//		char *inptPtrEnd = NULL;
//		char tempInptStr[256] = {0};
//		while(inptPtr1 != NULL)
//		{
//			inptPtrEnd = strstr(inptPtr1, ">");
//			if(inptPtrEnd != NULL)
//			{
//				ZeroMemory(tempInptStr, 256);
//				insz = inptPtrEnd - inptPtr1 + 1;
//				strncpy(tempInptStr, inptPtr1, (insz < 256 ? insz : 256));
//				inputVec.push_back(std::string(tempInptStr));
//				inptPtr1 = strstri(inptPtrEnd, "<input ");
//			}
//			else break;
//		};
//
//		if(inputVec.size() != 0)
//		{
//			_getInputVal(inputVec, userVal, "USER");
//			_getInputVal(inputVec, passVal, "PASS");
//		}
//		else
//		{
//			if(gNegDebugMode) stt->doEmitionFoundData("<a href=\"" + QString(ip) + ":" + QString::number(port) + "\"><font color=\"#c3c3c3\">" + QString(ip) + ":" + QString::number(port) + "</font></a> - [WF]: No text/password fields found.");							
//            ///fillGlobalLogData(ip, tport, std::to_string(size).c_str(), title, "NULL", "NULL", comment, cp, tclass);
//            ///putInFile(flag, ip, tport, size, title, cp);
//		};
//	}
//	else
//	{
//		stt->doEmitionFoundData("<a href=\"" + QString(ip) + ":" + QString::number(port) + "\"><font color=\"#c3c3c3\">" + QString(ip) + ":" + QString::number(port) + "</font></a> - [WF]: Cannot find form block.");
//        fillGlobalLogData(ip, port, std::to_string(size).c_str(), title, "NULL", "NULL", comment, cp, tclass);
//        putInFile(flag, ip, port, size, title, cp);
//	};
//	
//	if(strlen(methodVal) == 0)
//	{
//		strcpy(methodVal, "GET");
//	};
//	if(strlen(actionVal) == 0)
//	{
//		strcpy(actionVal, "/");
//	}
//	else
//	{
//		if(strstri(actionVal, "http") != NULL)
//		{
//			char tmp[128] = {0};
//			strncpy(tmp, actionVal, 128);
//			if(strstr(tmp, "//") != NULL)
//			{
//				char *tmp1 = strstr(tmp, "//");
//				char *tmp2 = strstr(tmp1 + 2, "/");
//				ZeroMemory(actionVal, 128);
//				if(tmp2 != NULL)
//				{
//					strncpy(actionVal, tmp2, strlen(tmp2));
//				}
//				else
//				{
//					strcpy(actionVal, "/");				
//				};
//			}
//			else if(strstr(tmp, "%2f%2f") != NULL)
//			{
//				char *tmp1 = strstr(tmp, "%2f%2f");
//				char *tmp2 = strstr(tmp1 + 6, "%2f");
//				ZeroMemory(actionVal, 128);
//				if(tmp2 != NULL)
//				{
//					strcpy(actionVal, "/");
//					strncpy(actionVal, tmp2 + 3, strlen(tmp2) - 3);
//				}
//				else
//				{
//					strcpy(actionVal, "/");				
//				};
//			};			
//		};
//		if(actionVal[0] != '/')
//		{
//			char temp[128] = {0};
//			strncpy(temp, actionVal, 128);
//			strcpy(actionVal, "/");
//			strncat(actionVal, temp, strlen(temp));
//		};
//	};
//
//	if(inputVec.size() > 0)
//	{
//		if(strlen(userVal) != 0 && strlen(passVal) != 0)
//        {
//            WFClass WFC;
//            lopaStr lps = WFC._WFBrute(ip, port, methodVal, actionVal, userVal, passVal, formVal);
//
//			if(strstr(lps.login, "UNKNOWN") == NULL && strlen(lps.other) == 0) 
//			{
//                _specFillerWF(ip, port, title, lps.login, lps.pass, flag);
//		
//                fillGlobalLogData(ip, port, std::to_string(size).c_str(), title, lps.login, lps.pass, comment, cp, tclass);
//                putInFile(flag, ip, port, size, title, cp);
//			};
//		}
//		else
//		{
//            if(gNegDebugMode) stt->doEmitionFoundData("<a href=\"" + QString(ip) + ":" + QString::number(port) +
//                                                      "\"><font color=\"#c3c3c3\">" + QString(ip) + ":" + QString::number(port) +
//                                                      "</font></a> - [WF]: Cannot find user/pass field.");
//		};
//    };
//}

void _specWEBIPCAMBrute(const char *ip, int port, char *finalstr, int flag, char *comment, char *cp, int size, char *SPEC, std::string *cookie)
{
	IPC ipc;
    lopaStr lps = ipc.IPCLobby(ip, port, SPEC, cookie);

	if(strstr(lps.login, "UNKNOWN") == NULL && strlen(lps.other) == 0) 
	{
		_specFillerCustom(ip, port, finalstr, lps.login, lps.pass, flag, "[WIC]");

		//fillGlobalLogData(ip, port, std::to_string(size).c_str(), finalstr, lps.login, lps.pass, comment, cp, "Basic Authorization");
	};
}

int _specBrute(const char *ip, int port,
               const char *finalstr, int flag,
                char *path, char *comment, char *cp, int size, bool performDoubleCheck = false, bool isBA = false)
{
	const lopaStr &lps = BA::BALobby((string(ip) + string(path)).c_str(), port, performDoubleCheck);
	
	if (strcmp(lps.other, "404") == 0) {

		stt->doEmitionRedFoundData("BA - 404 <a style=\"color:#717171;\" href=\"" + QString(ip) + QString(path) + "/\">" + QString(ip) + QString(path) + "</a>");
		return -1;
	}

	if(strstr(lps.login, "UNKNOWN") == NULL && strlen(lps.other) == 0) 
	{
		if (isBA) {
			_specFillerBA(ip, port, finalstr, lps.login, lps.pass, flag);
		}
		else {
			_specFillerCustom(ip, port, finalstr, lps.login, lps.pass, flag, "[WF]");
		}
		return 0;
        //fillGlobalLogData(ip, port, std::to_string(size).c_str(), finalstr, lps.login, lps.pass, "", cp, "Basic Authorization");
	};

	return 1;
}

int _specRTSPBrute(const char *ip, int port,
	const char *finalstr, int flag,
	char *comment, char *cp, int size)
{
	const lopaStr &lps = RTSP::RTSPLobby(ip, port);

	if (strcmp(lps.other, "404") == 0) {

		stt->doEmitionFoundDataCustom("RSTP - 404 <a style=\"color:#717171;\" href=\"" + QString(ip) + "/\">" + QString(ip) + "</a>", "FF69B4");
		return -1;
	}

	if (strstr(lps.login, "UNKNOWN") == NULL)
	{
		char title[512] = { 0 };
		sprintf(title, "%s %s", lps.other, finalstr);
		_specFillerRSTP(ip, port, title, lps.login, lps.pass, flag);
		//fillGlobalLogData(ip, port, std::to_string(size).c_str(), finalstr, lps.login, lps.pass, "", cp, "RTSP");
	};
}

const char *GetTitle(const char* str)
{
	char delimiterT[] = "<title id=titletext>";
	char delimiterT2[] = "<title id=\"title\">";
	const char *firstStr, *secondStr;
	char finalstr[512] = { 0 };
	
	if (strstri(str, "realm") != NULL)	
	{
		if (strstr(str, "\"") != NULL) 
		{
			int hm;
			firstStr = strstr(str, "\"");
			if(strstr((firstStr+1), "\"") != NULL) 
			{
				secondStr = strstr((firstStr+1), "\"");
				hm = (int)(secondStr-firstStr);
			}
			else hm = 10;
			if(hm > 127) hm = 20;
			strncat(finalstr, firstStr, hm+1);
		};
	};

	if(strlen(finalstr) != 0) strcat(finalstr, "::");

	if(strstri(str, "<card") != NULL)
	{
		char *str1 = strstri(str, "<card");
		if(strstri(str1, "title=") != NULL)
		{
			char *str2 = strstri(str1, "title=");
			if(strstri(str2, ">") != NULL)
			{
				char *str3 = strstri(str2, ">");

				int y = str3 - str2;
				if(y > 256)
				{ 
					strcpy(finalstr, "[Strange title]");
				}
				else
				{
					strncat(finalstr, (char*)(str2 + strlen("title=")), y);
					strcat(finalstr, " += ");
				};
			};
		};
	};

	if(strstri(str, "<title>") != NULL) 
	{  
		if(strstri(str, "<title>") != NULL) firstStr = strstri(str, "<title>");
		if(strstri(firstStr, "</title>") != NULL) secondStr = strstri(firstStr, "</title>");
		else 
		{
			strcat(finalstr, "[Corrupted title]");
			return finalstr;
		};
		int hm = (int)(secondStr - firstStr);
		if(hm > 256) hm = 20;
		strncat(finalstr, firstStr + 7, hm - 7);

		if(strstri(finalstr, "index of /") != NULL)
		{
			int hm = 0;
			strcat(finalstr, " (");
			if(strstri(firstStr, "description") != NULL) firstStr = strstri(firstStr, "description");
			if(strstri(firstStr, "<a href=") != NULL) firstStr = strstri(firstStr, "<a href=");
			else firstStr = NULL;
			int iterCount = 0;
			while(firstStr != NULL && strstr(firstStr , "\">") != NULL && strlen(finalstr) < 480) 
			{
				if(iterCount++ > 4 || strlen(finalstr) > 300) break;
				if(strstr(firstStr, "\">") != NULL) firstStr = strstr(firstStr, "\">");
				else break;
				secondStr = strstri(firstStr, "</a>");
				
				hm = (int)(secondStr-firstStr);
				if(hm > 16) hm = 16;

				strncat(finalstr, firstStr + 2, hm - 2);
				strcat(finalstr, " ");
				if(strstri(firstStr, "<a href=") != NULL) firstStr = strstri(firstStr, "<a href=");
				else break;
			};

			strcat(finalstr, ");");
		};
	};

	if(strstri(str, delimiterT2) != NULL) 
	{ 
		firstStr = strstri(str, delimiterT2);
		if(strstri(firstStr, "</title>") != NULL) secondStr = strstri(firstStr, "</title>");
		else
		{
			strcpy(finalstr, "[Corrupted title]");
			return finalstr;
		};
		int hm = (int)(secondStr-firstStr);
		if(hm > 127) hm = 30;
		strncat(finalstr, firstStr+18, hm-18);
	}
	else if(strstri(str, delimiterT) != NULL) 
	{  
		firstStr = strstri(str, delimiterT);
		if(strstri(firstStr, "</title>") != NULL) secondStr = strstri(firstStr, "</title>");
		int hm = (int)(secondStr-firstStr);
		if(hm > 127) hm = 30;
		strncat(finalstr, firstStr+20, hm-20);
	};

	return finalstr;
}

void _saveSSH(const char *ip, int port, int size, const char *buffcpy)
{
	if(buffcpy != NULL)
    {
		char log[2048] = {0};
		char logEmit[2048] = {0};
		char goodStr[256] = {0};
		char banner[256] = {0};

		const char *ptr1 = strstr(buffcpy, "|+|");
		if(ptr1 != NULL)
		{
			int gsz = ptr1 - buffcpy;
			strncpy(goodStr, buffcpy, gsz);
			if(strlen(ptr1 + 3) > 0) strcpy(banner, ptr1 + 3);
            sprintf(log, "[SSH] <font color=\"#00a8ff\"> %s:%d </font><font color=\"#323232\">; Banner:</font> <font color=\"#9cff00\"> %s </font>", goodStr, port, banner);
            sprintf(logEmit, "[SSH] <span style=\"color: #00a8ff;\"> %s:%d </span>", goodStr, port);

			++PieSSH;

            fputsf (log, -22);
			char loginSSH[128] = {0};
			char passSSH[128] = {0};
			const char *ptrl1 = strstr(buffcpy, ":");
			int lpsz = ptrl1 - buffcpy;
			strncpy(loginSSH, buffcpy, lpsz);
			const char *ptrl2 = strstr(buffcpy, "@");
			lpsz = ptrl2 - ptrl1;
			strncpy(passSSH, ptrl1 + 1, lpsz);
			fillGlobalLogData(ip, port, std::to_string(size).c_str(), "[SSH service]", loginSSH, passSSH, "NULL", "UTF-8", "SSH");
			stt->doEmitionFoundData(QString::fromLocal8Bit(logEmit));
		}
		else
		{
			stt->doEmitionRedFoundData("[_saveSSH] Wrong format! [" + QString(ip) + "]");
		};
	}
	else
	{
		stt->doEmitionRedFoundData("[_saveSSH] Empty buffer! [" + QString(ip) + "]");
	};
}

//int redirectReconnect(char *ip, int port, char *str, Lexems *ls, PathStr *ps, std::vector<std::string> *redirStrLst)
//{
//	if(ls->iterationCount++ == 5)
//	{	
//		ls->iterationCount = 0;
//
//		strcpy(ps->headr, "[!][Loop detected.]");
//		strcpy(ps->path, "");
//
//		return 0;
//	};
//
//	char tempIP[MAX_ADDR_LEN] = {0};
//	strcpy(tempIP, ip);
//	int tempPort = port;
//    char tempPath[1024] = {0};
//
//	if(strstri(str, "https://") != NULL)
//	{
//		tempPort = 443;
//		char *ptr1 = strstri(str, "https://");
//        char *ptr2 = _findFirst(ptr1 + 8, ":/?");
//
//		if(ptr2 != NULL)
//		{
//			int sz = ptr2 - ptr1 - 8;
//			ZeroMemory(tempIP, MAX_ADDR_LEN);
//			strncpy(tempIP, ptr1 + 8, sz < 128 ? sz : 128);
//			if(ptr2[0] == ':')
//			{
//				char *ptrPath = strstr(ptr2, "/");
//				if(ptrPath != NULL)
//				{
//					sz = ptrPath - ptr2 - 1;
//
//					char *pPth = strstr(ptr1 + 8, "/");
//					strcpy(tempPath, pPth);
//				}
//				else
//				{
//					strcpy(tempPath, "/");
//					sz = ptr2 - ptr1 - 9;
//				};
//				char tPort[8] = {0};
//				strncpy(tPort, ptr2 + 1, sz < 8 ? sz : 5);
//				tempPort = atoi(tPort);
//			}
//			else if(ptr2[0] == '/')
//			{
//				strncpy(tempPath, ptr2, strlen(ptr2));
//			}
//			else if(ptr2[0] == '?')
//			{
//				strcpy(tempPath, "/");
//				strncat(tempPath, ptr2, strlen(ptr2));
//			}
//			else
//			{
//				stt->doEmitionRedFoundData("[Redirect] Unknown protocol (" + QString(ip) + ":" + QString::number(port) + ")");
//			};
//		}
//		else
//		{
//			ZeroMemory(tempIP, MAX_ADDR_LEN);
//			strncpy(tempIP, ptr1 + 8, strlen(str) - 8);
//			strcpy(tempPath, "/");
//		};
//
//        std::unique_ptr<char[]> nip(new char[strlen(tempIP) + strlen(tempPath) + 1]);
//        sprintf(nip.get(), "%s%s", tempIP, tempPath);
//        std::string buffer;
//		Connector con;
//		int cSz = con.nConnect(nip.get(), tempPort, &buffer);
//
//        if(cSz > -1)
//        {
//            strcpy(ps->codepage, GetCodePage(buffer.c_str()));
//
//            ls->flag = ContentFilter(&buffer, tempPort, tempIP, ps->codepage, cSz);
//			ps->flag = ls->flag;
//
//			if(ls->flag == -1) 
//			{
//				ps->flag = -1;
//                strcpy(ps->path, tempPath);
//
//				return -1;
//			};
//
//			if(ls->flag >= 17 || ls->flag == 11 || ls->flag == 12 
//				|| ls->flag == 13 || ls->flag == 14 || ls->flag == 1 || ls->flag == 10) 
//			{
//                strcat(ps->headr, GetTitle(buffer.c_str()));
//				ps->flag = ls->flag;
//				strcpy(ps->path, tempPath);
//				ps->port = tempPort;
//				strcpy(ps->ip, tempIP);
//
//				return -2;
//			};
//
//			strcat(ps->headr, " -> ");
//            strcat(ps->headr, GetTitle(buffer.c_str()));
//
//            if (ls->header(tempIP, tempPort, buffer.c_str(), ls, ps, redirStrLst, cSz) == -1)
//			{
//				ps->flag = -1;
//                strcpy(ps->path, tempPath);
//
//				return -1;
//			};
//
//            ps->port = tempPort;
//		}
//		else
//		{
//			ps->flag = -1;
//			ls->flag = -1;
//			if(gNegDebugMode) stt->doEmitionDebugFoundData("[<a href=\"" + QString(ip) + ":" + QString::number(port) 
//				+ "/\"><font color=\"#0084ff\">" + QString(ip) + ":" + QString::number(port) + "</font></a>" 
//				+ "] Rejecting in _header::redirect [Dead host].");
//		};
//		
//		return -2;
//	}
//	else if(strstr(str, "http://") != NULL) //http
//	{
//		tempPort = 80;
//		char *ptr1 = strstri(str, "http://");
//        char *ptr2 = _findFirst(ptr1 + 7, ":/?");
//
//		if(ptr2 != NULL)
//		{
//			int sz = ptr2 - ptr1 - 7;
//			ZeroMemory(tempIP, MAX_ADDR_LEN);
//			strncpy(tempIP, ptr1 + 7, sz < 128 ? sz : 128);
//
//			if(ptr2[0] == ':')
//			{
//				char *ptrPath = strstr(ptr2, "/");
//
//				if(ptrPath != NULL)
//				{
//					sz = ptrPath - ptr2 - 1;
//
//					char *pPth = strstr(ptr1 + 7, "/");
//					strcpy(tempPath, pPth);
//				}
//				else
//				{
//					strcpy(tempPath, "/");
//					sz = ptr2 - ptr1 - 7;
//				};
//
//				char tPort[8] = {0};
//				strncpy(tPort, ptr2 + 1, sz < 8 ? sz : 5);
//				tempPort = atoi(tPort);
//			}
//			else if(ptr2[0] == '/')
//			{
//				strncpy(tempPath, ptr2, strlen(ptr2));
//			}
//			else if(ptr2[0] == '?')
//			{
//				strcpy(tempPath, "/");
//				strncat(tempPath, ptr2, strlen(ptr2));
//			}
//			else
//			{
//				stt->doEmitionRedFoundData("[Redirect] Unknown protocol (" + QString(ip) + ":" + QString::number(port) + ")");
//			};
//		}
//		else
//		{
//			ZeroMemory(tempIP, MAX_ADDR_LEN);
//			strncpy(tempIP, ptr1 + 7, strlen(str) - 7);
//			strcpy(tempPath, "/");
//		};
//
//        std::unique_ptr<char[]> nip(new char[strlen(tempIP) + strlen(tempPath) + 1]);
//        sprintf(nip.get(), "%s%s", tempIP, tempPath);
//		std::string buffer;
//		Connector con;
//        int cSz = con.nConnect(nip.get(), tempPort, &buffer);
//
//        if(cSz > -1)
//        {
//            strcpy(ps->codepage, GetCodePage(buffer.c_str()));
//
//            ls->flag = ContentFilter(&buffer, tempPort, tempIP, ps->codepage, cSz);
//			ps->flag = ls->flag;
//
//			if(ls->flag == -1) 
//			{
//				ps->flag = -1;
//                strcpy(ps->path, tempPath);
//
//				return -1;
//			};
//
//			if(ls->flag >= 17 || ls->flag == 11 || ls->flag == 12 
//				|| ls->flag == 13 || ls->flag == 14 || ls->flag == 1 || ls->flag == 10) 
//			{
//                strcat(ps->headr, GetTitle(buffer.c_str()));
//				ps->flag = ls->flag;
//                strcpy(ps->path, tempPath);
//				ps->port = tempPort;
//				strcpy(ps->ip, tempIP);
//
//				return -2;
//			};
//			
//			strcat(ps->headr, " -> ");
//            strcat(ps->headr, GetTitle(buffer.c_str()));
//
//            if (ls->header(tempIP, tempPort, buffer.c_str(), ls, ps, redirStrLst, cSz) == -1)
//			{
//				ps->flag = -1;
//                strcpy(ps->path, tempPath);
//
//				return -1;
//			};
//			ps->port = tempPort;
//		}
//		else
//		{
//			ps->flag = -1;
//			ls->flag = -1;
//			if(gNegDebugMode) stt->doEmitionDebugFoundData("[<a href=\"" + QString(ip) + ":" + QString::number(port) + 
//				"/\"><font color=\"#0084ff\">" + QString(ip) + ":" + QString::number(port) + "</font></a>" + 
//				"] Rejecting in _header::redirect [Dead host].");
//		};
//
//		return -2;
//	}
//	else if(str[0] == '/' || (str[0] == '.' && str[1] == '/') || (str[0] == '.' && str[1] == '.' && str[2] == '/'))
//	{
//		if(str[0] == '.' && str[1] == '.') strcpy(tempPath, str + 2);
//		else if(str[0] == '.') strcpy(tempPath, str + 1);
//		else strcpy(tempPath, str);
//
//        std::unique_ptr<char[]> nip(new char[strlen(tempIP) + strlen(tempPath) + 1]);
//        sprintf(nip.get(), "%s%s", tempIP, tempPath);
//		std::string buffer;
//		Connector con;
//        int cSz = con.nConnect(nip.get(), tempPort, &buffer);
//
//        if(cSz > -1)
//        {
//            strcpy(ps->codepage, GetCodePage(buffer.c_str()));
//
//            ls->flag = ContentFilter(&buffer, port, ip, ps->codepage, cSz);
//			ps->flag = ls->flag;
//
//			if(ls->flag == -1) 
//			{
//				ps->flag = -1;
//                strcpy(ps->path, tempPath);
//
//				return -2;
//			};
//
//			if(ls->flag >= 17 || ls->flag == 11 || ls->flag == 12 
//				|| ls->flag == 13 || ls->flag == 14 || ls->flag == 1 || ls->flag == 10) 
//			{
//                strcat(ps->headr, GetTitle(buffer.c_str()));
//				ps->flag = ls->flag;
//                strcpy(ps->path, tempPath);
//				ps->port = port;
//				strcpy(ps->ip, ip);
//
//				return -2;
//			};
//
//			strcat(ps->headr, "->");
//            strcat(ps->headr, GetTitle(buffer.c_str()));
//
//            if (ls->header(tempIP, tempPort, buffer.c_str(), ls, ps, redirStrLst, cSz) == -1)
//			{
//				ps->flag = -1;
//                strcpy(ps->path, tempPath);
//
//				return -1;
//			};
//			ps->port = tempPort;
//		}
//		else
//		{
//			ps->flag = -1;
//			ls->flag = -1;
//			if(gNegDebugMode) stt->doEmitionDebugFoundData("[<a href=\"" + QString(ip) + ":" + QString::number(port) + 
//				"/\"><font color=\"#0084ff\">" + QString(ip) + ":" + QString::number(port) + "</font></a>" + 
//				"] Rejecting in _header::redirect [Dead host].");
//		};
//		return -2;
//	}
//	else if(strlen(str) > 2)
//    {
//        std::unique_ptr<char[]> nip(new char[strlen(ip) + strlen(str) + 1]);
//        sprintf(nip.get(), "%s%s", ip, str);
//		std::string buffer;
//		Connector con;
//        int cSz = con.nConnect(nip.get(), port, &buffer);
//
//        if(cSz > -1)
//        {
//            strcpy(ps->codepage, GetCodePage(buffer.c_str()));
//
//            ls->flag = ContentFilter(&buffer, port, ip, ps->codepage, cSz);
//			ps->flag = ls->flag;
//
//			if(ls->flag == -1) 
//			{
//				ps->flag = -1;
//                strcpy(ps->path, tempPath);
//
//				return -1;
//			};
//
//			if(ls->flag >= 17 || ls->flag == 11 || ls->flag == 12 
//				|| ls->flag == 13 || ls->flag == 14 || ls->flag == 1 || ls->flag == 10) 
//			{
//                strcat(ps->headr, GetTitle(buffer.c_str()));
//				ps->flag = ls->flag;
//                strcpy(ps->path, tempPath);
//				ps->port = port;
//				strcpy(ps->ip, ip);
//
//				return -2;
//			};
//
//			strcat(ps->headr, " -> ");
//            strcat(ps->headr, GetTitle(buffer.c_str()));
//            ls->header(ip, port, buffer.c_str(), ls, ps, redirStrLst, cSz);
//			ps->port = tempPort;
//		}
//		else
//		{
//			ps->flag = -1;
//			ls->flag = -1;
//			if(gNegDebugMode) stt->doEmitionDebugFoundData("[<a href=\"" + QString(ip) + ":" + QString::number(port) + 
//				"/\"><font color=\"#0084ff\">" + QString(ip) + ":" + QString::number(port) + "</font></a>" + 
//				"] Rejecting in _header::redirect [Dead host].");
//		};
//		return -2;
//	};
//		
//	return -1;
//}

void _getPopupTitle(PathStr *ps, char *str)
{
	strcat(ps->headr, "[Popup. Title: "); 				

	char *ptr1 = strstr(str, ",");
	if(ptr1 != NULL)
	{
		char *ptr2 = strstr(ptr1 + 1, ",");
		if(ptr2 != NULL)
		{
			int sz = ptr2 - ptr1 - 1;
			if(sz >= 32) sz = 32;
			
			strncat(ps->headr, ptr1 + 1, sz < 32 ? sz : 32);
		}
		else
		{
			strcat(ps->headr, "[BOUNDARY ERROR]");
		};
	}
	else
	{
		char temp[32] = {0};
		if(strstr(str, "(") != NULL){
			strncpy(temp, strstr(str, "("), 32);
			strcat(ps->headr, temp);
		};
	};

	strcat(ps->headr, "]"); 
}

void _getLinkFromJSLocation(char *dataBuff, char *str, char *tag, char *ip, int port)
{
	if (strstri(str, ".title") != NULL) return;
	char *ptr1 = strstr(str, tag);
	if(ptr1 != NULL)
	{
		char *ptr2 = _findFirst(ptr1, "=(");
		char *ptrSemi = _findFirst(ptr1 + strlen(tag), ".;");
		if(ptrSemi == NULL)
		{
			ptrSemi = _findLast(ptr1 + strlen(tag) + 1, "'\"");
		}
		if(ptr2 != NULL && ptrSemi != NULL)
		{
			int sz = ptrSemi - ptr2;
			if(sz >= 2)
			{
				char *ptrQuote1 = _findFirst(ptr2, "\"'");
				if(ptrQuote1 != NULL)
				{
					char *ptrQuoteTemp = _findFirst(ptrQuote1 + 1, ";\n}");
					if(ptrQuoteTemp != NULL)
					{
						sz = ptrQuoteTemp - ptrQuote1 + 1;
					}
					else
					{
						ptrQuoteTemp = _findFirst(ptrQuote1 + 1, "\"'");
						sz = ptrQuoteTemp - ptrQuote1 + 1;
					}
					char *tempBuff = new char[sz + 1];
                    //ZeroMemory(tempBuff, sizeof(*tempBuff));
					tempBuff[0] = 0;
					strncpy(tempBuff, ptrQuote1 + 1, sz);
					memset(tempBuff + sz, 0, 1);
					char delim[2] = {0};
					delim[0] = ptrQuote1[0];
					delim[1] = '\0';

					char *ptrQuote2 = _findLast(tempBuff + 1, delim);
					if(ptrQuote2 != NULL)
					{
                        sz = ptrQuote2 - tempBuff;
						if(sz < 511)
						{
							if (tempBuff[0] == '.' && tempBuff[1] == '/')
							{
								strncat(dataBuff, tempBuff + 1, sz - 1);
							}
							else if(tempBuff[0] != '/' 
								&& strstri(tempBuff, "http://") == NULL 
								&& strstri(tempBuff, "https://") == NULL
								) 
							{
								strcpy(dataBuff, "/");
								strncat(dataBuff, tempBuff, sz);
							}
							else strncpy(dataBuff, tempBuff, sz);
						};
					};
					delete tempBuff;
				}
				else
				{
					ptrQuote1 = strstr(ptr2, "=");
					if(ptrQuote1 != NULL)
					{
						char *ptrQuote2 = _findFirst(ptr2, ";\n");
						if(ptrQuote2 != NULL)
						{
							int sz = ptrQuote2 - ptr2 - 1;
							char link1[512] = {0};
							strncpy(link1, ptr2 + 1, sz);
							char *ptrQuote3 = strstr(link1, "/");
							if(ptrQuote3 != NULL)
                            {
								strcpy(dataBuff, ptrQuote3);
							};
						};
					};
				};
			};
		}
		else
		{
			stt->doEmitionRedFoundData("[JSLocator] Location extraction failed [<a href=\"" + QString(ip) + "/\">" + QString(ip) + "</a>]");
		};
	};
}
//
//int Lexems::getHeader(char *ip, int port, const char *str, Lexems *l, PathStr *ps, std::vector<std::string> *redirStrLst, int size)
//{
//	std::string redirectStr = "";
//
//    strcpy(ps->codepage, GetCodePage(str));
//    char finalstr[512] = {0};
//	
//	if(strstri(str, "notice auth :*** looking up your hostname...") 
//		|| strstri(str, "451 * :You have not registered.")
//		) 
//	{ 
//		strcpy(ps->headr, "[IRC server]"); 
//		strcpy(ps->path, "/");  return 1; 
//	};
//
//	if((strstri(str, "ip camera") != NULL				|| strstr(str, "+tm01+") != NULL 
//		|| strstri(str, "camera web server") != NULL	|| strstri(str, "ipcam_language") != NULL
//		|| strstri(str, "/viewer/video.jpg") != NULL	|| strstri(str, "network camera") != NULL
//		|| strstri(str, "sanpshot_icon") != NULL		|| strstri(str, "snapshot_icon") != NULL
//		|| strstri(str, "lan camera") != NULL			|| strstri(str, "cgiuserlogin?") != NULL
//		|| strstri(str, "web camera") != NULL			|| strstri(str, "smart ip device") != NULL
//		|| strstri(str, "pan/tilt camera") != NULL		|| strstri(str, "/cgi-bin/viewer/getparam.cgi?") != NULL
//		|| strstri(str, "IPCam") != NULL				|| strstri(str, "/camera-cgi/admin") != NULL
//		) && strstr(str, "customer") == NULL
//		&& strstr(str, "purchase") == NULL
//		&& strstr(str, "contac") == NULL
//		&& strstr(str, "company") == NULL
//		) 
//	{
//		if (strstr(str, "CgiStart?page=Single") != NULL) strcpy(ps->headr, "[IP Camera (Unibrowser)]");
//		else strcpy(ps->headr, "[IP Camera]");
//		l->flag = 0;
//		ps->flag = 0;
//	};
//
//	if(strstri(str, "get_status.cgi") != NULL)			strcpy(ps->headr, "[It may be ip camera]"); 
//	if(strstri(str, "vo_logo.gif") != NULL 
//		|| strstri(str, "vo logo.gif") != NULL
//		)												strcpy(ps->headr, "[VIVOTEK camera detected?]"); 
//	
//	if(strstri(str, "$lock extended") != NULL) 
//	{ 
//		strcpy(ps->headr, "[DChub detected.]"); 
//		strcpy(ps->path, "/");  
//		return 0; 
//	};
//	if(strstri(str, "top.htm?currenttime") != NULL 
//		|| strstri(str, "top.htm?") != NULL
//		)												strcat(finalstr, " [?][SecCam detected]");
//
//    if(strstri(str, "http-equiv=\"refresh\"") != NULL
//        || strstri(str, "http-equiv=refresh") != NULL
//        || strstri(str, "http-equiv='refresh'") != NULL
//        )
//    {
//        char *temp = NULL;
//        char *strTmp = NULL;
//
//        if(strstri(str, "http-equiv=\"refresh\"") != NULL) strTmp = strstri(str, "http-equiv=\"refresh\"");
//        else if(strstri(str, "http-equiv=refresh") != NULL) strTmp = strstri(str, "http-equiv=refresh");
//        else if(strstri(str, "http-equiv='refresh'") != NULL) strTmp = strstri(str, "http-equiv='refresh'");
//
//        if(strstri(strTmp, "url=") != NULL )
//        {
//            if((int)(strstri(strTmp, "url=") - strTmp) < 100)
//            {
//                temp = strstri(strTmp, "url=");
//
//                char *temp2 = NULL, temp3[128] = {0};
//                int sz = 0;
//
//                if(temp[4] == '"' || temp[4] == '\'' || temp[4] == ' ' || temp[4] == '\n' || temp[4] == '\r')
//                {
//                    temp2 = _findFirst(temp + 6, " \n>\"'");
//                    if(temp2 != NULL)
//                    {
//                        sz = (int)(temp2 - temp) - 5;
//                        strncpy(temp3, (char*)(temp + 5), (sz < 128 ? sz : 127));
//                    };
//                }
//                else
//                {
//                    temp2 = _findFirst(temp + 4, " \n>\"'");
//                    if(temp2 != NULL)
//                    {
//                        sz = (int)(temp2 - temp) - 4;
//                        strncpy(temp3, (char*)(temp + 4), sz < 128 ? sz : 127);
//                    };
//                };
//
//                if(strstri(temp3, "http://") == NULL && strstri(temp3, "https://") == NULL)
//                {
//                    if(temp3[0] != '.')
//                    {
//                        if(temp3[0] != '/')
//                        {
//                            char temp4[128] = {0};
//                            strcpy(temp4, "/");
//                            strncat(temp4, temp3, 127);
//                            strncpy(temp3, temp4, 128);
//                        };
//                    };
//                };
//
//                redirectStr = std::string(temp3);
//                if(std::find(redirStrLst->begin(), redirStrLst->end(), redirectStr) == redirStrLst->end())
//                {
//                    redirStrLst->push_back(redirectStr);
//                    return redirectReconnect(ip, port, temp3, l, ps, redirStrLst);
//                } return -1;
//                strcat(ps->headr, " ");
//                return -2;
//            };
//
//            strcat(ps->headr, finalstr);
//            strcat(ps->headr, " ");
//            return 0;
//        };
//    };
//
//	if(strstri(str, "<script") != NULL)
//    {
//		char *ptr1 = strstri(str, "<script");
//		char *ptr2 = NULL;
//		char linkPtr[512] = {0};
//
//		do
//		{
//			ZeroMemory(linkPtr, 512);
//			ptr2 = strstri(ptr1, "</script>");
//			if(ptr2 != NULL)
//			{
//				int sz = ptr2 - ptr1;
//				char *scriptContainer = new char[sz + 1];
//				ZeroMemory(scriptContainer, sz + 1);
//				strncpy(scriptContainer, ptr1, sz);
//				memset(scriptContainer + sz, '\0', 1);
//				
//				ZeroMemory(linkPtr, 512);
//				if(strstri(scriptContainer, "location.href") != NULL)			_getLinkFromJSLocation(linkPtr, scriptContainer, "location.href", ip, port);
//				else if(strstri(scriptContainer, "location.replace") != NULL)	_getLinkFromJSLocation(linkPtr, scriptContainer, "location.replace", ip, port);
//				else if(strstri(scriptContainer, "location.reload") != NULL)	strcpy(linkPtr, "/");
//				else if(strstri(scriptContainer, "location") != NULL)			_getLinkFromJSLocation(linkPtr, scriptContainer, "location", ip, port);
//				
//				if(strlen(linkPtr) != 0)
//				{
//					redirectStr = std::string(linkPtr);
//					if(std::find(redirStrLst->begin(), redirStrLst->end(), redirectStr) == redirStrLst->end()) 
//					{
//						redirStrLst->push_back(redirectStr);
//                        redirectReconnect(ip, port, linkPtr, l, ps, redirStrLst);
//					};
//				};
//				delete []scriptContainer;
//				if(ps->flag >= 17 || ps->flag == 11 || ps->flag == 12 
//				|| ps->flag == 13 || ps->flag == 14 || ps->flag == 1 
//				|| ps->flag == 10
//				) 
//				return -2;
//				else if(ps->flag == -1) return -1;
//			}
//			else
//			{
//				strcat(ps->headr, "[Cannot retrieve \"<script>\"-block]"); 
//				strcat(ps->headr, " ");
//				break;
//			};
//			ptr1 = strstri(ptr2, "<script");
//		}
//		while(ptr1 != NULL);
//	}
//	
//	if(strstri(str, " onload") != NULL)
//	{
//		char *ptr1 = strstri(str, " onload");
//		char *ptr2 = strstr(ptr1, ">");
//		if(ptr2 != NULL)
//		{
//			int sz = ptr2 - ptr1;
//			if(sz < 512)
//			{
//				char linkPtr[512] = {0};
//				ZeroMemory(linkPtr, 512);
//				strncpy(linkPtr, ptr1, sz);
//				char *scriptContainer = new char[sz + 1];
//				ZeroMemory(scriptContainer, sz + 1);
//				strncpy(scriptContainer, ptr1, sz);
//				memset(scriptContainer + sz, '\0', 1);
//				
//				ZeroMemory(linkPtr, 512);
//				if(strstri(scriptContainer, "location.href") != NULL)			_getLinkFromJSLocation(linkPtr, scriptContainer, "location.href", ip, port);
//				else if(strstri(scriptContainer, "location.replace") != NULL)	_getLinkFromJSLocation(linkPtr, scriptContainer, "location.replace", ip, port);
//				else if(strstri(scriptContainer, "location.reload") != NULL)	strcpy(linkPtr, "/");
//				else if(strstri(scriptContainer, "location") != NULL)			_getLinkFromJSLocation(linkPtr, scriptContainer, "location", ip, port);
//				
//				if(strlen(linkPtr) != 0)
//				{
//					redirectStr = std::string(linkPtr);
//					if(std::find(redirStrLst->begin(), redirStrLst->end(), redirectStr) == redirStrLst->end()) 
//					{
//						redirStrLst->push_back(redirectStr);
//                        return redirectReconnect(ip, port, linkPtr, l, ps, redirStrLst);
//					} return -1;
//				};
//				delete []scriptContainer;
//				if(ps->flag >= 17 || ps->flag == 11 || ps->flag == 12 
//				|| ps->flag == 13 || ps->flag == 14 || ps->flag == 1 
//				|| ps->flag == 10
//				) return -2;
//				else if(ps->flag == -1) return -1;
//			};
//		};
//	};
//
//	if(strstri(str, "ActiveXObject") != NULL 
//		|| strstri(str, ".cab") != NULL 
//		|| strstri(str, "clsid:") != NULL
//		) strcat(ps->headr, "[ActiveX]");
//
//	if(strstri(str, "<applet") != NULL 
//		&& strstri(str, ".jar") != NULL
//		) strcat(ps->headr, "[Java]");
//	if(strstri(str, "<script") != NULL) strcat(ps->headr, "[Javascript]");
//	if(strstri(str, "<video") != NULL) strcat(ps->headr, "[Video]");
//	
//	if(strstri(str, "<frameset") != NULL || strstri(str, "<frame") != NULL || strstri(str, "<iframe") != NULL)
//	{
//		const char *str1 = str;
//		char *str2 = NULL;
//        char lol[128] = {0};
//		int AreaLen = 0;
//		do
//		{
//			if(strstri(str1, "<frameset") != NULL) str1 = strstri(str1, "<frameset");
//			else if(strstri(str1, "<frame") != NULL) str1 = strstri(str1, "<frame");
//			else if(strstri(str1, "<iframe") != NULL) str1 = strstri(str1, "<iframe");
//			else break;
//
//			if(strstri(str1, "src=\"") != NULL) 
//			{
//				str1 = strstri(str1, "src=\""); 
//				AreaLen = 5;
//			}
//			else if(strstri(str1, "src='") != NULL) 
//			{
//				str1 = strstri(str1, "src='");
//				AreaLen = 5;
//			}
//			else if(strstri(str1, "src = \"") != NULL)
//			{
//				str1 = strstri(str1, "src = \"");
//				AreaLen = 7;
//			}
//			else if(strstri(str1, "src = '") != NULL) 
//			{
//				str1 = strstri(str1, "src = '");
//				AreaLen = 7;
//			}
//			else if(strstri(str1, "src=") != NULL) 
//			{
//				str1 = strstri(str1, "src=");
//				AreaLen = 4;
//			}
//			else if(strstri(str1, "src = ") != NULL) 
//			{
//				str1 = strstri(str1, "src = ");
//				AreaLen = 6;
//			}
//			else 
//			{
//				str1 = NULL;
//				AreaLen = 0;
//			};
//
//			if(str1 != NULL)
//			{
//				str2 = _findFirst(str1 + AreaLen, "'\">");
//				if(str2 != NULL)
//				{
//					char script[128] = {0};
//					int sz = (int)(str2 - str1) - AreaLen;
//					if((int)(str2 - str1) < 128) strncpy(script, str1 + AreaLen, sz);
//					if(strstri(script, "http://") == NULL && strstri(script, "https://") == NULL) 
//					{
//						strcpy(lol, "http://");
//						strcat(lol, ip);
//						strcat(lol, ":");
//						strcat(lol, std::to_string(port).c_str());
//						if(script[0] != '/') strcat(lol, "/");
//						strcat(lol, script);
//					}
//					else strcpy(lol, script);
//
//					int flag = 0;
//					if(sz > 0) 
//					{
//						if(script[0] != '#')
//						{
//							redirectStr = std::string(lol);
//							if(std::find(redirStrLst->begin(), redirStrLst->end(), redirectStr) == redirStrLst->end()) 
//							{
//								redirStrLst->push_back(redirectStr);
//                                return redirectReconnect(ip, port, lol, l, ps, redirStrLst);
//							};
//						}
//						else
//						{
//							strcat(ps->headr, "[Unknown frame: \"");
//							strcat(ps->headr, script);
//							strcat(ps->headr, "\"]");
//						};
//					}
//
//					flag = ps->flag;
//					if(flag == 1 || flag == 11 || flag == 12
//						|| flag == 13 || flag == 14 || flag >= 17 || flag == 10) return -2;
//					else if(ps->flag == -1) return -1;
//				}
//				else
//				{
//					stt->doEmitionRedFoundData("[FrameLocator] Corrupted tag. [" + QString(ip) + ":" + 
//						QString::number(port) + "]");
//				};
//			};
//		}
//		while(str1 != NULL);
//		return -2;
//	};
//
//	if (strstri(str, "<form ") != NULL) strcat(ps->headr, "[Form]");
//	//if(strstri(str, "<form ") != NULL) 
//	//{
//	//	strcat(ps->headr, " [Login form detected]");
//	//	char *ptr1 = strstri(str, "<form");
//	//	char *ptr2 = strstri(ptr1, "action");
//	//	if(ptr2 != NULL)
//	//	{
//	//		char *ptr3 = strstri(ptr2, "=");
//	//		if(ptr3 != NULL)
//	//		{
//	//			char *ptr4 = NULL;
//	//			char *ptrEnd = NULL;
//	//			int sz = 0;
//	//			char redirStr[512] = {0};
//	//			if(ptr3[1] == ' ' || ptr3[1] == '"' || ptr3[1] == '\"')
//	//			{
//	//				ptr4 = _findFirst(ptr3, " \"'\n\r");
//	//				if(ptr4 != NULL)
//	//				{
//	//					ptrEnd = _findFirst(ptr4 + 1, " \"'\n\r");
//	//					if(ptrEnd != NULL)
//	//					{
//	//						sz = ptrEnd - ptr4 - 1;
//	//						strncpy(redirStr, ptr4 + 1, sz < 512 ? sz : 512);
//	//					};
//	//				};
//	//			}
//	//			else 
//	//			{
//	//				ptrEnd = _findFirst(ptr3, " \"'\n\r");
//	//				if(ptrEnd != NULL)
//	//				{
//	//					sz = ptrEnd - ptr3 - 1;
//	//					strncpy(redirStr, ptr3 + 1, sz < 512 ? sz : 512);
//	//				};
//	//			};
//
//	//			if (redirStr[0] != '#') {
//	//				if (std::find(redirStrLst->begin(), redirStrLst->end(), redirStr) == redirStrLst->end())
//	//				{
//	//					redirStrLst->push_back(redirStr);
// //                       return redirectReconnect(ip, port, redirStr, l, ps, redirStrLst);
//	//				} return -1;
//	//			}
//	//			return -2;
//	//		};
//	//	}
//	//	else
//	//	{
//	//		strcat(ps->headr, " [Form action not found]");
//	//	};
//	//	return 0;
//	//};
//	
//	if(strlen(ps->headr) == 0)
//	{
//		const char *ptr1 = strstr(str, "\r\n\r\n");
//		if( ptr1 != NULL) 
//		{
//			if (strlen(ptr1) - 4 >= 15)
//			{
//				strcat(ps->headr, " [Data: ");
//
//				char *ptr2 = strstri(ptr1 + 4, "<body");
//				if (ptr2 != NULL) strncat(ps->headr, ptr2 + 5, 64);
//				else {
//					ptr2 = strstri(ptr1 + 4, "<html");
//					if (ptr2 != NULL) strncat(ps->headr, strstri(ptr1, "<html") + 5, 64);
//					else strncat(ps->headr, ptr1 + 4, 64);
//				};
//
//				strcat(ps->headr, "]"); 
//			}
//			else
//			{
//				if(gNegDebugMode) 
//				{
//					stt->doEmitionDebugFoundData("[<a href=\"" + QString(ip) + ":" + QString::number(port) + 
//						"/\"><font color=\"#0084ff\">" + QString(ip) + ":" + QString::number(port) + "</font></a>" + 
//						"] Rejecting in _header::Lowload_body (&lt;15b)");
//				};
//
//				++filtered;
//				strcpy(ps->path, "/"); 
//				return -1; 
//			};
//		}
//		else
//		{
//			strcat(ps->headr, " [Data:");
//			strncat(ps->headr, str, 256);
//			strcat(ps->headr, "]"); 
//		};
//	};
//
//	std::string nBuff(str);
//	ps->flag = ContentFilter(&nBuff, port, ip, ps->codepage, size);
//
//	if(strstri(str, "window.open(") != NULL) _getPopupTitle(ps, strstri(str, "window.open("));
//
//	strcpy(ps->path, "/");
//	return 0;
//}






static std::atomic<bool> hikkaStop(false);
static std::atomic<bool> rviStop(false);
std::string getTitle(const char *str, const int flag) {
	if (59 == flag) {
		std::string strHeader = std::string(str);
		int strStart = strHeader.find("ver: ");
		std::string strChunk = strHeader.substr(strStart + 5);
		std::string headerFinal = strChunk.substr(0, strChunk.find("\r"));
		return headerFinal;
	}
	else {
		const char *ptr1 = NULL, *secondStr = NULL;
		char finalstr[512] = { 0 };

		if (strstri(str, "realm") != NULL)
		{
			if ((ptr1 = strstr(str, "\"")) != NULL)
			{
				int hm;
				if (strstr((ptr1 + 1), "\"") != NULL)
				{
					secondStr = strstr((ptr1 + 1), "\"");
					hm = (int)(secondStr - ptr1);
				}
				else hm = 10;
				strncat(finalstr, ptr1, (hm > 127 ? 20 : hm) + 1);
			}
			else {
				strcat(finalstr, "Strange realm.");
			}
			strcat(finalstr, "::");
		};

		if ((ptr1 = strstri(str, "<title>")) != NULL)
		{
			if (strstri(ptr1, "</title>") != NULL) secondStr = strstri(ptr1, "</title>");
			else
			{
				strcat(finalstr, "[Corrupted title]");
				return finalstr;
			};
			int hm = (int)(secondStr - ptr1);
			strncat(finalstr, ptr1 + 7, (hm > 256 ? 20 : hm) - 7);

			if (strstri(finalstr, "index of /") != NULL)
			{
				int hm = 0;
				strcat(finalstr, " (");
				if (strstri(ptr1, "description") != NULL) ptr1 = strstri(ptr1, "description");
				if (strstri(ptr1, "<a href=") != NULL) ptr1 = strstri(ptr1, "<a href=");
				else ptr1 = NULL;
				int iterCount = 0;
				while (ptr1 != NULL && strstr(ptr1, "\">") != NULL && strlen(finalstr) < 480)
				{
					if (iterCount++ > 6 || strlen(finalstr) > 300) break;
					if (strstr(ptr1, "\">") != NULL) ptr1 = strstr(ptr1, "\">");
					else break;
					secondStr = strstri(ptr1, "</a>");

					hm = (int)(secondStr - ptr1);

					strncat(finalstr, ptr1 + 2, (hm > 16 ? 16 : hm) - 2);
					strcat(finalstr, " ");
					if (strstri(ptr1, "<a href=") != NULL) ptr1 = strstri(ptr1, "<a href=");
					else break;
				};

				strcat(finalstr, ");");
			};
		}
		else if ((ptr1 = strstri(str, "<h1>")) != NULL) {
			char *ptr2 = strstri(ptr1, "</h1>");
			int sz = ptr2 - ptr1;

			strncat(finalstr, ptr1 + 4, (sz > 64 ? 64 : sz) - 4);
		}
		else if ((ptr1 = strstri(str, "<title id=\"title\">")) != NULL)
		{
			if (strstri(ptr1, "</title>") != NULL) secondStr = strstri(ptr1, "</title>");
			else
			{
				strcpy(finalstr, "[Corrupted title]");
				return finalstr;
			};
			int hm = (int)(secondStr - ptr1);
			strncat(finalstr, ptr1 + 18, (hm > 127 ? 30 : hm) - 18);
		}
		else if ((ptr1 = strstri(str, "<title id=titletext>")) != NULL)
		{
			if (strstri(ptr1, "</title>") != NULL) secondStr = strstri(ptr1, "</title>");
			int hm = (int)(secondStr - ptr1);
			strncat(finalstr, ptr1 + 20, (hm > 127 ? 30 : hm) - 20);
		}
		else if ((ptr1 = strstri(str, "<card")) != NULL)
		{
			if (strstri(ptr1, "title=") != NULL)
			{
				char *str2 = strstri(ptr1, "title=");
				if (strstri(str2, ">") != NULL)
				{
					char *str3 = strstri(str2, ">");

					int y = str3 - str2;
					if (y > 256)
					{
						strcpy(finalstr, "[Strange title]");
					}
					else
					{
						strncat(finalstr, (char*)(str2 + strlen("title=")), y);
						strcat(finalstr, "::");
					};
				};
			};
		}

		if (strlen(finalstr) == 0) {

			if ((ptr1 = strstri(str, "<body>")) != NULL) {
				char *ptr2 = strstri(ptr1, "</body>");
				if (NULL != ptr2) {
					int sz = ptr2 - ptr1;

					if (ptr1 + 6 == ptr2) {
						strcat(finalstr, "[Empty body]");
					}
					else {
						strncat(finalstr, ptr1 + 6, (sz > 64 ? 64 : sz) - 6);
					}
				}
				else {
					strcat(finalstr, "No closing tag found.");
				}
			}
			else if ((ptr1 = strstri(str, "<html>")) != NULL) {
				char *ptr2 = strstri(ptr1, "</html>");
				if (NULL != ptr2) {
					int sz = ptr2 - ptr1;

					strncat(finalstr, ptr1 + 6, (sz > 64 ? 64 : sz) - 6);
				}
				else {
					strcat(finalstr, "No closing tag found.");
				}
			}
			else if ((ptr1 = strstri(str, "\r\n\r\n")) != NULL) {
				strncat(finalstr, ptr1 + 4, 128);
			}
			else {
				int sz = strlen(str);
				strncat(finalstr, str, sz < 64 ? sz : 64);
			}
		}
		std::string result = "";

		if (flag == 1) {
			result = "[PK]";
		}

		result += std::string(finalstr);
		return result;
	}
}
std::string equivRedirectHandler(std::string *buff, char* ip, int port, Lexems *counter) {
	if (NULL == buff || 0 == buff->size()) {
		return "";
	}

	if (counter->iterationCount > 2) {
		/*stt->doEmitionFoundData("[<a href=\"" + QString(ip) + ":" + QString::number(port) +
			"/\"><font color=\"#663300\">" + QString(ip) + ":" + QString::number(port) +
			"</font></a>] - infinite loop detected.");*/
		return "";
	}

	std::string buffcpy = *buff;
	int pos = STRSTR((const std::string *) &buffcpy, "http-equiv=\"refresh\"");
	if (-1 == pos) pos = STRSTR((const std::string *) &buffcpy, "http-equiv=refresh");
	if (-1 == pos) pos = STRSTR((const std::string *) &buffcpy, "http-equiv='refresh'");
	if (-1 == pos) {
		return "";
	}

	const std::string tempString = buffcpy.substr(pos + 17);

	int urlPos = STRSTR(&tempString, "url=");
	if (-1 == urlPos) {
		return "";
	}
	int delimPosFirst = tempString.find_first_of(" \n>\"'", urlPos);
	int delimPosSecond = tempString.find_first_of(" \n>\"'", delimPosFirst);

	std::string location = "";
	if (delimPosFirst == delimPosSecond) {
		location = tempString.substr(urlPos + 4, delimPosFirst - (urlPos + 4));
	}
	else {
		location = tempString.substr(delimPosFirst + 1, delimPosSecond);
	}

	Connector con;
	int newPort = port;
	if (location[0] == '/') {
		std::string tIP = std::string(ip) + location;
		if (gDebugMode) {
			stt->doEmitionYellowFoundData("[Redirecting to -> <a href=\"" + QString(tIP.c_str()) +
				"/\"><font color=\"#0084ff\">" + QString(tIP.c_str()) + "</font></a>]");
		}
		con.nConnect(tIP.c_str(), port, &buffcpy);
		
	}
	else if (-1 != STRSTR(location, "http://")) {
		int httpProto = STRSTR(location, "http://");

		int portPos = location.find(":", 7);
		if (-1 != portPos) {
			int portPosEnd = location.find_first_of("/ \n>\"'", portPos);
			if (-1 != portPosEnd) {
				newPort = std::stoi(location.substr(portPos + 1, portPosEnd));
				if (gDebugMode) {
					stt->doEmitionYellowFoundData("[Redirecting to -> <a href=\"" + QString(location.c_str()) +
						"/\"><font color=\"#0084ff\">" + QString(location.c_str()) + "</font></a>]");
				}
				con.nConnect(location.c_str(), newPort, &buffcpy);
			}
			else {
				stt->doEmitionYellowFoundData(QString(ip) + " Redirector error -> " + QString(location.c_str()));
				return "";
			}
		}
		else {
			if (gDebugMode) {
				stt->doEmitionYellowFoundData("[Redirecting to -> <a href=\"" + QString(location.c_str()) +
					"/\"><font color=\"#0084ff\">" + QString(location.c_str()) + "</font></a>]");
			}
			con.nConnect(location.c_str(), port, &buffcpy);
		}
	}
	else if (-1 != STRSTR(location, "https://")) {
		int httpProto = STRSTR(location, "https://");

		int portPos = location.find(":", 8);
		if (-1 != portPos) {
			int portPosEnd = location.find_first_of("/ \n>\"'", portPos);
			if (-1 != portPosEnd) {
				newPort = std::stoi(location.substr(portPos + 1, portPosEnd));
				if (gDebugMode) {
					stt->doEmitionYellowFoundData("[Redirecting to -> <a href=\"" + QString(location.c_str()) +
						"/\"><font color=\"#0084ff\">" + QString(location.c_str()) + "</font></a>]");
				}
				con.nConnect(location.c_str(), newPort, &buffcpy);
			}
			else  {
				stt->doEmitionYellowFoundData(QString(ip) + " Redirector error -> " + QString(location.c_str()));
				return "";
			}
		}
		else {
			if (gDebugMode) {
				stt->doEmitionYellowFoundData("[Redirecting to -> <a href=\"" + QString(location.c_str()) +
					"/\"><font color=\"#0084ff\">" + QString(location.c_str()) + "</font></a>]");
			}
			con.nConnect(location.c_str(), 443, &buffcpy);
		}
	}
	else {
		std::string tIP = std::string(ip) + (location[0] == '/' ? "" : "/") + location;
		if (gDebugMode) {
			stt->doEmitionYellowFoundData("[Redirecting to -> <a href=\"" + QString(tIP.c_str()) +
				"/\"><font color=\"#0084ff\">" + QString(tIP.c_str()) + "</font></a>]");
		}
		con.nConnect(tIP.c_str(), port, &buffcpy);
	}

	++counter->iterationCount;

	const std::string &locationEquiv = equivRedirectHandler(&buffcpy, ip, newPort, counter);

	if (locationEquiv.size() > 0) {
		location = locationEquiv;
	}

	if (buffcpy.size() > 0) {
		buff->clear();
		buff->assign(buffcpy);
	}

	if (location.size() > 0 && location.at(0) != '/') {
		location.insert(0, "/");
	}

	return location;
}
std::string getScriptField(std::string *buff) {
	int pos1 = STRSTR((const std::string *)buff, "<script");
	pos1 = buff->find(">", pos1) + 1;
	int pos2 = STRSTR((const std::string *)buff, "</script>");
	if (-1 == pos2) {
		return "";
	}

	std::string tempBuff;
	tempBuff.assign(*buff);
	while (pos1 == pos2) {
		tempBuff.assign(tempBuff.substr(pos1 + 9));
		pos1 = STRSTR((const std::string *)&tempBuff, "<script");
		pos1 = tempBuff.find(">", pos1) + 1;
		pos2 = STRSTR((const std::string *)&tempBuff, "</script>");
		if (-1 == pos2) {
			return "";
		}
	}

	if (-1 == pos1 || -1 == pos2) {
		return "";
	}

	std::string scriptBuff = tempBuff.substr(pos1, pos2 - pos1);

	int commentPos1 = scriptBuff.find("<!--");
	if (-1 != commentPos1) {
		int commentPos2 = scriptBuff.find("-->");

		if (-1 != commentPos2) {
			std::string finalScriptBuff = scriptBuff.substr(0, commentPos1);
			finalScriptBuff.append(scriptBuff.substr(commentPos2 + 3, scriptBuff.size() - (commentPos2 + 3)));

			return finalScriptBuff;
		}
		else {
			return "";
		}
	}
	else {
		return scriptBuff;
	}
}
std::string jsRedirectHandler(std::string *buff, char* ip, int port, Lexems *counter) {
	if (NULL == buff || 0 == buff->size()) {
		return "";
	}

	if (counter->iterationCount > 3) {
		//stt->doEmitionFoundData(QString(ip) + ":" + QString::number(port) + " - infinite loop detected.");
		return "";
	}
	
	const std::string &buffcpy_ref = getScriptField(buff);
	std::string buffcpy = buffcpy_ref;
	int sz = buffcpy.size();
	if (sz > 500) return "";

	int pos = STRSTR((const std::string *) &buffcpy, "location.href =");
	if (-1 == pos) pos = STRSTR((const std::string *) &buffcpy, "location.href=");
	if (-1 == pos) pos = STRSTR((const std::string *) &buffcpy, "location.replace");
	if (-1 == pos) {
		return "";
	}

	int eqPos = buffcpy.find_first_of("=(", pos);
	int spacePosFirst = buffcpy.find_first_not_of(" ", eqPos);
	int spacePosSecond = buffcpy.find_first_of(");", spacePosFirst);
	std::string subRedirect = buffcpy.substr(spacePosFirst + 1, spacePosSecond - spacePosFirst - 1);
	int quotePosFirst = subRedirect.find_first_of("\"'");
	if (-1 == quotePosFirst) {
		return "";
	}
	int quotePosSecond = subRedirect.find_first_of("\"'", quotePosFirst + 1);
	
	std::string subLocation = subRedirect.substr(quotePosFirst + 1, quotePosSecond - quotePosFirst - 1);
	std::string location = "";
	if (-1 != STRSTR((const std::string *) &buffcpy, "http")) {
//		/*if (-1 != STRSTR((const std::string *) &buffcpy, "https")) {
//			location += "https://";
//		}
//		else if (-1 != STRSTR((const std::string *) &buffcpy, "http")) {
//			location += "http://";
//		}
//*/
//		int quotePosSecond2 = subLocation.find_first_of(";\n", quotePosFirst + 1);
//		std::string redirectLine = subLocation.substr(quotePosFirst + 1, quotePosSecond2 - (quotePosFirst + 1));
//		int posFinalAddition = redirectLine.find_last_of("+");
//		std::string finalAddition = redirectLine.substr(posFinalAddition + 1);
//		int pos1 = finalAddition.find_first_of("'\"");
//		int pos2 = finalAddition.find_first_of("'\"", pos1 + 1);
//
//		std::string path = finalAddition.substr(pos1 + 1, pos2 - (pos1 + 1));
		//location += std::string(ip) + "/" + path;

		int portDelim = subLocation.find(":", 7);
		if (-1 != portDelim) {
			std::string portString = subLocation.substr(portDelim + 1);
			int trimPos = portString.find("/");
			if (-1 != trimPos) {
				portString = portString.substr(0, trimPos);
			}
			port = std::stoi(portString);
		}
		location = subLocation;
	}
	else {
		location = std::string(ip) + (subLocation[0] == '/' ? "" : "/") + subLocation;
	}

	Connector con;
	if (gDebugMode) {
		stt->doEmitionYellowFoundData("[Redirecting to -> <a href=\"" + QString(location.c_str()) +
			"/\"><font color=\"#0084ff\">" + QString(location.c_str()) + "</font></a>]");
	}
	con.nConnect(location.c_str(), port, &buffcpy);

	++counter->iterationCount;

	const std::string &locationJS = jsRedirectHandler(&buffcpy, ip, port, counter);

	if (locationJS.size() > 0) {
		location = locationJS;
	}

	if (buffcpy.size() > 0) {
		buff->clear();
		buff->assign(buffcpy);
	}

	return location;
}
std::string getHeader(const std::string *buffcpy, const int flag) {
	if (STRSTR(buffcpy, "<frame name=\"mainframe\" src=\"main.html\"") != -1) {
		return "[IPCam]";
	}
	else if (STRSTR(buffcpy, "CgiTagMenu?page=Top") != -1) {
		return "[Panasonic IPCam]";
	}
	else if (STRSTR(buffcpy, "MOBOTIX AG") != -1) {
		return "[Mobotic IPCam]";
	}
	else if (STRSTR(buffcpy, "iomega=") != -1) {
		if (STRSTR(buffcpy, "<div id=\"nocontent\">")) {
			return "[IOmega NAS] (Empty)";
		}
		else {
			return "[IOmega NAS]";
		}
	}
	else if (STRSTR(buffcpy, "x-rtsp-tunnelled") != -1 || STRSTR(buffcpy, "rtsp://") != -1) {
		return "[RTSP]";
	}
	else {
		std::string tempBuff = buffcpy->c_str();
		const std::string &result_ref = getTitle(tempBuff.c_str(), flag);
		std::string result = result_ref;

		if (result.size() == 0) {
			if (Utils::ustrstr(buffcpy, "redir") != std::string::npos) {
				result.append("[R]");
			};
			//result += "[Ξ]";
			result += buffcpy->substr(0, 128);
		}
		else if (STRSTR(buffcpy, "camera") != -1 || STRSTR(buffcpy, "ipcam") != -1) {
			result = "[Probably IPCamera] " + result;
		}
		return result;
	}
}

#define RVI_START_FILE "<Organization>\n\t<Department name=\"root\">\n\t\t"
void parseFlag(int flag, char* ip, char *ipRaw, int port, std::string *buff, const std::string &header, char* cp) {

	int size = buff->size();
	//Streaming server?
	if (size > 180000) {
		putInFile(flag, ip, port, size, "[Overflow]", cp);
		return;
	}

	//Generic camera
	if (flag == 0) {
		++PieCamerasC1;
		++camerasC1;
		putInFile(flag, ip, port, size, header.c_str(), cp);
	}
	//Other
	else if (flag == 1) {
		++PieOther;
		++other;
		putInFile(flag, ip, port, size, header.c_str(), cp);
	}
	//Auth
	else if (flag == 2) {
		_specBrute(ip, port, header.c_str(), flag, "", "", cp, size, true, true);
	}
	//FTP
	else if (flag == 3) {
		char log[2048] = { 0 };
		char logEmit[2048] = { 0 };
		PathStr ps;

		const lopaStr &lps = FTPA::FTPLobby(ip, port, &ps);

		if (0 != ps.directoryCount) {
			if (strstr(lps.login, "UNKNOWN") == NULL && strlen(lps.other) == 0)
			{
				++PieBA;

				sprintf(log, "<div id=\"hostspan\"><font color=\"#0f62e2\">%s</font></div><div id=\"hostspan2\"> Size: %d</div><a href=\"ftp://%s:%s@%s/\"><div id=\"hostspan3\"><span style=\"color: #ff6600;\">ftp://%s:%s@%s</span></a>%s </div>(F:%d)",
					ip, size, lps.login, lps.pass, ipRaw, lps.login, lps.pass, ipRaw, ps.headr, ps.directoryCount);
				sprintf(logEmit, "[FTP]:<a href=\"ftp://%s:%s@%s\"><span style=\"color: #ff6600;\">ftp://%s:%s@%s</span></a> (F:%d)",
					lps.login, lps.pass, ipRaw, lps.login, lps.pass, ipRaw, ps.directoryCount);

				fputsf(log, flag);

				fillGlobalLogData(ip, port, std::to_string(size).c_str(), "[FTP service]", lps.login, lps.pass, "NULL", cp, "FTP");

				stt->doEmitionFoundData(QString::fromLocal8Bit(logEmit));
			}
			else if (strstr(lps.other, "ROUTER") != NULL)
			{
				++PieBA;

				sprintf(log, "<div id=\"hostspan\"><font color=\"#0f62e2\">%s:%d</font></div><div id=\"hostspan2\"> Size: %d</div><a href=\"ftp://%s:%s@%s/\"><div id=\"hostspan3\"><span style=\"color: #ff6600;\">ftp://%s:%s@%s</span></div></a> <font color=\"#43EC00\"><a href=\"%s\" style=\"color:#43EC00;\">[ROUTER]</a></font>%s",
					ip, port, size, lps.login, lps.pass, ip, lps.login, lps.pass, ip, ip, ps.headr);
				sprintf(logEmit, "[FTP]:<a href=\"ftp://%s:%s@%s/\"><span style=\"color: #ff6600;\">ftp://%s:%s@%s</span></a> <font color=\"#43EC00\"><a href=\"%s/\" style=\"color:#43EC00;\">[ROUTER]</a></font>",
					lps.login, lps.pass, ip, lps.login, lps.pass, ip, ip);

				fputsf(log, flag);

				fillGlobalLogData(ip, port, std::to_string(size).c_str(), "[FTP service]", lps.login, lps.pass, "Router FTP.", cp, "FTP");

				stt->doEmitionFoundData(QString::fromLocal8Bit(logEmit));
			}
			else if (strstr(lps.login, "Unknown protocol") != NULL)
			{
				strcat(log, "; [!] USER/PASS commands failed. Dunno what to do.");
				fputsf(log, flag);

				stt->doEmitionFoundData(QString::fromLocal8Bit(log));
			};
		}

		return;
	}

	const std::string &cookieRef = Utils::getHeaderValue(buff, "Set-Cookie: ", "Cookie: ");
	std::string cookie = cookieRef;

	if (flag == 4 && HikVis::isInitialized)
	{
		HikVis hv;
		lopaStr lps = hv.HVLobby(ip, port);
		if (strstr(lps.login, "UNKNOWN") == NULL && strlen(lps.other) == 0)
		{
			_specFillerCustom(ip, port, "[Hikvision IVMS]", lps.login, lps.pass, 0, "[SVC]");
			//fillGlobalLogData(ip, port, std::to_string(size).c_str(), "[Hikvision IVMS] ()",
			//	lps.login, lps.pass, "[Hikvision IVMS]", "UTF-8", "Basic Authorization");

			while (hikkaStop) Sleep(10);
			hikkaStop = true;
			char fileName[256] = { 0 };
			char date[64] = { 0 };
			strcpy(date, Utils::getStartDate().c_str());
			if (HikVis::hikCounter >= 255) {
				HikVis::hikCounter = 0;
				HikVis::hikPart++;
			}
			sprintf(fileName, "./" DIR_NAME "%s_%s/hikkafile_%s_part_%d.csv", date, Utils::getCurrentTarget().c_str(), date, HikVis::hikPart);
			FILE *f = fopen(fileName, "a");
			if (f != NULL) {
				char string[1024] = { 0 };
				sprintf(string, "\"%s\",\"0\",%s,\"%d\",\"2\",\"%s\",\"%s\",\"0\",\"1\",\"0\",\"0\"\n",
					ip, ip, port, lps.login, lps.pass);
				fputs(string, f);
				fclose(f);
			}
			else stt->doEmitionRedFoundData("Cannot open csv - \"" + QString(fileName));
			HikVis::hikCounter++;
			hikkaStop = false;
		};
		return;
	}
	else if (flag == 5)
	{
		HikVis hv;
		lopaStr lps = hv.RVILobby(ip, port);
		if (strstr(lps.login, "UNKNOWN") == NULL && strlen(lps.other) == 0)
		{
			_specFillerCustom(ip, port, "[RVI]", lps.login, lps.pass, 0, "[SVC]");
			/*fillGlobalLogData(ip, port, std::to_string(size).c_str(), "[RVI] ()",
				lps.login, lps.pass, "[RVI]", "UTF-8", "Basic Authorization");*/

			while (rviStop) Sleep(10);
			rviStop = true;
			char fileName[256] = { 0 };
			char date[64] = { 0 };
			strcpy(date, Utils::getStartDate().c_str());
			if (HikVis::rviCounter >= 255) {
				HikVis::rviCounter = 0;
				HikVis::rviPart++;
			}
			sprintf(fileName, "./" DIR_NAME "%s_%s/rvifile_%s(%s)_part_%d.xml", 
				date, Utils::getCurrentTarget().c_str(), date, Utils::getStartTime().c_str(), HikVis::rviPart);

			char string[1024] = { 0 };
			FILE *fc = fopen(fileName, "r");
			if (fc == NULL) {
				FILE *f = fopen(fileName, "a");
				if (f != NULL) {
					sprintf(string, RVI_START_FILE"<Device title=\"%s\" ip=\"%s\" port=\"%d\" user=\"%s\" password=\"%s\"/>\n\t</Department>\n</Organization>\n",
							ip, ip, port, lps.login, lps.pass);
					fputs(string, f);
					fclose(f);
				}
				else stt->doEmitionRedFoundData("Cannot open xml - \"" + QString::fromLocal8Bit(fileName) + " Err:" + QString::number(GetLastError()));
			} else {
				fclose(fc);
				FILE *f = fopen(fileName, "r+");
				if (f != NULL) {
					fseek(f, -35, SEEK_END);
					sprintf(string, "\n\t\t<Device title=\"%s\" ip=\"%s\" port=\"%d\" user=\"%s\" password=\"%s\"/>\n\t</Department>\n</Organization>\n",
						ip, ip, port, lps.login, lps.pass);
					fputs(string, f);
					fclose(f);
				}
				else stt->doEmitionRedFoundData("Cannot open xml - \"" + QString::fromLocal8Bit(fileName) + " Err:" + QString::number(GetLastError()));
			}
			HikVis::rviCounter++;
			rviStop = false;
		};
		return;
	}
	else if (flag == 21) //Eyeon
	{
		_specBrute(ip, port, "Eyeon Camera", flag, "/user/index.htm", "Basic Authorization", cp, size);
	}
	else if (flag == 22) //IP Camera control
	{
		_specBrute(ip, port, "IP camera Control webpage", flag, "/main/cs_motion.asp", "Basic Authorization", cp, size);
	}
	else if (flag == 23) //Network Camera BB-SC384
	{
		_specBrute(ip, port, "Network Camera BB-SC384", flag, "/live/index2.html", "Basic Authorization", cp, size);
	}
	else if (flag == 24) //Network Camera VB-M40
	{
		_specBrute(ip, port, "Network Camera VB-M40", flag, "/-wvhttp-01-/open.cgi?", "Basic Authorization", cp, size);
	}
	else if (flag == 25) //Panasonic Unibrowser-camera
	{
		_specBrute(ip, 60002, "Panasonic Unibrowser-camera", flag, "/SnapshotJPEG", "Basic Authorization", cp, size);
	}
	else if (flag == 26) //Sony Network Camera
	{
		_specBrute(ip, port, "Sony Network Camera", flag, "/oneshotimage?", "Basic Authorization", cp, size);
	}
	else if (flag == 27) //UA Network Camera
	{
		_specBrute(ip, port, "UA Network Camera", flag, "/webs.cgi?", "Basic Authorization", cp, size);
	}
	else if (flag == 28) //Network Camera VB-M40
	{
		_specBrute(ip, port, "Network Camera VB-??", flag, "/-wvhttp-01-/open.cgi?", "Basic Authorization", cp, size);
	}
	else if (flag == 29) //LG Smart IP Device
	{
		_specBrute(ip, port, "LG Smart IP Device Camera", flag, "/digest.php", "Basic Authorization", cp, size);
	}
	else if (flag == 30) //NAS
	{
		_specBrute(ip, port, "NAS", flag, "/cgi-bin/data/viostor-220/viostor/viostor.cgi", "Basic Authorization", cp, size);
	}
	else if (flag == 31) //ip cam
	{
		_specBrute(ip, port, "IP Camera", flag, "/check_user.cgi", "Basic Authorization", cp, size);
	}
	else if (flag == 32) //IPC WEB ip cam
	{
		_specWEBIPCAMBrute(ip, port, "[IPC] WEB IP Camera", flag, "WEB Authorization", cp, size, "IPC", &cookie);
	}
	else if (flag == 33) //GEOvision ip cam
	{
		_specWEBIPCAMBrute(ip, port, "[GEO] WEB IP Camera", flag, "WEB Authorization", cp, size, "GEO", &cookie);
	}
	else if (flag == 34) //Hikvision ip cam
	{
		char ipNew[256] = { 0 };
		if (strstri(ip, "https://") != NULL) {
			sprintf(ipNew, "https://%s", ipRaw);
		}
		else {
			sprintf(ipNew, "http://%s", ipRaw);
		}
		if (_specBrute(ipNew, port, "[Hikvision] IP Camera", flag, "/PSIA/Custom/SelfExt/userCheck", "[DIGEST]", cp, size) == -1){
			_specBrute(ipNew, port, "[Hikvision] IP Camera", flag, "/PSIA/Custom/HIK/userCheck", "[DIGEST]", cp, size);
		}
	}
	else if (flag == 35) //EasyCam
	{
		_specWEBIPCAMBrute(ip, port, "[EasyCam] WEB IP Camera", flag, "WEB Authorization", cp, size, "EasyCam", &cookie);
	}
	else if (flag == 36) //Panasonic Cam
	{
		_specBrute(ip, port, QString("[Panasonic] IP Camera (" + QString(ip) + ")").toLocal8Bit().data(), flag,
			"/config/index.cgi", "Basic Authorization", cp, size);

		stt->doEmitionYellowFoundData("[PaCr]Panasonic IPCam, crawling started.");

		std::string buff;
		Connector con;
		con.nConnect(std::string(std::string(ip) + "/config/cam_portal.cgi").c_str(), port, &buff);
		int nPort = port;
		for (int i = 0; i < 16; ++i) {
			const std::string &cam_link_data = Utils::getStrValue(buff, "cam_link[" + std::to_string(i) + "]", ";");
			if (cam_link_data.size() != 0) {
				const std::string &newURL = Utils::getStrValue(cam_link_data, "src=\"", "\"");
				if (newURL.size() != 0) {
					const std::string &newIP = Utils::getStrValue(newURL, "http://", "/");
					if (newIP.size() != 0) {
						const std::string &newPath = newURL.substr(newURL.find(newIP) + newIP.length());
						std::vector<std::string> portVec = Utils::splitToStrVector(newIP, ':');
						stt->doEmitionYellowFoundData("[PaCr] Url found:" + QString(newURL.c_str()));

						if(portVec.size() == 2) {
							nPort = std::stoi(portVec[1]);
						}

						_specBrute(newIP.c_str(), nPort, QString("[Panasonic] IP Camera (" +
							QString(newIP.c_str()) + ")").toLocal8Bit().data(), flag,
							(char*)newPath.c_str(), "Basic Authorization", cp, size);
					}
				}
				else stt->doEmitionRedFoundData("[Panasonic Cam URL] Cannot extract data " +
					QString(ip));
			}
			else stt->doEmitionRedFoundData("[Panasonic Cam cam_link] Cannot extract data " +
				QString(ip));
		}
	}
	else if (flag == 37) //Panasonic Cam
	{
		_specBrute(ip, port, "[Panasonic] IP Camera", flag, "/view/getuid.cgi", "Basic Authorization", cp, size);
	}
	else if (flag == 38) //Foscam
	{
		_specWEBIPCAMBrute(ip, port, "[Foscam] IP Camera", flag, "Web Authorization", cp, size, "Foscam", &cookie);
	}
	else if (flag == 39) //EagleEye
	{
		_specBrute(ip, port, "[EagleEye] IP Camera", flag, "/cgi-bin/guest/Video.cgi?", "Basic Authorization", cp, size);
	}
	else if (flag == 40) //Network Camera VB-C??
	{
		_specBrute(ip, port, "[Network Camera VB-C??] IP Camera", flag, "/admin/index.shtml?", "Basic Authorization", cp, size);
	}
	else if (flag == 41) //AVIOSYS-camera
	{
		_specWEBIPCAMBrute(ip, port, "[AVIOSYS] IP Camera", flag, "Web Authorization", cp, size, "AVIOSYS", &cookie);
	}
	else if (flag == 42) //NW_camera
	{
		_specBrute(ip, port, "[NW_camera] IP Camera", flag, "/cgi-bin/getuid?FILE=indexnw.html", "Basic Authorization", cp, size);
	}
	else if (flag == 43) //NW_camera
	{
		_specBrute(ip, port, "[Micros] IP Camera", flag, "/gui/rem_display.shtml", "Basic Authorization", cp, size);
	}
	else if (flag == 44) //Hikvision ip cam 2
	{
		_specBrute(ip, port, "[Hikvision] IP Camera 2", flag, "/ISAPI/Security/userCheck", "Basic Authorization", cp, size);
	}
	else if (flag == 45) //Panasonic ip cam
	{
		_specBrute(ip, port, "[Panasonic] IP Camera", flag, "/config/index.cgi", "Basic Authorization", cp, size);
	}
	else if (flag == 46) //Buffalo disk
	{
		_specWEBIPCAMBrute(ip, port, "[Buffalo] Lan Disk", flag, "Web Authorization", cp, size, "BUFFALO", &cookie);
	}
	else if (flag == 47) //Digital Video Server
	{
		_specWEBIPCAMBrute(ip, port, "[DVS] Camera", flag, "Web Authorization", cp, size, "DVS", &cookie);
	}
	else if (flag == 48) //ipCAM
	{
		_specWEBIPCAMBrute(ip, port, "[ipCAM] Camera", flag, "Web Authorization", cp, size, "IPCAM", &cookie);
	}
	else if (flag == 49) //IEORFOREFOX
	{
		_specWEBIPCAMBrute(ip, port, "[IEORFOREFOX] Camera", flag, "Web Authorization", cp, size, "IEORFOREFOX", &cookie);
	}
	else if (flag == 50) //IP Camera
	{
		_specBrute(ip, port, "IP Camera", flag, "/app/multi/single.asp", "Basic Authorization", cp, size);
	}
	else if (flag == 51) //MASPRO
	{
		_specWEBIPCAMBrute(ip, port, "[MASPRO] WEB IP Camera", flag, "WEB Authorization", cp, size, "MASPRO", &cookie);
	}
	else if (flag == 52) //webcamxp5
	{
		_specWEBIPCAMBrute(ip, port, "[WEBCAMXP] WEB IP Camera", flag, "WEB Authorization", cp, size, "WEBCAMXP", &cookie);
	}
	else if (flag == 53) //Jassun
	{
		_specWEBIPCAMBrute(ip, port, "[JASSUN] WEB IP Camera", flag, "WEB Authorization", cp, size, "JASSUN", &cookie);
	}
	else if (flag == 54) //Beward
	{
		_specWEBIPCAMBrute(ip, port, "[BEWARD] WEB IP Camera", flag, "WEB Authorization", cp, size, "BEWARD", &cookie);
	}
	else if (flag == 55) //QCam
	{
		_specBrute(ip, port, "IP Camera", flag, "/videostream.cgi", "Basic Authorization", cp, size);
	}
	else if (flag == 56) //EaglesEye
	{
		_specBrute(ip, port, "IP Camera", flag, "/Login.cgi?rnd=000148921789481", "Basic Authorization", cp, size);
	}
	else if (flag == 57) //Juan
	{
		_specWEBIPCAMBrute(ip, port, "[JUAN] WEB IP Camera", flag, "WEB Authorization", cp, size, "JUAN", &cookie);
	}
	else if (flag == 58) //QLikView
	{
		stt->doEmitionRedFoundData("QLikView found. " + QString(ip));
		_specBrute(ip, port, "[QLikView] IP Camera", flag, "/QvAJAXZfc/Authenticate.aspx?_=1453661324640", "Basic Authorization", cp, size);
	}
	else if (flag == 59) //RTSP
	{
		char newIP[128] = {0};
		sprintf(newIP, "rtsp://%s:%d/", ipRaw, port);
		_specRTSPBrute(newIP, port, header.c_str(), flag, "RTSP", cp, size);
	}
	else if (flag == 60) //ACTi
	{
		_specWEBIPCAMBrute(ip, port, "[ACTi] WEB IP Camera", flag, "WEB Authorization", cp, size, "ACTi", &cookie);
	}
	else if (flag == 61) //AirOS
	{
		_specWEBIPCAMBrute(ip, port, "[AirOS] WEB IP Camera", flag, "WEB Authorization", cp, size, "AirOS", &cookie);
	}
	else if (flag == 62) //XMSECU
	{
		_specWEBIPCAMBrute(ip, port, "[XMSECU] WEB IP Camera", flag, "WEB Authorization", cp, size, "XMSECU", &cookie);
	}
	
	else if (flag == 20) //AXIS Camera
	{
		if (_specBrute(ip, port, "AXIS Camera", flag, "/mjpg/video.mjpg", "Basic Authorization", cp, size) == -1) {
			if (_specBrute(ip, port, "AXIS Camera", flag, "/axis-cgi/com/ptz.cgi?", "Basic Authorization", cp, size) == -1) {
				_specBrute(ip, port, "AXIS Camera", flag, "/view/viewer_index.shtml?", "Basic Authorization", cp, size);
			}
		}
	}
	else if (flag == 19) //reecam cameras
	{
		_specBrute(ip, port, "Reecam (network camera)", flag, "/videostream.cgi", "Basic Authorization", cp, size);
	}
	else if (flag == 18) //linksys camera
	{
		_specBrute(ip, port, "Linksys camera", flag, "/img/main.cgi", "Basic Authorization", cp, size);
	}
	else if (flag == 17) //Real-time IP Camera Monitoring System
	{
		_specBrute(ip, port, "Real-time IP Camera Monitoring System", flag, "/live.htm", "Basic Authorization", cp, size);
	}
	else if (flag == 11)
	{
		_specBrute(ip, port, "Netwave IP Camera", flag, "/videostream.cgi", "Basic Authorization", cp, size);
	}
	else if (flag == 12)
	{
		_specBrute(ip, port, "IP Camera", flag, "/view/view.shtml?videos=", "Basic Authorization", cp, size);
	}
	else if (flag == 13)
	{
		_specBrute(ip, port, "IP Camera", flag, "/eng/view/indexjava.html", "Basic Authorization", cp, size);
	}
	else if (flag == 14)
	{
		_specBrute(ip, port, "IP Camera", flag, "/rdr.cgi", "Basic Authorization", cp, size);
	}
	else if (flag == 15) //For HFS
	{
		char log[512] = { 0 };
		++PieCamerasC1;
		++camerasC1;
		const lopaStr &lps = BA::BALobby((string(ip) + "/~login").c_str(), port, false);
		sprintf(log, "[HFS]:<a href=\"%s:%d/\"><span style=\"color: #a1a1a1;\">%s:%d</span></a><font color=\"#0084ff\"> T: </font><font color=\"#ff9600\">%s Pass: %s:%s</font>",
			ip, port, ip, port, header.c_str(), lps.login, lps.pass);

		fillGlobalLogData(ip, port, std::to_string(size).c_str(), header.c_str(), lps.login, lps.pass, "HFS-FTP", cp, "Basic Authorization");
		fputsf(log, flag);
		stt->doEmitionFoundData(QString::fromLocal8Bit(log));
	}
	//else if (flag == 1)
	//{
	//	_specBrute(ip, port, header.c_str(), flag, baPath, "[NORMAL]", cp, size);
	//}
	//else if (flag == 101)
	//{
	//	_specBrute(ip, port, header.c_str(), flag, baPath, "[DIGEST]", cp, size);
	//}
	//else if (flag == 10)
	//{
	//	_specWFBrute(ip, port, buffcpy->c_str(), flag, baPath, "Web Form", "Web Form", cp, size, finalstr);
	//}
	//else if (flag == 2)
	//{
	//	putInFile(0, ip, port, size, "[OVERFLOW]", cp);
	//}
	//else
	//{
	//	putInFile(flag, ip, port, size, finalstr, cp);
	//};
}
std::string handleRedirects(std::string *buffcpy, char* ip, int port) {
	Lexems counter;
	counter.iterationCount = 0;
	std::string location = std::string(ip);
	const std::string &locationEquiv = equivRedirectHandler(buffcpy, ip, port, &counter);
	if (locationEquiv.find("http") == 0) {
		location = locationEquiv;
	}
	else {
		location += locationEquiv;
	}

	counter.iterationCount = 0;
	if (Utils::ustrstr((const std::string *) buffcpy, "<frameset ") == -1) {
		const std::string &locationJS = jsRedirectHandler(buffcpy, ip, port, &counter);
		if (locationJS.size() > 0) {
			if (Utils::ustrstr(location, "http") != -1) {
				location = locationJS;
			}
			else {
				location += locationJS;
			}
		}
	};
	
	return location;
}
int handleFramesets(std::string *buffcpyOrig, char* ip, char* ipRaw, int port, int flag, char *cp) {
	if (NULL == buffcpyOrig || 0 == buffcpyOrig->size()) {
		return flag;
	}

	std::string buffcpy = *buffcpyOrig;
	std::transform(buffcpy.begin(), buffcpy.end(), buffcpy.begin(), ::tolower);

	const std::string &constHeadRef = getHeader((const std::string *) buffcpyOrig, flag);
	std::string header = constHeadRef;

	int pos;
	if ((pos = STRSTR(buffcpy, "<frameset ")) != -1) {

		Connector con;
		int framePos = buffcpy.find("<frame ", pos + 10);
		int counter = 0;
		while (framePos != -1) {
			int framePosEnd = buffcpy.find(">", framePos);

			std::string frameString = buffcpy.substr(framePos, framePosEnd - framePos);
			framePos = buffcpy.find("<frame ", framePos + 1);

			int frameSrcPos = frameString.find("src");
			if (-1 != frameSrcPos) {
				int eqPos = frameString.find_first_of("=", frameSrcPos);
				if (-1 != eqPos) {
					int quotePos1 = frameString.find_first_of("\"'", eqPos);
					if (-1 != quotePos1) {
						int quotePos2 = frameString.find_first_of("\"'", quotePos1 + 1);

						//if (quotePos1 != quotePos2) {
						std::string location = frameString.substr(quotePos1 + 1, quotePos2 - quotePos1 - 1);
						if (counter++ > 5) {
							return 0;
						};
						if (location.size() == 0) {
							continue;
						}
						std::string tIP = std::string(ip) + (location[0] == '/' ? "" : "/") + location;
						std::string buff;
						Sleep(1000);
						int sz = con.nConnect(tIP.c_str(), port, &buff);
						if (-1 == sz) {
							continue;
						}
						int flg = contentFilter((const std::string *) &buff, port, tIP.c_str(), "UTF-8", sz);
						if (flg == -1) {
							return -1;
						}
						else {
							if (2 == flg) {
								const std::string &headerBA = getHeader((const std::string *) &buff, flg);
								parseFlag(flg, (char*)tIP.c_str(), ipRaw, port, &buff, headerBA, cp);
								return flg;
							}
							else {
								header += " - " + getHeader((const std::string *) &buff, flg);
							}
						}
						//}
					}
				}
			}
		}
	}

	std::string newIP = std::string(ip);
	int newPort = port;
	std::string location = Utils::getHeaderValue(buffcpyOrig, "Location: ", "");
	if (location.size() > 0) {
		if (location.find("http://") != -1) {
			if (location.at(location.size() - 1) == '/') {
				location = location.substr(0, location.size() - 1);
			}
			newPort = port;
			int portPos = location.find(":", 8);
			if (-1 != portPos) {
				int secPos = location.find("/", 8);
				if (-1 != secPos) {
					if (portPos < secPos) {
						newPort = std::stoi(location.substr(portPos + 1, secPos - portPos - 1));
					}
				}
				else {
					newPort = std::stoi(location.substr(portPos + 1));
				}
			}
			newIP = location;
		}
		else if (location.find("https://") != -1) {
			if (location.at(location.size() - 1) == '/') {
				location = location.substr(0, location.size() - 1);
			}
			newPort = 443;
			int portPos = location.find(":", 8);
			if (-1 != portPos) {
				int secPos = location.find("/", 8);
				if (-1 != secPos) {
					if (portPos < secPos) {
						newPort = std::stoi(location.substr(portPos + 1, secPos - portPos - 1));
					}
				}
				else {
					newPort = std::stoi(location.substr(portPos + 1));
				}
			}
			newIP = location;
		}
		else {
			newIP += location;
		}
	}
	//stt->doEmitionYellowFoundData("Location: " + QString(newIP.c_str()) + "; Port: " + QString::number(newPort));
	parseFlag(flag, (char*)newIP.c_str(), ipRaw, newPort, buffcpyOrig, header, cp);
	return flag;
}
int Lexems::filler(char* ip, char *ipRaw, int port, std::string *buffcpy, int size, Lexems *lx)
{
	if (0 == size) {
		int flag = contentFilter((const std::string *) buffcpy, port, ip, NULL, size);
		parseFlag(flag, ip, ipRaw, port, buffcpy, "Empty", NULL);
		return flag;
	}
	else {
		if (port == 22) {
			_saveSSH(ip, 22, size, buffcpy->c_str());
			return -1;
		}
		else if (Utils::ustrstr((const std::string *) buffcpy, "SSH-2.0-OpenSSH") != -1 ||
			Utils::ustrstr((const std::string *) buffcpy, "SSH-2.0-mod_sftp") != -1) {
			std::string sshBuff;
			int res = SSHAuth::SSHLobby(ip, port, &sshBuff);
			if (res != -1 && res != -2) _saveSSH(ip, port, size, (char*)sshBuff.c_str());
			return -1;
		};

		const std::string &location = handleRedirects(buffcpy, ip, port);

		char cp[32] = { 0 };
		strncpy(cp, getCodePage(buffcpy->c_str()), 32);
		int flag = contentFilter((const std::string *) buffcpy, port, (location.size() > 0 ? location.c_str() : ip), cp, size);
		if (flag != -1) {
			const std::string &header = getHeader((const std::string *) buffcpy, flag);
			if (flag < 2 || flag > 6) {
				if ((flag = handleFramesets(buffcpy, (location.size() > 0 ? (char*)location.c_str() : ip), ipRaw, port, flag, cp)) == -1) {
					return -1;
				}
			}
			else {
				parseFlag(flag, (location.size() > 0 ? (char*)location.c_str() : ip), ipRaw, port, buffcpy, header, cp);
			}
			return flag;
		}
		else {
			return -1;
		}
	}
}

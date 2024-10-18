/*
 *
 * ©K. D. Hedger. Thu 17 Oct 14:06:33 BST 2024 keithdhedger@gmail.com

 * This file (globals.h) is part of KKTerminalQT.

 * KKTerminalQT is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * KKTerminalQT is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with KKTerminalQT.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _GLOBALS_
#define _GLOBALS_

#define MAXMSGSIZE 4096
#define MSGKEY 0x8000
#define NOSENDMSG 3

#define MSGANY 0
#define ALLMSGTYPES 0xfff
#define CONTINUEMSG 0x8000
#define SENDMSG 0x1000

//#include <stdlib.h>
//#include <stdio.h>
//#include <string.h>
//#include <sys/stat.h>
//#include <unistd.h>
//#include <sys/types.h>
#include <linux/limits.h>
//#include <sys/ioctl.h>
//#include <sys/mman.h>
//#include <fcntl.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
//#include <libgen.h>
//#include <time.h>
//
#include <string>
//#include <vector>
//#include <map>

//#include <sqlite3.h>

#include <QApplication>
#include <QtWidgets>
#include <QSettings>
#include <QObject>
#include <QProxyStyle>

#ifdef _USEQT6_
#include <qtermwidget6/qtermwidget.h>
#else
#include <qtermwidget5/qtermwidget.h>
#endif

struct msgStruct
{
	long mType=0;
	char mText[MAXMSGSIZE];
};

struct miniPrefsReturnStruct
{
	QDialog					*theDialog;
	QHash<int,QLineEdit*>	boxes;
};

#include "config.h"

#include "KKTerminalQTClass.h"

#endif
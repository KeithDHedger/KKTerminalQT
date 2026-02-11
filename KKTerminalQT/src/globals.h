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

enum {KKTERMINALQTQUIT=2000,KKTERMINALQTNEW,KKTERMINALQTNEWHERE,KKTERMINALQTNEWCOMMAND,KKTERMINALSENDTEXT,KKTERMINALQTACTIVATE,KKTERMINALQTRELOADTHEME};//TODO//

#define MAXMSGSIZE 256
#define MSGANY 0

#include <linux/limits.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h> 

#include <QApplication>
#include <QtWidgets>
#include <QSettings>
#include <QObject>
#include <QProxyStyle>

#include "config.h"

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

//struct miniPrefsReturnStruct
//{
//	QDialog					*theDialog;
//	QHash<int,QLineEdit*>	boxes;
//};

#define THEMEBOX 0
#define FONTBOX 0
#define BLINKBOX 0
#define CONFIRMPASTEBOX 1
#define CLOSETABONEXIT 2

#include "prefsClass.h"
#include "SingleInstanceClass.h"
#include "KKTerminalQTClass.h"

#endif
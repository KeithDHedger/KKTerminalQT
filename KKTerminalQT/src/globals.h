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

enum {KKTERMINALQTQUIT=2000,KKTERMINALQTNEW,KKTERMINALQTNEWHERE,KKTERMINALQTNEWCOMMAND,KKTERMINALSENDTEXT,KKTERMINALQTACTIVATE,KKTERMINALQTRELOADTHEME,KKTERMINALQTEXECCOMM};//TODO//

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
#include <getopt.h>

#include <QtWidgets>

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

#define THEMEBOX 0
#define FONTBOX 0
#define BLINKBOX 0
#define CONFIRMPASTEBOX 1
#define CLOSETABONEXIT 2

#define THEMEPREFSNAME "term/Theme"
#define FONTPREFSNAME "term/Font"
#define BLINKPREFSNAME "term/Blink Cursor"
#define CONFIRMPASTEPREFSNAME "term/Confirm Paste"
#define CLOSETABONEXITPREFSNAME "term/Close Tab On Exit"

#include "prefsClass.h"
#include "QT_AboutBox.h"
#include "SingleInstanceClass.h"
#include "KKTerminalQTClass.h"

static const char *helpText="Usage: kkterminalqt [options]\n\
Snippets file is at ~/.config/kkterminalqt.snippits\n\
One command per line as you would type it into the terminal.\n\
eg:\n\
( cd /usr/share/icons;xdg-open .;ls )\n\
File can be edited manually ( use Snippets->Reload snippits file to reload snippet file after editing ).\n\
Add the current clipboard to the file and reload automatically ( use Snippets->Add Clipboard to snippets ).\n\
Add the current selection to the file and reload automatically ( use Snippets->Add Selection to snippets ).\n\
If you hold down 'SHIFT' key when selecting from Snippits menu no newline is sent but a space is added at the end of the snippit text allowing for params to be entered.\n\
\n\
Options:\n\
  -h, --help           Displays help on commandline options.\n\
  -v, --version        Displays version information.\n\
  -k, --key <KeyID>    Force key ID.\n\
  -m, --multi          Force multiple instance.\n\
  -q, --quit           Quit app.\n\
  -n, --new-tab <ARG>  New tab in ARG.\n\
  -t, --tab            New tab in PWD.\n\
  -c, --command <ARG>  Execute ARG in new tab.\n\
  -f, --theme <ARG>    Change theme to ARG.\n\
  -x, --execute ...    Consume rest of line and run in new tab.\n";

#endif
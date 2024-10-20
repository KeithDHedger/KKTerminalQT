/*
 *
 * ©K. D. Hedger. Thu 17 Oct 13:43:47 BST 2024 keithdhedger@gmail.com

 * This file (KKTerminalQTClass.h) is part of KKTerminalQT.

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

#ifndef _KKTERMINALQTCLASS_
#define _KKTERMINALQTCLASS_

#include "globals.h"

class KKTerminalQTClass : public QObject
{
	Q_OBJECT

	public:
		KKTerminalQTClass(QApplication *app);
		~KKTerminalQTClass();

//app vars
		QMainWindow				*mainWindow=NULL;
		QApplication				*application=NULL;
		QTabWidget				*mainNotebook=NULL;
		QMenuBar					*menuBar=NULL;
		QString					theme="Falcon";
		QFont					font=QGuiApplication::font();
		int						key=-1;
		int						queueID=-1;

//app functions
		void						handleSignal(int signum);
		void						initApp(int argc,char** argv);

//scripting
		void						runCLICommands(int quid);
		QCommandLineParser		parser;

	private:
//file menu
		QMenu					*fileMenu;
//edit menu
		QMenu					*editMenu;
//help menu
		QMenu					*helpMenu;

		QTimer 					*checkMessages=NULL;
		int						prefsMsgTimer=2000;
		int						termNumber=1;

//app functions
		void						addTerminal(void);
		void						buildMainGui(void);
		void						writeExitData(void);
		void						doTimer(void);
		miniPrefsReturnStruct	miniPrefsDialog(QString prefsname,QStringList items);
};

#endif

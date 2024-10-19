/*
 *
 * ©K. D. Hedger. Thu 17 Oct 14:11:35 BST 2024 keithdhedger@gmail.com

 * This file (main.cpp) is part of KKTerminalQT.

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

#include "globals.h"
#include "SingleInstanceClass.h"

KKTerminalQTClass	*kkterminalqt=NULL;

int main (int argc, char **argv)
{
	int				status;
	QDir				commsDir;
	QApplication		*napp=new QApplication(argc,argv);

	napp->setOrganizationName("KDHedger");
	napp->setApplicationName("KKTerminalQT");

	kkterminalqt=new KKTerminalQTClass(napp);
	kkterminalqt->parser.addHelpOption();
	kkterminalqt->parser.addOptions(
		{
			{{"k","key"},"Force key ID.","KeyID"},
			{{"m","multi"},"Force multiple instance."},
			{{"q","quit"},"Quit app."},
			{{"n","new-tab"},"New Terminal.","ARG"},
			{{"t","tab"},"New Terminal In PWD."},
			{{"c","command"},"Execute ARG in new tab.","ARG"},
	});

	kkterminalqt->parser.process(kkterminalqt->application->arguments());

	if(kkterminalqt->parser.isSet("key"))
		kkterminalqt->sessionID=kkterminalqt->parser.value("key").toInt(nullptr,0);

	SingleInstanceClass siapp(kkterminalqt->application,kkterminalqt->sessionID,kkterminalqt->parser.isSet("multi"),argc,argv);
	if(siapp.getRunning()==true)
		{
			kkterminalqt->runCLICommands(siapp.queueID);
			return(0);
		}

	kkterminalqt->queueID=siapp.queueID;
	kkterminalqt->forcedMultInst=kkterminalqt->parser.isSet("multi");
	kkterminalqt->currentWorkSpace=siapp.workspace;
	kkterminalqt->sessionID=siapp.useKey;
	kkterminalqt->initApp(argc,argv);
	qDebug()<<kkterminalqt->sessionID;

	kkterminalqt->runCLICommands(kkterminalqt->queueID);

	if(getuid()!=0)
		kkterminalqt->application->setWindowIcon(QIcon(DATADIR "/pixmaps/" PACKAGE ".png"));
	else
		kkterminalqt->application->setWindowIcon(QIcon(DATADIR"/pixmaps/KKEditRoot.png"));

	status=kkterminalqt->application->exec();
	kkterminalqt->writeExitData();

	delete kkterminalqt;
	return status;
}


//
//
//
//
//exit(100);
//
//#include <QApplication>
//#include <QWidget>
//#include <QMessageBox>
//
//int main(int argv, char **args)
//{
//	QApplication	app(argv, args);
//	QMessageBox		msgBox;
//
//	msgBox.setText("Hello World!");
//	msgBox.show();
//	return app.exec();
//}
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
   // kkterminalqt->splash=new QSplashScreen(QString(DATADIR)+"/pixmaps/KKEditQT.png",Qt::FramelessWindowHint|Qt::X11BypassWindowManagerHint);

//tagClass tc(kkterminalqt);
//tc.getTagList(QStringList()<<"/home/keithhedger/Documents/test it.html"<<"/home/keithhedger/Documents/test.html");
//
//for (int j=0;j<tc.tagList.count();j++)
//{
//	qDebug()<<tc.tagList.at(j).tagName<<tc.tagList.at(j).tagType<<tc.tagList.at(j).lineNumberString<<tc.tagList.at(j).tagDefine<<tc.tagList.at(j).tagFilepath<<tc.tagList.at(j).lineNumber;
//}
	kkterminalqt->parser.addHelpOption();
	kkterminalqt->parser.addOptions(
		{
			{{"k","key"},"Force key ID.","KeyID"},
			{{"m","multi"},"Force multiple instance."},
			{{"s","safe"},"Safe mode ( no plugins, no plugin enable/disable data written )."},
			{{"v","very-safe"},"Very safe mode ( no plugins, no highlighting/themes, no prefs data written )."},
			{{"q","quit"},"Quit app."},
			{{"r","restore-session"},"Open session by name.","SessionName"}
	});

	kkterminalqt->parser.process(kkterminalqt->application->arguments());

	if(kkterminalqt->parser.isSet("key"))
		kkterminalqt->sessionID=kkterminalqt->parser.value("key").toInt(nullptr,0);

	//if(kkterminalqt->parser.isSet("safe"))
	//	kkterminalqt->safeFlag=true;

	//if(kkterminalqt->parser.isSet("very-safe"))
	//	{
	//		kkterminalqt->safeFlag=true;
	//		kkterminalqt->verySafeFlag=true;
	//	}

	SingleInstanceClass siapp(kkterminalqt->application,kkterminalqt->sessionID,kkterminalqt->parser.isSet("multi"),argc,argv);
	if(siapp.getRunning()==true)
		{
			kkterminalqt->runCLICommands(siapp.queueID);
			return(0);
		}

	//kkterminalqt->splash->show();
	kkterminalqt->queueID=siapp.queueID;
	kkterminalqt->forcedMultInst=kkterminalqt->parser.isSet("multi");
	kkterminalqt->currentWorkSpace=siapp.workspace;
	kkterminalqt->sessionID=siapp.useKey;
	//kkterminalqt->forceDefaultGeom=!siapp.isOnX11;
	kkterminalqt->initApp(argc,argv);

//test plugs
//#if 0
//#ifdef _DEBUGCODE_
//	//qSetMessagePattern("[%{type}] %{appname} (%{file}->%{function}->%{line}) - %{message}");
//	for(int j=0;j<kkterminalqt->plugins.count();j++)
//		kkterminalqt->plugins[j].printIt();
//#endif
//#endif

	kkterminalqt->runCLICommands(kkterminalqt->queueID);

	//kkterminalqt->setToolbarSensitive();
	if(getuid()!=0)
		kkterminalqt->application->setWindowIcon(QIcon(DATADIR "/pixmaps/" PACKAGE ".png"));
	else
		kkterminalqt->application->setWindowIcon(QIcon(DATADIR"/pixmaps/KKEditRoot.png"));

	//kkterminalqt->splash->finish(kkterminalqt->mainWindow);

	status=kkterminalqt->application->exec();
//	qDebug()<<"here>>>>>";
//	//qDebug()<<kkterminalqt->mainWindow->geometry();
	kkterminalqt->writeExitData();
//	qDebug()<<"<<<<<";
/*
 QSettings settings("MyCompany", "MyApp");
     settings.setValue("geometry", saveGeometry());
     settings.setValue("windowState", saveState());
*/
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
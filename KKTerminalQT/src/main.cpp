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

KKTerminalQTClass	*kkterminalqt=NULL;

void signalHandler(int signalNum)
{
	kkterminalqt->handleSignal(signalNum);
}

int main (int argc, char **argv)
{
	bool				forcedg=false;
	int				status;
	QDir				commsDir;
	QApplication		*napp;

	for(int i=1;i<argc;++i)
		if(qstrcmp(argv[i],"--qwindowgeometry")==0)
			forcedg=true;

	napp=new QApplication(argc,argv);
	napp->setOrganizationName("KDHedger");
	napp->setApplicationName("KKTerminalQT");
	napp->setApplicationVersion(PACKAGE_VERSION);
	QIcon::setFallbackThemeName("gnome");

	kkterminalqt=new KKTerminalQTClass(napp);
	kkterminalqt->forcedGeom=forcedg;
	signal(SIGUSR1,signalHandler);
	signal(SIGUSR2,signalHandler);
	signal(SIGTERM,signalHandler);
	signal(SIGINT,signalHandler);

	kkterminalqt->parser.addHelpOption();
	kkterminalqt->parser.addVersionOption();
	kkterminalqt->parser.addOptions(
		{
			{{"k","key"},"Force key ID.","KeyID"},
			{{"m","multi"},"Force multiple instance."},
			{{"q","quit"},"Quit app."},
			{{"n","new-tab"},"New tab in ARG.","ARG"},
			{{"t","tab"},"New tab in PWD."},
			{{"c","command"},"Execute ARG in new tab.","ARG"},
	});

	kkterminalqt->parser.setApplicationDescription("Snippets file is at ~/.config/kkterminalqt.snippits\nOne command per line as you would type it into the terminal.\neg:\n( cd /usr/share/icons;xdg-open .;ls )\nFile can be edited manually ( use Snippets->Reload snippits file to reload snippet file after editing ).\nAdd the current clipboard to the file and reload automatically ( use Snippets->Add clipboard to snippets ).\nIf you hold down 'SHIFT' key when selecting from Snippits menu no newline is sent but a space is added at the end of the snippit text allowing for params to be entered.");

	kkterminalqt->parser.process(kkterminalqt->application->arguments());
	if((kkterminalqt->parser.isSet("new-tab")) || (kkterminalqt->parser.isSet("tab")) || (kkterminalqt->parser.isSet("command")))
		kkterminalqt->startBlank=true;

	if(kkterminalqt->parser.isSet("key"))
		kkterminalqt->key=kkterminalqt->parser.value("key").toInt(nullptr,0);

	if(kkterminalqt->parser.isSet("multi"))
		{
			srandom(time(NULL));
			kkterminalqt->key=random();
		}
	SingleInstanceClass *siapp=new SingleInstanceClass("KKTerminalQT",kkterminalqt->key);
	if(kkterminalqt->parser.isSet("multi"))
		{
			kkterminalqt->queueID=siapp->queueID;
			kkterminalqt->key=siapp->key;
			kkterminalqt->initApp(argc,argv);
			kkterminalqt->runCLICommands(siapp->queueID);
			kill(getpid(),SIGUSR1);
			kkterminalqt->application->setWindowIcon(QIcon(DATADIR "/pixmaps/" PACKAGE ".png"));
			siapp->isMulti=true;
			shmdt(siapp->queueAddr);
			shmctl(siapp->shmQueueID,IPC_RMID,NULL);
			status=kkterminalqt->application->exec();
			msgctl(siapp->queueID,IPC_RMID,NULL);
			delete kkterminalqt;
			delete siapp;
			return status;
		}

	if(siapp->running==true)
		{
			msgStruct	message;
			int			msglen;
			msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s","XXX");
			message.mType=KKTERMINALQTACTIVATE;
			msgsnd(siapp->queueID,&message,msglen,0);
			kkterminalqt->runCLICommands(siapp->queueID);
			kill(atoi(siapp->queueAddr),SIGUSR1);
			delete kkterminalqt;
			delete siapp;
			return(0);
		}
	else
		{
			kkterminalqt->queueID=siapp->queueID;
			kkterminalqt->key=siapp->key;
		}
     
	kkterminalqt->initApp(argc,argv);
	kkterminalqt->runCLICommands(siapp->queueID);
	kill(atoi(siapp->queueAddr),SIGUSR1);

	kkterminalqt->application->setWindowIcon(QIcon(DATADIR "/pixmaps/" PACKAGE ".png"));

	status=kkterminalqt->application->exec();

	delete kkterminalqt;
	shmctl(siapp->shmQueueID,IPC_RMID,NULL);
	msgctl(siapp->queueID,IPC_RMID,NULL);
	delete siapp;
	return status;
}

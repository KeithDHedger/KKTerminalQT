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
	int				status;
	QDir				commsDir;
	QApplication		*napp=new QApplication(argc,argv);

	napp->setOrganizationName("KDHedger");
	napp->setApplicationName("KKTerminalQT");

	kkterminalqt=new KKTerminalQTClass(napp);
	signal(SIGUSR1,signalHandler);
	signal(SIGTERM,signalHandler);
	signal(SIGINT,signalHandler);

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
		kkterminalqt->key=kkterminalqt->parser.value("key").toInt(nullptr,0);
	if(kkterminalqt->parser.isSet("multi"))
		kkterminalqt->key=0xdeadbeef;

	SingleInstanceClass *siapp=new SingleInstanceClass("KKTerminalQT",kkterminalqt->key);
	if(kkterminalqt->parser.isSet("multi"))
		{
			kkterminalqt->queueID=siapp->queueID;
			kkterminalqt->key=siapp->key;
			//qDebug()<<"mult running...";
			//qDebug()<<"siapp->queueID"<<siapp->queueID;
			//qDebug()<<"siapp->key"<<siapp->key;
			kkterminalqt->initApp(argc,argv);
			kkterminalqt->runCLICommands(siapp->queueID);
			kill(getpid(),SIGUSR1);
			kkterminalqt->application->setWindowIcon(QIcon(DATADIR "/pixmaps/" PACKAGE ".png"));
			status=kkterminalqt->application->exec();
			delete kkterminalqt;
			delete siapp;
			return status;
		}

	if(siapp->running==true)
		{
			//qDebug()<<"already running...";
			//qDebug()<<"siapp->queueID"<<siapp->queueID;
			//qDebug()<<"siapp->key"<<siapp->key;
			msgStruct	message;
			int			msglen;
			msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s","XXX");
			message.mType=KKTERMINALQTACTIVATE;
			msgsnd(siapp->queueID,&message,msglen,0);
			kkterminalqt->runCLICommands(siapp->queueID);
			siapp->sh->attach();
			siapp->sh->lock();
				char *from=(char*)siapp->sh->data();
				kill(atoi(from),SIGUSR1);
			siapp->sh->unlock();
			delete kkterminalqt;
			delete siapp;
			return(0);
		}
	else
		{
			kkterminalqt->queueID=siapp->queueID;
			kkterminalqt->key=siapp->key;
			//qDebug()<<"kkterminalqt->queueID"<<kkterminalqt->queueID;
			//qDebug()<<"kkterminalqt->key"<<kkterminalqt->key;
		}

	kkterminalqt->initApp(argc,argv);
	kkterminalqt->runCLICommands(siapp->queueID);
	siapp->sh->attach();
	siapp->sh->lock();
		char *from=(char*)siapp->sh->data();
		kill(atoi(from),SIGUSR1);
	siapp->sh->unlock();

	kkterminalqt->application->setWindowIcon(QIcon(DATADIR "/pixmaps/" PACKAGE ".png"));

	status=kkterminalqt->application->exec();

	delete kkterminalqt;
	delete siapp;
	return status;
}

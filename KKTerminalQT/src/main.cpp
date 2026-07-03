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
	QApplication		*napp;
	int				status;
	QDir				commsDir;
	bool				forcemult=false;

	napp=new QApplication(argc,argv);
	napp->setOrganizationName("KDHedger");
	napp->setApplicationName(PACKAGE_NAME);
	napp->setApplicationVersion(PACKAGE_VERSION);

	kkterminalqt=new KKTerminalQTClass(napp);
	signal(SIGUSR1,signalHandler);
	signal(SIGUSR2,signalHandler);
	signal(SIGTERM,signalHandler);
	signal(SIGINT,signalHandler);

	option	long_options[]=
		{
			{"key",required_argument,NULL,'k'},
			{"multi",no_argument,NULL,'m'},
			{"quit",no_argument,NULL,'q'},
			{"new-tab",required_argument,NULL,'n'},
			{"tab",no_argument,NULL,'t'},
			{"command",required_argument,NULL,'c'},
			{"theme",required_argument,NULL,'f'},
			{0,0,0,0}
		};
//no_argument
//required_argument
//optional_argument
//check cli args
{
	bool parse;

	parse=kkterminalqt->cliargs.doCliArgs(argc,argv,long_options);

	if(parse==false)
		{
			fprintf(stderr,"%s",helpText);
//			fprintf(stderr,"%s","\
//Usage: kkterminalqt [options]\n\
//Snippets file is at ~/.config/kkterminalqt.snippits\n\
//One command per line as you would type it into the terminal.\n\
//eg:\n\
//( cd /usr/share/icons;xdg-open .;ls )\n\
//File can be edited manually ( use Snippets->Reload snippits file to reload snippet file after editing ).\n\
//Add the current clipboard to the file and reload automatically ( use Snippets->Add Clipboard to snippets ).\n\
//Add the current selection to the file and reload automatically ( use Snippets->Add Selection to snippets ).\n\
//If you hold down 'SHIFT' key when selecting from Snippits menu no newline is sent but a space is added at the end of the snippit text allowing for params to be entered.\n\
//\n\
//Options:\n\
//  -h, --help           Displays help on commandline options.\n\
//  -v, --version        Displays version information.\n\
//  -k, --key <KeyID>    Force key ID.\n\
//  -m, --multi          Force multiple instance.\n\
//  -q, --quit           Quit app.\n\
//  -n, --new-tab <ARG>  New tab in ARG.\n\
//  -t, --tab            New tab in PWD.\n\
//  -c, --command <ARG>  Execute ARG in new tab.\n\
//  -f, --theme <ARG>    Change theme to ARG.\n");

			exit(0);
		}

	if(kkterminalqt->cliargs.prefsData.contains(kkterminalqt->cliargs.hashFromKey("key")))
		{
			kkterminalqt->key=kkterminalqt->cliargs.getPrefValue("key").toString().toInt(nullptr,0);;
		}

	if(kkterminalqt->cliargs.prefsData.contains(kkterminalqt->cliargs.hashFromKey("multi")))
		{
			srandom(time(NULL));
			kkterminalqt->key=random();
			forcemult=true;
		}

	if(kkterminalqt->cliargs.prefsData.contains(kkterminalqt->cliargs.hashFromKey("command")) || kkterminalqt->cliargs.prefsData.contains(kkterminalqt->cliargs.hashFromKey("new-tab")) || kkterminalqt->cliargs.prefsData.contains(kkterminalqt->cliargs.hashFromKey("tab")))
		{
			kkterminalqt->startBlank=true;
		}
}

	SingleInstanceClass *siapp=new SingleInstanceClass("KKTerminalQT",kkterminalqt->key);
	fprintf(stderr,"msgKey=0x%x shmKey=0x%x\n",siapp->key,siapp->shmKey);

	if(forcemult==true)
		{
			kkterminalqt->queueID=siapp->queueID;
			kkterminalqt->key=siapp->key;
			kkterminalqt->shmKey=siapp->shmKey;
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
			kkterminalqt->shmKey=siapp->shmKey;
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

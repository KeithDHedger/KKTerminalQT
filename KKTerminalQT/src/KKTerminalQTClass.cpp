/*
 *
 * ©K. D. Hedger. Thu 17 Oct 13:54:05 BST 2024 keithdhedger@gmail.com

 * This file (KKTerminalQTClass.cpp) is part of KKTerminalQT.

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

#include "KKTerminalQTClass.h"
#ifndef moc_KKTerminalQTClass
#include "moc_KKTerminalQTClass.cpp"
#define moc_KKTerminalQTClass
#endif

KKTerminalQTClass::KKTerminalQTClass(QApplication *app)
{
	this->application=app;
}

KKTerminalQTClass::~KKTerminalQTClass()
{
	if(this->mainWindow!=NULL)
		this->writeExitData();
}

void KKTerminalQTClass::buildMainGui(void)
{
	QAction	*menuitem;

	this->mainWindow=new QMainWindow();
	this->mainNotebook=new QTabWidget(this->mainWindow);
	this->menuBar=new QMenuBar;

	this->fileMenu=new QMenu("&File");
	this->menuBar->addMenu(this->fileMenu);
	this->mainNotebook->setTabsClosable(true);
	this->mainNotebook->setMovable(true);
	QObject::connect(this->mainNotebook,&QTabWidget::tabCloseRequested,[this](int index)
		{
			if(this->mainNotebook->count()==0)
				this->currentConsole=NULL;
			delete this->mainNotebook->widget(index);
		});
	QObject::connect(this->mainNotebook,&QTabWidget::currentChanged,[this](int index)
		{
			if(this->currentConsole!=NULL)
				{
					QTermWidget *t=qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()));
					if(t!=NULL)
						this->mainWindow->setWindowTitle(t->workingDirectory());
			}
		});
	
//new
	menuitem=new QAction(QIcon::fromTheme("document-new"),"&New");
	this->fileMenu->addAction(menuitem);
	menuitem->setShortcut(QKeySequence::New);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			this->addTerminal();
			this->mainNotebook->setCurrentIndex(this->mainNotebook->count()-1);
		});

//close
	menuitem=new QAction(QIcon::fromTheme("window-close"),"&Close");
	this->fileMenu->addAction(menuitem);
	menuitem->setShortcut(QKeySequence::Close);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			delete this->mainNotebook->widget(this->mainNotebook->currentIndex());
			if(this->mainNotebook->count()==0)
				this->currentConsole=NULL;
	});

//prefs
	menuitem=new QAction(QIcon::fromTheme("preferences-desktop"),"&Preferences");
	this->fileMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			miniPrefsReturnStruct myprefs;

			myprefs=this->miniPrefsDialog("KKTerminalQT",QStringList()<<"Theme"<<"Font");
			myprefs.boxes[0]->setText(this->theme);
			myprefs.boxes[1]->setText(this->font.toString());
			int res=myprefs.theDialog->exec();
			if(res==1)
				{
					this->theme=myprefs.boxes[0]->text();
					qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->setColorScheme(this->theme);
					this->font.fromString(myprefs.boxes[1]->text());
					qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->setTerminalFont(this->font);
					QSettings	prefs;
					prefs.setValue("Theme",myprefs.boxes[0]->text());
					prefs.setValue("Font",myprefs.boxes[1]->text());
				}
			delete myprefs.theDialog;
		});
//quit
	menuitem=new QAction(QIcon::fromTheme("application-exit"),"&Quit");
	this->fileMenu->addAction(menuitem);
	menuitem->setShortcut(QKeySequence::Quit);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			this->application->quit();
		});

//edit menu
	this->editMenu=new QMenu("&Edit");
	this->menuBar->addMenu(this->editMenu);
//copy
	menuitem=new QAction(QIcon::fromTheme("edit-copy"),"&Copy");
	menuitem->setShortcut(QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_C));
	this->editMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->copyClipboard();
		});
//paste
	menuitem=new QAction(QIcon::fromTheme("edit-paste"),"&Paste");
	menuitem->setShortcut(QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_V));
	this->editMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->pasteClipboard();
		});
//clear
	menuitem=new QAction(QIcon::fromTheme("edit-clear"),"C&lear");
	this->editMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->clear();
		});

//help menu
	this->helpMenu=new QMenu("&Help");
	this->menuBar->addMenu(this->helpMenu);
//about
	menuitem=new QAction(QIcon::fromTheme("help-about"),"&About");
	this->helpMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			QMessageBox::about(this->mainWindow,"KKTerminalQT","QT version of KKTerminal<br><br><a href=\"https://keithdhedger.github.io\">Website</a><br><br><a href=\"mailto:keithdhedger@gmail.com\">Mail Me</a>");
		});
//about qt
	this->menuBar->addMenu(this->helpMenu);
	menuitem=new QAction(QIcon::fromTheme("help-about"),"About &QT");
	this->helpMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			this->application->aboutQt();
		});
//help
	this->menuBar->addMenu(this->helpMenu);
	menuitem=new QAction(QIcon::fromTheme("help-contents"),"&Help");
	this->helpMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->sendText("kkterminalqt -h\n");
		});

	this->mainWindow->setMenuBar(this->menuBar);
	this->mainWindow->setCentralWidget(this->mainNotebook);

	if(this->startBlank==false)
		this->addTerminal();
}

miniPrefsReturnStruct KKTerminalQTClass::miniPrefsDialog(QString prefsname,QStringList items)
{
	miniPrefsReturnStruct	prefs;
	QSettings				miniprefsname("KDHedger",prefsname);
	QWidget					*hbox;
	QVBoxLayout				*docvlayout=new QVBoxLayout;
	QHBoxLayout				*hlayout;
	QPushButton				*cancelbutton=new QPushButton("&Cancel");
	QPushButton				*okbutton=new QPushButton("&Apply");
	QString					prefsentry;

	prefs.theDialog=new QDialog();

	QObject::connect(cancelbutton,&QPushButton::clicked,[this,prefs]()
		{
			prefs.theDialog->reject();
		});
	QObject::connect(okbutton,&QPushButton::clicked,[this,prefs]()
		{
			prefs.theDialog->accept();
		});

	for(int j=0;j<items.size();j++)
		{
			hbox=new QWidget;
			hlayout=new QHBoxLayout;
			hlayout->setContentsMargins(0,0,0,0);
			hbox->setLayout(hlayout);
			hlayout->addWidget(new QLabel(items.at(j)),0,Qt::AlignLeft);
			prefsentry=items.at(j).trimmed();
			prefsentry.replace(QRegularExpression(" "),"");
			prefs.boxes[j]=new QLineEdit(miniprefsname.value(prefsentry,"").toString().simplified());
			hlayout->addWidget(prefs.boxes[j],1,Qt::AlignRight);
			docvlayout->addWidget(hbox);
		}
	hbox=new QWidget;
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,0,0,0);
	hbox->setLayout(hlayout);
	hlayout->addWidget(cancelbutton);
	hlayout->addStretch(0);
	hlayout->addWidget(okbutton);

	docvlayout->addWidget(hbox);

	prefs.theDialog->setLayout(docvlayout);
	return(prefs);
}

void KKTerminalQTClass::addTerminal(void)
{
	QTermWidget		*newconsole;
	newconsole=new QTermWidget(0,this->mainWindow);
	QObject::connect(newconsole,&QTermWidget::receivedData,[this,newconsole](const QString text)
		{
			if(this->currentConsole==newconsole)
				this->mainWindow->setWindowTitle(newconsole->workingDirectory());
		});
	QObject::connect(newconsole,&QTermWidget::termGetFocus,[this,newconsole]()
		{
			this->mainWindow->setWindowTitle(newconsole->workingDirectory());
			this->currentConsole=newconsole;
		});
	newconsole->setContextMenuPolicy(Qt::CustomContextMenu);
	QObject::connect(newconsole,&QWidget::customContextMenuRequested,[this,newconsole](const QPoint pos)
		{
			QPoint	globalpos=newconsole->mapToGlobal(pos);
			QMenu	cmenu;
			QAction	copyitem(QIcon::fromTheme("edit-copy"),"Copy");
			QAction	pasteitem(QIcon::fromTheme("edit-paste"),"Paste");
			QAction	pasteinitem(QIcon::fromTheme("edit-paste"),"Paste In Quotes");
			QAction	clearitem(QIcon::fromTheme("edit-clear"),"Clear");

			copyitem.setData(QVariant(101));
			cmenu.addAction(&copyitem);
			pasteitem.setData(QVariant(102));
			cmenu.addAction(&pasteitem);
			pasteinitem.setData(QVariant(103));
			cmenu.addAction(&pasteinitem);
			clearitem.setData(QVariant(104));
			cmenu.addAction(&clearitem);

			QAction	*selecteditem=cmenu.exec(globalpos);
			if(selecteditem!=NULL)
				{
					switch(selecteditem->data().toInt())
						{
							case 101:
							newconsole->copyClipboard();
							break;
							case 102:
								newconsole->pasteClipboard();
								break;
							case 103:
								newconsole->sendText(QString("'%1'").arg(QGuiApplication::clipboard()->text(QClipboard::Clipboard)));
								break;
							case 104:
								newconsole->clear();
								break;
						}
				}
	});

	newconsole->setColorScheme(this->theme);
	newconsole->setTerminalFont(this->font);
	newconsole->setKeyBindings("linux");
	newconsole->setScrollBarPosition(QTermWidget::ScrollBarRight);
	int tn=this->mainNotebook->addTab(newconsole,QString("Terminal %1").arg(this->termNumber++));
	if(this->currentConsole!=NULL)
		{
			newconsole->setWorkingDirectory(this->currentConsole->workingDirectory());
		}

	newconsole->startShellProgram();
	newconsole->setFocus();
}

void KKTerminalQTClass::handleSignal(int signum)
{
	switch(signum)
		{
			case SIGUSR1:
				this->doTimer();
				break;
			case SIGTERM:
			case SIGINT:
				this->application->quit();
				break;
			default:
				break;
		}
}

void KKTerminalQTClass::runCLICommands(int quid)
{
	msgStruct	message;
	int			msglen;
	QString		opensessionname;
	QStringList	list;
	char			*pathtopwd;

	//qDebug()<<"runCLICommands"<<this->parser.optionNames();
	if(this->parser.isSet("quit"))
		{
 			msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s","quit");
			message.mType=KKTERMINALQTQUIT;
			msgsnd(quid,&message,msglen,0);
		}

	for(int j=0;j<this->parser.values("new-tab").size();j++)
		{
 			msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s",this->parser.values("new-tab").at(j).toStdString().c_str());
			message.mType=KKTERMINALQTNEW;
			msgsnd(quid,&message,msglen,0);
		}
	
	for(int j=0;j<this->parser.optionNames().size();j++)
		{
			if((this->parser.optionNames().at(j).compare("tab")==0) || (this->parser.optionNames().at(j).compare("t")==0))
				{
 					msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s",getenv("PWD"));
					message.mType=KKTERMINALQTNEWHERE;
					msgsnd(quid,&message,msglen,0);
				}
		}

	for(int j=0;j<this->parser.values("command").size();j++)
		{
 			msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s\n",this->parser.value("command").toStdString().c_str());
			message.mType=KKTERMINALQTCOMMAND;
			msgsnd(quid,&message,msglen,0);
		}
}

void KKTerminalQTClass::doTimer(void)
{
	int			retcode=0;
	msgStruct	buffer;

	while(retcode!=-1)
		{
			buffer.mText[0]=0;
			retcode=msgrcv(this->queueID,&buffer,MAXMSGSIZE,MSGANY,IPC_NOWAIT);
			buffer.mText[retcode]=0;
			if(retcode!=-1)
				{
					switch(buffer.mType)
						{
							case KKTERMINALQTQUIT:
								this->application->quit();
								break;
							case KKTERMINALQTNEW:
								this->addTerminal();
								this->mainNotebook->setCurrentIndex(this->mainNotebook->count()-1);
								qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->changeDir(buffer.mText);
								break;
							case KKTERMINALQTNEWHERE:
								this->addTerminal();
								this->mainNotebook->setCurrentIndex(this->mainNotebook->count()-1);
								qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->changeDir(buffer.mText);
								break;
							case KKTERMINALQTCOMMAND:
								this->addTerminal();
								this->mainNotebook->setCurrentIndex(this->mainNotebook->count()-1);
								qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->sendText(buffer.mText);
								break;
							case KKTERMINALQTACTIVATE:
								this->mainWindow->activateWindow();
								this->mainWindow->raise();
								break;
						}
				}
		}
}

void KKTerminalQTClass::initApp(int argc,char** argv)
{
	QRect	r(0,0,800,400);
	QSettings					prefs;

	qDebug()<<QTermWidget::availableColorSchemes();

	this->theme=prefs.value("Theme","Linux").toString();
	this->font.fromString(prefs.value("Font").toString());
	this->buildMainGui();

	r=prefs.value("app/geometry",QVariant(QRect(50,50,1024,768))).value<QRect>();
	this->mainWindow->setGeometry(r);

	this->checkMessages=new QTimer();
	QObject::connect(this->checkMessages,&QTimer::timeout,[this]()
		{
			this->doTimer();
		});

	this->checkMessages->start(this->prefsMsgTimer);

	this->mainWindow->show();
}

void KKTerminalQTClass::writeExitData(void)
{
	QRect		rg;
	QRect		rf;
	QSettings	prefs;

	rg=this->mainWindow->geometry();
	rf=this->mainWindow->frameGeometry();
	rf.setHeight(rf.height()-(rf.height()-rg.height()));
	rf.setWidth(rf.width()-(rf.width()-rg.width()));
	prefs.setValue("app/geometry",rf);
}

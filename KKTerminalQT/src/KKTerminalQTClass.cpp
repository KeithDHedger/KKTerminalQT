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
	delete this->mainWindow;
}

void KKTerminalQTClass::rebuildSnips(void)
{
	QAction	*menuitem;
	this->snipsMenu->clear();
	menuitem=new QAction("Add Clipboard to snippits",this->snipsMenu);
	this->snipsMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this](bool checked)
		{
			if(this->application->clipboard()->text(QClipboard::Clipboard).isEmpty()==false)
				{
					QFile file(QString("%1/.config/kkterminalqt.snippits").arg(getenv("HOME")));
					if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
						{
							QTextStream out(&file);
							out <<this->application->clipboard()->text(QClipboard::Clipboard)<<Qt::endl;
							file.close();
						}
					else
						return;
				}
			this->rebuildSnips();
		});
	menuitem=new QAction("Add Selection to snippits",this->snipsMenu);
	this->snipsMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this](bool checked)
		{
			if(this->application->clipboard()->text(QClipboard::Selection).isEmpty()==false)
				{
					QFile file(QString("%1/.config/kkterminalqt.snippits").arg(getenv("HOME")));
					if(file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
						{
							QTextStream out(&file);
							out <<this->application->clipboard()->text(QClipboard::Selection)<<Qt::endl;
							file.close();
						}
					else
						return;
				}
			this->rebuildSnips();
		});

	menuitem=new QAction("Reload snippits file",this->snipsMenu);
	this->snipsMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this](bool checked)
		{
			this->rebuildSnips();
		});

	this->snipsMenu->addSeparator();
	QFile file(QString("%1/.config/kkterminalqt.snippits").arg(getenv("HOME")));
	QString snips;
	if(file.open(QFile::ReadOnly | QFile::Text))
		{
			QAction	*menuitem;
			while(!file.atEnd())
				{
					snips=file.readLine().trimmed();
					if(snips.isEmpty()==false)
						{
							menuitem=new QAction(snips,this->snipsMenu);
							this->snipsMenu->addAction(menuitem);
							QObject::connect(menuitem,&QAction::triggered,[this,menuitem](bool checked)
								{
									QChar	term=' ';
									if(QGuiApplication::keyboardModifiers()!=Qt::ShiftModifier)
										term='\n';

									qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->sendText(QString("%1%2").arg(menuitem->text()).arg(term));
								});
						}
				}
			file.close();
		}
}

void KKTerminalQTClass::buildMainGui(void)
{
	QAction	*menuitem;

	this->mainWindow=new QMainWindow;
	this->mainNotebook=new QTabWidget(this->mainWindow);
	this->menuBar=new QMenuBar;

	this->fileMenu=new QMenu("&File",this->menuBar);
	this->menuBar->addMenu(this->fileMenu);
	this->mainNotebook->setTabsClosable(true);
	this->mainNotebook->setMovable(true);
	QObject::connect(this->mainNotebook,&QTabWidget::tabCloseRequested,[this](int index)
		{
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
	menuitem=new QAction(QIcon::fromTheme("document-new"),"&New",this->fileMenu);
	this->fileMenu->addAction(menuitem);
	menuitem->setShortcut(QKeySequence::New);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			this->addTerminal();
			this->mainNotebook->setCurrentIndex(this->mainNotebook->count()-1);
		});

//close
	menuitem=new QAction(QIcon::fromTheme("window-close"),"&Close",this->fileMenu);
	this->fileMenu->addAction(menuitem);
	menuitem->setShortcut(QKeySequence::Close);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			delete this->mainNotebook->widget(this->mainNotebook->currentIndex());
			if(this->mainNotebook->count()==0)
				this->currentConsole=NULL;
	});

//prefs
	menuitem=new QAction(QIcon::fromTheme("preferences-desktop"),"&Preferences",this->fileMenu);
	this->fileMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			QStringList	prfs;
			QStringList	themes=QTermWidget::availableColorSchemes();
			prefsClass	newprefs;

			themes.sort();
			prfs<<"combostart"<<"term/Theme"<<"Linux"<<themes<<"comboend"<<"font"<<"term/Font"<<"Monospace,10,-1,5,50,0,0,0,0,0"<<"check"<<"term/Blink Cursor"<<"0"<<"check"<<"term/Confirm Paste"<<"0"<<"check"<<"term/Close Tab On Exit"<<"0";
			newprefs.createDialog("KKTerminalQT Prefs Class",prfs);
			if(newprefs.dialogPrefs.valid==true)
				{
					this->theme=newprefs.dialogPrefs.comboBoxes.value(THEMEBOX)->currentText();
					qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->setColorScheme(this->theme);
					this->font.fromString(newprefs.dialogPrefs.fontBoxes.value(FONTBOX)->text());
					qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->setTerminalFont(this->font);
					this->blinkCursor=newprefs.dialogPrefs.checkBoxes.value(BLINKBOX)->isChecked();
					qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->setBlinkingCursor(this->blinkCursor);
					this->confirmMLPaste=newprefs.dialogPrefs.checkBoxes.value(CONFIRMPASTEBOX)->isChecked();
					qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->setConfirmMultilinePaste(this->confirmMLPaste);
					this->closeTabOnExit=newprefs.dialogPrefs.checkBoxes.value(CLOSETABONEXIT)->isChecked();					
					newprefs.writePrefs();
				}
		});
//quit
	menuitem=new QAction(QIcon::fromTheme("application-exit"),"&Quit",this->fileMenu);
	this->fileMenu->addAction(menuitem);
	menuitem->setShortcut(QKeySequence::Quit);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			this->application->quit();
		});
//edit menu
	this->editMenu=new QMenu("&Edit",this->menuBar);
	this->menuBar->addMenu(this->editMenu);
//copy
	menuitem=new QAction(QIcon::fromTheme("edit-copy"),"&Copy",this->editMenu);
	menuitem->setShortcut(QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_C));
	this->editMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->copyClipboard();
		});
//paste
	menuitem=new QAction(QIcon::fromTheme("edit-paste"),"&Paste",this->editMenu);
	menuitem->setShortcut(QKeySequence(Qt::CTRL|Qt::SHIFT|Qt::Key_V));
	this->editMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->pasteClipboard();
		});
//clear
	menuitem=new QAction(QIcon::fromTheme("edit-clear"),"C&lear",this->editMenu);
	this->editMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->clear();
		});

//snipits menu
	this->snipsMenu=new QMenu("&Snippets",this->menuBar);
	this->menuBar->addMenu(this->snipsMenu);
	this->rebuildSnips();

//help menu
	this->helpMenu=new QMenu("&Help",this->menuBar);
	this->menuBar->addMenu(this->helpMenu);
//about
	menuitem=new QAction(QIcon::fromTheme("help-about"),"&About",this->helpMenu);
	this->helpMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			QMessageBox::about(this->mainWindow,"KKTerminalQT",PACKAGE_STRING "<br>QT version of KKTerminal<br><br><a href=\"https://keithdhedger.github.io\">Website</a><br><br><a href=\"mailto:keithdhedger@gmail.com\">Mail Me</a>");
		});
//about qt
	this->menuBar->addMenu(this->helpMenu);
	menuitem=new QAction(QIcon::fromTheme("help-about"),"About &QT",this->helpMenu);
	this->helpMenu->addAction(menuitem);
	QObject::connect(menuitem,&QAction::triggered,[this]()
		{
			this->application->aboutQt();
		});
//help
	this->menuBar->addMenu(this->helpMenu);
	menuitem=new QAction(QIcon::fromTheme("help-contents"),"&Help",this->helpMenu);
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

	QObject::connect(newconsole,&QTermWidget::finished,[this,newconsole]()
		{
			if(this->closeTabOnExit==false)
				return;
			for(int j=0;j<this->mainNotebook->count();j++)
				{
					if(this->mainNotebook->widget(j)==newconsole)
						this->mainNotebook->removeTab(j);
				}
		});
	newconsole->setContextMenuPolicy(Qt::CustomContextMenu);
	newconsole->setBlinkingCursor(this->blinkCursor);
	newconsole->setConfirmMultilinePaste(this->confirmMLPaste);

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

			cmenu.addSeparator();
			cmenu.addMenu(this->snipsMenu);

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

	newconsole->addCustomColorSchemeDir(DATADIR "/themes");
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
			case SIGUSR2:
				fprintf(stderr,"msgKey=0x%x\n",this->key);
				break;
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
 			msglen=snprintf(message.mText,MAXMSGSIZE-1,"%s",this->parser.value("command").toStdString().c_str());
			message.mType=KKTERMINALQTNEWCOMMAND;
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
							case KKTERMINALQTRELOADTHEME:
								{//TODO//to complete
									this->newprefs.readPrefs();
									this->theme=this->newprefs.getPrefValue("term/Theme").toString();
									for(int j=0;j<this->mainNotebook->count();j++)
										{
											this->mainNotebook->setCurrentIndex(j);
											qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->setColorScheme(this->theme);
											//qDebug()<<this->newprefs.getPrefValue("term/Theme").toString()<<this->theme<<QString(buffer.mText);
										}
								}
								break;
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
							case KKTERMINALQTNEWCOMMAND:
								this->addTerminal();
								this->mainNotebook->setCurrentIndex(this->mainNotebook->count()-1);
								qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->sendText(QString("%1\n").arg(buffer.mText));
								break;
							case KKTERMINALSENDTEXT:
								qobject_cast<QTermWidget*>(this->mainNotebook->widget(this->mainNotebook->currentIndex()))->sendText(QString("%1\n").arg(buffer.mText));
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
	QRect		r(100,100,800,400);
	QStringList	prfs={"term/Font","term/Theme","term/BlinkCursor","term/ConfirmPaste","app/geometry","term/CloseTabOnExit"};
	//#prefsClass	newprefs;

	this->newprefs.setPrefs(prfs);
	this->newprefs.setPrefValue("term/Font",QVariant("Monospace,10,-1,5,50,0,0,0,0,0").toString());
	this->newprefs.setPrefValue("term/Theme",QVariant("Linux").toString());
	this->newprefs.setPrefValue("term/BlinkCursor",QVariant(false).toBool());
	this->newprefs.setPrefValue("term/ConfirmPaste",QVariant(false).toBool());
	this->newprefs.setPrefValue("term/CloseTabOnExit",QVariant(false).toBool());
	this->newprefs.setPrefValue("app/geometry",QVariant(r));
	this->newprefs.readPrefs();
	this->theme=this->newprefs.getPrefValue("term/Theme").toString();
	this->font.fromString(this->newprefs.getPrefValue("term/Font").toString());
	this->blinkCursor=this->newprefs.getPrefValue("term/BlinkCursor").toBool();
	this->confirmMLPaste=this->newprefs.getPrefValue("term/ConfirmPaste").toBool();
	this->closeTabOnExit=this->newprefs.getPrefValue("term/CloseTabOnExit").toBool();
	r=this->newprefs.getPrefValue("app/geometry").toRect();

	this->buildMainGui();
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

	if(this->forcedGeom==true)
		return;
	rg=this->mainWindow->geometry();
	rf=this->mainWindow->frameGeometry();
	rf.setHeight(rf.height()-(rf.height()-rg.height()));
	rf.setWidth(rf.width()-(rf.width()-rg.width()));
	prefs.setValue("app/geometry",rf);
}

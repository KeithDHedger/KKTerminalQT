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
}

void KKTerminalQTClass::buildMainGui(void)
{
	this->mainWindow=new QMainWindow();
	this->mainNotebook=new QTabWidget(this->mainWindow);
	this->menuBar=new QMenuBar;

	this->fileMenu=new QMenu("&File");
	this->menuBar->addMenu(this->fileMenu);
	this->mainNotebook->setTabsClosable(true);
	QObject::connect(this->mainNotebook,&QTabWidget::tabCloseRequested,[this](int index)
		{
			delete this->mainNotebook->widget(index);
		});
	
//new
	this->newMenuItem=new QAction(QIcon::fromTheme("document-new"),"&New");
	this->fileMenu->addAction(this->newMenuItem);
	this->newMenuItem->setShortcut(QKeySequence::New);
	QObject::connect(this->newMenuItem,&QAction::triggered,[this]()
		{
			this->addTerminal();
			this->mainNotebook->setCurrentIndex(this->mainNotebook->count()-1);
		});

//close
	this->closeMenuItem=new QAction(QIcon::fromTheme("document-close"),"&Close");
	this->fileMenu->addAction(this->closeMenuItem);
	this->closeMenuItem->setShortcut(QKeySequence::Close);
	QObject::connect(this->closeMenuItem,&QAction::triggered,[this]()
		{
			delete this->mainNotebook->widget(this->mainNotebook->currentIndex());
		});
//prefs
	this->prefsMenuItem=new QAction(QIcon::fromTheme("preferences-desktop"),"&Preferences");
	this->fileMenu->addAction(this->prefsMenuItem);
	QObject::connect(this->prefsMenuItem,&QAction::triggered,[this]()
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
	this->quitMenuItem=new QAction(QIcon::fromTheme("application-exit"),"&Quit");
	this->fileMenu->addAction(this->quitMenuItem);
	this->quitMenuItem->setShortcut(QKeySequence::Quit);
	QObject::connect(this->quitMenuItem,&QAction::triggered,[this]()
		{
			this->application->quit();
		});

	this->mainWindow->setMenuBar(this->menuBar);
	this->mainWindow->setCentralWidget(this->mainNotebook);

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
	newconsole->setColorScheme(this->theme);
	newconsole->setTerminalFont(this->font);

	newconsole->setScrollBarPosition(QTermWidget::ScrollBarRight);
	int tn=this->mainNotebook->addTab(newconsole,QString("Terminal %1").arg(this->termNumber++));
	newconsole->startShellProgram ();
}

void KKTerminalQTClass::runCLICommands(int quid)
{
	qDebug()<<"runCLICommands";
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

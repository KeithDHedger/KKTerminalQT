/*
 *
 * ©K. D. Hedger. Fri  1 Nov 13:52:11 GMT 2024 keithdhedger@gmail.com

 * This file (this->dialogPrefsClass.cpp) is part of Classes-DEV.

 * Classes-DEV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * Classes-DEV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with Classes-DEV.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "prefsClass.h"

/**
* this->dialogPrefsClass class destroy.
*/
prefsClass::~prefsClass()
{
	for(int j=0;j<this->dialogPrefs.comboBoxCnt;j++)
		delete this->dialogPrefs.comboBoxes[j];
				
	for(int j=0;j<this->dialogPrefs.editBoxCnt;j++)
		delete this->dialogPrefs.editBoxes[j];

	for(int j=0;j<this->dialogPrefs.checkBoxCnt;j++)
		delete this->dialogPrefs.checkBoxes[j];

	for(int j=0;j<this->dialogPrefs.colourBoxCnt;j++)
		delete this->dialogPrefs.colourBoxes[j];

	for(int j=0;j<this->dialogPrefs.fontBoxCnt;j++)
		delete this->dialogPrefs.fontBoxes[j];

	delete this->dialogPrefs.theDialog;
}

/**
* this->dialogPrefsClass.
*/
prefsClass::prefsClass()
{
}

QString prefsClass::bestFontColour(QString colour)
{
	QColor	cc(colour);
    int		r=0,g=0,b=0;

	r=cc.red();
	g=cc.green();
	b=cc.blue();

	if((r+r+r+b+g+g+g+g)>>3>128)
		return("black");
	else
		return("white");
}

unsigned long prefsClass::hashFromKey(QString key)
{
	unsigned long hash=0;

	for(int i=0;i<key.length();i++)
		hash=31*hash+key.at(i).toLatin1();

	return(hash);
}

void prefsClass::setPrefValue(QString name,QVariant val)
{
	this->prefsData[this->hashFromKey(name)]=val;
}

QVariant prefsClass::getPrefValue(QString name)
{
	return(this->prefsData.value(this->hashFromKey(name)));
}

void prefsClass::writePrefs(void)
{
	QSettings	defaults;

	for(int j=0;j<this->prefsData.size();j++)
		{
			defaults.setValue(this->prefsNames.at(j),this->prefsData.value(this->hashFromKey(this->prefsNames.at(j))));
		}
}

void prefsClass::readPrefs(void)
{
	QSettings	defaults;

	for(int j=0;j<this->prefsData.size();j++)
		{
			this->prefsData[this->hashFromKey(this->prefsNames.at(j))]=defaults.value(this->prefsNames.at(j),this->prefsData.value(this->hashFromKey(this->prefsNames.at(j))));
		}
}

void prefsClass::setPrefs(QStringList items)
{
	int			j=0;
	QSettings	defaults;

	this->prefsNames=items;
	this->prefsData.clear();

	while(j<items.size())
		{
			this->prefsData[this->hashFromKey(items.at(j))]=QVariant(0);
			j++;
		}
}

void prefsClass::createDialog(QString title,QStringList items)
{
	QWidget					*hbox=NULL;
	QVBoxLayout				*docvlayout=new QVBoxLayout;
	QHBoxLayout				*hlayout=NULL;
	QPushButton				*cancelbutton=new QPushButton("&Cancel");
	QPushButton				*okbutton=new QPushButton("&Apply");
	QString					prefsentry;
	QSettings				defaults;
	int						j=0;
	QString					labelstr;

	this->dialogPrefs.theDialog=new QDialog();
	this->dialogPrefs.theDialog->setWindowTitle(title);
	this->dialogPrefs.theDialog->setGeometry(defaults.value("prefsgeometry",QRect(100,100,320,128)).toRect());
	QObject::connect(cancelbutton,&QPushButton::clicked,[this]()
		{
			this->dialogPrefs.theDialog->reject();
			this->dialogPrefs.valid=false;
		});
	QObject::connect(okbutton,&QPushButton::clicked,[this]()
		{
			QSettings	defaults;
			QRect		rf,rg;

			rg=this->dialogPrefs.theDialog->geometry();
			rf=this->dialogPrefs.theDialog->frameGeometry();
			rf.setHeight(rf.height()-(rf.height()-rg.height()));
			rf.setWidth(rf.width()-(rf.width()-rg.width()));
			defaults.setValue("prefsgeometry",rf);
			this->dialogPrefs.theDialog->accept();
			this->dialogPrefs.valid=true;
		});

	j=0;
	while(j<items.size())
		{
			//combo
			if(items.at(j).compare("combostart")==0)
				{
					hbox=new QWidget;
					hlayout=new QHBoxLayout;
					hlayout->setContentsMargins(0,0,0,0);
					hbox->setLayout(hlayout);
					j++;
					labelstr=items.at(j);
					labelstr.replace(QRegularExpression("^.*/"),"");
					hlayout->addWidget(new QLabel(labelstr),1);
					this->dialogPrefs.comboBoxes[this->dialogPrefs.comboBoxCnt]=new QComboBox();
					prefsentry=items.at(j).trimmed();
					prefsentry.replace(QRegularExpression(" "),"");
					this->dialogPrefs.comboBoxesPrefsName[this->dialogPrefs.comboBoxCnt]=prefsentry;
					j++;
					QString defstr=items.at(j).trimmed();
					j++;
					while(items.at(j).compare("comboend")!=0)
						{
							this->dialogPrefs.comboBoxes[this->dialogPrefs.comboBoxCnt]->addItem(items.at(j));
							j++;
						}
					this->dialogPrefs.comboBoxes[this->dialogPrefs.comboBoxCnt]->setCurrentText(defstr);
					this->dialogPrefs.comboBoxes[this->dialogPrefs.comboBoxCnt]->setCurrentText(defaults.value(this->dialogPrefs.comboBoxesPrefsName[this->dialogPrefs.comboBoxCnt],items.at(j)).toString());
					hlayout->addWidget(this->dialogPrefs.comboBoxes[this->dialogPrefs.comboBoxCnt],1);
					docvlayout->addWidget(hbox);
					this->dialogPrefs.comboBoxCnt++;
				}

			//edits
			if(items.at(j).compare("edit")==0)
				{
					hbox=new QWidget;
					hlayout=new QHBoxLayout;
					hlayout->setContentsMargins(0,0,0,0);
					hbox->setLayout(hlayout);
					j++;
					labelstr=items.at(j);
					labelstr.replace(QRegularExpression("^.*/"),"");
					hlayout->addWidget(new QLabel(labelstr),1);
					prefsentry=items.at(j).trimmed();
					prefsentry.replace(QRegularExpression(" "),"");
					this->dialogPrefs.editBoxesPrefsName[this->dialogPrefs.editBoxCnt]=prefsentry;
					j++;
					this->dialogPrefs.editBoxes[this->dialogPrefs.editBoxCnt]=new QLineEdit(defaults.value(this->dialogPrefs.editBoxesPrefsName[this->dialogPrefs.editBoxCnt],items.at(j)).toString());
					hlayout->addWidget(this->dialogPrefs.editBoxes[this->dialogPrefs.editBoxCnt],1);
					docvlayout->addWidget(hbox);
					this->dialogPrefs.editBoxCnt++;
				}

			//checkboxes
			if(items.at(j).compare("check")==0)
				{
					hbox=new QWidget;
					hlayout=new QHBoxLayout;
					hlayout->setContentsMargins(0,0,0,0);
					hbox->setLayout(hlayout);
					j++;
					prefsentry=items.at(j).trimmed();
					prefsentry.replace(QRegularExpression(" "),"");
					this->dialogPrefs.checkBoxesPrefsName[this->dialogPrefs.checkBoxCnt]=prefsentry;
					labelstr=items.at(j);
					labelstr.replace(QRegularExpression("^.*/"),"");
					this->dialogPrefs.checkBoxes[this->dialogPrefs.checkBoxCnt]=new QCheckBox(labelstr);
					j++;
					this->dialogPrefs.checkBoxes[this->dialogPrefs.checkBoxCnt]->setChecked(defaults.value(this->dialogPrefs.checkBoxesPrefsName[this->dialogPrefs.checkBoxCnt],items.at(j)).toBool());
					hlayout->addWidget(this->dialogPrefs.checkBoxes[this->dialogPrefs.checkBoxCnt],1);
					docvlayout->addWidget(hbox);
					this->dialogPrefs.checkBoxCnt++;
				}

			//colours
			if(items.at(j).compare("colour")==0)
				{
					hbox=new QWidget;
					QWidget *hbox2=new QWidget;
					hlayout=new QHBoxLayout;
					QHBoxLayout *hlayout2=new QHBoxLayout;
					QPushButton *pb;
					int			thisnumber;
					hlayout->setContentsMargins(0,0,0,0);
					hlayout->setSpacing(0);
					hlayout2->setContentsMargins(3,0,0,0);
					hlayout2->setSpacing(0);
					j++;
					labelstr=items.at(j);
					labelstr.replace(QRegularExpression("^.*/"),"");
					hlayout->addWidget(new QLabel(labelstr),2);
					prefsentry=items.at(j).trimmed();
					prefsentry.replace(QRegularExpression(" "),"");
					this->dialogPrefs.colourBoxesPrefsName[this->dialogPrefs.colourBoxCnt]=prefsentry;
					j++;
					pb=new QPushButton("C");
					pb->setMaximumWidth(24);
					hlayout2->addWidget(pb,1);
					this->dialogPrefs.colourBoxes[this->dialogPrefs.colourBoxCnt]=new QLineEdit(defaults.value(this->dialogPrefs.colourBoxesPrefsName[this->dialogPrefs.colourBoxCnt],items.at(j)).toString());	
					this->dialogPrefs.colourBoxes[this->dialogPrefs.colourBoxCnt]->setStyleSheet(QString("background: %1;\ncolor: %2;").arg(this->dialogPrefs.colourBoxes[this->dialogPrefs.colourBoxCnt]->text()).arg(this->bestFontColour(this->dialogPrefs.colourBoxes[this->dialogPrefs.colourBoxCnt]->text())));
					this->dialogPrefs.colourBoxes[this->dialogPrefs.colourBoxCnt]->update();
					thisnumber=this->dialogPrefs.colourBoxCnt;
					QObject::connect(pb,&QPushButton::clicked,[this,pb,thisnumber]()
						{
							QColor colour;
							
							colour=QColorDialog::getColor(this->dialogPrefs.colourBoxes[thisnumber]->text(),nullptr,"Select Colour",QColorDialog::ShowAlphaChannel);
							if(colour.isValid()==true)
								{
									QString s;
									s=QString("background: %1;\ncolor: %2;").arg(colour.name(QColor::HexArgb)).arg(this->bestFontColour(colour.name(QColor::HexArgb)));
									this->dialogPrefs.colourBoxes[thisnumber]->setStyleSheet(s);//ding ding ding
									this->dialogPrefs.colourBoxes[thisnumber]->update();
									this->dialogPrefs.colourBoxes[thisnumber]->setText(colour.name(QColor::HexArgb));
								}
						});
					hlayout2->addWidget(this->dialogPrefs.colourBoxes[this->dialogPrefs.colourBoxCnt],2);
					hbox2->setLayout(hlayout2);
					hbox->setLayout(hlayout);
					hlayout->addWidget(hbox2,2);
					docvlayout->addWidget(hbox);
					this->dialogPrefs.colourBoxCnt++;
				}

			//font
			if(items.at(j).compare("font")==0)
				{
					QWidget		*hbox2=new QWidget;
					QHBoxLayout	*hlayout2=new QHBoxLayout;
					QPushButton	*pb;
					QFont		font;
					int			thisnumber;

					hlayout=new QHBoxLayout;
					hbox=new QWidget;
					hlayout->setContentsMargins(0,0,0,0);
					hlayout->setSpacing(0);
					hlayout2->setContentsMargins(3,0,0,0);
					hlayout2->setSpacing(0);
					j++;
					QString l=items.at(j);
					l.replace(QRegularExpression("^.*/"),"");
					hlayout->addWidget(new QLabel(l),2);
					prefsentry=items.at(j).trimmed();
					prefsentry.replace(QRegularExpression(" "),"");
					this->dialogPrefs.fontBoxesPrefsName[this->dialogPrefs.fontBoxCnt]=prefsentry;
					j++;
					pb=new QPushButton("F");
					pb->setMaximumWidth(24);
					hlayout2->addWidget(pb,1);
					this->dialogPrefs.fontBoxes[this->dialogPrefs.fontBoxCnt]=new QLineEdit(defaults.value(this->dialogPrefs.fontBoxesPrefsName[this->dialogPrefs.fontBoxCnt],items.at(j)).toString());	
					font.fromString(this->dialogPrefs.fontBoxes[this->dialogPrefs.fontBoxCnt]->text());
					this->dialogPrefs.fontBoxes[this->dialogPrefs.fontBoxCnt]->setFont(font);
					this->dialogPrefs.fontBoxes[this->dialogPrefs.fontBoxCnt]->update();
					thisnumber=this->dialogPrefs.fontBoxCnt;
					QObject::connect(pb,&QPushButton::clicked,[this,pb,thisnumber]()
						{
							bool		ok;
							QFont	tfont;
							QFont	font;

							tfont.fromString(this->dialogPrefs.fontBoxes[thisnumber]->text());
							font=QFontDialog::getFont(&ok,tfont,nullptr,"Seleect Font");
							if(ok==true)
								{
									this->dialogPrefs.fontBoxes[thisnumber]->setText(font.toString());
									this->dialogPrefs.fontBoxes[thisnumber]->setFont(font);
									this->dialogPrefs.fontBoxes[thisnumber]->update();
								}
						});
					hlayout2->addWidget(this->dialogPrefs.fontBoxes[this->dialogPrefs.fontBoxCnt],2);
					hbox2->setLayout(hlayout2);
					hbox->setLayout(hlayout);
					hlayout->addWidget(hbox2,2);
					docvlayout->addWidget(hbox);
					this->dialogPrefs.fontBoxCnt++;
				}
			j++;
		}

	hbox=new QWidget;
	hlayout=new QHBoxLayout;
	hlayout->setContentsMargins(0,8,0,0);
	hbox->setLayout(hlayout);
	hlayout->addWidget(cancelbutton);
	hlayout->addStretch(0);
	hlayout->addWidget(okbutton);

	docvlayout->addWidget(hbox);

	this->dialogPrefs.theDialog->setLayout(docvlayout);
	int res=this->dialogPrefs.theDialog->exec();
	if(res==1)
		{
			QSettings	prefs;

			for(int j=0;j<this->dialogPrefs.comboBoxCnt;j++)
				prefs.setValue(this->dialogPrefs.comboBoxesPrefsName[j],this->dialogPrefs.comboBoxes[j]->currentText());
				
			for(int j=0;j<this->dialogPrefs.editBoxCnt;j++)
				prefs.setValue(this->dialogPrefs.editBoxesPrefsName[j],this->dialogPrefs.editBoxes[j]->text());

			for(int j=0;j<this->dialogPrefs.checkBoxCnt;j++)
				prefs.setValue(this->dialogPrefs.checkBoxesPrefsName[j],this->dialogPrefs.checkBoxes[j]->isChecked());

			for(int j=0;j<this->dialogPrefs.colourBoxCnt;j++)
				prefs.setValue(this->dialogPrefs.colourBoxesPrefsName[j],this->dialogPrefs.colourBoxes[j]->text());

			for(int j=0;j<this->dialogPrefs.fontBoxCnt;j++)
				prefs.setValue(this->dialogPrefs.fontBoxesPrefsName[j],this->dialogPrefs.fontBoxes[j]->text());
		}

	return;
}

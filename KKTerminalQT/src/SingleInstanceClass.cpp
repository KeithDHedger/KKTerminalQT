/*
 *
 * ©K. D. Hedger. Thu 17 Oct 14:13:11 BST 2024 keithdhedger@gmail.com

 * This file (SingleInstanceClass.cpp) is part of KKTerminalQT.

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

void* SingleInstanceClass::getX11Prop(Display	*display,Window w,Atom prop,Atom type,int fmt,unsigned long *rcountp)
{
	void				*ptr=NULL;
	unsigned long	count=32;
	Atom				rtype;
	int				rfmt;
	unsigned long	rafter;

	for (;;)
		{
			if (XGetWindowProperty(display,w,prop,0L,count,False,type,&rtype,&rfmt,rcountp,&rafter,(unsigned char **)&ptr) != Success)
				return NULL;
			else if (rtype != type || rfmt != fmt)
				return NULL;
			else if (rafter>0)
				{
					XFree(ptr);
					ptr=NULL;
					count *= 2;
				}
			else
				return ptr;
		}
	return(NULL);
}

long SingleInstanceClass::getSIWorkSpace(Display *display)
{
	if(display!=NULL)
		{
			unsigned long	rootwin=DefaultRootWindow(display);
			unsigned long	n=0;
			Atom				NET_WM_DESKTOP;
			long				*deskp;

			NET_WM_DESKTOP=XInternAtom(display,"_NET_CURRENT_DESKTOP",False);
			deskp=(long*)getX11Prop(display,rootwin,NET_WM_DESKTOP,XA_CARDINAL,32,&n);
			if (n !=0)
				{
					long retval=*deskp;
					XFree(deskp);
					return(retval);
				}
		}
	return(-1);
}

unsigned long SingleInstanceClass::hashFromKey(QString key)
{
	unsigned long hash=0;

	for(int i=0;i<key.length();i++)
		hash=31*hash+key.at(i).toLatin1();

	return(hash);
}

SingleInstanceClass::SingleInstanceClass(QString name,int suppliedkey)
{
	bool		gotsh;
	Display	*display=NULL;
	int		workspace=-1;
	int		screen;
	QString	displaystr;
	QString	keystr;

	this->appName=name;

	display=XOpenDisplay(NULL);
	workspace=getSIWorkSpace(display);
	screen=DefaultScreen(display);
	displaystr=DisplayString(display);

	if(suppliedkey==-1)
		{
			keystr=QString("%1%2%3%4").arg(this->appName).arg(workspace).arg(screen).arg(displaystr);
			this->key=hashFromKey(keystr);
		}
	else
		{
			this->key=suppliedkey;
			keystr=QString("%1%2").arg(this->appName).arg(this->key);
		}

	this->queueID=msgget(this->key,IPC_CREAT|0660);
	//qDebug()<<"key"<<this->key<<"keystr"<<this->keystr<<"queueID"<<this->queueID;

	this->sh=new QSharedMemory(keystr);
	gotsh=sh->create(MAXMSGSIZE,QSharedMemory::ReadWrite);
	if(gotsh==true)
		{
			this->sh->lock();
				char *to=(char*)this->sh->data();
    				sprintf(to,"%i\n",getpid());
    			this->sh->unlock();
    			this->running=false;
			//QMessageBox::about(nullptr,"KKTerminalQT",QString("data for single inst\nworkspace=%1\nscreen=%2\ndisplay str=%3").arg(workspace).arg(screen).arg(this->displaystr));
		}
	else
		{
			this->running=true;
		}
	XCloseDisplay(display);
}

SingleInstanceClass::~SingleInstanceClass()
{
	delete this->sh;
}



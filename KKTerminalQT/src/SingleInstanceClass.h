/*
 *
 * ©K. D. Hedger. Thu 17 Oct 14:12:59 BST 2024 keithdhedger@gmail.com

 * This file (SingleInstanceClass.h) is part of KKTerminalQT.

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

#ifndef _SINGLEINSTANCECLASS_
#define _SINGLEINSTANCECLASS_

#include <X11/Xlib.h>
#include <X11/Xatom.h>

#define SHSIZE 128

class SingleInstanceClass
{
	public:
		SingleInstanceClass(QString name,int suppliedkey=-1);
		~SingleInstanceClass();

		long				getSIWorkSpace(Display *display);
		void				*getX11Prop(Display	*display,Window w,Atom prop,Atom type,int fmt,unsigned long *rcountp);
		unsigned long	hashFromKey(QString key);

		int				key;
		int				queueID=-1;

		int				shmQueueID=-1;
		unsigned long	shmKey=0;
		char				*queueAddr=NULL;
		
		bool				running=false;
	private:
		QString			appName;
};

#endif



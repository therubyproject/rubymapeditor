//////////////////////////////////////////////////////////////////////
// This file is part of Ruby Map Editor
//////////////////////////////////////////////////////////////////////
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//////////////////////////////////////////////////////////////////////
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/threads.h $
// $Id: threads.h 298 2010-02-23 17:09:13Z admin $

#ifndef RME_THREAD_H_
#define RME_THREAD_H_

#include "main.h"

class Thread : public wxThread {
public:
	Thread(wxThreadKind);

	void Execute(); // Calls "Create" and then "Run"
};

class JoinableThread : public Thread {
public:
	JoinableThread() : Thread(wxTHREAD_JOINABLE) {}
};

class DetachedThread : public Thread {
public:
	DetachedThread() : Thread(wxTHREAD_DETACHED) {}
};

inline Thread::Thread(wxThreadKind kind) : wxThread(kind) {}

inline void Thread::Execute() {
	Create();
	Run();
}

#endif

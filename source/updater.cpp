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
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/updater.hpp $
// $Id: updater.hpp 310 2010-02-26 18:03:48Z admin $

#include "main.h"

#ifdef _USE_UPDATER_

#include <wx/url.h>

#include "json.h"

#include "updater.h"

const wxEventType EVT_UPDATE_CHECK_FINISHED = wxNewEventType();

UpdateChecker::UpdateChecker()
{
	////
}

UpdateChecker::~UpdateChecker()
{
	////
}

void UpdateChecker::connect(wxEvtHandler* receiver)
{
	wxString address = "http://www.remeresmapeditor.com/update.php";
	address << "?os=" <<
#ifdef __WINDOWS__
	"windows";
#elif __LINUX__
	"linux";
#else
	"unknown";
#endif
	address << "&verid=" << __RME_VERSION_ID__;
#ifdef __EXPERIMENTAL__
	address << "&beta";
#endif
	wxURL* url = newd wxURL(address);
	UpdateConnectionThread* connection = newd UpdateConnectionThread(receiver, url);
	connection->Execute();
}

UpdateConnectionThread::UpdateConnectionThread(wxEvtHandler* receiver, wxURL* url) :
	receiver(receiver),
	url(url)
{
	////
}

UpdateConnectionThread::~UpdateConnectionThread()
{
	////
}

wxThread::ExitCode UpdateConnectionThread::Entry()
{
	wxInputStream* input = url->GetInputStream();
	if(!input) {
		delete input;
		delete url;
		return 0;
	}

	std::string data;
	while(!input->Eof()) {
		data += input->GetC();
	}

	delete input;
	delete url;
	wxCommandEvent event(EVT_UPDATE_CHECK_FINISHED);
	event.SetClientData(newd std::string(data));
	if(receiver) receiver->AddPendingEvent(event);
	return 0;
}

#endif

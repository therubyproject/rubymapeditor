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
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/about_window.h $
// $Id: about_window.h 298 2010-02-23 17:09:13Z admin $

#ifndef RME_ABOUT_WINDOW_H_
#define RME_ABOUT_WINDOW_H_

#include "main.h"

class GamePanel;

class AboutWindow : public wxDialog {
public:
	AboutWindow(wxWindow* parent);
	~AboutWindow();

	void OnClickOK(wxCommandEvent&);
	void OnClickLicense(wxCommandEvent&);

	void OnTetris(wxCommandEvent&);
	void OnSnake(wxCommandEvent&);
private:
	wxSizer* topsizer;
	GamePanel* game_panel;

	DECLARE_EVENT_TABLE()
};

#endif

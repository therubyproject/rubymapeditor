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
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/iomap.hpp $
// $Id: iomap.hpp 264 2009-10-05 06:36:21Z remere $

#include "main.h"
#include "gui.h"

void IOMap::error(const wxString format, ...)
{
	va_list argp;
	va_start(argp, format);
	errorstr.PrintfV(format, argp);
	va_end(argp);
};

void IOMap::warning(const wxString format, ...)
{
	wxString s;
	va_list argp;
	va_start(argp, format);
	s.PrintfV(format, argp);
	va_end(argp);
	warnings.push_back(s);
};

bool IOMap::queryUser(const wxString& title, const wxString& text)
{
	return g_gui.PopupDialog(title, text, wxYES | wxNO) == wxID_YES;
}

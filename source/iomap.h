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
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/iomap.h $
// $Id: iomap.h 306 2010-02-24 10:38:25Z admin $

#ifndef RME_MAP_IO_H_
#define RME_MAP_IO_H_

#include "client_version.h"

enum ImportType
{
	IMPORT_DONT,
	IMPORT_MERGE,
	IMPORT_SMART_MERGE,
	IMPORT_INSERT,
};

class Map;

class IOMap
{
protected:
	wxArrayString warnings;
	wxString errorstr;

	bool queryUser(const wxString& title, const wxString& format);
	void warning(const wxString format, ...);
	void error(const wxString format, ...);
public:
	IOMap() {
		version = MAP_RBM_1;
	}
	virtual ~IOMap() {}

	MapVersionID version;

	wxArrayString& getWarnings() {return warnings;}
	wxString& getError() {return errorstr;}

	virtual bool loadMap(Map& map, const FileName& identifier) = 0;
	virtual bool saveMap(Map& map, const FileName& identifier) = 0;
};


class VirtualIOMap : public IOMap {
public:
	VirtualIOMap(MapVersionID v) {
		version = v;
	}

	virtual bool loadMap(Map& map, const FileName& identifier) {return false;}
	virtual bool saveMap(Map& map, const FileName& identifier) {return false;}
};

#endif

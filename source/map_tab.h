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
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/map_tab.h $
// $Id: map_tab.h 310 2010-02-26 18:03:48Z admin $

#ifndef RME_MAP_TAB_H_
#define RME_MAP_TAB_H_

#include "editor_tabs.h"
#include "application.h"
#include "map_window.h"

class MapTab : public EditorTab, public MapWindow {
public:
	MapTab(MapTabbook* aui, Editor* editor);
	// Constructs a newd window, but it uses the same internal editor as 'other'
	// AND the same parent, aui_notebook etc.
	MapTab(const MapTab* other);
	~MapTab();

	bool IsUniqueReference() const;
	bool HasSameReference(MapTab* other) const;

	// Properties
	MapWindow* GetView() const;
	MapCanvas* GetCanvas() const;
	wxWindow* GetWindow() const;
	wxString GetTitle() const;
	Editor* GetEditor() const;
	Map* GetMap() const;

	// Event handlers
	void OnSwitchEditorMode(EditorMode mode);

protected:
	struct InternalReference {
		Editor* editor;
		int owner_count;
	};
	MapTabbook* aui;
	InternalReference* iref;
};

inline bool MapTab::HasSameReference(MapTab* other) const {
	return other->iref == iref;
}

#endif

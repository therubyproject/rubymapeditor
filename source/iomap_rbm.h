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
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/iomap_rbm.h $
// $Id: iomap_rbm.h 298 2010-02-23 17:09:13Z admin $

#ifndef RME_RBM_MAP_IO_H_
#define RME_RBM_MAP_IO_H_

#include "iomap.h"

// Pragma pack is VERY important since otherwise it won't be able to load the structs correctly
#pragma pack(1)

enum RBM_ItemAttribute
{
	RBM_ATTR_DESCRIPTION = 1,
	RBM_ATTR_EXT_FILE = 2,
	RBM_ATTR_TILE_FLAGS = 3,
	RBM_ATTR_ACTION_ID = 4,
	RBM_ATTR_UNIQUE_ID = 5,
	RBM_ATTR_TEXT = 6,
	RBM_ATTR_DESC = 7,
	RBM_ATTR_TELE_DEST = 8,
	RBM_ATTR_ITEM = 9,
	RBM_ATTR_DEPOT_ID = 10,
	RBM_ATTR_EXT_SPAWN_FILE = 11,
	RBM_ATTR_RUNE_CHARGES = 12,
	RBM_ATTR_EXT_HOUSE_FILE = 13,
	RBM_ATTR_HOUSEDOORID = 14,
	RBM_ATTR_COUNT = 15,
	RBM_ATTR_DURATION = 16,
	RBM_ATTR_DECAYING_STATE = 17,
	RBM_ATTR_WRITTENDATE = 18,
	RBM_ATTR_WRITTENBY = 19,
	RBM_ATTR_SLEEPERGUID = 20,
	RBM_ATTR_SLEEPSTART = 21,
	RBM_ATTR_CHARGES = 22,

	RBM_ATTR_ATTRIBUTE_MAP = 128
};

enum RBM_NodeTypes_t
{
	RBM_ROOTV1 = 1,
	RBM_MAP_DATA = 2,
	RBM_ITEM_DEF = 3,
	RBM_TILE_AREA = 4,
	RBM_TILE = 5,
	RBM_ITEM = 6,
	RBM_TILE_SQUARE = 7,
	RBM_TILE_REF = 8,
	RBM_SPAWNS = 9,
	RBM_SPAWN_AREA = 10,
	RBM_MONSTER = 11,
	RBM_TOWNS = 12,
	RBM_TOWN = 13,
	RBM_HOUSETILE = 14,
	RBM_WAYPOINTS = 15,
	RBM_WAYPOINT = 16,
};

struct RBM_root_header
{
	uint32_t version;
	uint16_t width;
	uint16_t height;
	uint32_t majorVersionItems;
	uint32_t minorVersionItems;
};

struct RBM_TeleportDest
{
	uint16_t x;
	uint16_t y;
	uint8_t z;
};

struct RBM_Tile_area_coords
{
	uint16_t x;
	uint16_t y;
	uint8_t z;
};

struct RBM_Tile_coords
{
	uint8_t x;
	uint8_t y;
};

struct RBM_TownTemple_coords
{
	uint16_t x;
	uint16_t y;
	uint8_t z;
};

struct RBM_HouseTile_coords
{
	uint8_t x;
	uint8_t y;
	uint32_t houseid;
};

#pragma pack()

class IOMapRBM : public IOMap
{
public:
	IOMapRBM(MapVersionID ver) {version = ver;}
	~IOMapRBM() {}

	static bool getVersionInfo(const FileName& identifier, MapVersionID& out_ver);

	virtual bool loadMap(Map& map, const FileName& identifier);
	virtual bool saveMap(Map& map, const FileName& identifier);

protected:
	static bool getVersionInfo(NodeFileReadHandle* f,  MapVersionID& out_ver);

	virtual bool loadMap(Map& map, NodeFileReadHandle& handle);
	bool loadSpawns(Map& map, const FileName& dir);
	bool loadSpawns(Map& map, pugi::xml_document& doc);
	bool loadHouses(Map& map, const FileName& dir);
	bool loadHouses(Map& map, pugi::xml_document& doc);

	virtual bool saveMap(Map& map, NodeFileWriteHandle& handle);
	bool saveSpawns(Map& map, const FileName& dir);
	bool saveSpawns(Map& map, pugi::xml_document& doc);
	bool saveHouses(Map& map, const FileName& dir);
	bool saveHouses(Map& map, pugi::xml_document& doc);
};

#endif

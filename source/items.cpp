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
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/items.hpp $
// $Id: items.hpp 310 2010-02-26 18:03:48Z admin $

#include "main.h"

#include "materials.h"
#include "gui.h"
#include <string.h> // memcpy

#include "items.h"
#include "item.h"
#include "pugicast.h"

#include <yaml-cpp/yaml.h>

ItemDatabase g_items;

ItemType::ItemType() :
	sprite(nullptr),
	id(0),
	clientID(0),
	brush(nullptr),
	doodad_brush(nullptr),
	raw_brush(nullptr),
	is_metaitem(false),
	has_raw(false),
	in_other_tileset(false),
	group(ITEM_GROUP_NONE),
	type(ITEM_TYPE_NONE),
	volume(0),
	maxTextLen(0),
	//writeOnceItemID(0),
	ground_equivalent(0),
	border_group(0),
	has_equivalent(false),
	wall_hate_me(false),
	name(""),
	description(""),
	weight(0.0f),
	attack(0),
	defense(0),
	armor(0),
	charges(0),
	client_chargeable(false),
	extra_chargeable(false),
	ignoreLook(false),

	isHangable(false),
	hookEast(false),
	hookSouth(false),
	canReadText(false),
	canWriteText(false),
	allowDistRead(false),
	replaceable(true),
	decays(false),
	stackable(false),
	moveable(true),
	alwaysOnBottom(false),
	pickupable(false),
	rotable(false),
	isBorder(false),
	isOptionalBorder(false),
	isWall(false),
	isBrushDoor(false),
	isOpen(false),
	isTable(false),
	isCarpet(false),

	floorChangeDown(false),
	floorChangeNorth(false),
	floorChangeSouth(false),
	floorChangeEast(false),
	floorChangeWest(false),
	floorChange(false),

	unpassable(false),
	blockPickupable(false),
	blockMissiles(false),
	blockPathfinder(false),
	hasElevation(false),

	alwaysOnTopOrder(0),
	rotateTo(0),
	border_alignment(BORDER_NONE)
{
	////
}

ItemType::~ItemType()
{
	////
}

bool ItemType::isFloorChange() const
{
	return floorChange || floorChangeDown || floorChangeNorth || floorChangeSouth || floorChangeEast || floorChangeWest;
}

ItemDatabase::ItemDatabase() :
	// Version information
	MajorVersion(0),
	MinorVersion(0),
	BuildNumber(0),

	// Count of GameSprite types
	item_count(0),
	effect_count(0),
	monster_count(0),
	distance_count(0),

	minclientID(0),
	maxclientID(0),

	max_item_id(0)
{
	////
}

ItemDatabase::~ItemDatabase()
{
	clear();
}

void ItemDatabase::clear()
{
	for(uint32_t i = 0; i < items.size(); i++) {
		delete items[i];
		items.set(i, nullptr);
	}
}

bool ItemDatabase::loadFromOtbVer3(BinaryNode* itemNode, wxString& error, wxArrayString& warnings) {
	uint8_t u8;
	for( ; itemNode != nullptr; itemNode = itemNode->advance()) {
		if(!itemNode->getU8(u8)) {
			// Invalid!
			warnings.push_back("Invalid item type encountered...");
			continue;
		}

		if(ItemGroup_t(u8) == ITEM_GROUP_DEPRECATED)
			continue;

		ItemType* t = newd ItemType();
		t->group = ItemGroup_t(u8);

		switch(t->group) {
			case ITEM_GROUP_NONE:
			case ITEM_GROUP_GROUND:
			case ITEM_GROUP_SPLASH:
			case ITEM_GROUP_FLUID:
				break;
			case ITEM_GROUP_CONTAINER: t->type = ITEM_TYPE_CONTAINER; break;
				break;
			default:
				warnings.push_back("Unknown item group declaration");
		}

		uint32_t flags;
		if(itemNode->getU32(flags)) {
			t->unpassable = ((flags & FLAG_UNPASSABLE) == FLAG_UNPASSABLE);
			t->blockMissiles = ((flags & FLAG_BLOCK_MISSILES) == FLAG_BLOCK_MISSILES);
			t->blockPathfinder = ((flags & FLAG_BLOCK_PATHFINDER) == FLAG_BLOCK_PATHFINDER);
			t->hasElevation = ((flags & FLAG_HAS_ELEVATION) == FLAG_HAS_ELEVATION);
			t->pickupable = ((flags & FLAG_PICKUPABLE) == FLAG_PICKUPABLE);
			t->moveable = ((flags & FLAG_MOVEABLE) == FLAG_MOVEABLE);
			t->stackable = ((flags & FLAG_STACKABLE) == FLAG_STACKABLE);
			t->floorChangeDown = ((flags & FLAG_FLOORCHANGEDOWN) == FLAG_FLOORCHANGEDOWN);
			t->floorChangeNorth = ((flags & FLAG_FLOORCHANGENORTH) == FLAG_FLOORCHANGENORTH);
			t->floorChangeEast = ((flags & FLAG_FLOORCHANGEEAST) == FLAG_FLOORCHANGEEAST);
			t->floorChangeSouth = ((flags & FLAG_FLOORCHANGESOUTH) == FLAG_FLOORCHANGESOUTH);
			t->floorChangeWest = ((flags & FLAG_FLOORCHANGEWEST) == FLAG_FLOORCHANGEWEST);
			t->floorChange = t->floorChangeDown || t->floorChangeNorth || t->floorChangeEast || t->floorChangeSouth || t->floorChangeWest;
			// Now this is confusing, just accept that the ALWAYSONTOP flag means it's always on bottom, got it?!
			t->alwaysOnBottom = ((flags & FLAG_ALWAYSONTOP) == FLAG_ALWAYSONTOP);
			t->isHangable = ((flags & FLAG_HANGABLE) == FLAG_HANGABLE);
			t->hookEast = ((flags & FLAG_HOOK_EAST) == FLAG_HOOK_EAST);
			t->hookSouth = ((flags & FLAG_HOOK_SOUTH) == FLAG_HOOK_SOUTH);
			t->allowDistRead = ((flags & FLAG_ALLOWDISTREAD) == FLAG_ALLOWDISTREAD);
			t->rotable = ((flags & FLAG_ROTABLE) == FLAG_ROTABLE);
			t->canReadText = ((flags & FLAG_READABLE) == FLAG_READABLE);
			t->client_chargeable = ((flags & FLAG_CLIENTCHARGES) == FLAG_CLIENTCHARGES);
			t->ignoreLook = ((flags & FLAG_IGNORE_LOOK) == FLAG_IGNORE_LOOK);
		}

		uint8_t attribute;
		while(itemNode->getU8(attribute)) {
			uint16_t datalen;
			if(!itemNode->getU16(datalen)) {
				warnings.push_back("Invalid item type property");
				break;
			}

			switch(attribute) {
				case ITEM_ATTR_SERVERID: {
					if(datalen != sizeof(uint16_t)) {
						error = "items.otb: Unexpected data length of server id block (Should be 2 bytes)";
						return false;
					}

					if(!itemNode->getU16(t->id))
						warnings.push_back("Invalid item type property (2)");

					if(max_item_id < t->id)
						max_item_id = t->id;
					break;
				}

				case ITEM_ATTR_CLIENTID: {
					if(datalen != sizeof(uint16_t)) {
						error = "items.otb: Unexpected data length of client id block (Should be 2 bytes)";
						return false;
					}

					if(!itemNode->getU16(t->clientID))
						warnings.push_back("Invalid item type property (2)");

					t->sprite = static_cast<GameSprite*>(g_gui.gfx.getSprite(t->clientID));
					break;
				}

				case ITEM_ATTR_SPEED: {
					if(datalen != sizeof(uint16_t)) {
						error = "items.otb: Unexpected data length of speed block (Should be 2 bytes)";
						return false;
					}

					//t->speed = itemNode->getU16();
					if(!itemNode->skip(2)) // Just skip two bytes, we don't need speed
						warnings.push_back("Invalid item type property (3)");
					break;
				}

				case ITEM_ATTR_LIGHT2: {
					if(datalen != sizeof(lightBlock2))
					{
						warnings.push_back("items.otb: Unexpected data length of item light (2) block (Should be " + i2ws(sizeof(lightBlock2)) + " bytes)");
						break;
					}
					if(!itemNode->skip(4)) // Just skip two bytes, we don't need light
						warnings.push_back("Invalid item type property (4)");

					//t->lightLevel = itemNode->getU16();
					//t->lightColor = itemNode->getU16();
					break;
				}

				case ITEM_ATTR_TOPORDER: {
					if(datalen != sizeof(uint8_t)) {
						warnings.push_back("items.otb: Unexpected data length of item toporder block (Should be 1 byte)");
						break;
					}

					if(!itemNode->getU8(u8))
						warnings.push_back("Invalid item type property (5)");

					t->alwaysOnTopOrder = u8;
					break;
				}

				default: {
					//skip unknown attributes
					itemNode->skip(datalen);
					//warnings.push_back("items.otb: Skipped unknown attribute");
					break;
				}
			}
		}

		if(t) {
			if(items[t->id]) {
				warnings.push_back("items.otb: Duplicate items");
				delete items[t->id];
			}
			items.set(t->id, t);
		}
	}
	return true;
}

bool ItemDatabase::loadFromYAML(const FileName& datafile, wxString& error, wxArrayString& warnings)
{
	std::string filename = nstr((datafile.GetPath(wxPATH_GET_VOLUME | wxPATH_GET_SEPARATOR) + datafile.GetFullName()));
	YAML::Node itemsNode = YAML::LoadFile(filename);

	for (auto it = itemsNode.begin(); it != itemsNode.end(); ++it) {
        const YAML::Node& item = *it;
        const YAML::Node& attributes = item["attributes"];
        const YAML::Node& properties = item["properties"];

		ItemType* iType = newd ItemType();

		iType->id = item["id"].as<uint16_t>();
		iType->clientID = iType->id;
		iType->sprite = static_cast<GameSprite*>(g_gui.gfx.getSprite(iType->id));

		// group
		if (properties["ground"]) {
			iType->group = ItemGroup_t(ITEM_GROUP_GROUND);
		} else if (properties["fuild_container"]) {
			iType->group = ItemGroup_t(ITEM_GROUP_FLUID);
		} else if (properties["fluid"]) {
			iType->group = ItemGroup_t(ITEM_GROUP_SPLASH);
		} else if (properties["container"]) {
			iType->group = ItemGroup_t(ITEM_GROUP_CONTAINER);
			iType->type = ITEM_TYPE_CONTAINER;
		} else {
			iType->group = ItemGroup_t(ITEM_GROUP_NONE);
		}

		iType->name       = attributes["name"]       ? attributes["name"].as<std::string>() : "";

		iType->pickupable = properties["pickupable"] ? properties["pickupable"].as<bool>()  : false;
		iType->stackable  = properties["stackable"]  ? properties["stackable"].as<bool>()   : false;
		iType->hookEast   = properties["hook_east"]  ? properties["hook_east"].as<bool>()   : false;
		iType->hookSouth  = properties["hook_south"] ? properties["hook_south"].as<bool>()  : false;
		iType->isHangable = properties["hangable"]   ? properties["hangable"].as<bool>()    : false;
		iType->alwaysOnBottom = properties["on_bottom"] ? properties["on_bottom"].as<bool>() : false;

		iType->volume     = properties["volume"]     ? properties["volume"].as<uint32_t>()  : 0;

		if(iType) {
			if(items[iType->id]) {
				warnings.push_back("items.yaml: Duplicate items");
				delete items[iType->id];
			}
			items.set(iType->id, iType);
			max_item_id = iType->id;
		}
	}

	return true;
}

bool ItemDatabase::loadMetaItem(pugi::xml_node node)
{
	if(pugi::xml_attribute attribute = node.attribute("id")) {
		int32_t id = pugi::cast<int32_t>(attribute.value());
		if(items[id]) {
			//std::cout << "Occupied ID " << id << " : " << items[id]->id << ":" << items[id]->name << std::endl;
			return false;
		}
		items.set(id, newd ItemType());
		items[id]->is_metaitem = true;
		items[id]->id = id;
	} else {
		return false;
	}
	return true;
}

ItemType& ItemDatabase::getItemType(int id)
{
	ItemType* it = items[id];
	if(it)
		return *it;
	else {
		static ItemType dummyItemType; // use this for invalid ids
		return dummyItemType;
	}
}

bool ItemDatabase::typeExists(int id) const
{
	ItemType* it = items[id];
	return it != nullptr;
}

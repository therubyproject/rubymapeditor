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
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/iomap_rbm.hpp $
// $Id: iomap_rbm.hpp 310 2010-02-26 18:03:48Z admin $

#include "main.h"

#include <wx/wfstream.h>
#include <wx/tarstrm.h>
#include <wx/zstream.h>
#include <wx/mstream.h>
#include <wx/datstrm.h>

#include "settings.h"
#include "gui.h" // Loadbar

#include "creatures.h"
#include "creature.h"
#include "map.h"
#include "tile.h"
#include "item.h"
#include "complexitem.h"
#include "town.h"

#include "iomap_rbm.h"
#include "pugicast.h"

typedef uint8_t attribute_t;
typedef uint32_t flags_t;

// H4X
void reform(Map* map, Tile* tile, Item* item)
{
	/*
	int aid = item->getActionID();
	int id = item->getID();
	int uid = item->getUniqueID();

	if(item->isDoor()) {
		item->eraseAttribute("aid");
		item->setAttribute("keyid", aid);
	}

	if((item->isDoor()) && tile && tile->getHouseID()) {
		Door* self = static_cast<Door*>(item);
		House* house = map->houses.getHouse(tile->getHouseID());
		self->setDoorID(house->getEmptyDoorID());
	}
	*/
}

// ============================================================================
// Item

Item* Item::Create_RBM(const IOMap& maphandle, BinaryNode* stream)
{
	uint16_t _id;
	if(!stream->getU16(_id)) {
		return nullptr;
	}

	uint8_t _count = 0;

	const ItemType& iType = g_items[_id];
	return Item::Create(_id, _count);
}

bool Item::readItemAttribute_RBM(const IOMap& maphandle, RBM_ItemAttribute attr, BinaryNode* stream)
{
	switch (attr) {
		case RBM_ATTR_COUNT: {
			uint8_t subtype;
			if(!stream->getU8(subtype)) {
				return false;
			}
			setSubtype(subtype);
			break;
		}
		case RBM_ATTR_ACTION_ID: {
			uint16_t aid;
			if(!stream->getU16(aid)) {
				return false;
			}
			setActionID(aid);
			break;
		}
		case RBM_ATTR_UNIQUE_ID: {
			uint16_t uid;
			if(!stream->getU16(uid)) {
				return false;
			}
			setUniqueID(uid);
			break;
		}
		case RBM_ATTR_CHARGES: {
			uint16_t charges;
			if(!stream->getU16(charges)) {
				return false;
			}
			setSubtype(charges);
			break;
		}
		case RBM_ATTR_TEXT: {
			std::string text;
			if(!stream->getString(text)) {
				return false;
			}
			setText(text);
			break;
		}
		case RBM_ATTR_DESC: {
			std::string text;
			if(!stream->getString(text)) {
				return false;
			}
			setDescription(text);
			break;
		}
		case RBM_ATTR_RUNE_CHARGES: {
			uint8_t subtype;
			if(!stream->getU8(subtype)) {
				return false;
			}
			setSubtype(subtype);
			break;
		}

		// The following *should* be handled in the derived classes
		// However, we still need to handle them here since otherwise things
		// will break horribly
		case RBM_ATTR_DEPOT_ID: return stream->skip(2);
		case RBM_ATTR_HOUSEDOORID: return stream->skip(1);
		case RBM_ATTR_TELE_DEST: return stream->skip(5);
		default: return false;
	}
	return true;
}

bool Item::unserializeAttributes_RBM(const IOMap& maphandle, BinaryNode* stream)
{
	uint8_t attribute;
	while(stream->getU8(attribute)) {
		if(attribute == RBM_ATTR_ATTRIBUTE_MAP) {
			if(!ItemAttributes::unserializeAttributeMap(maphandle, stream)) {
				return false;
			}
		} else if(!readItemAttribute_RBM(maphandle, static_cast<RBM_ItemAttribute>(attribute), stream)) {
			return false;
		}
	}
	return true;
}

bool Item::unserializeItemNode_RBM(const IOMap& maphandle, BinaryNode* node)
{
	return unserializeAttributes_RBM(maphandle, node);
}

void Item::serializeItemAttributes_RBM(const IOMap& maphandle, NodeFileWriteHandle& stream) const
{
	const ItemType& iType = g_items[id];
	if(iType.stackable || iType.isSplash() || iType.isFluidContainer()) {
		stream.addU8(RBM_ATTR_COUNT);
		stream.addU8(getSubtype());
	}	

	// Lets use it on the future
	// if(maphandle.version >= MAP_RBM_4) {
	// 	if(attributes && !attributes->empty()) {
	// 		stream.addU8(RBM_ATTR_ATTRIBUTE_MAP);
	// 		serializeAttributeMap(maphandle, stream);
	// 	}
	// } else {
		if(isCharged()) {
			stream.addU8(RBM_ATTR_CHARGES);
			stream.addU16(getSubtype());
		}

		uint16_t actionId = getActionID();
		if(actionId > 0) {
			stream.addU8(RBM_ATTR_ACTION_ID);
			stream.addU16(actionId);
		}

		uint16_t uniqueId = getUniqueID();
		if(uniqueId > 0) {
			stream.addU8(RBM_ATTR_UNIQUE_ID);
			stream.addU16(uniqueId);
		}

		const std::string& text = getText();
		if(!text.empty()) {
			stream.addU8(RBM_ATTR_TEXT);
			stream.addString(text);
		}

		const std::string& description = getDescription();
		if(!description.empty()) {
			stream.addU8(RBM_ATTR_DESC);
			stream.addString(description);
		}
	// }
}

void Item::serializeItemCompact_RBM(const IOMap& maphandle, NodeFileWriteHandle& stream) const
{
	stream.addU16(id);

	/* This is impossible
	const ItemType& iType = g_items[id];

	if(iType.stackable || iType.isSplash() || iType.isFluidContainer()){
		stream.addU8(getSubtype());
	}
	*/
}

bool Item::serializeItemNode_RBM(const IOMap& maphandle, NodeFileWriteHandle& file) const
{
	file.addNode(RBM_ITEM);
	file.addU16(id);

	serializeItemAttributes_RBM(maphandle, file);
	file.endNode();
	return true;
}

// ============================================================================
// Teleport

bool Teleport::readItemAttribute_RBM(const IOMap& maphandle, RBM_ItemAttribute attribute, BinaryNode* stream)
{
	if(RBM_ATTR_TELE_DEST == attribute) {
		uint16_t x, y;
		uint8_t z;
		if(!stream->getU16(x) || !stream->getU16(y) || !stream->getU8(z)) {
			return false;
		}
		destination = Position(x, y, z);
		return true;
	} else {
		return Item::readItemAttribute_RBM(maphandle, attribute, stream);
	}
}

void Teleport::serializeItemAttributes_RBM(const IOMap& maphandle, NodeFileWriteHandle& stream) const
{
	Item::serializeItemAttributes_RBM(maphandle, stream);

	stream.addByte(RBM_ATTR_TELE_DEST);
	stream.addU16(destination.x);
	stream.addU16(destination.y);
	stream.addU8(destination.z);
}

// ============================================================================
// Door

bool Door::readItemAttribute_RBM(const IOMap& maphandle, RBM_ItemAttribute attribute, BinaryNode* stream)
{
	if(RBM_ATTR_HOUSEDOORID == attribute) {
		uint8_t id = 0;
		if(!stream->getU8(id)) {
			return false;
		}
		doorId = id;
		return true;
	} else {
		return Item::readItemAttribute_RBM(maphandle, attribute, stream);
	}
}

void Door::serializeItemAttributes_RBM(const IOMap& maphandle, NodeFileWriteHandle& stream) const
{
	Item::serializeItemAttributes_RBM(maphandle, stream);
	if(doorId) {
		stream.addByte(RBM_ATTR_HOUSEDOORID);
		stream.addU8(doorId);
	}
}

// ============================================================================
// Depots

bool Depot::readItemAttribute_RBM(const IOMap& maphandle, RBM_ItemAttribute attribute, BinaryNode* stream)
{
	if(RBM_ATTR_DEPOT_ID == attribute) {
		uint16_t id = 0;
		if(!stream->getU16(id)) {
			return false;
		}
		depotId = id;
		return true;
	} else {
		return Item::readItemAttribute_RBM(maphandle, attribute, stream);
	}
}

void Depot::serializeItemAttributes_RBM(const IOMap& maphandle, NodeFileWriteHandle& stream) const
{
	Item::serializeItemAttributes_RBM(maphandle, stream);
	if(depotId) {
		stream.addByte(RBM_ATTR_DEPOT_ID);
		stream.addU16(depotId);
	}
}

// ============================================================================
// Container

bool Container::unserializeItemNode_RBM(const IOMap& maphandle, BinaryNode* node)
{
	if(!Item::unserializeAttributes_RBM(maphandle, node)) {
		return false;
	}

	BinaryNode* child = node->getChild();
	if(child) {
		do {
			uint8_t type;
			if(!child->getByte(type)) {
				return false;
			}

			if(type != RBM_ITEM) {
				return false;
			}

			Item* item = Item::Create_RBM(maphandle, child);
			if(!item) {
				return false;
			}

			if(!item->unserializeItemNode_RBM(maphandle, child)) {
				delete item;
				return false;
			}

			contents.push_back(item);
		} while(child->advance());
	}
	return true;
}

bool Container::serializeItemNode_RBM(const IOMap& maphandle, NodeFileWriteHandle& file) const
{
	file.addNode(RBM_ITEM);
	file.addU16(id);

	serializeItemAttributes_RBM(maphandle, file);
	for(Item* item : contents) {
		item->serializeItemNode_RBM(maphandle, file);
	}

	file.endNode();
	return true;
}

/*
	RBM_ROOTV1
	|
	|--- RBM_MAP_DATA
	|	|
	|	|--- RBM_TILE_AREA
	|	|	|--- RBM_TILE
	|	|	|--- RBM_TILE_SQUARE (not implemented)
	|	|	|--- RBM_TILE_REF (not implemented)
	|	|	|--- RBM_HOUSETILE
	|	|
	|	|--- RBM_SPAWNS (not implemented)
	|	|	|--- RBM_SPAWN_AREA (not implemented)
	|	|	|--- RBM_MONSTER (not implemented)
	|	|
	|	|--- RBM_TOWNS
	|		|--- RBM_TOWN
	|
	|--- RBM_ITEM_DEF (not implemented)
*/

bool IOMapRBM::getVersionInfo(const FileName& filename, MapVersionID& out_ver)
{
#ifdef OTGZ_SUPPORT
	if(filename.GetExt() == "rgz") {
		// Open the archive
		std::shared_ptr<struct archive> a(archive_read_new(), archive_read_free);
		archive_read_support_filter_all(a.get());
		archive_read_support_format_all(a.get());
		if(archive_read_open_filename(a.get(), nstr(filename.GetFullPath()).c_str(), 10240) != ARCHIVE_OK)
			 return false;

		// Loop over the archive entries until we find the rbm file
		struct archive_entry* entry;
		while(archive_read_next_header(a.get(), &entry) == ARCHIVE_OK) {
			std::string entryName = archive_entry_pathname(entry);

			if(entryName == "world/map.rbm") {
				// Read the RBM header into temporary memory
				uint8_t buffer[8096];
				memset(buffer, 0, 8096);

				// Read from the archive
				int read_bytes = archive_read_data(a.get(), buffer, 8096);

				// Check so it at least contains the 4-byte file id
				if(read_bytes < 4)
					return false;

				// Create a read handle on it
				std::shared_ptr<NodeFileReadHandle> f(new MemoryNodeFileReadHandle(buffer + 4, read_bytes - 4));

				// Read the version info
				return getVersionInfo(f.get(), out_ver);
			}
		}

		// Didn't find RBM file, lame
		return false;
	}
#endif

	// Just open a disk-based read handle
	DiskNodeFileReadHandle f(nstr(filename.GetFullPath()), StringVector(1, "RBM"));
	if(!f.isOk())
		return false;
	return getVersionInfo(&f, out_ver);
}

bool IOMapRBM::getVersionInfo(NodeFileReadHandle* f,  MapVersionID& out_ver)
{
	BinaryNode* root = f->getRootNode();
	if(!root)
		return false;

	root->skip(1); // Skip the type byte

	uint16_t u16;
	uint32_t u32;

	if(!root->getU32(u32)) // Version
		return false;
	out_ver = (MapVersionID)u32;


	root->getU16(u16);
	root->getU16(u16);
	root->getU32(u32);

	if(!root->getU32(u32)) // OTB minor version // Please remove soon
		return false;

	return true;
}

bool IOMapRBM::loadMap(Map& map, const FileName& filename)
{
#ifdef OTGZ_SUPPORT
	if(filename.GetExt() == "rgz") {
		// Open the archive
		std::shared_ptr<struct archive> a(archive_read_new(), archive_read_free);
		archive_read_support_filter_all(a.get());
		archive_read_support_format_all(a.get());
		if(archive_read_open_filename(a.get(), nstr(filename.GetFullPath()).c_str(), 10240) != ARCHIVE_OK)
			 return false;

		// Memory buffers for the houses & spawns
		std::shared_ptr<uint8_t> house_buffer;
		std::shared_ptr<uint8_t> spawn_buffer;
		size_t house_buffer_size = 0;
		size_t spawn_buffer_size = 0;

		// See if the rbm file has been loaded
		bool rbm_loaded = false;

		// Loop over the archive entries until we find the rbm file
		g_gui.SetLoadDone(0, "Decompressing archive...");
		struct archive_entry* entry;
		while(archive_read_next_header(a.get(), &entry) == ARCHIVE_OK) {
			std::string entryName = archive_entry_pathname(entry);

			if(entryName == "world/map.rbm") {
				// Read the entire RBM file into a memory region
				size_t rbm_size = archive_entry_size(entry);
				std::shared_ptr<uint8_t> rbm_buffer(new uint8_t[rbm_size], [](uint8_t* p) { delete[] p; });

				// Read from the archive
				size_t read_bytes = archive_read_data(a.get(), rbm_buffer.get(), rbm_size);

				// Check so it at least contains the 4-byte file id
				if(read_bytes < 4)
					return false;

				if(read_bytes < rbm_size) {
					error("Could not read file.");
					return false;
				}

				g_gui.SetLoadDone(0, "Loading RBM map...");

				// Create a read handle on it
				std::shared_ptr<NodeFileReadHandle> f(
					new MemoryNodeFileReadHandle(rbm_buffer.get() + 4, rbm_size - 4));

				// Read the version info
				if(!loadMap(map, *f.get())) {
					error("Could not load RBM file inside archive");
					return false;
				}

				rbm_loaded = true;
			} else if(entryName == "world/houses.xml") {
				house_buffer_size = archive_entry_size(entry);
				house_buffer.reset(new uint8_t[house_buffer_size]);

				// Read from the archive
				size_t read_bytes = archive_read_data(a.get(), house_buffer.get(), house_buffer_size);

				// Check so it at least contains the 4-byte file id
				if(read_bytes < house_buffer_size) {
					house_buffer.reset();
					house_buffer_size = 0;
					warning("Failed to decompress houses.");
				}
			} else if(entryName == "world/spawns.xml") {
				spawn_buffer_size = archive_entry_size(entry);
				spawn_buffer.reset(new uint8_t[spawn_buffer_size]);

				// Read from the archive
				size_t read_bytes = archive_read_data(a.get(), spawn_buffer.get(), spawn_buffer_size);

				// Check so it at least contains the 4-byte file id
				if(read_bytes < spawn_buffer_size) {
					spawn_buffer.reset();
					spawn_buffer_size = 0;
					warning("Failed to decompress spawns.");
				}
			}
		}

		if(!rbm_loaded) {
			error("RBM file not found inside archive.");
			return false;
		}

		// Load the houses from the stored buffer
		if(house_buffer.get() && house_buffer_size > 0) {
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_buffer(house_buffer.get(), house_buffer_size);
			if(result) {
				if(!loadHouses(map, doc)) {
					warning("Failed to load houses.");
				}
			} else {
				warning("Failed to load houses due to XML parse error.");
			}
		}

		// Load the spawns from the stored buffer
		if(spawn_buffer.get() && spawn_buffer_size > 0) {
			pugi::xml_document doc;
			pugi::xml_parse_result result = doc.load_buffer(spawn_buffer.get(), spawn_buffer_size);
			if(result) {
				if(!loadSpawns(map, doc)) {
					warning("Failed to load spawns.");
				}
			} else {
				warning("Failed to load spawns due to XML parse error.");
			}
		}

		return true;
	}
#endif

	DiskNodeFileReadHandle f(nstr(filename.GetFullPath()), StringVector(1, "RBM"));
	if(!f.isOk()) {
		error(("Couldn't open file for reading\nThe error reported was: " + wxstr(f.getErrorMessage())).wc_str());
		return false;
	}

	if(!loadMap(map, f))
		return false;

	// Read auxilliary files
	if(!loadHouses(map, filename)) {
		warning("Failed to load houses.");
		map.housefile = nstr(filename.GetName()) + "-house.xml";
	}
	if(!loadSpawns(map, filename)) {
		warning("Failed to load spawns.");
		map.spawnfile = nstr(filename.GetName()) + "-spawn.xml";
	}
	return true;
}

bool IOMapRBM::loadMap(Map& map, NodeFileReadHandle& f)
{
	BinaryNode* root = f.getRootNode();
	if(!root) {
		error("Could not read root node.");
		return false;
	}
	root->skip(1); // Skip the type byte

	uint8_t u8;
	uint16_t u16;
	uint32_t u32;

	if(!root->getU32(u32))
		return false;

	version = (MapVersionID) u32;

	if(version > MAP_RBM_1) {
		// Failed to read version
		if(g_gui.PopupDialog("Map error",
			"The loaded map appears to be a RBM format that is not supported by the editor."
			"Do you still want to attempt to load the map?", wxYES | wxNO) == wxID_YES)
		{
			warning("Unsupported or damaged map version");
		} else {
			error("Unsupported RBM version, could not load map");
			return false;
		}
	}

	if(!root->getU16(u16))
		return false;

	map.width = u16;
	if(!root->getU16(u16))
		return false;

	map.height = u16;

	// remove
	if(!root->getU32(u32) || u32 > (unsigned long)g_items.MajorVersion) { // OTB major version
		// if(g_gui.PopupDialog("Map error",
		// 	"The loaded map appears to be a items.otb format that deviates from the "
		// 	"items.otb loaded by the editor. Do you still want to attempt to load the map?", wxYES | wxNO) == wxID_YES)
		// {
		// 	warning("Unsupported or damaged map version");
		// } else {
		// 	error("Outdated items.otb, could not load map");
		// 	return false;
		// }
	}

	// remove
	if(!root->getU32(u32) || u32 > (unsigned long)g_items.MinorVersion) { // OTB minor version
		// warning("This editor needs an updated items.otb version");
	}

	//version.client = (ClientVersionID)55;

	BinaryNode* mapHeaderNode = root->getChild();
	if(mapHeaderNode == nullptr || !mapHeaderNode->getByte(u8) || u8 != RBM_MAP_DATA) {
		error("Could not get root child node. Cannot recover from fatal error!");
		return false;
	}

	uint8_t attribute;
	while(mapHeaderNode->getU8(attribute)) {
		switch(attribute) {
			case RBM_ATTR_DESCRIPTION: {
				if(!mapHeaderNode->getString(map.description)) {
					warning("Invalid map description tag");
				}
				//std::cout << "Map description: " << mapDescription << std::endl;
				break;
			}
			case RBM_ATTR_EXT_SPAWN_FILE: {
				if(!mapHeaderNode->getString(map.spawnfile)) {
					warning("Invalid map spawnfile tag");
				}
				break;
			}
			case RBM_ATTR_EXT_HOUSE_FILE: {
				if(!mapHeaderNode->getString(map.housefile)) {
					warning("Invalid map housefile tag");
				}
				break;
			}
			default: {
				warning("Unknown header node.");
				break;
			}
		}
	}

	int nodes_loaded = 0;

	for(BinaryNode* mapNode = mapHeaderNode->getChild(); mapNode != nullptr; mapNode = mapNode->advance()) {
		++nodes_loaded;
		if(nodes_loaded % 15 == 0) {
			g_gui.SetLoadDone(static_cast<int32_t>(100.0 * f.tell() / f.size()));
		}

		uint8_t node_type;
		if(!mapNode->getByte(node_type)) {
			warning("Invalid map node");
			continue;
		}
		if(node_type == RBM_TILE_AREA) {
			uint16_t base_x, base_y;
			uint8_t base_z;
			if(!mapNode->getU16(base_x) || !mapNode->getU16(base_y) || !mapNode->getU8(base_z)) {
				warning("Invalid map node, no base coordinate");
				continue;
			}

			for(BinaryNode* tileNode = mapNode->getChild(); tileNode != nullptr; tileNode = tileNode->advance()) {
				Tile* tile = nullptr;
				uint8_t tile_type;
				if(!tileNode->getByte(tile_type)) {
					warning("Invalid tile type");
					continue;
				}
				if(tile_type == RBM_TILE || tile_type == RBM_HOUSETILE) {
					//printf("Start\n");
					uint8_t x_offset, y_offset;
					if(!tileNode->getU8(x_offset) || !tileNode->getU8(y_offset)) {
						warning("Could not read position of tile");
						continue;
					}
					const Position pos(base_x + x_offset, base_y + y_offset, base_z);

					if(map.getTile(pos)) {
						warning("Duplicate tile at %d:%d:%d, discarding duplicate", pos.x, pos.y, pos.z);
						continue;
					}

					tile = map.allocator(map.createTileL(pos));
					House* house = nullptr;
					if(tile_type == RBM_HOUSETILE) {
						uint32_t house_id;
						if(!tileNode->getU32(house_id)) {
							warning("House tile without house data, discarding tile");
							continue;
						}
						if(house_id) {
							house = map.houses.getHouse(house_id);
							if(!house) {
								house = newd House(map);
								house->id = house_id;
								map.houses.addHouse(house);
							}
						} else {
							warning("Invalid house id from tile %d:%d:%d", pos.x, pos.y, pos.z);
						}
					}

					//printf("So far so good\n");

					uint8_t attribute;
					while(tileNode->getU8(attribute)) {
						switch(attribute) {
							case RBM_ATTR_TILE_FLAGS: {
								uint32_t flags = 0;
								if(!tileNode->getU32(flags)) {
									warning("Invalid tile flags of tile on %d:%d:%d", pos.x, pos.y, pos.z);
								}
								tile->setMapFlags(flags);
								break;
							}
							case RBM_ATTR_ITEM: {
								Item* item = Item::Create_RBM(*this, tileNode);
								if(item == nullptr)
								{
									warning("Invalid item at tile %d:%d:%d", pos.x, pos.y, pos.z);
								}
								tile->addItem(item);
								break;
							}
							default: {
								warning("Unknown tile attribute at %d:%d:%d", pos.x, pos.y, pos.z);
								break;
							}
						}
					}

					//printf("Didn't die in loop\n");

					for(BinaryNode* itemNode = tileNode->getChild(); itemNode != nullptr; itemNode = itemNode->advance()) {
						Item* item = nullptr;
						uint8_t item_type;
						if(!itemNode->getByte(item_type)) {
							warning("Unknown item type %d:%d:%d", pos.x, pos.y, pos.z);
							continue;
						}
						if(item_type == RBM_ITEM) {
							item = Item::Create_RBM(*this, itemNode);
							if(item) {
								if(!item->unserializeItemNode_RBM(*this, itemNode)) {
									warning("Couldn't unserialize item attributes at %d:%d:%d", pos.x, pos.y, pos.z);
								}
								//reform(&map, tile, item);
								tile->addItem(item);
							}
						} else {
							warning("Unknown type of tile child node");
						}
					}

					tile->update();
					if(house)
						house->addTile(tile);

					map.setTile(pos.x, pos.y, pos.z, tile);
				} else {
					warning("Unknown type of tile node");
				}
			}
		} else if(node_type == RBM_TOWNS) {
			for(BinaryNode* townNode = mapNode->getChild(); townNode != nullptr; townNode = townNode->advance()) {
				Town* town = nullptr;
				uint8_t town_type;
				if(!townNode->getByte(town_type)) {
					warning("Invalid town type (1)");
					continue;
				}
				if(town_type != RBM_TOWN) {
					warning("Invalid town type (2)");
					continue;
				}
				uint32_t town_id;
				if(!townNode->getU32(town_id)) {
					warning("Invalid town id");
					continue;
				}

				town = map.towns.getTown(town_id);
				if(town) {
					warning("Duplicate town id %d, discarding duplicate", town_id);
					continue;
				} else {
					town = newd Town(town_id);
					if(!map.towns.addTown(town)) {
						delete town;
						continue;
					}
				}
				std::string town_name;
				if(!townNode->getString(town_name)) {
					warning("Invalid town name");
					continue;
				}
				town->setName(town_name);
				Position pos;
				uint16_t x;
				uint16_t y;
				uint8_t z;
				if(!townNode->getU16(x) || !townNode->getU16(y) || !townNode->getU8(z)) {
					warning("Invalid town temple position");
					continue;
				}
				pos.x = x;
				pos.y = y;
				pos.z = z;
				town->setTemplePosition(pos);
			}
		} else if(node_type == RBM_WAYPOINTS) {
			for(BinaryNode* waypointNode = mapNode->getChild(); waypointNode != nullptr; waypointNode = waypointNode->advance()) {
				uint8_t waypoint_type;
				if(!waypointNode->getByte(waypoint_type)) {
					warning("Invalid waypoint type (1)");
					continue;
				}
				if(waypoint_type != RBM_WAYPOINT) {
					warning("Invalid waypoint type (2)");
					continue;
				}

				Waypoint wp;

				if(!waypointNode->getString(wp.name)) {
					warning("Invalid waypoint name");
					continue;
				}
				uint16_t x;
				uint16_t y;
				uint8_t z;
				if(!waypointNode->getU16(x) || !waypointNode->getU16(y) || !waypointNode->getU8(z)) {
					warning("Invalid waypoint position");
					continue;
				}
				wp.pos.x = x;
				wp.pos.y = y;
				wp.pos.z = z;

				map.waypoints.addWaypoint(newd Waypoint(wp));
			}
		}
	}

	if(!f.isOk())
		warning(wxstr(f.getErrorMessage()).wc_str());
	return true;
}

bool IOMapRBM::loadSpawns(Map& map, const FileName& dir)
{
	std::string fn = (const char*)(dir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME).mb_str(wxConvUTF8));
	fn += map.spawnfile;

	FileName filename(wxstr(fn));
	if(!filename.FileExists())
		return false;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fn.c_str());
	if(!result) {
		return false;
	}
	return loadSpawns(map, doc);
}

bool IOMapRBM::loadSpawns(Map& map, pugi::xml_document& doc)
{
	pugi::xml_node node = doc.child("spawns");
	if(!node) {
		warnings.push_back("IOMapRBM::loadSpawns: Invalid rootheader.");
		return false;
	}

	for(pugi::xml_node spawnNode = node.first_child(); spawnNode; spawnNode = spawnNode.next_sibling()) {
		if(as_lower_str(spawnNode.name()) != "spawn") {
			continue;
		}

		Position spawnPosition;
		spawnPosition.x = pugi::cast<int32_t>(spawnNode.attribute("centerx").value());
		spawnPosition.y = pugi::cast<int32_t>(spawnNode.attribute("centery").value());
		spawnPosition.z = pugi::cast<int32_t>(spawnNode.attribute("centerz").value());

		if(spawnPosition.x == 0 || spawnPosition.y == 0) {
			warning("Bad position data on one spawn, discarding...");
			continue;
		}

		int32_t radius = pugi::cast<int32_t>(spawnNode.attribute("radius").value());
		if(radius < 1) {
			warning("Couldn't read radius of spawn.. discarding spawn...");
			continue;
		}

		Tile* tile = map.getTile(spawnPosition);
		if(tile && tile->spawn) {
			warning("Duplicate spawn on position %d:%d:%d\n", tile->getX(), tile->getY(), tile->getZ());
			continue;
		}

		Spawn* spawn = newd Spawn(radius);
		if(!tile) {
			tile = map.allocator(map.createTileL(spawnPosition));
			map.setTile(spawnPosition, tile);
		}

		tile->spawn = spawn;
		map.addSpawn(tile);

		for(pugi::xml_node creatureNode = spawnNode.first_child(); creatureNode; creatureNode = creatureNode.next_sibling()) {
			const std::string& creatureNodeName = as_lower_str(creatureNode.name());
			if(creatureNodeName != "monster" && creatureNodeName != "npc") {
				continue;
			}

			bool isNpc = creatureNodeName == "npc";
			const std::string& name = creatureNode.attribute("name").as_string();
			if(name.empty()) {
				wxString err;
				err << "Bad creature position data, discarding creature at spawn " << spawnPosition.x << ":" << spawnPosition.y << ":" << spawnPosition.z << " due missing name.";
				warnings.Add(err);
				break;
			}

			int32_t spawntime = pugi::cast<int32_t>(creatureNode.attribute("spawntime").value());
			if(spawntime == 0) {
				spawntime = g_settings.getInteger(Config::DEFAULT_SPAWNTIME);
			}

			Direction direction = NORTH;
			int dir = creatureNode.attribute("direction").as_int(-1);
			if(dir >= DIRECTION_FIRST && dir <= DIRECTION_LAST) {
				direction = (Direction)dir;
			}

			Position creaturePosition(spawnPosition);

			pugi::xml_attribute xAttribute = creatureNode.attribute("x");
			pugi::xml_attribute yAttribute = creatureNode.attribute("y");
			if(!xAttribute || !yAttribute) {
				wxString err;
				err << "Bad creature position data, discarding creature \"" << name << "\" at spawn " << creaturePosition.x << ":" << creaturePosition.y << ":" << creaturePosition.z << " due to invalid position.";
				warnings.Add(err);
				break;
			}

			creaturePosition.x += pugi::cast<int32_t>(xAttribute.value());
			creaturePosition.y += pugi::cast<int32_t>(yAttribute.value());

			radius = std::max<int32_t>(radius, std::abs(creaturePosition.x - spawnPosition.x));
			radius = std::max<int32_t>(radius, std::abs(creaturePosition.y - spawnPosition.y));
			radius = std::min<int32_t>(radius, g_settings.getInteger(Config::MAX_SPAWN_RADIUS));

			Tile* creatureTile;
			if(creaturePosition == spawnPosition) {
				creatureTile = tile;
			} else {
				creatureTile = map.getTile(creaturePosition);
			}

			if(!creatureTile) {
				wxString err;
				err << "Discarding creature \"" << name << "\" at " << creaturePosition.x << ":" << creaturePosition.y << ":" << creaturePosition.z << " due to invalid position.";
				warnings.Add(err);
				break;
			}

			if(creatureTile->creature) {
				wxString err;
				err << "Duplicate creature \"" << name << "\" at " << creaturePosition.x << ":" << creaturePosition.y << ":" << creaturePosition.z << " was discarded.";
				warnings.Add(err);
				break;
			}

			CreatureType* type = g_creatures[name];
			if(!type) {
				type = g_creatures.addMissingCreatureType(name, isNpc);
			}

			Creature* creature = newd Creature(type);
			creature->setDirection(direction);
			creature->setSpawnTime(spawntime);
			creatureTile->creature = creature;

			if(creatureTile->getLocation()->getSpawnCount() == 0) {
				// No spawn, create a newd one
				ASSERT(creatureTile->spawn == nullptr);
				Spawn* spawn = newd Spawn(5);
				creatureTile->spawn = spawn;
				map.addSpawn(creatureTile);
			}
		}
	}
	return true;
}

bool IOMapRBM::loadHouses(Map& map, const FileName& dir)
{
	std::string fn = (const char*)(dir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME).mb_str(wxConvUTF8));
	fn += map.housefile;
	FileName filename(wxstr(fn));
	if(!filename.FileExists())
		return false;

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file(fn.c_str());
	if(!result) {
		return false;
	}
	return loadHouses(map, doc);
}

bool IOMapRBM::loadHouses(Map& map, pugi::xml_document& doc)
{
	pugi::xml_node node = doc.child("houses");
	if(!node) {
		warnings.push_back("IOMapRBM::loadHouses: Invalid rootheader.");
		return false;
	}

	pugi::xml_attribute attribute;
	for(pugi::xml_node houseNode = node.first_child(); houseNode; houseNode = houseNode.next_sibling()) {
		if(as_lower_str(houseNode.name()) != "house") {
			continue;
		}

		House* house = nullptr;
		if((attribute = houseNode.attribute("houseid"))) {
			house = map.houses.getHouse(pugi::cast<int32_t>(attribute.value()));
			if(!house) {
				break;
			}
		}

		if((attribute = houseNode.attribute("name"))) {
			house->name = attribute.as_string();
		} else {
			house->name = "House #" + std::to_string(house->id);
		}

		Position exitPosition(
			pugi::cast<int32_t>(houseNode.attribute("entryx").value()),
			pugi::cast<int32_t>(houseNode.attribute("entryy").value()),
			pugi::cast<int32_t>(houseNode.attribute("entryz").value())
		);
		if(exitPosition.x != 0 && exitPosition.y != 0 && exitPosition.z != 0) {
			house->setExit(exitPosition);
		}

		if((attribute = houseNode.attribute("rent"))) {
			house->rent = pugi::cast<int32_t>(attribute.value());
		}

		if((attribute = houseNode.attribute("guildhall"))) {
			house->guildhall = attribute.as_bool();
		}

		if((attribute = houseNode.attribute("townid"))) {
			house->townid = pugi::cast<int32_t>(attribute.value());
		} else {
			warning("House %d has no town! House was removed.", house->id);
			map.houses.removeHouse(house);
		}
	}
	return true;
}

bool IOMapRBM::saveMap(Map& map, const FileName& identifier)
{
#ifdef OTGZ_SUPPORT
	if(identifier.GetExt() == "rgz") {
		// Create the archive
		struct archive* a = archive_write_new();
		struct archive_entry* entry = nullptr;
		std::ostringstream streamData;

		archive_write_set_compression_gzip(a);
		archive_write_set_format_pax_restricted(a);
		archive_write_open_filename(a, nstr(identifier.GetFullPath()).c_str());

		g_gui.SetLoadDone(0, "Saving spawns...");

		pugi::xml_document spawnDoc;
		if(saveSpawns(map, spawnDoc)) {
			// Write the data
			spawnDoc.save(streamData, "", pugi::format_raw, pugi::encoding_utf8);
			std::string xmlData = streamData.str();

			// Write to the arhive
			entry = archive_entry_new();
			archive_entry_set_pathname(entry, "world/spawns.xml");
			archive_entry_set_size(entry, xmlData.size());
			archive_entry_set_filetype(entry, AE_IFREG);
			archive_entry_set_perm(entry, 0644);

			// Write to the archive
			archive_write_header(a, entry);
			archive_write_data(a, xmlData.data(), xmlData.size());

			// Free the entry
			archive_entry_free(entry);
			streamData.str("");
		}

		g_gui.SetLoadDone(0, "Saving houses...");

		pugi::xml_document houseDoc;
		if(saveHouses(map, houseDoc)) {
			// Write the data
			houseDoc.save(streamData, "", pugi::format_raw, pugi::encoding_utf8);
			std::string xmlData = streamData.str();

			// Write to the arhive
			entry = archive_entry_new();
			archive_entry_set_pathname(entry, "world/houses.xml");
			archive_entry_set_size(entry, xmlData.size());
			archive_entry_set_filetype(entry, AE_IFREG);
			archive_entry_set_perm(entry, 0644);

			// Write to the archive
			archive_write_header(a, entry);
			archive_write_data(a, xmlData.data(), xmlData.size());

			// Free the entry
			archive_entry_free(entry);
			streamData.str("");
		}

		g_gui.SetLoadDone(0, "Saving RBM map...");

		MemoryNodeFileWriteHandle rbmWriter;
		saveMap(map, rbmWriter);

		g_gui.SetLoadDone(75, "Compressing...");

		// Create an archive entry for the rbm file
		entry = archive_entry_new();
		archive_entry_set_pathname(entry, "world/map.rbm");
		archive_entry_set_size(entry, rbmWriter.getSize() + 4); // 4 bytes extra for header
		archive_entry_set_filetype(entry, AE_IFREG);
		archive_entry_set_perm(entry, 0644);
		archive_write_header(a, entry);

		// Write the version header
		char rbm_identifier[] = "RBM";
		archive_write_data(a, rbm_identifier, 4);

		// Write the RBM data
		archive_write_data(a, rbmWriter.getMemory(), rbmWriter.getSize());
		archive_entry_free(entry);

		// Free / close the archive
		archive_write_close(a);
		archive_write_free(a);

		g_gui.DestroyLoadBar();
		return true;
	}
#endif

	DiskNodeFileWriteHandle f(
		nstr(identifier.GetFullPath()),
		(g_settings.getInteger(Config::SAVE_WITH_OTB_MAGIC_NUMBER) ? "RBM" : std::string(4, '\0'))
		);

	if(!f.isOk()) {
		error("Can not open file %s for writing", (const char*)identifier.GetFullPath().mb_str(wxConvUTF8));
		return false;
	}

	if(!saveMap(map, f))
		return false;

	g_gui.SetLoadDone(99, "Saving spawns...");
	saveSpawns(map, identifier);

	g_gui.SetLoadDone(99, "Saving houses...");
	saveHouses(map, identifier);
	return true;
}

bool IOMapRBM::saveMap(Map& map, NodeFileWriteHandle& f)
{
	/* STOP!
	 * Before you even think about modifying this, please reconsider.
	 * while adding stuff to the binary format may be "cool", you'll
	 * inevitably make it incompatible with any future releases of
	 * the map editor, meaning you cannot reuse your map. Before you
	 * try to modify this, PLEASE consider using an external file
	 * like spawns.xml or houses.xml, as that will be MUCH easier
	 * to port to newer versions of the editor than a custom binary
	 * format.
	 */

	const IOMapRBM& self = *this;

	FileName tmpName;
	MapVersionID mapVersion = MAP_RBM_1; // CHANGE PLEASE

	f.addNode(0);
	{
		f.addU32(mapVersion); // Version

		f.addU16(map.width);
		f.addU16(map.height);

		f.addU32(g_items.MajorVersion); // remove
		f.addU32(g_items.MinorVersion); // remove

		f.addNode(RBM_MAP_DATA);
		{
			f.addByte(RBM_ATTR_DESCRIPTION);
			// Neither SimOne's nor OpenTibia cares for additional description tags
			f.addString("Saved with Ruby Map Editor " + __RME_VERSION__);

			f.addU8(RBM_ATTR_DESCRIPTION);
			f.addString(map.description);

			tmpName.Assign(wxstr(map.spawnfile));
			f.addU8(RBM_ATTR_EXT_SPAWN_FILE);
			f.addString(nstr(tmpName.GetFullName()));

			tmpName.Assign(wxstr(map.housefile));
			f.addU8(RBM_ATTR_EXT_HOUSE_FILE);
			f.addString(nstr(tmpName.GetFullName()));

			// Start writing tiles
			uint32_t tiles_saved = 0;
			bool first = true;

			int local_x = -1, local_y = -1, local_z = -1;

			MapIterator map_iterator = map.begin();
			while(map_iterator != map.end()) {
				// Update progressbar
				++tiles_saved;
				if(tiles_saved % 8192 == 0)
					g_gui.SetLoadDone(int(tiles_saved / double(map.getTileCount()) * 100.0));

				// Get tile
				Tile* save_tile = (*map_iterator)->get();

				// Is it an empty tile that we can skip? (Leftovers...)
				if(!save_tile || save_tile->size() == 0) {
					++map_iterator;
					continue;
				}

				const Position& pos = save_tile->getPosition();

				// Decide if newd node should be created
				if(pos.x < local_x || pos.x >= local_x + 256 || pos.y < local_y || pos.y >= local_y + 256 || pos.z != local_z) {
					// End last node
					if(!first) {
						f.endNode();
					}
					first = false;

					// Start newd node
					f.addNode(RBM_TILE_AREA);
					f.addU16(local_x = pos.x & 0xFF00);
					f.addU16(local_y = pos.y & 0xFF00);
					f.addU8( local_z = pos.z);
				}
				f.addNode(save_tile->isHouseTile()? RBM_HOUSETILE : RBM_TILE);

				f.addU8(save_tile->getX() & 0xFF);
				f.addU8(save_tile->getY() & 0xFF);

				if(save_tile->isHouseTile()) {
					f.addU32(save_tile->getHouseID());
				}

				if(save_tile->getMapFlags()) {
					f.addByte(RBM_ATTR_TILE_FLAGS);
					f.addU32(save_tile->getMapFlags());
				}

				if(save_tile->ground) {
					Item* ground = save_tile->ground;
					if(ground->isMetaItem()) {
						// Do nothing, we don't save metaitems...
					} else if(ground->hasBorderEquivalent()) {
						bool found = false;
						for(Item* item : save_tile->items) {
							if(item->getGroundEquivalent() == ground->getID()) {
								// Do nothing
								// Found equivalent
								found = true;
								break;
							}
						}

						if(!found) {
							ground->serializeItemNode_RBM(self, f);
						}
					} else if(ground->isComplex()) {
						ground->serializeItemNode_RBM(self, f);
					} else {
						f.addByte(RBM_ATTR_ITEM);
						ground->serializeItemCompact_RBM(self, f);
					}
				}

				for(Item* item : save_tile->items) {
					if(!item->isMetaItem()) {
						item->serializeItemNode_RBM(self, f);
					}
				}

				f.endNode();
				++map_iterator;
			}

			// Only close the last node if one has actually been created
			if(!first) {
				f.endNode();
			}

			f.addNode(RBM_TOWNS);
			for(const auto& townEntry : map.towns) {
				Town* town = townEntry.second;
				const Position& townPosition = town->getTemplePosition();
				f.addNode(RBM_TOWN);
					f.addU32(town->getID());
					f.addString(town->getName());
					f.addU16(townPosition.x);
					f.addU16(townPosition.y);
					f.addU8(townPosition.z);
				f.endNode();
			}
			f.endNode();

			f.addNode(RBM_WAYPOINTS);
			for(const auto& waypointEntry : map.waypoints) {
				Waypoint* waypoint = waypointEntry.second;
				f.addNode(RBM_WAYPOINT);
					f.addString(waypoint->name);
					f.addU16(waypoint->pos.x);
					f.addU16(waypoint->pos.y);
					f.addU8(waypoint->pos.z);
				f.endNode();
			}
			f.endNode();
		}
		f.endNode();
	}
	f.endNode();
	return true;
}

bool IOMapRBM::saveSpawns(Map& map, const FileName& dir)
{
	wxString filepath = dir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
	filepath += wxString(map.spawnfile.c_str(), wxConvUTF8);

	// Create the XML file
	pugi::xml_document doc;
	if(saveSpawns(map, doc)) {
		return doc.save_file(filepath.wc_str(), "\t", pugi::format_default, pugi::encoding_utf8);
	}
	return false;
}

bool IOMapRBM::saveSpawns(Map& map, pugi::xml_document& doc)
{
	pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
	if(!decl) {
		return false;
	}

	decl.append_attribute("version") = "1.0";

	CreatureList creatureList;

	pugi::xml_node spawnNodes = doc.append_child("spawns");
	for(const auto& spawnPosition : map.spawns) {
		Tile *tile = map.getTile(spawnPosition);
		if (tile == nullptr)
			continue;

		Spawn* spawn = tile->spawn;
		ASSERT(spawn);

		pugi::xml_node spawnNode = spawnNodes.append_child("spawn");

		spawnNode.append_attribute("centerx") = spawnPosition.x;
		spawnNode.append_attribute("centery") = spawnPosition.y;
		spawnNode.append_attribute("centerz") = spawnPosition.z;

		int32_t radius = spawn->getSize();
		spawnNode.append_attribute("radius") = radius;

		for(int32_t y = -radius; y <= radius; ++y) {
			for(int32_t x = -radius; x <= radius; ++x) {
				Tile* creature_tile = map.getTile(spawnPosition + Position(x, y, 0));
				if(creature_tile) {
					Creature* creature = creature_tile->creature;
					if(creature && !creature->isSaved()) {
						pugi::xml_node creatureNode = spawnNode.append_child(creature->isNpc() ? "npc" : "monster");

						creatureNode.append_attribute("name") = creature->getName().c_str();
						creatureNode.append_attribute("x") = x;
						creatureNode.append_attribute("y") = y;
						creatureNode.append_attribute("z") = spawnPosition.z;
						creatureNode.append_attribute("spawntime") = creature->getSpawnTime();
						if(creature->getDirection() != NORTH) {
							creatureNode.append_attribute("direction") = creature->getDirection();
						}

						// Mark as saved
						creature->save();
						creatureList.push_back(creature);
					}
				}
			}
		}
	}

	for(Creature* creature : creatureList) {
		creature->reset();
	}
	return true;
}

bool IOMapRBM::saveHouses(Map& map, const FileName& dir)
{
	wxString filepath = dir.GetPath(wxPATH_GET_SEPARATOR | wxPATH_GET_VOLUME);
	filepath += wxString(map.housefile.c_str(), wxConvUTF8);

	// Create the XML file
	pugi::xml_document doc;
	if(saveHouses(map, doc)) {
		return doc.save_file(filepath.wc_str(), "\t", pugi::format_default, pugi::encoding_utf8);
	}
	return false;
}

bool IOMapRBM::saveHouses(Map& map, pugi::xml_document& doc)
{
	pugi::xml_node decl = doc.prepend_child(pugi::node_declaration);
	if(!decl) {
		return false;
	}

	decl.append_attribute("version") = "1.0";

	pugi::xml_node houseNodes = doc.append_child("houses");
	for(const auto& houseEntry : map.houses) {
		const House* house = houseEntry.second;
		pugi::xml_node houseNode = houseNodes.append_child("house");

		houseNode.append_attribute("name") = house->name.c_str();
		houseNode.append_attribute("houseid") = house->id;

		const Position& exitPosition = house->getExit();
		houseNode.append_attribute("entryx") = exitPosition.x;
		houseNode.append_attribute("entryy") = exitPosition.y;
		houseNode.append_attribute("entryz") = exitPosition.z;

		houseNode.append_attribute("rent") = house->rent;
		if(house->guildhall) {
			houseNode.append_attribute("guildhall") = true;
		}

		houseNode.append_attribute("townid") = house->townid;
		houseNode.append_attribute("size") = static_cast<int32_t>(house->size());
	}
	return true;
}

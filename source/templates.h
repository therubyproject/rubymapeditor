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
// $URL: http://svn.rebarp.se/svn/RME/trunk/source/templates.h $
// $Id: templates.h 298 2010-02-23 17:09:13Z admin $

#ifndef RME_MAP_TEMPLATES_H_
#define RME_MAP_TEMPLATES_H_

#include "common.h"

struct ConversionMap {
	// Many to Many
	typedef std::map<std::vector<uint16_t>, std::vector<uint16_t> > MTM;
	// Single to Many
	typedef std::map<uint16_t, std::vector<uint16_t> > STM;

	MTM mtm;
	STM stm;
};

ConversionMap getReplacementMapFrom800To810();
ConversionMap getReplacementMapFrom760To740();
ConversionMap getReplacementMapFrom854To854();
ConversionMap getReplacementMapClassic();

#endif

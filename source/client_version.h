//////////////////////////////////////////////////////////////////////
// This file is part of Ruby Map Editor
//////////////////////////////////////////////////////////////////////

#ifndef RME_CLIENT_VERSION_H_
#define RME_CLIENT_VERSION_H_

#include "main.h"
#include "settings.h"

typedef int ClientVersionID;

// RBM versions
enum MapVersionID
{
	MAP_RBM_UNKNOWN = -1,
	MAP_RBM_1 = 0,
};

enum DatFlags : uint8_t
{
	DatFlagGround = 0,
	DatFlagGroundBorder = 1,
	DatFlagOnBottom = 2,
	DatFlagOnTop = 3,
	DatFlagContainer = 4,
	DatFlagStackable = 5,
	DatFlagForceUse = 6,
	DatFlagMultiUse = 7,
	DatFlagWritable = 8,
	DatFlagWritableOnce = 9,
	DatFlagFluidContainer = 10,
	DatFlagSplash = 11,
	DatFlagNotWalkable = 12,
	DatFlagNotMoveable = 13,
	DatFlagBlockProjectile = 14,
	DatFlagNotPathable = 15,
	DatFlagPickupable = 16,
	DatFlagHangable = 17,
	DatFlagHookSouth = 18,
	DatFlagHookEast = 19,
	DatFlagRotateable = 20,
	DatFlagLight = 21,
	DatFlagDontHide = 22,
	DatFlagTranslucent = 23,
	DatFlagDisplacement = 24,
	DatFlagElevation = 25,
	DatFlagLyingCorpse = 26,
	DatFlagAnimateAlways = 27,
	DatFlagMinimapColor = 28,
	DatFlagLensHelp = 29,
	DatFlagFullGround = 30,
	DatFlagLook = 31,
	DatFlagCloth = 32,
	DatFlagMarket = 33,
	DatFlagUsable = 34,
	DatFlagWrappable = 35,
	DatFlagUnwrappable = 36,
	DatFlagTopEffect = 37,

	DatFlagFloorChange = 252,
	DatFlagNoMoveAnimation = 253, // 10.10: real value is 16, but we need to do this for backwards compatibility
	DatFlagChargeable = 254,
	DatFlagLast = 255
};

#endif

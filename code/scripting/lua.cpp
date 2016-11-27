#include "ai/ai.h"
#include "ai/aigoals.h"
#include "asteroid/asteroid.h"
#include "camera/camera.h"
#include "cfile/cfilesystem.h"
#include "cmdline/cmdline.h"
#include "cutscene/movie.h"
#include "debris/debris.h"
#include "freespace.h"
#include "gamesequence/gamesequence.h"
#include "globalincs/linklist.h"
#include "graphics/2d.h"
#include "graphics/font.h"
#include "graphics/generic.h"
#include "graphics/opengl/gropenglpostprocessing.h"
#include "headtracking/headtracking.h"
#include "hud/hudbrackets.h"
#include "hud/hudconfig.h"
#include "hud/hudescort.h"
#include "hud/hudets.h"
#include "hud/hudgauges.h"
#include "hud/hudshield.h"
#include "iff_defs/iff_defs.h"
#include "io/joy.h"
#include "io/key.h"
#include "io/mouse.h"
#include "io/timer.h"
#include "jumpnode/jumpnode.h"
#include "lighting/lighting.h"
#include "menuui/credits.h"
#include "mission/missioncampaign.h"
#include "mission/missiongoals.h"
#include "mission/missionload.h"
#include "mission/missionlog.h"
#include "mission/missionmessage.h"
#include "mission/missiontraining.h"
#include "missionui/missionbrief.h"
#include "model/model.h"
#include "network/multi.h"
#include "network/multimsgs.h"
#include "object/objectshield.h"
#include "object/waypoint.h"
#include "scripting/lua.h"
#include "parse/parselo.h"
#include "scripting/scripting.h"
#include "particle/particle.h"
#include "playerman/player.h"
#include "render/3d.h"
#include "render/3dinternal.h"
#include "scripting/ade_api.h"
#include "ship/ship.h"
#include "ship/shipfx.h"
#include "ship/shiphit.h"
#include "sound/audiostr.h"
#include "sound/ds.h"
#include "weapon/beam.h"
#include "weapon/weapon.h"

#define BMPMAN_INTERNAL
#include "bmpman/bm_internal.h"

#include "scripting/api/bitops.h"
#include "scripting/api/enums.h"
#include "scripting/api/vecmath.h"
#include "scripting/api/event.h"
#include "scripting/api/file.h"
#include "scripting/api/font.h"
#include "scripting/api/gameevent.h"
#include "scripting/api/gamestate.h"
#include "scripting/api/hudgauge.h"
#include "scripting/api/eye.h"

using namespace scripting;
using namespace scripting::api;

int ade_set_object_with_breed(lua_State *L, int obj_idx);

//**********Handles
/*ade_obj<int> l_Camera("camera", "Camera handle");
ade_obj<int> l_Cmission("cmission", "Campaign mission handle"); //WMC - We can get away with a pointer right now, but if it ever goes dynamic, it'd be a prob
ade_obj<enum_h> l_Enum("enumeration", "Enumeration object");
ade_obj<int> l_Event("event", "Mission event handle");
ade_obj<int> l_Font("font", "font handle");
ade_obj<matrix_h> l_Matrix("orientation", "Orientation matrix object");
ade_obj<int> l_Model("model", "3D Model (POF) handle");
ade_obj<object_h> l_Object("object", "Object handle");
ade_obj<physics_info_h> l_Physics("physics", "Physics handle");
ade_obj<int> l_Player("player", "Player handle");
ade_obj<object_h> l_Shields("shields", "Shields handle");
ade_obj<object_h> l_Ship("ship", "Ship handle", &l_Object);
ade_obj<int> l_Shipclass("shipclass", "Ship class handle");
ade_obj<object_h> l_ShipTextures("shiptextures", "Ship textures handle");
ade_obj<int> l_Shiptype("shiptype", "Ship type handle");
ade_obj<int> l_Species("species", "Species handle");
ade_obj<ship_subsys_h> l_Subsystem("subsystem", "Ship subsystem handle");
ade_obj<int> l_Team("team", "Team handle");
ade_obj<int> l_Texture("texture", "Texture handle");
ade_obj<int> l_Wing("wing", "Wing handle");
ade_obj<vec3d> l_Vector("vector", "Vector object");
ade_obj<object_h> l_Weapon("weapon", "Weapon handle", &l_Object);
ade_obj<ship_bank_h> l_WeaponBank("weaponbank", "Ship/subystem weapons bank handle");
ade_obj<ship_banktype_h> l_WeaponBankType("weaponbanktype", "Ship/subsystem weapons bank type handle");
ade_obj<int> l_Weaponclass("weaponclass", "Weapon class handle");
*/
//###########################################################
//########################<IMPORTANT>########################
//###########################################################
//If you are a coder who wants to add libraries, functions,
//or objects to Lua, then you want to be below this point.
//###########################################################
//########################</IMPORTANT>#######################
//###########################################################

flag_def_list plr_commands[] = {
	{	"TARGET_NEXT",							TARGET_NEXT,							0	},
	{	"TARGET_PREV",							TARGET_PREV,							0	},
	{	"TARGET_NEXT_CLOSEST_HOSTILE",			TARGET_NEXT_CLOSEST_HOSTILE,			0	},
	{	"TARGET_PREV_CLOSEST_HOSTILE",			TARGET_PREV_CLOSEST_HOSTILE,			0	},
	{	"TOGGLE_AUTO_TARGETING",				TOGGLE_AUTO_TARGETING,					0	},
	{	"TARGET_NEXT_CLOSEST_FRIENDLY",			TARGET_NEXT_CLOSEST_FRIENDLY,			0	},
	{	"TARGET_PREV_CLOSEST_FRIENDLY",			TARGET_PREV_CLOSEST_FRIENDLY,			0	},
	{	"TARGET_SHIP_IN_RETICLE",				TARGET_SHIP_IN_RETICLE,					0	},
	{	"TARGET_CLOSEST_SHIP_ATTACKING_TARGET",	TARGET_CLOSEST_SHIP_ATTACKING_TARGET,	0	},
	{	"TARGET_LAST_TRANMISSION_SENDER",		TARGET_LAST_TRANMISSION_SENDER,			0	},
	{	"STOP_TARGETING_SHIP",					STOP_TARGETING_SHIP,					0	},
	{	"TARGET_SUBOBJECT_IN_RETICLE",			TARGET_SUBOBJECT_IN_RETICLE,			0	},
	{	"TARGET_NEXT_SUBOBJECT",				TARGET_NEXT_SUBOBJECT,					0	},
	{	"TARGET_PREV_SUBOBJECT",				TARGET_PREV_SUBOBJECT,					0	},
	{	"STOP_TARGETING_SUBSYSTEM",				STOP_TARGETING_SUBSYSTEM,				0	},
	{	"MATCH_TARGET_SPEED",					MATCH_TARGET_SPEED,						0	},
	{	"TOGGLE_AUTO_MATCH_TARGET_SPEED",		TOGGLE_AUTO_MATCH_TARGET_SPEED,			0	},
	{	"FIRE_PRIMARY",							FIRE_PRIMARY,							0	},
	{	"FIRE_SECONDARY",						FIRE_SECONDARY,							0	},
	{	"CYCLE_NEXT_PRIMARY",					CYCLE_NEXT_PRIMARY,						0	},
	{	"CYCLE_PREV_PRIMARY",					CYCLE_PREV_PRIMARY,						0	},
	{	"CYCLE_SECONDARY",						CYCLE_SECONDARY,						0	},
	{	"CYCLE_NUM_MISSLES",					CYCLE_NUM_MISSLES,						0	},
	{	"LAUNCH_COUNTERMEASURE",				LAUNCH_COUNTERMEASURE,					0	},
	{	"FORWARD_THRUST",						FORWARD_THRUST,							0	},
	{	"REVERSE_THRUST",						REVERSE_THRUST,							0	},
	{	"BANK_LEFT",							BANK_LEFT,								0	},
	{	"BANK_RIGHT",							BANK_RIGHT,								0	},
	{	"PITCH_FORWARD",						PITCH_FORWARD,							0	},
	{	"PITCH_BACK",							PITCH_BACK,								0	},
	{	"YAW_LEFT",								YAW_LEFT,								0	},
	{	"YAW_RIGHT",							YAW_RIGHT,								0	},
	{	"ZERO_THROTTLE",						ZERO_THROTTLE,							1	},
	{	"MAX_THROTTLE",							MAX_THROTTLE,							1	},
	{	"ONE_THIRD_THROTTLE",					ONE_THIRD_THROTTLE,						1	},
	{	"TWO_THIRDS_THROTTLE",					TWO_THIRDS_THROTTLE,					1	},
	{	"PLUS_5_PERCENT_THROTTLE",				PLUS_5_PERCENT_THROTTLE,				1	},
	{	"MINUS_5_PERCENT_THROTTLE",				MINUS_5_PERCENT_THROTTLE,				1	},
	{	"ATTACK_MESSAGE",						ATTACK_MESSAGE,							1	},
	{	"DISARM_MESSAGE",						DISARM_MESSAGE,							1	},
	{	"DISABLE_MESSAGE",						DISABLE_MESSAGE,						1	},
	{	"ATTACK_SUBSYSTEM_MESSAGE",				ATTACK_SUBSYSTEM_MESSAGE,				1	},
	{	"CAPTURE_MESSAGE",						CAPTURE_MESSAGE,						1	},
	{	"ENGAGE_MESSAGE",						ENGAGE_MESSAGE,							1	},
	{	"FORM_MESSAGE",							FORM_MESSAGE,							1	},
	{	"IGNORE_MESSAGE",						IGNORE_MESSAGE,							1	},
	{	"PROTECT_MESSAGE",						PROTECT_MESSAGE,						1	},
	{	"COVER_MESSAGE",						COVER_MESSAGE,							1	},
	{	"WARP_MESSAGE",							WARP_MESSAGE,							1	},
	{	"REARM_MESSAGE",						REARM_MESSAGE,							1	},
	{	"TARGET_CLOSEST_SHIP_ATTACKING_SELF",	TARGET_CLOSEST_SHIP_ATTACKING_SELF,		1	},
	{	"VIEW_CHASE",							VIEW_CHASE,								1	},
	{	"VIEW_EXTERNAL",						VIEW_EXTERNAL,							1	},
	{	"VIEW_EXTERNAL_TOGGLE_CAMERA_LOCK",		VIEW_EXTERNAL_TOGGLE_CAMERA_LOCK,		1	},
	{	"VIEW_SLEW",							VIEW_SLEW,								1	},
	{	"VIEW_OTHER_SHIP",						VIEW_OTHER_SHIP,						1	},
	{	"VIEW_DIST_INCREASE",					VIEW_DIST_INCREASE,						1	},
	{	"VIEW_DIST_DECREASE",					VIEW_DIST_DECREASE,						1	},
	{	"VIEW_CENTER",							VIEW_CENTER,							1	},
	{	"PADLOCK_UP",							PADLOCK_UP,								1	},
	{	"PADLOCK_DOWN",							PADLOCK_DOWN,							1	},
	{	"PADLOCK_LEFT",							PADLOCK_LEFT,							1	},
	{	"PADLOCK_RIGHT",						PADLOCK_RIGHT,							1	},
	{	"RADAR_RANGE_CYCLE",					RADAR_RANGE_CYCLE,						1	},
	{	"SQUADMSG_MENU",						SQUADMSG_MENU,							2	},
	{	"SHOW_GOALS",							SHOW_GOALS,								2	},
	{	"END_MISSION",							END_MISSION,							2	},
	{	"TARGET_TARGETS_TARGET",				TARGET_TARGETS_TARGET,					2	},
	{	"AFTERBURNER",							AFTERBURNER,							2	},
	{	"INCREASE_WEAPON",						INCREASE_WEAPON,						2	},
	{	"DECREASE_WEAPON",						DECREASE_WEAPON,						2	},
	{	"INCREASE_SHIELD",						INCREASE_SHIELD,						2	},
	{	"DECREASE_SHIELD",						DECREASE_SHIELD,						2	},
	{	"INCREASE_ENGINE",						INCREASE_ENGINE,						2	},
	{	"DECREASE_ENGINE",						DECREASE_ENGINE,						2	},
	{	"ETS_EQUALIZE",							ETS_EQUALIZE,							2	},
	{	"SHIELD_EQUALIZE",						SHIELD_EQUALIZE,						2	},
	{	"SHIELD_XFER_TOP",						SHIELD_XFER_TOP,						2	},
	{	"SHIELD_XFER_BOTTOM",					SHIELD_XFER_BOTTOM,						2	},
	{	"SHIELD_XFER_LEFT",						SHIELD_XFER_LEFT,						2	},
	{	"SHIELD_XFER_RIGHT",					SHIELD_XFER_RIGHT,						2	},
	{	"XFER_SHIELD",							XFER_SHIELD,							2	},
	{	"XFER_LASER",							XFER_LASER,								2	},
	{	"GLIDE_WHEN_PRESSED",					GLIDE_WHEN_PRESSED,						2	},
	{	"BANK_WHEN_PRESSED",					BANK_WHEN_PRESSED,						2	},
	{	"SHOW_NAVMAP",							SHOW_NAVMAP,							2	},
	{	"ADD_REMOVE_ESCORT",					ADD_REMOVE_ESCORT,						2	},
	{	"ESCORT_CLEAR",							ESCORT_CLEAR,							2	},
	{	"TARGET_NEXT_ESCORT_SHIP",				TARGET_NEXT_ESCORT_SHIP,				2	},
	{	"TARGET_CLOSEST_REPAIR_SHIP",			TARGET_CLOSEST_REPAIR_SHIP,				2	},
	{	"TARGET_NEXT_UNINSPECTED_CARGO",		TARGET_NEXT_UNINSPECTED_CARGO,			2	},
	{	"TARGET_PREV_UNINSPECTED_CARGO",		TARGET_PREV_UNINSPECTED_CARGO,			2	},
	{	"TARGET_NEWEST_SHIP",					TARGET_NEWEST_SHIP,						2	},
	{	"TARGET_NEXT_LIVE_TURRET",				TARGET_NEXT_LIVE_TURRET,				2	},
	{	"TARGET_PREV_LIVE_TURRET",				TARGET_PREV_LIVE_TURRET,				2	},
	{	"TARGET_NEXT_BOMB",						TARGET_NEXT_BOMB,						2	},
	{	"TARGET_PREV_BOMB",						TARGET_PREV_BOMB,						3	},
	{	"MULTI_MESSAGE_ALL",					MULTI_MESSAGE_ALL,						3	},
	{	"MULTI_MESSAGE_FRIENDLY",				MULTI_MESSAGE_FRIENDLY,					3	},
	{	"MULTI_MESSAGE_HOSTILE",				MULTI_MESSAGE_HOSTILE,					3	},
	{	"MULTI_MESSAGE_TARGET",					MULTI_MESSAGE_TARGET,					3	},
	{	"MULTI_OBSERVER_ZOOM_TO",				MULTI_OBSERVER_ZOOM_TO,					3	},
	{	"TIME_SPEED_UP",						TIME_SPEED_UP,							3	},
	{	"TIME_SLOW_DOWN",						TIME_SLOW_DOWN,							3	},
	{	"TOGGLE_HUD_CONTRAST",					TOGGLE_HUD_CONTRAST,					3	},
	{	"MULTI_TOGGLE_NETINFO",					MULTI_TOGGLE_NETINFO,					3	},
	{	"MULTI_SELF_DESTRUCT",					MULTI_SELF_DESTRUCT,					3	},
	{	"TOGGLE_HUD",							TOGGLE_HUD,								3	},
	{	"RIGHT_SLIDE_THRUST",					RIGHT_SLIDE_THRUST,						3	},
	{	"LEFT_SLIDE_THRUST",					LEFT_SLIDE_THRUST,						3	},
	{	"UP_SLIDE_THRUST",						UP_SLIDE_THRUST,						3	},
	{	"DOWN_SLIDE_THRUST",					DOWN_SLIDE_THRUST,						3	},
	{	"HUD_TARGETBOX_TOGGLE_WIREFRAME",		HUD_TARGETBOX_TOGGLE_WIREFRAME,			3	},
	{	"VIEW_TOPDOWN",							VIEW_TOPDOWN,							3	},
	{	"VIEW_TRACK_TARGET",					VIEW_TRACK_TARGET,						3	},
	{	"AUTO_PILOT_TOGGLE",					AUTO_PILOT_TOGGLE,						3	},
	{	"NAV_CYCLE",							NAV_CYCLE,								3	},
	{	"TOGGLE_GLIDING",						TOGGLE_GLIDING,							3	},
	{	"TOGGLE_AUTO_SHIELD_EQUALIZE",			TOGGLE_AUTO_SHIELD_EQUALIZE,			3	},
};

int num_plr_commands = sizeof(plr_commands)/sizeof(flag_def_list);

//**********HANDLE: model
class model_h
{
protected:
	int mid;
	polymodel *model;

public:
	polymodel *Get(){
		if(!this->IsValid())
			return NULL; 

		return model;
	}

	int GetID(){
		if(!this->IsValid())
			return -1;

		return mid;
	}

	bool IsValid(){
		return (model != NULL && mid > -1 && model_get(mid) == model);
	}

	model_h(int n_modelnum) {
		mid = n_modelnum;
		if(mid > 0)
			model = model_get(mid);
		else
			model = NULL;
	}
	model_h(polymodel *n_model) {
		model = n_model;
		if(n_model != NULL)
			mid = n_model->id;
		else
			mid = -1;
	}
	model_h(){
		mid=-1;
		model = NULL;
	}
};

class modeltextures_h : public model_h
{
public:
	modeltextures_h(polymodel *pm) : model_h(pm){}
	modeltextures_h() : model_h(){}
};
ade_obj<model_h> l_Model("model", "3D Model (POF) handle");

ade_obj<modeltextures_h> l_ModelTextures("modeltextures_h", "Array of materials");

class eyepoints_h : public model_h
{
public:
	eyepoints_h(polymodel *pm) : model_h(pm){}
	eyepoints_h() : model_h(){}
};

ade_obj<eyepoints_h> l_Eyepoints("eyepoints", "Array of model eye points");

// Thrusters:
class thrusters_h : public model_h 
{
	public:
	thrusters_h(polymodel *pm) : model_h(pm){}
	thrusters_h() : model_h(){}
};

ade_obj<thrusters_h> l_Thrusters("thrusters", "The thrusters of a model");

// Thrusterbank:
struct thrusterbank_h
{
	thruster_bank *bank;

	thrusterbank_h()
	{
		bank = NULL;
	}

	thrusterbank_h(thruster_bank* ba)
	{
		bank = ba;
	}

	thruster_bank *Get()
	{
		if (!isValid())
			return NULL;

		return bank;	
	}

	bool isValid()
	{
		return bank != NULL;
	}
};

ade_obj<thrusterbank_h> l_Thrusterbank("thrusterbank", "A model thrusterbank");

// Glowpoint:
struct glowpoint_h 
{
	glow_point *point;

	glowpoint_h() 
	{
	}

	glowpoint_h(glow_point* np)
	{
		point = np;
	}

	glow_point* Get()
	{
		if (!isValid())
			return NULL;

		return point;
	}

	bool isValid()
	{
		return point != NULL;
	}

};

ade_obj<glowpoint_h> l_Glowpoint("glowpoint", "A model glowpoint");

// Glowbanks:
class dockingbays_h : public model_h 
{
	public:
	dockingbays_h(polymodel *pm) : model_h(pm){}
	dockingbays_h() : model_h(){}
};
ade_obj<dockingbays_h> l_Dockingbays("dockingbays", "The docking bays of a model");

ADE_VIRTVAR(Textures, l_Model, "modeltextures_h", "Model textures", "modeltextures_h", "Model textures, or an invalid modeltextures handle if the model handle is invalid")
{
	model_h *mdl = NULL;
	modeltextures_h *oth = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_ModelTextures.GetPtr(&oth)))
		return ade_set_error(L, "o", l_ModelTextures.Set(modeltextures_h()));

	polymodel *pm = mdl->Get();
	if(pm == NULL)
		return ade_set_error(L, "o", l_ModelTextures.Set(modeltextures_h()));

	if(ADE_SETTING_VAR && oth && oth->IsValid()) {
		//WMC TODO: Copy code
		LuaError(L, "Attempt to use Incomplete Feature: Modeltextures copy");
	}

	return ade_set_args(L, "o", l_ModelTextures.Set(modeltextures_h(pm)));
}

ADE_VIRTVAR(Thrusters, l_Model, "thrusters", "Model thrusters", "thrusters", "Thrusters of the model or invalid handle")
{
	model_h *mdl = NULL;
	thrusters_h *oth = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_Thrusters.GetPtr(&oth)))
		return ade_set_error(L, "o", l_Thrusters.Set(thrusters_h()));

	polymodel *pm = mdl->Get();
	if(pm == NULL)
		return ade_set_error(L, "o", l_Thrusters.Set(thrusters_h()));

	if(ADE_SETTING_VAR && oth && oth->IsValid()) {
		LuaError(L, "Attempt to use Incomplete Feature: Thrusters copy");
	}

	return ade_set_args(L, "o", l_Thrusters.Set(thrusters_h(pm)));
}

ADE_VIRTVAR(Eyepoints, l_Model, "eyepoints", "Model eyepoints", "eyepoints", "Array of eyepoints or invalid handle on error")
{
	model_h *mdl = NULL;
	eyepoints_h *eph = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_Eyepoints.GetPtr(&eph)))
		return ade_set_error(L, "o", l_Eyepoints.Set(eyepoints_h()));

	polymodel *pm = mdl->Get();
	if(pm == NULL)
		return ade_set_error(L, "o", l_Eyepoints.Set(eyepoints_h()));

	if(ADE_SETTING_VAR && eph && eph->IsValid()) {
		LuaError(L, "Attempt to use Incomplete Feature: Eyepoints copy");
	}

	return ade_set_args(L, "o", l_Eyepoints.Set(eyepoints_h(pm)));
}

ADE_VIRTVAR(Dockingbays, l_Model, "dockingbays", "Docking bays handle of model", "dockingbays", "Array of docking bays on this model, or invalid handle on error")
{
	model_h *mdl = NULL;
	dockingbays_h *dbh = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_Dockingbays.GetPtr(&dbh)))
		return ade_set_error(L, "o", l_Dockingbays.Set(dockingbays_h()));

	polymodel *pm = mdl->Get();
	if(pm == NULL)
		return ade_set_error(L, "o", l_Dockingbays.Set(dockingbays_h()));

	if(ADE_SETTING_VAR && dbh && dbh->IsValid()) {
		LuaError(L, "Attempt to use Incomplete Feature: Docking bays copy");
	}

	return ade_set_args(L, "o", l_Dockingbays.Set(dockingbays_h(pm)));
}

extern void model_calc_bound_box( vec3d *box, vec3d *big_mn, vec3d *big_mx);

ADE_VIRTVAR(BoundingBoxMax, l_Model, "vector", "Model bounding box maximum", "vector", "Model bounding box, or an empty vector if the handle is invalid")
{
	model_h *mdl = NULL;
	vec3d *v = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v != NULL) {
		pm->maxs = *v;
		
		//Recalculate this, so it stays valid
		model_calc_bound_box(pm->bounding_box, &pm->mins, &pm->maxs);
	}

	return ade_set_args(L, "o", l_Vector.Set(pm->maxs));
}

ADE_VIRTVAR(BoundingBoxMin, l_Model, "vector", "Model bounding box minimum", "vector", "Model bounding box, or an empty vector if the handle is invalid")
{
	model_h *mdl = NULL;
	vec3d *v = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v != NULL) {
		pm->mins = *v;
		
		//Recalculate this, so it stays valid
		model_calc_bound_box(pm->bounding_box, &pm->mins, &pm->maxs);
	}

	return ade_set_args(L, "o", l_Vector.Set(pm->mins));
}

ADE_VIRTVAR(Filename, l_Model, "string", "Model filename", "string", "Model filename, or an empty string if the handle is invalid")
{
	model_h *mdl = NULL;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Model.GetPtr(&mdl), &s))
		return ade_set_error(L, "s", "");

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR) {
		strncpy(pm->filename, s, sizeof(pm->filename) - sizeof(char));
	}

	return ade_set_args(L, "s", pm->filename);
}

ADE_VIRTVAR(Mass, l_Model, "number", "Model mass", "number", "Model mass, or 0 if the model handle is invalid")
{
	model_h *mdl = NULL;
	float nm = 0.0f;
	if(!ade_get_args(L, "o|f", l_Model.GetPtr(&mdl), &nm))
		return ade_set_error(L, "f", 0.0f);

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pm->mass = nm;
	}

	return ade_set_args(L, "f", pm->mass);
}

ADE_VIRTVAR(MomentOfInertia, l_Model, "orientation", "Model moment of inertia", "orientation", "Moment of Inertia matrix or identity matrix if invalid" )
{
	model_h *mdl = NULL;
	matrix_h *mh = NULL;
	if(!ade_get_args(L, "o|o", l_Model.GetPtr(&mdl), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	if(ADE_SETTING_VAR && mh != NULL) {
		matrix *mtx = mh->GetMatrix();
		memcpy(&pm->moment_of_inertia, mtx, sizeof(*mtx));
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&pm->moment_of_inertia)));
}

ADE_VIRTVAR(Radius, l_Model, "number", "Model radius (Used for collision & culling detection)", "number", "Model Radius or 0 if invalid")
{
	model_h *mdl = NULL;
	float nr = 0.0f;
	if(!ade_get_args(L, "o|f", l_Model.GetPtr(&mdl), &nr))
		return ade_set_error(L, "f", 0.0f);

	polymodel *pm = mdl->Get();

	if(pm == NULL)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pm->rad = nr;
	}

	return ade_set_args(L, "f", pm->rad);
}

ADE_FUNC(isValid, l_Model, NULL, "True if valid, false or nil if not", "boolean", "Detects whether handle is valid")
{
	model_h *mdl;
	if(!ade_get_args(L, "o", l_Model.GetPtr(&mdl)))
		return ADE_RETURN_NIL;

	return mdl->IsValid();
}

//**********HANDLE: eyepoints
ADE_FUNC(__len, l_Eyepoints, NULL, "Gets the number of eyepoints on this model", "number", "Number of eyepoints on this model or 0 on error")
{
	eyepoints_h *eph = NULL;
	if (!ade_get_args(L, "o", l_Eyepoints.GetPtr(&eph)))
	{
		return ade_set_error(L, "i", 0);
	}

	if (!eph->IsValid())
	{
		return ade_set_error(L, "i", 0);
	}

	polymodel *pm = eph->Get();

	if (pm == NULL)
	{
		return ade_set_error(L, "i", 0);
	}

	return ade_set_args(L, "i", pm->n_view_positions);
}

ADE_INDEXER(l_Eyepoints, "eyepoint", "Gets en eyepoint handle", "eyepoint", "eye handle or invalid handle on error")
{
	eyepoints_h *eph = NULL;
	int index = -1;
	eye_h *eh = NULL;

	if (!ade_get_args(L, "oi|o", l_Eyepoints.GetPtr(&eph), &index, l_Eyepoint.GetPtr(&eh)))
	{
		return ade_set_error(L, "o", l_Eyepoint.Set(eye_h()));
	}

	if (!eph->IsValid())
	{
		return ade_set_error(L, "o", l_Eyepoint.Set(eye_h()));
	}

	polymodel *pm = eph->Get();

	if (pm == NULL)
	{
		return ade_set_error(L, "o", l_Eyepoint.Set(eye_h()));
	}

	index--; // Lua -> FS2

	if (index < 0 || index >= pm->n_view_positions)
	{
		return ade_set_error(L, "o", l_Eyepoint.Set(eye_h()));
	}

	if (ADE_SETTING_VAR && eh && eh->IsValid())
	{
		LuaError(L, "Attempted to use incomplete feature: Eyepoint copy");
	}

	return ade_set_args(L, "o", l_Eyepoint.Set(eye_h(eph->GetID(), index)));
}

ADE_FUNC(isValid, l_Eyepoints, NULL, "Detects whether handle is valid or not", "boolean", "true if valid false otherwise")
{
	eyepoints_h *eph;
	if(!ade_get_args(L, "o", l_Eyepoints.GetPtr(&eph)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", eph->IsValid());
}

//**********HANDLE: thrusters
ADE_FUNC(__len, l_Thrusters, NULL, "Number of thruster banks on the model", "number", "Number of thrusterbanks")
{
	thrusters_h *trh;
	if(!ade_get_args(L, "o", l_Thrusters.GetPtr(&trh)))
		return ade_set_error(L, "i", -1);

	if(!trh->IsValid())
		return ade_set_error(L, "i", -1);

	polymodel *pm = trh->Get();

	if(pm == NULL)
		return ade_set_error(L, "i", -1);

	return ade_set_args(L, "i", pm->n_thrusters);
}

ADE_INDEXER(l_Thrusters, "number Index", "Array of all thrusterbanks on this thruster", "thrusterbank", "Handle to the thrusterbank or invalid handle if index is invalid")
{
	thrusters_h *trh = NULL;
	char *s = NULL;
	thrusterbank_h newThr;

	if (!ade_get_args(L, "os|o", l_Thrusters.GetPtr(&trh), &s, l_Thrusterbank.Get(&newThr)))
		return ade_set_error(L, "o", l_Thrusterbank.Set(thrusterbank_h()));

	polymodel *pm = trh->Get();

	if (!trh->IsValid() || s == NULL || pm == NULL)
		return ade_set_error(L, "o", l_Thrusterbank.Set(thrusterbank_h()));
	
	//Determine index
	int idx = atoi(s) - 1;	//Lua->FS2

	if (idx < 0 || idx >= pm->n_thrusters)
		return ade_set_error(L, "o", l_Thrusterbank.Set(thrusterbank_h()));
	
	thruster_bank* bank = &pm->thrusters[idx];
	
	if (ADE_SETTING_VAR && trh != NULL)
	{
		if (newThr.isValid())
		{
			pm->thrusters[idx] = *(newThr.Get());
		}
	}

	return ade_set_args(L, "o", l_Thrusterbank.Set(bank));
}

ADE_FUNC(isValid, l_Thrusters, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	thrusters_h *trh;
	if(!ade_get_args(L, "o", l_Thrusters.GetPtr(&trh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", trh->IsValid());
}

//**********HANDLE: thrusterbank
ADE_FUNC(__len, l_Thrusterbank, NULL, "Number of thrusters on this thrusterbank", "number", "Number of thrusters on this bank or 0 if handle is invalid")
{
	thrusterbank_h *tbh = NULL;
	if(!ade_get_args(L, "o", l_Thrusterbank.GetPtr(&tbh)))
		return ade_set_error(L, "i", -1);

	if(!tbh->isValid())
		return ade_set_error(L, "i", -1);
	
	thruster_bank* bank = tbh->Get();

	return ade_set_args(L, "i", bank->num_points);
}

ADE_INDEXER(l_Thrusterbank, "number Index", "Array of glowpoint", "glowpoint", "Glowpoint, or invalid glowpoint handle on failure")
{
	thrusterbank_h *tbh = NULL;
	char *s = NULL;
	glowpoint_h *glh = NULL;

	if (!ade_get_args(L, "os|o", l_Thrusterbank.GetPtr(&tbh), &s, l_Glowpoint.GetPtr(&glh)))
		return ade_set_error(L, "o", l_Glowpoint.Set(glowpoint_h()));

	if (!tbh->isValid() || s==NULL)
		return ade_set_error(L, "o", l_Glowpoint.Set(glowpoint_h()));
	
	thruster_bank* bank = tbh->Get();

	//Determine index
	int idx = atoi(s) - 1;	//Lua->FS2

	if (idx < 0 || idx >= bank->num_points)
		return ade_set_error(L, "o", l_Glowpoint.Set(glowpoint_h()));
	
	glow_point* glp = &bank->points[idx];
	
	if (ADE_SETTING_VAR && glh != NULL)
	{
		if (glh->isValid())
		{
			bank->points[idx] = *(glh->Get());
		}
	}

	return ade_set_args(L, "o", l_Glowpoint.Set(glp));
}

ADE_FUNC(isValid, l_Thrusterbank, NULL, "Detectes if this handle is valid", "boolean", "true if this handle is valid, false otherwise")
{
	thrusterbank_h* trh;
	if(!ade_get_args(L, "o", l_Thrusterbank.GetPtr(&trh)))
		return ADE_RETURN_FALSE;

	if (!trh->isValid())
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", trh->isValid());
}

//**********HANDLE: glowpoint
ADE_VIRTVAR(Position, l_Glowpoint, NULL, "The (local) vector to the position of the glowpoint", "vector", "The local vector to the glowpoint or nil invalid")
{
	glowpoint_h *glh = NULL;
	vec3d newVec;

	if(!ade_get_args(L, "o|o", l_Glowpoint.GetPtr(&glh), l_Vector.Get(&newVec)))
		return ADE_RETURN_NIL;

	if (!glh->isValid())
		return ADE_RETURN_NIL;
	
	vec3d vec = glh->point->pnt;

	if (ADE_SETTING_VAR)
	{
		glh->point->pnt = newVec;
	}

	return ade_set_args(L, "o", l_Vector.Set(vec));
}

ADE_VIRTVAR(Radius, l_Glowpoint, NULL, "The radius of the glowpoint", "number", "The radius of the glowpoint or -1 of invalid")
{
	glowpoint_h* glh = NULL;
	float newVal;

	if(!ade_get_args(L, "o|f", l_Glowpoint.GetPtr(&glh), &newVal))
		return ade_set_error(L, "f", -1.0f);

	if (!glh->isValid())
		return ade_set_error(L, "f", -1.0f);

	float radius = glh->point->radius;

	if (ADE_SETTING_VAR)
	{
		glh->point->radius = newVal;
	}

	return ade_set_args(L, "f", radius);
}

ADE_FUNC(isValid, l_Glowpoint, NULL, "Returns wether this handle is valid or not", "boolean", "True if handle is valid, false otherwise")
{
	glowpoint_h glh = NULL;

	if(!ade_get_args(L, "o", l_Glowpoint.Get(&glh)))
		return ADE_RETURN_FALSE;
	
	return ade_set_args(L, "b", glh.isValid());
}

class dockingbay_h : public model_h
{
private:
	int dock_id;

public:
	dockingbay_h(polymodel *pm, int dock_idx) : model_h(pm), dock_id(dock_idx) {}
	dockingbay_h() : model_h(), dock_id(-1){}

	bool IsValid()
	{
		if (!model_h::IsValid())
		{
			return false;
		}
		else
		{
			return dock_id >= 0 && dock_id < this->Get()->n_docks;
		}
	}

	dock_bay* getDockingBay()
	{
		if (!this->IsValid())
		{
			return NULL;
		}

		return &this->Get()->docking_bays[dock_id];
	}
};
ade_obj<dockingbay_h> l_Dockingbay("dockingbay", "Handle to a model docking bay");

//**********HANDLE: dockingbays
ADE_INDEXER(l_Dockingbays, "dockingbay", "Gets a dockingbay handle from this model. If a string is given then a dockingbay with that name is searched.", "dockingbay", "Handle or invalid handle on error")
{
	dockingbays_h *dbhp = NULL;
	int index = -1;
	dockingbay_h *newVal = NULL;

	if (lua_isnumber(L, 2))
	{
		if (!ade_get_args(L, "oi|o", l_Dockingbays.GetPtr(&dbhp), &index, l_Dockingbay.GetPtr(&newVal)))
			return ade_set_error(L, "o", l_Dockingbay.Set(dockingbay_h()));

		if (!dbhp->IsValid())
			return ade_set_error(L, "o", l_Dockingbay.Set(dockingbay_h()));

		index--; // Lua --> C/C++
	}
	else
	{
		char* name = NULL;

		if (!ade_get_args(L, "os|o", l_Dockingbays.GetPtr(&dbhp), &name, l_Dockingbay.GetPtr(&newVal)))
		{
			return ade_set_error(L, "o", l_Dockingbay.Set(dockingbay_h()));
		}

		if (!dbhp->IsValid() && name != NULL)
			return ade_set_error(L, "o", l_Dockingbay.Set(dockingbay_h()));
		
		index = model_find_dock_name_index(dbhp->GetID(), name);
	}

	polymodel *pm = dbhp->Get();

	if (index < 0 || index >= pm->n_docks)
	{
		return ade_set_error(L, "o", l_Dockingbay.Set(dockingbay_h()));
	}

	return ade_set_args(L, "o", l_Dockingbay.Set(dockingbay_h(pm, index)));
}

ADE_FUNC(__len, l_Dockingbays, NULL, "Retrieves the number of dockingbays on this model", "number", "number of docking bays or 0 on error")
{
	dockingbays_h *dbhp = NULL;

	if (!ade_get_args(L, "o", l_Dockingbays.GetPtr(&dbhp)))
		return ade_set_error(L, "i", 0);

	if (!dbhp->IsValid())
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", dbhp->Get()->n_docks);
}

//**********HANDLE: dockingbay
ADE_FUNC(__len, l_Dockingbay, NULL, "Gets the number of docking points in this bay", "number", "The number of docking points or 0 on error")
{
	dockingbay_h* dbh = NULL;

	if (!ade_get_args(L, "o", l_Dockingbay.GetPtr(&dbh)))
	{
		return ade_set_error(L, "i", 0);
	}

	if (dbh == NULL || !dbh->IsValid())
	{
		return ade_set_error(L, "i", 0);
	}

	return ade_set_args(L, "i", dbh->getDockingBay()->num_slots);
}

ADE_FUNC(getName, l_Dockingbay, NULL, "Gets the name of this docking bay", "string", "The name or an empty string on error")
{
	dockingbay_h* dbh = NULL;
	if (!ade_get_args(L, "o", l_Dockingbay.GetPtr(&dbh)))
	{
		return ade_set_error(L, "s", "");
	}

	if (dbh == NULL || !dbh->IsValid())
	{
		return ade_set_error(L, "s", "");
	}

	dock_bay* dbp = dbh->getDockingBay();

	return ade_set_args(L, "s", dbp->name);
}

ADE_FUNC(getPoint, l_Dockingbay, "number index", "Gets the location of a docking point in this bay", "vector", "The local location or null vector on error")
{
	dockingbay_h* dbh = NULL;
	int index = -1;

	if (!ade_get_args(L, "oi", l_Dockingbay.GetPtr(&dbh), &index))
	{
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	}

	index--; // Lua --> C/C++

	if (dbh == NULL || !dbh->IsValid())
	{
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	}

	dock_bay* dbp = dbh->getDockingBay();

	if (index < 0 || index > dbp->num_slots)
	{
		LuaError(L, "Invalid dock bay index %d!", (index + 1));
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	}

	return ade_set_args(L, "o", l_Vector.Set(dbp->pnt[index]));
}

ADE_FUNC(getNormal, l_Dockingbay, "number index", "Gets the normal of a docking point in this bay", "vector", "The normal vector or null vector on error")
{
	dockingbay_h* dbh = NULL;
	int index = -1;

	if (!ade_get_args(L, "oi", l_Dockingbay.GetPtr(&dbh), &index))
	{
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	}

	index--; // Lua --> C/C++

	if (dbh == NULL || !dbh->IsValid())
	{
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	}

	dock_bay* dbp = dbh->getDockingBay();

	if (index < 0 || index > dbp->num_slots)
	{
		LuaError(L, "Invalid dock bay index %d!", (index + 1));
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	}

	return ade_set_args(L, "o", l_Vector.Set(dbp->norm[index]));
}

ADE_FUNC(computeDocker, l_Dockingbay, "dockingbay", "Computes the final position and orientation of a docker bay that docks with this bay.", "vector, orientation", "The local location and orientation of the docker vessel in the reference to the vessel of the docking bay handle, or a nil value on error")
{
	dockingbay_h *dockee_bay_h = NULL, *docker_bay_h = NULL;
	vec3d final_pos;
	matrix final_orient;

	if (!ade_get_args(L, "oo", l_Dockingbay.GetPtr(&dockee_bay_h), l_Dockingbay.GetPtr(&docker_bay_h)))
	{
		return ADE_RETURN_NIL;
	}

	if (!dockee_bay_h->IsValid() || !docker_bay_h->IsValid())
	{
		return ADE_RETURN_NIL;
	}

	dock_bay* dockee_bay = dockee_bay_h->getDockingBay();
	dock_bay* docker_bay = docker_bay_h->getDockingBay();

	// Mostly the same as aicode.cpp: dock_orient_and_approach
	vec3d dockee_dock_pos, docker_dock_pos_local, docker_dock_pos;
	vec3d dock_up_dir;

	// Get the center between the two docking points
	vm_vec_avg(&dockee_dock_pos, &dockee_bay->pnt[0], &dockee_bay->pnt[1]);
	vm_vec_avg(&docker_dock_pos_local, &docker_bay->pnt[0], &docker_bay->pnt[1]);

	// Get the up-vector of the docking bay
	vm_vec_sub(&dock_up_dir, &dockee_bay->pnt[1], &dockee_bay->pnt[0]);

	// Compute the orientation
	vm_vector_2_matrix(&final_orient, &dockee_bay->norm[0], &dock_up_dir, NULL);

	// Rotate the docker position into the right orientation
	vm_vec_unrotate(&docker_dock_pos, &docker_dock_pos_local, &final_orient);

	// The docker vector points into the wrong direction, we need to scale it appropriately
	vm_vec_scale(&docker_dock_pos, -1.0f);

	// Now get the position of the other vessel
	vm_vec_add(&final_pos, &dockee_dock_pos, &docker_dock_pos);

	return ade_set_args(L, "oo", l_Vector.Set(final_pos),l_Matrix.Set(matrix_h(&final_orient)));
}

ADE_FUNC(isValid, l_Dockingbay, NULL, "Detects whether is valid or not", "number", "<i>true</i> if valid, <i>false</i> otherwise")
{
	dockingbay_h* dbh = NULL;

	if (!ade_get_args(L, "o", l_Dockingbay.GetPtr(&dbh)))
	{
		return ade_set_error(L, "i", 0);
	}

	if (dbh == NULL)
	{
		return ADE_RETURN_FALSE;
	}
	
	return ade_set_args(L, "b", dbh->IsValid());
}

//**********HANDLE: physics
struct physics_info_h
{
	object_h objh;
	physics_info *pi;

	physics_info_h() {
		objh = object_h();
		pi = NULL;
	}

	physics_info_h(object *objp) {
		objh = object_h(objp);
		pi = &objp->phys_info;
	}

	physics_info_h(physics_info *in_pi) {
		pi = in_pi;
	}

	bool IsValid(){if(objh.objp != NULL) return objh.IsValid(); else return (pi != NULL);}
};

ade_obj<physics_info_h> l_Physics("physics", "Physics handle");

ADE_VIRTVAR(AfterburnerAccelerationTime, l_Physics, "number", "Afterburner acceleration time", "number", "Afterburner acceleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->afterburner_forward_accel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->afterburner_forward_accel_time_const);
}

ADE_VIRTVAR(AfterburnerVelocityMax, l_Physics, "vector", "Afterburner max velocity (Local vector)", "vector", "Afterburner max velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->afterburner_max_vel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->afterburner_max_vel));
}

ADE_VIRTVAR(BankingConstant, l_Physics, "number", "Banking constant", "number", "Banking constant, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->delta_bank_const = f;
	}

	return ade_set_args(L, "f", pih->pi->delta_bank_const);
}

ADE_VIRTVAR(ForwardAccelerationTime, l_Physics, "number", "Forward acceleration time", "number", "Forward acceleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->forward_accel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->forward_accel_time_const);
}

ADE_VIRTVAR(ForwardDecelerationTime, l_Physics, "number", "Forward deceleration time", "number", "Forward decleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->forward_decel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->forward_decel_time_const);
}

ADE_VIRTVAR(ForwardThrust, l_Physics, "number", "Forward thrust amount (0-1), used primarily for thruster graphics", "number", "Forward thrust, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->forward_thrust = f;
	}

	return ade_set_args(L, "f", pih->pi->forward_thrust);
}

ADE_VIRTVAR(Mass, l_Physics, "number", "Object mass", "number", "Object mass, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->mass = f;
	}

	return ade_set_args(L, "f", pih->pi->mass);
}

ADE_VIRTVAR(RotationalVelocity, l_Physics, "vector", "Rotational velocity (Local vector)", "vector", "Rotational velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->rotvel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->rotvel));
}

ADE_VIRTVAR(RotationalVelocityDamping, l_Physics, "number", "Rotational damping, ie derivative of rotational speed", "number", "Rotational damping, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->rotdamp = f;
	}

	return ade_set_args(L, "f", pih->pi->rotdamp);
}

ADE_VIRTVAR(RotationalVelocityDesired, l_Physics, "vector", "Desired rotational velocity", "vector", "Desired rotational velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->desired_rotvel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->desired_rotvel));
}

ADE_VIRTVAR(RotationalVelocityMax, l_Physics, "vector", "Maximum rotational velocity (Local vector)", "vector", "Maximum rotational velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->max_rotvel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->max_rotvel));
}

ADE_VIRTVAR(ShockwaveShakeAmplitude, l_Physics, "number", "How much shaking from shockwaves is applied to object", "number", "Shockwave shake amplitude, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->shockwave_shake_amp = f;
	}

	return ade_set_args(L, "f", pih->pi->shockwave_shake_amp);
}

ADE_VIRTVAR(SideThrust, l_Physics, "number", "Side thrust amount (0-1), used primarily for thruster graphics", "number", "Side thrust amount, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->side_thrust = f;
	}

	return ade_set_args(L, "f", pih->pi->side_thrust);
}

ADE_VIRTVAR(SlideAccelerationTime, l_Physics, "number", "Time to accelerate to maximum slide velocity", "number", "Sliding acceleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->slide_accel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->slide_accel_time_const);
}

ADE_VIRTVAR(SlideDecelerationTime, l_Physics, "number", "Time to decelerate from maximum slide speed", "number", "Sliding deceleration time, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->slide_decel_time_const = f;
	}

	return ade_set_args(L, "f", pih->pi->slide_decel_time_const);
}

ADE_VIRTVAR(Velocity, l_Physics, "vector", "Object world velocity (World vector)", "vector", "Object velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->vel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->vel));
}

ADE_VIRTVAR(VelocityDesired, l_Physics, "vector", "Desired velocity (World vector)", "vector", "Desired velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->desired_vel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->desired_vel));
}

ADE_VIRTVAR(VelocityMax, l_Physics, "vector", "Object max local velocity (Local vector)", "vector", "Maximum velocity, or null vector if handle is invalid")
{
	physics_info_h *pih;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!pih->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		pih->pi->max_vel = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(pih->pi->max_vel));
}

ADE_VIRTVAR(VerticalThrust, l_Physics, "number", "Vertical thrust amount (0-1), used primarily for thruster graphics", "number", "Vertical thrust amount, or 0 if handle is invalid")
{
	physics_info_h *pih;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Physics.GetPtr(&pih), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		pih->pi->vert_thrust = f;
	}

	return ade_set_args(L, "f", pih->pi->vert_thrust);
}

ADE_VIRTVAR(AfterburnerActive, l_Physics, "boolean", "Specifies if the afterburner is active or not", "boolean", "true if afterburner is active false otherwise")
{
	physics_info_h *pih;
	bool set = false;

	if(!ade_get_args(L, "o|b", l_Physics.GetPtr(&pih), &set))
		return ade_set_error(L, "b", false);

	if(!pih->IsValid())
		return ade_set_error(L, "b", false);
	
	if (ADE_SETTING_VAR)
	{
		if(set)
			pih->pi->flags |= PF_AFTERBURNER_ON;
		else
			pih->pi->flags &= ~PF_AFTERBURNER_ON;
	}

	if (pih->pi->flags & PF_AFTERBURNER_ON)
		return ade_set_args(L, "b",  true);
	else
		return ade_set_args(L, "b",  false);
}

ADE_FUNC(isValid, l_Physics, NULL, "True if valid, false or nil if not", "boolean", "Detects whether handle is valid")
{
	physics_info_h *pih;
	if(!ade_get_args(L, "o", l_Physics.GetPtr(&pih)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", pih->IsValid());
}

ADE_FUNC(getSpeed, l_Physics, NULL, "Gets total speed as of last frame", "number", "Total speed, or 0 if handle is invalid")
{
	physics_info_h *pih;
	if(!ade_get_args(L, "o", l_Physics.GetPtr(&pih)))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", pih->pi->speed);
}

ADE_FUNC(getForwardSpeed, l_Physics, NULL, "Gets total speed in the ship's 'forward' direction as of last frame", "number", "Total forward speed, or 0 if handle is invalid")
{
	physics_info_h *pih;
	if(!ade_get_args(L, "o", l_Physics.GetPtr(&pih)))
		return ade_set_error(L, "f", 0.0f);

	if(!pih->IsValid())
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", pih->pi->fspeed);
}

// Nuke's afterburner function
ADE_FUNC(isAfterburnerActive, l_Physics, NULL, "True if Afterburners are on, false or nil if not", "boolean", "Detects whether afterburner is active")
{
	physics_info_h *pih;
	if(!ade_get_args(L, "o", l_Physics.GetPtr(&pih)))
		return ADE_RETURN_NIL;

	if(!pih->IsValid())
		return ade_set_error(L, "b", false);

	if (pih->pi->flags & PF_AFTERBURNER_ON)
		return ade_set_args(L, "b",  true);
	else
		return ade_set_args(L, "b",  false);
}

//nukes glide function
ADE_FUNC(isGliding, l_Physics, NULL, "True if glide mode is on, false or nil if not", "boolean", "Detects if ship is gliding")
{
	physics_info_h *pih;
	if(!ade_get_args(L, "o", l_Physics.GetPtr(&pih)))
		return ADE_RETURN_NIL;

	if(!pih->IsValid())
		return ade_set_error(L, "b", false);

	if (pih->pi->flags & (PF_GLIDING|PF_FORCE_GLIDE))
		return ade_set_args(L, "b",  true);
	else
		return ade_set_args(L, "b",  false);
}

ADE_FUNC(applyWhack, l_Physics, "vector Impulse, [ vector Position]", "Applies a whack to an object at a position (a local vector) based on impulse supplied (a world vector). If no position is supplied, an empty vector is used.", "boolean", "true if it succeeded, false otherwise")
 {
	object_h objh;
	physics_info_h *pih;
	vec3d *impulse;
	vec3d *offset = &vmd_zero_vector;
	
	if (!ade_get_args(L, "oo|o", l_Physics.GetPtr(&pih), l_Vector.GetPtr(&impulse), l_Vector.GetPtr(&offset)))
		return ADE_RETURN_NIL;
	
	objh = pih->objh;
	
	physics_apply_whack(impulse, offset, pih->pi, &objh.objp->orient, pih->pi->mass);
	
	return ADE_RETURN_TRUE;
	
}


//**********HANDLE: sexpvariable
struct sexpvar_h
{
	int idx;
	char variable_name[TOKEN_LENGTH];

	sexpvar_h(){idx=-1;variable_name[0]='\0';}
	sexpvar_h(int n_idx){idx = n_idx; strcpy_s(variable_name, Sexp_variables[n_idx].variable_name);}
	bool IsValid(){
		return (idx > -1
				&& idx < MAX_SEXP_VARIABLES
				&& (Sexp_variables[idx].type & SEXP_VARIABLE_SET)
				&& !strcmp(Sexp_variables[idx].variable_name, variable_name));}
};

ade_obj<sexpvar_h> l_SEXPVariable("sexpvariable", "SEXP Variable handle");

ADE_VIRTVAR(Persistence, l_SEXPVariable, "enumeration", "SEXP Variable persistance, uses SEXPVAR_*_PERSISTENT enumerations", "enumeration", "SEXPVAR_*_PERSISTENT enumeration, or invalid numeration if handle is invalid")
{
	sexpvar_h *svh = NULL;
	enum_h *type = NULL;
	if(!ade_get_args(L, "o|o", l_SEXPVariable.GetPtr(&svh), l_Enum.GetPtr(&type)))
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	if(!svh->IsValid())
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	sexp_variable *sv = &Sexp_variables[svh->idx];

	if(ADE_SETTING_VAR && type != NULL)
	{
		if(type->index == LE_SEXPVAR_PLAYER_PERSISTENT)
		{
			sv->type &= ~(SEXP_VARIABLE_CAMPAIGN_PERSISTENT);
			sv->type |= SEXP_VARIABLE_PLAYER_PERSISTENT;
		}
		else if(type->index == LE_SEXPVAR_CAMPAIGN_PERSISTENT)
		{
			sv->type |= SEXP_VARIABLE_CAMPAIGN_PERSISTENT;
			sv->type &= ~(SEXP_VARIABLE_PLAYER_PERSISTENT);
		}
		else if(type->index == LE_SEXPVAR_NOT_PERSISTENT)
		{
			sv->type &= ~(SEXP_VARIABLE_CAMPAIGN_PERSISTENT);
			sv->type &= ~(SEXP_VARIABLE_PLAYER_PERSISTENT);
		}
	}

	enum_h ren;
	if(sv->type & SEXP_VARIABLE_PLAYER_PERSISTENT)
		ren.index = LE_SEXPVAR_PLAYER_PERSISTENT;
	else if(sv->type & SEXP_VARIABLE_CAMPAIGN_PERSISTENT)
		ren.index = LE_SEXPVAR_CAMPAIGN_PERSISTENT;
	else
		ren.index = LE_SEXPVAR_NOT_PERSISTENT;

	return ade_set_args(L, "o", l_Enum.Set(ren));
}

ADE_VIRTVAR(Type, l_SEXPVariable, "enumeration", "SEXP Variable type, uses SEXPVAR_TYPE_* enumerations", "enumeration", "SEXPVAR_TYPE_* enumeration, or invalid numeration if handle is invalid")
{
	sexpvar_h *svh = NULL;
	enum_h *type = NULL;
	if(!ade_get_args(L, "o|o", l_SEXPVariable.GetPtr(&svh), l_Enum.GetPtr(&type)))
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	if(!svh->IsValid())
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	sexp_variable *sv = &Sexp_variables[svh->idx];

	if(ADE_SETTING_VAR && type != NULL)
	{
		if(type->index == LE_SEXPVAR_TYPE_NUMBER)
		{
			sv->type &= ~(SEXP_VARIABLE_NUMBER);
			sv->type |= SEXP_VARIABLE_STRING;
		}
		else if(type->index == LE_SEXPVAR_TYPE_STRING)
		{
			sv->type |= SEXP_VARIABLE_NUMBER;
			sv->type &= ~(SEXP_VARIABLE_STRING);
		}
	}

	enum_h ren;
	if(sv->type & SEXP_VARIABLE_NUMBER)
		ren.index = LE_SEXPVAR_TYPE_NUMBER;
	else if(sv->type & SEXP_VARIABLE_STRING)
		ren.index = LE_SEXPVAR_TYPE_STRING;

	return ade_set_args(L, "o", l_Enum.Set(ren));
}

ADE_VIRTVAR(Value, l_SEXPVariable, "number/string", "SEXP variable value", "string", "SEXP variable contents, or nil if the variable is of an invalid type or the handle is invalid")
{
	sexpvar_h *svh = NULL;
	char *newvalue = NULL;
	char number_as_str[TOKEN_LENGTH];

	if(lua_type(L, 2) == LUA_TNUMBER)
	{
		int newnumber = 0;
		if(!ade_get_args(L, "o|i", l_SEXPVariable.GetPtr(&svh), &newnumber))
			return ADE_RETURN_NIL;

		sprintf(number_as_str, "%d", newnumber);
		newvalue = number_as_str;
	}
	else
	{
		if(!ade_get_args(L, "o|s", l_SEXPVariable.GetPtr(&svh), &newvalue))
			return ADE_RETURN_NIL;
	}

	if(!svh->IsValid())
		return ADE_RETURN_NIL;

	sexp_variable *sv = &Sexp_variables[svh->idx];

	if(ADE_SETTING_VAR && newvalue)
	{
		sexp_modify_variable(newvalue, svh->idx, false);
	}

	if(sv->type & SEXP_VARIABLE_NUMBER)
		return ade_set_args(L, "i", atoi(sv->text));
	else if(sv->type & SEXP_VARIABLE_STRING)
		return ade_set_args(L, "s", sv->text);
	else
		return ADE_RETURN_NIL;
}

ADE_FUNC(__tostring, l_SEXPVariable, NULL, "Returns SEXP name", "string", "SEXP name, or empty string if handle is invalid")
{
	sexpvar_h *svh = NULL;
	if(!ade_get_args(L, "o", l_SEXPVariable.GetPtr(&svh)))
		return ade_set_error(L, "s", "");

	if(!svh->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Sexp_variables[svh->idx].variable_name);
}

ADE_FUNC(isValid, l_SEXPVariable, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	sexpvar_h *svh = NULL;
	if(!ade_get_args(L, "o", l_SEXPVariable.GetPtr(&svh)))
		return ADE_RETURN_NIL;

	if(!svh->IsValid())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(delete, l_SEXPVariable, NULL, "Deletes a SEXP Variable", "boolean", "True if successful, false if the handle is invalid")
{
	sexpvar_h *svh = NULL;
	if(!ade_get_args(L, "o", l_SEXPVariable.GetPtr(&svh)))
		return ade_set_error(L, "b", false);

	if(!svh->IsValid())
		return ade_set_error(L, "b", false);

	sexp_variable_delete(svh->idx);

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Shields
ade_obj<object_h> l_Shields("shields", "Shields handle");

ADE_FUNC(__len, l_Shields, NULL, "Number of shield segments", "number", "Number of shield segments or 0 if handle is invalid")
{
	object_h *objh;

	if(!ade_get_args(L, "o", l_Shields.GetPtr(&objh)))
		return ade_set_error(L, "i", -1);

	if(!objh->IsValid())
		return ade_set_error(L, "i", -1);

	return ade_set_args(L, "i", objh->objp->n_quadrants);
}

ADE_INDEXER(l_Shields, "enumeration/number", "Gets or sets shield segment strength. Use \"SHIELD_*\" enumerations (for standard 4-quadrant shields) or index of a specific segment, or NONE for the entire shield", "number", "Segment/shield strength, or 0 if handle is invalid")
{
	object_h *objh;
	float nval = -1.0f;

	object *objp = NULL;
	int qdx = -1;

	if(lua_isstring(L, 2))
	{
		char *qd = NULL;
		if(!ade_get_args(L, "os|f", l_Shields.GetPtr(&objh), &qd, &nval))
			return ade_set_error(L, "f", 0.0f);

		if(!objh->IsValid())
			return ade_set_error(L, "f", 0.0f);

		objp = objh->objp;

		//Which quadrant?
		int qdi;
		if(qd == NULL)
			qdx = -1;
		else if((qdi = atoi(qd)) > 0 && qdi <= objp->n_quadrants)
			qdx = qdi-1;	//LUA->FS2
		else
			return ade_set_error(L, "f", 0.0f);
	} else {
		enum_h *qd = NULL;
		if(!ade_get_args(L, "oo|f", l_Shields.GetPtr(&objh), l_Enum.GetPtr(&qd), &nval))
			return 0;

		if(!objh->IsValid())
			return ade_set_error(L, "f", 0.0f);

		objp = objh->objp;

		switch(qd->index)
		{
			case LE_NONE:
				qdx = -1;
				break;
			case LE_SHIELD_FRONT:
				qdx = FRONT_QUAD;
				break;
			case LE_SHIELD_LEFT:
				qdx = LEFT_QUAD;
				break;
			case LE_SHIELD_RIGHT:
				qdx = RIGHT_QUAD;
				break;
			case LE_SHIELD_BACK:
				qdx = REAR_QUAD;
				break;
			default:
				return ade_set_error(L, "f", 0.0f);
		}
	}

	//Set/get all quadrants
	if(qdx == -1) {
		if(ADE_SETTING_VAR && nval >= 0.0f)
			shield_set_strength(objp, nval);

		return ade_set_args(L, "f", shield_get_strength(objp));
	}

	//Set one quadrant?
	if(ADE_SETTING_VAR && nval >= 0.0f)
		shield_set_quad(objp, qdx, nval);

	//Get one quadrant
	return ade_set_args(L, "f", shield_get_quad(objp, qdx));
}

//WMC - Not sure if I want this to be a variable. It'd make more sense
//as a function, since it modifies all quadrant variables
//WMC - Ehh, screw it.
ADE_VIRTVAR(CombinedLeft, l_Shields, "number", "Total shield hitpoints left (for all segments combined)", "number", "Combined shield strength, or 0 if handle is invalid")
{
	object_h *objh;
	float nval = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shields.GetPtr(&objh), &nval))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && nval >= 0.0f) {
		shield_set_strength(objh->objp, nval);
	}

	return ade_set_args(L, "f", shield_get_strength(objh->objp));
}

ADE_VIRTVAR(CombinedMax, l_Shields, "number", "Maximum shield hitpoints (for all segments combined)", "number", "Combined maximum shield strength, or 0 if handle is invalid")
{
	object_h *objh;
	float nval = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shields.GetPtr(&objh), &nval))
			return 0;

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && nval >= 0.0f) {
		shield_set_max_strength(objh->objp, nval);
	}

	return ade_set_args(L, "f", shield_get_max_strength(objh->objp));
}

ADE_FUNC(isValid, l_Shields, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_Shields.GetPtr(&oh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", oh->IsValid());
}

//**********HANDLE: Shiptype
ade_obj<int> l_Shiptype("shiptype", "Ship type handle");
extern int Species_initted;

ADE_VIRTVAR(Name, l_Shiptype, "string", "Ship type name", "string", "Ship type name, or empty string if handle is invalid")
{
	if(!Species_initted)
		return ade_set_error(L, "s", "");

	char *s = NULL;
	int idx;
	if(!ade_get_args(L, "o|s", l_Shiptype.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= (int)Ship_types.size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Ship_types[idx].name, s, sizeof(Ship_types[idx].name)-1);
	}

	return ade_set_args(L, "s", Ship_types[idx].name);
}

ADE_FUNC(isValid, l_Shiptype, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shiptype.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= (int)Ship_types.size())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Species
ade_obj<int> l_Species("species", "Species handle");
extern int Species_initted;

ADE_VIRTVAR(Name, l_Species, "string", "Species name", "string", "Species name, or empty string if handle is invalid")
{
	if(!Species_initted)
		return ade_set_error(L, "s", "");

	char *s = NULL;
	int idx;
	if(!ade_get_args(L, "o|s", l_Species.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= (int)Species_info.size())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Species_info[idx].species_name, s, sizeof(Species_info[idx].species_name)-1);
	}

	return ade_set_args(L, "s", Species_info[idx].species_name);
}

ADE_FUNC(isValid, l_Species, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Species.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= (int)Species_info.size())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Team
ade_obj<int> l_Team("team", "Team handle");

ADE_FUNC(__eq, l_Team, "team, team", "Checks whether two teams are the same team", "boolean", "true if equal")
{
	int t1, t2;
	if(!ade_get_args(L, "oo", l_Team.Get(&t1), l_Team.Get(&t2)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", (t1 == t2));
}

ADE_VIRTVAR(Name, l_Team, "string", "Team name", "string", "Team name, or empty string if handle is invalid")
{
	int tdx=-1;
	char *s=NULL;
	if(!ade_get_args(L, "o|s", l_Team.Get(&tdx), &s))
		return ade_set_error(L, "s", "");

	if(tdx < 0 || tdx > Num_iffs)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Iff_info[tdx].iff_name, s, NAME_LENGTH-1);
	}

	return ade_set_args(L, "s", Iff_info[tdx].iff_name);
}

ADE_FUNC(getColor, l_Team, NULL, "Gets the IFF color of the specified Team", "number, number, number", "rgb color for the specified team or nil if invalid") {
	int idx;
	int r,g,b;
	if(!ade_get_args(L, "o", l_Team.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_iffs)
		return ADE_RETURN_NIL;

	color* col = iff_get_color_by_team(idx, 0, 0);

	r = col->red;
	g = col->green;
	b = col->blue;

	return ade_set_args(L, "iii", r, g, b);
}

ADE_FUNC(isValid, l_Team, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Team.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_iffs)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(attacks, l_Team, "team", "Checks the IFF status of another team", "boolean", "True if this team attacks the specified team")
{
	int x, y;
	ade_get_args(L, "oo", l_Team.Get(&x), l_Team.Get(&y));
	if (iff_x_attacks_y(x, y) > 0)
		return ADE_RETURN_TRUE;
	
	return ADE_RETURN_FALSE;
}

//**********HANDLE: streamingAnimation

class streaminganim_h {
public:
	generic_anim ga;

	bool IsValid() {
		return (ga.num_frames > 0);
	}
	streaminganim_h (const char* filename)
	{
		generic_anim_init(&ga, filename);
	}
};

ade_obj<streaminganim_h> l_streaminganim("streaminganim", "Streaming Animation handle");

ADE_FUNC(__gc, l_streaminganim, NULL, "Auto-deletes streaming animation", NULL, NULL)
{
	// NOTE: very similar to the l_streaminganim ADE_FUNC unload
	streaminganim_h* sah;

	if(!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	// don't bother to check if valid before unloading
	// generic_anim_unload has safety checks
	generic_anim_unload(&sah->ga);

	return ADE_RETURN_NIL;
}

ADE_VIRTVAR(Loop, l_streaminganim, "[boolean loop]", "Make the streaming animation loop.", "boolean", "Is the animation looping, or nil if anim invalid")
{
	streaminganim_h* sah;
	bool loop = false;

	if(!ade_get_args(L, "o|b", l_streaminganim.GetPtr(&sah), &loop))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR) {
		if (loop == false)
			sah->ga.direction |= GENERIC_ANIM_DIRECTION_NOLOOP;
		else
			sah->ga.direction &= ~GENERIC_ANIM_DIRECTION_NOLOOP;
	}

	return ((sah->ga.direction & GENERIC_ANIM_DIRECTION_NOLOOP) == false ? ADE_RETURN_TRUE : ADE_RETURN_FALSE);
}

ADE_VIRTVAR(Pause, l_streaminganim, "[boolean pause]", "Pause the streaming animation.", "boolean", "Is the animation paused, or nil if anim invalid")
{
	streaminganim_h* sah;
	bool pause = false;

	if(!ade_get_args(L, "o|b", l_streaminganim.GetPtr(&sah), &pause))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR) {
		if (pause == true)
			sah->ga.direction |= GENERIC_ANIM_DIRECTION_PAUSED;
		else
			sah->ga.direction &= ~GENERIC_ANIM_DIRECTION_PAUSED;
	}

	return ((sah->ga.direction & GENERIC_ANIM_DIRECTION_PAUSED) ? ADE_RETURN_TRUE : ADE_RETURN_FALSE);
}

ADE_VIRTVAR(Reverse, l_streaminganim, "[boolean reverse]", "Make the streaming animation play in reverse.", "boolean", "Is the animation playing in reverse, or nil if anim invalid")
{
	streaminganim_h* sah;
	bool reverse = false;

	if(!ade_get_args(L, "o|b", l_streaminganim.GetPtr(&sah), &reverse))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR) {
		if (reverse == true)
			sah->ga.direction |= GENERIC_ANIM_DIRECTION_BACKWARDS;
		else
			sah->ga.direction &= ~GENERIC_ANIM_DIRECTION_BACKWARDS;
	}

	return ((sah->ga.direction & GENERIC_ANIM_DIRECTION_BACKWARDS) ? ADE_RETURN_TRUE : ADE_RETURN_FALSE);
}

ADE_FUNC(getFilename, l_streaminganim, NULL, "Get the filename of the animation", "string", "Filename or nil if invalid")
{
	streaminganim_h* sah;

	if (!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	return ade_set_args(L, "s", sah->ga.filename);
}

ADE_FUNC(getFrameCount, l_streaminganim, NULL, "Get the number of frames in the animation.", "integer", "Total number of frames")
{
	streaminganim_h* sah;

	if (!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	return ade_set_args(L, "i", sah->ga.num_frames);
}

ADE_FUNC(getFrameIndex, l_streaminganim, NULL, "Get the current frame index of the animation", "integer", "Current frame index")
{
	streaminganim_h* sah;

	if (!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	int cframe = sah->ga.current_frame;
	if (cframe < 0 || cframe >= sah->ga.num_frames) {
		return ADE_RETURN_NIL;
	}

	return ade_set_args(L, "i", ++cframe); // C++ to LUA array index
}

ADE_FUNC(getHeight, l_streaminganim, NULL, "Get the height of the animation in pixels", "integer", "Height or nil if invalid")
{
	streaminganim_h* sah;

	if (!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	return ade_set_args(L, "i", sah->ga.height);
}

ADE_FUNC(getWidth, l_streaminganim, NULL, "Get the width of the animation in pixels", "integer", "Width or nil if invalid")
{
	streaminganim_h* sah;

	if (!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	return ade_set_args(L, "i", sah->ga.width);
}

ADE_FUNC(isValid, l_streaminganim, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	streaminganim_h* sah;
	if(!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	return sah->IsValid();
}

ADE_FUNC(preload, l_streaminganim, NULL, "Load all apng animations into memory, enabling apng frame cache if not already enabled", "boolean", "true if preload was successful, nil if a syntax/type error occurs")
{
	streaminganim_h* sah;
	if(!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	if (sah->ga.type != BM_TYPE_PNG)
		return ADE_RETURN_NIL;

	sah->ga.png.anim->preload();

	return ADE_RETURN_TRUE;
}

ADE_FUNC(process, l_streaminganim, "[int x1, int y1, int x2, int y2, float u0, float v0, float u1, float v1, float alpha, boolean draw]",
		"Processes a streaming animation, including selecting the correct frame & drawing it.",
		"boolean", "True if processing was successful, otherwise nil")
{
	streaminganim_h* sah;
	generic_extras ge;
	int x1 = 0, y1 = 0;
	int x2 = INT_MAX, y2 = INT_MAX;

	if(!ade_get_args(L, "o|iiiifffffb", l_streaminganim.GetPtr(&sah),
				&x1, &y1, &x2, &y2,
				&ge.u0, &ge.v0, &ge.u1, &ge.v1, &ge.alpha,
				&ge.draw))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	ge.width = sah->ga.width;
	ge.height = sah->ga.height;
	if (x2 != INT_MAX) ge.width = x2-x1;
	if (y2 != INT_MAX) ge.height = y2-y1;

	// note; generic_anim_render will default to GR_RESIZE_NONE when ge is provided
	generic_anim_render(&sah->ga, flFrametime, x1, y1, false, &ge);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(reset, l_streaminganim, "[none]", "Reset a streaming animation back to its 1st frame", "boolean", "True if successful, otherwise nil")
{
	streaminganim_h* sah;

	if(!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	sah->ga.png.anim->goto_start();
	sah->ga.current_frame = 0;
	sah->ga.anim_time = 0.0f;
	sah->ga.png.previous_frame_time = 0.0f;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(timeLeft, l_streaminganim, NULL, "Get the amount of time left in the animation, in seconds", "float", "Time left in secs or nil if invalid")
{
	streaminganim_h* sah;

	if (!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	if(!sah->IsValid())
		return ADE_RETURN_NIL;

	float timeLeft = 0.0f;
	if ((sah->ga.direction & GENERIC_ANIM_DIRECTION_BACKWARDS) == true)
		timeLeft = sah->ga.anim_time;
	else
		timeLeft = sah->ga.total_time - sah->ga.anim_time;

	return ade_set_args(L, "f", timeLeft);
}

ADE_FUNC(unload, l_streaminganim, NULL, "Unloads a streaming animation from memory", NULL, NULL)
{
	streaminganim_h* sah;

	if(!ade_get_args(L, "o", l_streaminganim.GetPtr(&sah)))
		return ADE_RETURN_NIL;

	// don't bother to check if valid before unloading
	// generic_anim_unload has safety checks
	generic_anim_unload(&sah->ga);

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Texture
ade_obj<int> l_Texture("texture", "Texture handle");
//WMC - int should NEVER EVER be an invalid handle. Return Nil instead. Nil FTW.

static float lua_Opacity = 1.0f;
static int lua_Opacity_type = GR_ALPHABLEND_FILTER;

ADE_FUNC(__gc, l_Texture, NULL, "Auto-deletes texture", NULL, NULL)
{
	int idx;

	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ADE_RETURN_NIL;

	// Note: due to some unknown reason, in some circumstances this function
	// might get called even for handles to bitmaps which are actually still in
	// use, and in order to prevent that we want to double-check the load count
	// here before unloading the bitmap. -zookeeper
	if(idx > -1 && bm_is_valid(idx) && bm_bitmaps[bm_get_cache_slot(idx, 0)].load_count < 1)
		bm_release(idx);

	return ADE_RETURN_NIL;
}

ADE_FUNC(__eq, l_Texture, "texture, texture", "Checks if two texture handles refer to the same texture", "boolean", "True if textures are equal")
{
	int idx,idx2;

	if(!ade_get_args(L, "oo", l_Texture.Get(&idx), l_Texture.Get(&idx2)))
		return ADE_RETURN_NIL;

	if(idx == idx2)
		return ADE_RETURN_TRUE;

	return ADE_RETURN_FALSE;
}

ADE_INDEXER(l_Texture, "number",
			"Returns texture handle to specified frame number in current texture's animation."
			"This means that [1] will always return the first frame in an animation, no matter what frame an animation is."
			"You cannot change a texture animation frame.",
			"texture",
			"Texture handle, or invalid texture handle if index is invalid")
{
	int idx;
	int frame=-1;
	int newframe=-1;	//WMC - Ignore for now
	if(!ade_get_args(L, "oi|i", l_Texture.Get(&idx), &frame, &newframe))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(frame < 1)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	//Get me some info
	int num=-1;
	int first=-1;
	first = bm_get_info(idx, NULL, NULL, NULL, &num);

	//Check it's a valid one
	if(first < 0 || frame > num)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	frame--; //Lua->FS2

	//Get actual texture handle
	frame = first + frame;

	return ade_set_args(L, "o", l_Texture.Set(frame));
}

ADE_FUNC(isValid, l_Texture, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", bm_is_valid(idx));
}

ADE_FUNC(unload, l_Texture, NULL, "Unloads a texture from memory", NULL, NULL)
{
	int *idx;

	if(!ade_get_args(L, "o", l_Texture.GetPtr(&idx)))
		return ADE_RETURN_NIL;

	if(!bm_is_valid(*idx))
		return ADE_RETURN_NIL;

	bm_release(*idx);

	//WMC - invalidate this handle
	*idx = -1;

	return ADE_RETURN_NIL;
}

ADE_FUNC(getFilename, l_Texture, NULL, "Returns filename for texture", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ade_set_error(L, "s", "");

	if(!bm_is_valid(idx))
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", bm_get_filename(idx));
}

ADE_FUNC(getWidth, l_Texture, NULL, "Gets texture width", "number", "Texture width, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ade_set_error(L, "i", 0);

	if(!bm_is_valid(idx))
		return ade_set_error(L, "i", 0);

	int w = -1;

	if(bm_get_info(idx, &w) < 0)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", w);
}

ADE_FUNC(getHeight, l_Texture, NULL, "Gets texture height", "number", "Texture height, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ade_set_error(L, "i", 0);

	if(!bm_is_valid(idx))
		return ade_set_error(L, "i", 0);

	int h=-1;

	if(bm_get_info(idx, NULL, &h) < 0)
		return ade_set_error(L, "i", 0);
	
	return ade_set_args(L, "i", h);
}

ADE_FUNC(getFPS, l_Texture, NULL,"Gets frames-per-second of texture", "number", "Texture FPS, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ade_set_error(L, "i", 0);

	if(!bm_is_valid(idx))
		return ade_set_error(L, "i", 0);

	int fps=-1;

	if(bm_get_info(idx, NULL, NULL, NULL, NULL, &fps) < 0)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", fps);
}

ADE_FUNC(getFramesLeft, l_Texture, NULL, "Gets number of frames left, from handle's position in animation", "number", "Frames left, or 0 if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Texture.Get(&idx)))
		return ADE_RETURN_NIL;

	if(!bm_is_valid(idx))
		return ADE_RETURN_NIL;

	int num=-1;

	if(bm_get_info(idx, NULL, NULL, NULL, &num) < 0)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", num);
}

ADE_FUNC(getFrame, l_Texture, "number Elapsed time (secs), [boolean Loop]",
		 "Get the frame number from the elapsed time of the animation<br>"
		 "The 1st argument is the time that has elapsed since the animation started<br>"
		 "If 2nd argument is set to true, the animation is expected to loop when the elapsed time exceeds the duration of a single playback",
		 "integer",
		 "Frame number")
{
	int idx, frame = 0;
	float elapsed_time;
	bool loop = false;

	if (!ade_get_args(L, "of|b", l_Texture.Get(&idx), &elapsed_time, &loop))
		return ADE_RETURN_NIL;

	if (!bm_is_valid(idx))
		return ADE_RETURN_NIL;

	frame = bm_get_anim_frame(idx, elapsed_time, 0.0f, loop);
	frame++;  // C++ to LUA array idx

	return ade_set_args(L, "i", frame);
}

//**********HANDLE: material
static const int THT_INDEPENDENT	= 0;
static const int THT_OBJECT			= 1;
static const int THT_MODEL			= 2;
class texture_map_h
{
protected:
	int type;
	object_h obj;
	model_h mdl;

	texture_map *tmap;	//Pointer to subsystem, or NULL for the hull

public:
	texture_map_h(){
		type = THT_INDEPENDENT;
		tmap = NULL;
	}

	texture_map_h(object *objp, texture_map *n_tmap = NULL) {
		type = THT_OBJECT;
		obj = object_h(objp);
		tmap = n_tmap;
	}

	texture_map_h(int modelnum, texture_map *n_tmap = NULL) {
		type = THT_MODEL;
		mdl = model_h(modelnum);
		tmap = n_tmap;
	}

	texture_map_h(polymodel *n_model, texture_map *n_tmap = NULL) {
		type = THT_MODEL;
		mdl = model_h(n_model);
		tmap = n_tmap;
	}

	texture_map *Get()
	{
		if(!this->IsValid())
			return NULL;

		return tmap;
	}

	int GetSize()
	{
		if(!this->IsValid())
			return 0;

		switch(type)
		{
			case THT_MODEL:
				return mdl.Get()->n_textures;
			case THT_OBJECT:
				return 0;	//Can't do this right now.
			default:
				return 0;
		}
	}

	bool IsValid() {
		if(tmap == NULL)
			return false;

		switch(type)
		{
			case THT_INDEPENDENT:
				return true;
			case THT_OBJECT:
				return obj.IsValid();
			case THT_MODEL:
				return mdl.IsValid();
			default:
				Error(LOCATION, "Bad type in texture_map_h; debug this.");
				return false;
		}
	}
};
ade_obj<texture_map_h> l_TextureMap("material", "Texture map, including diffuse, glow, and specular textures");

ADE_VIRTVAR(BaseMap, l_TextureMap, "texture", "Base texture", "texture", "Base texture, or invalid texture handle if material handle is invalid")
{
	texture_map_h *tmh = NULL;
	int new_tex = -1;
	if(!ade_get_args(L, "o|o", l_TextureMap.GetPtr(&tmh), l_Texture.Get(&new_tex)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	texture_map *tmap = tmh->Get();
	if(tmap == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(ADE_SETTING_VAR && new_tex > -1) {
		tmap->textures[TM_BASE_TYPE].SetTexture(new_tex);
	}

	return ade_set_args(L, "o", l_Texture.Set(tmap->textures[TM_BASE_TYPE].GetTexture()));
}

ADE_VIRTVAR(GlowMap, l_TextureMap, "texture", "Glow texture", "texture", "Glow texture, or invalid texture handle if material handle is invalid")
{
	texture_map_h *tmh = NULL;
	int new_tex = -1;
	if(!ade_get_args(L, "o|o", l_TextureMap.GetPtr(&tmh), l_Texture.Get(&new_tex)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	texture_map *tmap = tmh->Get();
	if(tmap == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(ADE_SETTING_VAR && new_tex > -1) {
		tmap->textures[TM_GLOW_TYPE].SetTexture(new_tex);
	}

	return ade_set_args(L, "o", l_Texture.Set(tmap->textures[TM_GLOW_TYPE].GetTexture()));
}

ADE_VIRTVAR(SpecularMap, l_TextureMap, "texture", "Specular texture", "texture", "Texture handle, or invalid texture handle if material handle is invalid")
{
	texture_map_h *tmh = NULL;
	int new_tex = -1;
	if(!ade_get_args(L, "o|o", l_TextureMap.GetPtr(&tmh), l_Texture.Get(&new_tex)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	texture_map *tmap = tmh->Get();
	if(tmap == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(ADE_SETTING_VAR && new_tex > -1) {
		tmap->textures[TM_SPECULAR_TYPE].SetTexture(new_tex);
	}

	return ade_set_args(L, "o", l_Texture.Set(tmap->textures[TM_SPECULAR_TYPE].GetTexture()));
}

//**********HANDLE: Weaponclass
ade_obj<int> l_Weaponclass("weaponclass", "Weapon class handle");

ADE_FUNC(__tostring, l_Weaponclass, NULL, "Weapon class name", "string", "Weapon class name, or an empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Weapon_info[idx].name);
}

ADE_FUNC(__eq, l_Weaponclass, "weaponclass, weaponclass", "Checks if the two classes are equal", "boolean", "true if equal false otherwise")
{
	int idx1,idx2;
	if(!ade_get_args(L, "oo", l_Weaponclass.Get(&idx1), l_Weaponclass.Get(&idx2)))
		return ade_set_error(L, "b", false);

	if(idx1 < 0 || idx1 >= Num_weapon_types)
		return ade_set_error(L, "b", false);

	if(idx2 < 0 || idx2 >= Num_weapon_types)
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", idx1 == idx2);
}

ADE_VIRTVAR(Name, l_Weaponclass, "string", "Weapon class name", "string", "Weapon class name, or empty string if handle is invalid")

{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Weapon_info[idx].name, s, sizeof(Weapon_info[idx].name)-1);
	}

	return ade_set_args(L, "s", Weapon_info[idx].name);
}

ADE_VIRTVAR(Title, l_Weaponclass, "string", "Weapon class title", "string", "Weapon class title, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Weapon_info[idx].title, s, sizeof(Weapon_info[idx].title)-1);
	}

	return ade_set_args(L, "s", Weapon_info[idx].title);
}

ADE_VIRTVAR(Description, l_Weaponclass, "string", "Weapon class description string", "string", "Description string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "s", "");

	weapon_info *wip = &Weapon_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(wip->desc);
		if(s != NULL) {
			wip->desc = (char*)vm_malloc(strlen(s)+1);
			strcpy(wip->desc, s);
		} else {
			wip->desc = NULL;
		}
	}

	if(wip->desc != NULL)
		return ade_set_args(L, "s", wip->desc);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(TechTitle, l_Weaponclass, "string", "Weapon class tech title", "string", "Tech title, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Weapon_info[idx].tech_title, s, sizeof(Weapon_info[idx].tech_title)-1);
	}

	return ade_set_args(L, "s", Weapon_info[idx].tech_title);
}

ADE_VIRTVAR(TechAnimationFilename, l_Weaponclass, "string", "Weapon class animation filename", "string", "Filename, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Weapon_info[idx].tech_anim_filename, s, sizeof(Weapon_info[idx].tech_anim_filename)-1);
	}

	return ade_set_args(L, "s", Weapon_info[idx].tech_anim_filename);
}

ADE_VIRTVAR(TechDescription, l_Weaponclass, "string", "Weapon class tech description string", "string", "Description string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Weaponclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "s", "");

	weapon_info *wip = &Weapon_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(wip->tech_desc);
		if(s != NULL) {
			wip->tech_desc = (char*)vm_malloc(strlen(s)+1);
			strcpy(wip->tech_desc, s);
		} else {
			wip->tech_desc = NULL;
		}
	}

	if(wip->tech_desc != NULL)
		return ade_set_args(L, "s", wip->tech_desc);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(Model, l_Weaponclass, "model", "Model", "model", "Weapon class model, or invalid model handle if weaponclass handle is invalid")
{
	int weapon_info_idx=-1;
	model_h *mdl = NULL;
	if(!ade_get_args(L, "o|o", l_Weaponclass.Get(&weapon_info_idx), l_Model.GetPtr(&mdl)))
		return ade_set_error(L, "o", l_Model.Set(-1));

	if(weapon_info_idx < 0 || weapon_info_idx >= Num_weapon_types)
		return ade_set_error(L, "o", l_Model.Set(-1));

	weapon_info *wip = &Weapon_info[weapon_info_idx];

	int mid = (mdl ? mdl->GetID() : -1);

	if(ADE_SETTING_VAR && mid > -1) {
		wip->model_num = mid;
	}

	return ade_set_args(L, "o", l_Model.Set(model_h(wip->model_num)));
}

ADE_VIRTVAR(ArmorFactor, l_Weaponclass, "number", "Amount of weapon damage applied to ship hull (0-1.0)", "number", "Armor factor, or empty string if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].armor_factor = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].armor_factor);
}

ADE_VIRTVAR(Damage, l_Weaponclass, "number", "Amount of damage that weapon deals", "number", "Damage amount, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].damage = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].damage);
}

ADE_VIRTVAR(FireWait, l_Weaponclass, "number", "Weapon fire wait (cooldown time) in seconds", "number", "Fire wait time, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].fire_wait = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].fire_wait);
}

ADE_VIRTVAR(FreeFlightTime, l_Weaponclass, "number", "The time the weapon will fly before turing onto its target", "number", "Free flight time or emty string if invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].free_flight_time = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].free_flight_time);
}

ADE_VIRTVAR(LifeMax, l_Weaponclass, "number", "Life of weapon in seconds", "number", "Life of weapon, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].lifetime = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].lifetime);
}

ADE_VIRTVAR(Range, l_Weaponclass, "number", "Range of weapon in meters", "number", "Weapon Range, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].weapon_range = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].weapon_range);
}

ADE_VIRTVAR(Mass, l_Weaponclass, "number", "Weapon mass", "number", "Weapon mass, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].mass = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].mass);
}

ADE_VIRTVAR(ShieldFactor, l_Weaponclass, "number", "Amount of weapon damage applied to ship shields (0-1.0)", "number", "Shield damage factor, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].shield_factor = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].shield_factor);
}

ADE_VIRTVAR(SubsystemFactor, l_Weaponclass, "number", "Amount of weapon damage applied to ship subsystems (0-1.0)", "number", "Subsystem damage factor, or 0 if handle is invalid")
{
	int idx;
	float f = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].subsystem_factor = f;
	}

	return ade_set_args(L, "f", Weapon_info[idx].subsystem_factor);
}

ADE_VIRTVAR(TargetLOD, l_Weaponclass, "number", "LOD used for weapon model in the targeting computer", "number", "LOD number, or 0 if handle is invalid")
{
	int idx;
	int lod = 0;
	if(!ade_get_args(L, "o|i", l_Weaponclass.Get(&idx), &lod))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].hud_target_lod = lod;
	}

	return ade_set_args(L, "i", Weapon_info[idx].hud_target_lod);
}

ADE_VIRTVAR(Speed, l_Weaponclass, "number", "Weapon max speed, aka $Velocity in weapons.tbl", "number", "Weapon speed, or 0 if handle is invalid")
{
	int idx;
	float spd = 0.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &spd))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		Weapon_info[idx].max_speed = spd;
	}

	return ade_set_args(L, "f", Weapon_info[idx].max_speed);
}

ADE_VIRTVAR(Bomb, l_Weaponclass, "boolean", "Is weapon class flagged as bomb", "boolean", "New flag")
{
	int idx;
	bool newVal = false;
	if(!ade_get_args(L, "o|b", l_Weaponclass.Get(&idx), &newVal))
		return ADE_RETURN_FALSE;

	if(idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;
	
	weapon_info *info = &Weapon_info[idx];

	if(ADE_SETTING_VAR)
	{
        info->wi_flags.set(Weapon::Info_Flags::Bomb, newVal);
	}
		

	if (info->wi_flags[Weapon::Info_Flags::Bomb])
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_VIRTVAR(CargoSize, l_Weaponclass, "number", "The cargo size of this weapon class", "number", "The new cargo size or -1 on error")
{
	int idx;
	float newVal = -1.0f;
	if(!ade_get_args(L, "o|f", l_Weaponclass.Get(&idx), &newVal))
		return ade_set_args(L, "f", -1.0f);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_args(L, "f", -1.0f);
	
	weapon_info *info = &Weapon_info[idx];

	if(ADE_SETTING_VAR)
	{
		if(newVal > 0)
		{
			info->cargo_size = newVal;
		}
		else
		{
			LuaError(L, "Cargo size must be bigger than zero, got %f!", newVal);
		}
	}

	return ade_set_args(L, "f", info->cargo_size);
}

ADE_FUNC(isValid, l_Weaponclass, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getWeaponClassIndex, l_Weaponclass, NULL, "Gets the index value of the weapon class", "number", "index value of the weapon class")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ade_set_args(L, "i", -1);

	if(idx < 0 || idx >= Num_weapon_types)
		return ade_set_args(L, "i", -1);

	return ade_set_args(L, "i", idx + 1);
}

ADE_FUNC(isLaser, l_Weaponclass, NULL, "Return true if the weapon is a primary weapon (this includes Beams). This function is deprecated, use isPrimary instead.", "boolean", "true if the weapon is a primary, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].subtype == WP_LASER)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isMissile, l_Weaponclass, NULL, "Return true if the weapon is a secondary weapon. This function is deprecated, use isSecondary instead.", "boolean", "true if the weapon is a secondary, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].subtype == WP_MISSILE)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isPrimary, l_Weaponclass, NULL, "Return true if the weapon is a primary weapon (this includes Beams)", "boolean", "true if the weapon is a primary, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].subtype == WP_LASER)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isSecondary, l_Weaponclass, NULL, "Return true if the weapon is a secondary weapon", "boolean", "true if the weapon is a secondary, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].subtype == WP_MISSILE)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isBeam, l_Weaponclass, NULL, "Return true if the weapon is a beam", "boolean", "true if the weapon is a beam, false otherwise")
{
	int idx;
	if(!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;

	if (Weapon_info[idx].wi_flags[Weapon::Info_Flags::Beam] || Weapon_info[idx].subtype == WP_BEAM)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

// Checks if a weapon has been paged in (counted as used)
ADE_FUNC(isWeaponUsed, l_Weaponclass, NULL, "Return true if the weapon is paged in.", "boolean", "True if the weapon is paged in, false if otherwise")
{
	int idx = -1;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;

	if (!weapon_used(idx)) {
		return ADE_RETURN_FALSE;
	} else {
		return ADE_RETURN_TRUE;
	}
}

// Pages in a weapon
ADE_FUNC(loadWeapon, l_Weaponclass, NULL, "Pages in a weapon. Returns True on success.", "boolean", "True if page in was successful, false otherwise.")
{
	int idx = -1;
	if (!ade_get_args(L, "o", l_Weaponclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= Num_weapon_types)
		return ADE_RETURN_FALSE;

	if (!weapon_page_in(idx)) {
		return ADE_RETURN_FALSE;
	} else {
		return ADE_RETURN_TRUE;
	}
}

class mc_info_h
{
protected:
	mc_info* info;
public:
	mc_info_h(mc_info* val) : info(val) {}

	mc_info_h() : info(NULL) {}

	mc_info *Get()
	{
		return info;
	}

	void deleteInfo()
	{
		if (!this->IsValid())
			return;

		delete info;

		info = NULL;
	}

	bool IsValid()
	{
		return info != NULL;
	}
};

//**********HANDLE: Collision info
ade_obj<mc_info_h> l_ColInfo("collision info", "Information about a collision");

ADE_FUNC(__gc, l_ColInfo, NULL, "Removes the allocated reference of this handle", NULL, NULL)
{
	mc_info_h* info;

	if(!ade_get_args(L, "o", l_ColInfo.GetPtr(&info)))
		return ADE_RETURN_NIL;

	if (info->IsValid())
		info->deleteInfo();

	return ADE_RETURN_NIL;
}

ADE_VIRTVAR(Model, l_ColInfo, "model", "The model this collision info is about", "model", "The model")
{	
	mc_info_h* info;
	model_h * mh = nullptr;

	if(!ade_get_args(L, "o|o", l_ColInfo.GetPtr(&info), l_Model.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Model.Set(model_h()));

	if (!info->IsValid())
		return ade_set_error(L, "o", l_Model.Set(model_h()));

	mc_info *collide = info->Get();

	int modelNum = collide->model_num;

	if (ADE_SETTING_VAR && mh)
	{
		if (mh->IsValid())
		{
			collide->model_num = mh->GetID();
		}
	}

	return ade_set_args(L, "o", l_Model.Set(model_h(modelNum)));
}

ADE_FUNC(getCollisionDistance, l_ColInfo, NULL, "The distance to the closest collision point", "number", "distance or -1 on error")
{
	mc_info_h* info;

	if(!ade_get_args(L, "o", l_ColInfo.GetPtr(&info)))
		return ade_set_error(L, "f", -1.0f);

	if (!info->IsValid())
		return ade_set_error(L, "f", -1.0f);

	mc_info *collide = info->Get();

	if (collide->num_hits <= 0) 
	{
		return ade_set_args(L, "f", -1.0f);;
	}
	else
	{
		return ade_set_args(L, "f", collide->hit_dist);
	}
}

ADE_FUNC(getCollisionPoint, l_ColInfo, "[boolean local]", "The collision point of this information (local to the object if boolean is set to <i>true</i>)", "vector", "The collision point or nil of none")
{
	mc_info_h* info;
	bool local = false;

	if(!ade_get_args(L, "o|b", l_ColInfo.GetPtr(&info), &local))
		return ADE_RETURN_NIL;

	if (!info->IsValid())
		return ADE_RETURN_NIL;

	mc_info *collide = info->Get();
	
	if (collide->num_hits <= 0) 
	{
		return ADE_RETURN_NIL;
	}
	else
	{
		if (local)
			return ade_set_args(L, "o", l_Vector.Set(collide->hit_point));
		else
			return ade_set_args(L, "o", l_Vector.Set(collide->hit_point_world));
	}
}

ADE_FUNC(getCollisionNormal, l_ColInfo, "[boolean local]", "The collision normal of this information (local to object if boolean is set to <i>true</i>)", "vector", "The collision normal or nil of none")
{
	mc_info_h* info;
	bool local = false;

	if(!ade_get_args(L, "o|b", l_ColInfo.GetPtr(&info), &local))
		return ADE_RETURN_NIL;

	if (!info->IsValid())
		return ADE_RETURN_NIL;

	mc_info *collide = info->Get();

	if (collide->num_hits <= 0) 
	{
		return ADE_RETURN_NIL;
	}
	else
	{
		if (!local)
		{
			vec3d normal;

			vm_vec_unrotate(&normal, &collide->hit_normal, collide->orient);

			return ade_set_args(L, "o", l_Vector.Set(normal));
		}
		else
		{
			return ade_set_args(L, "o", l_Vector.Set(collide->hit_normal));
		}
	}
}

ADE_FUNC(isValid, l_ColInfo, NULL, "Detects if this handle is valid", "boolean", "true if valid false otherwise")
{
	mc_info_h* info;

	if(!ade_get_args(L, "o", l_ColInfo.GetPtr(&info)))
		return ADE_RETURN_NIL;

	if (info->IsValid())
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

//**********HANDLE: modeltextures
ADE_FUNC(__len, l_ModelTextures, NULL, "Number of textures on model", "number", "Number of model textures")
{
	modeltextures_h *mth;
	if(!ade_get_args(L, "o", l_ModelTextures.GetPtr(&mth)))
		return ade_set_error(L, "i", 0);

	if(!mth->IsValid())
		return ade_set_error(L, "i", 0);

	polymodel *pm = mth->Get();

	if(pm == NULL)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", TM_NUM_TYPES*pm->n_textures);
}

ADE_INDEXER(l_ModelTextures, "texture", "number Index/string TextureName", "texture", "Model textures, or invalid modeltextures handle if model handle is invalid")
{
	modeltextures_h *mth = NULL;
	int new_tex = -1;
	char *s = NULL;

	if (!ade_get_args(L, "os|o", l_ModelTextures.GetPtr(&mth), &s, l_Texture.Get(&new_tex)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	polymodel *pm = mth->Get();

	if (!mth->IsValid() || s == NULL || pm == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	texture_info *tinfo = NULL;
	texture_map *tmap = NULL;

	if(strspn(s, "0123456789") == strlen(s))
	{
		int num_textures = TM_NUM_TYPES*pm->n_textures;
		int idx = atoi(s) - 1;	//Lua->FS2

		if (idx < 0 || idx >= num_textures)
			return ade_set_error(L, "o", l_Texture.Set(-1));

		tmap = &pm->maps[idx / TM_NUM_TYPES];
		tinfo = &tmap->textures[idx % TM_NUM_TYPES];
	}

	if(tinfo == NULL)
	{
		for (int i = 0; i < pm->n_textures; i++)
		{
			tmap = &pm->maps[i];

			int tnum = tmap->FindTexture(s);
			if(tnum > -1)
				tinfo = &tmap->textures[tnum];
		}
	}
	
	if(tinfo == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if (ADE_SETTING_VAR) {
		tinfo->SetTexture(new_tex);
	}

	return ade_set_args(L, "o", l_Texture.Set(tinfo->GetTexture()));
}

ADE_FUNC(isValid, l_ModelTextures, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	modeltextures_h *mth;
	if(!ade_get_args(L, "o", l_ModelTextures.GetPtr(&mth)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", mth->IsValid());
}

//**********HANDLE: Object
ade_obj<object_h> l_Object("object", "Object handle");
//Helper function
//Returns 1 if object sig stored in idx exists, and stores Objects[] index in idx
//Returns 0 if object sig does not exist, and does not change idx

ADE_FUNC(__eq, l_Object, "object, object", "Checks whether two object handles are for the same object", "boolean", "True if equal, false if not or a handle is invalid")
{
	object_h *o1, *o2;
	if(!ade_get_args(L, "oo", l_Object.GetPtr(&o1), l_Object.GetPtr(&o2)))
		return ADE_RETURN_FALSE;

	if(!o1->IsValid() || !o2->IsValid())
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", o1->sig == o2->sig);
}

ADE_FUNC(__tostring, l_Object, NULL, "Returns name of object (if any)", "string", "Object name, or empty string if handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Object.GetPtr(&objh)))
		return ade_set_error(L, "s", "");

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	char buf[512];

	switch(objh->objp->type)
	{
		case OBJ_SHIP:
			sprintf(buf, "%s", Ships[objh->objp->instance].ship_name);
			break;
		case OBJ_WEAPON:
			sprintf(buf, "%s projectile", Weapon_info[Weapons[objh->objp->instance].weapon_info_index].name);
			break;
		default:
			sprintf(buf, "Object %d [%d]", OBJ_INDEX(objh->objp), objh->sig);
	}

	return ade_set_args(L, "s", buf);
}

ADE_VIRTVAR(Parent, l_Object, "object", "Parent of the object. Value may also be a deriviative of the 'object' class, such as 'ship'.", "object", "Parent handle, or invalid handle if object is invalid")
{
	object_h *objh;
	object_h *newparenth = NULL;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Object.GetPtr(&newparenth)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR)
	{
		if(newparenth != NULL && newparenth->IsValid())
		{
			objh->objp->parent = OBJ_INDEX(newparenth->objp);
			objh->objp->parent_sig = newparenth->sig;
			objh->objp->parent_type = newparenth->objp->type;
		}
		else
		{
			objh->objp->parent = -1;
			objh->objp->parent_sig = 0;
			objh->objp->parent_type = OBJ_NONE;
		}
	}

	if(objh->objp->parent > -1)
		return ade_set_object_with_breed(L, objh->objp->parent);
	else
		return ade_set_args(L, "o", l_Object.Set(object_h()));
}

ADE_VIRTVAR(Position, l_Object, "vector", "Object world position (World vector)", "vector", "World position, or null vector if handle is invalid")
{
	object_h *objh;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		objh->objp->pos = *v3;
		if (objh->objp->type == OBJ_WAYPOINT) {
			waypoint *wpt = find_waypoint_with_objnum(OBJ_INDEX(objh->objp));
			wpt->set_pos(v3);
		}
	}

	return ade_set_args(L, "o", l_Vector.Set(objh->objp->pos));
}

ADE_VIRTVAR(LastPosition, l_Object, "vector", "Object world position as of last frame (World vector)", "vector", "World position, or null vector if handle is invalid")
{
	object_h *objh;
	vec3d *v3=NULL;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v3 != NULL) {
		objh->objp->last_pos = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(objh->objp->last_pos));
}

ADE_VIRTVAR(Orientation, l_Object, "orientation", "Object world orientation (World orientation)", "orientation", "Orientation, or null orientation if handle is invalid")
{
	object_h *objh;
	matrix_h *mh=NULL;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h(&vmd_identity_matrix)));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h(&vmd_identity_matrix)));

	if(ADE_SETTING_VAR && mh != NULL) {
		objh->objp->orient = *mh->GetMatrix();
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&objh->objp->orient)));
}

ADE_VIRTVAR(LastOrientation, l_Object, "orientation", "Object world orientation as of last frame (World orientation)", "orientation", "Orientation, or null orientation if handle is invalid")
{
	object_h *objh;
	matrix_h *mh=NULL;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h(&vmd_identity_matrix)));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h(&vmd_identity_matrix)));

	if(ADE_SETTING_VAR && mh != NULL) {
		objh->objp->last_orient = *mh->GetMatrix();
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&objh->objp->last_orient)));
}

ADE_VIRTVAR(Physics, l_Object, "physics", "Physics data used to move ship between frames", "physics", "Physics data, or invalid physics handle if object handle is invalid")
{
	object_h *objh;
	physics_info_h *pih = nullptr;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Physics.GetPtr(&pih)))
		return ade_set_error(L, "o", l_Physics.Set(physics_info_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Physics.Set(physics_info_h()));

	if(ADE_SETTING_VAR && pih && pih->IsValid()) {
		objh->objp->phys_info = *pih->pi;
	}

	return ade_set_args(L, "o", l_Physics.Set(physics_info_h(objh->objp)));
}

ADE_VIRTVAR(HitpointsLeft, l_Object, "number", "Hitpoints an object has left", "number", "Hitpoints left, or 0 if handle is invalid")
{
	object_h *objh = NULL;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Object.GetPtr(&objh), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	//Set hull strength.
	if(ADE_SETTING_VAR) {
		objh->objp->hull_strength = f;
	}

	return ade_set_args(L, "f", objh->objp->hull_strength);
}

ADE_VIRTVAR(Shields, l_Object, "shields", "Shields", "shields", "Shields handle, or invalid shields handle if object handle is invalid")
{
	object_h *objh;
	object_h *sobjh = nullptr;
	if(!ade_get_args(L, "o|o", l_Object.GetPtr(&objh), l_Shields.GetPtr(&sobjh)))
		return ade_set_error(L, "o", l_Shields.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Shields.Set(object_h()));

	//WMC - copy shields
	if(ADE_SETTING_VAR && sobjh && sobjh->IsValid())
	{
		for(int i = 0; i < objh->objp->n_quadrants; i++)
			shield_set_quad(objh->objp, i, shield_get_quad(sobjh->objp, i));
	}

	return ade_set_args(L, "o", l_Shields.Set(object_h(objh->objp)));
}

ADE_FUNC(getSignature, l_Object, NULL, "Gets the object's unique signature", "number", "Returns the object's unique numeric signature, or -1 if invalid.  Useful for creating a metadata system")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_Object.GetPtr(&oh)))
		return ade_set_error(L, "i", -1);

	if(!oh->IsValid())
		return ade_set_error(L, "i", -1);
 
	// this shouldn't be possible, added here to trap the offending object
	Assert(oh->sig > 0);

	return ade_set_args(L, "i", oh->sig);
}

ADE_FUNC(isValid, l_Object, NULL, "Detects whether handle is valid", "boolean", "true if handle is valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_Object.GetPtr(&oh)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", oh->IsValid());
}

ADE_FUNC(getBreedName, l_Object, NULL, "Gets object type", "string", "Object type name, or empty string if handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Object.GetPtr(&objh)))
		return ade_set_error(L, "s", "");

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Object_type_names[objh->objp->type]);
}

ADE_VIRTVAR(CollisionGroups, l_Object, "number", "Collision group data", "number", "Current collision group signature. NOTE: This is a bitfield, NOT a normal number.")
{
	object_h *objh = NULL;
	int id = 0;
	if(!ade_get_args(L, "o|i", l_Object.GetPtr(&objh), &id))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	//Set collision group data
	if(ADE_SETTING_VAR) {
		objh->objp->collision_group_id = id;
	}

	return ade_set_args(L, "i", objh->objp->collision_group_id);
}

ADE_FUNC(getfvec, l_Object, "[boolean normalize]", "Returns the objects' current fvec.", "vector", "Objects' forward vector, or nil if invalid. If called with a true argument, vector will be normalized.")
{
	object_h *objh = NULL;
	object *obj = NULL;
	bool normalize = false;
	
	if (!ade_get_args(L, "o|b", l_Object.GetPtr(&objh), &normalize)) {
		return ADE_RETURN_NIL;
	}

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	obj = objh->objp;
	vec3d v1 = obj->orient.vec.fvec;
	if (normalize)
		vm_vec_normalize(&v1);

	return ade_set_args(L, "o", l_Vector.Set(v1));
}

ADE_FUNC(getuvec, l_Object, "[boolean normalize]", "Returns the objects' current uvec.", "vector", "Objects' up vector, or nil if invalid. If called with a true argument, vector will be normalized.")
{
	object_h *objh = NULL;
	object *obj = NULL;
	bool normalize = false;
	
	if (!ade_get_args(L, "o|b", l_Object.GetPtr(&objh), &normalize)) {
		return ADE_RETURN_NIL;
	}

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	obj = objh->objp;
	vec3d v1 = obj->orient.vec.uvec;
	if (normalize)
		vm_vec_normalize(&v1);

	return ade_set_args(L, "o", l_Vector.Set(v1));
}

ADE_FUNC(getrvec, l_Object, "[boolean normalize]", "Returns the objects' current rvec.", "vector", "Objects' rvec, or nil if invalid. If called with a true argument, vector will be normalized.")
{
	object_h *objh = NULL;
	object *obj = NULL;
	bool normalize = false;
	
	if (!ade_get_args(L, "o|b", l_Object.GetPtr(&objh), &normalize)) {
		return ADE_RETURN_NIL;
	}

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	obj = objh->objp;
	vec3d v1 = obj->orient.vec.rvec;
	if (normalize)
		vm_vec_normalize(&v1);

	return ade_set_args(L, "o", l_Vector.Set(v1));
}

ADE_FUNC(checkRayCollision, l_Object, "vector Start Point, vector End Point, [boolean Local]", "Checks the collisions between the polygons of the current object and a ray", "vector, collision info", "World collision point (local if boolean is set to true) and the specific collsision info, nil if no collisions")
{
	object_h *objh = NULL;
	object *obj = NULL;
	int model_num = -1, model_instance_num = -1, temp = 0;
	vec3d *v3a, *v3b;
	bool local = false;
	if(!ade_get_args(L, "ooo|b", l_Object.GetPtr(&objh), l_Vector.GetPtr(&v3a), l_Vector.GetPtr(&v3b), &local))
		return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	obj = objh->objp;
	int flags = 0;
	int submodel = -1;

	switch(obj->type) {
		case OBJ_SHIP:
			model_num = Ship_info[Ships[obj->instance].ship_info_index].model_num;
			flags = (MC_CHECK_MODEL | MC_CHECK_RAY);
			break;
		case OBJ_WEAPON:
			model_num = Weapon_info[Weapons[obj->instance].weapon_info_index].model_num;
			flags = (MC_CHECK_MODEL | MC_CHECK_RAY);
			break;
		case OBJ_DEBRIS:
			model_num = Debris[obj->instance].model_num;
			flags = (MC_CHECK_MODEL | MC_CHECK_RAY | MC_SUBMODEL);
			submodel = Debris[obj->instance].submodel_num;
			break;
		case OBJ_ASTEROID:
			temp = Asteroids[obj->instance].asteroid_subtype;
			model_num = Asteroid_info[Asteroids[obj->instance].asteroid_type].model_num[temp];
			flags = (MC_CHECK_MODEL | MC_CHECK_RAY);
			break;
		default:
			return ADE_RETURN_NIL;
	}

	if (model_num < 0)
		return ADE_RETURN_NIL;

	if (obj->type == OBJ_SHIP) {
		model_instance_num = Ships[obj->instance].model_instance_num;
	} else if (obj->type == OBJ_WEAPON) {
		model_instance_num = Weapons[obj->instance].model_instance_num;
	} else if (obj->type == OBJ_ASTEROID) {
		model_instance_num = Asteroids[obj->instance].model_instance_num;
	}

	mc_info hull_check;
	mc_info_init(&hull_check);

	hull_check.model_num = model_num;
	hull_check.model_instance_num = model_instance_num;
	hull_check.submodel_num = submodel;
	hull_check.orient = &obj->orient;
	hull_check.pos = &obj->pos;
	hull_check.p0 = v3a;
	hull_check.p1 = v3b;
	hull_check.flags = flags;

	if ( !model_collide(&hull_check) ) {
		return ADE_RETURN_NIL;
	}

	if (local)
		return ade_set_args(L, "oo", l_Vector.Set(hull_check.hit_point), l_ColInfo.Set(mc_info_h(new mc_info(hull_check))));
	else
		return ade_set_args(L, "oo", l_Vector.Set(hull_check.hit_point_world),  l_ColInfo.Set(mc_info_h(new mc_info(hull_check))));
}

//**********HANDLE: Asteroid
ade_obj<object_h> l_Asteroid("asteroid", "Asteroid handle", &l_Object);

ADE_VIRTVAR(Target, l_Asteroid, "object", "Asteroid target object; may be object derivative, such as ship.", "object", "Target object, or invalid handle if asteroid handle is invalid")
{
	object_h *oh = NULL;
	object_h *th = NULL;
	if(!ade_get_args(L, "o|o", l_Asteroid.GetPtr(&oh), l_Object.GetPtr(&th)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	asteroid *asp = &Asteroids[oh->objp->instance];

	if(ADE_SETTING_VAR && th != NULL) {
		if(th->IsValid())
			asp->target_objnum = OBJ_INDEX(th->objp);
		else
			asp->target_objnum = -1;
	}

	if(asp->target_objnum > 0 && asp->target_objnum < MAX_OBJECTS)
		return ade_set_object_with_breed(L, asp->target_objnum);
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

}

ADE_FUNC(kill, l_Asteroid, "[ship killer=nil, wvector hitpos=nil]", "Kills the asteroid. Set \"killer\" to designate a specific ship as having been the killer, and \"hitpos\" to specify the world position of the hit location; if nil, the asteroid center is used.", "boolean", "True if successful, false or nil otherwise")
{
	object_h *victim,*killer=NULL;
	vec3d *hitpos=NULL;
	if(!ade_get_args(L, "o|oo", l_Asteroid.GetPtr(&victim), l_Ship.GetPtr(&killer), l_Vector.GetPtr(&hitpos)))
		return ADE_RETURN_NIL;

	if(!victim->IsValid())
		return ADE_RETURN_NIL;

	if(killer != NULL && !killer->IsValid())
		return ADE_RETURN_NIL;

	if (!hitpos)
		hitpos = &victim->objp->pos;

	if (killer)
		asteroid_hit(victim->objp, killer->objp, hitpos, victim->objp->hull_strength + 1);
	else
		asteroid_hit(victim->objp, NULL,         hitpos, victim->objp->hull_strength + 1);

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Cockpit Display info
class cockpit_disp_info_h
{
private:
	ship_info *m_sip;
	size_t m_display_num;

public:
	cockpit_disp_info_h() : m_sip( NULL ), m_display_num( INVALID_ID ) {}
	cockpit_disp_info_h(ship_info *sip, size_t display_num)
	{
		this->m_sip = sip;
		this->m_display_num = display_num;
	}

	cockpit_display_info *Get()
	{
		if (!this->isValid())
			return NULL;

		return &m_sip->displays[m_display_num];
	}

	bool isValid()
	{
		if (m_sip == NULL)
		{
			return false;
		}

		if (m_display_num == INVALID_ID)
		{
			return false;
		}

		if ( m_display_num >= m_sip->displays.size())
		{
			return false;
		}

		if (!m_sip->hud_enabled)
		{
			return false;
		}

		return true;
	}
};

ade_obj<cockpit_disp_info_h> l_DisplayInfo("display info", "Ship cockpit display information handle");

ADE_FUNC(getName, l_DisplayInfo, NULL, "Gets the name of this cockpit display as defined in ships.tbl", "string", "Name string or empty string on error")
{
	cockpit_disp_info_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_DisplayInfo.GetPtr(&cdh)))
		return ade_set_error(L, "s", "");

	if (!cdh->isValid())
		return ade_set_error(L, "s", "");

	cockpit_display_info *cdi = cdh->Get();

	if (cdi == NULL)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", cdi->name);
}

ADE_FUNC(getFileName, l_DisplayInfo, NULL, "Gets the file name of the target texture of this cockpit display", "string", "Texture name string or empty string on error")
{
	cockpit_disp_info_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_DisplayInfo.GetPtr(&cdh)))
		return ade_set_error(L, "s", "");

	if (!cdh->isValid())
		return ade_set_error(L, "s", "");

	cockpit_display_info *cdi = cdh->Get();

	if (cdi == NULL)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", cdi->filename);
}

ADE_FUNC(getForegroundFileName, l_DisplayInfo, NULL, "Gets the file name of the foreground texture of this cockpit display", "string", "Foreground texture name string or nil if texture is not set or on error")
{
	cockpit_disp_info_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_DisplayInfo.GetPtr(&cdh)))
		return ADE_RETURN_NIL;

	if (!cdh->isValid())
		return ADE_RETURN_NIL;

	cockpit_display_info *cdi = cdh->Get();

	if (cdi == NULL)
		return ADE_RETURN_NIL;

	if (cdi->fg_filename[0] == 0)
		return ADE_RETURN_NIL;

	return ade_set_args(L, "s", cdi->fg_filename);
}

ADE_FUNC(getBackgroundFileName, l_DisplayInfo, NULL, "Gets the file name of the background texture of this cockpit display", "string", "Background texture name string or nil if texture is not set or on error")
{
	cockpit_disp_info_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_DisplayInfo.GetPtr(&cdh)))
		return ADE_RETURN_NIL;

	if (!cdh->isValid())
		return ADE_RETURN_NIL;

	cockpit_display_info *cdi = cdh->Get();

	if (cdi == NULL)
		return ADE_RETURN_NIL;

	if (cdi->bg_filename[0] == 0)
		return ADE_RETURN_NIL;

	return ade_set_args(L, "s", cdi->bg_filename);
}

ADE_FUNC(getSize, l_DisplayInfo, NULL, "Gets the size of this cockpit display", "number, number", "Width and height of the display or -1, -1 on error")
{
	cockpit_disp_info_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_DisplayInfo.GetPtr(&cdh)))
		return ade_set_error(L, "ii", -1, -1);

	if (!cdh->isValid())
		return ade_set_error(L, "ii", -1, -1);

	cockpit_display_info *cdi = cdh->Get();

	if (cdi == NULL)
		return ade_set_error(L, "ii", -1, -1);

	return ade_set_args(L, "ii", cdi->size[0], cdi->size[1]);
}

ADE_FUNC(getOffset, l_DisplayInfo, NULL, "Gets the offset of this cockpit display", "number, number", "x and y offset of the display or -1, -1 on error")
{
	cockpit_disp_info_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_DisplayInfo.GetPtr(&cdh)))
		return ade_set_error(L, "ii", -1, -1);

	if (!cdh->isValid())
		return ade_set_error(L, "ii", -1, -1);

	cockpit_display_info *cdi = cdh->Get();

	if (cdi == NULL)
		return ade_set_error(L, "ii", -1, -1);

	return ade_set_args(L, "ii", cdi->offset[0], cdi->offset[1]);
}

ADE_FUNC(isValid, l_DisplayInfo, NULL, "Detects whether this handle is valid", "boolean", "true if valid false otherwise")
{
	cockpit_disp_info_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_DisplayInfo.GetPtr(&cdh)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", cdh->isValid());
}

//**********HANDLE: Cockpit Display
class cockpit_display_h
{
private:
	int obj_num;
	object *m_objp;
	size_t m_display_num;

public:
	cockpit_display_h() : obj_num( -1 ), m_objp( NULL ), m_display_num( INVALID_ID ) {}
	cockpit_display_h(object *objp, size_t display_num)
	{
		this->obj_num = OBJ_INDEX(objp);
		this->m_objp = objp;

		this->m_display_num = display_num;
	}

	cockpit_display *Get()
	{
		if (!isValid())
		{
			return NULL;
		}

		return &Player_displays[m_display_num];
	}

	size_t GetId()
	{
		if (!isValid())
		{
			return INVALID_ID;
		}

		return m_display_num;
	}

	bool isValid()
	{
		if (obj_num < 0 || obj_num > MAX_OBJECTS)
		{
			return false;
		}

		if (m_objp == NULL || OBJ_INDEX(m_objp) != obj_num)
		{
			return false;
		}

		// Only player has cockpit displays
		if (m_objp != Player_obj)
		{
			return false;
		}

		if (m_display_num == INVALID_ID)
		{
			return false;
		}

		if (m_display_num >= Player_displays.size())
		{
			return false;		}

		return true;
	}
};
ade_obj<cockpit_display_h> l_CockpitDisplay("display", "Cockpit display handle");

ADE_FUNC(startRendering, l_CockpitDisplay, "[boolean setClip = true]", "Starts rendering to this cockpit display. That means if you get a valid texture handle from this function then the rendering system is ready to do a render to texture. If setClip is true then the clipping region will be set to the region of the cockpit display.<br><b>Important:</b> You have to call stopRendering after you're done or this render target will never be released!", "texture", "texture handle that is being drawn to or invalid handle on error")
{
	cockpit_display_h *cdh = NULL;
	bool setClip = true;

	if (!ade_get_args(L, "o|b", l_CockpitDisplay.GetPtr(&cdh), &setClip))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if (!cdh->isValid())
		return ade_set_error(L, "o", l_Texture.Set(-1));

	int bm_handle = ship_start_render_cockpit_display(cdh->GetId());

	if (bm_is_valid(bm_handle) && setClip)
	{
		cockpit_display *cd = cdh->Get();
		gr_set_clip(cd->offset[0], cd->offset[1], cd->size[0], cd->size[1], GR_RESIZE_NONE);
	}

	return ade_set_args(L, "o", l_Texture.Set(bm_handle));
}

ADE_FUNC(stopRendering, l_CockpitDisplay, NULL, "Stops rendering to this cockpit display", "boolean", "true if successfull, false otherwise")
{
	cockpit_display_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_CockpitDisplay.GetPtr(&cdh)))
		return ADE_RETURN_FALSE;

	if (!cdh->isValid())
		return ADE_RETURN_FALSE;

	ship_end_render_cockpit_display(cdh->GetId());
	gr_reset_clip();

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getBackgroundTexture, l_CockpitDisplay, NULL, "Gets the background texture handle of this cockpit display", "texture", "texture handle or invalid handle if no background texture or an error happened")
{
	cockpit_display_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_CockpitDisplay.GetPtr(&cdh)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if (!cdh->isValid())
		return ade_set_error(L, "o", l_Texture.Set(-1));

	cockpit_display *cd = cdh->Get();

	if (cd == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	return ade_set_args(L, "o", l_Texture.Set(cd->background));
}

ADE_FUNC(getForegroundTexture, l_CockpitDisplay, NULL, "Gets the foreground texture handle of this cockpit display<br>"
														"<b>Important:</b> If you want to do render to texture then you have to use startRendering/stopRendering", "texture", "texture handle or invalid handle if no foreground texture or an error happened")
{
	cockpit_display_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_CockpitDisplay.GetPtr(&cdh)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if (!cdh->isValid())
		return ade_set_error(L, "o", l_Texture.Set(-1));

	cockpit_display *cd = cdh->Get();

	if (cd == NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	return ade_set_args(L, "o", l_Texture.Set(cd->foreground));
}

ADE_FUNC(getSize, l_CockpitDisplay, NULL, "Gets the size of this cockpit display", "number, number", "Width and height of the display or -1, -1 on error")
{
	cockpit_display_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_CockpitDisplay.GetPtr(&cdh)))
		return ade_set_error(L, "ii", -1, -1);

	if (!cdh->isValid())
		return ade_set_error(L, "ii", -1, -1);

	cockpit_display *cd = cdh->Get();

	if (cd == NULL)
		return ade_set_error(L, "ii", -1, -1);

	return ade_set_args(L, "ii", cd->size[0], cd->size[1]);
}

ADE_FUNC(getOffset, l_CockpitDisplay, NULL, "Gets the offset of this cockpit display", "number, number", "x and y offset of the display or -1, -1 on error")
{
	cockpit_display_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_CockpitDisplay.GetPtr(&cdh)))
		return ade_set_error(L, "ii", -1, -1);

	if (!cdh->isValid())
		return ade_set_error(L, "ii", -1, -1);

	cockpit_display *cd = cdh->Get();

	if (cd == NULL)
		return ade_set_error(L, "ii", -1, -1);

	return ade_set_args(L, "ii", cd->offset[0], cd->offset[1]);
}

ADE_FUNC(isValid, l_CockpitDisplay, NULL, "Detects whether this handle is valid or not", "boolean", "true if valid, false otherwise")
{
	cockpit_display_h *cdh = NULL;

	if (!ade_get_args(L, "o", l_CockpitDisplay.GetPtr(&cdh)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", cdh->isValid());
}

//**********HANDLE: CockpitDisplayArray
class cockpit_displays_info_h
{
private:
	int m_ship_info_idx;
public:
	cockpit_displays_info_h() : m_ship_info_idx( -1 ) {}
	cockpit_displays_info_h(int ship_info_idx)
	{
		this->m_ship_info_idx = ship_info_idx;
	}

	ship_info *Get()
	{
		if (!isValid())
			return NULL;

		return &Ship_info[m_ship_info_idx];
	}

	bool isValid()
	{
		if (m_ship_info_idx < 0 || m_ship_info_idx >= static_cast<int>(Ship_info.size()))
		{
			return false;
		}

		if (!Ship_info[m_ship_info_idx].hud_enabled)
		{
			return false;
		}

		return true;
	}
};
ade_obj<cockpit_displays_info_h> l_CockpitDisplayInfos("cockpitdisplays", "Array of cockpit display information");

ADE_FUNC(__len, l_CockpitDisplayInfos, NULL, "Number of cockpit displays for this ship class", "number", "number of cockpit displays or -1 on error")
{
	cockpit_displays_info_h *cdih = NULL;
	if (!ade_get_args(L, "o", l_CockpitDisplayInfos.GetPtr(&cdih)))
		return ade_set_error(L, "i", -1);

	if (!cdih->isValid())
		return ade_set_error(L, "i", -1);

	return ade_set_args(L, "i", (int) cdih->Get()->displays.size());
}

ADE_INDEXER(l_CockpitDisplayInfos, "number/string", "Returns the handle at the requested index or the handle with the specified name", "display info", "display handle or invalid handle on error")
{
	if (lua_isnumber(L, 2))
	{
		cockpit_displays_info_h *cdih = NULL;
		int index = -1;

		if (!ade_get_args(L, "oi", l_CockpitDisplayInfos.GetPtr(&cdih), &index))
		{
			return ade_set_error(L, "o", l_DisplayInfo.Set(cockpit_disp_info_h()));
		}

		if (index < 0)
		{
			return ade_set_error(L, "o", l_DisplayInfo.Set(cockpit_disp_info_h()));
		}

		index--; // Lua -> C/C++
		
		return ade_set_args(L, "o", l_DisplayInfo.Set(cockpit_disp_info_h(cdih->Get(), index)));
	}
	else
	{
		cockpit_displays_info_h *cdih = NULL;
		char *name = NULL;

		if (!ade_get_args(L, "os", l_CockpitDisplayInfos.GetPtr(&cdih), &name))
		{
			return ade_set_error(L, "o", l_DisplayInfo.Set(cockpit_disp_info_h()));
		}

		if (!cdih->isValid())
		{
			return ade_set_error(L, "o", l_DisplayInfo.Set(cockpit_disp_info_h()));
		}

		if (name == NULL)
		{
			return ade_set_error(L, "o", l_DisplayInfo.Set(cockpit_disp_info_h()));
		}

		ship_info *sip = cdih->Get();

		if (!sip->hud_enabled)
		{
			return ade_set_error(L, "o", l_DisplayInfo.Set(cockpit_disp_info_h()));
		}

		size_t index = 0;

		for (SCP_vector<cockpit_display_info>::iterator iter = sip->displays.begin(); iter != sip->displays.end(); ++iter)
		{
			if (!strcmp(name, iter->name))
			{
				break;
			}
			else
			{
				index++;
			}
		}

		if (index == sip->displays.size())
		{
			LuaError(L, "Couldn't find cockpit display info with name \"%s\"", name);
			return ade_set_error(L, "o", l_DisplayInfo.Set(cockpit_disp_info_h()));
		}

		return ade_set_args(L, "o", l_DisplayInfo.Set(cockpit_disp_info_h(cdih->Get(), index)));
	}
}

ADE_FUNC(isValid, l_CockpitDisplayInfos, NULL, "Detects whether this handle is valid", "boolean", "true if valid, false otehrwise")
{
	cockpit_displays_info_h *cdih = NULL;
	if (!ade_get_args(L, "o", l_CockpitDisplayInfos.GetPtr(&cdih)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", cdih->isValid());
}

//**********HANDLE: Shipclass
ade_obj<int> l_Shipclass("shipclass", "Ship class handle");

ADE_FUNC(__tostring, l_Shipclass, NULL, "Ship class name", "string", "Ship class name, or an empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Ship_info[idx].name);
}

ADE_FUNC(__eq, l_Shipclass, "shipclass, shipclass", "Checks if the two classes are equal", "boolean", "true if equal false otherwise")
{
	int idx1,idx2;
	if(!ade_get_args(L, "oo", l_Shipclass.Get(&idx1), l_Shipclass.Get(&idx2)))
		return ade_set_error(L, "b", false);

	if(idx1 < 0 || idx1 >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "b", false);

	if(idx2 < 0 || idx2 >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", idx1 == idx2);
}

ADE_VIRTVAR(Name, l_Shipclass, "string", "Ship class name", "string", "Ship class name, or an empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Ship_info[idx].name, s, sizeof(Ship_info[idx].name)-1);
	}

	return ade_set_args(L, "s", Ship_info[idx].name);
}

ADE_VIRTVAR(ShortName, l_Shipclass, "string", "Ship class short name", "string", "Ship short name, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Ship_info[idx].short_name, s, sizeof(Ship_info[idx].short_name)-1);
	}

	return ade_set_args(L, "s", Ship_info[idx].short_name);
}

ADE_VIRTVAR(TypeString, l_Shipclass, "string", "Ship class type string", "string", "Type string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->type_str);
		if(s != NULL) {
			sip->type_str = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->type_str, s);
		} else {
			sip->type_str = NULL;
		}
	}

	if(sip->type_str != NULL)
		return ade_set_args(L, "s", sip->type_str);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(ManeuverabilityString, l_Shipclass, "string", "Ship class maneuverability string", "string", "Maneuverability string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->maneuverability_str);
		if(s != NULL) {
			sip->maneuverability_str = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->maneuverability_str, s);
		} else {
			sip->maneuverability_str = NULL;
		}
	}

	if(sip->maneuverability_str != NULL)
		return ade_set_args(L, "s", sip->maneuverability_str);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(ArmorString, l_Shipclass, "string", "Ship class armor string", "string", "Armor string, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->armor_str);
		if(s != NULL) {
			sip->armor_str = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->armor_str, s);
		} else {
			sip->armor_str = NULL;
		}
	}

	if(sip->armor_str != NULL)
		return ade_set_args(L, "s", sip->armor_str);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(ManufacturerString, l_Shipclass, "string", "Ship class manufacturer", "string", "Manufacturer, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->manufacturer_str);
		if(s != NULL) {
			sip->manufacturer_str = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->manufacturer_str, s);
		} else {
			sip->manufacturer_str = NULL;
		}
	}

	if(sip->manufacturer_str != NULL)
		return ade_set_args(L, "s", sip->manufacturer_str);
	else
		return ade_set_args(L, "s", "");
}


ADE_VIRTVAR(Description, l_Shipclass, "string", "Ship class description", "string", "Description, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->desc);
		if(s != NULL) {
			sip->desc = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->desc, s);
		} else {
			sip->desc = NULL;
		}
	}

	if(sip->desc != NULL)
		return ade_set_args(L, "s", sip->desc);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(TechDescription, l_Shipclass, "string", "Ship class tech description", "string", "Tech description, or empty string if handle is invalid")
{
	int idx;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &s))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	ship_info *sip = &Ship_info[idx];

	if(ADE_SETTING_VAR) {
		vm_free(sip->tech_desc);
		if(s != NULL) {
			sip->tech_desc = (char*)vm_malloc(strlen(s)+1);
			strcpy(sip->tech_desc, s);
		} else {
			sip->tech_desc = NULL;
		}
	}

	if(sip->tech_desc != NULL)
		return ade_set_args(L, "s", sip->tech_desc);
	else
		return ade_set_args(L, "s", "");
}

ADE_VIRTVAR(AfterburnerFuelMax, l_Shipclass, "number", "Afterburner fuel capacity", "number", "Afterburner capacity, or 0 if handle is invalid")
{
	int idx;
	float fuel = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &fuel))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && fuel >= 0.0f)
		Ship_info[idx].afterburner_fuel_capacity = fuel;

	return ade_set_args(L, "f", Ship_info[idx].afterburner_fuel_capacity);
}

ADE_VIRTVAR(CountermeasuresMax, l_Shipclass, "number", "Maximum number of countermeasures the ship can carry", "number", "Countermeasure capacity, or 0 if handle is invalid")
{
	int idx;
	int i = -1;
	if(!ade_get_args(L, "o|i", l_Shipclass.Get(&idx), &i))
		return ade_set_error(L, "i", 0);

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR && i > -1) {
		Ship_info[idx].cmeasure_max = i;
	}

	return ade_set_args(L, "i", Ship_info[idx].cmeasure_max);
}

ADE_VIRTVAR(Model, l_Shipclass, "model", "Model", "model", "Ship class model, or invalid model handle if shipclass handle is invalid")
{
	int ship_info_idx=-1;
	model_h *mdl = NULL;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&ship_info_idx), l_Model.GetPtr(&mdl)))
		return ade_set_error(L, "o", l_Model.Set(-1));

	if(ship_info_idx < 0 || ship_info_idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "o", l_Model.Set(-1));

	ship_info *sip = &Ship_info[ship_info_idx];

	int mid = (mdl ? mdl->GetID() : -1);

	if(ADE_SETTING_VAR && mid > -1) {
		sip->model_num = mid;
	}

	return ade_set_args(L, "o", l_Model.Set(model_h(sip->model_num)));
}

ADE_VIRTVAR(CockpitModel, l_Shipclass, "model", "Model used for first-person cockpit", "model", "Cockpit model")
{
	int ship_info_idx=-1;
	model_h *mdl = NULL;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&ship_info_idx), l_Model.GetPtr(&mdl)))
		return ade_set_error(L, "o", l_Model.Set(model_h()));

	if(ship_info_idx < 0 || ship_info_idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "o", l_Model.Set(model_h()));

	ship_info *sip = &Ship_info[ship_info_idx];

	int mid = (mdl ? mdl->GetID() : -1);

	if(ADE_SETTING_VAR) {
		sip->cockpit_model_num = mid;
	}

	return ade_set_args(L, "o", l_Model.Set(model_h(sip->cockpit_model_num)));
}

ADE_VIRTVAR(CockpitDisplays, l_Shipclass, "cockpitdisplays", "Gets the cockpit display information array of this ship class", "cockpitdisplays", "Array handle containing the information or invalid handle on error")
{
	int ship_info_idx=-1;
	cockpit_displays_info_h *cdih = NULL;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&ship_info_idx), l_CockpitDisplayInfos.GetPtr(&cdih)))
		return ade_set_error(L, "o", l_CockpitDisplayInfos.Set(cockpit_displays_info_h()));

	if(ship_info_idx < 0 || ship_info_idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "o", l_CockpitDisplayInfos.Set(cockpit_displays_info_h()));

	if(ADE_SETTING_VAR) {
		LuaError(L, "Attempted to use incomplete feature: Cockpit display information copy");
	}

	return ade_set_args(L, "o", l_CockpitDisplayInfos.Set(cockpit_displays_info_h(ship_info_idx)));
}

ADE_VIRTVAR(HitpointsMax, l_Shipclass, "number", "Ship class hitpoints", "number", "Hitpoints, or 0 if handle is invalid")
{
	int idx;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Shipclass.Get(&idx), &f))
		return ade_set_error(L, "f", 0.0f);

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && f >= 0.0f) {
		Ship_info[idx].max_hull_strength = f;
	}

	return ade_set_args(L, "f", Ship_info[idx].max_hull_strength);
}

ADE_VIRTVAR(Species, l_Shipclass, "Species", "Ship class species", "species", "Ship class species, or invalid species handle if shipclass handle is invalid")
{
	int idx;
	int sidx = -1;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&idx), l_Species.Get(&sidx)))
		return ade_set_error(L, "o", l_Species.Set(-1));

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "o", l_Species.Set(-1));

	if(ADE_SETTING_VAR && sidx > -1 && sidx < (int)Species_info.size()) {
		Ship_info[idx].species = sidx;
	}

	return ade_set_args(L, "o", l_Species.Set(Ship_info[idx].species));
}

ADE_VIRTVAR(Type, l_Shipclass, "shiptype", "Ship class type", "shiptype", "Ship type, or invalid handle if shipclass handle is invalid")
{
	int idx;
	int sidx = -1;
	if(!ade_get_args(L, "o|o", l_Shipclass.Get(&idx), l_Shiptype.Get(&sidx)))
		return ade_set_error(L, "o", l_Shiptype.Set(-1));

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "o", l_Shiptype.Set(-1));

	if(ADE_SETTING_VAR && sidx > -1 && sidx < (int)Ship_types.size()) {
		Ship_info[idx].class_type = sidx;
	}

	return ade_set_args(L, "o", l_Shiptype.Set(Ship_info[idx].class_type));
}

ADE_VIRTVAR(AltName, l_Shipclass, "string", "Alternate name for ship class", "string", "Alternate string or empty string if handle is invalid")
{
	char* newName = NULL;
	int idx;
	if(!ade_get_args(L, "o|s", l_Shipclass.Get(&idx), &newName))
		return ade_set_error(L, "s", "");

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && newName != NULL) {
		if (strlen(newName) >= NAME_LENGTH)
		{
			LuaError(L, "Cannot set alternate name value to '%s' because it is too long, maximum length is %d!", newName, NAME_LENGTH - 1);
			return ade_set_error(L, "s", "");
		}

		strcpy_s(Ship_info[idx].alt_name, newName);
	}

	return ade_set_args(L, "s", Ship_info[idx].alt_name);
}

ADE_FUNC(isValid, l_Shipclass, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ADE_RETURN_NIL;

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(isInTechroom, l_Shipclass, NULL, "Gets whether or not the ship class is available in the techroom", "boolean", "Whether ship has been revealed in the techroom, false if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_error(L, "b", false);

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_error(L, "b", false);

	bool b = false;
	if(Player != NULL && (Player->flags & PLAYER_FLAGS_IS_MULTI) && (Ship_info[idx].flags[Ship::Info_Flags::In_tech_database_m])) {
		b = true;
	} else if(Ship_info[idx].flags[Ship::Info_Flags::In_tech_database]) {
		b = true;
	}

	return ade_set_args(L, "b", b);
}

ADE_FUNC(renderTechModel, l_Shipclass, "X1, Y1, X2, Y2, [Rotation %=0, Pitch %=0, Bank %=40, number Zoom=1.3]", "Draws ship model as if in techroom", "boolean", "Whether ship was rendered")
{
	int x1,y1,x2,y2;
	angles rot_angles = {0.0f, 0.0f, 40.0f};
	int idx;
	float zoom = 1.3f;
	if(!ade_get_args(L, "oiiii|ffff", l_Shipclass.Get(&idx), &x1, &y1, &x2, &y2, &rot_angles.h, &rot_angles.p, &rot_angles.b, &zoom))
		return ade_set_error(L, "b", false);

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_args(L, "b", false);

	if(x2 < x1 || y2 < y1)
		return ade_set_args(L, "b", false);

    CLAMP(rot_angles.p, 0.0f, 100.0f);
    CLAMP(rot_angles.b, 0.0f, 100.0f);
    CLAMP(rot_angles.h, 0.0f, 100.0f);

	ship_info *sip = &Ship_info[idx];
	model_render_params render_info;

	if (sip->uses_team_colors) {
		render_info.set_team_color(sip->default_team_name, "none", 0, 0);
	}

	//Make sure model is loaded
	sip->model_num = model_load(sip->pof_file, sip->n_subsystems, &sip->subsystems[0], 0);

	if(sip->model_num < 0)
		return ade_set_args(L, "b", false);

	//Handle angles
	matrix orient = vmd_identity_matrix;
	angles view_angles = {-0.6f, 0.0f, 0.0f};
	vm_angles_2_matrix(&orient, &view_angles);

	rot_angles.p = (rot_angles.p*0.01f) * PI2;
	rot_angles.b = (rot_angles.b*0.01f) * PI2;
	rot_angles.h = (rot_angles.h*0.01f) * PI2;
	vm_rotate_matrix_by_angles(&orient, &rot_angles);

	//Clip
	gr_set_clip(x1,y1,x2-x1,y2-y1,GR_RESIZE_NONE);

	//Handle 3D init stuff
	g3_start_frame(1);
	g3_set_view_matrix(&sip->closeup_pos, &vmd_identity_matrix, sip->closeup_zoom * zoom);

	gr_set_proj_matrix( Proj_fov, gr_screen.clip_aspect, Min_draw_distance, Max_draw_distance);
	gr_set_view_matrix(&Eye_position, &Eye_matrix);

	//Handle light
	light_reset();
	vec3d light_dir = vmd_zero_vector;
	light_dir.xyz.y = 1.0f;	
	light_add_directional(&light_dir, 0.65f, 1.0f, 1.0f, 1.0f);
	light_rotate_all();

	//Draw the ship!!
	model_clear_instance(sip->model_num);
	render_info.set_detail_level_lock(0);

	uint render_flags = MR_AUTOCENTER | MR_NO_FOGGING;

	if(sip->flags[Ship::Info_Flags::No_lighting])
		render_flags |= MR_NO_LIGHTING;

	render_info.set_flags(render_flags);

	model_render_immediate(&render_info, sip->model_num, &orient, &vmd_zero_vector);

	//OK we're done
	gr_end_view_matrix();
	gr_end_proj_matrix();

	//Bye!!
	g3_end_frame();
	gr_reset_clip();

	return ade_set_args(L, "b", true);
}

// Nuke's alternate tech model rendering function
ADE_FUNC(renderTechModel2, l_Shipclass, "X1, Y1, X2, Y2, [orientation Orientation=nil, number Zoom=1.3]", "Draws ship model as if in techroom", "boolean", "Whether ship was rendered")
{
	int x1,y1,x2,y2;
	int idx;
	float zoom = 1.3f;
	matrix_h *mh = NULL;
	if(!ade_get_args(L, "oiiiio|f", l_Shipclass.Get(&idx), &x1, &y1, &x2, &y2,  l_Matrix.GetPtr(&mh), &zoom))
		return ade_set_error(L, "b", false);

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_args(L, "b", false);

	if(x2 < x1 || y2 < y1)
		return ade_set_args(L, "b", false);

	ship_info *sip = &Ship_info[idx];
	model_render_params render_info;

	if (sip->uses_team_colors) {
		render_info.set_team_color(sip->default_team_name, "none", 0, 0);
	}

	//Make sure model is loaded
	sip->model_num = model_load(sip->pof_file, sip->n_subsystems, &sip->subsystems[0], 0);

	if(sip->model_num < 0)
		return ade_set_args(L, "b", false);

	//Handle angles
	matrix *orient = mh->GetMatrix();

	//Clip
	gr_set_clip(x1,y1,x2-x1,y2-y1,GR_RESIZE_NONE);

	//Handle 3D init stuff
	g3_start_frame(1);
	g3_set_view_matrix(&sip->closeup_pos, &vmd_identity_matrix, sip->closeup_zoom * zoom);

	gr_set_proj_matrix( Proj_fov, gr_screen.clip_aspect, Min_draw_distance, Max_draw_distance);
	gr_set_view_matrix(&Eye_position, &Eye_matrix);

	//Handle light
	light_reset();
	vec3d light_dir = vmd_zero_vector;
	light_dir.xyz.y = 1.0f;	
	light_add_directional(&light_dir, 0.65f, 1.0f, 1.0f, 1.0f);
	light_rotate_all();

	//Draw the ship!!
	model_clear_instance(sip->model_num);
	render_info.set_detail_level_lock(0);

	uint render_flags = MR_AUTOCENTER | MR_NO_FOGGING;

	if(sip->flags[Ship::Info_Flags::No_lighting])
		render_flags |= MR_NO_LIGHTING;

	render_info.set_flags(render_flags);

	model_render_immediate(&render_info, sip->model_num, orient, &vmd_zero_vector);

	//OK we're done
	gr_end_view_matrix();
	gr_end_proj_matrix();

	//Bye!!
	g3_end_frame();
	gr_reset_clip();

	return ade_set_args(L, "b", true);
}

ADE_FUNC(isModelLoaded, l_Shipclass, "[boolean Load = false]", "Checks if the model used for this shipclass is loaded or not and optionally loads the model, which might be a slow operation.", "boolean", "If the model is loaded or not") 
{
	int idx;
	bool load_check = false;
	if(!ade_get_args(L, "o|b", l_Shipclass.Get(&idx), &load_check))
		return ADE_RETURN_FALSE;

	ship_info *sip = &Ship_info[idx];

	if (sip == NULL)
		return ADE_RETURN_FALSE;

	if(load_check){
		sip->model_num = model_load(sip->pof_file, sip->n_subsystems, &sip->subsystems[0]);	
	}

	if (sip->model_num > -1)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(getShipClassIndex, l_Shipclass, NULL, "Gets the index valus of the ship class", "number", "index value of the ship class")
{
	int idx;
	if(!ade_get_args(L, "o", l_Shipclass.Get(&idx)))
		return ade_set_args(L, "i", -1);

	if(idx < 0 || idx >= static_cast<int>(Ship_info.size()))
		return ade_set_args(L, "i", -1);

	return ade_set_args(L, "i", idx + 1); // Lua is 1-based
}

//**********HANDLE: Debris
ade_obj<object_h> l_Debris("debris", "Debris handle", &l_Object);

ADE_VIRTVAR(IsHull, l_Debris, "boolean", "Whether or not debris is a piece of hull", "boolean", "Whether debris is a hull fragment, or false if handle is invalid")
{
	object_h *oh;
	bool b=false;
	if(!ade_get_args(L, "o|b", l_Debris.GetPtr(&oh), &b))
		return ade_set_error(L, "b", false);

	if(!oh->IsValid())
		return ade_set_error(L, "b", false);

	debris *db = &Debris[oh->objp->instance];

	if(ADE_SETTING_VAR) {
		db->is_hull = b ? 1 : 0;
	}

	return ade_set_args(L, "b", db->is_hull ? true : false);

}

ADE_VIRTVAR(OriginClass, l_Debris, "shipclass", "The shipclass of the ship this debris originates from", "shipclass", "The shipclass of the ship that created this debris")
{
	object_h *oh;
	int shipIdx = -1;
	if(!ade_get_args(L, "o|o", l_Debris.GetPtr(&oh), &shipIdx))
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	debris *db = &Debris[oh->objp->instance];

	if(ADE_SETTING_VAR) {
		if (shipIdx >= 0 && shipIdx < static_cast<int>(Ship_info.size()))
			db->ship_info_index = shipIdx;
	}

	return ade_set_error(L, "o", l_Shipclass.Set(db->ship_info_index));
}

ADE_FUNC(getDebrisRadius, l_Debris, NULL, "The radius of this debris piece", "number", "The radius of this debris piece or -1 if invalid")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_Debris.GetPtr(&oh)))
		return ade_set_error(L, "f", -1.0f);

	if(!oh->IsValid())
		return ade_set_error(L, "f", -1.0f);

	debris *db = &Debris[oh->objp->instance];

	polymodel *pm = model_get(db->model_num);

	if (pm == NULL)
		return ade_set_error(L, "f", -1.0f);

	if (db->submodel_num < 0 || pm->n_models <= db->submodel_num)
		return ade_set_error(L, "f", -1.0f);

	return ade_set_error(L, "f", pm->submodel[db->submodel_num].rad);
}

ADE_FUNC(isValid, l_Debris, NULL, "Return if this debris handle is valid", "boolean", "true if valid false otherwise")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_Debris.GetPtr(&oh)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", oh != NULL && oh->IsValid());
}

//**********HANDLE: Waypoint
ade_obj<object_h> l_Waypoint("waypoint", "waypoint handle", &l_Object);

//**********HANDLE: WaypointList
struct waypointlist_h
{
	waypoint_list *wlp;
	char name[NAME_LENGTH];
	waypointlist_h(){wlp=NULL;name[0]='\0';}
	waypointlist_h(waypoint_list *n_wlp){
		wlp = n_wlp;
		if(n_wlp != NULL)
			strcpy_s(name, wlp->get_name());
	}
	waypointlist_h(char wlname[NAME_LENGTH]) {
		wlp = NULL;
		if ( wlname != NULL ) {
			strcpy_s(name, wlname);
			wlp = find_matching_waypoint_list(wlname);
		}
	}
	bool IsValid() {
		return (wlp != NULL && !strcmp(wlp->get_name(), name));
	}
};

ade_obj<waypointlist_h> l_WaypointList("waypointlist", "waypointlist handle");

ADE_INDEXER(l_WaypointList, "number Index", "Array of waypoints that are part of the waypoint list", "waypoint", "Waypoint, or invalid handle if the index or waypointlist handle is invalid")
{
	int idx = -1;
	waypointlist_h* wlh = NULL;
	char wpname[128];
	if( !ade_get_args(L, "oi", l_WaypointList.GetPtr( &wlh ), &idx))
		return ade_set_error( L, "o", l_Waypoint.Set( object_h() ) );

	if(!wlh->IsValid())
		return ade_set_error( L, "o", l_Waypoint.Set( object_h() ) );

	//Lua-->FS2
	idx--;

	//Get waypoint name
	sprintf(wpname, "%s:%d", wlh->wlp->get_name(), calc_waypoint_index(idx) + 1);
	waypoint *wpt = find_matching_waypoint( wpname );
	if( (idx >= 0) && ((uint) idx < wlh->wlp->get_waypoints().size()) && (wpt != NULL) && (wpt->get_objnum() >= 0) ) {
		return ade_set_args( L, "o", l_Waypoint.Set( object_h( &Objects[wpt->get_objnum()] ), Objects[wpt->get_objnum()].signature ) );
	}

	return ade_set_error(L, "o", l_Waypoint.Set( object_h() ) );
}

ADE_FUNC(__len, l_WaypointList,
		 NULL,
		 "Number of waypoints in the list. "
		 "Note that the value returned cannot be relied on for more than one frame.",
		 "number",
		 "Number of waypoints in the list, or 0 if handle is invalid")
{
	waypointlist_h* wlh = NULL;
	if ( !ade_get_args(L, "o", l_WaypointList.GetPtr(&wlh)) ) {
		return ade_set_error( L, "o", l_Waypoint.Set( object_h() ) );
	}
	return ade_set_args(L, "i", wlh->wlp->get_waypoints().size());
}

ADE_VIRTVAR(Name, l_WaypointList, "string", "Name of WaypointList", "string", "Waypointlist name, or empty string if handle is invalid")
{
	waypointlist_h* wlh = NULL;
	char *s = NULL;
	if ( !ade_get_args(L, "o|s", l_WaypointList.GetPtr(&wlh), &s) ) {
		return ade_set_error(L, "s", "");
	}

	if(ADE_SETTING_VAR && s != NULL) {
		wlh->wlp->set_name(s);
		strcpy_s(wlh->name,s);
	}

	return ade_set_args( L, "s", wlh->name);
}

ADE_FUNC(isValid, l_WaypointList, NULL, "Return if this waypointlist handle is valid", "boolean", "true if valid false otherwise")
{
	waypointlist_h* wlh = NULL;
	if ( !ade_get_args(L, "o", l_WaypointList.GetPtr(&wlh)) ) {
		return ADE_RETURN_FALSE;
	}
	return ade_set_args(L, "b", wlh != NULL && wlh->IsValid());
}

//WMC - Waypoints are messed. Gonna leave this for later.
/*
ade_lib l_WaypointList_Waypoints("Waypoints", &l_Waypoint, NULL, NULL);

ADE_INDEXER(l_WaypointList_Waypoints, "waypoint index", "waypoint", "Gets waypoint handle")
{
	waypoint_list *wlp = NULL;
	int idx;
	if(!ade_get_args(L, "o|i", &idx))
		return ade_set_error(L, "o", l_Waypoint.Set(object_h()));

	//Remember, Lua indices start at 0.
	int count=0;

	object *ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list))
	{
		if (ptr->type == OBJ_WAYPOINT)
			count++;

		if(count == idx) {
			return ade_set_args(L, "o", l_Waypoint.Set(object_h(ptr)));
		}

		ptr = GET_NEXT(ptr);
	}

	return ade_set_error(L, "o", l_Weapon.Set(object_h()));
}

ADE_FUNC(__len, l_WaypointList_Waypoints, NULL, "Number of waypoints in the mission",
		 "Gets number of waypoints in mission. Note that this is only accurate for one frame.")
{
	int count=0;
	for(int i = 0; i < MAX_OBJECTS; i++)
	{
		if (Objects[i].type == OBJ_WAYPOINT)
			count++;
	}

	return ade_set_args(L, "i", count);
}*/

ADE_FUNC(getList, l_Waypoint, NULL, "Returns the waypoint list", "waypointlist", "waypointlist handle or invalid handle if waypoint was invalid")
{
	object_h *oh = NULL;
	waypointlist_h wpl;
	waypoint_list *wp_list = NULL;
	if(!ade_get_args(L, "o", l_Waypoint.GetPtr(&oh)))
		return ade_set_error(L, "o", l_WaypointList.Set(waypointlist_h()));

	if(oh->IsValid() && oh->objp->type == OBJ_WAYPOINT) {
		wp_list = find_waypoint_list_with_instance(oh->objp->instance);
		if(wp_list != NULL)
			wpl = waypointlist_h(wp_list);
	}

	if (wpl.IsValid()) {
		return ade_set_args(L, "o", l_WaypointList.Set(wpl));
	}

	return ade_set_error(L, "o", l_WaypointList.Set(waypointlist_h()));
}

//**********HANDLE: Weaponbank
#define SWH_NONE		0
#define SWH_PRIMARY		1
#define SWH_SECONDARY	2
#define SWH_TERTIARY	3

struct ship_banktype_h : public object_h
{
	int type;
	ship_weapon *sw;

	ship_banktype_h() : object_h () {
		sw = NULL;
		type = SWH_NONE;
	}
	ship_banktype_h(object *objp_in, ship_weapon *wpn, int in_type) : object_h(objp_in) {
		sw = wpn;
		type = in_type;
	}

	bool IsValid()
	{
		return object_h::IsValid() && sw != NULL && (type == SWH_PRIMARY || type == SWH_SECONDARY || type == SWH_TERTIARY);
	}
};
struct ship_bank_h : public ship_banktype_h
{
	int bank;

	ship_bank_h() : ship_banktype_h() {
		bank = -1;
	}
	ship_bank_h(object *objp_in, ship_weapon *wpn, int in_type, int in_bank) : ship_banktype_h(objp_in, wpn, in_type) {
		bank = in_bank;
	}

	bool IsValid()
	{
		if(!ship_banktype_h::IsValid())
			return false;

		if(bank < 0)
			return false;

		if(type == SWH_PRIMARY && bank >= sw->num_primary_banks)
			return false;
		if(type == SWH_SECONDARY && bank >= sw->num_secondary_banks)
			return false;
		if(type == SWH_TERTIARY && bank >= sw->num_tertiary_banks)
			return false;

		return true;
	}
};

//**********HANDLE: Ship bank
ade_obj<ship_bank_h> l_WeaponBank("weaponbank", "Ship/subystem weapons bank handle");

ADE_VIRTVAR(WeaponClass, l_WeaponBank, "weaponclass", "Class of weapon mounted in the bank", "weaponclass", "Weapon class, or an invalid weaponclass handle if bank handle is invalid")
{
	ship_bank_h *bh = NULL;
	int weaponclass=-1;
	if(!ade_get_args(L, "o|o", l_WeaponBank.GetPtr(&bh), l_Weaponclass.Get(&weaponclass)))
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	if(!bh->IsValid())
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR && weaponclass > -1) {
				bh->sw->primary_bank_weapons[bh->bank] = weaponclass;
			}

			return ade_set_args(L, "o", l_Weaponclass.Set(bh->sw->primary_bank_weapons[bh->bank]));
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR && weaponclass > -1) {
				bh->sw->secondary_bank_weapons[bh->bank] = weaponclass;
			}

			return ade_set_args(L, "o", l_Weaponclass.Set(bh->sw->secondary_bank_weapons[bh->bank]));
		case SWH_TERTIARY:
			if(ADE_SETTING_VAR && weaponclass > -1) {
				//bh->sw->tertiary_bank_weapons[bh->bank] = weaponclass;
			}

			// return ade_set_args(L, "o", l_Weaponclass.Set(bh->sw->tertiary_bank_weapons[bh->bank]));
			// Error(LOCATION, "Tertiary bank support is still in progress");
			// WMC: TODO
			return ADE_RETURN_FALSE;
	}

	return ade_set_error(L, "o", l_Weaponclass.Set(-1));
}

ADE_VIRTVAR(AmmoLeft, l_WeaponBank, "number", "Ammo left for the current bank", "number", "Ammo left, or 0 if handle is invalid")
{
	ship_bank_h *bh = NULL;
	int ammo;
	if(!ade_get_args(L, "o|i", l_WeaponBank.GetPtr(&bh), &ammo))
		return ade_set_error(L, "i", 0);

	if(!bh->IsValid())
		return ade_set_error(L, "i", 0);

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR && ammo > -1) {
				bh->sw->primary_bank_ammo[bh->bank] = ammo;
			}

			return ade_set_args(L, "i", bh->sw->primary_bank_ammo[bh->bank]);
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR && ammo > -1) {
				bh->sw->secondary_bank_ammo[bh->bank] = ammo;
			}

			return ade_set_args(L, "i", bh->sw->secondary_bank_ammo[bh->bank]);
		case SWH_TERTIARY:
			if(ADE_SETTING_VAR && ammo > -1) {
				bh->sw->tertiary_bank_ammo = ammo;
			}
			return ade_set_args(L, "i", bh->sw->tertiary_bank_ammo);
	}

	return ade_set_error(L, "i", 0);
}

ADE_VIRTVAR(AmmoMax, l_WeaponBank, "number", "Maximum ammo for the current bank<br>"
			"<b>Note:</b> Setting this value actually sets the <i>capacity</i> of the weapon bank. To set the actual maximum ammunition use <tt>AmmoMax = <amount> * class.CargoSize</tt>", "number", "Ammo capacity, or 0 if handle is invalid")
{
	ship_bank_h *bh = NULL;
	int ammomax;
	if(!ade_get_args(L, "o|i", l_WeaponBank.GetPtr(&bh), &ammomax))
		return ade_set_error(L, "i", 0);

	if(!bh->IsValid())
		return ade_set_error(L, "i", 0);

	switch(bh->type)
	{
		case SWH_PRIMARY:
			{
				if(ADE_SETTING_VAR && ammomax > -1) {
					bh->sw->primary_bank_capacity[bh->bank] = ammomax;
				}

				int weapon_class = bh->sw->primary_bank_weapons[bh->bank];

				Assert(bh->objp->type == OBJ_SHIP);

				return ade_set_args(L, "i", get_max_ammo_count_for_primary_bank(Ships[bh->objp->instance].ship_info_index, bh->bank, weapon_class));
			}
		case SWH_SECONDARY:
			{
				if(ADE_SETTING_VAR && ammomax > -1) {
					bh->sw->secondary_bank_capacity[bh->bank] = ammomax;
				}

				int weapon_class = bh->sw->secondary_bank_weapons[bh->bank];

				Assert(bh->objp->type == OBJ_SHIP);

				return ade_set_args(L, "i", get_max_ammo_count_for_bank(Ships[bh->objp->instance].ship_info_index, bh->bank, weapon_class));
			}
		case SWH_TERTIARY:
			if(ADE_SETTING_VAR && ammomax > -1) {
				bh->sw->tertiary_bank_capacity = ammomax;
			}

			return ade_set_args(L, "i", bh->sw->tertiary_bank_capacity);
	}

	return ade_set_error(L, "i", 0);
}

ADE_VIRTVAR(Armed, l_WeaponBank, "boolean", "Weapon armed status. Does not take linking into account.", "boolean", "True if armed, false if unarmed or handle is invalid")
{
	ship_bank_h *bh = NULL;
	bool armthis=false;
	if(!ade_get_args(L, "o|b", l_WeaponBank.GetPtr(&bh), &armthis))
		return ade_set_error(L, "b", false);

	if(!bh->IsValid())
		return ade_set_error(L, "b", false);

	int new_armed_bank = -1;
	if(armthis)
		new_armed_bank = bh->bank;

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR) {
				bh->sw->current_primary_bank = new_armed_bank;
			}
			return ade_set_args(L, "b", bh->sw->current_primary_bank == bh->bank);
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR) {
				bh->sw->current_secondary_bank = new_armed_bank;
			}
			return ade_set_args(L, "b", bh->sw->current_secondary_bank == bh->bank);
		case SWH_TERTIARY:
			if(ADE_SETTING_VAR) {
				bh->sw->current_tertiary_bank = new_armed_bank;
			}
			return ade_set_args(L, "b", bh->sw->current_tertiary_bank == bh->bank);
	}

	return ade_set_error(L, "b", false);
}

ADE_VIRTVAR(Capacity, l_WeaponBank, "number", "The actual capacity of a weapon bank as specified in the table", "number", "The capacity or -1 if handle is invalid")
{
	ship_bank_h *bh = NULL;
	int newCapacity = -1;
	if(!ade_get_args(L, "o|i", l_WeaponBank.GetPtr(&bh), &newCapacity))
		return ade_set_error(L, "i", -1);

	if(!bh->IsValid())
		return ade_set_error(L, "i", -1);

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR && newCapacity > 0) {
				bh->sw->primary_bank_capacity[bh->bank] = newCapacity;
			}
			return ade_set_args(L, "i", bh->sw->primary_bank_capacity[bh->bank]);
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR && newCapacity > 0) {
				bh->sw->secondary_bank_capacity[bh->bank] = newCapacity;
			}
			return ade_set_args(L, "i", bh->sw->secondary_bank_capacity[bh->bank]);
		case SWH_TERTIARY:
			if(ADE_SETTING_VAR && newCapacity > 0) {
				bh->sw->tertiary_bank_capacity = newCapacity;
			}
			return ade_set_args(L, "i", bh->sw->tertiary_bank_capacity);
	}
	
	return ade_set_error(L, "i", -1);
}

ADE_VIRTVAR(FOFCooldown, l_WeaponBank, "number", "The FOF cooldown value. A value of 0 means the default weapon FOF is used. A value of 1 means that the max FOF will be used", "number", "The cooldown value or -1 if invalid")
{
	ship_bank_h *bh = NULL;
	float newValue = -1.f;
	if(!ade_get_args(L, "o|i", l_WeaponBank.GetPtr(&bh), &newValue))
		return ade_set_error(L, "f", -1.f);

	if(!bh->IsValid())
		return ade_set_error(L, "f", -1.f);

	if (ADE_SETTING_VAR) {
		LuaError(L, "This function does not support setting values yet!");
		return ade_set_error(L, "f", -1.f);
	}

	switch(bh->type)
	{
		case SWH_PRIMARY:
		{
			auto wif = &Weapon_info[bh->sw->primary_bank_weapons[bh->bank]];
			float reset_amount = (timestamp_until(bh->sw->last_primary_fire_stamp[bh->bank]) / 1000.0f) * wif->fof_reset_rate;
			auto val = bh->sw->primary_bank_fof_cooldown[bh->bank] + reset_amount;
			CLAMP(val, 0.0f, 1.0f);
			return ade_set_args(L, "f", val);
		}
		case SWH_SECONDARY:
		case SWH_TERTIARY:
			LuaError(L, "FOF cooldown is not valid for secondary or tertiary banks!");
			return ade_set_args(L, "f", -1.f);
	}

	return ade_set_error(L, "f", -1.f);
}

ADE_FUNC(isValid, l_WeaponBank, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	ship_bank_h *bh;
	if(!ade_get_args(L, "o", l_WeaponBank.GetPtr(&bh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", bh->IsValid());
}

//**********HANDLE: Weaponbanktype
ade_obj<ship_banktype_h> l_WeaponBankType("weaponbanktype", "Ship/subsystem weapons bank type handle");

ADE_INDEXER(l_WeaponBankType, "number Index", "Array of weapon banks", "weaponbank", "Weapon bank, or invalid handle on failure")
{
	ship_banktype_h *sb=NULL;
	int idx = -1;
	ship_bank_h *newbank = nullptr;
	if(!ade_get_args(L, "oi|o", l_WeaponBankType.GetPtr(&sb), &idx, l_WeaponBank.GetPtr(&newbank)))
		return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

	if(!sb->IsValid())
		return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

	switch(sb->type)
	{
		case SWH_PRIMARY:
				if(idx < 1 || idx > sb->sw->num_primary_banks)
					return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

				idx--; //Lua->FS2

				if(ADE_SETTING_VAR && newbank && newbank->IsValid()) {
					sb->sw->primary_bank_weapons[idx] = newbank->sw->primary_bank_weapons[idx];
					sb->sw->next_primary_fire_stamp[idx] = timestamp(0);
					sb->sw->primary_bank_ammo[idx] = newbank->sw->primary_bank_ammo[idx];
					sb->sw->primary_bank_start_ammo[idx] = newbank->sw->primary_bank_start_ammo[idx];
					sb->sw->primary_bank_capacity[idx] = newbank->sw->primary_bank_capacity[idx];
					sb->sw->primary_bank_rearm_time[idx] = timestamp(0);
				}
				break;
		case SWH_SECONDARY:
				if(idx < 1 || idx > sb->sw->num_secondary_banks)
					return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

				idx--; //Lua->FS2

				if(ADE_SETTING_VAR && newbank && newbank->IsValid()) {
					sb->sw->primary_bank_weapons[idx] = newbank->sw->primary_bank_weapons[idx];
					sb->sw->next_primary_fire_stamp[idx] = timestamp(0);
					sb->sw->primary_bank_ammo[idx] = newbank->sw->primary_bank_ammo[idx];
					sb->sw->primary_bank_start_ammo[idx] = newbank->sw->primary_bank_start_ammo[idx];
					sb->sw->primary_bank_capacity[idx] = newbank->sw->primary_bank_capacity[idx];
				}
				break;
		case SWH_TERTIARY:
				if(idx < 1 || idx > sb->sw->num_tertiary_banks)
					return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));

				idx--; //Lua->FS2

				if(ADE_SETTING_VAR && newbank && newbank->IsValid()) {
					Error(LOCATION, "Tertiary bank support is still in progress");
					//WMC: TODO
				}
				break;
		default:
			return ade_set_error(L, "o", l_WeaponBank.Set(ship_bank_h()));	//Invalid type
	}

	return ade_set_args(L, "o", l_WeaponBank.Set(ship_bank_h(sb->objp, sb->sw, sb->type, idx)));
}

ADE_VIRTVAR(Linked, l_WeaponBankType, "boolean", "Whether bank is in linked or unlinked fire mode (Primary-only)", "boolean", "Link status, or false if handle is invalid")
{
	ship_banktype_h *bh;
	bool newlink = false;
	int numargs = ade_get_args(L, "o|b", l_WeaponBankType.GetPtr(&bh), &newlink);

	if(!numargs)
		return ade_set_error(L, "b", false);

	if(!bh->IsValid())
		return ade_set_error(L, "b", false);

	switch(bh->type)
	{
		case SWH_PRIMARY:
			if(ADE_SETTING_VAR && numargs > 1) {
				Ships[bh->objp->instance].flags.set(Ship::Ship_Flags::Primary_linked, newlink);
			}

			return ade_set_args(L, "b", (Ships[bh->objp->instance].flags[Ship::Ship_Flags::Primary_linked]));

		case SWH_SECONDARY:
		case SWH_TERTIARY:
			return ADE_RETURN_FALSE;
	}

	return ade_set_error(L, "b", false);
}

ADE_VIRTVAR(DualFire, l_WeaponBankType, "boolean", "Whether bank is in dual fire mode (Secondary-only)", "boolean", "Dual fire status, or false if handle is invalid")
{
	ship_banktype_h *bh;
	bool newfire = false;
	int numargs = ade_get_args(L, "o|b", l_WeaponBankType.GetPtr(&bh), &newfire);

	if(!numargs)
		return ade_set_error(L, "b", false);

	if(!bh->IsValid())
		return ade_set_error(L, "b", false);

	switch(bh->type)
	{
		case SWH_SECONDARY:
			if(ADE_SETTING_VAR && numargs > 1) {
                Ships[bh->objp->instance].flags.set(Ship::Ship_Flags::Secondary_dual_fire, newfire);
			}

			return ade_set_args(L, "b", (Ships[bh->objp->instance].flags[Ship::Ship_Flags::Secondary_dual_fire]));

		case SWH_PRIMARY:
		case SWH_TERTIARY:
			return ADE_RETURN_FALSE;
	}

	return ade_set_error(L, "b", false);
}

ADE_FUNC(isValid, l_WeaponBankType, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	ship_banktype_h *sb;
	if(!ade_get_args(L, "o", l_WeaponBankType.GetPtr(&sb)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", sb->IsValid());
}

ADE_FUNC(__len, l_WeaponBankType, NULL, "Number of weapons in the mounted bank", "number", "Number of bank weapons, or 0 if handle is invalid")
{
	ship_banktype_h *sb=NULL;
	if(!ade_get_args(L, "o", l_WeaponBankType.GetPtr(&sb)))
		return ade_set_error(L, "i", 0);

	if(!sb->IsValid())
		return ade_set_error(L, "i", 0);

	switch(sb->type)
	{
		case SWH_PRIMARY:
			return ade_set_args(L, "i", sb->sw->num_primary_banks);
		case SWH_SECONDARY:
			return ade_set_args(L, "i", sb->sw->num_secondary_banks);
		case SWH_TERTIARY:
			return ade_set_args(L, "i", sb->sw->num_tertiary_banks);
		default:
			return ade_set_error(L, "i", 0);	//Invalid type
	}
}

//**********HANDLE: Subsystem
struct ship_subsys_h : public object_h
{
	ship_subsys *ss;	//Pointer to subsystem, or NULL for the hull

	bool IsValid(){return object_h::IsValid() && objp->type == OBJ_SHIP && ss != NULL;}
	ship_subsys_h() : object_h() {
		ss = NULL;
	}
	ship_subsys_h(object *objp_in, ship_subsys *sub) : object_h(objp_in) {
		ss = sub;
	}
};
ade_obj<ship_subsys_h> l_Subsystem("subsystem", "Ship subsystem handle");

ADE_FUNC(__tostring, l_Subsystem, NULL, "Returns name of subsystem", "string", "Subsystem name, or empty string if handle is invalid")
{
	ship_subsys_h *sso;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "s", "");

	if(!sso->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", ship_subsys_get_name(sso->ss));
}

ADE_VIRTVAR(ArmorClass, l_Subsystem, "string", "Current Armor class", "string", "Armor class name, or empty string if none is set")
{
	ship_subsys_h *sso;
	char *s = NULL;
	char *name = NULL;
	
	if(!ade_get_args(L, "o|s", l_Subsystem.GetPtr(&sso), &s))
		return ade_set_error(L, "s", "");

	if(!sso->IsValid())
		return ade_set_error(L, "s", "");

	ship_subsys *ssys = sso->ss;

	int atindex = -1;
	if (ADE_SETTING_VAR && s != NULL) {
		atindex = armor_type_get_idx(s);
		ssys->armor_type_idx = atindex;
	}

	if (atindex != -1)
		name = Armor_types[atindex].GetNamePtr();
	else
		name = "";

	return ade_set_args(L, "s", name);
}

ADE_VIRTVAR(AWACSIntensity, l_Subsystem, "number", "Subsystem AWACS intensity", "number", "AWACS intensity, or 0 if handle is invalid")
{
	ship_subsys_h *sso;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!sso->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && f >= 0.0f)
		sso->ss->awacs_intensity = f;

	return ade_set_args(L, "f", sso->ss->awacs_intensity);
}

ADE_VIRTVAR(AWACSRadius, l_Subsystem, "number", "Subsystem AWACS radius", "number", "AWACS radius, or 0 if handle is invalid")
{
	ship_subsys_h *sso;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!sso->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && f >= 0.0f)
		sso->ss->awacs_radius = f;

	return ade_set_args(L, "f", sso->ss->awacs_radius);
}

ADE_VIRTVAR(Orientation, l_Subsystem, "orientation", "Orientation of subobject or turret base", "orientation", "Subsystem orientation, or null orientation if handle is invalid")
{
	ship_subsys_h *sso;
	matrix_h *mh = nullptr;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	if(ADE_SETTING_VAR && mh)
	{
		sso->ss->submodel_info_1.angs = *mh->GetAngles();
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&sso->ss->submodel_info_1.angs)));
}

ADE_VIRTVAR(GunOrientation, l_Subsystem, "orientation", "Orientation of turret gun", "orientation", "Gun orientation, or null orientation if handle is invalid")
{
	ship_subsys_h *sso;
	matrix_h *mh = nullptr;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));

	if(ADE_SETTING_VAR && mh)
	{
		sso->ss->submodel_info_2.angs = *mh->GetAngles();
	}

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&sso->ss->submodel_info_2.angs)));
}

ADE_VIRTVAR(HitpointsLeft, l_Subsystem, "number", "Subsystem hitpoints left", "number", "Hitpoints left, or 0 if handle is invalid. Setting a value of 0 will disable it - set a value of -1 or lower to actually blow it up.")
{
	ship_subsys_h *sso;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!sso->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR)
	{
		//Only go down to 0 hits
		sso->ss->current_hits = MAX(0.0f, f);

		ship *shipp = &Ships[sso->objp->instance];
		if (f <= -1.0f && sso->ss->current_hits <= 0.0f) {
			do_subobj_destroyed_stuff(shipp, sso->ss, NULL);
		}
		ship_recalc_subsys_strength(shipp);
	}

	return ade_set_args(L, "f", sso->ss->current_hits);
}

ADE_VIRTVAR(HitpointsMax, l_Subsystem, "number", "Subsystem hitpoints max", "number", "Max hitpoints, or 0 if handle is invalid")
{
	ship_subsys_h *sso;
	float f = -1.0f;
	if(!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!sso->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR)
	{
		sso->ss->max_hits = MIN(0.0f, f);

		ship_recalc_subsys_strength(&Ships[sso->objp->instance]);
	}

	return ade_set_args(L, "f", sso->ss->max_hits);
}

ADE_VIRTVAR(Position, l_Subsystem, "vector", "Subsystem position with regards to main ship (Local Vector)", "vector", "Subsystem position, or null vector if subsystem handle is invalid")
{
	ship_subsys_h *sso;
	vec3d *v = NULL;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && v != NULL)
	{
		sso->ss->system_info->pnt = *v;
	}

	return ade_set_args(L, "o", l_Vector.Set(sso->ss->system_info->pnt));
}

ADE_VIRTVAR(GunPosition, l_Subsystem, "vector", "Subsystem gun position with regards to main ship (Local vector)", "vector", "Gun position, or null vector if subsystem handle is invalid")
{
	ship_subsys_h *sso;
	vec3d *v = NULL;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Vector.GetPtr(&v)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	polymodel *pm = model_get(Ship_info[Ships[sso->objp->instance].ship_info_index].model_num);
	Assert(pm != NULL);

	if(sso->ss->system_info->turret_gun_sobj < 0)
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	bsp_info *sm = &pm->submodel[sso->ss->system_info->turret_gun_sobj];

	if(ADE_SETTING_VAR && v != NULL)
		sm->offset = *v;

	return ade_set_args(L, "o", l_Vector.Set(sm->offset));
}

ADE_VIRTVAR(Name, l_Subsystem, "string", "Subsystem name", "string", "Subsystem name, or an empty string if handle is invalid")
{
	ship_subsys_h *sso;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Subsystem.GetPtr(&sso), &s))
		return ade_set_error(L, "s", "");

	if(!sso->IsValid())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL && strlen(s))
	{
		ship_subsys_set_name(sso->ss, s);
	}

	return ade_set_args(L, "s", ship_subsys_get_name(sso->ss));
}

ADE_FUNC(getModelName, l_Subsystem, NULL, "Returns the original name of the subsystem in the model file", "string", "name or empty string on error")
{
	ship_subsys_h *sso;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "s", "");

	if(!sso->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", sso->ss->system_info->subobj_name);
}

ADE_VIRTVAR(PrimaryBanks, l_Subsystem, "weaponbanktype", "Array of primary weapon banks", "weaponbanktype", "Primary banks, or invalid weaponbanktype handle if subsystem handle is invalid")
{
	ship_subsys_h *sso, *sso2 = nullptr;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Subsystem.GetPtr(&sso2)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &sso->ss->weapons;

	if(ADE_SETTING_VAR && sso2 && sso2->IsValid()) {
		ship_weapon *src = &sso2->ss->weapons;

		dst->current_primary_bank = src->current_primary_bank;
		dst->num_primary_banks = src->num_primary_banks;

		memcpy(dst->next_primary_fire_stamp, src->next_primary_fire_stamp, sizeof(dst->next_primary_fire_stamp));
		memcpy(dst->primary_animation_done_time, src->primary_animation_done_time, sizeof(dst->primary_animation_done_time));
		memcpy(dst->primary_animation_position, src->primary_animation_position, sizeof(dst->primary_animation_position));
		memcpy(dst->primary_bank_ammo, src->primary_bank_ammo, sizeof(dst->primary_bank_ammo));
		memcpy(dst->primary_bank_capacity, src->primary_bank_capacity, sizeof(dst->primary_bank_capacity));
		memcpy(dst->primary_bank_rearm_time, src->primary_bank_rearm_time, sizeof(dst->primary_bank_rearm_time));
		memcpy(dst->primary_bank_start_ammo, src->primary_bank_start_ammo, sizeof(dst->primary_bank_start_ammo));
		memcpy(dst->primary_bank_weapons, src->primary_bank_weapons, sizeof(dst->primary_bank_weapons));
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(sso->objp, dst, SWH_PRIMARY)));
}

ADE_VIRTVAR(SecondaryBanks, l_Subsystem, "weaponbanktype", "Array of secondary weapon banks", "weaponbanktype", "Secondary banks, or invalid weaponbanktype handle if subsystem handle is invalid")
{
	ship_subsys_h *sso, *sso2 = nullptr;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Subsystem.GetPtr(&sso2)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &sso->ss->weapons;

	if(ADE_SETTING_VAR && sso2 && sso2->IsValid()) {
		ship_weapon *src = &sso2->ss->weapons;

		dst->current_secondary_bank = src->current_secondary_bank;
		dst->num_secondary_banks = src->num_secondary_banks;

		memcpy(dst->next_secondary_fire_stamp, src->next_secondary_fire_stamp, sizeof(dst->next_secondary_fire_stamp));
		memcpy(dst->secondary_animation_done_time, src->secondary_animation_done_time, sizeof(dst->secondary_animation_done_time));
		memcpy(dst->secondary_animation_position, src->secondary_animation_position, sizeof(dst->secondary_animation_position));
		memcpy(dst->secondary_bank_ammo, src->secondary_bank_ammo, sizeof(dst->secondary_bank_ammo));
		memcpy(dst->secondary_bank_capacity, src->secondary_bank_capacity, sizeof(dst->secondary_bank_capacity));
		memcpy(dst->secondary_bank_rearm_time, src->secondary_bank_rearm_time, sizeof(dst->secondary_bank_rearm_time));
		memcpy(dst->secondary_bank_start_ammo, src->secondary_bank_start_ammo, sizeof(dst->secondary_bank_start_ammo));
		memcpy(dst->secondary_bank_weapons, src->secondary_bank_weapons, sizeof(dst->secondary_bank_weapons));
		memcpy(dst->secondary_next_slot, src->secondary_next_slot, sizeof(dst->secondary_next_slot));
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(sso->objp, dst, SWH_SECONDARY)));
}


ADE_VIRTVAR(Target, l_Subsystem, "object", "Object targeted by this subsystem. If used to set a new target, AI targeting will be switched off.", "object", "Targeted object, or invalid object handle if subsystem handle is invalid")
{
	ship_subsys_h *sso;
	object_h *objh = nullptr;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Object.GetPtr(&objh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	ship_subsys *ss = sso->ss;

	if(ADE_SETTING_VAR && objh && objh->IsValid())
	{
		ss->turret_enemy_objnum = OBJ_INDEX(objh->objp);
		ss->turret_enemy_sig = objh->sig;
		ss->targeted_subsys = NULL;
		ss->scripting_target_override = true;
	}

	return ade_set_object_with_breed(L, ss->turret_enemy_objnum);
}

ADE_VIRTVAR(TurretResets, l_Subsystem, "boolean", "Specifies wether this turrets resets after a certain time of inactivity", "boolean", "true if turret resets, false otherwise")
{
	ship_subsys_h *sso;
	bool newVal = false;
	if (!ade_get_args(L, "o|b", l_Subsystem.GetPtr(&sso), &newVal))
		return ADE_RETURN_FALSE;

	if (!sso->IsValid())
		return ADE_RETURN_FALSE;

	if(ADE_SETTING_VAR)
	{
        sso->ss->system_info->flags.set(Model::Subsystem_Flags::Turret_reset_idle, newVal);
	}

	if (sso->ss->system_info->flags[Model::Subsystem_Flags::Turret_reset_idle])
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_VIRTVAR(TurretResetDelay, l_Subsystem, "number", "The time (in milliseconds) after that the turret resets itself", "number", "Reset delay")
{
	ship_subsys_h *sso;
	int newVal = -1;
	if (!ade_get_args(L, "o|i", l_Subsystem.GetPtr(&sso), &newVal))
		return ade_set_error(L, "i", -1);

	if (!sso->IsValid())
		return ade_set_error(L, "i", -1);

	if (!(sso->ss->system_info->flags[Model::Subsystem_Flags::Turret_reset_idle]))
		return ade_set_error(L, "i", -1);

	if(ADE_SETTING_VAR)
	{
		if ((sso->ss->system_info->flags[Model::Subsystem_Flags::Turret_reset_idle]))
			sso->ss->system_info->turret_reset_delay = newVal;
	}

	return ade_set_args(L, "i", sso->ss->system_info->turret_reset_delay);
}

ADE_VIRTVAR(TurnRate, l_Subsystem, "number", "The turn rate", "number", "Turnrate or -1 on error")
{
	ship_subsys_h *sso;
	float newVal = -1.0f;
	if (!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &newVal))
		return ade_set_error(L, "f", -1.0f);

	if (!sso->IsValid())
		return ade_set_error(L, "f", -1.0f);

	if(ADE_SETTING_VAR)
	{
		sso->ss->system_info->turret_turning_rate = newVal;
	}

	return ade_set_args(L, "f", sso->ss->system_info->turret_turning_rate);
}

ADE_VIRTVAR(Targetable, l_Subsystem, "boolean", "Targetability of this subsystem", "boolean", "true if targetable, false otherwise or on error")
{
	ship_subsys_h *sso;
	bool newVal = false;
	if (!ade_get_args(L, "o|b", l_Subsystem.GetPtr(&sso), &newVal))
		return ade_set_error(L, "b", false);

	if (!sso->IsValid())
		return ade_set_error(L, "b", false);

	if(ADE_SETTING_VAR)
	{
        sso->ss->flags.set(Ship::Subsystem_Flags::Untargetable, newVal);
	}

	return ade_set_args(L, "b", !(sso->ss->flags[Ship::Subsystem_Flags::Untargetable]));
}

ADE_VIRTVAR(Radius, l_Subsystem, "number", "The radius of this subsystem", "number", "The radius or 0 on error")
{
	ship_subsys_h *sso;
	if (!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "f", 0.0f);
	
	if (!sso->IsValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR)
	{
		LuaError(L, "Setting radius for subsystems is not allowed!");
	}

	return ade_set_args(L, "f", sso->ss->system_info->radius);
}

ADE_VIRTVAR(TurretLocked, l_Subsystem, "boolean", "Whether the turret is locked. Setting to true locks the turret, setting to false frees it.", "boolean", "True if turret is locked, false otherwise")
{
	ship_subsys_h *sso;
	bool newVal = false;
	if (!ade_get_args(L, "o|b", l_Subsystem.GetPtr(&sso), &newVal))
		return ade_set_error(L, "b", false);

	if (!sso->IsValid())
		return ade_set_error(L, "b", false);

	if(ADE_SETTING_VAR)
	{
		sso->ss->weapons.flags.set(Ship::Weapon_Flags::Turret_Lock, newVal);
	}

	return ade_set_args(L, "b", (sso->ss->weapons.flags[Ship::Weapon_Flags::Turret_Lock]));
}

ADE_VIRTVAR(NextFireTimestamp, l_Subsystem, "number", "The next time the turret may attempt to fire", "number", "Mission time (seconds) or -1 on error")
{
	ship_subsys_h *sso;
	float newVal = -1.0f;
	if (!ade_get_args(L, "o|f", l_Subsystem.GetPtr(&sso), &newVal))
		return ade_set_error(L, "f", -1.0f);

	if (!sso->IsValid())
		return ade_set_error(L, "f", -1.0f);

	if(ADE_SETTING_VAR)
	{
		sso->ss->turret_next_fire_stamp = (int)(newVal * 1000);
	}

	return ade_set_args(L, "f", sso->ss->turret_next_fire_stamp / 1000.0f);
}

ADE_FUNC(targetingOverride, l_Subsystem, "boolean", "If set to true, AI targeting for this turret is switched off. If set to false, the AI will take over again.", "boolean", "Returns true if successful, false otherwise")
{
	bool targetOverride = false;
	ship_subsys_h *sso;
	if(!ade_get_args(L, "ob", l_Subsystem.GetPtr(&sso), &targetOverride))
		return ADE_RETURN_FALSE;

	if(!sso->IsValid())
		return ADE_RETURN_FALSE;

	ship_subsys *ss = sso->ss;

	ss->scripting_target_override = targetOverride;
	return ADE_RETURN_TRUE;
}

ADE_FUNC(hasFired, l_Subsystem, NULL, "Determine if a subsystem has fired", "boolean", "true if if fired, false if not fired, or nil if invalid. resets fired flag when called.")
{
	ship_subsys_h *sso;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ADE_RETURN_NIL;

	if(!sso->IsValid())
		return ADE_RETURN_NIL;

	if(sso->ss->flags[Ship::Subsystem_Flags::Has_fired]){
        sso->ss->flags.remove(Ship::Subsystem_Flags::Has_fired);
        return ADE_RETURN_TRUE;
    }
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(isTurret, l_Subsystem, NULL, "Determines if this subsystem is a turret", "boolean", "true if subsystem is turret, false otherwise or nil on error")
{
	ship_subsys_h *sso;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ADE_RETURN_NIL;

	if (!sso->IsValid())
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", sso->ss->system_info->type == SUBSYSTEM_TURRET);
}

ADE_FUNC(isTargetInFOV, l_Subsystem, "object Target", "Determines if the object is in the turrets FOV", "boolean", "true if in FOV, false if not, nil on error or if subsystem is not a turret ")
{
	ship_subsys_h *sso;
	object_h *newh = nullptr;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Object.GetPtr(&newh)))
		return ADE_RETURN_NIL;

	if (!sso->IsValid() || !newh || !newh->IsValid() || !(sso->ss->system_info->type == SUBSYSTEM_TURRET))
		return ADE_RETURN_NIL;

	vec3d	tpos,tvec;
	ship_get_global_turret_info(sso->objp, sso->ss->system_info, &tpos, &tvec);

	int in_fov = object_in_turret_fov(newh->objp,sso->ss,&tvec,&tpos,vm_vec_dist(&newh->objp->pos,&tpos));

	if (in_fov)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

bool turret_fire_weapon(int weapon_num, ship_subsys *turret, int parent_objnum, vec3d *turret_pos, vec3d *turret_fvec, vec3d *predicted_pos = NULL, float flak_range_override = 100.0f);
ADE_FUNC(fireWeapon, l_Subsystem, "[Turret weapon index = 1, Flak range = 100]", "Fires weapon on turret", NULL, NULL)
{
	ship_subsys_h *sso;
	int wnum = 1;
	float flak_range = 100.0f;
	if(!ade_get_args(L, "o|if", l_Subsystem.GetPtr(&sso), &wnum, &flak_range))
		return ADE_RETURN_NIL;

	if(!sso->IsValid())
		return ADE_RETURN_NIL;

	if (sso->ss->current_hits <= 0)
	{
		return ADE_RETURN_FALSE;
	}
	
	wnum--;	//Lua->FS2

	//Get default turret info
	vec3d gpos, gvec;
	
	ship_get_global_turret_gun_info(sso->objp, sso->ss, &gpos, &gvec, 1, NULL);

	bool rtn = turret_fire_weapon(wnum, sso->ss, OBJ_INDEX(sso->objp), &gpos, &gvec, NULL, flak_range);

	sso->ss->turret_next_fire_pos++;

	return ade_set_args(L, "b", rtn);
}

ADE_FUNC(rotateTurret, l_Subsystem, "vector Pos[, boolean reset=false", "Rotates the turret to face Pos or resets the turret to its original state", "boolean", "true on success false otherwise")
{
	ship_subsys_h *sso;
	vec3d pos = vmd_zero_vector;
	bool reset = false;
	if (!ade_get_args(L, "oo|b", l_Subsystem.GetPtr(&sso), l_Vector.Get(&pos), &reset))
		return ADE_RETURN_NIL;

	//Get default turret info
	vec3d gpos, gvec;
	model_subsystem *tp = sso->ss->system_info;
	object *objp = sso->objp;

	//Rotate turret position with ship
	vm_vec_unrotate(&gpos, &tp->pnt, &sso->objp->orient);

	//Add turret position to appropriate world space
	vm_vec_add2(&gpos, &sso->objp->pos);

	// Find direction of turret
	model_instance_find_world_dir(&gvec, &tp->turret_norm, Ships[objp->instance].model_instance_num, tp->turret_gun_sobj, &objp->orient);
		
	int ret_val = model_rotate_gun(Ship_info[(&Ships[sso->objp->instance])->ship_info_index].model_num, tp, &Objects[sso->objp->instance].orient, &sso->ss->submodel_info_1.angs, &sso->ss->submodel_info_2.angs, &Objects[sso->objp->instance].pos, &pos, (&Ships[sso->objp->instance])->objnum, reset);

	if (ret_val)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(getTurretHeading, l_Subsystem, NULL, "Returns the turrets forward vector", "vector", "Returns a normalized version of the forward vector or null vector on error")
{
	ship_subsys_h *sso;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	
	vec3d gvec;
	object *objp = sso->objp;

	model_instance_find_world_dir(&gvec, &sso->ss->system_info->turret_norm, Ships[objp->instance].model_instance_num, sso->ss->system_info->turret_gun_sobj, &objp->orient);

	vec3d out;
	vm_vec_rotate(&out, &gvec, &sso->objp->orient);

	return ade_set_args(L, "o", l_Vector.Set(out));
}

ADE_FUNC(getFOVs, l_Subsystem, NULL, "Returns current turrets FOVs", "number, number, number", "Standard FOV, maximum barrel elevation, turret base fov.")
{
	ship_subsys_h *sso;
	float fov, fov_e, fov_y;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ADE_RETURN_NIL;

	if(!sso->IsValid())
		return ADE_RETURN_NIL;

	model_subsystem *tp = sso->ss->system_info;

	fov = tp->turret_fov;
	fov_e = tp->turret_max_fov;
	fov_y = tp->turret_y_fov;

	return ade_set_args(L, "fff", fov, fov_e, fov_y);
}

ADE_FUNC(getNextFiringPosition, l_Subsystem, NULL, "Retrieves the next position and firing normal this turret will fire from. This function returns a world position", "vector, vector", "vector or null vector on error")
{
	ship_subsys_h *sso;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "oo", l_Vector.Set(vmd_zero_vector), l_Vector.Set(vmd_zero_vector));
	
	if(!sso->IsValid())
		return ade_set_error(L, "oo", l_Vector.Set(vmd_zero_vector), l_Vector.Set(vmd_zero_vector));

	vec3d gpos, gvec;

	ship_get_global_turret_gun_info(sso->objp, sso->ss, &gpos, &gvec, 1, NULL);

	return ade_set_args(L, "oo", l_Vector.Set(gpos), l_Vector.Set(gvec));
}

ADE_FUNC(getTurretMatrix, l_Subsystem, NULL, "Returns current subsystems turret matrix", "matrix", "Turret matrix.")
{
	ship_subsys_h *sso;
	matrix m;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ADE_RETURN_NIL;
	
	if(!sso->IsValid())
		return ADE_RETURN_NIL;

	model_subsystem *tp = sso->ss->system_info;

	m = tp->turret_matrix;

	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&m)));
}

ADE_FUNC(getParent, l_Subsystem, NULL, "The object parent of this subsystem, is of type ship", "object", "object handle or invalid handle on error")
{
	ship_subsys_h *sso = NULL;
	object_h *objhp = NULL;
	if(!ade_get_args(L, "o|o", l_Subsystem.GetPtr(&sso), l_Ship.GetPtr(&objhp)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!sso->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	return ade_set_args(L, "o", l_Object.Set(object_h(sso->objp)));
}

ADE_FUNC(isValid, l_Subsystem, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	ship_subsys_h *sso;
	if(!ade_get_args(L, "o", l_Subsystem.GetPtr(&sso)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", sso->IsValid());
}

//**********HANDLE: order
struct order_h
{
	object_h objh;
	int odx;
	int sig;
	ai_goal *aigp;

	order_h() {
		objh = object_h();
		odx = -1;
		sig = -1;
		aigp = NULL;
	}

	order_h(object *objp, int n_odx)
	{
		objh = object_h(objp);
		if(objh.IsValid() && objh.objp->type == OBJ_SHIP && n_odx > -1 && n_odx < MAX_AI_GOALS)
		{
			odx = n_odx;
			sig = Ai_info[Ships[objh.objp->instance].ai_index].goals[odx].signature;
			aigp = &Ai_info[Ships[objh.objp->instance].ai_index].goals[odx];
		}
		else
		{
			odx = -1;
			sig = -1;
			aigp = NULL;
		}
	}

	bool IsValid()
	{
		if (objh.objp == NULL || aigp == NULL)
			return false;

		return objh.IsValid() && objh.objp->type == OBJ_SHIP && odx > -1 && odx < MAX_AI_GOALS && sig == Ai_info[Ships[objh.objp->instance].ai_index].goals[odx].signature;
	}
};

ade_obj<order_h> l_Order("order", "order handle");

ADE_VIRTVAR(Priority, l_Order, "number", "Priority of the given order", "number", "Order priority or 0 if invalid")
{
	order_h *ohp = NULL;
	int priority = 1;

	if(!ade_get_args(L, "o|i", l_Order.GetPtr(&ohp), &priority))
		return ade_set_error(L, "i", 0);

	if(!ohp->IsValid())
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR) {
		ohp->aigp->priority = priority;
	}

	return ade_set_args(L, "i", ohp->aigp->priority);
}

ADE_FUNC(remove, l_Order, NULL, "Removes the given order from the ship's priority queue.", "boolean", "True if order was successfully removed, otherwise false or nil.")
{
	order_h *ohp = NULL;
	if(!ade_get_args(L, "o", l_Order.GetPtr(&ohp)))
		return ADE_RETURN_NIL;

	if(!ohp->IsValid())
		return ADE_RETURN_FALSE;

	ai_info *aip = &Ai_info[Ships[ohp->objh.objp->instance].ai_index];

	ai_remove_ship_goal(aip, ohp->odx);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getType, l_Order, NULL, "Gets the type of the order.", "enumeration", "The type of the order as one of the ORDER_* enumerations.")
{
	order_h *ohp = NULL;
	int eh_idx = -1;
	if(!ade_get_args(L, "o", l_Order.GetPtr(&ohp)))
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	if(!ohp->IsValid())
		return ade_set_error(L, "o", l_Enum.Set(enum_h()));

	switch(ohp->aigp->ai_mode){
	case AI_GOAL_DESTROY_SUBSYSTEM:
	case AI_GOAL_CHASE_WEAPON:
	case AI_GOAL_CHASE:
		eh_idx = LE_ORDER_ATTACK;
		break;
	case AI_GOAL_DOCK:
		eh_idx = LE_ORDER_DOCK;
		break;
	case AI_GOAL_WAYPOINTS:
		eh_idx = LE_ORDER_WAYPOINTS;
		break;
	case AI_GOAL_WAYPOINTS_ONCE:
		eh_idx = LE_ORDER_WAYPOINTS_ONCE;
		break;
	case AI_GOAL_WARP:
		eh_idx = LE_ORDER_DEPART;
		break;
	case AI_GOAL_FORM_ON_WING:
		eh_idx = LE_ORDER_FORM_ON_WING;
		break;
	case AI_GOAL_UNDOCK:
		eh_idx = LE_ORDER_UNDOCK;
		break;
	case AI_GOAL_GUARD:
		eh_idx = LE_ORDER_GUARD;
		break;
	case AI_GOAL_DISABLE_SHIP:
		eh_idx = LE_ORDER_DISABLE;
		break;
	case AI_GOAL_DISARM_SHIP:
		eh_idx = LE_ORDER_DISARM;
		break;
	case AI_GOAL_CHASE_ANY:
		eh_idx = LE_ORDER_ATTACK_ANY;
		break;
	case AI_GOAL_IGNORE_NEW:
	case AI_GOAL_IGNORE:
		eh_idx = LE_ORDER_IGNORE;
		break;
	case AI_GOAL_EVADE_SHIP:
		eh_idx = LE_ORDER_EVADE;
		break;
	case AI_GOAL_STAY_NEAR_SHIP:
		eh_idx = LE_ORDER_STAY_NEAR;
		break;
	case AI_GOAL_KEEP_SAFE_DISTANCE:
		eh_idx = LE_ORDER_KEEP_SAFE_DISTANCE;
		break;
	case AI_GOAL_REARM_REPAIR:
		eh_idx = LE_ORDER_REARM;
		break;
	case AI_GOAL_STAY_STILL:
		eh_idx = LE_ORDER_STAY_STILL;
		break;
	case AI_GOAL_PLAY_DEAD:
		eh_idx = LE_ORDER_PLAY_DEAD;
		break;
	case AI_GOAL_FLY_TO_SHIP:
		eh_idx = LE_ORDER_FLY_TO;
		break;
	case AI_GOAL_CHASE_WING:
		eh_idx = LE_ORDER_ATTACK_WING;
		break;
	case AI_GOAL_GUARD_WING:
		eh_idx = LE_ORDER_GUARD_WING;
		break;
	case AI_GOAL_CHASE_SHIP_CLASS:
		eh_idx = LE_ORDER_ATTACK_SHIP_CLASS;
		break;
	}

	return ade_set_args(L, "o", l_Enum.Set(enum_h(eh_idx)));
}

ADE_VIRTVAR(Target, l_Order, "object", "Target of the order. Value may also be a deriviative of the 'object' class, such as 'ship'.", "object", "Target object or invalid object handle if order handle is invalid or order requires no target.")
{
	order_h *ohp = NULL;
	object_h *newh = NULL;
	ai_info *aip = NULL;
	waypoint_list *wpl = NULL;
	int shipnum = -1, objnum = -1;
	if(!ade_get_args(L, "o|o", l_Order.GetPtr(&ohp), l_Object.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!ohp->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	aip = &Ai_info[Ships[ohp->objh.objp->instance].ai_index];

	if(ADE_SETTING_VAR){
		if(newh && newh->IsValid()){
			switch(ohp->aigp->ai_mode){
			case AI_GOAL_DESTROY_SUBSYSTEM:
			case AI_GOAL_CHASE:
			case AI_GOAL_FORM_ON_WING:
			case AI_GOAL_GUARD:
			case AI_GOAL_DISABLE_SHIP:
			case AI_GOAL_DISARM_SHIP:
			case AI_GOAL_IGNORE_NEW:
			case AI_GOAL_IGNORE:
			case AI_GOAL_EVADE_SHIP:
			case AI_GOAL_STAY_NEAR_SHIP:
			case AI_GOAL_KEEP_SAFE_DISTANCE:
			case AI_GOAL_FLY_TO_SHIP:
			case AI_GOAL_STAY_STILL:
				if ((newh->objp->type == OBJ_SHIP) && !stricmp(Ships[newh->objp->instance].ship_name, ohp->aigp->target_name)){
					ohp->aigp->target_name = Ships[newh->objp->instance].ship_name;
					ohp->aigp->time = Missiontime;
					if(ohp->odx == 0) {
						aip->ok_to_target_timestamp = timestamp(0);
						set_target_objnum(aip, OBJ_INDEX(newh->objp));
					}
				}
				break;
			case AI_GOAL_CHASE_WEAPON:
				if ((newh->objp->type == OBJ_WEAPON) && (ohp->aigp->target_signature != newh->sig)){
					ohp->aigp->target_instance = newh->objp->instance;
					ohp->aigp->target_signature = Weapons[newh->objp->instance].objnum;
					ohp->aigp->time = Missiontime;
					if(ohp->odx == 0) {
						aip->ok_to_target_timestamp = timestamp(0);
						set_target_objnum(aip, OBJ_INDEX(newh->objp));
					}
				}
				break;
			case AI_GOAL_CHASE_SHIP_CLASS:
				// a ship class isn't an in-mission object
				return ade_set_error(L, "o", l_Object.Set(object_h()));
			case AI_GOAL_WAYPOINTS:
			case AI_GOAL_WAYPOINTS_ONCE:
				if (newh->objp->type == OBJ_WAYPOINT){
					wpl = find_waypoint_list_with_instance(newh->objp->instance);
					if (!stricmp(wpl->get_name(),ohp->aigp->target_name)){
						ohp->aigp->target_name = wpl->get_name();
						ohp->aigp->time = Missiontime;
						if(ohp->odx == 0) {
							int flags = 0;
							if ( ohp->aigp->ai_mode == AI_GOAL_WAYPOINTS)
								flags |= WPF_REPEAT;
							ai_start_waypoints(ohp->objh.objp, wpl, flags);
						}
					}
				}
				break;
			case AI_GOAL_CHASE_WING:
				if((newh->objp->type == OBJ_SHIP) && !stricmp(Ships[newh->objp->instance].ship_name, ohp->aigp->target_name)){
					ship *shipp = &Ships[newh->objp->instance];
					if (shipp->wingnum != -1){
						ohp->aigp->target_name = Wings[shipp->wingnum].name;
						if(ohp->odx == 0) {
							aip->ok_to_target_timestamp = timestamp(0);
							ai_attack_wing(ohp->objh.objp,shipp->wingnum);
						}
					}
				}
				break;
			case AI_GOAL_GUARD_WING:
				if((newh->objp->type == OBJ_SHIP) && !stricmp(Ships[newh->objp->instance].ship_name, ohp->aigp->target_name)){
					ship *shipp = &Ships[newh->objp->instance];
					if (shipp->wingnum != -1){
						ohp->aigp->target_name = Wings[shipp->wingnum].name;
						if(ohp->odx == 0) {
							aip->ok_to_target_timestamp = timestamp(0);
							ai_set_guard_wing(ohp->objh.objp,shipp->wingnum);
						}
					}
				}
				break;
			}
		}
	}

	switch(ohp->aigp->ai_mode){
	case AI_GOAL_DESTROY_SUBSYSTEM:
	case AI_GOAL_CHASE:
	case AI_GOAL_DOCK:
	case AI_GOAL_FORM_ON_WING:
	case AI_GOAL_GUARD:
	case AI_GOAL_DISABLE_SHIP:
	case AI_GOAL_DISARM_SHIP:
	case AI_GOAL_IGNORE_NEW:
	case AI_GOAL_IGNORE:
	case AI_GOAL_EVADE_SHIP:
	case AI_GOAL_STAY_NEAR_SHIP:
	case AI_GOAL_KEEP_SAFE_DISTANCE:
	case AI_GOAL_REARM_REPAIR:
	case AI_GOAL_FLY_TO_SHIP:
	case AI_GOAL_UNDOCK:
		shipnum = ship_name_lookup(ohp->aigp->target_name);
		objnum = Ships[shipnum].objnum;
		break;
	case AI_GOAL_CHASE_WEAPON:
		objnum = Weapons[ohp->aigp->target_instance].objnum;
		break;
	case AI_GOAL_WAYPOINTS:
	case AI_GOAL_WAYPOINTS_ONCE:
		wpl = find_matching_waypoint_list(ohp->aigp->target_name);
		if(ohp->odx == 0) {
			objnum = aip->wp_list->get_waypoints()[aip->wp_index].get_objnum();
		} else {
			objnum = wpl->get_waypoints().front().get_objnum();
		}
		break;
	case AI_GOAL_STAY_STILL:
		shipnum = ship_name_lookup(ohp->aigp->target_name);
		if (shipnum != -1){
			objnum = Ships[shipnum].objnum;
			break;
		}
	case AI_GOAL_CHASE_WING:
	case AI_GOAL_GUARD_WING:
		int wingnum = wing_name_lookup(ohp->aigp->target_name);
		if (Wings[wingnum].current_count > 0){
			shipnum = Wings[wingnum].ship_index[0];
			objnum = Ships[shipnum].objnum;
		}
		break;
	}

	return ade_set_object_with_breed(L, objnum);
}


ADE_VIRTVAR(TargetSubsystem, l_Order, "subsystem", "Target subsystem of the order.", "subsystem", "Target subsystem, or invalid subsystem handle if order handle is invalid or order requires no subsystem target.")
{
	order_h *ohp = NULL;
	ship_subsys_h *newh = NULL;
	ai_info *aip = NULL;
	object *objp = NULL;
	if(!ade_get_args(L, "o|o", l_Order.GetPtr(&ohp), l_Subsystem.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!ohp->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	aip = &Ai_info[Ships[ohp->objh.objp->instance].ai_index];

	if(ADE_SETTING_VAR)
	{
		if(newh && newh->IsValid() && (ohp->aigp->ai_mode == AI_GOAL_DESTROY_SUBSYSTEM))
		{
			objp = &Objects[newh->ss->parent_objnum];
			if(!stricmp(Ships[objp->instance].ship_name, ohp->aigp->target_name)) {
				ohp->aigp->target_name = Ships[objp->instance].ship_name;
				ohp->aigp->time = Missiontime;
				if(ohp->odx == 0) {
					aip->ok_to_target_timestamp = timestamp(0);
					set_target_objnum(aip, OBJ_INDEX(objp));
				}
			}
			ohp->aigp->ai_submode = ship_get_subsys_index( &Ships[objp->instance], newh->ss->system_info->subobj_name );
			if(ohp->odx == 0) {
				set_targeted_subsys(aip, newh->ss, OBJ_INDEX(objp));
			}
			if (aip == Player_ai) {
				Ships[newh->ss->parent_objnum].last_targeted_subobject[Player_num] = newh->ss;
			}
		}
	}

	if(ohp->aigp->ai_mode == AI_GOAL_DESTROY_SUBSYSTEM){
		return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(&Objects[Ships[ship_name_lookup(ohp->aigp->target_name)].objnum], ship_get_indexed_subsys(&Ships[ship_name_lookup(ohp->aigp->target_name)],ohp->aigp->ai_submode))));
	} else {
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));
	}
}

ADE_FUNC(isValid, l_Order, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	order_h *ohp = NULL;
	if(!ade_get_args(L, "o", l_Order.GetPtr(&ohp)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", ohp->IsValid());
}

//**********HANDLE: shiporders
ade_obj<object_h> l_ShipOrders("shiporders", "Ship orders");

ADE_FUNC(__len, l_ShipOrders, NULL, "Number of ship orders", "number", "Number of ship orders, or 0 if handle is invalid")
{
	object_h *objh = NULL;
	if(!ade_get_args(L, "o", l_ShipOrders.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid() || objh->objp->type != OBJ_SHIP || Ships[objh->objp->instance].ai_index < 0)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", ai_goal_num(&Ai_info[Ships[objh->objp->instance].ai_index].goals[0]));
}

ADE_INDEXER(l_ShipOrders, "number Index", "Array of ship orders", "order", "Order, or invalid order handle on failure")
{
	object_h *objh = NULL;
	int i;

	if (!ade_get_args(L, "oi", l_ShipOrders.GetPtr(&objh), &i))
		return ade_set_error(L, "o", l_Order.Set(order_h()));

	i--; //Lua->FS2

	if (!objh->IsValid() || i < 0 || i >= MAX_AI_GOALS)
		return ade_set_error(L, "o", l_Order.Set(order_h()));

	ai_info *aip = &Ai_info[Ships[objh->objp->instance].ai_index];

	if (aip->goals[i].ai_mode != AI_GOAL_NONE)
		return ade_set_args(L, "o", l_Order.Set(order_h(objh->objp, i)));
	else
		return ade_set_args(L, "o", l_Order.Set(order_h()));
}

ADE_FUNC(isValid, l_ShipOrders, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_ShipOrders.GetPtr(&oh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", oh->IsValid());
}



//**********HANDLE: shiptextures
ade_obj<object_h> l_ShipTextures("shiptextures", "Ship textures handle");

ADE_FUNC(__len, l_ShipTextures, NULL, "Number of textures on ship", "number", "Number of textures on ship, or 0 if handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_ShipTextures.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	polymodel *pm = model_get(Ship_info[Ships[objh->objp->instance].ship_info_index].model_num);

	if(pm == NULL)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", pm->n_textures*TM_NUM_TYPES);
}

ADE_INDEXER(l_ShipTextures, "number Index/string TextureFilename", "Array of ship textures", "texture", "Texture, or invalid texture handle on failure")
{
	object_h *oh;
	char *s;
	int tdx=-1;
	if (!ade_get_args(L, "os|o", l_ShipTextures.GetPtr(&oh), &s, l_Texture.Get(&tdx)))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if (!oh->IsValid() || s==NULL)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	ship *shipp = &Ships[oh->objp->instance];
	polymodel *pm = model_get(Ship_info[shipp->ship_info_index].model_num);
	int final_index = -1;
	int i;

	char fname[MAX_FILENAME_LEN];
	if (shipp->ship_replacement_textures != NULL)
	{
		for(i = 0; i < MAX_REPLACEMENT_TEXTURES; i++)
		{
			bm_get_filename(shipp->ship_replacement_textures[i], fname);

			if(!strextcmp(fname, s)) {
				final_index = i;
				break;
			}
		}
	}

	if(final_index < 0)
	{
		for (i = 0; i < pm->n_textures; i++)
		{
			int tm_num = pm->maps[i].FindTexture(s);
			if(tm_num > -1)
			{
				final_index = i*TM_NUM_TYPES+tm_num;
				break;
			}
		}
	}

	if (final_index < 0)
	{
		final_index = atoi(s) - 1;	//Lua->FS2

		if (final_index < 0 || final_index >= MAX_REPLACEMENT_TEXTURES)
			return ade_set_error(L, "o", l_Texture.Set(-1));
  	}

	if (ADE_SETTING_VAR) {
		if (shipp->ship_replacement_textures == NULL) {
			shipp->ship_replacement_textures = (int *) vm_malloc(MAX_REPLACEMENT_TEXTURES * sizeof(int));

			for (i = 0; i < MAX_REPLACEMENT_TEXTURES; i++)
				shipp->ship_replacement_textures[i] = -1;
		}

		if(bm_is_valid(tdx))
			shipp->ship_replacement_textures[final_index] = tdx;
		else
			shipp->ship_replacement_textures[final_index] = -1;
	}

	if (shipp->ship_replacement_textures != NULL && shipp->ship_replacement_textures[final_index] >= 0)
		return ade_set_args(L, "o", l_Texture.Set(shipp->ship_replacement_textures[final_index]));
	else
		return ade_set_args(L, "o", l_Texture.Set(pm->maps[final_index / TM_NUM_TYPES].textures[final_index % TM_NUM_TYPES].GetTexture()));
}

ADE_FUNC(isValid, l_ShipTextures, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	object_h *oh;
	if(!ade_get_args(L, "o", l_ShipTextures.GetPtr(&oh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", oh->IsValid());
}

//**********HANDLE: CockpitDisplayArray
class cockpit_displays_h
{
private:
	object *m_objp;
public:
	cockpit_displays_h() : m_objp( NULL ) {}
	cockpit_displays_h(object *objp)
	{
		this->m_objp = objp;
	}

	bool isValid()
	{
		if (m_objp == NULL)
		{
			return false;
		}

		if (m_objp != Player_obj)
		{
			return false;
		}

		if ( Ship_info[Player_ship->ship_info_index].cockpit_model_num < 0 ) {
			return false;
		}

		if ( Player_cockpit_textures == NULL ) {
			return false;
		}

		return true;
	}
};
ade_obj<cockpit_displays_h> l_CockpitDisplays("displays", "Player cockpit displays array handle");

ADE_FUNC(__len, l_CockpitDisplays, NULL, "Gets the number of cockpit displays for the player ship", "number", "number of displays or -1 on error")
{
	cockpit_displays_h *cdh = NULL;
	if(!ade_get_args(L, "o", l_CockpitDisplays.GetPtr(&cdh)))
		return ade_set_error(L, "i", -1);

	if (!cdh->isValid())
		return ade_set_error(L, "i", -1);

	return ade_set_args(L, "i", (int) Player_displays.size());
}

ADE_INDEXER(l_CockpitDisplays, "number/string", "Gets a cockpit display from the present player displays by either the index or the name of the display", "display", "Display handle or invalid handle on error")
{
	if (lua_isnumber(L, 2))
	{
		cockpit_displays_h *cdh = NULL;
		int index = -1;

		if (!ade_get_args(L, "oi", l_CockpitDisplays.GetPtr(&cdh), &index))
		{
			return ade_set_error(L, "o", l_CockpitDisplay.Set(cockpit_display_h()));
		}

		if (index < 0)
		{
			return ade_set_error(L, "o", l_CockpitDisplay.Set(cockpit_display_h()));
		}

		index--; // Lua -> C/C++

		return ade_set_args(L, "o", l_CockpitDisplay.Set(cockpit_display_h(Player_obj, index)));
	}
	else
	{
		cockpit_displays_h *cdh = NULL;
		char *name = NULL;

		if (!ade_get_args(L, "os", l_CockpitDisplays.GetPtr(&cdh), &name))
		{
			return ade_set_error(L, "o", l_CockpitDisplay.Set(cockpit_display_h()));
		}

		if (!cdh->isValid())
		{
			return ade_set_error(L, "o", l_CockpitDisplay.Set(cockpit_display_h()));
		}

		if (name == NULL)
		{
			return ade_set_error(L, "o", l_CockpitDisplay.Set(cockpit_display_h()));
		}

		size_t index = 0;

		for (SCP_vector<cockpit_display>::iterator iter = Player_displays.begin(); iter != Player_displays.end(); ++iter)
		{
			if (!strcmp(name, iter->name))
			{
				break;
			}
			else
			{
				index++;
			}
		}

		if (index == Player_displays.size())
		{
			LuaError(L, "Couldn't find cockpit display info with name \"%s\"", name);
			return ade_set_error(L, "o", l_CockpitDisplay.Set(cockpit_display_h()));
		}

		return ade_set_args(L, "o", l_CockpitDisplay.Set(cockpit_display_h(Player_obj, index)));
	}
}

ADE_FUNC(isValid, l_CockpitDisplays, NULL, "Detects whether this handle is valid or not", "boolean", "true if valid, false otherwise")
{
	cockpit_displays_h *cdh = NULL;
	if(!ade_get_args(L, "o", l_CockpitDisplays.GetPtr(&cdh)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", cdh->isValid());
}

//**********HANDLE: SoundEntry
struct sound_entry_h
{
	int idx;

	sound_entry_h()
	{
		idx = -1;
	}

	sound_entry_h(int n_idx)
	{
		idx = n_idx;
	}

	game_snd *Get()
	{
		if (!this->IsValid())
			return NULL;

		return &Snds[idx];
	}

	bool IsValid()
	{
		if (idx < 0 || idx >= (int) Snds.size())
			return false;

		return true;
	}

	int getId() 
	{
		if (!IsValid())
			return -1;

		game_snd *snd = Get();

		if (snd == NULL)
			return -1;

		return snd->id;
	}

};

//**********HANDLE: SoundEntry
ade_obj<sound_entry_h> l_SoundEntry("soundentry", "sounds.tbl table entry handle");

ADE_VIRTVAR(DefaultVolume, l_SoundEntry, "number", "The default volume of this game sound", "number", "Volume in the range from 1 to 0 or -1 on error")
{
	sound_entry_h *seh = NULL;
	float newVal = -1.0f;

	if (!ade_get_args(L, "o|f", l_SoundEntry.GetPtr(&seh), &newVal))
		return ade_set_error(L, "f", -1.0f);

	if (seh == NULL || !seh->IsValid())
		return ade_set_error(L, "f", -1.0f);

	if (ADE_SETTING_VAR)
	{
		if (seh->Get() != NULL)
		{
			CAP(newVal, 0.0f, 1.0f);

			seh->Get()->default_volume = newVal;
		}
	}

	return ade_set_args(L, "f", seh->Get()->default_volume);
}

ADE_FUNC(getFilename, l_SoundEntry, NULL, "Returns the filename of this sound", "string", "filename or empty string on error")
{
	sound_entry_h *seh = NULL;

	if (!ade_get_args(L, "o", l_SoundEntry.GetPtr(&seh)))
		return ade_set_error(L, "s", "");

	if (seh == NULL || !seh->IsValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", seh->Get()->filename);
}

ADE_FUNC(getDuration, l_SoundEntry, NULL, "Gives the length of the sound in seconds.", "number", "the length, or -1 on error")
{
	sound_entry_h *seh = NULL;

	if (!ade_get_args(L, "o", l_SoundEntry.GetPtr(&seh)))
		return ade_set_error(L, "f", -1.0f);

	if (seh == NULL || !seh->IsValid())
		return ade_set_error(L, "f", -1.0f);

	return ade_set_args(L, "f", (i2fl(snd_get_duration(seh->getId())) / 1000.0f));
}

ADE_FUNC(get3DValues, l_SoundEntry, "vector Postion[, number radius=0.0]", "Computes the volume and the panning of the sound when it would be played from the specified position.<br>"
	"If range is given then the volume will diminish when the listener is withing that distance to the source.<br>"
	"The position of the listener is always the the current viewing position.", "number, number", "The volume and the panning, in that sequence, or both -1 on error")
{
	sound_entry_h *seh = NULL;
	vec3d *sourcePos = NULL;
	float radius = 0.0f;

	float vol = 0.0f;
	float pan = 0.0f;

	if (!ade_get_args(L, "oo|f", l_SoundEntry.GetPtr(&seh), l_Vector.GetPtr(&sourcePos), &radius))
		return ade_set_error(L, "ff", -1.0f, -1.0f);

	if (seh == NULL || !seh->IsValid())
		return ade_set_error(L, "ff", -1.0f, -1.0f);

	int result = snd_get_3d_vol_and_pan(seh->Get(), sourcePos, &vol, &pan, radius);

	if (result < 0)
	{
		return ade_set_args(L, "ff", -1.0f, -1.0f);
	}
	else
	{
		return ade_set_args(L, "ff", vol, pan);
	}
}

ADE_FUNC(isValid, l_SoundEntry, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	sound_entry_h *seh;
	if(!ade_get_args(L, "o", l_SoundEntry.GetPtr(&seh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", seh->IsValid());
}

//**********HANDLE: Sound
struct sound_h : public sound_entry_h
{
	int sig;

	sound_h():sound_entry_h()
	{
		sig=-1;
	}

	sound_h(int n_gs_idx, int n_sig) : sound_entry_h(n_gs_idx)
	{
		sig=n_sig;
	}

	int getSignature()
	{
		if (!IsValid())
			return -1;

		return sig;
	}

	bool IsSoundValid()
	{
		if(sig < 0 || ds_get_channel(sig) < 0)
			return false;

		return true;
	}

	bool IsValid()
	{
		if(!sound_entry_h::IsValid())
			return false;

		if(sig < 0 || ds_get_channel(sig) < 0)
			return false;

		return true;
	}
};

ade_obj<sound_h> l_Sound("sound", "sound instance handle");

ADE_VIRTVAR(Pitch, l_Sound, "number", "Pitch of sound, from 100 to 100000", "number", "Pitch, or 0 if handle is invalid")
{
	sound_h *sh;
	int newpitch = 100;
	if(!ade_get_args(L, "o|i", l_Sound.GetPtr(&sh), &newpitch))
		return ade_set_error(L, "f", 0.0f);

	if (!sh->IsSoundValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR)
	{
		if(newpitch < 100)
			newpitch = 100;
		if(newpitch > 100000)
			newpitch = 100000;

		snd_set_pitch(sh->sig, newpitch);
	}

	return ade_set_args(L, "f", snd_get_pitch(sh->sig));
}

ADE_FUNC(getRemainingTime, l_Sound, NULL, "The remaining time of this sound handle", "number", "Remaining time, or -1 on error")
{
	sound_h *sh;
	if(!ade_get_args(L, "o", l_Sound.GetPtr(&sh)))
		return ade_set_error(L, "f", -1.0f);

	if (!sh->IsSoundValid())
		return ade_set_error(L, "f", -1.0f);

	int remaining = snd_time_remaining(sh->getSignature());

	return ade_set_args(L, "f", i2fl(remaining) / 1000.0f);
}

ADE_FUNC(setVolume, l_Sound, "number", "Sets the volume of this sound instance", "boolean", "true if succeeded, false otherwise")
{
	sound_h *sh;
	float newVol = -1.0f;
	if(!ade_get_args(L, "of", l_Sound.GetPtr(&sh), &newVol))
		return ADE_RETURN_FALSE;

	if (!sh->IsSoundValid())
		return ADE_RETURN_FALSE;

	CAP(newVol, 0.0f, 1.0f);

	snd_set_volume(sh->getSignature(), newVol);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(setPanning, l_Sound, "number", "Sets the panning of this sound. Argument ranges from -1 for left to 1 for right", "boolean", "true if succeeded, false otherwise")
{
	sound_h *sh;
	float newPan = -1.0f;
	if(!ade_get_args(L, "of", l_Sound.GetPtr(&sh), &newPan))
		return ADE_RETURN_FALSE;

	if (!sh->IsSoundValid())
		return ADE_RETURN_FALSE;

	CAP(newPan, -1.0f, 1.0f);

	snd_set_pan(sh->getSignature(), newPan);

	return ADE_RETURN_TRUE;
}


ADE_FUNC(setPosition, l_Sound, "number[,boolean = true]", 
		 "Sets the absolute position of the sound. If boolean argument is true then the value is given as a percentage<br>"
		 "This operation fails if there is no backing soundentry!", 
		 "boolean", "true if successfull, false otherwise")
{
	sound_h *sh;
	float val = -1.0f;
	bool percent = true;
	if(!ade_get_args(L, "of|b", l_Sound.GetPtr(&sh), &val, &percent))
		return ADE_RETURN_FALSE;

	if (!sh->IsValid())
		return ADE_RETURN_FALSE;

	if (val <= 0.0f)
		return ADE_RETURN_FALSE;

	snd_set_pos(sh->getSignature(), sh->Get(), val, percent);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(rewind, l_Sound, "number", "Rewinds the sound by the given number of seconds<br>"
		 "This operation fails if there is no backing soundentry!", "boolean", "true if succeeded, false otherwise")
{
	sound_h *sh;
	float val = -1.0f;
	if(!ade_get_args(L, "of", l_Sound.GetPtr(&sh), &val))
		return ADE_RETURN_FALSE;

	if (!sh->IsValid())
		return ADE_RETURN_FALSE;

	if (val <= 0.0f)
		return ADE_RETURN_FALSE;

	snd_rewind(sh->getSignature(), sh->Get(), val);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(skip, l_Sound, "number", "Skips the given number of seconds of the sound<br>"
		 "This operation fails if there is no backing soundentry!", "boolean", "true if succeeded, false otherwise")
{
	sound_h *sh;
	float val = -1.0f;
	if(!ade_get_args(L, "of", l_Sound.GetPtr(&sh), &val))
		return ADE_RETURN_FALSE;

	if (!sh->IsValid())
		return ADE_RETURN_FALSE;

	if (val <= 0.0f)
		return ADE_RETURN_FALSE;

	snd_ffwd(sh->getSignature(), sh->Get(), val);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(isPlaying, l_Sound, NULL, "Specifies if this handle is currently playing", "boolean", "true if playing, false if otherwise")
{
	sound_h *sh;
	if(!ade_get_args(L, "o", l_Sound.GetPtr(&sh)))
		return ade_set_error(L, "b", false);

	if (!sh->IsSoundValid())
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", snd_is_playing(sh->getSignature()) == 1);
}

ADE_FUNC(stop, l_Sound, NULL, "Stops the sound of this handle", "boolean", "true if succeeded, false otherwise")
{
	sound_h *sh;
	if(!ade_get_args(L, "o", l_Sound.GetPtr(&sh)))
		return ade_set_error(L, "b", false);

	if (!sh->IsSoundValid())
		return ade_set_error(L, "b", false);

	snd_stop(sh->getSignature());

	return ADE_RETURN_TRUE;
}

ADE_FUNC(isValid, l_Sound, NULL, "Detects whether the whole handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	sound_h *sh;
	if(!ade_get_args(L, "o", l_Sound.GetPtr(&sh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", sh->IsValid());
}

ADE_FUNC(isSoundValid, l_Sound, NULL, "Checks if only the sound is valid, should be used for non soundentry sounds",
		 "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	sound_h *sh;
	if(!ade_get_args(L, "o", l_Sound.GetPtr(&sh)))
		return ADE_RETURN_NIL;

	return ade_set_args(L, "b", sh->IsSoundValid());
}

ade_obj<sound_h> l_Sound3D("3Dsound", "3D sound instance handle", &l_Sound);

ADE_FUNC(updatePosition, l_Sound3D, "vector Position[, number radius = 0.0]", "Updates the given 3D sound with the specified position and an optional range value", "boolean", "true if succeesed, false otherwise")
{
	sound_h *sh;
	vec3d *newPos = NULL;
	float radius = 0.0f;

	if(!ade_get_args(L, "oo|f", l_Sound.GetPtr(&sh), l_Vector.GetPtr(&newPos), &radius))
		return ade_set_error(L, "b", false);

	if (!sh->IsValid() || newPos == NULL)
		return ade_set_error(L, "b", false);

	snd_update_3d_pos(sh->getSignature(), sh->Get(), newPos, radius);

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Soundfile
ade_obj<int> l_Soundfile("soundfile", "Handle to a sound file");

ADE_VIRTVAR(Duration, l_Soundfile, "number", "The duration of the sound file, in seconds", "number", "The duration or -1 on error")
{
	int snd_idx = -1;

	if (!ade_get_args(L, "o", l_Soundfile.Get(&snd_idx)))
		return ade_set_error(L, "f", -1.0f);

	if (snd_idx < 0)
		return ade_set_error(L, "f", -1.0f);

	int duration = snd_get_duration(snd_idx);

	return ade_set_args(L, "f", i2fl(duration) / 1000.0f);
}

ADE_VIRTVAR(Filename, l_Soundfile, "string", "The filename of the file", "string", "The file name or empty string on error")
{
	int snd_idx = -1;

	if (!ade_get_args(L, "o", l_Soundfile.Get(&snd_idx)))
		return ade_set_error(L, "s", "");

	if (snd_idx < 0)
		return ade_set_error(L, "s", "");

	const char* filename = snd_get_filename(snd_idx);

	return ade_set_args(L, "s", filename);
}


ADE_FUNC(play, l_Soundfile, "[number volume = 1.0[, number panning = 0.0]]", "Plays the sound", "sound", "A sound handle or invalid handle on error")
{
	int snd_idx = -1;
	float volume = 1.0f;
	float panning = 0.0f;

	if (!ade_get_args(L, "o|ff", l_Soundfile.Get(&snd_idx)))
		return ade_set_error(L, "o", l_Sound.Set(sound_h()));

	if (snd_idx < 0)
		return ade_set_error(L, "o", l_Sound.Set(sound_h()));

	if (volume < 0.0f)
	{
		LuaError(L, "Invalid volume value of %f specified!", volume);
		return ade_set_error(L, "o", l_Sound.Set(sound_h()));
	}

	int handle = snd_play_raw(snd_idx, panning, volume);
	
	return ade_set_args(L, "o", l_Sound.Set(sound_h(-1, handle)));
}

ADE_FUNC(isValid, l_Soundfile, NULL, "Checks if the soundfile handle is valid", "boolean", "true if valid, false otherwise")
{	
	int idx = -1;
	if (!ade_get_args(L, "o", l_Soundfile.Get(&idx)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", idx >= 0);
}

//**********HANDLE: Persona
ade_obj<int> l_Persona("persona", "Persona handle");

ADE_VIRTVAR(Name, l_Persona, "string", "The name of the persona", "string", "The name or empty string on error")
{
	int idx = -1;

	if (!ade_get_args(L, "o", l_Persona.Get(&idx)))
		return ade_set_error(L, "s", "");

	if (Personas == NULL)
		return ade_set_error(L, "s", "");

	if (idx < 0 || idx >= Num_personas)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Personas[idx].name);
}

ADE_FUNC(isValid, l_Persona, NULL, "Detect if the handle is valid", "boolean", "true if valid, false otherwise")
{
	int idx = -1;

	if (!ade_get_args(L, "o", l_Persona.Get(&idx)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", idx >= 0 && idx < Num_personas);
}

//**********HANDLE: Message
ade_obj<int> l_Message("message", "Handle to a mission message");

ADE_VIRTVAR(Name, l_Message, "string", "The name of the message as specified in the mission file", "string", "The name or an empty string if handle is invalid")
{
	int idx = -1;
	if (!ade_get_args(L, "o", l_Message.Get(&idx)))
		return ade_set_error(L, "s", "");
	
	if (idx < 0 && idx >= (int) Messages.size())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Messages[idx].name);
}

ADE_VIRTVAR(Message, l_Message, "string", "The unaltered text of the message, see getMessage() for options to replace variables<br>"
			"<b>NOTE:</b> Changing the text will also change the text for messages not yet played but already in the message queue!",
			"string", "The message or an empty string if handle is invalid")
{
	int idx = -1;
	char* newText = NULL;
	if (!ade_get_args(L, "o|s", l_Message.Get(&idx), &newText))
		return ade_set_error(L, "s", "");
	
	if (idx < 0 && idx >= (int) Messages.size())
		return ade_set_error(L, "s", "");

	if (ADE_SETTING_VAR && newText != NULL)
	{
		if (strlen(newText) > MESSAGE_LENGTH)
			LuaError(L, "New message text is too long, maximum is %d!", MESSAGE_LENGTH);
		else
			strcpy_s(Messages[idx].message, newText);
	}

	return ade_set_args(L, "s", Messages[idx].message);
}

// from mission/missionmessage.cpp
extern int add_wave( const char *wave_name );
ADE_VIRTVAR(VoiceFile, l_Message, "soundfile", "The voice file of the message", "soundfile", "The voice file handle or invalid handle when not present")
{
	int idx = -1;
	int sndIdx = -1;

	if (!ade_get_args(L, "o|o", l_Message.Get(&idx), l_Soundfile.Get(&sndIdx)))
		return ade_set_error(L, "o", l_Soundfile.Set(-1));
	
	if (idx < 0 && idx >= (int) Messages.size())
		return ade_set_error(L, "o", l_Soundfile.Set(-1));

	MissionMessage* msg = &Messages[idx];

	if (ADE_SETTING_VAR)
	{
		if (sndIdx >= 0)
		{
			const char* newFilename = snd_get_filename(sndIdx);

			msg->wave_info.index = add_wave(newFilename);
		}
		else
		{
			msg->wave_info.index = -1;
		}
	}

	if (msg->wave_info.index < 0)
	{
		return ade_set_args(L, "o", l_Soundfile.Set(-1));
	}
	else
	{
		int index = msg->wave_info.index;
		// Load the sound before using it
		message_load_wave(index, Message_waves[index].name);
		
		return ade_set_args(L, "o", l_Soundfile.Set(Message_waves[index].num));
	}
}

ADE_VIRTVAR(Persona, l_Message, "persona", "The persona of the message", "persona", "The persona handle or invalid handle if not present")
{
	int idx = -1;
	int newPersona = -1;

	if (!ade_get_args(L, "o|o", l_Message.Get(&idx), l_Persona.Get(&newPersona)))
		return ade_set_error(L, "o", l_Soundfile.Set(-1));
	
	if (idx < 0 && idx >= (int) Messages.size())
		return ade_set_error(L, "o", l_Soundfile.Set(-1));
	
	if (ADE_SETTING_VAR && newPersona >= 0 && newPersona < Num_personas)
	{
		Messages[idx].persona_index = newPersona;
	}

	return ade_set_args(L, "o", l_Persona.Set(Messages[idx].persona_index));
}

ADE_FUNC(getMessage, l_Message, "[boolean replaceVars = true]", "Gets the text of the message and optionally replaces SEXP variables with their respective values.", "string", "The message or an empty string if handle is invalid")
{
	int idx = -1;
	bool replace = true;
	if (!ade_get_args(L, "o|b", l_Message.Get(&idx), &replace))
		return ade_set_error(L, "s", "");
	
	if (idx < 0 && idx >= (int) Messages.size())
		return ade_set_error(L, "s", "");

	if (!replace)
		return ade_set_args(L, "s", Messages[idx].message);
	else
	{
		char temp_buf[MESSAGE_LENGTH];
		strcpy_s(temp_buf, Messages[idx].message);

		sexp_replace_variable_names_with_values(temp_buf, MESSAGE_LENGTH);

		return ade_set_args(L, "s", temp_buf);
	}
}

ADE_FUNC(isValid, l_Message, NULL, "Checks if the message handle is valid", "boolean", "true if valid, false otherwise")
{	
	int idx = -1;
	if (!ade_get_args(L, "o", l_Message.Get(&idx)))
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", idx >= 0 && idx < (int) Messages.size());
}

//**********HANDLE: Wing
ade_obj<int> l_Wing("wing", "Wing handle");

ADE_INDEXER(l_Wing, "number Index", "Array of ships in the wing", "ship", "Ship handle, or invalid ship handle if index is invalid or wing handle is invalid")
{
	int wdx;
	int sdx;
	object_h *ndx=NULL;
	if(!ade_get_args(L, "oi|o", l_Wing.Get(&wdx), &sdx, l_Ship.GetPtr(&ndx)))
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	if(wdx < 0 || wdx >= Num_wings || sdx < 1 || sdx > Wings[wdx].current_count) {
		return ade_set_error(L, "o", l_Ship.Set(object_h()));
	}

	//Lua-->FS2
	sdx--;

	if(ADE_SETTING_VAR && ndx != NULL && ndx->IsValid()) {
		Wings[wdx].ship_index[sdx] = ndx->objp->instance;
	}

	return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[Ships[Wings[wdx].ship_index[sdx]].objnum])));
}

ADE_FUNC(__len, l_Wing, NULL, "Gets the number of ships in the wing", "number", "Number of ships in wing, or 0 if invalid handle")
{
	int wdx;
	if(!ade_get_args(L, "o", l_Wing.Get(&wdx)) || wdx < 0 || wdx >= Num_wings)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", Wings[wdx].current_count);
}

ADE_VIRTVAR(Name, l_Wing, "string", "Name of Wing", "string", "Wing name, or empty string if handle is invalid")
{
	int wdx;
	char *s = NULL;
	if ( !ade_get_args(L, "o|s", l_Wing.Get(&wdx), &s) || wdx < 0 || wdx >= Num_wings )
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(Wings[wdx].name, s, sizeof(Wings[wdx].name)-1);
	}

	return ade_set_args(L, "s", Wings[wdx].name);
}

ADE_FUNC(isValid, l_Wing, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Wing.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= Num_wings)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Ship
ade_obj<object_h> l_Ship("ship", "Ship handle", &l_Object);

ADE_INDEXER(l_Ship, "string Name/number Index", "Array of ship subsystems", "subsystem", "Subsystem handle, or invalid subsystem handle if index or ship handle is invalid")
{
	object_h *objh;
	char *s = NULL;
	ship_subsys_h *sub = nullptr;
	if(!ade_get_args(L, "o|so", l_Ship.GetPtr(&objh), &s, l_Subsystem.GetPtr(&sub)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	ship *shipp = &Ships[objh->objp->instance];
	ship_subsys *ss = ship_get_subsys(shipp, s);

	if(ss == NULL)
	{
		int idx = atoi(s);
		if(idx > 0 && idx <= ship_get_num_subsys(shipp))
		{
			idx--; //Lua->FS2
			ss = ship_get_indexed_subsys(shipp, idx);
		}
	}

	if(ss == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(objh->objp, ss)));
}

ADE_FUNC(__len, l_Ship, NULL, "Number of subsystems on ship", "number", "Subsystem number, or 0 if handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", ship_get_num_subsys(&Ships[objh->objp->instance]));
}

ADE_VIRTVAR(ShieldArmorClass, l_Ship, "string", "Current Armor class of the ships' shield", "string", "Armor class name, or empty string if none is set")
{
	object_h *objh;
	char *s = NULL;
	char *name = NULL;
	
	if(!ade_get_args(L, "o|s", l_Ship.GetPtr(&objh), &s))
		return ade_set_error(L, "s", "");

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	ship *shipp = &Ships[objh->objp->instance];
	int atindex = -1;
	if (ADE_SETTING_VAR && s != NULL) {
		atindex = armor_type_get_idx(s);
		shipp->shield_armor_type_idx = atindex;
	}

	if (atindex != -1)
		name = Armor_types[atindex].GetNamePtr();
	else
		name = "";

	return ade_set_args(L, "s", name);
}

ADE_VIRTVAR(ArmorClass, l_Ship, "string", "Current Armor class", "string", "Armor class name, or empty string if none is set")
{
	object_h *objh;
	char *s = NULL;
	char *name = NULL;
	
	if(!ade_get_args(L, "o|s", l_Ship.GetPtr(&objh), &s))
		return ade_set_error(L, "s", "");

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	ship *shipp = &Ships[objh->objp->instance];
	int atindex = -1;
	if (ADE_SETTING_VAR && s != NULL) {
		atindex = armor_type_get_idx(s);
		shipp->armor_type_idx = atindex;
	}

	if (atindex != -1)
		name = Armor_types[atindex].GetNamePtr();
	else
		name = "";

	return ade_set_args(L, "s", name);
}

ADE_VIRTVAR(Name, l_Ship, "string", "Ship name", "string", "Ship name, or empty string if handle is invalid")
{
	object_h *objh;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Ship.GetPtr(&objh), &s))
		return ade_set_error(L, "s", "");

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && s != NULL) {
		strncpy(shipp->ship_name, s, sizeof(shipp->ship_name)-1);
	}

	return ade_set_args(L, "s", shipp->ship_name);
}

ADE_VIRTVAR(AfterburnerFuelLeft, l_Ship, "number", "Afterburner fuel left", "number", "Afterburner fuel left, or 0 if handle is invalid")
{
	object_h *objh;
	float fuel = -1.0f;
	if(!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &fuel))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && fuel >= 0.0f)
		shipp->afterburner_fuel = fuel;

	return ade_set_args(L, "f", shipp->afterburner_fuel);
}

ADE_VIRTVAR(AfterburnerFuelMax, l_Ship, "number", "Afterburner fuel capacity", "number", "Afterburner fuel capacity, or 0 if handle is invalid")
{
	object_h *objh;
	float fuel = -1.0f;
	if(!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &fuel))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	ship_info *sip = &Ship_info[Ships[objh->objp->instance].ship_info_index];

	if(ADE_SETTING_VAR && fuel >= 0.0f)
		sip->afterburner_fuel_capacity = fuel;

	return ade_set_args(L, "f", sip->afterburner_fuel_capacity);
}

ADE_VIRTVAR(Class, l_Ship, "shipclass", "Ship class", "shipclass", "Ship class, or invalid shipclass handle if ship handle is invalid")
{
	object_h *objh;
	int idx=-1;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_Shipclass.Get(&idx)))
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && idx > -1) {
		change_ship_type(objh->objp->instance, idx, 1);
		if (shipp == Player_ship) {
			set_current_hud();
		}
	}

	if(shipp->ship_info_index < 0)
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	return ade_set_args(L, "o", l_Shipclass.Set(shipp->ship_info_index));
}

ADE_VIRTVAR(CountermeasuresLeft, l_Ship, "number", "Number of countermeasures left", "number", "Countermeasures left, or 0 if ship handle is invalid")
{
	object_h *objh;
	int newcm = -1;
	if(!ade_get_args(L, "o|i", l_Ship.GetPtr(&objh), &newcm))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && newcm > -1)
		shipp->cmeasure_count = newcm;

	return ade_set_args(L, "i", shipp->cmeasure_count);
}

ADE_VIRTVAR(CockpitDisplays, l_Ship, "displays", "An array of the cockpit displays on this ship.<br>NOTE: Only the ship of the player has these", "displays", "displays handle or invalid handle on error")
{
	object_h *objh;
	cockpit_displays_h *cdh = NULL;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_CockpitDisplays.GetPtr(&cdh)))
		return ade_set_error(L, "o", l_CockpitDisplays.Set(cockpit_displays_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_CockpitDisplays.Set(cockpit_displays_h()));

	if(ADE_SETTING_VAR)
	{
		LuaError(L, "Attempted to use incomplete feature: Cockpit displays copy");
	}

	return ade_set_args(L, "o", l_CockpitDisplays.Set(cockpit_displays_h(objh->objp)));
}

ADE_VIRTVAR(CountermeasureClass, l_Ship, "weaponclass", "Weapon class mounted on this ship's countermeasure point", "weaponclass", "Countermeasure hardpoint weapon class, or invalid weaponclass handle if no countermeasure class or ship handle is invalid")
{
	object_h *objh;
	int newcm = -1;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_Weaponclass.Get(&newcm)))
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));;

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));;

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR) {
			shipp->current_cmeasure = newcm;
	}

	if(shipp->current_cmeasure > -1)
		return ade_set_args(L, "o", l_Weaponclass.Set(shipp->current_cmeasure));
	else
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));;
}

ADE_VIRTVAR(HitpointsMax, l_Ship, "number", "Total hitpoints", "number", "Ship maximum hitpoints, or 0 if handle is invalid")
{
	object_h *objh;
	float newhits = -1;
	if(!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &newhits))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && newhits > -1)
		shipp->ship_max_hull_strength = newhits;

	return ade_set_args(L, "f", shipp->ship_max_hull_strength);
}

ADE_VIRTVAR(WeaponEnergyLeft, l_Ship, "number", "Current weapon energy reserves", "number", "Ship current weapon energy reserve level, or 0 if invalid")
{
	object_h *objh;
	float neweng = -1;
	if(!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &neweng))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && neweng > -1)
		shipp->weapon_energy = neweng;

	return ade_set_args(L, "f", shipp->weapon_energy);
}

ADE_VIRTVAR(WeaponEnergyMax, l_Ship, "number", "Maximum weapon energy", "number", "Ship maximum weapon energy reserve level, or 0 if invalid")
{
	object_h *objh;
	float neweng = -1;
	if(!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &neweng))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	ship_info *sip = &Ship_info[Ships[objh->objp->instance].ship_info_index];

	if(ADE_SETTING_VAR && neweng > -1)
		sip->max_weapon_reserve = neweng;

	return ade_set_args(L, "f", sip->max_weapon_reserve);
}

ADE_VIRTVAR(AutoaimFOV, l_Ship, "number", "FOV of ship's autoaim, if any", "number", "FOV (in degrees), or 0 if ship uses no autoaim or if handle is invalid")
{
	object_h *objh;
	float fov = -1;
	if(!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &fov))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR && fov >= 0.0f) {
		if (fov > 180.0)
			fov = 180.0;

		shipp->autoaim_fov = fov * PI / 180.0f;
	}

	return ade_set_args(L, "f", shipp->autoaim_fov * 180.0f / PI);
}

ADE_VIRTVAR(PrimaryTriggerDown, l_Ship, "boolean", "Determines if primary trigger is pressed or not", "boolean", "True if pressed, false if not, nil if ship handle is invalid")
{
	object_h *objh;
	bool trig = false;
	if(!ade_get_args(L, "o|b", l_Ship.GetPtr(&objh), &trig))
		return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR)
    {
		if(trig)
			shipp->flags.set(Ship::Ship_Flags::Trigger_down);
		else
			shipp->flags.remove(Ship::Ship_Flags::Trigger_down);
    }

	if (shipp->flags[Ship::Ship_Flags::Trigger_down])
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}


ADE_VIRTVAR(PrimaryBanks, l_Ship, "weaponbanktype", "Array of primary weapon banks", "weaponbanktype", "Primary weapon banks, or invalid weaponbanktype handle if ship handle is invalid")
{
	object_h *objh;
	ship_banktype_h *swh = nullptr;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_WeaponBankType.GetPtr(&swh)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &Ships[objh->objp->instance].weapons;

	if(ADE_SETTING_VAR && swh && swh->IsValid()) {
		ship_weapon *src = &Ships[swh->objp->instance].weapons;

		dst->current_primary_bank = src->current_primary_bank;
		dst->num_primary_banks = src->num_primary_banks;

		memcpy(dst->next_primary_fire_stamp, src->next_primary_fire_stamp, sizeof(dst->next_primary_fire_stamp));
		memcpy(dst->primary_animation_done_time, src->primary_animation_done_time, sizeof(dst->primary_animation_done_time));
		memcpy(dst->primary_animation_position, src->primary_animation_position, sizeof(dst->primary_animation_position));
		memcpy(dst->primary_bank_ammo, src->primary_bank_ammo, sizeof(dst->primary_bank_ammo));
		memcpy(dst->primary_bank_capacity, src->primary_bank_capacity, sizeof(dst->primary_bank_capacity));
		memcpy(dst->primary_bank_rearm_time, src->primary_bank_rearm_time, sizeof(dst->primary_bank_rearm_time));
		memcpy(dst->primary_bank_start_ammo, src->primary_bank_start_ammo, sizeof(dst->primary_bank_start_ammo));
		memcpy(dst->primary_bank_weapons, src->primary_bank_weapons, sizeof(dst->primary_bank_weapons));
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(objh->objp, dst, SWH_PRIMARY)));
}

ADE_VIRTVAR(SecondaryBanks, l_Ship, "weaponbanktype", "Array of secondary weapon banks", "weaponbanktype", "Secondary weapon banks, or invalid weaponbanktype handle if ship handle is invalid")
{
	object_h *objh;
	ship_banktype_h *swh = nullptr;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_WeaponBankType.GetPtr(&swh)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &Ships[objh->objp->instance].weapons;

	if(ADE_SETTING_VAR && swh && swh->IsValid()) {
		ship_weapon *src = &Ships[swh->objp->instance].weapons;

		dst->current_secondary_bank = src->current_secondary_bank;
		dst->num_secondary_banks = src->num_secondary_banks;

		memcpy(dst->next_secondary_fire_stamp, src->next_secondary_fire_stamp, sizeof(dst->next_secondary_fire_stamp));
		memcpy(dst->secondary_animation_done_time, src->secondary_animation_done_time, sizeof(dst->secondary_animation_done_time));
		memcpy(dst->secondary_animation_position, src->secondary_animation_position, sizeof(dst->secondary_animation_position));
		memcpy(dst->secondary_bank_ammo, src->secondary_bank_ammo, sizeof(dst->secondary_bank_ammo));
		memcpy(dst->secondary_bank_capacity, src->secondary_bank_capacity, sizeof(dst->secondary_bank_capacity));
		memcpy(dst->secondary_bank_rearm_time, src->secondary_bank_rearm_time, sizeof(dst->secondary_bank_rearm_time));
		memcpy(dst->secondary_bank_start_ammo, src->secondary_bank_start_ammo, sizeof(dst->secondary_bank_start_ammo));
		memcpy(dst->secondary_bank_weapons, src->secondary_bank_weapons, sizeof(dst->secondary_bank_weapons));
		memcpy(dst->secondary_next_slot, src->secondary_next_slot, sizeof(dst->secondary_next_slot));
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(objh->objp, dst, SWH_SECONDARY)));
}

ADE_VIRTVAR(TertiaryBanks, l_Ship, "weaponbanktype", "Array of tertiary weapon banks", "weaponbanktype", "Tertiary weapon banks, or invalid weaponbanktype handle if ship handle is invalid")
{
	object_h *objh;
	ship_banktype_h *swh = nullptr;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_WeaponBankType.GetPtr(&swh)))
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_WeaponBankType.Set(ship_banktype_h()));

	ship_weapon *dst = &Ships[objh->objp->instance].weapons;

	if(ADE_SETTING_VAR && swh && swh->IsValid()) {
		ship_weapon *src = &Ships[swh->objp->instance].weapons;

		dst->current_tertiary_bank = src->current_tertiary_bank;
		dst->num_tertiary_banks = src->num_tertiary_banks;

		dst->next_tertiary_fire_stamp = src->next_tertiary_fire_stamp;
		dst->tertiary_bank_ammo = src->tertiary_bank_ammo;
		dst->tertiary_bank_capacity = src->tertiary_bank_capacity;
		dst->tertiary_bank_rearm_time = src->tertiary_bank_rearm_time;
		dst->tertiary_bank_start_ammo = src->tertiary_bank_start_ammo;
	}

	return ade_set_args(L, "o", l_WeaponBankType.Set(ship_banktype_h(objh->objp, dst, SWH_TERTIARY)));
}

ADE_VIRTVAR(Target, l_Ship, "object", "Target of ship. Value may also be a deriviative of the 'object' class, such as 'ship'.", "object", "Target object, or invalid object handle if no target or ship handle is invalid")
{
	object_h *objh;
	object_h *newh = nullptr;
	//WMC - Maybe use two argument return capabilities of Lua to set/return subsystem?
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_Object.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	ai_info *aip = NULL;
	if(Ships[objh->objp->instance].ai_index > -1)
		aip = &Ai_info[Ships[objh->objp->instance].ai_index];
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR && newh)
	{
		if(aip->target_signature != newh->sig)
		{
			if(newh->IsValid())
			{
				aip->target_objnum = OBJ_INDEX(newh->objp);
				aip->target_signature = newh->sig;
				aip->target_time = 0.0f;

				if (aip == Player_ai)
					hud_shield_hit_reset(newh->objp);
			}
			else
			{
				aip->target_objnum = -1;
				aip->target_signature = 0;
				aip->target_time = 0.0f;
			}

			set_targeted_subsys(aip, NULL, -1);
		}
	}

	return ade_set_object_with_breed(L, aip->target_objnum);
}

ADE_VIRTVAR(TargetSubsystem, l_Ship, "subsystem", "Target subsystem of ship.", "subsystem", "Target subsystem, or invalid subsystem handle if no target or ship handle is invalid")
{
	object_h *oh;
	ship_subsys_h *newh = nullptr;
	//WMC - Maybe use two argument return capabilities of Lua to set/return subsystem?
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&oh), l_Subsystem.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	ai_info *aip = NULL;
	if(Ships[oh->objp->instance].ai_index > -1)
		aip = &Ai_info[Ships[oh->objp->instance].ai_index];
	else
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh && newh->IsValid())
		{
			if (aip == Player_ai) {
				if (aip->target_signature != newh->sig)
					hud_shield_hit_reset(newh->objp);

				Ships[newh->ss->parent_objnum].last_targeted_subobject[Player_num] = newh->ss;
			}

			aip->target_objnum = OBJ_INDEX(newh->objp);
			aip->target_signature = newh->sig;
			aip->target_time = 0.0f;
			set_targeted_subsys(aip, newh->ss, aip->target_objnum);
		}
		else
		{
			aip->target_objnum = -1;
			aip->target_signature = 0;
			aip->target_time = 0.0f;

			set_targeted_subsys(aip, NULL, -1);
		}
	}

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(&Objects[aip->target_objnum], aip->targeted_subsys)));
}

ADE_VIRTVAR(Team, l_Ship, "team", "Ship's team", "team", "Ship team, or invalid team handle if ship handle is invalid")
{
	object_h *oh=NULL;
	int nt=-1;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&oh), l_Team.Get(&nt)))
		return ade_set_error(L, "o", l_Team.Set(-1));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Team.Set(-1));

	ship *shipp = &Ships[oh->objp->instance];

	if(ADE_SETTING_VAR && nt > -1) {
		shipp->team = nt;
	}

	return ade_set_args(L, "o", l_Team.Set(shipp->team));
}

ADE_VIRTVAR(Textures, l_Ship, "shiptextures", "Gets ship textures", "shiptextures", "Ship textures, or invalid shiptextures handle if ship handle is invalid")
{
	object_h *sh = nullptr;
	object_h *dh;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&dh), l_Ship.GetPtr(&sh)))
		return ade_set_error(L, "o", l_ShipTextures.Set(object_h()));

	if(!dh->IsValid())
		return ade_set_error(L, "o", l_ShipTextures.Set(object_h()));

	if(ADE_SETTING_VAR && sh && sh->IsValid()) {
		ship *src = &Ships[sh->objp->instance];
		ship *dest = &Ships[dh->objp->instance];
		
		if (src->ship_replacement_textures != NULL)
		{
			if (dest->ship_replacement_textures == NULL)
				dest->ship_replacement_textures = (int *) vm_malloc(MAX_REPLACEMENT_TEXTURES * sizeof(int));

			memcpy(dest->ship_replacement_textures, src->ship_replacement_textures, MAX_REPLACEMENT_TEXTURES * sizeof(int));
		}
	}

	return ade_set_args(L, "o", l_ShipTextures.Set(object_h(dh->objp)));
}

ADE_VIRTVAR(FlagAffectedByGravity, l_Ship, "boolean", "Checks for the \"affected-by-gravity\" flag", "boolean", "True if flag is set, false if flag is not set and nil on error") 
{
	object_h *objh=NULL;
	bool set = false;

	if (!ade_get_args(L, "o|b", l_Ship.GetPtr(&objh), &set))
		return ADE_RETURN_NIL;

	if (!objh->IsValid())
		return ADE_RETURN_NIL;
	
	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR)
    {
		shipp->flags.set(Ship::Ship_Flags::Affected_by_gravity, set);
    }

	if (shipp->flags[Ship::Ship_Flags::Affected_by_gravity])
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

extern void ship_reset_disabled_physics(object *objp, int ship_class);
ADE_VIRTVAR(Disabled, l_Ship, "boolean", "The disabled state of this ship", "boolean", "true if ship is diabled, false otherwise")
{
	object_h *objh=NULL;
	bool set = false;

	if (!ade_get_args(L, "o|b", l_Ship.GetPtr(&objh), &set))
		return ADE_RETURN_FALSE;

	if (!objh->IsValid())
		return ADE_RETURN_FALSE;

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR)
	{
		shipp->flags.set(Ship::Ship_Flags::Disabled, set);
		if(set)
		{
			mission_log_add_entry(LOG_SHIP_DISABLED, shipp->ship_name, NULL );
		}
		else
		{
			ship_reset_disabled_physics( &Objects[shipp->objnum], shipp->ship_info_index );
		}
	}

	if (shipp->flags[Ship::Ship_Flags::Disabled])
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_VIRTVAR(Stealthed, l_Ship, "boolean", "Stealth status of this ship", "boolean", "true if stealthed, false otherwise or on error")
{
	object_h *objh=NULL;
	bool stealthed = false;

	if (!ade_get_args(L, "o|b", l_Ship.GetPtr(&objh), &stealthed))
		return ADE_RETURN_FALSE;

	if (!objh->IsValid())
		return ADE_RETURN_FALSE;

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR)
	{
        shipp->flags.set(Ship::Ship_Flags::Stealth, stealthed);
	}

	if (shipp->flags[Ship::Ship_Flags::Stealth])
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_VIRTVAR(HiddenFromSensors, l_Ship, "boolean", "Hidden from sensors status of this ship", "boolean", "true if invisible to hidden from sensors, false otherwise or on error")
{
	object_h *objh=NULL;
	bool hidden = false;

	if (!ade_get_args(L, "o|b", l_Ship.GetPtr(&objh), &hidden))
		return ADE_RETURN_FALSE;

	if (!objh->IsValid())
		return ADE_RETURN_FALSE;

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR)
	{
        shipp->flags.set(Ship::Ship_Flags::Hidden_from_sensors, hidden);
	}

	if (shipp->flags[Ship::Ship_Flags::Hidden_from_sensors])
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_VIRTVAR(Gliding, l_Ship, "boolean", "Specifies whether this ship is currently gliding or not.", "boolean", "true if gliding, false otherwise or in case of error")
{
	object_h *objh=NULL;
	bool gliding = false;

	if (!ade_get_args(L, "o|b", l_Ship.GetPtr(&objh), &gliding))
		return ADE_RETURN_FALSE;

	if (!objh->IsValid())
		return ADE_RETURN_FALSE;

	ship *shipp = &Ships[objh->objp->instance];

	if(ADE_SETTING_VAR)
	{
		if (Ship_info[shipp->ship_info_index].can_glide)
		{
			object_set_gliding(&Objects[shipp->objnum], gliding, true);
		}
	}

	if (objh->objp->phys_info.flags & PF_GLIDING || objh->objp->phys_info.flags & PF_FORCE_GLIDE)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_VIRTVAR(EtsEngineIndex, l_Ship, "number", "(SET not implemented, see EtsSetIndexes)", "number", "Ships ETS Engine index value, 0 to MAX_ENERGY_INDEX")
{
	object_h *objh=NULL;
	int ets_idx = 0;

	if (!ade_get_args(L, "o|i", l_Ship.GetPtr(&objh), &ets_idx))
		return ade_set_error(L, "i", 0);

	if (!objh->IsValid())
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR)
		LuaError(L, "Attempted to set incomplete feature: ETS Engine Index (see EtsSetIndexes)");

	return ade_set_args(L, "i", Ships[objh->objp->instance].engine_recharge_index);
}

ADE_VIRTVAR(EtsShieldIndex, l_Ship, "number", "(SET not implemented, see EtsSetIndexes)", "number", "Ships ETS Shield index value, 0 to MAX_ENERGY_INDEX")
{
	object_h *objh=NULL;
	int ets_idx = 0;

	if (!ade_get_args(L, "o|i", l_Ship.GetPtr(&objh), &ets_idx))
		return ade_set_error(L, "i", 0);

	if (!objh->IsValid())
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR)
		LuaError(L, "Attempted to set incomplete feature: ETS Shield Index (see EtsSetIndexes)");

	return ade_set_args(L, "i", Ships[objh->objp->instance].shield_recharge_index);
}

ADE_VIRTVAR(EtsWeaponIndex, l_Ship, "number", "(SET not implemented, see EtsSetIndexes)", "number", "Ships ETS Weapon index value, 0 to MAX_ENERGY_INDEX")
{
	object_h *objh=NULL;
	int ets_idx = 0;

	if (!ade_get_args(L, "o|i", l_Ship.GetPtr(&objh), &ets_idx))
		return ade_set_error(L, "i", 0);

	if (!objh->IsValid())
		return ade_set_error(L, "i", 0);

	if(ADE_SETTING_VAR)
		LuaError(L, "Attempted to set incomplete feature: ETS Weapon Index (see EtsSetIndexes)");

	return ade_set_args(L, "i", Ships[objh->objp->instance].weapon_recharge_index);
}

ADE_VIRTVAR(Orders, l_Ship, "shiporders", "Array of ship orders", "shiporders", "Ship orders, or invalid handle if ship handle is invalid")
{
	object_h *objh = NULL;
	object_h *newh = NULL;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&objh), l_ShipOrders.GetPtr(&newh)))
		return ade_set_error(L, "o", l_ShipOrders.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_ShipOrders.Set(object_h()));;

	if(ADE_SETTING_VAR)
	{
		LuaError(L, "Attempted to use incomplete feature: Ai orders copy. Use giveOrder instead");
	}

	return ade_set_args(L, "o", l_ShipOrders.Set(object_h(objh->objp)));
}

ADE_FUNC(kill, l_Ship, "[object Killer]", "Kills the ship. Set \"Killer\" to the ship you are killing to self-destruct", "boolean", "True if successful, false or nil otherwise")
{
	object_h *victim,*killer=NULL;
	if(!ade_get_args(L, "o|o", l_Ship.GetPtr(&victim), l_Ship.GetPtr(&killer)))
		return ADE_RETURN_NIL;

	if(!victim->IsValid())
		return ADE_RETURN_NIL;

	if(!killer || !killer->IsValid())
		return ADE_RETURN_NIL;

	//Ripped straight from shiphit.cpp
	float percent_killed = -get_hull_pct(victim->objp);
	if (percent_killed > 1.0f){
		percent_killed = 1.0f;
	}

	ship_hit_kill(victim->objp, killer->objp, percent_killed, (victim->sig == killer->sig) ? 1 : 0);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(addShipEffect, l_Ship, "string name, int duration (in milliseconds)", "Activates an effect for this ship. Effect names are defined in Post_processing.tbl, and need to be implemented in the main shader. This functions analogous to the ship-effect sexp. NOTE: only one effect can be active at any time, adding new effects will override effects already in progress.\n", "boolean", "Returns true if the effect was successfully added, false otherwise") {
	object_h *shiph;
	char* effect = NULL;
	int duration;
	int effect_num;

	if (!ade_get_args(L, "o|si", l_Ship.GetPtr(&shiph), &effect, &duration))
		return ade_set_error(L, "b", false);

	if (!shiph->IsValid())
		return ade_set_error(L, "b", false);

	effect_num = get_effect_from_name(effect);
	if (effect_num == -1)
		return ade_set_error(L, "b", false);

	ship* shipp = &Ships[shiph->objp->instance];

	shipp->shader_effect_active = true;
	shipp->shader_effect_num = effect_num;
	shipp->shader_effect_duration = duration;
	shipp->shader_effect_start_time = timer_get_milliseconds();

	return ade_set_args(L, "b", true);
}

ADE_FUNC(hasShipExploded, l_Ship, NULL, "Checks if the ship explosion event has already happened", "number", "Returns 1 if first explosion timestamp is passed, 2 if second is passed, 0 otherwise")
{
	object_h *shiph;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&shiph)))
		return ade_set_error(L, "i", 0);

	if(!shiph->IsValid())
		return ade_set_error(L, "i", 0);

	ship *shipp = &Ships[shiph->objp->instance];

	if (shipp->flags[Ship::Ship_Flags::Dying]) {
		if (shipp->final_death_time == 0) {
			return ade_set_args(L, "i", 2);
		}		
		if (shipp->pre_death_explosion_happened == 1) {
			return ade_set_args(L, "i", 1);
		}
		return ade_set_args(L, "i", 3);
	}

	return ade_set_args(L, "i", 0);
}

ADE_FUNC(fireCountermeasure, l_Ship, NULL, "Launches a countermeasure from the ship", "boolean", "Whether countermeasure was launched or not")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ade_set_error(L, "b", false);

	if(!objh->IsValid())
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", ship_launch_countermeasure(objh->objp));
}

ADE_FUNC(firePrimary, l_Ship, NULL, "Fires ship primary bank(s)", "number", "Number of primary banks fired")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	int i = 0;
	i += ship_fire_primary(objh->objp, 0);
	i += ship_fire_primary(objh->objp, 1);

	return ade_set_args(L, "i", i);
}

ADE_FUNC(fireSecondary, l_Ship, NULL, "Fires ship secondary bank(s)", "number", "Number of secondary banks fired")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ade_set_error(L, "i", 0);

	if(!objh->IsValid())
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", ship_fire_secondary(objh->objp, 0));
}

ADE_FUNC(getAnimationDoneTime, l_Ship, "number Type, number Subtype", "Gets time that animation will be done", "number", "Time (seconds), or 0 if ship handle is invalid")
{
	object_h *objh;
	char *s = NULL;
	int subtype=-1;
	if(!ade_get_args(L, "o|si", l_Ship.GetPtr(&objh), &s, &subtype))
		return ade_set_error(L, "f", 0.0f);

	if(!objh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	int type = model_anim_match_type(s);
	if(type < 0)
		return ADE_RETURN_FALSE;

	int time_ms = model_anim_get_time_type(&Ships[objh->objp->instance], type, subtype);
	float time_s = (float)time_ms / 1000.0f;

	return ade_set_args(L, "f", time_s);
}

ADE_FUNC(clearOrders, l_Ship, NULL, "Clears a ship's orders list", "boolean", "True if successful, otherwise false or nil")
{
	object_h *objh = NULL;
	if(!ade_get_args(L, "o", l_Object.GetPtr(&objh)))
		return ADE_RETURN_NIL;
	if(!objh->IsValid())
		return ade_set_error(L, "b", false);

	//The actual clearing of the goals
	ai_clear_ship_goals( &Ai_info[Ships[objh->objp->instance].ai_index]);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(giveOrder, l_Ship, "enumeration Order, [object Target=nil, subsystem TargetSubsystem=nil, number Priority=1.0, shipclass TargetShipclass=nil]", "Uses the goal code to execute orders", "boolean", "True if order was given, otherwise false or nil")
{
	object_h *objh = NULL;
	enum_h *eh = NULL;
	float priority = 1.0f;
	int sclass = -1;
	object_h *tgh = NULL;
	ship_subsys_h *tgsh = NULL;
	if(!ade_get_args(L, "oo|oofo", l_Object.GetPtr(&objh), l_Enum.GetPtr(&eh), l_Object.GetPtr(&tgh), l_Subsystem.GetPtr(&tgsh), &priority, l_Shipclass.Get(&sclass)))
		return ADE_RETURN_NIL;

	if(!objh->IsValid() || !eh->IsValid())
		return ade_set_error(L, "b", false);

	//wtf...
	if(priority < 0.0f)
		return ade_set_error(L, "b", false);

	if(priority > 1.0f)
		priority = 1.0f;

	bool tgh_valid = tgh && tgh->IsValid();
	bool tgsh_valid = tgsh && tgsh->IsValid();
	int ai_mode = AI_GOAL_NONE;
	int ai_submode = -1234567;
	char *ai_shipname = NULL;
	switch(eh->index)
	{
		case LE_ORDER_ATTACK:
		{
			if(tgsh_valid)
			{
				ai_mode = AI_GOAL_DESTROY_SUBSYSTEM;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = ship_get_subsys_index( &Ships[tgsh->objp->instance], tgsh->ss->system_info->subobj_name );
			}
			else if(tgh_valid && tgh->objp->type == OBJ_WEAPON)
			{
				ai_mode = AI_GOAL_CHASE_WEAPON;
				ai_submode = tgh->objp->instance;
			}
			else if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_CHASE;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = SM_ATTACK;
			}
			break;
		}
		case LE_ORDER_DOCK:
		{
			ai_shipname = Ships[tgh->objp->instance].ship_name;
			ai_mode = AI_GOAL_DOCK;
			ai_submode = AIS_DOCK_0;
			break;
		}
		case LE_ORDER_WAYPOINTS:
		{
			if(tgh_valid && tgh->objp->type == OBJ_WAYPOINT)
			{
				ai_mode = AI_GOAL_WAYPOINTS;
				waypoint_list *wp_list = find_waypoint_list_with_instance(tgh->objp->instance);
				if(wp_list != NULL)
					ai_shipname = wp_list->get_name();
			}
			break;
		}
		case LE_ORDER_WAYPOINTS_ONCE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_WAYPOINT)
			{
				ai_mode = AI_GOAL_WAYPOINTS_ONCE;
				waypoint_list *wp_list = find_waypoint_list_with_instance(tgh->objp->instance);
				if(wp_list != NULL)
					ai_shipname = wp_list->get_name();
			}
			break;
		}
		case LE_ORDER_DEPART:
		{
			ai_mode = AI_GOAL_WARP;
			ai_submode = -1;
			break;
		}
		case LE_ORDER_FORM_ON_WING:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_FORM_ON_WING;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = 0;
			}
			break;
		}
		case LE_ORDER_UNDOCK:
		{
			ai_mode = AI_GOAL_UNDOCK;
			ai_submode = AIS_UNDOCK_0;

			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_GUARD:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_GUARD;
				ai_submode = AIS_GUARD_PATROL;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_DISABLE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_DISABLE_SHIP;
				ai_submode = -SUBSYSTEM_ENGINE;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_DISARM:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_DISARM_SHIP;
				ai_submode = -SUBSYSTEM_TURRET;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_ATTACK_ANY:
		{
			ai_mode = AI_GOAL_CHASE_ANY;
			ai_submode = SM_ATTACK;
			break;
		}
		case LE_ORDER_IGNORE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_IGNORE_NEW;
				ai_submode = 0;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_EVADE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_EVADE_SHIP;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_STAY_NEAR:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_STAY_NEAR_SHIP;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = -1;
			}
			break;
		}
		case LE_ORDER_KEEP_SAFE_DISTANCE:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_KEEP_SAFE_DISTANCE;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = -1;
			}
			break;
		}
		case LE_ORDER_REARM:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_REARM_REPAIR;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = 0;
			}
			break;
		}
		case LE_ORDER_STAY_STILL:
		{
			ai_mode = AI_GOAL_STAY_STILL;
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_shipname = Ships[tgh->objp->instance].ship_name;
			}
			break;
		}
		case LE_ORDER_PLAY_DEAD:
		{
			ai_mode = AI_GOAL_PLAY_DEAD;
			break;
		}
		case LE_ORDER_FLY_TO:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ai_mode = AI_GOAL_FLY_TO_SHIP;
				ai_shipname = Ships[tgh->objp->instance].ship_name;
				ai_submode = 0;
			}
			break;
		}
		case LE_ORDER_ATTACK_WING:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ship *shipp = &Ships[tgh->objp->instance];
				if (shipp->wingnum != -1)
				{
					ai_mode = AI_GOAL_CHASE_WING;
					ai_shipname = Wings[shipp->wingnum].name;
					ai_submode = SM_ATTACK;
				}
			}
			break;
		}
		case LE_ORDER_GUARD_WING:
		{
			if(tgh_valid && tgh->objp->type == OBJ_SHIP)
			{
				ship *shipp = &Ships[tgh->objp->instance];
				if (shipp->wingnum != -1)
				{
					ai_mode = AI_GOAL_GUARD_WING;
					ai_shipname = Wings[shipp->wingnum].name;
					ai_submode = AIS_GUARD_STATIC;
				}
			}
			break;
		}
		case LE_ORDER_ATTACK_SHIP_CLASS:
		{
			if(sclass >= 0)
			{
				ai_mode = AI_GOAL_CHASE_SHIP_CLASS;
				ai_shipname = Ship_info[sclass].name;
				ai_submode = SM_ATTACK;
			}
			break;
		}
	}

	//Nothing got set!
	if(ai_mode == AI_GOAL_NONE)
		return ade_set_error(L, "b", false);

	//Fire off the goal
	ai_add_ship_goal_scripting(ai_mode, ai_submode, (int)(priority*100.0f), ai_shipname, &Ai_info[Ships[objh->objp->instance].ai_index]);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(doManeuver, l_Ship, "number Duration, number Heading, number Pitch, number Bank, boolean Force Rotation, number Vertical, number Horizontal, number Forward, boolean Force Movement", "Sets ship maneuver over the defined time period", "boolean", "True if maneuver order was given, otherwise false or nil")
{
	object_h *objh;
	float arr[6];
	bool f_rot = false, f_move = false;
	int t, i;
	if(!ade_get_args(L, "oifffbfffb", l_Ship.GetPtr(&objh), &t, &arr[0], &arr[1], &arr[2], &f_rot, &arr[3], &arr[4], &arr[5], &f_move))
		return ADE_RETURN_NIL;

	ship *shipp = &Ships[objh->objp->instance];
	ai_info *aip = &Ai_info[shipp->ai_index];
	control_info *cip = &aip->ai_override_ci;

	aip->ai_override_timestamp = timestamp(t);
	aip->ai_override_flags.reset();

	if (t < 2)
		return ADE_RETURN_FALSE;

	for(i = 0; i < 6; i++) {
		if((arr[i] < -1.0f) || (arr[i] > 1.0f))
			arr[i] = 0;
	}

	if(f_rot) {
		aip->ai_override_flags.set(AI::Maneuver_Override_Flags::Full);
		cip->heading = arr[0];
		cip->pitch = arr[1];
		cip->bank = arr[2];
	} else {
		if (arr[0] != 0) {
			cip->heading = arr[0];
			aip->ai_override_flags.set(AI::Maneuver_Override_Flags::Heading);
		} 
		if (arr[1] != 0) {
			cip->pitch = arr[1];
			aip->ai_override_flags.set(AI::Maneuver_Override_Flags::Pitch);
		} 
		if (arr[2] != 0) {
			cip->bank = arr[2];
			aip->ai_override_flags.set(AI::Maneuver_Override_Flags::Roll);
		} 
	}
	if(f_move) {
		aip->ai_override_flags.set(AI::Maneuver_Override_Flags::Full_lat);
		cip->vertical = arr[3];
		cip->sideways = arr[4];
		cip->forward = arr[5];	
	} else {
		if (arr[3] != 0) {
			cip->vertical = arr[3];
			aip->ai_override_flags.set(AI::Maneuver_Override_Flags::Up);
		} 
		if (arr[4] != 0) {
			cip->sideways = arr[4];
			aip->ai_override_flags.set(AI::Maneuver_Override_Flags::Sideways);
		} 
		if (arr[5] != 0) {
			cip->forward = arr[5];
			aip->ai_override_flags.set(AI::Maneuver_Override_Flags::Forward);
		} 
	}
	return ADE_RETURN_TRUE;
}

ADE_FUNC(triggerAnimation, l_Ship, "string Type, [number Subtype, boolean Forwards, boolean Instant]",
		 "Triggers an animation. Type is the string name of the animation type, "
		 "Subtype is the subtype number, such as weapon bank #, Forwards and Instant are boolean, defaulting to true & false respectively."
		 "<br><strong>IMPORTANT: Function is in testing and should not be used with official mod releases</strong>",
		 "boolean",
		 "True if successful, false or nil otherwise")
{
	object_h *objh;
	char *s = NULL;
	bool b = true;
	bool instant = false;
	int subtype=-1;
	if(!ade_get_args(L, "o|sibb", l_Ship.GetPtr(&objh), &s, &subtype, &b, &instant))
		return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	int type = model_anim_match_type(s);
	if(type < 0)
		return ADE_RETURN_FALSE;

	int dir = 1;
	if(!b)
		dir = -1;

	model_anim_start_type(&Ships[objh->objp->instance], type, subtype, dir, instant);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(warpIn, l_Ship, NULL, "Warps ship in", "boolean", "True if successful, or nil if ship handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	shipfx_warpin_start(objh->objp);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(warpOut, l_Ship, NULL, "Warps ship out", "boolean", "True if successful, or nil if ship handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	shipfx_warpout_start(objh->objp);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(canWarp, l_Ship, NULL, "Checks whether ship has a working subspace drive and is allowed to use it", "boolean", "True if successful, or nil if ship handle is invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	ship *shipp = &Ships[objh->objp->instance];
	if(shipp->flags[Ship::Ship_Flags::No_subspace_drive]){
		return ADE_RETURN_FALSE;
	}

	return ADE_RETURN_TRUE;
}

// Aardwolf's function for finding if a ship should be drawn as blue on the radar/minimap
ADE_FUNC(isWarpingIn, l_Ship, NULL, "Checks if ship is warping in", "boolean", "True if the ship is warping in, false or nil otherwise")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
	return ADE_RETURN_NIL;

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	ship *shipp = &Ships[objh->objp->instance];
	if(shipp->flags[Ship::Ship_Flags::Arriving_stage_1]){
		return ADE_RETURN_TRUE;
	}

	return ADE_RETURN_FALSE;
}

ADE_FUNC(getEMP, l_Ship, NULL, "Returns the current emp effect strength acting on the object", "number", "Current EMP effect strength or NIL if object is invalid")
{
	object_h *objh = NULL;
	object *obj = NULL;

	if (!ade_get_args(L, "o", l_Ship.GetPtr(&objh))) {
		return ADE_RETURN_NIL;
	}

	if(!objh->IsValid())
		return ADE_RETURN_NIL;

	obj = objh->objp;

	ship *shipp = &Ships[obj->instance];

	return ade_set_args(L, "f", shipp->emp_intensity);
}

ADE_FUNC(getTimeUntilExplosion, l_Ship, NULL, "Returns the time in seconds until the ship explodes", "number", "Time until explosion or -1, if invalid handle or ship isn't exploding")
{
	object_h *objh = NULL;

	if (!ade_get_args(L, "o", l_Ship.GetPtr(&objh))) {
		return ade_set_error(L, "f", -1.0f);
	}

	if(!objh->IsValid())
		return ade_set_error(L, "f", -1.0f);

	ship *shipp = &Ships[objh->objp->instance];

	if (!timestamp_valid(shipp->final_death_time))
	{
		return ade_set_args(L, "f", -1.0f);
	}

	int time_until = timestamp_until(shipp->final_death_time);

	return ade_set_args(L, "f", (i2fl(time_until) / 1000.0f));
}

ADE_FUNC(getCallsign, l_Ship, NULL, "Gets the callsign of the ship in the current mission", "string", "The callsign or an empty string if the ship doesn't have a callsign or an error occurs")
{
	object_h *objh = NULL;

	if (!ade_get_args(L, "o", l_Ship.GetPtr(&objh))) {
		return ade_set_error(L, "s", "");
	}

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	ship *shipp = &Ships[objh->objp->instance];

	if (shipp->callsign_index < 0)
		return ade_set_args(L, "s", "");
	
	char temp_callsign[NAME_LENGTH];
	
	*temp_callsign = 0;
	mission_parse_lookup_callsign_index(shipp->callsign_index, temp_callsign);

	if (*temp_callsign)
		return ade_set_args(L, "s", temp_callsign);
	else
		return ade_set_args(L, "s", "");
}

ADE_FUNC(getAltClassName, l_Ship, NULL, "Gets the alternate class name of the ship", "string", "The alternate class name or an empty string if the ship doesn't have such a thing or an error occurs")
{
	object_h *objh = NULL;

	if (!ade_get_args(L, "o", l_Ship.GetPtr(&objh))) {
		return ade_set_error(L, "s", "");
	}

	if(!objh->IsValid())
		return ade_set_error(L, "s", "");

	ship *shipp = &Ships[objh->objp->instance];

	if (shipp->alt_type_index < 0)
		return ade_set_args(L, "s", "");
	
	char temp[NAME_LENGTH];
	
	*temp = 0;
	mission_parse_lookup_alt_index(shipp->alt_type_index, temp);

	if (*temp)
		return ade_set_args(L, "s", temp);
	else
		return ade_set_args(L, "s", "");
}

ADE_FUNC(getMaximumSpeed, l_Ship, "[number energy = 0.333]", "Gets the maximum speed of the ship with the given energy on the engines", "number", "The maximum speed or -1 on error")
{
	object_h *objh = NULL;
	float energy = 0.333f;

	if (!ade_get_args(L, "o|f", l_Ship.GetPtr(&objh), &energy)) {
		return ade_set_error(L, "f", -1.0f);
	}

	if(!objh->IsValid())
		return ade_set_error(L, "f", -1.0f);

	if (energy < 0.0f || energy > 1.0f)
	{
		LuaError(L, "Invalid energy level %f! Needs to be in [0, 1].", energy);

		return ade_set_args(L, "f", -1.0f);
	}
	else
	{
		return ade_set_args(L, "f", ets_get_max_speed(objh->objp, energy));
	}
}

ADE_FUNC(EtsSetIndexes, l_Ship, "number Engine Index, number Shield Index, number Weapon Index",
		"Sets ships ETS systems to specified values",
		"boolean",
		"True if successful, false if target ships ETS was missing, or only has one system")
{
	object_h *objh=NULL;
	int ets_idx[num_retail_ets_gauges] = {0};

	if (!ade_get_args(L, "oiii", l_Ship.GetPtr(&objh), &ets_idx[ENGINES], &ets_idx[SHIELDS], &ets_idx[WEAPONS]))
		return ADE_RETURN_FALSE;

	if (!objh->IsValid())
		return ADE_RETURN_FALSE;

	sanity_check_ets_inputs(ets_idx);

	int sindex = objh->objp->instance;
	if (validate_ship_ets_indxes(sindex, ets_idx)) {
		Ships[sindex].engine_recharge_index = ets_idx[ENGINES];
		Ships[sindex].shield_recharge_index = ets_idx[SHIELDS];
		Ships[sindex].weapon_recharge_index = ets_idx[WEAPONS];
		return ADE_RETURN_TRUE;
	} else {
		return ADE_RETURN_FALSE;
	}
}

ADE_FUNC(getWing, l_Ship, NULL, "Returns the ship's wing", "wing", "Wing handle, or invalid wing handle if ship is not part of a wing")
{
	object_h *objh = NULL;
	ship *shipp = NULL;

	if (!ade_get_args(L, "o", l_Ship.GetPtr(&objh)))
		return ade_set_error(L, "o", l_Wing.Set(-1));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Wing.Set(-1));

	shipp = &Ships[objh->objp->instance];
	return ade_set_args(L, "o", l_Wing.Set(shipp->wingnum));
}

//**********HANDLE: Weapon
ade_obj<object_h> l_Weapon("weapon", "Weapon handle", &l_Object);

ADE_VIRTVAR(Class, l_Weapon, "weaponclass", "Weapon's class", "weaponclass", "Weapon class, or invalid weaponclass handle if weapon handle is invalid")
{
	object_h *oh=NULL;
	int nc=-1;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&oh), l_Weaponclass.Get(&nc)))
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	weapon *wp = &Weapons[oh->objp->instance];

	if(ADE_SETTING_VAR && nc > -1) {
		wp->weapon_info_index = nc;
	}

	return ade_set_args(L, "o", l_Weaponclass.Set(wp->weapon_info_index));
}

ADE_VIRTVAR(DestroyedByWeapon, l_Weapon, "boolean", "Whether weapon was destroyed by another weapon", "boolean", "True if weapon was destroyed by another weapon, false if weapon was destroyed by another object or if weapon handle is invalid")
{
	object_h *oh=NULL;
	bool b = false;

	int numargs = ade_get_args(L, "o|b", l_Weapon.GetPtr(&oh), &b);
	
	if(!numargs)
		return ade_set_error(L, "b", false);

	if(!oh->IsValid())
		return ade_set_error(L, "b", false);

	weapon *wp = &Weapons[oh->objp->instance];

	if(ADE_SETTING_VAR && numargs > 1) {
		wp->weapon_flags.set(Weapon::Weapon_Flags::Destroyed_by_weapon, b);
	}

	return ade_set_args(L, "b", wp->weapon_flags[Weapon::Weapon_Flags::Destroyed_by_weapon]);
}

ADE_VIRTVAR(LifeLeft, l_Weapon, "number", "Weapon life left (in seconds)", "number", "Life left (seconds) or 0 if weapon handle is invalid")
{
	object_h *oh=NULL;
	float nll = -1.0f;
	if(!ade_get_args(L, "o|f", l_Weapon.GetPtr(&oh), &nll))
		return ade_set_error(L, "f", 0.0f);

	if(!oh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	weapon *wp = &Weapons[oh->objp->instance];

	if(ADE_SETTING_VAR && nll >= 0.0f) {
		wp->lifeleft = nll;
	}

	return ade_set_args(L, "f", wp->lifeleft);
}

ADE_VIRTVAR(FlakDetonationRange, l_Weapon, "number", "Range at which flak will detonate (meters)", "number", "Detonation range (meters) or 0 if weapon handle is invalid")
{
	object_h *oh=NULL;
	float rng = -1.0f;
	if(!ade_get_args(L, "o|f", l_Weapon.GetPtr(&oh), &rng))
		return ade_set_error(L, "f", 0.0f);

	if(!oh->IsValid())
		return ade_set_error(L, "f", 0.0f);

	weapon *wp = &Weapons[oh->objp->instance];

	if(ADE_SETTING_VAR && rng >= 0.0f) {
		wp->det_range = rng;
	}

	return ade_set_args(L, "f", wp->det_range);
}

ADE_VIRTVAR(Target, l_Weapon, "object", "Target of weapon. Value may also be a deriviative of the 'object' class, such as 'ship'.", "object", "Weapon target, or invalid object handle if weapon handle is invalid")
{
	object_h *objh;
	object_h *newh = nullptr;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&objh), l_Object.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	weapon *wp = NULL;
	if(objh->objp->instance > -1)
		wp = &Weapons[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh && newh->IsValid())
		{
			if(wp->target_sig != newh->sig)
			{
				weapon_set_tracking_info(OBJ_INDEX(objh->objp), objh->objp->parent, OBJ_INDEX(newh->objp), 1);
			}
		}
		else
		{
			weapon_set_tracking_info(OBJ_INDEX(objh->objp), objh->objp->parent, -1);
		}
	}

	return ade_set_object_with_breed(L, wp->target_num);
}

ADE_VIRTVAR(ParentTurret, l_Weapon, "subsystem", "Turret which fired this weapon.", "subsystem", "Turret subsystem handle, or an invalid handle if the weapon not fired from a turret")
{
	object_h *objh;
	ship_subsys_h *newh = nullptr;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&objh), l_Subsystem.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	weapon *wp = NULL;
	if(objh->objp->instance > -1)
		wp = &Weapons[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh && newh->IsValid())
		{
			if(wp->turret_subsys != newh->ss)
			{
				wp->turret_subsys = newh->ss;
			}
		}
		else
		{
			wp->turret_subsys = NULL;
		}
	}

    if(wp->turret_subsys == NULL)
        return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));
    else
        return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(&Objects[wp->turret_subsys->parent_objnum], wp->turret_subsys)));
}

ADE_VIRTVAR(HomingObject, l_Weapon, "object", "Object that weapon will home in on. Value may also be a deriviative of the 'object' class, such as 'ship'", "object", "Object that weapon is homing in on, or an invalid object handle if weapon is not homing or the weapon handle is invalid")
{
	object_h *objh;
	object_h *newh = nullptr;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&objh), l_Object.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	weapon *wp = NULL;
	if(objh->objp->instance > -1)
		wp = &Weapons[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR)
	{
		if (newh && newh->IsValid())
		{
			if (wp->target_sig != newh->sig)
			{
				weapon_set_tracking_info(OBJ_INDEX(objh->objp), objh->objp->parent, OBJ_INDEX(newh->objp), 1);
			}
		}
		else
		{
			weapon_set_tracking_info(OBJ_INDEX(objh->objp), objh->objp->parent, -1);
		}
	}

	if(wp->homing_object == &obj_used_list)
		return ade_set_args(L, "o", l_Object.Set(object_h()));
	else
		return ade_set_object_with_breed(L, OBJ_INDEX(wp->homing_object));
}

ADE_VIRTVAR(HomingPosition, l_Weapon, "vector", "Position that weapon will home in on (World vector), setting this without a homing object in place will not have any effect!",
	"vector", "Homing point, or null vector if weapon handle is invalid")
{
	object_h *objh;
	vec3d *v3 = nullptr;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&objh), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	weapon *wp = NULL;
	if(objh->objp->instance > -1)
		wp = &Weapons[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR)
	{
		if(v3)
		{
			wp->homing_pos = *v3;
		}
		else
		{
			wp->homing_pos = vmd_zero_vector;
		}
	}

	return ade_set_args(L, "o", l_Vector.Set(wp->homing_pos));
}

ADE_VIRTVAR(HomingSubsystem, l_Weapon, "subsystem", "Subsystem that weapon will home in on.", "subsystem", "Homing subsystem, or invalid subsystem handle if weapon is not homing or weapon handle is invalid")
{
	object_h *objh;
	ship_subsys_h *newh = nullptr;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&objh), l_Subsystem.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	weapon *wp = NULL;
	if(objh->objp->instance > -1)
		wp = &Weapons[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh && newh->IsValid())
		{
			if(wp->target_sig != newh->sig)
			{
				wp->homing_object = newh->objp;
				wp->homing_subsys = newh->ss;
				get_subsystem_pos(&wp->homing_pos, wp->homing_object, wp->homing_subsys);
			}
		}
		else
		{
			wp->homing_object = &obj_used_list;
			wp->homing_pos = vmd_zero_vector;
			wp->homing_subsys = NULL;
		}
	}

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(wp->homing_object, wp->homing_subsys)));
}

ADE_VIRTVAR(Team, l_Weapon, "team", "Weapon's team", "team", "Weapon team, or invalid team handle if weapon handle is invalid")
{
	object_h *oh=NULL;
	int nt=-1;
	if(!ade_get_args(L, "o|o", l_Weapon.GetPtr(&oh), l_Team.Get(&nt)))
		return ade_set_error(L, "o", l_Team.Set(-1));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Team.Set(-1));

	weapon *wp = &Weapons[oh->objp->instance];

	if(ADE_SETTING_VAR && nt > -1 && nt < Num_teams) {
		wp->team = nt;
	}

	return ade_set_args(L, "o", l_Team.Set(wp->team));
}

ADE_FUNC(isArmed, l_Weapon, "[boolean Hit target]", "Checks if the weapon is armed.", "boolean", "boolean value of the weapon arming status")
{
	object_h *oh = NULL;
	bool hit_target = false;
	if(!ade_get_args(L, "o|b", l_Weapon.GetPtr(&oh), &hit_target))
		return ADE_RETURN_FALSE;

	if(!oh->IsValid())
		return ADE_RETURN_FALSE;

	weapon *wp = &Weapons[oh->objp->instance];
	
	if(weapon_armed(wp, hit_target))
		return ADE_RETURN_TRUE;
	
	return ADE_RETURN_FALSE;
}

ADE_FUNC(getCollisionInformation, l_Weapon, NULL, "Returns the collision information for this weapon", "collision info", "The collision information or invalid handle if none")
{
	object_h *oh=NULL;
	if(!ade_get_args(L, "o", l_Weapon.GetPtr(&oh)))
		return ADE_RETURN_NIL;

	if(!oh->IsValid())
		return ADE_RETURN_NIL;

	weapon *wp = &Weapons[oh->objp->instance];
	
	if (wp->collisionInfo != nullptr)
		return ade_set_args(L, "o", l_ColInfo.Set(mc_info_h(new mc_info(*(wp->collisionInfo)))));
	else
		return ade_set_args(L, "o", l_ColInfo.Set(mc_info_h()));
}


//**********HANDLE: Beam
ade_obj<object_h> l_Beam("beam", "Beam handle", &l_Object);

ADE_VIRTVAR(Class, l_Beam, "weaponclass", "Weapon's class", "weaponclass", "Weapon class, or invalid weaponclass handle if beam handle is invalid")
{
	object_h *oh=NULL;
	int nc=-1;
	if(!ade_get_args(L, "o|o", l_Beam.GetPtr(&oh), l_Weaponclass.Get(&nc)))
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	beam *bp = &Beams[oh->objp->instance];

	if(ADE_SETTING_VAR && nc > -1) {
		bp->weapon_info_index = nc;
	}

	return ade_set_args(L, "o", l_Weaponclass.Set(bp->weapon_info_index));
}

ADE_VIRTVAR(LastShot, l_Beam, "vector", "End point of the beam", "vector", "vector or null vector if beam handle is not valid")
{
	object_h *oh=NULL;
	vec3d *vec3 = nullptr;
	if(!ade_get_args(L, "o|o", l_Beam.GetPtr(&oh), l_Vector.GetPtr(&vec3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	beam *bp = &Beams[oh->objp->instance];

	if(ADE_SETTING_VAR && vec3) {
		bp->last_shot = *vec3;
	}

	return ade_set_args(L, "o", l_Vector.Set(bp->last_shot));
}

ADE_VIRTVAR(LastStart, l_Beam, "vector", "Start point of the beam", "vector", "vector or null vector if beam handle is not valid")
{
	object_h *oh=NULL;
	vec3d *v3 = nullptr;
	if(!ade_get_args(L, "o|o", l_Beam.GetPtr(&oh), l_Vector.GetPtr(&v3)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!oh->IsValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	beam *bp = &Beams[oh->objp->instance];

	if(ADE_SETTING_VAR && v3) {
		bp->last_start = *v3;
	}

	return ade_set_args(L, "o", l_Vector.Set(bp->last_start));
}

ADE_VIRTVAR(Target, l_Beam, "object", "Target of beam. Value may also be a deriviative of the 'object' class, such as 'ship'.", "object", "Beam target, or invalid object handle if beam handle is invalid")
{
	object_h *objh;
	object_h *newh = nullptr;
	if(!ade_get_args(L, "o|o", l_Beam.GetPtr(&objh), l_Object.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh && newh->IsValid())
		{
			if(bp->target_sig != newh->sig)
			{
				bp->target = newh->objp;
				bp->target_sig = newh->sig;
			}
		}
		else
		{
			bp->target = NULL;
			bp->target_sig = 0;
		}
	}

	return ade_set_object_with_breed(L, OBJ_INDEX(bp->target));
}

ADE_VIRTVAR(TargetSubsystem, l_Beam, "subsystem", "Subsystem that beam is targeting.", "subsystem", "Target subsystem, or invalid subsystem handle if beam handle is invalid")
{
	object_h *objh;
	ship_subsys_h *newh = nullptr;
	if(!ade_get_args(L, "o|o", l_Beam.GetPtr(&objh), l_Subsystem.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh && newh->IsValid())
		{
			if(bp->target_sig != newh->sig)
			{
				bp->target = newh->objp;
				bp->target_subsys = newh->ss;
				bp->target_sig = newh->sig;
			}
		}
		else
		{
			bp->target = NULL;
			bp->target_subsys = NULL;
		}
	}

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(bp->target, bp->target_subsys)));
}

ADE_VIRTVAR(ParentShip, l_Beam, "object", "Parent of the beam.", "object", "Beam parent, or invalid object handle if beam handle is invalid")
{
	object_h *objh;
	object_h *newh = nullptr;
	if(!ade_get_args(L, "o|o", l_Beam.GetPtr(&objh), l_Object.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh && newh->IsValid())
		{
			if(bp->sig != newh->sig)
			{
				bp->objp = newh->objp;
				bp->sig = newh->sig;
			}
		}
		else
		{
			bp->objp = NULL;
			bp->sig = 0;
		}
	}

	return ade_set_object_with_breed(L, OBJ_INDEX(bp->objp));
}

ADE_VIRTVAR(ParentSubsystem, l_Beam, "subsystem", "Subsystem that beam is fired from.", "subsystem", "Parent subsystem, or invalid subsystem handle if beam handle is invalid")
{
	object_h *objh;
	ship_subsys_h *newh = nullptr;
	if(!ade_get_args(L, "o|o", l_Beam.GetPtr(&objh), l_Subsystem.GetPtr(&newh)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!objh->IsValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR)
	{
		if(newh && newh->IsValid())
		{
			if(bp->sig != newh->sig)
			{
				bp->objp = newh->objp;
				bp->subsys = newh->ss;
			}
		}
		else
		{
			bp->objp = NULL;
			bp->subsys = NULL;
		}
	}

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(bp->objp, bp->subsys)));
}

ADE_FUNC(getCollisionCount, l_Beam, NULL, "Get the number of collisions in frame.", "number", "Number of beam collisions")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Beam.GetPtr(&objh)))
		return ADE_RETURN_NIL;

	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ADE_RETURN_NIL;

	return ade_set_args(L, "i", bp->f_collision_count);
}

ADE_FUNC(getCollisionPosition, l_Beam, "number", "Get the position of the defined collision.", "vector", "World vector")
{
	object_h *objh;
	int idx;
	if(!ade_get_args(L, "oi", l_Beam.GetPtr(&objh), &idx))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	// convert from Lua to C
	idx--;
	if ((idx >= MAX_FRAME_COLLISIONS) || (idx < 0))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	// so we have valid beam and valid indexer
	return ade_set_args(L, "o", l_Vector.Set(bp->f_collisions[idx].cinfo.hit_point_world));
}

ADE_FUNC(getCollisionInformation, l_Beam, "number", "Get the collision information of the specified collision", "collision info", "handle to information or invalid handle on error")
{
	object_h *objh;
	int idx;
	if(!ade_get_args(L, "oi", l_Beam.GetPtr(&objh), &idx))
		return ade_set_error(L, "o", l_ColInfo.Set(mc_info_h()));

	// convert from Lua to C
	idx--;
	if ((idx >= MAX_FRAME_COLLISIONS) || (idx < 0))
		return ade_set_error(L, "o", l_ColInfo.Set(mc_info_h()));

	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_ColInfo.Set(mc_info_h()));

	// so we have valid beam and valid indexer
	return ade_set_args(L, "o", l_ColInfo.Set(mc_info_h(new mc_info(bp->f_collisions[idx].cinfo))));
}

ADE_FUNC(getCollisionObject, l_Beam, "number", "Get the target of the defined collision.", "object", "Object the beam collided with")
{
	object_h *objh;
	int idx;
	if(!ade_get_args(L, "oi", l_Beam.GetPtr(&objh), &idx))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	// convert from Lua to C
	idx--;
	if ((idx >= MAX_FRAME_COLLISIONS) || (idx < 0))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	// so we have valid beam and valid indexer
	return ade_set_object_with_breed(L, bp->f_collisions[idx].c_objnum);
}

ADE_FUNC(isExitCollision, l_Beam, "number", "Checks if the defined collision was exit collision.", "boolean", "True if the collision was exit collision, false if entry, nil otherwise")
{
	object_h *objh;
	int idx;
	if(!ade_get_args(L, "oi", l_Beam.GetPtr(&objh), &idx))
		return ADE_RETURN_NIL;

	// convert from Lua to C
	idx--;
	if ((idx >= MAX_FRAME_COLLISIONS) || (idx < 0))
		return ADE_RETURN_NIL;

	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ADE_RETURN_NIL;

	// so we have valid beam and valid indexer
	if (bp->f_collisions[idx].is_exit_collision)
		return ADE_RETURN_TRUE;
	
	return ADE_RETURN_FALSE;
}

ADE_FUNC(getStartDirectionInfo, l_Beam, NULL, "Gets the start information about the direction. The vector is a normalized vector from LastStart showing the start direction of a slashing beam", "vector", "The start direction or null vector if invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Beam.GetPtr(&objh)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	
	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	beam_info inf = bp->binfo;
		
	return ade_set_args(L, "o", l_Vector.Set(inf.dir_a));
}

ADE_FUNC(getEndDirectionInfo, l_Beam, NULL, "Gets the end information about the direction. The vector is a normalized vector from LastStart showing the end direction of a slashing beam", "vector", "The start direction or null vector if invalid")
{
	object_h *objh;
	if(!ade_get_args(L, "o", l_Beam.GetPtr(&objh)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	
	beam *bp = NULL;
	if(objh->objp->instance > -1)
		bp = &Beams[objh->objp->instance];
	else
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	beam_info inf = bp->binfo;
		
	return ade_set_args(L, "o", l_Vector.Set(inf.dir_b));
}

//**********HANDLE: Player
ade_obj<int> l_Player("player", "Player handle");

ADE_FUNC(isValid, l_Player, NULL, "Detects whether handle is valid", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ADE_RETURN_NIL;

	if (idx < 0 || idx >= MAX_PLAYERS)
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getName, l_Player, NULL, "Gets current player name", "string", "Player name, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "s", "");

	if (idx < 0 || idx >= MAX_PLAYERS)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Players[idx].callsign);
}

ADE_FUNC(getCampaignFilename, l_Player, NULL, "Gets current player campaign filename", "string", "Campaign name, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "s", "");

	if (idx < 0 || idx >= MAX_PLAYERS)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Players[idx].current_campaign);
}

ADE_FUNC(getImageFilename, l_Player, NULL, "Gets current player image filename", "string", "Player image filename, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "s", "");

	if (idx < 0 || idx >= MAX_PLAYERS)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Players[idx].image_filename);
}

ADE_FUNC(getMainHallName, l_Player, NULL, "Gets player's current main hall name", "string", "Main hall name, or name of first mainhall in campaign if something goes wrong")
{
	SCP_string hallname;
	// FS2-->Lua
	if (Campaign.next_mission == -1) {
		hallname = Campaign.missions[0].main_hall;
	} else {
		hallname = Campaign.missions[Campaign.next_mission].main_hall;
	}

	return ade_set_args(L, "i", hallname.c_str());
}

// use getMainHallName if at all possible.
ADE_FUNC(getMainHallIndex, l_Player, NULL, "Gets player's current main hall number", "number", "Main hall index, or index of first mainhall in campaign if something goes wrong")
{
	int hallnum = 0;
	//FS2-->Lua
	if (Campaign.next_mission == -1) {
		hallnum = main_hall_get_index(Campaign.missions[0].main_hall);
	} else {
		hallnum = main_hall_get_index(Campaign.missions[Campaign.next_mission].main_hall);
	}

	return ade_set_args(L, "i", hallnum);
}

ADE_FUNC(getSquadronName, l_Player, NULL, "Gets current player squad name", "string", "Squadron name, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "s", "");

	if (idx < 0 || idx >= MAX_PLAYERS)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Players[idx].s_squad_name);
}

ADE_FUNC(getMultiSquadronName, l_Player, NULL, "Gets current player multi squad name", "string", "Squadron name, or empty string if handle is invalid")
{
	int idx;
	if(!ade_get_args(L, "o", l_Player.Get(&idx)))
		return ade_set_error(L, "s", "");

	if (idx < 0 || idx >= MAX_PLAYERS)
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", Players[idx].m_squad_name);
}

//WMC - This isn't working
/*
ADE_FUNC(getSquadronImage, l_Player, NULL, "Squad image (string)", "Gets current player squad image")
{
	int idx;
	player_helper(L, &idx);

	return ade_set_args(L, "s", Players[idx].squad_filename);
}*/

//**********HANDLE: Camera
ade_obj<camid> l_Camera("camera", "Camera handle");

ADE_FUNC(__tostring, l_Camera, NULL, "Camera name", "string", "Camera name, or an empty string if handle is invalid")
{
	camid cid;
	if(!ade_get_args(L, "o", l_Camera.Get(&cid)))
		return ade_set_error(L, "s", "");

	if(!cid.isValid())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", cid.getCamera()->get_name());
}

ADE_FUNC(isValid, l_Camera, NULL, "True if valid, false or nil if not", "boolean", "true if valid, false if handle is invalid, nil if a syntax/type error occurs")
{
	camid cid;
	if(!ade_get_args(L, "o", l_Camera.Get(&cid)))
		return ADE_RETURN_NIL;

	if(!cid.isValid())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_VIRTVAR(Name, l_Camera, "string", "New camera name", "string", "Camera name")
{
	camid cid;
	char *s = NULL;
	if(!ade_get_args(L, "o|s", l_Camera.Get(&cid), &s))
		return ade_set_error(L, "s", "");

	if(!cid.isValid())
		return ade_set_error(L, "s", "");

	if(ADE_SETTING_VAR && s != NULL) {
		cid.getCamera()->set_name(s);
	}

	return ade_set_args(L, "s", cid.getCamera()->get_name());
}

ADE_VIRTVAR(FOV, l_Camera, "number", "New camera FOV (in radians)", "number", "Camera FOV (in radians)")
{
	camid cid;
	float f = VIEWER_ZOOM_DEFAULT;
	if(!ade_get_args(L, "o|f", l_Camera.Get(&cid), &f))
		return ade_set_error(L, "f", 0.0f);

	if(!cid.isValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR) {
		cid.getCamera()->set_fov(f);
	}

	return ade_set_args(L, "f", cid.getCamera()->get_fov());
}

ADE_VIRTVAR(Orientation, l_Camera, "orientation", "New camera orientation", "orientation", "Camera orientation")
{
	camid cid;
	matrix_h *mh = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Matrix.GetPtr(&mh)))
		return ade_set_error(L, "f", 0.0f);

	if(!cid.isValid())
		return ade_set_error(L, "f", 0.0f);

	if(ADE_SETTING_VAR && mh != NULL) {
		cid.getCamera()->set_rotation(mh->GetMatrix());
	}

	matrix mtx;
	cid.getCamera()->get_info(NULL, &mtx);
	return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&mtx)));
}

ADE_VIRTVAR(Position, l_Camera, "vector", "New camera position", "vector", "Camera position")
{
	camid cid;
	vec3d *pos = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Vector.GetPtr(&pos)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if(ADE_SETTING_VAR && pos) {
		cid.getCamera()->set_position(pos);
	}

	vec3d v = vmd_zero_vector;
	cid.getCamera()->get_info(&v, NULL);
	return ade_set_args(L, "o", l_Vector.Set(v));
}

ADE_VIRTVAR(Self, l_Camera, "object", "New mount object", "object", "Camera object")
{
	camid cid;
	object_h *oh = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Object.GetPtr(&oh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR && oh && oh->IsValid()) {
		cid.getCamera()->set_object_host(oh->objp);
	}

	return ade_set_args(L, "o", l_Object.Set(object_h(cid.getCamera()->get_object_host())));
}

ADE_VIRTVAR(SelfSubsystem, l_Camera, "subsystem", "New mount object subsystem", "subsystem", "Subsystem that the camera is mounted on")
{
	camid cid;
	ship_subsys_h *sso = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR && sso && sso->IsValid()) {
		cid.getCamera()->set_object_host(sso->objp, sso->ss->system_info->subobj_num);
	}

	object *objp = cid.getCamera()->get_object_host();
	if(objp == NULL || objp->type != OBJ_SHIP)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	int submodel = cid.getCamera()->get_object_host_submodel();
	if(submodel < 0)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	ship *shipp = &Ships[objp->instance];
	polymodel *pm = model_get(Ship_info[shipp->ship_info_index].model_num);

	if(pm == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	bsp_info *sm = &pm->submodel[submodel];

	ship_subsys *ss = ship_get_subsys(shipp, sm->name);
	
	if(ss == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(objp, ss)));
}

ADE_VIRTVAR(Target, l_Camera, "object", "New target object", "object", "Camera target object")
{
	camid cid;
	object_h *oh = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Object.GetPtr(&oh)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if(ADE_SETTING_VAR && oh && oh->IsValid()) {
		cid.getCamera()->set_object_target(oh->objp);
	}

	return ade_set_args(L, "o", l_Object.Set(object_h(cid.getCamera()->get_object_target())));
}

ADE_VIRTVAR(TargetSubsystem, l_Camera, "subsystem", "New target subsystem", "subsystem", "Subsystem that the camera is pointed at")
{
	camid cid;
	ship_subsys_h *sso = NULL;
	if(!ade_get_args(L, "o|o", l_Camera.Get(&cid), l_Subsystem.GetPtr(&sso)))
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(!cid.isValid())
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	if(ADE_SETTING_VAR && sso && sso->IsValid()) {
		cid.getCamera()->set_object_target(sso->objp, sso->ss->system_info->subobj_num);
	}

	object *objp = cid.getCamera()->get_object_target();
	if(objp == NULL || objp->type != OBJ_SHIP)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	int submodel = cid.getCamera()->get_object_target_submodel();
	if(submodel < 0)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	ship *shipp = &Ships[objp->instance];
	polymodel *pm = model_get(Ship_info[shipp->ship_info_index].model_num);

	if(pm == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	bsp_info *sm = &pm->submodel[submodel];

	ship_subsys *ss = ship_get_subsys(shipp, sm->name);
	
	if(ss == NULL)
		return ade_set_error(L, "o", l_Subsystem.Set(ship_subsys_h()));

	return ade_set_args(L, "o", l_Subsystem.Set(ship_subsys_h(objp, ss)));
}

ADE_FUNC(setFOV, l_Camera, "[number FOV, number Zoom Time, number Zoom Acceleration Time, number Zoom deceleration Time]",
		 "Sets camera FOV"
		 "<br>FOV is the final field of view, in radians, of the camera."
		 "<br>Zoom Time is the total time to take zooming in or out."
		 "<br>Acceleration Time is the total time it should take the camera to get up to full zoom speed."
		 "<br>Deceleration Time is the total time it should take the camera to slow down from full zoom speed.",
		 "boolean", "true if successful, false or nil otherwise")
{
	camid cid;
	float n_fov = VIEWER_ZOOM_DEFAULT;
	float time=0.0f;
	float acc_time=0.0f;
	float dec_time=0.0f;
	if(!ade_get_args(L, "o|ffff", l_Camera.Get(&cid), &n_fov, &time, &acc_time, &dec_time))
		return ADE_RETURN_NIL;
	
	if(!cid.isValid())
		return ADE_RETURN_NIL;

	cid.getCamera()->set_fov(n_fov, time, acc_time, dec_time);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(setOrientation, l_Camera, "[world orientation Orientation, number Rotation Time, number Acceleration Time, number Deceleration time]",
		"Sets camera orientation and velocity data."
		"<br>Orientation is the final orientation for the camera, after it has finished moving. If not specified, the camera will simply stop at its current orientation."
		"<br>Rotation time (seconds) is how long total, including acceleration, the camera should take to rotate. If it is not specified, the camera will jump to the specified orientation."
		"<br>Acceleration time (seconds) is how long it should take the camera to get 'up to speed'. If not specified, the camera will instantly start moving."
		"<br>Deceleration time (seconds) is how long it should take the camera to slow down. If not specified, the camera will instantly stop moving.",
		"boolean", "true if successful, false or nil otherwise")
{
	camid cid;
	matrix_h *mh=NULL;
	float time=0.0f;
	float acc_time=0.0f;
	float dec_time=0.0f;
	if(!ade_get_args(L, "o|offf", l_Camera.Get(&cid), l_Matrix.GetPtr(&mh), &time, &acc_time, &dec_time))
		return ADE_RETURN_NIL;
	
	if(!cid.isValid())
		return ADE_RETURN_NIL;

	camera *cam = cid.getCamera();

	if(mh != NULL)
	{
		cam->set_rotation(mh->GetMatrix(), time, acc_time, dec_time);
	}
	else
	{
		cam->set_rotation();
	}

	return ADE_RETURN_TRUE;
}

ADE_FUNC(setPosition, l_Camera, "[wvector Position, number Translation Time, number Acceleration Time, number Deceleration Time]",
		"Sets camera position and velocity data."
		"<br>Position is the final position for the camera. If not specified, the camera will simply stop at its current position."
		"<br>Translation time (seconds) is how long total, including acceleration, the camera should take to move. If it is not specified, the camera will jump to the specified position."
		"<br>Acceleration time (seconds) is how long it should take the camera to get 'up to speed'. If not specified, the camera will instantly start moving."
		"<br>Deceleration time (seconds) is how long it should take the camera to slow down. If not specified, the camera will instantly stop moving.",
		"boolean", "true if successful, false or nil otherwise")
{
	camid cid;
	vec3d *pos=NULL;
	float time=0.0f;
	float acc_time=0.0f;
	float dec_time=0.0f;
	if(!ade_get_args(L, "o|offf", l_Camera.Get(&cid), l_Vector.GetPtr(&pos), &time, &acc_time, &dec_time))
		return ADE_RETURN_NIL;
	
	if(!cid.isValid())
		return ADE_RETURN_NIL;

	cid.getCamera()->set_position(pos, time, acc_time, dec_time);

	return ADE_RETURN_TRUE;
}

//**********HANDLE: Control Info
ade_obj<int> l_Control_Info("control info", "control info handle");

ADE_VIRTVAR(Pitch, l_Control_Info, "number", "Pitch of the player ship", "number", "Pitch")
{
	int idx;
	float new_ci = 0.0f;

	if(!ade_get_args(L, "o|f", l_Control_Info.Get(&idx), &new_ci))
		return ade_set_error(L, "f", new_ci);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.pitch = new_ci;
	}

	return ade_set_args(L, "f", Player->lua_ci.pitch);
}

ADE_VIRTVAR(Heading, l_Control_Info, "number", "Heading of the player ship", "number", "Heading")
{
	int idx;
	float new_ci = 0.0f;

	if(!ade_get_args(L, "o|f", l_Control_Info.Get(&idx), &new_ci))
		return ade_set_error(L, "f", new_ci);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.heading = new_ci;
	}

	return ade_set_args(L, "f", Player->lua_ci.heading);
}

ADE_VIRTVAR(Bank, l_Control_Info, "number", "Bank of the player ship", "number", "Bank")
{
	int idx;
	float new_ci = 0.0f;

	if(!ade_get_args(L, "o|f", l_Control_Info.Get(&idx), &new_ci))
		return ade_set_error(L, "f", new_ci);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.bank = new_ci;
	}

	return ade_set_args(L, "f", Player->lua_ci.bank);
}

ADE_VIRTVAR(Vertical, l_Control_Info, "number", "Vertical control of the player ship", "number", "Vertical control")
{
	int idx;
	float new_ci = 0.0f;

	if(!ade_get_args(L, "o|f", l_Control_Info.Get(&idx), &new_ci))
		return ade_set_error(L, "f", new_ci);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.vertical = new_ci;
	}

	return ade_set_args(L, "f", Player->lua_ci.vertical);
}

ADE_VIRTVAR(Sideways, l_Control_Info, "number", "Sideways control of the player ship", "number", "Sideways control")
{
	int idx;
	float new_ci = 0.0f;

	if(!ade_get_args(L, "o|f", l_Control_Info.Get(&idx), &new_ci))
		return ade_set_error(L, "f", new_ci);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.sideways = new_ci;
	}

	return ade_set_args(L, "f", Player->lua_ci.sideways);
}

ADE_VIRTVAR(Forward, l_Control_Info, "number", "Forward control of the player ship", "number", "Forward")
{
	int idx;
	float new_ci = 0.0f;

	if(!ade_get_args(L, "o|f", l_Control_Info.Get(&idx), &new_ci))
		return ade_set_error(L, "f", new_ci);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.forward = new_ci;
	}

	return ade_set_args(L, "f", Player->lua_ci.forward);
}

ADE_VIRTVAR(ForwardCruise, l_Control_Info, "number", "Forward control of the player ship", "number", "Forward")
{
	int idx;
	float new_ci = 0.0f;

	if(!ade_get_args(L, "o|f", l_Control_Info.Get(&idx), &new_ci))
		return ade_set_error(L, "f", new_ci);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.forward_cruise_percent = new_ci*100.0f;
	}

	return ade_set_args(L, "f", Player->lua_ci.forward_cruise_percent*0.01f);
}

ADE_VIRTVAR(PrimaryCount, l_Control_Info, "number", "Number of primary weapons that will fire", "number", "Number of weapons to fire, or 0 if handle is invalid")
{
	int idx;
	int new_pri = 0;

	if(!ade_get_args(L, "o|i", l_Control_Info.Get(&idx), &new_pri))
		return ade_set_error(L, "i", new_pri);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.fire_primary_count = new_pri;
	}

	return ade_set_args(L, "i", Player->lua_ci.fire_primary_count);
}

ADE_VIRTVAR(SecondaryCount, l_Control_Info, "number", "Number of secondary weapons that will fire", "number", "Number of weapons to fire, or 0 if handle is invalid")
{
	int idx;
	int new_sec = 0;

	if(!ade_get_args(L, "o|i", l_Control_Info.Get(&idx), &new_sec))
		return ade_set_error(L, "i", new_sec);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.fire_secondary_count = new_sec;
	}

	return ade_set_args(L, "i", Player->lua_ci.fire_secondary_count);
}

ADE_VIRTVAR(CountermeasureCount, l_Control_Info, "number", "Number of countermeasures that will launch", "number", "Number of countermeasures to launch, or 0 if handle is invalid")
{
	int idx;
	int new_cm = 0;

	if(!ade_get_args(L, "o|i", l_Control_Info.Get(&idx), &new_cm))
		return ade_set_error(L, "i", new_cm);

	if(ADE_SETTING_VAR) {
		Player->lua_ci.fire_countermeasure_count = new_cm;
	}

	return ade_set_args(L, "i", Player->lua_ci.fire_countermeasure_count);
}

ADE_FUNC(clearLuaButtonInfo, l_Control_Info, NULL, "Clears the lua button control info", NULL, NULL)
{
	button_info_clear(&Player->lua_bi);

	return ADE_RETURN_NIL;
}

ADE_FUNC(getButtonInfo, l_Control_Info, NULL, "Access the four bitfields containing the button info", "number, number, number,number", "Four bitfields")
{
	int i;
	int bi_status[4];

	for(i=0;i<4;i++)
		bi_status[i] = Player->lua_bi.status[i];

	return ade_set_args(L, "iiii", bi_status[0], bi_status[1], bi_status[2], bi_status[3]);
}

ADE_FUNC(accessButtonInfo, l_Control_Info, "number, number, number, number", "Access the four bitfields containing the button info", "number, number, number,number", "Four bitfields")
{
	int i;
	int bi_status[4];
	
	for(i=0;i<4;i++)
		bi_status[i] = 0;

	if(!ade_get_args(L, "|iiii", &bi_status[0], &bi_status[1], &bi_status[2], &bi_status[3]))
		return ADE_RETURN_NIL;

	if(ADE_SETTING_VAR) {
		for(i=0;i<4;i++)
			Player->lua_bi.status[i] = bi_status[i];
	}

	for(i=0;i<4;i++)
		bi_status[i] = Player->lua_bi.status[i];

	return ade_set_args(L, "iiii", bi_status[0], bi_status[1], bi_status[2], bi_status[3]);
}

ADE_FUNC(useButtonControl, l_Control_Info, "number, string", "Adds the defined button control to lua button control data, if number is -1 it tries to use the string", NULL, NULL)
{
	int index;
	char *buf = NULL;

	if(!ade_get_args(L, "i|s", &index, &buf))
		return ADE_RETURN_NIL;

	if(index != -1) {
		// Process the number
		if (index > (4 * 32))
			return ADE_RETURN_NIL;

		int a, b;
		a = index / 32;
		b = index % 32;

		// now add the processed bit
		Player->lua_bi.status[a] |= (1<<b);
	} else if (buf != NULL) {
		int i;
		for(i=0; i<num_plr_commands; i++) {
			if(!(strcmp(buf, plr_commands[i].name))) {
				int a;
				a = plr_commands[i].def / 32;
				Player->lua_bi.status[plr_commands[i].var] |= (1<<a);
				break;
			}
		}
	}

	return ADE_RETURN_NIL;
}

ADE_FUNC(getButtonControlName, l_Control_Info, "number", "Gives the name of the command corresponding with the given number", "string", "Name of the command")
{
	int index;

	if(!ade_get_args(L, "i", &index))
		return ade_set_error(L, "s", "");

	if((index < 0) || (index > num_plr_commands))
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", plr_commands[index].name);
}

ADE_FUNC(getButtonControlNumber, l_Control_Info, "string", "Gives the number of the command corresponding with the given string", "number", "Number of the command")
{
	int i;
	char *buf;

	if(!ade_get_args(L, "s", &buf))
		return ade_set_error(L, "i", -1);

	for(i = 0; i < num_plr_commands; i++) {
		if (!(strcmp(buf, plr_commands[i].name))) {
			return ade_set_args(L, "i", plr_commands[i].def);
		}
	}

	return ade_set_error(L, "i", -1);
}

ADE_VIRTVAR(AllButtonPolling, l_Control_Info, "boolean", "Toggles the all button polling for lua", "boolean", "If the all button polling is enabled or not")
{
	bool p;
	int idx;

	if(!ade_get_args(L, "o|b", l_Control_Info.Get(&idx), &p))
		return ADE_RETURN_FALSE;

	if (ADE_SETTING_VAR) {
		if (p)
			lua_game_control |= LGC_B_POLL_ALL;
		else
			lua_game_control &= ~LGC_B_POLL_ALL;
	}

	if (lua_game_control & LGC_B_POLL_ALL)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(pollAllButtons, l_Control_Info, NULL, "Access the four bitfields containing the button info", "number, number, number,number", "Four bitfields")
{
	int i;
	int bi_status[4];

	if(!(lua_game_control & LGC_B_POLL_ALL))
		return ADE_RETURN_NIL;

	for(i=0;i<4;i++)
		bi_status[i] = Player->lua_bi_full.status[i];

	return ade_set_args(L, "iiii", bi_status[0], bi_status[1], bi_status[2], bi_status[3]);
}

class particle_h
{
protected:
	particle::WeakParticlePtr part;
public:
	particle_h()
	{
        part = particle::WeakParticlePtr();
	}

    particle_h(particle::WeakParticlePtr part_p)
	{
		this->part = part_p;
	}

	particle::WeakParticlePtr Get()
	{
		return this->part;
	}

	bool isValid()
	{
		return !part.expired();
	}

	~particle_h()
	{
	}
};

//**********HANDLE: Particle
ade_obj<particle_h> l_Particle("particle", "Handle to a particle");

ADE_VIRTVAR(Position, l_Particle, "vector", "The current position of the particle (world vector)", "vector", "The current position")
{
	particle_h *ph = NULL;
	vec3d newVec = vmd_zero_vector;
	if (!ade_get_args(L, "o|o", l_Particle.GetPtr(&ph), l_Vector.Get(&newVec)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (ph == NULL)
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (!ph->isValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (ADE_SETTING_VAR)
	{
		ph->Get().lock()->pos = newVec;
	}

	return ade_set_args(L, "o", l_Vector.Set(ph->Get().lock()->pos));
}

ADE_VIRTVAR(Velocity, l_Particle, "vector", "The current velocity of the particle (world vector)", "vector", "The current velocity")
{
	particle_h *ph = NULL;
	vec3d newVec = vmd_zero_vector;
	if (!ade_get_args(L, "o|o", l_Particle.GetPtr(&ph), l_Vector.Get(&newVec)))
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));
	
	if (ph == NULL)
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (!ph->isValid())
		return ade_set_error(L, "o", l_Vector.Set(vmd_zero_vector));

	if (ADE_SETTING_VAR)
	{
		ph->Get().lock()->velocity = newVec;
	}

	return ade_set_args(L, "o", l_Vector.Set(ph->Get().lock()->velocity));
}

ADE_VIRTVAR(Age, l_Particle, "number", "The time this particle already lives", "number", "The current age or -1 on error")
{
	particle_h *ph = NULL;
	float newAge = -1.0f;
	if (!ade_get_args(L, "o|f", l_Particle.GetPtr(&ph), &newAge))
		return ade_set_error(L, "f", -1.0f);
	
	if (ph == NULL)
		return ade_set_error(L, "f", -1.0f);

	if (!ph->isValid())
		return ade_set_error(L, "f", -1.0f);

	if (ADE_SETTING_VAR)
	{
		if (newAge >= 0)
			ph->Get().lock()->age = newAge;
	}

	return ade_set_args(L, "f", ph->Get().lock()->age);
}

ADE_VIRTVAR(MaximumLife, l_Particle, "number", "The time this particle can live", "number", "The maximal life or -1 on error")
{
	particle_h *ph = NULL;
	float newLife = -1.0f;
	if (!ade_get_args(L, "o|f", l_Particle.GetPtr(&ph), &newLife))
		return ade_set_error(L, "f", -1.0f);
	
	if (ph == NULL)
		return ade_set_error(L, "f", -1.0f);

	if (!ph->isValid())
		return ade_set_error(L, "f", -1.0f);

	if (ADE_SETTING_VAR)
	{
		if (newLife >= 0)
			ph->Get().lock()->max_life = newLife;
	}

	return ade_set_args(L, "f", ph->Get().lock()->max_life);
}

ADE_VIRTVAR(Radius, l_Particle, "number", "The radius of the particle", "number", "The radius or -1 on error")
{
	particle_h *ph = NULL;
	float newRadius = -1.0f;
	if (!ade_get_args(L, "o|f", l_Particle.GetPtr(&ph), &newRadius))
		return ade_set_error(L, "f", -1.0f);
	
	if (ph == NULL)
		return ade_set_error(L, "f", -1.0f);

	if (!ph->isValid())
		return ade_set_error(L, "f", -1.0f);

	if (ADE_SETTING_VAR)
	{
		if (newRadius >= 0)
			ph->Get().lock()->radius = newRadius;
	}

	return ade_set_args(L, "f", ph->Get().lock()->radius);
}

ADE_VIRTVAR(TracerLength, l_Particle, "number", "The tracer legth of the particle", "number", "The radius or -1 on error")
{
	particle_h *ph = NULL;
	float newTracer = -1.0f;
	if (!ade_get_args(L, "o|f", l_Particle.GetPtr(&ph), &newTracer))
		return ade_set_error(L, "f", -1.0f);
	
	if (ph == NULL)
		return ade_set_error(L, "f", -1.0f);

	if (!ph->isValid())
		return ade_set_error(L, "f", -1.0f);

	// tracer_length has been deprecated
	return ade_set_args(L, "f", -1.0f);
}

ADE_VIRTVAR(AttachedObject, l_Particle, "object", "The object this particle is attached to. If valid the position will be relativ to this object and the velocity will be ignored.", "object", "Attached object or invalid object handle on error")
{
	particle_h *ph = NULL;
	object_h *newObj = nullptr;
	if (!ade_get_args(L, "o|o", l_Particle.GetPtr(&ph), l_Object.GetPtr(&newObj)))
		return ade_set_error(L, "o", l_Object.Set(object_h()));
	
	if (ph == NULL)
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if (!ph->isValid())
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if (ADE_SETTING_VAR)
	{
		if (newObj->IsValid())
			ph->Get().lock()->attached_objnum = newObj->objp->signature;
	}

	return ade_set_args(L, "o", l_Object.Set(object_h(&Objects[ph->Get().lock()->attached_objnum])));
}

ADE_FUNC(isValid, l_Particle, NULL, "Detects whether this handle is valid", "boolean", "true if valid false if not")
{
	particle_h *ph = NULL;
	if (!ade_get_args(L, "o", l_Particle.GetPtr(&ph)))
		return ADE_RETURN_FALSE;
	
	if (ph == NULL)
		return ADE_RETURN_FALSE;

	return ade_set_args(L, "b", ph->isValid());
}

//**********LIBRARY: Audio
ade_lib l_Audio("Audio", NULL, "ad", "Sound/Music Library");

ADE_FUNC(getSoundentry, l_Audio, "string/number", "Return a sound entry matching the specified index or name. If you are using a number then the first valid index is 1", "soundentry", "soundentry or invalid handle on error")
{
	int index = -1;

	if (lua_isnumber(L, 1))
	{
		int idx = -1;
		if(!ade_get_args(L, "i", &idx))
			return ade_set_error(L, "o", l_SoundEntry.Set(sound_entry_h()));
		
		index = gamesnd_get_by_tbl_index(idx);
	}
	else
	{
		char *s = NULL;
		if(!ade_get_args(L, "s", &s))
			return ade_set_error(L, "o", l_SoundEntry.Set(sound_entry_h()));

		if (s == NULL)
			return ade_set_error(L, "o", l_SoundEntry.Set(sound_entry_h()));

		index = gamesnd_get_by_name(s);
	}

	if (index < 0)
	{
		return ade_set_args(L, "o", l_SoundEntry.Set(sound_entry_h()));
	}
	else
	{
		return ade_set_args(L, "o", l_SoundEntry.Set(sound_entry_h(index)));
	}
}

ADE_FUNC(loadSoundfile, l_Audio, "string filename", "Loads the specified sound file", "soundfile", "A soundfile handle")
{
	char* fileName = NULL;

	if (!ade_get_args(L, "s", &fileName))
		return ade_set_error(L, "o", l_Soundfile.Set(-1));

	game_snd tmp_gs;
	strcpy_s( tmp_gs.filename, fileName );
	int n = snd_load( &tmp_gs, 0 );

	return ade_set_error(L, "o", l_Soundfile.Set(n));
}

ADE_FUNC(playSound, l_Audio, "soundentry", "Plays the specified sound entry handle", "sound", "A handle to the playing sound")
{
	sound_entry_h *seh = NULL;

	if (!ade_get_args(L, "o", l_SoundEntry.GetPtr(&seh)))
		return ade_set_error(L, "o", l_Sound.Set(sound_h()));

	if (seh == NULL || !seh->IsValid())
		return ade_set_error(L, "o", l_Sound.Set(sound_h()));

	int handle = snd_play(seh->Get());

	if (handle < 0)
	{
		return ade_set_args(L, "o", l_Sound.Set(sound_h()));
	}
	else
	{
		return ade_set_args(L, "o", l_Sound.Set(sound_h(seh->idx, handle)));
	}
}

ADE_FUNC(playLoopingSound, l_Audio, "soundentry", "Plays the specified sound as a looping sound", "sound", "A handle to the playing sound or invalid handle if playback failed")
{
	sound_entry_h *seh = NULL;

	if (!ade_get_args(L, "o", l_SoundEntry.GetPtr(&seh)))
		return ade_set_error(L, "o", l_Sound.Set(sound_h()));

	if (seh == NULL || !seh->IsValid())
		return ade_set_error(L, "o", l_Sound.Set(sound_h()));

	int handle = snd_play_looping(seh->Get());

	if (handle < 0)
	{
		return ade_set_args(L, "o", l_Sound.Set(sound_h()));
	}
	else
	{
		return ade_set_args(L, "o", l_Sound.Set(sound_h(seh->idx, handle)));
	}
}

ADE_FUNC(play3DSound, l_Audio, "soundentry[, vector source[, vector listener]]", "Plays the specified sound entry handle. Source if by default 0, 0, 0 and listener is by default the current viewposition", "3Dsound", "A handle to the playing sound")
{
	sound_entry_h *seh = NULL;
	vec3d *source = &vmd_zero_vector;
	vec3d *listener = &View_position;

	if (!ade_get_args(L, "o|oo", l_SoundEntry.GetPtr(&seh), l_Vector.GetPtr(&source), l_Vector.GetPtr(&listener)))
		return ade_set_error(L, "o", l_Sound3D.Set(sound_h()));

	if (seh == NULL || !seh->IsValid())
		return ade_set_error(L, "o", l_Sound3D.Set(sound_h()));

	int handle = snd_play_3d(seh->Get(), source, listener);

	if (handle < 0)
	{
		return ade_set_args(L, "o", l_Sound3D.Set(sound_h()));
	}
	else
	{
		return ade_set_args(L, "o", l_Sound3D.Set(sound_h(seh->idx, handle)));
	}
}

ADE_FUNC(playGameSound, l_Audio, "Sound index, [Panning (-1.0 left to 1.0 right), Volume %, Priority 0-3, Voice Message?]", "Plays a sound from #Game Sounds in sounds.tbl. A priority of 0 indicates that the song must play; 1-3 will specify the maximum number of that sound that can be played", "boolean", "True if sound was played, false if not (Replaced with a sound instance object in the future)")
{
	int idx, gamesnd_idx;
	float pan=0.0f;
	float vol=100.0f;
	int pri=0;
	bool voice_msg = false;
	if(!ade_get_args(L, "i|ffib", &idx, &pan, &vol, &pri, &voice_msg))
		return ADE_RETURN_NIL;
	
	if(idx < 0)
		return ADE_RETURN_FALSE;

	if(pri < 0 || pri > 3)
		pri = 0;

    CLAMP(pan, -1.0f, 1.0f);
    CLAMP(vol, 0.0f, 100.0f);

	gamesnd_idx = gamesnd_get_by_tbl_index(idx);

	if (gamesnd_idx >= 0) {
		int sound_handle = snd_play(&Snds[gamesnd_idx], pan, vol*0.01f, pri, voice_msg);
		return ade_set_args(L, "b", sound_handle >= 0);
	} else {
		LuaError(L, "Invalid sound index %i (Snds[%i]) in playGameSound()", idx, gamesnd_idx);
		return ADE_RETURN_FALSE;
	}
}

ADE_FUNC(playInterfaceSound, l_Audio, "Sound index", "Plays a sound from #Interface Sounds in sounds.tbl", "boolean", "True if sound was played, false if not")
{
	int idx, gamesnd_idx;
	if(!ade_get_args(L, "i", &idx))
		return ade_set_error(L, "b", false);

	gamesnd_idx = gamesnd_get_by_iface_tbl_index(idx);

	if (gamesnd_idx >= 0) {
		gamesnd_play_iface(gamesnd_idx);
		return ade_set_args(L, "b", true);
	} else {
		LuaError(L, "Invalid sound index %i (Snds[%i]) in playInterfaceSound()", idx, gamesnd_idx);
		return ADE_RETURN_FALSE;
	}
}

extern float Master_event_music_volume;
ADE_FUNC(playMusic, l_Audio, "string Filename, [float volume = 1.0, bool looping = true]", "Plays a music file using FS2Open's builtin music system. Volume is currently ignored, uses players music volume setting. Files passed to this function are looped by default.", "number", "Audiohandle of the created audiostream, or -1 on failure")
{
	char *s;
	float volume = 1.0f;
	bool loop = true;
	if (!ade_get_args(L, "s|fb", &s, &volume, &loop))
		return ade_set_error(L, "i", -1);

	int ah = audiostream_open(s, ASF_MENUMUSIC);
	if(ah < 0)
		return ade_set_error(L, "i", -1);

	// didn't remove the volume parameter because it'll break the API
	volume = Master_event_music_volume;

	audiostream_play(ah, volume, loop ? 1 : 0);
	return ade_set_args(L, "i", ah);
}

ADE_FUNC(stopMusic, l_Audio, "int audiohandle, [bool fade = false], [string 'briefing|credits|mainhall']", "Stops a playing music file, provided audiohandle is valid. If the 3rd arg is set to one of briefing,credits,mainhall then that music will be stopped despite the audiohandle given.", NULL, NULL)
{
	int ah;
	bool fade = false;
	char *music_type = NULL;

	if(!ade_get_args(L, "i|bs", &ah, &fade, &music_type))
		return ADE_RETURN_NIL;

	if (ah >= MAX_AUDIO_STREAMS || ah < 0 )
		return ADE_RETURN_NIL;

	if (music_type == NULL) {
		audiostream_close_file(ah, fade);
	} else {
		if (!stricmp(music_type, "briefing"))	{
			briefing_stop_music(fade);
		} else if (!stricmp(music_type, "credits")) {
			credits_stop_music(fade);
		} else if (!stricmp(music_type, "mainhall")) {
			main_hall_stop_music(fade);
		} else {
			LuaError(L, "Invalid music type (%s) passed to stopMusic", music_type);
		}
	}

	return ADE_RETURN_NIL;
}

ADE_FUNC(pauseMusic, l_Audio, "int audiohandle, bool pause", "Pauses or unpauses a playing music file, provided audiohandle is valid. The boolean argument should be true to pause and false to unpause. If the audiohandle is -1, *all* audio streams are paused or unpaused.", NULL, NULL)
{
	int ah;
	bool pause;

	if(!ade_get_args(L, "ib", &ah, &pause))
		return ADE_RETURN_NIL;

	if (ah >= 0 && ah < MAX_AUDIO_STREAMS)
	{
		if (pause)
			audiostream_pause(ah, true);
		else
			audiostream_unpause(ah, true);
	}
	else if (ah == -1)
	{
		if (pause)
			audiostream_pause_all(true);
		else
			audiostream_unpause_all(true);
	}

		return ADE_RETURN_NIL;
}


//**********LIBRARY: Base
ade_lib l_Base("Base", NULL, "ba", "Base FreeSpace 2 functions");

ADE_FUNC(print, l_Base, "string Message", "Prints a string", NULL, NULL)
{
	mprintf(("%s", lua_tostring(L, -1)));

	return ADE_RETURN_NIL;
}

ADE_FUNC(warning, l_Base, "string Message", "Displays a FreeSpace warning (debug build-only) message with the string provided", NULL, NULL)
{
	Warning(LOCATION, "%s", lua_tostring(L, -1));

	return ADE_RETURN_NIL;
}

ADE_FUNC(error, l_Base, "string Message", "Displays a FreeSpace error message with the string provided", NULL, NULL)
{
	Error(LOCATION, "%s", lua_tostring(L, -1));

	return ADE_RETURN_NIL;
}

ADE_FUNC(createOrientation, l_Base, "[p/r1c1, b/r1c2, h/r1c3, r2c1, r2c2, r2c3, r3c1, r3c2, r3c3]", "Given 0, 3, or 9 arguments, creates an orientation object with that orientation.", "orientation", "New orientation object, or null orientation on failure")
{
	matrix m;
	int numargs = ade_get_args(L, "|fffffffff", &m.a1d[0], &m.a1d[1], &m.a1d[2], &m.a1d[3], &m.a1d[4], &m.a1d[5], &m.a1d[6], &m.a1d[7], &m.a1d[8]);
	if(!numargs)
	{
		return ade_set_args(L, "o", l_Matrix.Set( matrix_h(&vmd_identity_matrix) ));
	}
	else if(numargs == 3)
	{
		angles a = {m.a1d[0], m.a1d[1], m.a1d[2]};
		return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&a)));
	}
	else if(numargs == 9)
	{
		return ade_set_args(L, "o", l_Matrix.Set(matrix_h(&m)));
	}

	return ade_set_error(L, "o", l_Matrix.Set(matrix_h()));
}

ADE_FUNC(createVector, l_Base, "[x, y, z]", "Creates a vector object", "vector", "Vector object")
{
	vec3d v3 = vmd_zero_vector;
	ade_get_args(L, "|fff", &v3.xyz.x, &v3.xyz.y, &v3.xyz.z);

	return ade_set_args(L, "o", l_Vector.Set(v3));
}

ADE_FUNC(getFrametimeOverall, l_Base, NULL, "The overall frame time in seconds since the engine has started", "number", "Overall time (seconds)")
{
	return ade_set_args(L, "x", game_get_overall_frametime());
}

ADE_FUNC(getFrametime, l_Base, "[Do not adjust for time compression (Boolean)]", "Gets how long this frame is calculated to take. Use it to for animations, physics, etc to make incremental changes.", "number", "Frame time (seconds)")
{
	bool b=false;
	ade_get_args(L, "|b", &b);

	return ade_set_args(L, "f", b ? flRealframetime : flFrametime);
}

ADE_FUNC(getCurrentGameState, l_Base, "[Depth (number)]", "Gets current FreeSpace state; if a depth is specified, the state at that depth is returned. (IE at the in-game options game, a depth of 1 would give you the game state, while the function defaults to 0, which would be the options screen.", "gamestate", "Current game state at specified depth, or invalid handle if no game state is active yet")
{
	int depth = 0;
	ade_get_args(L, "|i", &depth);

	if(depth > gameseq_get_depth())
		return ade_set_args(L, "o", l_GameState.Set(gamestate_h()));

	return ade_set_args(L, "o", l_GameState.Set(gamestate_h(gameseq_get_state(depth))));
}

ADE_FUNC(getCurrentMPStatus, l_Base, "NIL", "Gets this computers current MP status", "string", "Current MP status" )
{
	if ( MULTIPLAYER_MASTER )
		return ade_set_args(L, "s", "MULTIPLAYER_MASTER");

	if ( MULTIPLAYER_HOST )
		return ade_set_args(L, "s", "MULTIPLAYER_HOST");

	if ( MULTIPLAYER_CLIENT )
		return ade_set_args(L, "s", "MULTIPLAYER_CLIENT");

	if ( MULTIPLAYER_STANDALONE )
		return ade_set_args(L, "s", "MULTIPLAYER_STANDALONE");

	return ade_set_args(L, "s", "SINGLEPLAYER");
}

ADE_FUNC(getCurrentPlayer, l_Base, NULL, "Gets a handle of the currently used player.<br><b>Note:</b> If there is no current player then the first player will be returned, check the game state to make sure you have a valid player handle.", "player", "Player handle")
{
	return ade_set_args(L, "o", l_Player.Set(Player_num));
}

ADE_FUNC(setControlMode, l_Base, "NIL or enumeration LE_*_CONTROL", "Sets the current control mode for the game.", "string", "Current control mode")
{
	enum_h *e = NULL;
	if (!(ade_get_args(L, "|o", l_Enum.GetPtr(&e)))) {
		if (lua_game_control & LGC_NORMAL)
			return ade_set_args(L, "s", "NORMAL");
		else if (lua_game_control & LGC_STEERING)
			return ade_set_args(L, "s", "STEERING");
		else if (lua_game_control & LGC_FULL)
			return ade_set_args(L, "s", "FULL");
		else
			return ade_set_error(L, "s", "");
	}

	if (!e) {
		return ade_set_error(L, "s", "");
	}

	switch (e->index) {
		case LE_NORMAL_CONTROLS:
			lua_game_control |= LGC_NORMAL;
			lua_game_control &= ~(LGC_STEERING|LGC_FULL);
			return ade_set_args(L, "s", "NORMAL CONTROLS");
		case LE_LUA_STEERING_CONTROLS:
			lua_game_control |= LGC_STEERING;
			lua_game_control &= ~(LGC_NORMAL|LGC_FULL);
			return ade_set_args(L, "s", "LUA STEERING CONTROLS");
		case LE_LUA_FULL_CONTROLS:
			lua_game_control |= LGC_FULL;
			lua_game_control &= ~(LGC_STEERING|LGC_NORMAL);
			return ade_set_args(L, "s", "LUA FULL CONTROLS");
		default:
			return ade_set_error(L, "s", "");
	}
}

ADE_FUNC(setButtonControlMode, l_Base, "NIL or enumeration LE_*_BUTTON_CONTROL", "Sets the current control mode for the game.", "string", "Current control mode")
{
	enum_h *e = NULL;
	if (!(ade_get_args(L, "|o", l_Enum.GetPtr(&e)))) {
		if (lua_game_control & LGC_B_NORMAL)
			return ade_set_args(L, "s", "NORMAL");
		else if (lua_game_control & LGC_B_OVERRIDE)
			return ade_set_args(L, "s", "OVERRIDE");
		else if (lua_game_control & LGC_B_ADDITIVE)
			return ade_set_args(L, "s", "ADDITIVE");
		else
			return ade_set_error(L, "s", "");
	}

	if (!e) {
		return ade_set_error(L, "s", "");
	}

	switch (e->index) {
		case LE_NORMAL_BUTTON_CONTROLS:
			lua_game_control |= LGC_B_NORMAL;
			lua_game_control &= ~(LGC_B_ADDITIVE|LGC_B_OVERRIDE);
			return ade_set_args(L, "s", "NORMAL BUTTON CONTROL");
		case LE_LUA_ADDITIVE_BUTTON_CONTROL:
			lua_game_control |= LGC_B_ADDITIVE;
			lua_game_control &= ~(LGC_B_NORMAL|LGC_B_OVERRIDE);
			return ade_set_args(L, "s", "LUA OVERRIDE BUTTON CONTROL");
		case LE_LUA_OVERRIDE_BUTTON_CONTROL:
			lua_game_control |= LGC_B_OVERRIDE;
			lua_game_control &= ~(LGC_B_ADDITIVE|LGC_B_NORMAL);
			return ade_set_args(L, "s", "LUA ADDITIVE BUTTON CONTROL");
		default:
			return ade_set_error(L, "s", "");
	}
}

ADE_FUNC(getControlInfo, l_Base, NULL, "Gets the control info handle.", "control info", "control info handle")
{
	return ade_set_args(L, "o", l_Control_Info.Set(1));
}

ADE_FUNC(setTips, l_Base, "True or false", "Sets whether to display tips of the day the next time the current pilot enters the mainhall.", NULL, NULL)
{
	if (Player == NULL)
		return ADE_RETURN_NIL;

	bool tips = false;

	ade_get_args(L, "b", &tips);

	if (tips)
		Player->tips = 1;
	else
		Player->tips = 0;

	return ADE_RETURN_NIL;
}

ADE_FUNC(getGameDifficulty, l_Base, NULL, "Returns the difficulty level from 1-5, 1 being the lowest, (Very Easy) and 5 being the highest (Insane)", "integer", "Difficulty level as integer")
{
	return ade_set_args(L, "i", Game_skill_level+1);
}

ADE_FUNC(postGameEvent, l_Base, "gameevent Event", "Sets current game event. Note that you can crash FreeSpace 2 by posting an event at an improper time, so test extensively if you use it.", "boolean", "True if event was posted, false if passed event was invalid")
{
	gameevent_h *gh = NULL;
	if(!ade_get_args(L, "o", l_GameEvent.GetPtr(&gh)))
		return ade_set_error(L, "b", false);

	if(!gh->IsValid())
		return ade_set_error(L, "b", false);

	if (Om_tracker_flag) 
		Multi_options_g.protocol = NET_TCP;
	psnet_use_protocol(Multi_options_g.protocol);
	
	gameseq_post_event(gh->Get());

	return ADE_RETURN_TRUE;
}

//**********SUBLIBRARY: Base/Events
ade_lib l_Base_Events("GameEvents", &l_Base, NULL, "Freespace 2 game events");

ADE_INDEXER(l_Base_Events, "number Index/string Name", "Array of game events", "gameevent", "Game event, or invalid gameevent handle if index is invalid")
{
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_GameEvent.Set(gameevent_h()));

	int idx = gameseq_get_event_idx(name);

	if(idx < 0)
	{
		idx = atoi(name);

		//Lua-->FS2
		idx--;

		if(idx < 0 || idx >= Num_gs_event_text)
			return ade_set_error(L, "o", l_GameEvent.Set(gameevent_h()));
	}

	return ade_set_args(L, "o", l_GameEvent.Set(gameevent_h(idx)));
}

ADE_FUNC(__len, l_Base_Events, NULL, "Number of events", "number", "Number of events")
{
	return ade_set_args(L, "i", Num_gs_event_text);
}

//**********SUBLIBRARY: Base/States
ade_lib l_Base_States("GameStates", &l_Base, NULL, "Freespace 2 states");

ADE_INDEXER(l_Base_States, "number Index/string Name", "Array of game states", "gamestate", "Game state, or invalid gamestate handle if index is invalid")
{
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_GameState.Set(gamestate_h()));

	int idx = gameseq_get_state_idx(name);

	if(idx < 0)
	{
		idx = atoi(name);

		//Lua-->FS2
		idx--;

		if(idx < 0 || idx >= Num_gs_state_text)
			return ade_set_error(L, "o", l_GameState.Set(gamestate_h()));
	}

	return ade_set_args(L, "o", l_GameState.Set(gamestate_h(idx)));
}

ADE_FUNC(__len, l_Base_States, NULL, "Number of states", "number", "Number of states")
{
	return ade_set_args(L, "i", Num_gs_state_text);
}

//**********LIBRARY: CFILE
//WMC - It's on my to-do list! (Well, if I had one anyway)
//WMC - Did it. I had to invent a to-do list first, though.
//Ironically, I never actually put this on it.
ade_lib l_CFile("CFile", NULL, "cf", "CFile FS2 filesystem access");

int l_cf_get_path_id(char* n_path)
{
	size_t i;
	size_t path_len = strlen(n_path);

	char *buf = (char*) vm_malloc((path_len+1) * sizeof(char));
	
	if (!buf) 
		return CF_TYPE_INVALID;
		
	strcpy(buf, n_path);

	//Remove trailing slashes; avoid buffer overflow on 1-char strings
	i = path_len - 1;
	while(i < std::numeric_limits<size_t>::max() && (buf[i] == '\\' || buf[i] == '/'))
		buf[i--] = '\0';

	//Remove leading slashes
	i = 0;
	while(i < path_len && (buf[i] == '\\' || buf[i] == '/'))
		buf[i++] = '\0';

	//Use official DIR_SEPARATOR_CHAR
	for(i = 0; i < path_len; i++)
	{
		if(buf[i] == '\\' || buf[i] == '/')
			buf[i] = DIR_SEPARATOR_CHAR;
	}
	for(i = 0; i < CF_MAX_PATH_TYPES; i++)
	{
		if(Pathtypes[i].path != NULL && !stricmp(buf, Pathtypes[i].path)) {
			vm_free(buf);
			buf = NULL;
			return Pathtypes[i].index;
		}
	}

	vm_free(buf);
	buf = NULL;
	return CF_TYPE_INVALID;
}

ADE_FUNC(deleteFile, l_CFile, "string Filename, string Path", "Deletes given file. Path must be specified. Use a slash for the root directory.", "boolean", "True if deleted, false")
{
	char *n_filename = NULL;
	char *n_path = "";
	if(!ade_get_args(L, "ss", &n_filename, &n_path))
		return ade_set_error(L, "b", false);

	int path = CF_TYPE_INVALID;
	if(n_path != NULL && strlen(n_path))
		path = l_cf_get_path_id(n_path);

	if(path == CF_TYPE_INVALID)
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", cf_delete(n_filename, path) != 0);
}

ADE_FUNC(fileExists, l_CFile, "string Filename, [string Path = \"\", boolean CheckVPs = false]", "Checks if a file exists. Use a blank string for path for any directory, or a slash for the root directory.", "boolean", "True if file exists, false or nil otherwise")
{
	char *n_filename = NULL;
	char *n_path = "";
	bool check_vps = false;
	if(!ade_get_args(L, "s|sb", &n_filename, &n_path, &check_vps))
		return ADE_RETURN_NIL;

	int path = CF_TYPE_ANY;
	if(n_path != NULL && strlen(n_path))
		path = l_cf_get_path_id(n_path);

	if(path == CF_TYPE_INVALID)
		return ade_set_error(L, "b", false);

	if(!check_vps)
		return ade_set_args(L, "b", cf_exists(n_filename, path) != 0);
	else
		return ade_set_args(L, "b", cf_exists_full(n_filename, path ) != 0);
}

ADE_FUNC(openFile, l_CFile, "string Filename, [string Mode=\"r\", string Path = \"\"]",
		 "Opens a file. 'Mode' uses standard C fopen arguments. Use a blank string for path for any directory, or a slash for the root directory."
		 "Be EXTREMELY CAREFUL when using this function, as you may PERMANENTLY delete any file by accident",
		 "file",
		 "File handle, or invalid file handle if the specified file couldn't be opened")
{
	char *n_filename = NULL;
	char *n_mode = "r";
	char *n_path = "";
	if(!ade_get_args(L, "s|ss", &n_filename, &n_mode, &n_path))
		return ade_set_error(L, "o", l_File.Set(NULL));

	int type = CFILE_NORMAL;

	int path = CF_TYPE_ANY;
	if(n_path != NULL && strlen(n_path))
		path = l_cf_get_path_id(n_path);

	if(path == CF_TYPE_INVALID)
		return ade_set_error(L, "o", l_File.Set(NULL));

	CFILE *cfp = cfopen(n_filename, n_mode, type, path);
	
	if(!cf_is_valid(cfp))
		return ade_set_error(L, "o", l_File.Set(NULL));

	return ade_set_args(L, "o", l_File.Set(cfp));
}

ADE_FUNC(openTempFile, l_CFile, NULL, "Opens a temp file that is automatically deleted when closed", "file", "File handle, or invalid file handle if tempfile couldn't be created")
{
	return ade_set_args(L, "o", l_File.Set(ctmpfile()));
}

ADE_FUNC(renameFile, l_CFile, "string CurrentFilename, string NewFilename, string Path", "Renames given file. Path must be specified. Use a slash for the root directory.", "boolean", "True if file was renamed, otherwise false")
{
	char *n_filename = NULL;
	char *n_new_filename = NULL;
	char *n_path = "";
	if(!ade_get_args(L, "ss|s", &n_filename, &n_new_filename, &n_path))
		return ade_set_error(L, "b", false);

	int path = CF_TYPE_INVALID;
	if(n_path != NULL && strlen(n_path))
		path = l_cf_get_path_id(n_path);

	if(path == CF_TYPE_INVALID)
		return ade_set_error(L, "b", false);

	return ade_set_args(L, "b", cf_rename(n_filename, n_new_filename, path) != 0);
}

//**********LIBRARY: Controls library
ade_lib l_Mouse("Controls", NULL, "io", "Controls library");

extern int mouse_inited;

ADE_FUNC(getMouseX, l_Mouse, NULL, "Gets Mouse X pos", "number", "Mouse x position, or 0 if mouse is not initialized yet")
{
	if(!mouse_inited)
		return ade_set_error(L, "i", 0);

	int x;

	mouse_get_pos(&x, NULL);

	return ade_set_args(L, "i", x);
}

ADE_FUNC(getMouseY, l_Mouse, NULL, "Gets Mouse Y pos", "number", "Mouse y position, or 0 if mouse is not initialized yet")
{
	if(!mouse_inited)
		return ade_set_error(L, "i", 0);

	int y;

	mouse_get_pos(NULL, &y);

	return ade_set_args(L, "i", y);
}

ADE_FUNC(isMouseButtonDown, l_Mouse, "{MOUSE_*_BUTTON enumeration}, [..., ...]", "Returns whether the specified mouse buttons are up or down", "boolean", "Whether specified mouse buttons are down, or false if mouse is not initialized yet")
{
	if(!mouse_inited)
		return ade_set_error(L, "b", false);

	enum_h *e[3] = {NULL, NULL, NULL};
	ade_get_args(L, "o|oo", l_Enum.GetPtr(&e[0]), l_Enum.GetPtr(&e[1]), l_Enum.GetPtr(&e[2]));	//Like a snake!

	bool rtn = false;
	int check_flags = 0;

	for(int i = 0; i < 3; i++)
	{
		if(e[i] == NULL)
			break;

		if(e[i]->index == LE_MOUSE_LEFT_BUTTON)
			check_flags |= MOUSE_LEFT_BUTTON;
		if(e[i]->index == LE_MOUSE_MIDDLE_BUTTON)
			check_flags |= MOUSE_MIDDLE_BUTTON;
		if(e[i]->index == LE_MOUSE_RIGHT_BUTTON)
			check_flags |= MOUSE_RIGHT_BUTTON;
	}

	if(mouse_down(check_flags))
		rtn = true;

	return ade_set_args(L, "b", rtn);
}

ADE_FUNC(setCursorImage, l_Mouse, "Image filename", "Sets mouse cursor image, and allows you to lock/unlock the image. (A locked cursor may only be changed with the unlock parameter)", "boolean", "true if successful, false otherwise")
{
	using namespace io::mouse;

	if(!mouse_inited || !Gr_inited)
		return ade_set_error(L, "b", false);

	if (Is_standalone)
		return ade_set_error(L, "b", false);

	char *s = NULL;
	enum_h *u = NULL; // This isn't used anymore

	if(!ade_get_args(L, "s|o", &s, l_Enum.GetPtr(&u)))
		return ade_set_error(L, "b", false);

	Cursor* cursor = CursorManager::get()->loadCursor(s);

	if (cursor == NULL)
	{
		return ade_set_error(L, "b", false);
	}

	CursorManager::get()->setCurrentCursor(cursor);
	return ade_set_args(L, "b", true);
}

ADE_FUNC(setCursorHidden, l_Mouse, "boolean hide[, boolean grab]", "Hides the cursor when <i>hide</i> is true, otherwise shows it. <i>grab</i> determines if "
				"the mouse will be restricted to the window. Set this to true when hiding the cursor while in game. By default grab will be true when we are in the game play state, false otherwise.", NULL, NULL)
{
	if(!mouse_inited)
		return ADE_RETURN_NIL;

	bool b = false;
	bool grab = gameseq_get_state() == GS_STATE_GAME_PLAY;
	ade_get_args(L, "b|b", &b, &grab);

	io::mouse::CursorManager::get()->showCursor(!b, grab);

	return ADE_RETURN_NIL;
}

ADE_FUNC(forceMousePosition, l_Mouse, "number, number (coordinates)", "function to force mouse position", "boolean", "if the operation succeeded or not")
{
	if(!mouse_inited)
		return ADE_RETURN_FALSE;

	if(!Gr_inited)
		return ADE_RETURN_FALSE;

	int x, y;
	if (!(ade_get_args(L, "ii", &x, &y)))
		return ADE_RETURN_FALSE;

	if (!((x >= 0) && (x <= gr_screen.max_w)))
		return ADE_RETURN_FALSE;

	if (!((y >= 0) && (y <= gr_screen.max_h)))
		return ADE_RETURN_FALSE;

	mouse_set_pos(x, y);

	return ADE_RETURN_TRUE;
}

ADE_VIRTVAR(MouseControlStatus, l_Mouse, "boolean", "Gets and sets the retail mouse control status", "boolean", "if the retail mouse is on or off")
{
	if(!mouse_inited)
		return ADE_RETURN_NIL;

	bool newVal = false;
	if (!ade_get_args(L, "*|b", &newVal))
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR)
	{
		if (newVal)
		{
			Use_mouse_to_fly = 1;
		}
		else
		{
			Use_mouse_to_fly = 0;
		}
	}

	if (Use_mouse_to_fly)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(getMouseSensitivity, l_Mouse, NULL, "Gets mouse sensitivity setting", "number", "Mouse sensitivity in range of 0-9")
{
	return ade_set_args(L, "i", Mouse_sensitivity);
}

ADE_FUNC(getJoySensitivity, l_Mouse, NULL, "Gets joystick sensitivity setting", "number", "Joystick sensitivity in range of 0-9")
{
	return ade_set_args(L, "i", Joy_sensitivity);
}

ADE_FUNC(getJoyDeadzone, l_Mouse, NULL, "Gets joystick deadzone setting", "number", "Joystick deadzone in range of 0-9")
{
	return ade_set_args(L, "i", Joy_dead_zone_size / 5);
}

//trackir funcs
ADE_FUNC(updateTrackIR, l_Mouse, NULL, "Updates Tracking Data. Call before using get functions", "boolean", "Checks if trackir is available and updates variables, returns true if successful, otherwise false")
{
	if( !headtracking::isEnabled() )
		return ADE_RETURN_FALSE;

	if (!headtracking::query())
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getTrackIRPitch, l_Mouse, NULL, "Gets pitch axis from last update", "number", "Pitch value -1 to 1, or 0 on failure")
{
	if (!headtracking::isEnabled())
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args( L, "f", headtracking::getStatus()->pitch);
}

ADE_FUNC(getTrackIRYaw, l_Mouse, NULL, "Gets yaw axis from last update", "number", "Yaw value -1 to 1, or 0 on failure")
{
	if (!headtracking::isEnabled())
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", headtracking::getStatus()->yaw);
}

ADE_FUNC(getTrackIRRoll, l_Mouse, NULL, "Gets roll axis from last update", "number", "Roll value -1 to 1, or 0 on failure")
{
	if (!headtracking::isEnabled())
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", headtracking::getStatus()->roll);
}

ADE_FUNC(getTrackIRX, l_Mouse, NULL, "Gets x position from last update", "number", "X value -1 to 1, or 0 on failure")
{
	if (!headtracking::isEnabled())
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", headtracking::getStatus()->x);
}

ADE_FUNC(getTrackIRY, l_Mouse, NULL, "Gets y position from last update", "number", "Y value -1 to 1, or 0 on failure")
{
	if (!headtracking::isEnabled())
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", headtracking::getStatus()->y);
}

ADE_FUNC(getTrackIRZ, l_Mouse, NULL, "Gets z position from last update", "number", "Z value -1 to 1, or 0 on failure")
{
	if (!headtracking::isEnabled() )
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "f", headtracking::getStatus()->z);
}

//**********LIBRARY: HUD library
ade_lib l_HUD("HUD", NULL, "hu", "HUD library");

ADE_VIRTVAR(HUDDrawn, l_HUD, "boolean", "Current HUD draw status", "boolean", "If the HUD is drawn or not")
{
	bool to_draw = false;

	if(!ade_get_args(L, "*|b", &to_draw))
		return ADE_RETURN_NIL;

	if(ADE_SETTING_VAR)
	{
		if (to_draw)
			HUD_draw = 1;
		else
			HUD_draw = 0;
	}

	if (HUD_draw)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_VIRTVAR(HUDDisabledExceptMessages, l_HUD, "boolean", "Specifies if only the messages gauges of the hud are drawn", "boolean", "true if only the message gauges are drawn, false otherwise")
{
	bool to_draw = false;

	if (!ade_get_args(L, "*|b", &to_draw))
		return ADE_RETURN_NIL;

	if (ADE_SETTING_VAR)
	{
		hud_disable_except_messages(to_draw);
	}

	if (hud_disabled_except_messages())
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(setHUDGaugeColor, l_HUD, "number (index number of the gauge), [integer red, number green, number blue, number alpha]", "Color used to draw the gauge", "boolean", "If the operation was successful")
{
	int idx = -1; 
	int r = 0;
	int g = 0;
	int b = 0;
	int a = 0;

	if(!ade_get_args(L, "i|iiii", &idx, &r, &g, &b, &a))
		return ADE_RETURN_FALSE;

	if ((idx < 0) || (idx >= NUM_HUD_GAUGES))
		return ADE_RETURN_FALSE;

	gr_init_alphacolor(&HUD_config.clr[idx], r, g, b, a);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getHUDGaugeColor, l_HUD, "number (index number of the gauge)", "Color used to draw the gauge", "number, number, number, number", "Red, green, blue, and alpha of the gauge")
{
	int idx = -1;

	if(!ade_get_args(L, "i", &idx))
		return ADE_RETURN_NIL;

	if ((idx < 0) || (idx >= NUM_HUD_GAUGES))
		return ADE_RETURN_NIL;

	color c = HUD_config.clr[idx];
	
	return ade_set_args(L, "iiii", (int) c.red, (int) c.green, (int) c.blue, (int) c.alpha);	
}

ADE_FUNC(getHUDGaugeHandle, l_HUD, "string Name", "Returns a handle to a specified HUD gauge", "HudGauge", "HUD Gauge handle, or nil if invalid")
{
	char* name;
	if (!ade_get_args(L, "s", &name))
		return ADE_RETURN_NIL;
	HudGauge* gauge = NULL;

	gauge = hud_get_gauge(name);

	if (gauge == NULL)
		return ADE_RETURN_NIL;
	else
		return ade_set_args(L, "o", l_HudGauge.Set(*gauge));
}

//**********LIBRARY: Graphics
ade_lib l_Graphics("Graphics", NULL, "gr", "Graphics Library");

//****SUBLIBRARY: Graphics/Cameras
ade_lib l_Graphics_Cameras("Cameras", &l_Graphics, NULL, "Cameras");

ADE_INDEXER(l_Graphics_Cameras, "number Index/string Name", "Gets camera", "camera", "Ship handle, or invalid ship handle if index was invalid")
{
	char *s = NULL;
	if(!ade_get_args(L, "*s", &s))
		return ade_set_error(L, "o", l_Camera.Set(camid()));

	camid cid = cam_lookup(s);
	if(!cid.isValid())
	{
		int cn = atoi(s);
		if(cn > 0)
		{
			//Lua-->FS2
			cn--;
			cid = cam_get_camera(cn);
		}
	}

	return ade_set_args(L, "o", l_Camera.Set(cid));
}

ADE_FUNC(__len, l_Graphics_Cameras, NULL, "Gets number of cameras", "number", "Number of cameras")
{
	return ade_set_args(L, "i", (int)cam_get_num());
}

//****SUBLIBRARY: Graphics/Fonts
ade_lib l_Graphics_Fonts("Fonts", &l_Graphics, NULL, "Font library");

ADE_FUNC(__len, l_Graphics_Fonts, NULL, "Number of loaded fonts", "number", "Number of loaded fonts")
{
	return ade_set_args(L, "i", font::FontManager::numberOfFonts());
}

ADE_INDEXER(l_Graphics_Fonts, "number Index/string Filename", "Array of loaded fonts", "font", "Font handle, or invalid font handle if index is invalid")
{
	if (lua_isnumber(L, 2))
	{
		int index = -1;

		if (!ade_get_args(L, "*i", &index))
			return ade_set_error(L, "o", l_Font.Set(font_h()));

		auto realIdx = index - 1;

		if (realIdx < 0 || realIdx >= font::FontManager::numberOfFonts())
		{
			LuaError(L, "Invalid font index %d specified, must be between 1 and %d!", index, font::FontManager::numberOfFonts());
		}

		return ade_set_args(L, "o", l_Font.Set(font_h(font::FontManager::getFont(index - 1))));
	}
	else
	{
		char *s = NULL;

		if(!ade_get_args(L, "*s", &s))
			return ade_set_error(L, "o", l_Font.Set(font_h()));

		return ade_set_args(L, "o", l_Font.Set(font_h(font::FontManager::getFont(s))));
	}
}

ADE_VIRTVAR(CurrentFont, l_Graphics, "font", "Current font", "font", NULL)
{
	font_h *newFh = NULL;

	if(!ade_get_args(L, "*|o", l_Font.GetPtr(&newFh)))
		return ade_set_error(L, "o", l_Font.Set(font_h()));

	if(ADE_SETTING_VAR && newFh->isValid()) {
		font::FontManager::setCurrentFont(newFh->Get());
	}
	
	return ade_set_args(L, "o", l_Font.Set(font_h(font::FontManager::getCurrentFont())));
}

//****SUBLIBRARY: Graphics/PostEffects
ade_lib l_Graphics_Posteffects("PostEffects", &l_Graphics, NULL, "Post processing effects");

ADE_INDEXER(l_Graphics_Posteffects, "number index", "Gets the name of the specified post processing index", "string", "post processing name or empty string on error")
{
	int index = -1;
	if(!ade_get_args(L, "*i", &index))
		return ade_set_error(L, "s", "");

	index--; // Lua -> C/C++

	if (index < 0)
		return ade_set_error(L, "s", "");

	SCP_vector<SCP_string> names;
	get_post_process_effect_names(names);
	names.push_back(SCP_string("lightshafts"));

	if (index >= (int) names.size())
		return ade_set_error(L, "s", "");

	return ade_set_args(L, "s", const_cast<char*>(names[index].c_str()));
}

ADE_FUNC(__len, l_Graphics_Posteffects, NULL, "Gets the number or available post processing effects", "number", "number of post processing effects or 0 on error")
{
	SCP_vector<SCP_string> names;
	get_post_process_effect_names(names);

	// Add one for lightshafts
	return ade_set_args(L, "i", ((int) names.size()) + 1);
}

ADE_FUNC(setPostEffect, l_Graphics, "string name, [number value=0]", "Sets the intensity of the specified post processing effect", "boolean", "true when successful, false otherwise")
{
	char* name = NULL;
	int intensity = 0;

	if (!ade_get_args(L, "s|i", &name, &intensity))
		return ADE_RETURN_FALSE;

	if (name == NULL || intensity < 0)
		return ADE_RETURN_FALSE;

	gr_post_process_set_effect(name, intensity);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(resetPostEffects, l_Graphics, NULL, "Resets all post effects to their default values", "boolean", "true if successful, false otherwise")
{
	gr_post_process_set_defaults();

	return ADE_RETURN_TRUE;
}

ADE_VIRTVAR(CurrentOpacityType, l_Graphics, "enumeration", "Current alpha blending type; uses ALPHABLEND_* enumerations", "enumeration", NULL)
{
	enum_h *alphatype = NULL;

	if(!ade_get_args(L, "*|o", l_Enum.GetPtr(&alphatype)))
		return ADE_RETURN_NIL;

	if(ADE_SETTING_VAR)
	{
		if(alphatype != NULL && alphatype->index == LE_ALPHABLEND_FILTER)
			lua_Opacity_type = GR_ALPHABLEND_FILTER;
		else
			lua_Opacity_type = GR_ALPHABLEND_NONE;
	}

	int rtn;
	switch(lua_Opacity_type)
	{
		case GR_ALPHABLEND_FILTER:
			rtn = LE_ALPHABLEND_FILTER;
			break;
		default:
			rtn = LE_ALPHABLEND_NONE;
	}

	return ade_set_args(L, "o", l_Enum.Set(enum_h(rtn)));
}

ADE_VIRTVAR(CurrentRenderTarget, l_Graphics, "texture", "Current rendering target", "texture", "Current rendering target, or invalid texture handle if screen is render target")
{
	int newtx = -1;

	if(ADE_SETTING_VAR && lua_isnil(L, 2))
	{
		bm_set_render_target(-1);
		return ade_set_args(L, "o", l_Texture.Set(gr_screen.rendering_to_texture));
	}
	else
	{

		if(!ade_get_args(L, "*|o", l_Texture.Get(&newtx)))
			return ade_set_error(L, "o", l_Texture.Set(-1));

		if(ADE_SETTING_VAR) {
			if(newtx > -1 && bm_is_valid(newtx))
				bm_set_render_target(newtx, 0);
			else
				bm_set_render_target(-1);
		}

		return ade_set_args(L, "o", l_Texture.Set(gr_screen.rendering_to_texture));
	}
}

ADE_FUNC(clearScreen, l_Graphics, "[integer red, number green, number blue, number alpha]", "Clears the screen to black, or the color specified.", NULL, NULL)
{
	int r,g,b,a;
	r=g=b=0;
	a=255;
	ade_get_args(L, "|iiii", &r, &g, &b, &a);

	//WMC - Set to valid values
	if(r != 0 || g != 0 || b != 0 || a!= 255)
	{
		CAP(r,0,255);
		CAP(g,0,255);
		CAP(b,0,255);
		CAP(a,0,255);
		gr_set_clear_color(r,g,b);
		gr_screen.current_clear_color.alpha = (ubyte)a;
		gr_clear();
		gr_set_clear_color(0,0,0);

		return ADE_RETURN_NIL;
	}

	gr_clear();
	return ADE_RETURN_NIL;
}

ADE_FUNC(createCamera, l_Graphics,
		 "string Name, [wvector Position, orientation Orientation]",
		 "Creates a new camera using the specified position and orientation (World)",
		 "camera",
		 "Camera handle, or invalid camera handle if camera couldn't be created")
{
	char *s = NULL;
	vec3d *v = &vmd_zero_vector;
	matrix_h *mh = NULL;
	if(!ade_get_args(L, "s|oo", &s, l_Vector.GetPtr(&v), l_Matrix.GetPtr(&mh)))
		return ADE_RETURN_NIL;

	matrix *mtx = &vmd_identity_matrix;
	if(mh != NULL)
		mtx = mh->GetMatrix();
	camid cid = cam_create(s, v, mtx);

	//Set position
	return ade_set_args(L, "o", l_Camera.Set(cid));
}

ADE_FUNC(isMenuStretched, l_Graphics, NULL, "Returns whether the standard interface is stretched", "boolean", "True if stretched, false if aspect ratio is maintained")
{
	if(!Gr_inited)
		return ade_set_error(L, "b", 0);

	return ade_set_args(L, "b", Cmdline_stretch_menu);
}

ADE_FUNC(getScreenWidth, l_Graphics, NULL, "Gets screen width", "number", "Width in pixels, or 0 if graphics are not initialized yet")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", gr_screen.max_w);
}

ADE_FUNC(getScreenHeight, l_Graphics, NULL, "Gets screen height", "number", "Height in pixels, or 0 if graphics are not initialized yet")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", gr_screen.max_h);
}

ADE_FUNC(getCenterWidth, l_Graphics, NULL, "Gets width of center monitor (should be used in conjuction with getCenterOffsetX)", "number", "Width of center monitor in pixels, or 0 if graphics are not initialized yet")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", gr_screen.center_w);
}

ADE_FUNC(getCenterHeight, l_Graphics, NULL, "Gets height of center monitor (should be used in conjuction with getCenterOffsetY)", "number", "Height of center monitor in pixels, or 0 if graphics are not initialized yet")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", gr_screen.center_h);
}

ADE_FUNC(getCenterOffsetX, l_Graphics, NULL, "Gets X offset of center monitor", "number", "X offset of center monitor in pixels")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", gr_screen.center_offset_x);
}

ADE_FUNC(getCenterOffsetY, l_Graphics, NULL, "Gets Y offset of center monitor", "number", "Y offset of center monitor in pixels")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	return ade_set_args(L, "i", gr_screen.center_offset_y);
}

ADE_FUNC(getCurrentCamera, l_Graphics, "[boolean]", "Gets the current camera handle, if argument is <i>true</i> then it will also return the main camera when no custom camera is in use", "camera", "camera handle or invalid handle on error")
{
	camid current;

	bool rtnMain = false;

	ade_get_args(L, "|b", &rtnMain);

	if (!rtnMain || Viewer_mode & VM_FREECAMERA)
		current = cam_get_current();
	else
		current = Main_camera;

	return ade_set_args(L, "o", l_Camera.Set(current));
}

ADE_FUNC(getVectorFromCoords, l_Graphics,
		 "[number X=center, number Y=center, number Depth, boolean normalize = false]",
		 "Returns a vector through screen coordinates x and y. "
		 "If depth is specified, vector is extended to Depth units into space"
		 "If normalize is true, vector will be normalized.",
		 "vector",
		 "Vector, or zero vector on failure")
{
	int x = gr_screen.max_w/2;
	int y = gr_screen.max_h/2;
	float depth = 0.0f;
	bool normalize = false; 
	ade_get_args(L, "|iifb", &x, &y, &depth, &normalize);

	vec3d pos = vmd_zero_vector;

	bool in_frame = g3_in_frame() > 0;
	if(!in_frame) {
		g3_start_frame(0);

		vec3d cam_pos;
		matrix cam_orient;

		camid cid;
		if (Viewer_mode & VM_FREECAMERA)
			cid = cam_get_current();
		else
			cid = Main_camera;

		camera *cam = cid.getCamera();

		if (cam != NULL) {
			cam->get_info(&cam_pos, &cam_orient);
			g3_set_view_matrix(&cam_pos, &cam_orient, View_zoom);
		} else {
			g3_set_view_matrix(&Eye_position, &Eye_matrix, View_zoom);
		}

		gr_set_proj_matrix( Proj_fov, gr_screen.clip_aspect, Min_draw_distance, Max_draw_distance);
		gr_set_view_matrix(&Eye_position, &Eye_matrix);
	}

	g3_point_to_vec(&pos, x, y);

	if(!in_frame) {
		gr_end_view_matrix();
		gr_end_proj_matrix();
		g3_end_frame();
	}

	if(depth)
		vm_vec_scale(&pos, depth);

	if (normalize)
		vm_vec_normalize_quick(&pos);

	vm_vec_add2(&pos, &View_position);

	return ade_set_args(L, "o", l_Vector.Set(pos));
}

ADE_FUNC(setTarget, l_Graphics, "[texture Texture]",
		"If texture is specified, sets current rendering surface to a texture."
		"Otherwise, sets rendering surface back to screen.",
		"boolean",
		"True if successful, false otherwise")
{
	if(!Gr_inited)
		return ade_set_error(L, "b", false);

	int idx = -1;
	ade_get_args(L, "|o", l_Texture.Get(&idx));

	return ade_set_args(L, "b", bm_set_render_target(idx, 0));
}

ADE_FUNC(setCamera, l_Graphics, "[camera handle Camera]", "Sets current camera, or resets camera if none specified", "boolean", "true if successful, false or nil otherwise")
{
	camid cid;
	if(!ade_get_args(L, "|o", l_Camera.Get(&cid)))
	{
		cam_reset_camera();
		return ADE_RETURN_NIL;
	}

	if(!cid.isValid())
		return ADE_RETURN_NIL;

	cam_set_camera(cid);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(setColor, l_Graphics, "integer Red, number Green, number Blue, [integer Alpha]", "Sets 2D drawing color; each color number should be from 0 (darkest) to 255 (brightest)", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int r,g,b,a=255;

	if(!ade_get_args(L, "iii|i", &r, &g, &b, &a))
		return ADE_RETURN_NIL;

	color ac;
	gr_init_alphacolor(&ac,r,g,b,a);
	gr_set_color_fast(&ac);

	return ADE_RETURN_NIL;
}

ADE_FUNC(setLineWidth, l_Graphics, "[number width=1.0]", "Sets the line width for lines. This call might fail if the specified width is not supported by the graphics implementation. Then the width will be the nearest supported value.", "boolean", "true if succeeded, false otherwise")
{
	if(!Gr_inited)
		return ADE_RETURN_FALSE;

	float width = 1.0f;

	ade_get_args(L, "|f", &width);

	if (width <= 0.0f)
	{
		return ADE_RETURN_FALSE;
	}

	gr_set_line_width(width);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(drawCircle, l_Graphics, "number Radius, number X, number Y, [boolean Filled=true]", "Draws a circle", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x,y,ra;
	bool fill = true;

	if(!ade_get_args(L, "iii|b", &ra,&x,&y,&fill))
		return ADE_RETURN_NIL;

	if (fill) {
		//WMC - Circle takes...diameter.
		gr_circle(x,y, ra*2, GR_RESIZE_NONE);
	} else {
		gr_unfilled_circle(x,y, ra*2, GR_RESIZE_NONE);
	}

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawArc, l_Graphics, "number Radius, number X, number Y, number StartAngle, number EndAngle, [boolean Filled=true]", "Draws an arc", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x,y;
	float ra,angle_start,angle_end;
	bool fill = true;

	if(!ade_get_args(L, "fiiff|b", &ra,&x,&y,&angle_start,&angle_end,&fill)) {
		return ADE_RETURN_NIL;
	}

	gr_arc(x,y, ra, angle_start, angle_end, fill, GR_RESIZE_NONE);

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawCurve, l_Graphics, "number X, number Y, number Radius", "Draws a curve", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x,y,ra,dir = 0;

	if(!ade_get_args(L, "iii|i", &x,&y,&ra, &dir))
		return ADE_RETURN_NIL;

	//WMC - direction should be settable at a certain point via enumerations.
	//Not gonna deal with it now.
    gr_curve(x, y, ra, dir, GR_RESIZE_NONE);

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawGradientLine, l_Graphics, "number X1, number Y1, number X2, number Y2", "Draws a line from (x1,y1) to (x2,y2) with the CurrentColor that steadily fades out", NULL, NULL)
{
	if(!Gr_inited)
		return 0;

	int x1,y1,x2,y2;

	if(!ade_get_args(L, "iiii", &x1, &y1, &x2, &y2))
		return ADE_RETURN_NIL;

	gr_gradient(x1,y1,x2,y2,GR_RESIZE_NONE);

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawLine, l_Graphics, "number X1, number Y1, number X2, number Y2", "Draws a line from (x1,y1) to (x2,y2) with CurrentColor", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x1,y1,x2,y2;

	if(!ade_get_args(L, "iiii", &x1, &y1, &x2, &y2))
		return ADE_RETURN_NIL;

	gr_line(x1,y1,x2,y2,GR_RESIZE_NONE);

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawPixel, l_Graphics, "number X, number Y", "Sets pixel to CurrentColor", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x,y;

	if(!ade_get_args(L, "ii", &x, &y))
		return ADE_RETURN_NIL;

	gr_pixel(x,y,GR_RESIZE_NONE);

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawPolygon, l_Graphics, "texture Texture, [vector Position={0,0,0}, orientation Orientation=nil, number Width=1.0, number Height=1.0]", "Draws a polygon. May not work properly in hooks other than On Object Render.", NULL, NULL)
{
	int tdx = -1;
	vec3d pos = vmd_zero_vector;
	matrix_h *mh = NULL;
	float width = 1.0f;
	float height = 1.0f;
	if(!ade_get_args(L, "o|ooff", l_Texture.Get(&tdx), l_Vector.Get(&pos), l_Matrix.GetPtr(&mh), &width, &height))
		return ADE_RETURN_NIL;

	if(!bm_is_valid(tdx))
		return ADE_RETURN_FALSE;

	matrix *orip = &vmd_identity_matrix;
	if(mh != NULL)
		orip = mh->GetMatrix();

	//Do 3D stuff
	bool in_frame = g3_in_frame() > 0;
	if(!in_frame)
		g3_start_frame(0);

	//gr_set_bitmap(tdx, lua_Opacity_type, GR_BITBLT_MODE_NORMAL, lua_Opacity);
	//g3_draw_polygon(&pos, orip, width, height, TMAP_FLAG_TEXTURED | TMAP_HTL_3D_UNLIT);
	material mat_params;
	material_set_unlit(&mat_params, tdx, lua_Opacity, lua_Opacity_type == GR_ALPHABLEND_FILTER ? true : false, false);
	g3_render_rect_oriented(&mat_params, &pos, orip, width, height);

	if(!in_frame)
		g3_end_frame();

	return ADE_RETURN_TRUE;
}

ADE_FUNC(drawRectangle, l_Graphics, "number X1, number Y1, number X2, number Y2, [boolean Filled=true]", "Draws a rectangle with CurrentColor", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int x1,y1,x2,y2;
	bool f=true;

	if(!ade_get_args(L, "iiii|b", &x1, &y1, &x2, &y2, &f))
		return ADE_RETURN_NIL;

	if(f)
	{
		gr_set_bitmap(0);  // gr_rect will use the last bitmaps info, so set to zero to flush any previous alpha state
		gr_rect(x1, y1, x2-x1, y2-y1, GR_RESIZE_NONE);
	}
	else
	{
		gr_line(x1,y1,x2,y1,GR_RESIZE_NONE);	//Top
		gr_line(x1,y2,x2,y2,GR_RESIZE_NONE); //Bottom
		gr_line(x1,y1,x1,y2,GR_RESIZE_NONE);	//Left
		gr_line(x2,y1,x2,y2,GR_RESIZE_NONE);	//Right
	}

	return ADE_RETURN_NIL;
}

ADE_FUNC(drawSphere, l_Graphics, "[number Radius = 1.0, vector Position]", "Draws a sphere with radius Radius at world vector Position. May not work properly in hooks other than On Object Render.", "boolean", "True if successful, false or nil otherwise")
{
	float rad = 1.0f;
	vec3d pos = vmd_zero_vector;
	ade_get_args(L, "|fo", &rad, l_Vector.Get(&pos));

	bool in_frame = g3_in_frame() > 0;
	if(!in_frame) {
		g3_start_frame(0);

		vec3d cam_pos;
		matrix cam_orient;

		camid cid;
		
		if (Viewer_mode & VM_FREECAMERA)
			cid = cam_get_current();
		else
			cid = Main_camera;

		camera *cam = cid.getCamera();

		if (cam != NULL) {
			cam->get_info(&cam_pos, &cam_orient);
			g3_set_view_matrix(&cam_pos, &cam_orient, View_zoom);
		} else {
			g3_set_view_matrix(&Eye_position, &Eye_matrix, View_zoom);
		}

		gr_set_proj_matrix( Proj_fov, gr_screen.clip_aspect, Min_draw_distance, Max_draw_distance);
		gr_set_view_matrix(&Eye_position, &Eye_matrix);
	}

	vertex vtx;
	vtx.world = pos;
	g3_rotate_vertex(&vtx, &pos);
	g3_draw_sphere(&vtx, rad);

	if(!in_frame) {
		gr_end_view_matrix();
		gr_end_proj_matrix();
		g3_end_frame();
	}
	return ADE_RETURN_TRUE;
}

// Aardwolf's test code to render a model, supposed to emulate WMC's gr.drawModel function
ADE_FUNC(drawModel, l_Graphics, "model, position, orientation", "Draws the given model with the specified position and orientation - Use with extreme care, may not work properly in all scripting hooks.", "int", "Zero if successful, otherwise an integer error code")
{
	model_h *mdl = NULL;
	vec3d *v = &vmd_zero_vector;
	matrix_h *mh = NULL;
	if(!ade_get_args(L, "ooo", l_Model.GetPtr(&mdl), l_Vector.GetPtr(&v), l_Matrix.GetPtr(&mh)))
		return ade_set_args(L, "i", 1);

	if(mdl == NULL)
		return ade_set_args(L, "i", 2);

	int model_num = mdl->GetID();
	if(model_num < 0)
		return ade_set_args(L, "i", 3);

	//Handle angles
	matrix *orient = mh->GetMatrix();

	//Clip
	gr_set_clip(0, 0, gr_screen.max_w, gr_screen.max_h, GR_RESIZE_NONE);

	//Handle 3D init stuff
	g3_start_frame(1);

	vec3d cam_pos;
	matrix cam_orient;

	camid cid;
	if (Viewer_mode & VM_FREECAMERA)
		cid = cam_get_current();
	else
		cid = Main_camera;

	camera *cam = cid.getCamera();

	if (cam != NULL) {
		cam->get_info(&cam_pos, &cam_orient);
		g3_set_view_matrix(&cam_pos, &cam_orient, View_zoom);
	} else {
		g3_set_view_matrix(&Eye_position, &Eye_matrix, View_zoom);
	}

	gr_set_proj_matrix( Proj_fov, gr_screen.clip_aspect, Min_draw_distance, Max_draw_distance);
	gr_set_view_matrix(&Eye_position, &Eye_matrix);

	//Draw the ship!!
	model_clear_instance(model_num);
	model_set_detail_level(0);
	model_render_params render_info;

	render_info.set_detail_level_lock(0);

	model_render_immediate(&render_info, model_num, orient, v);

	//OK we're done
	gr_end_view_matrix();
	gr_end_proj_matrix();

	//Bye!!
	g3_end_frame();
	gr_reset_clip();

	return ade_set_args(L, "i", 0);
}

// Wanderer
ADE_FUNC(drawModelOOR, l_Graphics, "model Model, vector Position, matrix Orientation, [integer Flags]", "Draws the given model with the specified position and orientation - Use with extreme care, designed to operate properly only in On Object Render hooks.", "int", "Zero if successful, otherwise an integer error code")
{
	model_h *mdl = NULL;
	vec3d *v = &vmd_zero_vector;
	matrix_h *mh = NULL;
	int flags = MR_NORMAL;
	if(!ade_get_args(L, "ooo|i", l_Model.GetPtr(&mdl), l_Vector.GetPtr(&v), l_Matrix.GetPtr(&mh), &flags))
		return ade_set_args(L, "i", 1);

	if(mdl == NULL)
		return ade_set_args(L, "i", 2);

	polymodel *pm = mdl->Get();

	if (pm == NULL)
		return ade_set_args(L, "i", 3);

	int model_num = pm->id;

	if(model_num < 0)
		return ade_set_args(L, "i", 3);

	//Handle angles
	matrix *orient = mh->GetMatrix();

	//Draw the ship!!
	model_clear_instance(model_num);

	model_render_params render_info;
	render_info.set_flags(flags);

	model_render_immediate(&render_info, model_num, orient, v);

	return ade_set_args(L, "i", 0);
}

// Aardwolf's targeting brackets function
ADE_FUNC(drawTargetingBrackets, l_Graphics, "object Object, [boolean draw=true, int padding=5]",
	"Gets the edge positions of targeting brackets for the specified object. The brackets will only be drawn if draw is true or the default value of draw is used. Brackets are drawn with the current color. The brackets will have a padding (distance from the actual bounding box); the default value (used elsewhere in FS2) is 5.",
	"number,number,number,number",
	"Left, top, right, and bottom positions of the brackets, or nil if invalid")
{
	if(!Gr_inited) {
		return ADE_RETURN_NIL;
	}

	object_h *objh = NULL;
	bool draw_box = true;
	int padding = 5;

	if( !ade_get_args(L, "o|bi", l_Object.GetPtr(&objh), &draw_box, &padding) ) {
		return ADE_RETURN_NIL;
	}

	// The following code is mostly copied from
	// void hud_show_brackets(object *targetp, vertex *projected_v)
	// in hudtarget.cpp

	if( !objh->IsValid()) {
		return ADE_RETURN_NIL;
	}

	object *targetp = objh->objp;

	int x1,x2,y1,y2;
	int bound_rc, pof;
	int modelnum;
	bool entered_frame = false;
	SCP_list<CJumpNode>::iterator jnp;
	
	if ( !(g3_in_frame( ) > 0 ) )
	{
		g3_start_frame( 0 );
		entered_frame = true;
	}


	switch ( targetp->type ) {
		case OBJ_SHIP:
			modelnum = Ship_info[Ships[targetp->instance].ship_info_index].model_num;
			bound_rc = model_find_2d_bound_min( modelnum, &targetp->orient, &targetp->pos,&x1,&y1,&x2,&y2 );
			if ( bound_rc != 0 ) {
				if ( entered_frame )
					g3_end_frame( );
				return ADE_RETURN_NIL;
			}
			break;
		case OBJ_DEBRIS:
			modelnum = Debris[targetp->instance].model_num;
			bound_rc = submodel_find_2d_bound_min( modelnum, Debris[targetp->instance].submodel_num, &targetp->orient, &targetp->pos,&x1,&y1,&x2,&y2 );
			if ( bound_rc != 0 ) {
				if ( entered_frame )
					g3_end_frame( );
				return ADE_RETURN_NIL;
			}
			break;
		case OBJ_WEAPON:
			Assert(Weapon_info[Weapons[targetp->instance].weapon_info_index].subtype == WP_MISSILE);
			modelnum = Weapon_info[Weapons[targetp->instance].weapon_info_index].model_num;
			bound_rc = model_find_2d_bound_min( modelnum, &targetp->orient, &targetp->pos,&x1,&y1,&x2,&y2 );
			break;
		case OBJ_ASTEROID:
			pof = Asteroids[targetp->instance].asteroid_subtype;
			modelnum = Asteroid_info[Asteroids[targetp->instance].asteroid_type].model_num[pof];
			bound_rc = model_find_2d_bound_min( modelnum, &targetp->orient, &targetp->pos,&x1,&y1,&x2,&y2 );
			break;
		case OBJ_JUMP_NODE:
			for (jnp = Jump_nodes.begin(); jnp != Jump_nodes.end(); ++jnp) {
				if(jnp->GetSCPObject() == targetp)
					break;
			}
			
			modelnum = jnp->GetModelNumber();
			bound_rc = model_find_2d_bound_min( modelnum, &targetp->orient, &targetp->pos,&x1,&y1,&x2,&y2 );
			break;
		default: //Someone passed an invalid pointer.
			if ( entered_frame )
				g3_end_frame( );
			return ADE_RETURN_NIL;
	}

	x1 -= padding;
	x2 += padding;
	y1 -= padding;
	y2 += padding;
	if ( draw_box ) {
		draw_brackets_square(x1, y1, x2, y2, GR_RESIZE_NONE);
	}

	if ( entered_frame )
		g3_end_frame( );

	return ade_set_args(L, "iiii", x1, y1, x2, y2);
}

ADE_FUNC(drawSubsystemTargetingBrackets, l_Graphics, "subsystem subsys, [boolean draw=true, boolean setColor=false]",
	"Gets the edge position of the targeting brackets drawn for a subsystem as if they were drawn on the HUD. Only actually draws the brackets if <i>draw</i> is true, optionally sets the color the as if it was drawn on the HUD",
	"number,number,number,number",
	"Left, top, right, and bottom positions of the brackets, or nil if invalid or off-screen")
{
	if(!Gr_inited) {
		return ADE_RETURN_NIL;
	}

	ship_subsys_h *sshp = NULL;
	bool draw = true;
	bool set_color = false;

	if( !ade_get_args(L, "o|bb", l_Subsystem.GetPtr(&sshp), &draw, &set_color) ) {
		return ADE_RETURN_NIL;
	}

	if (!sshp->IsValid())
	{
		return ADE_RETURN_NIL;
	}

	bool entered_frame = false;
	
	if ( !(g3_in_frame( ) > 0 ) )
	{
		g3_start_frame( 0 );
		entered_frame = true;
	}

	int coords[4];

	int in_sight = draw_subsys_brackets(sshp->ss, 24, 24, draw, set_color, coords);

	if ( entered_frame )
		g3_end_frame( );

	if (in_sight > 0)
	{
		return ade_set_args(L, "iiii", coords[0], coords[1], coords[2], coords[3]);
	}
	else
	{
		return ADE_RETURN_NIL;
	}
}

ADE_FUNC(drawOffscreenIndicator, l_Graphics, "object Object, [boolean draw=true, boolean setColor=false]",
	"Draws an off-screen indicator for the given object. The indicator will not be drawn if draw=false, but the coordinates will be returned in either case. The indicator will be drawn using the current color if setColor=true and using the IFF color of the object if setColor=false.",
	"number,number",
	"Coordinates of the indicator (at the very edge of the screen), or nil if object is on-screen")
{
	object_h *objh = NULL;
	bool draw = false;
	bool setcolor = false;
	vec2d outpoint = { -1.0f, -1.0f };
	
	if(!Gr_inited) {
		return ADE_RETURN_NIL;
	}
	
	if( !ade_get_args(L, "o|bb", l_Object.GetPtr(&objh), &draw, &setcolor) ) {
		return ADE_RETURN_NIL;
	}

	if( !objh->IsValid()) {
		return ADE_RETURN_NIL;
	}

	object *targetp = objh->objp;
	bool in_frame = g3_in_frame() > 0;

	if (!in_frame)
		g3_start_frame(0);

	vertex target_point;
	g3_rotate_vertex(&target_point, &targetp->pos);
	g3_project_vertex(&target_point);

	if (!in_frame)
		g3_end_frame();

	if(target_point.codes == 0)
		return ADE_RETURN_NIL;

	hud_target_clear_display_list();
	hud_target_add_display_list(targetp, &target_point, &targetp->pos, 5, NULL, NULL, TARGET_DISPLAY_DIST);

	size_t j, num_gauges;
	num_gauges = default_hud_gauges.size();

	for(j = 0; j < num_gauges; j++) {
		if (default_hud_gauges[j]->getObjectType() == HUD_OBJECT_OFFSCREEN) {
			HudGaugeOffscreen *offscreengauge = static_cast<HudGaugeOffscreen*>(default_hud_gauges[j]);
			
			offscreengauge->preprocess();
			offscreengauge->onFrame(flFrametime);

			if ( !offscreengauge->canRender() ) {
				break;
			}

			offscreengauge->resetClip();
			offscreengauge->setFont();
			int dir;
			float tri_separation;

			offscreengauge->calculatePosition(&target_point, &targetp->pos, &outpoint, &dir, &tri_separation);

			if (draw) {
				float distance = hud_find_target_distance(targetp, Player_obj);

				if (!setcolor)
					hud_set_iff_color(targetp, 1);

				offscreengauge->renderOffscreenIndicator(&outpoint, dir, distance, tri_separation, true);
			}

			offscreengauge->resize(&outpoint.x, &outpoint.y);

			break;
		}
	}

	if (outpoint.x >= 0 && outpoint.y >=0)
		return ade_set_args(L, "ii", (int)outpoint.x, (int)outpoint.y);
	else
		return ADE_RETURN_NIL;
}

#define MAX_TEXT_LINES		256
static char *BooleanValues[] = {"False", "True"};
static const int NextDrawStringPosInitial[] = {0, 0};
static int NextDrawStringPos[] = {NextDrawStringPosInitial[0], NextDrawStringPosInitial[1]};
ADE_FUNC(drawString, l_Graphics, "string Message, [number X1, number Y1, number X2, number Y2]",
		 "Draws a string. Use x1/y1 to control position, x2/y2 to limit textbox size."
		 "Text will automatically move onto new lines, if x2/y2 is specified."
		 "Additionally, calling drawString with only a string argument will automatically"
		 "draw that string below the previously drawn string (or 0,0 if no strings"
		 "have been drawn yet",
		 "number",
		 "Number of lines drawn, or 0 on failure")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	int x=NextDrawStringPos[0];
	int y = NextDrawStringPos[1];

	char *s = "(null)";
	int x2=-1,y2=-1;
	int num_lines = 0;

	if(lua_isboolean(L, 1))
	{
		bool b = false;
		if(!ade_get_args(L, "b|iiii", &b, &x, &y, &x2, &y2))
			return ade_set_error(L, "i", 0);

		if(b)
			s = BooleanValues[1];
		else
			s = BooleanValues[0];
	}
	else if(lua_isstring(L, 1))
	{
		if(!ade_get_args(L, "s|iiii", &s, &x, &y, &x2, &y2))
			return ade_set_error(L, "i", 0);
	}
	else
	{
		ade_get_args(L, "|*iiii", &x, &y, &x2, &y2);
	}

	NextDrawStringPos[0] = x;
	if(x2 < 0)
	{
		num_lines = 1;
		gr_string(x,y,s,GR_RESIZE_NONE);

		int height = 0;
		gr_get_string_size(NULL, &height, s);
		NextDrawStringPos[1] = y+height;
	}
	else
	{
		int linelengths[MAX_TEXT_LINES];
		const char *linestarts[MAX_TEXT_LINES];

		if (y2 >= 0 && y2 < y)
		{
			// Invalid y2 value
			Warning(LOCATION, "Illegal y2 value passed to drawString. Got %d y2 value but %d for y.", y2, y);
		
			int temp = y;
			y = y2;
			y2 = temp;
		}

		num_lines = split_str(s, x2-x, linelengths, linestarts, MAX_TEXT_LINES);

		//Make sure we don't go over size
		int line_ht = gr_get_font_height();
		num_lines = MIN(num_lines, (y2 - y) / line_ht);

		int curr_y = y;
		for(int i = 0; i < num_lines; i++)
		{
			//Contrary to WMC's previous comment, let's make a new string each line
			int len = linelengths[i];
			char *buf = new char[len+1];
			strncpy(buf, linestarts[i], len);
			buf[len] = '\0';

			//Draw the string
			gr_string(x,curr_y,buf,GR_RESIZE_NONE);

			//Free the string we made
			delete[] buf;

			//Increment line height
			curr_y += line_ht;
		}
		
		if (num_lines <= 0)
		{
			// If no line was drawn then we need to add one so the next line is 
			// aligned right
			curr_y += line_ht;
		}
		
		NextDrawStringPos[1] = curr_y;
	}
	return ade_set_error(L, "i", num_lines);
}

ADE_FUNC(getStringWidth, l_Graphics, "string String", "Gets string width", "number", "String width, or 0 on failure")
{
	if(!Gr_inited)
		return ade_set_error(L, "i", 0);

	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "i", 0);

	int w;

	gr_get_string_size(&w, NULL, s);
	
	return ade_set_args(L, "i", w);
}

ADE_FUNC(loadStreamingAnim, l_Graphics, "string Filename, [boolean loop, boolean reverse, boolean pause, boolean cache]",
		 "Plays a streaming animation, returning its handle. The optional booleans (except cache) can also be set via the handles virtvars<br>"
		 "cache is best set to false when loading animations that are only intended to play once, e.g. headz<br>"
		 "Remember to call the unload() function when you're finished using the animation to free up memory.",
		 "streaminganim",
		 "Streaming animation handle, or invalid handle if animation couldn't be loaded")
{
	char *s;
	int rc = -1;
	bool loop = false, reverse = false, pause = false, cache = true;

	if(!ade_get_args(L, "s|bbbb", &s, &loop, &reverse, &pause, &cache))
		return ADE_RETURN_NIL;

	streaminganim_h sah(s);
	if (loop == false) {
		sah.ga.direction |= GENERIC_ANIM_DIRECTION_NOLOOP;
	}
	if (reverse == true) {
		sah.ga.direction |= GENERIC_ANIM_DIRECTION_BACKWARDS;
	}
	if (pause == true) {
		sah.ga.direction |= GENERIC_ANIM_DIRECTION_PAUSED;
	}
	rc = generic_anim_stream(&sah.ga, cache);

	if(rc < 0)
		return ade_set_args(L, "o", l_streaminganim.Set(sah)); // this object should be "invalid", matches loadTexture behaviour

	return ade_set_args(L, "o", l_streaminganim.Set(sah));
}

ADE_FUNC(createTexture, l_Graphics, "[number Width=512, number Height=512, enumeration Type=TEXTURE_DYNAMIC]",
		 "Creates a texture for rendering to."
		 "Types are TEXTURE_STATIC - for infrequent rendering - and TEXTURE_DYNAMIC - for frequent rendering.",
		 "texture",
		 "New texture handle, or invalid texture handle if texture couldn't be created")
{
	int w=512;
	int h=512;
	enum_h *e = NULL;

	//GET ARGS
	ade_get_args(L, "|iio", &w, &h, l_Enum.GetPtr(&e));

	int t = BMP_FLAG_RENDER_TARGET_DYNAMIC;
	if(e != NULL)
	{
		if(e->index == LE_TEXTURE_STATIC)
			t = BMP_FLAG_RENDER_TARGET_STATIC;
		else if(e->index == LE_TEXTURE_DYNAMIC)
			t = BMP_FLAG_RENDER_TARGET_DYNAMIC;
	}

	int idx = bm_make_render_target(w, h, t);

	if(idx < 0)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	return ade_set_args(L, "o", l_Texture.Set(idx));
}

ADE_FUNC(loadTexture, l_Graphics, "string Filename, [boolean LoadIfAnimation, boolean NoDropFrames]",
		 "Gets a handle to a texture. If second argument is set to true, animations will also be loaded."
		 "If third argument is set to true, every other animation frame will not be loaded if system has less than 48 MB memory."
		 "<br><strong>IMPORTANT:</strong> Textures will not be unload themselves unless you explicitly tell them to do so."
		 "When you are done with a texture, call the Unload() function to free up memory.",
		 "texture",
		 "Texture handle, or invalid texture handle if texture couldn't be loaded")
{
	char *s;
	int idx=-1;
	bool b=false;
	bool d=false;

	if(!ade_get_args(L, "s|bb", &s, &b, &d))
		return ade_set_error(L, "o", l_Texture.Set(-1));

	if(b == true) {
		idx = bm_load_animation(s, nullptr, nullptr, nullptr, nullptr, d);
	}
	if(idx < 0) {
		idx = bm_load(s);
	}

	if(idx < 0)
		return ade_set_error(L, "o", l_Texture.Set(-1));

	return ade_set_args(L, "o", l_Texture.Set(idx));
}

ADE_FUNC(drawImage, l_Graphics, "string Filename/texture Texture, [number X1=0, Y1=0, number X2, number Y2, number UVX1 = 0.0, number UVY1 = 0.0, number UVX2=1.0, number UVY2=1.0, number alpha=1.0]",
		 "Draws an image or texture. Any image extension passed will be ignored."
		 "The UV variables specify the UV value for each corner of the image. "
		 "In UV coordinates, (0,0) is the top left of the image; (1,1) is the lower right.",
		 "boolean",
		 "Whether image was drawn")
{
	if(!Gr_inited)
		return ade_set_error(L, "b", false);

	int idx;
	int x1 = 0;
	int y1 = 0;
	int x2=INT_MAX;
	int y2=INT_MAX;
	float uv_x1=0.0f;
	float uv_y1=0.0f;
	float uv_x2=1.0f;
	float uv_y2=1.0f;
	float alpha=1.0f;

	if(lua_isstring(L, 1))
	{
		char *s = NULL;
		if(!ade_get_args(L, "s|iiiifffff", &s,&x1,&y1,&x2,&y2,&uv_x1,&uv_y1,&uv_x2,&uv_y2,&alpha))
			return ade_set_error(L, "b", false);

		idx = Script_system.LoadBm(s);

		if(idx < 0)
			return ADE_RETURN_FALSE;
	}
	else
	{
		if(!ade_get_args(L, "o|iiiifffff", l_Texture.Get(&idx),&x1,&y1,&x2,&y2,&uv_x1,&uv_y1,&uv_x2,&uv_y2,&alpha))
			return ade_set_error(L, "b", false);
	}

	if(!bm_is_valid(idx))
		return ade_set_error(L, "b", false);

	int w, h;
	if(bm_get_info(idx, &w, &h) < 0)
		return ADE_RETURN_FALSE;

	if(x2!=INT_MAX)
		w = x2-x1;

	if(y2!=INT_MAX)
		h = y2-y1;

	gr_set_bitmap(idx, lua_Opacity_type, GR_BITBLT_MODE_NORMAL, alpha);
	bitmap_rect_list brl = bitmap_rect_list(x1, y1, w, h, uv_x1, uv_y1, uv_x2, uv_y2);
	gr_bitmap_list(&brl, 1, GR_RESIZE_NONE);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(drawMonochromeImage, l_Graphics, "string Filename/texture Texture, number X1, number Y1, [number X2, number Y2, number alpha=1.0]", "Draws a monochrome image using the current color", "boolean", "Whether image was drawn")
{
	if(!Gr_inited)
		return ade_set_error(L, "b", false);

	int idx;
	int x,y;
	int x2=INT_MAX;
	int y2=INT_MAX;
	int sx=0;
	int sy=0;
	bool m = false;
	float alpha=1.0;

	if(lua_isstring(L, 1))
	{
		char *s = NULL;
		if(!ade_get_args(L, "sii|iif", &s,&x,&y,&x2,&y2,&alpha))
			return ade_set_error(L, "b", false);

		idx = Script_system.LoadBm(s);

		if(idx < 0)
			return ADE_RETURN_FALSE;
	}
	else
	{
		if(!ade_get_args(L, "oii|iif", l_Texture.Get(&idx),&x,&y,&x2,&y2,&alpha))
			return ade_set_error(L, "b", false);
	}

	if(!bm_is_valid(idx))
		return ade_set_error(L, "b", false);

	int w, h;
	if(bm_get_info(idx, &w, &h) < 0)
		return ADE_RETURN_FALSE;

	if(sx < 0)
		sx = w + sx;

	if(sy < 0)
		sy = h + sy;
	
	if(x2!=INT_MAX)
		w = x2-x;

	if(y2!=INT_MAX)
		h = y2-y;

	gr_set_bitmap(idx, lua_Opacity_type, GR_BITBLT_MODE_NORMAL,alpha);
	gr_aabitmap_ex(x, y, w, h, sx, sy, GR_RESIZE_NONE, m);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(getImageWidth, l_Graphics, "string Filename", "Gets image width", "number", "Image width, or 0 if filename is invalid")
{
	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "i", 0);

	int w;
	
	int idx = bm_load(s);

	if(idx < 0)
		return ade_set_error(L, "i", 0);

	bm_get_info(idx, &w);
	return ade_set_args(L, "i", w);
}

ADE_FUNC(getImageHeight, l_Graphics, "Image name", "Gets image height", "number", "Image height, or 0 if filename is invalid")
{
	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "i", 0);

	int h;
	
	int idx = bm_load(s);

	if(idx < 0)
		return ade_set_error(L, "i", 0);

	bm_get_info(idx, NULL, &h);
	return ade_set_args(L, "i", h);
}

ADE_FUNC(flashScreen, l_Graphics, "number Red, number Green, number Blue", "Flashes the screen", NULL, NULL)
{
	if(!Gr_inited)
		return ADE_RETURN_NIL;

	int r,g,b;

	if(!ade_get_args(L, "iii", &r, &g, &b))
		return ADE_RETURN_NIL;

	gr_flash(r,g,b);

	return ADE_RETURN_NIL;
}

ADE_FUNC(loadModel, l_Graphics, "string Filename", "Loads the model - will not setup subsystem data, DO NOT USE FOR LOADING SHIP MODELS", "model", "Handle to a model")
{
	char *s;
	int model_num = -1;

	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "o", l_Model.Set(-1));

	if (s[0] == '\0')
		return ade_set_error(L, "o", l_Model.Set(-1));

	model_num = model_load(s, 0, NULL);

	return ade_set_args(L, "o", l_Model.Set(model_h(model_num)));
}

ADE_FUNC(hasViewmode, l_Graphics, "enumeration", "Specifies if the current viemode has the specified flag, see VM_* enumeration", "boolean", "true if flag is present, false otherwise")
{
	enum_h *type = NULL;

	if (!ade_get_args(L, "o", l_Enum.GetPtr(&type)))
		return ade_set_error(L, "b", false);

	if (type == NULL || !type->IsValid())
		return ade_set_error(L, "b", false);

	int bit = 0;

	switch(type->index)
	{
	case LE_VM_INTERNAL:
		return ade_set_args(L, "b", (Viewer_mode & ~(VM_CAMERA_LOCKED | VM_CENTERING)) == 0);	// z64: Ignore camera lock state and centering state
		break;

	case LE_VM_EXTERNAL:
		bit = VM_EXTERNAL;
		break;

	case LE_VM_OTHER_SHIP:
		bit = VM_OTHER_SHIP;
		break;

	case LE_VM_CHASE:
		bit = VM_CHASE;
		break;

	case LE_VM_DEAD_VIEW:
		bit = VM_DEAD_VIEW;
		break;

	case LE_VM_EXTERNAL_CAMERA_LOCKED:
		return ade_set_args(L, "b", (Viewer_mode & VM_CAMERA_LOCKED) && (Viewer_mode & VM_EXTERNAL));
		break;

	case LE_VM_CAMERA_LOCKED:
		bit = VM_CAMERA_LOCKED;
		break;

	case LE_VM_FREECAMERA:
		bit = VM_FREECAMERA;
		break;

	case LE_VM_PADLOCK_LEFT:
		bit = VM_PADLOCK_LEFT;
		break;

	case LE_VM_PADLOCK_REAR:
		bit = VM_PADLOCK_REAR;
		break;

	case LE_VM_PADLOCK_RIGHT:
		bit = VM_PADLOCK_RIGHT;
		break;

	case LE_VM_PADLOCK_UP:
		bit = VM_PADLOCK_UP;
		break;

	case LE_VM_TOPDOWN:
		bit = VM_TOPDOWN;
		break;

	case LE_VM_TRACK:
		bit = VM_TRACK;
		break;

	case LE_VM_WARP_CHASE:
		bit = VM_WARP_CHASE;
		break;

	case LE_VM_WARPIN_ANCHOR:
		bit = VM_WARPIN_ANCHOR;
		break;

	case LE_VM_CENTERING:
		bit = VM_CENTERING;
		break;

	default:
		LuaError(L, "Attempted to use hasViewmode with an invalid enumeration! Only VM_* enumerations are allowed!");
		return ade_set_error(L, "b", false);
		break;
	}

	return ade_set_args(L, "b", (Viewer_mode & bit) != 0);
}

ADE_FUNC(setClip, l_Graphics, "x, y, width, height", "Sets the clipping region to the specified rectangle. Most drawing functions are able to handle the offset.", "boolean", "true if successful, false otherwise")
{
	int x, y, width, height;

	if (!ade_get_args(L, "iiii", &x, &y, &width, &height))
		return ADE_RETURN_FALSE;

	gr_set_clip(x, y, width, height, GR_RESIZE_NONE);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(resetClip, l_Graphics, NULL, "Resets the clipping region that might have been set", "boolean", "true if successful, false otherwise")
{
	gr_reset_clip();

	return ADE_RETURN_TRUE;
}

//**********LIBRARY: Scripting Variables
ade_lib l_HookVar("HookVariables", NULL, "hv", "Hook variables repository");

//WMC: IMPORTANT
//Be very careful when modifying this library, as the Globals[] library does depend
//on the current number of items in the library. If you add _anything_, modify __len.
//Or run changes by me.

//*****LIBRARY: Scripting Variables
ade_lib l_HookVar_Globals("Globals", &l_HookVar);

ADE_INDEXER(l_HookVar_Globals, "number Index", "Array of current HookVariable names", "string", "Hookvariable name, or empty string if invalid index specified")
{
	int idx;
	if(!ade_get_args(L, "*i", &idx))
		return ade_set_error(L, "s", "");

	//Get lib
	lua_getglobal(L, l_HookVar.GetName());
	int lib_ldx = lua_gettop(L);
	if(!lua_isuserdata(L, lib_ldx))
	{
		lua_pop(L, 1);
		return ade_set_error(L, "s", "");
	}

	//Get metatable
	lua_getmetatable(L, lib_ldx);
	int mtb_ldx = lua_gettop(L);
	if(!lua_istable(L, mtb_ldx))
	{
		lua_pop(L, 2);
		return ade_set_error(L, "s", "");
	}

	//Get ade members table
	lua_pushstring(L, "__ademembers");
	lua_rawget(L, mtb_ldx);
	int amt_ldx = lua_gettop(L);
	if(!lua_istable(L, amt_ldx))
	{
		lua_pop(L, 3);
		return ade_set_error(L, "s", "");
	}

	//List 'em
	char *keyname = NULL;
	int count = 1;
	lua_pushnil(L);
	while(lua_next(L, amt_ldx))
	{
		//Now on stack: Key, value
		lua_pushvalue(L, -2);
		keyname = (char *)lua_tostring(L, -1);
		if(strcmp(keyname, "Globals"))
		{
			if(count == idx)
			{
				//lib, mtb, amt, key, value, string go bye-bye
				lua_pop(L, 5);
				return ade_set_args(L, "s", keyname);
			}
			count++;
		}
		lua_pop(L, 2);	//Value, string
	}

	lua_pop(L, 3);	//lib, mtb, amt

	return ade_set_error(L, "s", "");
}

ADE_FUNC(__len, l_HookVar_Globals, NULL, "Number of HookVariables", "number", "Number of HookVariables")
{
	//Get metatable
	lua_getglobal(L, l_HookVar.GetName());
	int lib_ldx = lua_gettop(L);
	if(!lua_isuserdata(L, lib_ldx))
	{
		lua_pop(L, 1);
		return ade_set_error(L, "i", 0);
	}

	lua_getmetatable(L, lib_ldx);
	int mtb_ldx = lua_gettop(L);
	if(!lua_istable(L, mtb_ldx))
	{
		lua_pop(L, 2);
		return ade_set_error(L, "i", 0);
	}

	//Get ade members table
	lua_pushstring(L, "__ademembers");
	lua_rawget(L, mtb_ldx);
	int amt_ldx = lua_gettop(L);
	if(!lua_istable(L, amt_ldx))
	{
		lua_pop(L, 3);
		return ade_set_error(L, "i", 0);
	}

	int total_len = 0;
	lua_pushnil(L);
	while(lua_next(L, amt_ldx))
	{
		total_len++;
		lua_pop(L, 1);	//value
	}
	size_t num_sub = ade_manager::getInstance()->getEntry(l_HookVar.GetIdx()).Num_subentries;

	lua_pop(L, 3);

	//WMC - Return length, minus the 'Globals' library
	return ade_set_args(L, "i", (int)(total_len - num_sub));
}

//**********LIBRARY: Mission
ade_lib l_Mission("Mission", NULL, "mn", "Mission library");

// for use in creating faster metadata systems, use in conjunction with getSignature()
ADE_FUNC(getObjectFromSignature, l_Mission, "number Signature", "Gets a handle of an object from its signature", "object", "Handle of object with signaure, invalid handle if signature is not in use")
{
	int sig = -1;
	int objnum;
	if(!ade_get_args(L, "i", &sig))
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	if (sig == -1) {
		return ade_set_error(L, "o", l_Object.Set(object_h()));
	}

	objnum = obj_get_by_signature(sig);

	return ade_set_object_with_breed(L, objnum);
}

ADE_FUNC(evaluateSEXP, l_Mission, "string", "Runs the defined SEXP script", "boolean", "if the operation was successful")
{
	char *s;
	int r_val;

	if(!ade_get_args(L, "s", &s))
		return ADE_RETURN_FALSE;

	r_val = run_sexp(s);

	if (r_val == SEXP_TRUE)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

ADE_FUNC(runSEXP, l_Mission, "string", "Runs the defined SEXP script", "boolean", "if the operation was successful")
{
	char *s;
	int r_val;
	char buf[8192];

	if(!ade_get_args(L, "s", &s))
		return ADE_RETURN_FALSE;

	while (is_white_space(*s))
		s++;
	if (*s != '(')
	{
		static bool Warned_about_runSEXP_parentheses = false;
		if (!Warned_about_runSEXP_parentheses)
		{
			Warned_about_runSEXP_parentheses = true;
			Warning(LOCATION, "Invalid SEXP syntax: SEXPs must be surrounded by parentheses.  For backwards compatibility, the string has been enclosed in parentheses.  This may not be correct in all use cases.");
		}
		// this is the old sexp handling method, which is incorrect
		snprintf(buf, 8191, "( when ( true ) ( %s ) )", s);
	}
	else
	{
		// this is correct usage
		snprintf(buf, 8191, "( when ( true ) %s )", s);
	}

	r_val = run_sexp(buf);

	if (r_val == SEXP_TRUE)
		return ADE_RETURN_TRUE;
	else
		return ADE_RETURN_FALSE;
}

//****SUBLIBRARY: Mission/Asteroids
ade_lib l_Mission_Asteroids("Asteroids", &l_Mission, NULL, "Asteroids in the mission");

ADE_INDEXER(l_Mission_Asteroids, "number Index", "Gets asteroid", "asteroid", "Asteroid handle, or invalid handle if invalid index specified")
{
	int idx = -1;
	if( !ade_get_args(L, "*i", &idx) ) {
		return ade_set_error( L, "o", l_Asteroid.Set( object_h() ) );
	}
	if( idx > -1 && idx < asteroid_count() ) {
		idx--; //Convert from Lua to C, as lua indices start from 1, not 0
		return ade_set_args( L, "o", l_Asteroid.Set( object_h( &Objects[Asteroids[idx].objnum] ), Objects[Asteroids[idx].objnum].signature ) );
	}

	return ade_set_error(L, "o", l_Asteroid.Set( object_h() ) );
}

ADE_FUNC(__len, l_Mission_Asteroids, NULL,
		 "Number of asteroids in mission. Note that the value returned is only good until an asteroid is destroyed, and so cannot be relied on for more than one frame.",
		 "number",
		 "Number of asteroids in the mission, or 0 if asteroids are not enabled")
{
	if(Asteroids_enabled) {
		return ade_set_args(L, "i", asteroid_count());
	}
	return ade_set_args(L, "i", 0);
}

//****SUBLIBRARY: Mission/Debris
ade_lib l_Mission_Debris("Debris", &l_Mission, NULL, "debris in the mission");

ADE_INDEXER(l_Mission_Debris, "number Index", "Array of debris in the current mission", "debris", "Debris handle, or invalid debris handle if index wasn't valid")
{
	int idx = -1;
	if( !ade_get_args( L, "*i", &idx ) ) {
		return ade_set_error(L, "o", l_Debris.Set(object_h()));
	}
	if( idx > -1 && idx < Num_debris_pieces ) {
		idx--; // Lua -> C
		if (Debris[idx].objnum == -1) //Somehow accessed an invalid debris piece
			return ade_set_error(L, "o", l_Debris.Set(object_h()));
		return ade_set_args(L, "o", l_Debris.Set(object_h(&Objects[Debris[idx].objnum]), Objects[Debris[idx].objnum].signature));
	}

	return ade_set_error(L, "o", l_Debris.Set(object_h()));
}

ADE_FUNC(__len, l_Mission_Debris, NULL, 
		 "Number of debris pieces in the mission. "
		 "Note that the value returned is only good until a piece of debris is destroyed, and so cannot be relied on for more than one frame.",
		 "number",
		 "Current number of debris particles")
{
	return ade_set_args(L, "i", Num_debris_pieces);
}

//****SUBLIBRARY: Mission/EscortShips
ade_lib l_Mission_EscortShips("EscortShips", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_EscortShips, "number Index", "Gets escort ship at specified index on escort list", "ship", "Specified ship, or invalid ship handle if invalid index")
{
	int idx;
	if(!ade_get_args(L, "*i", &idx))
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	if(idx < 1 || idx > hud_escort_num_ships_on_list())
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	//Lua->FS2
	idx--;

	idx = hud_escort_return_objnum(idx);
	
	if(idx < 0)
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[idx])));
}

ADE_FUNC(__len, l_Mission_EscortShips, NULL, "Current number of escort ships", "number", "Current number of escort ships")
{
	return ade_set_args(L, "i", hud_escort_num_ships_on_list());
}

//****SUBLIBRARY: Mission/Events
ade_lib l_Mission_Events("Events", &l_Mission, NULL, "Events");

ADE_INDEXER(l_Mission_Events, "number Index/string Name", "Indexes events list", "event", "Event handle, or invalid event handle if index was invalid")
{
	char *s;
	if(!ade_get_args(L, "*s", &s))
		return ade_set_error(L, "o", l_Event.Set(-1));

	int i;
	for(i = 0; i < Num_mission_events; i++)
	{
		if(!stricmp(Mission_events[i].name, s))
			return ade_set_args(L, "o", l_Event.Set(i));
	}

	//Now try as a number
	i = atoi(s);
	if(i < 1 || i > Num_mission_events)
		return ade_set_error(L, "o", l_Event.Set(-1));

	//Lua-->FS2
	i--;

	return ade_set_args(L, "o", l_Event.Set(i));
}

ADE_FUNC(__len, l_Mission_Events, NULL, "Number of events in mission", "number", "Number of events in mission")
{
	return ade_set_args(L, "i", Num_mission_events);
}

//****SUBLIBRARY: Mission/SEXPVariables
ade_lib l_Mission_SEXPVariables("SEXPVariables", &l_Mission, NULL, "SEXP Variables");

ADE_INDEXER(l_Mission_SEXPVariables, "number Index/string Name", "Array of SEXP variables. Note that you can set a sexp variable using the array, eg \'SEXPVariables[\"newvariable\"] = \"newvalue\"\'", "sexpvariable", "Handle to SEXP variable, or invalid sexpvariable handle if index was invalid")
{
	char *name = NULL;
	char *newval = NULL;
	if(!ade_get_args(L, "*s|s", &name, &newval))
		return ade_set_error(L, "o", l_SEXPVariable.Set(sexpvar_h()));

	int idx = get_index_sexp_variable_name(name);
	if(idx < 0)
	{
		idx = atoi(name);

		//Lua-->FS2
		idx--;
	}

	if(idx < 0 || idx >= MAX_SEXP_VARIABLES)
	{
		if(ADE_SETTING_VAR && newval != NULL)
		{
			idx = sexp_add_variable(newval, name, lua_type(L, 2) == LUA_TNUMBER ? SEXP_VARIABLE_NUMBER : SEXP_VARIABLE_STRING);
		}

		//We have failed.
		if(idx < 0)
		{
			return ade_set_error(L, "o", l_SEXPVariable.Set(sexpvar_h()));
		}
	}
	else
	{
		if(ADE_SETTING_VAR && newval != NULL)
		{
			sexp_modify_variable(newval, idx, false);
		}
	}

	return ade_set_args(L, "o", l_SEXPVariable.Set(sexpvar_h(idx)));
}

ADE_FUNC(__len, l_Mission_SEXPVariables, NULL, "Current number of SEXP variables", "number", "Counts number of loaded SEXP Variables. May be slow.")
{
	return ade_set_args(L, "i", sexp_variable_count());
}

//****SUBLIBRARY: Mission/Ships
ade_lib l_Mission_Ships("Ships", &l_Mission, NULL, "Ships in the mission");

ADE_INDEXER(l_Mission_Ships, "number Index/string Name", "Gets ship", "ship", "Ship handle, or invalid ship handle if index was invalid")
{
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_Ship.Set(object_h()));

	int idx = ship_name_lookup(name);

	if(idx > -1)
	{
		return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[Ships[idx].objnum]), Objects[Ships[idx].objnum].signature));
	}
	else
	{
		idx = atoi(name);
		if(idx > 0)
		{
			int count=1;

			for(int i = 0; i < MAX_SHIPS; i++)
			{
				if (Ships[i].objnum < 0 || Objects[Ships[i].objnum].type != OBJ_SHIP)
					continue;

				if(count == idx) {
					return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[Ships[i].objnum]), Objects[Ships[i].objnum].signature));
				}

				count++;
			}
		}
	}

	return ade_set_error(L, "o", l_Ship.Set(object_h()));
}

extern int ships_inited;
ADE_FUNC(__len, l_Mission_Ships, NULL,
		 "Number of ships in the mission. "
		 "This function is somewhat slow, and should be set to a variable for use in looping situations. "
		 "Note that the value returned is only good until a ship is destroyed, and so cannot be relied on for more than one frame.",
		 "number",
		 "Number of ships in the mission, or 0 if ships haven't been initialized yet")
{
	if(ships_inited)
		return ade_set_args(L, "i", ship_get_num_ships());
	else
		return ade_set_args(L, "i", 0);
}

//****SUBLIBRARY: Mission/Waypoints
ade_lib l_Mission_Waypoints("Waypoints", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Waypoints, "number Index", "Array of waypoints in the current mission", "waypoint", "Waypoint handle, or invalid waypoint handle if index was invalid")
{
	int idx;
	if(!ade_get_args(L, "*i", &idx))
		return ade_set_error(L, "o", l_Waypoint.Set(object_h()));

	//Remember, Lua indices start at 0.
	int count=0;

	object *ptr = GET_FIRST(&obj_used_list);
	while (ptr != END_OF_LIST(&obj_used_list))
	{
		if (ptr->type == OBJ_WAYPOINT)
			count++;

		if(count == idx) {
			return ade_set_args(L, "o", l_Waypoint.Set(object_h(ptr)));
		}

		ptr = GET_NEXT(ptr);
	}

	return ade_set_error(L, "o", l_Weapon.Set(object_h()));
}

ADE_FUNC(__len, l_Mission_Waypoints, NULL, "Gets number of waypoints in mission. Note that this is only accurate for one frame.", "number", "Number of waypoints in the mission")
{
	uint count=0;
	for(uint i = 0; i < MAX_OBJECTS; i++)
	{
		if (Objects[i].type == OBJ_WAYPOINT)
			count++;
	}

	return ade_set_args(L, "i", count);
}

//****SUBLIBRARY: Mission/WaypointLists
ade_lib l_Mission_WaypointLists("WaypointLists", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_WaypointLists, "number Index/string WaypointListName", "Array of waypoint lists", "waypointlist", "Gets waypointlist handle")
{
	waypointlist_h wpl;
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_WaypointList.Set(waypointlist_h()));

	wpl = waypointlist_h(name);

	if (!wpl.IsValid()) {
		int idx = atoi(name) - 1;
		wpl = waypointlist_h(find_waypoint_list_at_index(idx));
	}

	if (wpl.IsValid()) {
		return ade_set_args(L, "o", l_WaypointList.Set(wpl));
	}

	return ade_set_error(L, "o", l_WaypointList.Set(waypointlist_h()));
}

ADE_FUNC(__len, l_Mission_WaypointLists, NULL, "Number of waypoint lists in mission. Note that this is only accurate for one frame.", "number", "Number of waypoint lists in the mission")
{
	return ade_set_args(L, "i", Waypoint_lists.size());
}

//****SUBLIBRARY: Mission/Weapons
ade_lib l_Mission_Weapons("Weapons", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Weapons, "number Index", "Gets handle to a weapon object in the mission.", "weapon", "Weapon handle, or invalid weapon handle if index is invalid")
{
	int idx;
	if(!ade_get_args(L, "*i", &idx))
		return ade_set_error(L, "o", l_Weapon.Set(object_h()));

	//Remember, Lua indices start at 0.
	int count=1;

	for(int i = 0; i < MAX_WEAPONS; i++)
	{
		if (Weapons[i].weapon_info_index < 0 || Weapons[i].objnum < 0 || Objects[Weapons[i].objnum].type != OBJ_WEAPON)
			continue;

		if(count == idx) {
			return ade_set_args(L, "o", l_Weapon.Set(object_h(&Objects[Weapons[i].objnum])));
		}

		count++;
	}

	return ade_set_error(L, "o", l_Weapon.Set(object_h()));
}
ADE_FUNC(__len, l_Mission_Weapons, NULL, "Number of weapon objects in mission. Note that this is only accurate for one frame.", "number", "Number of weapon objects in mission")
{
	return ade_set_args(L, "i", Num_weapons);
}

//****SUBLIBRARY: Mission/Beams
ade_lib l_Mission_Beams("Beams", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Beams, "number Index", "Gets handle to a beam object in the mission.", "beam", "Beam handle, or invalid beam handle if index is invalid")
{
	int idx;
	if(!ade_get_args(L, "*i", &idx))
		return ade_set_error(L, "o", l_Beam.Set(object_h()));

	//Remember, Lua indices start at 0.
	int count=1;

	for(int i = 0; i < MAX_BEAMS; i++)
	{
		if (Beams[i].weapon_info_index < 0 || Beams[i].objnum < 0 || Objects[Beams[i].objnum].type != OBJ_BEAM)
			continue;

		if(count == idx) {
			return ade_set_args(L, "o", l_Beam.Set(object_h(&Objects[Beams[i].objnum])));
		}

		count++;
	}

	return ade_set_error(L, "o", l_Beam.Set(object_h()));
}
ADE_FUNC(__len, l_Mission_Beams, NULL, "Number of beam objects in mission. Note that this is only accurate for one frame.", "number", "Number of beam objects in mission")
{
	return ade_set_args(L, "i", Beam_count);
}

//****SUBLIBRARY: Campaign
ade_lib l_Campaign("Campaign", NULL, "ca", "Campaign Library");

ADE_FUNC(getNextMissionFilename, l_Campaign, NULL, "Gets next mission filename", "string", "Next mission filename, or nil if the next mission is invalid")
{
	if (Campaign.next_mission < 0 || Campaign.next_mission >= MAX_CAMPAIGN_MISSIONS) {
		return ADE_RETURN_NIL;
	}
	return ade_set_args(L, "s", Campaign.missions[Campaign.next_mission].name);
}

ADE_FUNC(getPrevMissionFilename, l_Campaign, NULL, "Gets previous mission filename", "string", "Previous mission filename, or nil if the previous mission is invalid")
{
	if (Campaign.prev_mission < 0 || Campaign.prev_mission >= MAX_CAMPAIGN_MISSIONS) {
		return ADE_RETURN_NIL;
	}
	return ade_set_args(L, "s", Campaign.missions[Campaign.prev_mission].name);
}

// DahBlount - This jumps to a mission, the reason it accepts a boolean value is so that players can return to campaign maps
ADE_FUNC(jumpToMission, l_Campaign, "string filename, [boolean hub]", "Jumps to a mission based on the filename. Optionally, the player can be sent to a hub mission without setting missions to skipped.", "boolean", "Jumps to a mission, or returns nil.")
{
	char *filename = NULL;;
	bool hub = false;
	if (!ade_get_args(L, "s|b", &filename, &hub))
		return ADE_RETURN_NIL;

	mission_campaign_jump_to_mission(filename, hub);

	return ADE_RETURN_TRUE;
}

// TODO: add a proper indexer type that returns a handle
// something like ca.Mission[filename/index]

//****SUBLIBRARY: Mission/Wings
ade_lib l_Mission_Wings("Wings", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Wings, "number Index/string WingName", "Wings in the mission", "wing", "Wing handle, or invalid wing handle if index or name was invalid")
{
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_Wing.Set(-1));

	//MageKing17 - Make the count-ignoring version of the lookup and leave checking if the wing has any ships to the scripter
	int idx = wing_lookup(name);
	
	if(idx < 0)
	{
		idx = atoi(name);
		if(idx < 1 || idx > Num_wings)
			return ade_set_error(L, "o", l_Wing.Set(-1));

		idx--;	//Lua->FS2
	}

	return ade_set_args(L, "o", l_Wing.Set(idx));
}

ADE_FUNC(__len, l_Mission_Wings, NULL, "Number of wings in mission", "number", "Number of wings in mission")
{
	return ade_set_args(L, "i", Num_wings);
}

//****SUBLIBRARY: Mission/Teams
ade_lib l_Mission_Teams("Teams", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Teams, "number Index/string TeamName", "Teams in the mission", "team", "Team handle or invalid team handle if the requested team could not be found")
{
	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_Team.Set(-1));

	int idx = iff_lookup(name);
	
	if(idx < 0)
	{
		idx = atoi(name);

		idx--;	//Lua->FS2
	}

	if(idx < 0 || idx >= Num_iffs)
		return ade_set_error(L, "o", l_Team.Set(-1));

	return ade_set_args(L, "o", l_Team.Set(idx));
}

ADE_FUNC(__len, l_Mission_Teams, NULL, "Number of teams in mission", "number", "Number of teams in mission")
{
	return ade_set_args(L, "i", Num_iffs);
}

//****SUBLIBRARY: Mission/Messages
ade_lib l_Mission_Messages("Messages", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Messages, "number Index/string messageName", "Messages of the mission", "message", "Message handle or invalid handle on error")
{
	int idx = -1;

	if (lua_isnumber(L, 2))
	{
		if (!ade_get_args(L, "*i", &idx))
			return ade_set_args(L, "o", l_Message.Set(-1));

		idx--; // Lua --> FS2

		idx += Num_builtin_messages;
	}
	else
	{
		char* name = NULL;

		if (!ade_get_args(L, "*s", &name))
			return ade_set_args(L, "o", l_Message.Set(-1));

		if (name == NULL)
			return ade_set_args(L, "o", l_Message.Set(-1));

		for (int i = Num_builtin_messages; i < (int) Messages.size(); i++)
		{
			if (!stricmp(Messages[i].name, name))
			{
				idx = i;
				break;
			}
		}
	}

	if (idx < Num_builtin_messages || idx >= (int) Messages.size())
		return ade_set_args(L, "o", l_Message.Set(-1));
	else
		return ade_set_args(L, "o", l_Message.Set(idx));
}

ADE_FUNC(__len, l_Mission_Messages, NULL, "Number of messages in the mission", "number", "Number of messages in mission")
{
	return ade_set_args(L, "i", (int) Messages.size() - Num_builtin_messages);
}

//****SUBLIBRARY: Mission/BuiltinMessages
ade_lib l_Mission_BuiltinMessages("BuiltinMessages", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_BuiltinMessages, "number Index/string messageName", "Built-in messages of the mission", "message", "Message handle or invalid handle on error")
{
	int idx = -1;

	if (lua_isnumber(L, 2))
	{
		if (!ade_get_args(L, "*i", &idx))
			return ade_set_args(L, "o", l_Message.Set(-1));

		idx--; // Lua --> FS2
	}
	else
	{
		char* name = NULL;

		if (!ade_get_args(L, "*s", &name))
			return ade_set_args(L, "o", l_Message.Set(-1));

		if (name == NULL)
			return ade_set_args(L, "o", l_Message.Set(-1));

		for (int i = 0; i < Num_builtin_messages; i++)
		{
			if (!stricmp(Messages[i].name, name))
			{
				idx = i;
				break;
			}
		}
	}

	if (idx < 0 || idx >= Num_builtin_messages)
		return ade_set_args(L, "o", l_Message.Set(-1));
	else
		return ade_set_args(L, "o", l_Message.Set(idx));
}

ADE_FUNC(__len, l_Mission_BuiltinMessages, NULL, "Number of built-in messages in the mission", "number", "Number of messages in mission")
{
	return ade_set_args(L, "i", Num_builtin_messages);
}

//****SUBLIBRARY: Mission/Personas
ade_lib l_Mission_Personas("Personas", &l_Mission, NULL, NULL);

ADE_INDEXER(l_Mission_Personas, "number Index/string name", "Personas of the mission", "persona", "Persona handle or invalid handle on error")
{
	int idx = -1;

	if (lua_isnumber(L, 2))
	{
		if (!ade_get_args(L, "*i", &idx))
			return ade_set_args(L, "o", l_Persona.Set(-1));

		idx--; // Lua --> FS2
	}
	else
	{
		char* name = NULL;

		if (!ade_get_args(L, "*s", &name))
			return ade_set_args(L, "o", l_Persona.Set(-1));

		if (name == NULL)
			return ade_set_args(L, "o", l_Persona.Set(-1));

		idx = message_persona_name_lookup(name);
	}

	if (idx < 0 || idx >= Num_personas)
		return ade_set_args(L, "o", l_Persona.Set(-1));
	else
		return ade_set_args(L, "o", l_Persona.Set(idx));
}

ADE_FUNC(__len, l_Mission_Personas, NULL, "Number of personas in the mission", "number", "Number of messages in mission")
{
	return ade_set_args(L, "i", Num_personas);
}

ADE_FUNC(addMessage, l_Mission, "string name, string text[, persona persona]", "Adds a message", "message", "The new message or invalid handle on error")
{
	char* name = NULL;
	char* text = NULL;
	int personaIdx = -1;

	if (!ade_get_args(L, "ss|o", &name, &text, l_Persona.Get(&personaIdx)))
		return ade_set_error(L, "o", l_Message.Set(-1));

	if (name == NULL || text == NULL)
		return ade_set_error(L, "o", l_Message.Set(-1));

	if (personaIdx < 0 || personaIdx >= Num_personas)
		personaIdx = -1;

	add_message(name, text, personaIdx, 0);

	return ade_set_error(L, "o", l_Message.Set((int) Messages.size() - 1));
}

ADE_FUNC(sendMessage, l_Mission, "string sender, message message[, number delay=0.0[, enumeration priority = MESSAGE_PRIORITY_NORMAL[, boolean fromCommand = false]]]",
		 "Sends a message from the given source (not from a ship!) with the given priority or optionally sends it from the missions command source.<br>"
		 "If delay is specified the message will be delayed by the specified time in seconds<br>"
		 "If you pass <i>nil</i> as the sender then the message will not have a sender.",
		 "boolean", "true if successfull, false otherwise")
{
	char* sender = NULL;
	int messageIdx = -1;
	int priority = MESSAGE_PRIORITY_NORMAL;
	bool fromCommand = false;
	int messageSource = MESSAGE_SOURCE_SPECIAL;
	float delay = 0.0f;

	enum_h* ehp = NULL;

	// if first is nil then use no source
	if (lua_isnil(L, 1))
	{
		if (!ade_get_args(L, "*o|fob", l_Message.Get(&messageIdx), &delay, l_Enum.GetPtr(&ehp), &fromCommand))
			return ADE_RETURN_FALSE;

		messageSource = MESSAGE_SOURCE_NONE;
	}
	else
	{
		if (!ade_get_args(L, "so|fob", &sender, l_Message.Get(&messageIdx), &delay, l_Enum.GetPtr(&ehp), &fromCommand))
			return ADE_RETURN_FALSE;

		if (sender == NULL)
			return ADE_RETURN_FALSE;
	}

	if (fromCommand)
		messageSource = MESSAGE_SOURCE_COMMAND;

	if (messageIdx < 0 || messageIdx >= (int) Messages.size())
		return ADE_RETURN_FALSE;

	if (messageIdx < Num_builtin_messages)
	{
		LuaError(L, "Cannot send built-in messages!");
		return ADE_RETURN_FALSE;
	}

	if (delay < 0.0f)
	{
		LuaError(L, "Invalid negative delay of %f!", delay);
		return ADE_RETURN_FALSE;
	}

	if (ehp != NULL)
	{
		switch(ehp->index)
		{
		case LE_MESSAGE_PRIORITY_HIGH:
			priority = MESSAGE_PRIORITY_HIGH;
			break;
		case LE_MESSAGE_PRIORITY_NORMAL:
			priority = MESSAGE_PRIORITY_NORMAL;
			break;
		case LE_MESSAGE_PRIORITY_LOW:
			priority = MESSAGE_PRIORITY_LOW;
			break;
		default:
			LuaError(L, "Invalid enumeration used! Must be one of MESSAGE_PRIORITY_*.");
			return ADE_RETURN_FALSE;
		}
	}

	if (messageSource == MESSAGE_SOURCE_NONE)
		message_send_unique_to_player(Messages[messageIdx].name, NULL, MESSAGE_SOURCE_NONE, priority, 0, fl2i(delay * 1000.0f));
	else
		message_send_unique_to_player(Messages[messageIdx].name, (void*) sender, messageSource, priority, 0, fl2i(delay * 1000.0f));

	return ADE_RETURN_TRUE;
}

ADE_FUNC(sendTrainingMessage, l_Mission, "message message, number time[, number delay=0.0]",
		 "Sends a training message to the player. <i>time</i> is the amount in seconds to display the message, only whole seconds are used!",
		 "boolean", "true if successfull, false otherwise")
{
	int messageIdx = -1;
	float delay = 0.0f;
	int time = -1;

	if (!ade_get_args(L, "oi|f", l_Message.Get(&messageIdx), &time, &delay))
		return ADE_RETURN_FALSE;

	if (messageIdx < 0 || messageIdx >= (int) Messages.size())
		return ADE_RETURN_FALSE;

	if (delay < 0.0f)
	{
		LuaError(L, "Got invalid delay of %f seconds!", delay);
		return ADE_RETURN_FALSE;
	}

	if (time < 0)
	{
		LuaError(L, "Got invalid time of %d seconds!", time);
		return ADE_RETURN_FALSE;
	}

	message_training_queue(Messages[messageIdx].name, timestamp(fl2i(delay * 1000.0f)), time);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(createShip, l_Mission, "[string Name, shipclass Class=Shipclass[1], orientation Orientation=null, vector Position={0,0,0}]", "Creates a ship and returns a handle to it using the specified name, class, world orientation, and world position", "ship", "Ship handle, or invalid ship handle if ship couldn't be created")
{
	char *name = NULL;
	int sclass = -1;
	matrix_h *orient = NULL;
	vec3d pos = vmd_zero_vector;
	ade_get_args(L, "|sooo", &name, l_Shipclass.Get(&sclass), l_Matrix.GetPtr(&orient), l_Vector.Get(&pos));

	matrix *real_orient = &vmd_identity_matrix;
	if(orient != NULL)
	{
		real_orient = orient->GetMatrix();
	}
	
	int obj_idx = ship_create(real_orient, &pos, sclass, name);

	if(obj_idx > -1) {
		model_page_in_textures(Ship_info[sclass].model_num, sclass);

		return ade_set_args(L, "o", l_Ship.Set(object_h(&Objects[obj_idx]), Objects[obj_idx].signature));
	} else
		return ade_set_error(L, "o", l_Ship.Set(object_h()));
}

ADE_FUNC(createWaypoint, l_Mission, "[vector Position, waypointlist List]",
		 "Creates a waypoint",
		 "waypoint",
		 "Waypoint handle, or invalid waypoint handle if waypoint couldn't be created")
{
	vec3d *v3 = NULL;
	waypointlist_h *wlh = NULL;
	if(!ade_get_args(L, "|oo", l_Vector.GetPtr(&v3), l_WaypointList.GetPtr(&wlh)))
		return ade_set_error(L, "o", l_Waypoint.Set(object_h()));

	// determine where we need to create it - it looks like we were given a waypoint list but not a waypoint itself
	int waypoint_instance = -1;
	if (wlh && wlh->IsValid())
	{
		int wp_list_index = find_index_of_waypoint_list(wlh->wlp);
		int wp_index = (int) wlh->wlp->get_waypoints().size() - 1;
		waypoint_instance = calc_waypoint_instance(wp_list_index, wp_index);
	}
	int obj_idx = waypoint_add(v3 != NULL ? v3 : &vmd_zero_vector, waypoint_instance);

	if(obj_idx >= 0)
		return ade_set_args(L, "o", l_Waypoint.Set(object_h(&Objects[obj_idx])));
	else
		return ade_set_args(L, "o", l_Waypoint.Set(object_h()));
}

ADE_FUNC(createWeapon, l_Mission, "[weaponclass Class=WeaponClass[1], orientation Orientation=null, world vector Position={0,0,0}, object Parent = nil, number Group = -1",
		 "Creates a weapon and returns a handle to it. 'Group' is used for lighting grouping purposes;"
		 " for example, quad lasers would only need to act as one light source.",
		 "weapon",
		 "Weapon handle, or invalid weapon handle if weapon couldn't be created.")
{
	int wclass = -1;
	object_h *parent = NULL;
	int group = -1;
	matrix_h *orient = NULL;
	vec3d pos = vmd_zero_vector;
	ade_get_args(L, "|ooooi", l_Weaponclass.Get(&wclass), l_Matrix.GetPtr(&orient), l_Vector.Get(&pos), l_Object.GetPtr(&parent), &group);

	matrix *real_orient = &vmd_identity_matrix;
	if(orient != NULL)
	{
		real_orient = orient->GetMatrix();
	}

	int parent_idx = (parent && parent->IsValid()) ? OBJ_INDEX(parent->objp) : -1;

	int obj_idx = weapon_create(&pos, real_orient, wclass, parent_idx, group);

	if(obj_idx > -1)
		return ade_set_args(L, "o", l_Weapon.Set(object_h(&Objects[obj_idx]), Objects[obj_idx].signature));
	else
		return ade_set_error(L, "o", l_Weapon.Set(object_h()));
}

ADE_FUNC(getMissionFilename, l_Mission, NULL, "Gets mission filename", "string", "Mission filename, or empty string if game is not in a mission")
{
	return ade_set_args(L, "s", Game_current_mission_filename);
}

ADE_FUNC(startMission, l_Mission, "[Filename or MISSION_* enumeration, Briefing = true]", "Starts the defined mission", "boolean", "True, or false if the function fails")
{
	bool b = true;
	char s[MAX_FILENAME_LEN];
	char *str = s;

	if(lua_isstring(L, 1))
	{
		if (!ade_get_args(L, "s|b", &str, &b))
			return ade_set_args(L, "b", false);

	} else {
		enum_h *e = NULL;

		if (!ade_get_args(L, "o|b", l_Enum.GetPtr(&e), &b))
			return ade_set_args(L, "b", false);

		if (e->index == LE_MISSION_REPEAT) {
			if (Num_recent_missions > 0)  {
				strncpy( s, Recent_missions[0], MAX_FILENAME_LEN );
			} else {
				return ade_set_args(L, "b", false);
			}
		} else {
			return ade_set_args(L, "b", false);
		}
	}

	// no filename... bail
	if (str == NULL)
		return ade_set_args(L, "b", false);

	// if mission name has extension... it needs to be removed...
	char *file_ext;

	file_ext = strrchr(str, '.');
	if (file_ext)
		*file_ext = 0;

	// game is in MP mode... or if the file does not exist... bail
	if ((Game_mode & GM_MULTIPLAYER) || (cf_exists_full(str, CF_TYPE_MISSIONS) != 0))
		return ade_set_args(L, "b", false);

	// mission is already running...
	if (Game_mode & GM_IN_MISSION) {
		// TO DO... All the things needed if this function is called in any state of the game while mission is running.
		//    most likely all require 'stricmp(str, Game_current_mission_filename)' to make sure missions arent mixed
		//    but after that it might be possible to imprement method for jumping directly into already running 
		//    missions.
		return ade_set_args(L, "b", false);
	// if mission is not running
	} else {
		// due safety checks of the game_start_mission() function allow only main menu for now.
		if (gameseq_get_state(gameseq_get_depth()) == GS_STATE_MAIN_MENU) {
			strcpy_s( Game_current_mission_filename, str );
			if (b == true) {
				// start mission - go via briefing screen
				gameseq_post_event(GS_EVENT_START_GAME);
			} else {
				// start mission - enter the game directly
				gameseq_post_event(GS_EVENT_START_GAME_QUICK);
			}
			return ade_set_args(L, "b", true);
		}
	}
	return ade_set_args(L, "b", false);
}

ADE_FUNC(getMissionTime, l_Mission, NULL, "Game time in seconds since the mission was started; is affected by time compression", "number", "Mission time (seconds), or 0 if game is not in a mission")
{
	if(!(Game_mode & GM_IN_MISSION))
		return ade_set_error(L, "f", 0.0f);

	return ade_set_args(L, "x", Missiontime);
}

//WMC - These are in freespace.cpp
ADE_FUNC(loadMission, l_Mission, "Mission name", "Loads a mission", "boolean", "True if mission was loaded, otherwise false")
{
	char *s;
	if(!ade_get_args(L, "s", &s))
		return ade_set_error(L, "b", false);

	// clear post processing settings
	gr_post_process_set_defaults();

	//NOW do the loading stuff
	game_stop_time();
	get_mission_info(s, &The_mission, false);
	game_level_init();

	if(mission_load(s) == -1)
		return ADE_RETURN_FALSE;

	game_post_level_init();

	Game_mode |= GM_IN_MISSION;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(unloadMission, l_Mission, NULL, "Stops the current mission and unloads it", NULL, NULL)
{
	if(Game_mode & GM_IN_MISSION)
	{
		game_level_close();
		Game_mode &= ~GM_IN_MISSION;
		strcpy_s(Game_current_mission_filename, "");
	}

	return ADE_RETURN_NIL;
}

ADE_FUNC(simulateFrame, l_Mission, NULL, "Simulates mission frame", NULL, NULL)
{
	game_update_missiontime();
	game_simulation_frame();

	return ADE_RETURN_TRUE;
}

ADE_FUNC(renderFrame, l_Mission, NULL, "Renders mission frame, but does not move anything", NULL, NULL)
{
	camid cid = game_render_frame_setup();
	game_render_frame( cid );
	game_render_post_frame();

	return ADE_RETURN_TRUE;
}

ADE_FUNC(applyShudder, l_Mission, "number time, number intesity", "Applies a shudder effects to the camera. Time is in seconds. Intensity specifies the shudder effect strength, the Maxim has a value of 1440.", "boolean", "true if successfull, false otherwise")
{
	float time = -1.0f;
	float intensity = -1.0f;

	if (!ade_get_args(L, "ff", &time, &intensity))
		return ADE_RETURN_FALSE;

	if (time < 0.0f || intensity < 0.0f)
	{
		LuaError(L, "Illegal shudder values given. Must be bigger than zero, got time of %f and intensity of %f.", time, intensity);
		return ADE_RETURN_FALSE;
	}

	int int_time = fl2i(time * 1000.0f);

	game_shudder_apply(int_time, intensity * 0.01f);

	return ADE_RETURN_TRUE;
}

ADE_FUNC(isInCampaign, l_Mission, NULL, "Get whether or not the current mission being played in a campaign (as opposed to the tech room's simulator)", "boolean", "true if in campaign, false if not")
{
	bool b = false;

	if (Game_mode & GM_CAMPAIGN_MODE) {
		b = true;
	}

	return ade_set_args(L, "b", b);
}

//**********LIBRARY: Tables
ade_lib l_Tables("Tables", NULL, "tb", "Tables library");

//*****SUBLIBRARY: Tables/ShipClasses
ade_lib l_Tables_ShipClasses("ShipClasses", &l_Tables, NULL, NULL);
ADE_INDEXER(l_Tables_ShipClasses, "number Index/string Name", "Array of ship classes", "shipclass", "Ship handle, or invalid ship handle if index is invalid")
{
	if(!ships_inited)
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	char *name;
	if(!ade_get_args(L, "*s", &name))
		return ade_set_error(L, "o", l_Shipclass.Set(-1));

	int idx = ship_info_lookup(name);
	
	if(idx < 0) {
		idx = atoi(name);
		if(idx < 1 || idx >= static_cast<int>(Ship_info.size()))
			return ade_set_error(L, "o", l_Shipclass.Set(-1));

		idx--;	//Lua->FS2
	}

	return ade_set_args(L, "o", l_Shipclass.Set(idx));
}

ADE_FUNC(__len, l_Tables_ShipClasses, NULL, "Number of ship classes", "number", "Number of ship classes, or 0 if ship classes haven't been loaded yet")
{
	if(!ships_inited)
		return ade_set_args(L, "i", 0);	//No ships loaded...should be 0

	return ade_set_args(L, "i", Ship_info.size());
}

//*****SUBLIBRARY: Tables/WeaponClasses
ade_lib l_Tables_WeaponClasses("WeaponClasses", &l_Tables, NULL, NULL);

extern int Weapons_inited;

ADE_INDEXER(l_Tables_WeaponClasses, "number Index/string WeaponName", "Array of weapon classes", "weapon", "Weapon class handle, or invalid weaponclass handle if index is invalid")
{
	if(!Weapons_inited)
		return ade_set_error(L, "o", l_Weaponclass.Set(-1));

	char *name;
	if(!ade_get_args(L, "*s", &name))
		return 0;

	int idx = weapon_info_lookup(name);
	
	if(idx < 0) {
		idx = atoi(name);

		// atoi is good enough here, 0 is invalid anyway
		if (idx > 0)
		{
			idx--; // Lua --> C/C++
		}
		else
		{
			return ade_set_args(L, "o", l_Weaponclass.Set(-1));
		}
	}

	return ade_set_args(L, "o", l_Weaponclass.Set(idx));
}

ADE_FUNC(__len, l_Tables_WeaponClasses, NULL, "Number of weapon classes", "number", "Number of weapon classes, or 0 if weapon classes haven't been loaded yet")
{
	if(!Weapons_inited)
		return ade_set_args(L, "i", 0);

	return ade_set_args(L, "i", Num_weapon_types);
}

//*************************Testing stuff*************************
//This section is for stuff that's considered experimental.
ade_lib l_Testing("Testing", NULL, "ts", "Experimental or testing stuff");

ADE_FUNC(avdTest, l_Testing, NULL, "Test the AVD Physics code", NULL, NULL)
{
	static bool initialized = false;
	static avd_movement avd;

	if(!initialized)
	{
		avd.setAVD(10.0f, 3.0f, 1.0f, 1.0f, 0.0f);
		initialized = true;
	}
	for(int i = 0; i < 3000; i++)
	{
		float Pc, Vc;
		avd.get((float)i/1000.0f, &Pc, &Vc);
		gr_set_color(0, 255, 0);
		gr_pixel(i/10, gr_screen.clip_bottom - (int)(Pc*10.0f), GR_RESIZE_NONE);
		gr_set_color(255, 0, 0);
		gr_pixel(i/10, gr_screen.clip_bottom - (int)(Vc*10.0f), GR_RESIZE_NONE);

		avd.get(&Pc, &Vc);
		gr_set_color(255, 255, 255);
		gr_pixel((timestamp()%3000)/10, gr_screen.clip_bottom - (int)(Pc*10.0f), GR_RESIZE_NONE);
		gr_pixel((timestamp()%3000)/10, gr_screen.clip_bottom - (int)(Vc*10.0f), GR_RESIZE_NONE);
	}

	return ADE_RETURN_NIL;
}

ADE_FUNC(createParticle, l_Testing, "vector Position, vector Velocity, number Lifetime, number Radius, enumeration Type, [number Tracer length=-1, boolean Reverse=false, texture Texture=Nil, object Attached Object=Nil]",
		 "Creates a particle. Use PARTICLE_* enumerations for type."
		 "Reverse reverse animation, if one is specified"
		 "Attached object specifies object that Position will be (and always be) relative to.",
		 "particle",
		 "Handle to the created particle")
{
	particle::particle_info pi;
	pi.type = particle::PARTICLE_DEBUG;
	pi.optional_data = -1;
	pi.attached_objnum = -1;
	pi.attached_sig = -1;
	pi.reverse = 0;

	// Need to consume tracer_length parameter but it isn't used anymore
	float temp;

	enum_h *type = NULL;
	bool rev=false;
	object_h *objh=NULL;
	if(!ade_get_args(L, "ooffo|fboo", l_Vector.Get(&pi.pos), l_Vector.Get(&pi.vel), &pi.lifetime, &pi.rad, l_Enum.GetPtr(&type), &temp, &rev, l_Texture.Get((int*)&pi.optional_data), l_Object.GetPtr(&objh)))
		return ADE_RETURN_NIL;

	if(type != NULL)
	{
		switch(type->index)
		{
			case LE_PARTICLE_DEBUG:
				pi.type = particle::PARTICLE_DEBUG;
				break;
			case LE_PARTICLE_FIRE:
				pi.type = particle::PARTICLE_FIRE;
				break;
			case LE_PARTICLE_SMOKE:
				pi.type = particle::PARTICLE_SMOKE;
				break;
			case LE_PARTICLE_SMOKE2:
				pi.type = particle::PARTICLE_SMOKE2;
				break;
			case LE_PARTICLE_BITMAP:
				if (pi.optional_data < 0)
				{
					LuaError(L, "Invalid texture specified for createParticle()!");
				}

				pi.type = particle::PARTICLE_BITMAP;
				break;
		}
	}

	if(rev)
		pi.reverse = 0;

	if(objh != NULL && objh->IsValid())
	{
		pi.attached_objnum = OBJ_INDEX(objh->objp);
		pi.attached_sig = objh->objp->signature;
	}

    particle::WeakParticlePtr p = particle::create(&pi);

	if (!p.expired())
		return ade_set_args(L, "o", l_Particle.Set(particle_h(p)));
	else
		return ADE_RETURN_NIL;
}

ADE_FUNC(getStack, l_Testing, NULL, "Generates an ADE stackdump", "string", "Current Lua stack")
{
	char buf[10240] = {'\0'};
	ade_stackdump(L, buf);
	return ade_set_args(L, "s", buf);
}

ADE_FUNC(isCurrentPlayerMulti, l_Testing, NULL, "Returns whether current player is a multiplayer pilot or not.", "boolean", "Whether current player is a multiplayer pilot or not")
{
	if(Player == NULL)
		return ade_set_error(L, "b", false);

	if(!(Player->flags & PLAYER_FLAGS_IS_MULTI))
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

// Om_tracker_flag should already be set in FreeSpace.cpp, needed to determine if PXO is enabled from the registry
extern int Om_tracker_flag; // needed for FS2OpenPXO config

ADE_FUNC(isPXOEnabled, l_Testing, NULL, "Returns whether PXO is currently enabled in the configuration.", "boolean", "Whether PXO is enabled or not")
{
	if(!(Om_tracker_flag))
		return ADE_RETURN_FALSE;

	return ADE_RETURN_TRUE;
}

ADE_FUNC(playCutscene, l_Testing, NULL, "Forces a cutscene by the specified filename string to play. Should really only be used in a non-gameplay state (i.e. start of GS_STATE_BRIEFING) otherwise odd side effects may occur. Highly Experimental.", "string", NULL)
{
	//This whole thing is a quick hack and can probably be done way better, but is currently functioning fine for my purposes.
	char *filename;

	if (!ade_get_args(L, "s", &filename))
		return ADE_RETURN_FALSE;

	movie::play(filename);

	return ADE_RETURN_TRUE;
}

// *************************Helper functions*********************
//WMC - This should be used anywhere that an 'object' is set, so
//that scripters can get access to as much relevant data to that
//object as possible.
//It should also be updated as new types are added to Lua.
int ade_set_object_with_breed(lua_State *L, int obj_idx)
{
	if(obj_idx < 0 || obj_idx >= MAX_OBJECTS)
		return ade_set_error(L, "o", l_Object.Set(object_h()));

	object *objp = &Objects[obj_idx];

	switch(objp->type)
	{
		case OBJ_SHIP:
			return ade_set_args(L, "o", l_Ship.Set(object_h(objp)));
		case OBJ_ASTEROID:
			return ade_set_args(L, "o", l_Asteroid.Set(object_h(objp)));
		case OBJ_DEBRIS:
			return ade_set_args(L, "o", l_Debris.Set(object_h(objp)));
		case OBJ_WAYPOINT:
			return ade_set_args(L, "o", l_Waypoint.Set(object_h(objp)));
		case OBJ_WEAPON:
			return ade_set_args(L, "o", l_Weapon.Set(object_h(objp)));
		case OBJ_BEAM:
			return ade_set_args(L, "o", l_Beam.Set(object_h(objp)));
		default:
			return ade_set_args(L, "o", l_Object.Set(object_h(objp)));
	}
}

//###########################################################
//########################<IMPORTANT>########################
//###########################################################
//If you are a coder who wants to add libraries, functions,
//or objects to Lua, then you want to be above this point.
//###########################################################
//########################</IMPORTANT>#######################
//###########################################################

// *************************Housekeeping*************************
//WMC - The miraculous lines of code that make Lua debugging worth something.
lua_Debug Ade_debug_info;
char debug_stack[4][32];

void ade_debug_ret(lua_State *L, lua_Debug *ar)
{
	Assert(L != NULL);
	Assert(ar != NULL);
	lua_getstack(L, 1, ar);
	lua_getinfo(L, "nSlu", ar);
	memcpy(&Ade_debug_info, ar, sizeof(lua_Debug));

	int n;
	for (n = 0; n < 4; n++) {
		debug_stack[n][0] = '\0';
	}

	for (n = 0; n < 4; n++) {
		if (lua_getstack(L,n+1, ar) == 0)
			break;
		lua_getinfo(L,"n", ar);
		if (ar->name == NULL)
			break;
		strcpy_s(debug_stack[n],ar->name);
	}
}

//WMC - because the behavior of the return keyword
//was changed, I now have to use this in hooks.
static int ade_return_hack(lua_State *L)
{
	int i = 0;
	int num = lua_gettop(L);
	for(i = 0; i < num; i++)
	{
		lua_pushvalue(L, i+1);
	}

	return num;
}

static void *vm_lua_alloc(void*, void *ptr, size_t, size_t nsize) {
	if (nsize == 0)
	{
		vm_free(ptr);
		return NULL;
	}
	else
	{
		return vm_realloc(ptr, nsize);
	}
}

//Inits LUA
//Note that "libraries" must end with a {NULL, NULL}
//element
int script_state::CreateLuaState()
{
	mprintf(("LUA: Opening LUA state...\n"));
	lua_State *L = lua_newstate(vm_lua_alloc, nullptr);

	if(L == NULL)
	{
		Warning(LOCATION, "Could not initialize Lua");
		return 0;
	}

	//*****INITIALIZE AUXILIARY LIBRARIES
	mprintf(("LUA: Initializing base Lua libraries...\n"));
	luaL_openlibs(L);

	//*****DISABLE DANGEROUS COMMANDS
	lua_pushstring(L, "os");
	lua_rawget(L, LUA_GLOBALSINDEX);
	int os_ldx = lua_gettop(L);
	if(lua_istable(L, os_ldx))
	{
		lua_pushstring(L, "execute");
		lua_pushnil(L);
		lua_rawset(L, os_ldx);
		lua_pushstring(L, "remove");
		lua_pushnil(L);
		lua_rawset(L, os_ldx);
		lua_pushstring(L, "rename");
		lua_pushnil(L);
		lua_rawset(L, os_ldx);
	}
	lua_pop(L, 1);	//os table

	//*****SET DEBUG HOOKS
#ifndef NDEBUG
	lua_sethook(L, ade_debug_ret, LUA_MASKRET, 0);
#endif

	//*****INITIALIZE ADE
	uint i;
	mprintf(("LUA: Beginning ADE initialization\n"));
	for(i = 0; i < ade_manager::getInstance()->getNumEntries(); i++)
	{
		//WMC - Do only toplevel table entries, doi
		if(ade_manager::getInstance()->getEntry(i).ParentIdx == UINT_MAX)			//WMC - oh hey, we're done with the meaty point in < 10 lines.
			ade_manager::getInstance()->getEntry(i).SetTable(L, LUA_GLOBALSINDEX, LUA_GLOBALSINDEX);	//Oh the miracles of OOP.
	}

	//*****INITIALIZE RETURN HACK FUNCTION
	lua_pushstring(L, "ade_return_hack");
	lua_pushboolean(L, 0);
	lua_pushcclosure(L, ade_return_hack, 2);
	lua_setglobal(L, "ade_return_hack");

	//*****INITIALIZE ENUMERATION CONSTANTS
	mprintf(("ADE: Initializing enumeration constants...\n"));
	enum_h eh;
	for(i = 0; i < Num_enumerations; i++)
	{
		eh.index = Enumerations[i].def;
		eh.is_constant = true;

		ade_set_args(L, "o", l_Enum.Set(eh));
		lua_setglobal(L, Enumerations[i].name);
	}

	//*****ASSIGN LUA SESSION
	mprintf(("ADE: Assigning Lua session...\n"));
	SetLuaSession(L);

//	(void)l_BitOps.GetName();

	return 1;
}

void script_state::EndLuaFrame()
{
	memcpy(NextDrawStringPos, NextDrawStringPosInitial, sizeof(NextDrawStringPos));
}

void ade_output_toc(FILE *fp, ade_table_entry *ate)
{
	Assert(fp != NULL);
	Assert(ate != NULL);

	//WMC - sanity checking
	if(ate->Name == NULL && ate->ShortName == NULL) {
		Warning(LOCATION, "Found ade_table_entry with no name or shortname");
		return;
	}

	fputs("<dd>", fp);

	if(ate->Name == NULL)
	{
		fprintf(fp, "<a href=\"#%s\">%s", ate->ShortName, ate->ShortName);
	}
	else
	{
		fprintf(fp, "<a href=\"#%s\">%s", ate->Name, ate->Name);
		if(ate->ShortName)
			fprintf(fp, " (%s)", ate->ShortName);
	}
	fputs("</a>", fp);

	if(ate->Description)
		fprintf(fp, " - %s\n", ate->Description);

	fputs("</dd>\n", fp);
}

static bool sort_table_entries(const ade_table_entry* left, const ade_table_entry* right) {
	const char* leftCmp = left->Name != nullptr ? left->Name : left->ShortName;
	const char* rightCmp = right->Name != nullptr ? right->Name : left->ShortName;

	if (leftCmp == nullptr) {
		return false;
	}
	if (rightCmp == nullptr) {
		return false;
	}

	SCP_string leftStr(leftCmp);
	std::transform(std::begin(leftStr), std::end(leftStr), std::begin(leftStr), ::tolower);

	SCP_string rightStr(rightCmp);
	std::transform(std::begin(rightStr), std::end(rightStr), std::begin(rightStr), ::tolower);

	return leftStr < rightStr;
}

static bool sort_doc_entries(const ade_table_entry* left, const ade_table_entry* right) {
	if (left->Instanced == right->Instanced) {
		// Same type -> compare as normal
		return sort_table_entries(left, right);
	}
	if (left->Instanced) {
		return true;
	}
	return false;
}

void script_state::OutputLuaMeta(FILE *fp)
{
	uint i;
	ade_table_entry *ate;
	fputs("<dl>\n", fp);

	SCP_vector<ade_table_entry*> table_entries;

	//***TOC: Libraries
	fputs("<dt><b>Libraries</b></dt>\n", fp);
	for(i = 0; i < ade_manager::getInstance()->getNumEntries(); i++)
	{
		ate = &ade_manager::getInstance()->getEntry(i);
		if(ate->ParentIdx == UINT_MAX && ate->Type == 'o' && ate->Instanced) {
			table_entries.push_back(ate);
		}
	}
	std::sort(std::begin(table_entries), std::end(table_entries), sort_table_entries);
	for (auto entry : table_entries) {
		ade_output_toc(fp, entry);
	}
	table_entries.clear();

	//***TOC: Objects
	fputs("<dt><b>Types</b></dt>\n", fp);
	for(i = 0; i < ade_manager::getInstance()->getNumEntries(); i++)
	{
		ate = &ade_manager::getInstance()->getEntry(i);
		if(ate->ParentIdx == UINT_MAX && ate->Type == 'o' && !ate->Instanced) {
			table_entries.push_back(ate);
		}
	}
	std::sort(std::begin(table_entries), std::end(table_entries), sort_table_entries);
	for (auto entry : table_entries) {
		ade_output_toc(fp, entry);
	}
	table_entries.clear();

	//***TOC: Enumerations
	fputs("<dt><b><a href=\"#Enumerations\">Enumerations</a></b></dt>", fp);

	//***End TOC
	fputs("</dl><br/><br/>", fp);

	//***Everything
	fputs("<dl>\n", fp);
	for(i = 0; i < ade_manager::getInstance()->getNumEntries(); i++)
	{
		ate = &ade_manager::getInstance()->getEntry(i);
		if(ate->ParentIdx == UINT_MAX)
			table_entries.push_back(ate);
	}

	std::sort(std::begin(table_entries), std::end(table_entries), sort_doc_entries);
	for (auto entry : table_entries) {
		entry->OutputMeta(fp);
	}
	table_entries.clear();

	//***Enumerations
	fprintf(fp, "<dt id=\"Enumerations\"><h2>Enumerations</h2></dt>");
	for(i = 0; i < Num_enumerations; i++)
	{
		//WMC - This is in case we ever want to add descriptions to enums.
		//fprintf(fp, "<dd><dl><dt><b>%s</b></dt><dd>%s</dd></dl></dd>", Enumerations[i].name, Enumerations[i].desc);

		//WMC - Otherwise, just use this.
		fprintf(fp, "<dd><b>%s</b></dd>", Enumerations[i].name);
	}
	fputs("</dl>\n", fp);

	//***End LUA
	fputs("</dl>\n", fp);
}

/*
 * Copyright (C) Volition, Inc. 1999.  All rights reserved.
 *
 * All source code herein is the property of Volition, Inc. You may not sell 
 * or otherwise commercially exploit the source or things you created based on the 
 * source.
 *
*/ 

#include <stdio.h>
#include <stdarg.h>

#include "cfile/cfile.h"
#include "controlconfig/controlsconfig.h"
#include "io/key.h"
#include "io/joy.h"
#include "localization/localize.h"
#include "parse/parselo.h"
#include "globalincs/systemvars.h"
#include "globalincs/def_files.h"

// z64: These enumerations MUST equal to those in controlsconfig.cpp...
// z64: Really need a better way than this.
enum CC_tab {
	TARGET_TAB			=0,
	SHIP_TAB			=1,
	WEAPON_TAB			=2,
	COMPUTER_TAB		=3
};

int Failed_key_index;

// assume control keys are used as modifiers until we find out 
int Shift_is_modifier;
int Ctrl_is_modifier;
int Alt_is_modifier;

int Axis_enabled[JOY_NUM_AXES] = { 1, 1, 1, 0, 0, 0 };
int Axis_enabled_defaults[JOY_NUM_AXES] = { 1, 1, 1, 0, 0, 0 };
int Invert_axis[JOY_NUM_AXES] = { 0, 0, 0, 0, 0, 0 };
int Invert_axis_defaults[JOY_NUM_AXES] = { 0, 0, 0, 0, 0, 0 };

//! arrays which hold the key mappings.  The array index represents a key-independent action.
//! please use SPACES for aligning the fields of this array
//XSTR:OFF
SCP_vector<config_item> Control_config;

static const size_t dummy = sizeof(Control_config);

char *Scan_code_text_german[] = {
	"",				"Esc",				"1",				"2",				"3",				"4",				"5",				"6",
	"7",				"8",				"9",				"0",				"Akzent '",				"Eszett",				"R\x81""cktaste",		"Tab",
	"Q",				"W",				"E",				"R",				"T",				"Z",				"U",				"I",
	"O",				"P",				"\x9A",				"+",				"Eingabe",			"Strg Links",			"A",				"S",

	"D",				"F",				"G",				"H",				"J",				"K",				"L",				"\x99",
	"\x8E",				"`",				"Shift",			"#",				"Y",				"X",				"C",				"V",
	"B",				"N",				"M",				",",				".",				"-",				"Shift",			"Num *",
	"Alt",				"Leertaste",			"Hochstell",			"F1",				"F2",				"F3",				"F4",				"F5",

	"F6",				"F7",				"F8",				"F9",				"F10",				"Pause",			"Rollen",			"Num 7",
	"Num 8",			"Num 9",			"Num -",			"Num 4",			"Num 5",			"Num 6",			"Num +",			"Num 1",
	"Num 2",			"Num 3",			"Num 0",			"Num ,",			"",				"",				"",				"F11",
	"F12",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"Num Eingabe",			"Strg Rechts",			"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"Num /",			"",				"Druck",
	"Alt",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"Num Lock",			"",				"Pos 1",
	"Pfeil Hoch",			"Bild Hoch",			"",				"Pfeil Links",			"",				"Pfeil Rechts",			"",				"Ende",
	"Pfeil Runter", 			"Bild Runter",			"Einfg",			"Entf",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
};

char *Joy_button_text_german[] = {
	"Knopf 1",		"Knopf 2",		"Knopf 3",		"Knopf 4",		"Knopf 5",		"Knopf 6",
	"Knopf 7",		"Knopf 8",		"Knopf 9",		"Knopf 10",		"Knopf 11",		"Knopf 12",
	"Knopf 13",		"Knopf 14",		"Knopf 15",		"Knopf 16",		"Knopf 17",		"Knopf 18",
	"Knopf 19",		"Knopf 20",		"Knopf 21",		"Knopf 22",		"Knopf 23",		"Knopf 24",
	"Knopf 25",		"Knopf 26",		"Knopf 27",		"Knopf 28",		"Knopf 29",		"Knopf 30",
	"Knopf 31",		"Knopf 32",		"Hut Hinten",	"Hut Vorne",	"Hut Links",	"Hut Rechts"
};

char *Scan_code_text_french[] = {
	"",				"\x90""chap",			"1",				"2",				"3",				"4",				"5",				"6",
	"7",				"8",				"9",				"0",				"-",				"=",				"Fl\x82""che Ret.",			"Tab",
	"Q",				"W",				"E",				"R",				"T",				"Y",				"U",				"I",
	"O",				"P",				"[",				"]",				"Entr\x82""e",			"Ctrl Gauche",			"A",				"S",

	"D",				"F",				"G",				"H",				"J",				"K",				"L",				";",
	"'",				"`",				"Maj.",			"\\",				"Z",				"X",				"C",				"V",
	"B",				"N",				"M",				",",				".",				"/",				"Maj.",			"Pav\x82 *",
	"Alt",				"Espace",			"Verr. Maj.",			"F1",				"F2",				"F3",				"F4",				"F5",

	"F6",				"F7",				"F8",				"F9",				"F10",				"Pause",			"Arret defil",		"Pav\x82 7",
	"Pav\x82 8",			"Pav\x82 9",			"Pav\x82 -",			"Pav\x82 4",			"Pav\x82 5",			"Pav\x82 6",			"Pav\x82 +",			"Pav\x82 1",
	"Pav\x82 2",			"Pav\x82 3",			"Pav\x82 0",			"Pav\x82 .",			"",				"",				"",				"F11",
	"F12",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"Pav\x82 Entr",			"Ctrl Droite",		"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"Pav\x82 /",			"",				"Impr \x82""cran",
	"Alt",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"Verr num",			"",				"Orig.",
	"Fl\x82""che Haut",			"Page Haut",			"",				"Fl\x82""che Gauche",			"",				"Fl\x82""che Droite",			"",			"Fin",
	"Fl\x82""che Bas", 			"Page Bas",			"Inser",			"Suppr",			"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
};

char *Joy_button_text_french[] = {
	"Bouton 1",		"Bouton 2",		"Bouton 3",		"Bouton 4",		"Bouton 5",		"Bouton 6",
	"Bouton 7",		"Bouton 8",		"Bouton 9",		"Bouton 10",		"Bouton 11",		"Bouton 12",
	"Bouton 13",		"Bouton 14",		"Bouton 15",		"Bouton 16",		"Bouton 17",		"Bouton 18",
	"Bouton 19",		"Bouton 20",		"Bouton 21",		"Bouton 22",		"Bouton 23",		"Bouton 24",
	"Bouton 25",		"Bouton 26",		"Bouton 27",		"Bouton 28",		"Bouton 29",		"Bouton 30",
	"Bouton 31",		"Bouton 32",		"Chapeau Arri\x8Are",		"Chapeau Avant",		"Chapeau Gauche",		"Chapeau Droite"
};

char *Scan_code_text_polish[] = {
	"",				"Esc",			"1",				"2",				"3",				"4",				"5",				"6",
	"7",				"8",				"9",				"0",				"-",				"=",				"Backspace",	"Tab",
	"Q",				"W",				"E",				"R",				"T",				"Y",				"U",				"I",
	"O",				"P",				"[",				"]",				"Enter",			"Lewy Ctrl",	"A",				"S",

	"D",				"F",				"G",				"H",				"J",				"K",				"L",				";",
	"'",				"`",				"LShift",			"\\",				"Z",				"X",				"C",				"V",
	"B",				"N",				"M",				",",				".",				"/",				"PShift",			"Num *",
	"Alt",			"Spacja",		"CapsLock",	"F1",				"F2",				"F3",				"F4",				"F5",

	"F6",				"F7",				"F8",				"F9",				"F10",			"Pause",			"Scrlock",	"Num 7",
	"Num 8",			"Num 9",			"Num -",			"Num 4",			"Num 5",			"Num 6",			"Num +",			"Num 1",
	"Num 2",			"Num 3",			"Num 0",			"Num .",			"",				"",				"",				"F11",
	"F12",			"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"Num Enter",	"Prawy Ctrl",	"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"Num /",			"",				"PrntScr",
	"Alt",			"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"Num Lock",		"",				"Home",
	"Kursor G\xF3ra",		"Page Up",		"",				"Kursor Lewo",	"",				"Kursor Prawo",	"",				"End",
	"Kursor D\xF3\xB3",  "Page Down",	"Insert",		"Delete",		"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
};

char *Joy_button_text_polish[] = {
	"Przyc.1",		"Przyc.2",		"Przyc.3",		"Przyc.4",		"Przyc.5",		"Przyc.6",
	"Przyc.7",		"Przyc.8",		"Przyc.9",		"Przyc.10",	"Przyc.11",	"Przyc.12",
	"Przyc.13",	"Przyc.14",	"Przyc.15",	"Przyc.16",	"Przyc.17",	"Przyc.18",
	"Przyc.19",	"Przyc.20",	"Przyc.21",	"Przyc.22",	"Przyc.23",	"Przyc.24",
	"Przyc.25",	"Przyc.26",	"Przyc.27",	"Przyc.28",	"Przyc.29",	"Przyc.30",
	"Przyc.31",	"Przyc.32",	"Hat Ty\xB3",		"Hat Prz\xF3\x64",	"Hat Lewo",		"Hat Prawo"
};

//!	This is the text that is displayed on the screen for the keys a player selects
char *Scan_code_text_english[] = {
	"",				"Esc",			"1",				"2",				"3",				"4",				"5",				"6",
	"7",				"8",				"9",				"0",				"-",				"=",				"Backspace",	"Tab",
	"Q",				"W",				"E",				"R",				"T",				"Y",				"U",				"I",
	"O",				"P",				"[",				"]",				"Enter",			"Left Ctrl",	"A",				"S",

	"D",				"F",				"G",				"H",				"J",				"K",				"L",				";",
	"'",				"`",				"Shift",			"\\",				"Z",				"X",				"C",				"V",
	"B",				"N",				"M",				",",				".",				"/",				"Shift",			"Pad *",
	"Alt",			"Spacebar",		"Caps Lock",	"F1",				"F2",				"F3",				"F4",				"F5",

	"F6",				"F7",				"F8",				"F9",				"F10",			"Pause",			"Scroll Lock",	"Pad 7",
	"Pad 8",			"Pad 9",			"Pad -",			"Pad 4",			"Pad 5",			"Pad 6",			"Pad +",			"Pad 1",
	"Pad 2",			"Pad 3",			"Pad 0",			"Pad .",			"",				"",				"",				"F11",
	"F12",			"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"Pad Enter",	"Right Ctrl",	"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"Pad /",			"",				"Print Scrn",
	"Alt",			"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"Num Lock",		"",				"Home",
	"Up Arrow",		"Page Up",		"",				"Left Arrow",	"",				"Right Arrow",	"",				"End",
	"Down Arrow",  "Page Down",	"Insert",		"Delete",		"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",

	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
	"",				"",				"",				"",				"",				"",				"",				"",
};

char *Joy_button_text_english[] = {
	"Button 1",		"Button 2",		"Button 3",		"Button 4",		"Button 5",		"Button 6",
	"Button 7",		"Button 8",		"Button 9",		"Button 10",	"Button 11",	"Button 12",
	"Button 13",	"Button 14",	"Button 15",	"Button 16",	"Button 17",	"Button 18",
	"Button 19",	"Button 20",	"Button 21",	"Button 22",	"Button 23",	"Button 24",
	"Button 25",	"Button 26",	"Button 27",	"Button 28",	"Button 29",	"Button 30",
	"Button 31",	"Button 32",	"Hat Back",		"Hat Forward",	"Hat Left",		"Hat Right"
};

char **Scan_code_text = Scan_code_text_english;
char **Joy_button_text = Joy_button_text_english;

SCP_list<Ccfg_vector> Control_config_presets;
SCP_vector<SCP_string> Control_config_preset_names;

void set_modifier_status()
{
	int i;

	Alt_is_modifier = 0;
	Shift_is_modifier = 0;
	Ctrl_is_modifier = 0;

	for (i=0; i<CCFG_MAX; i++) {
		if (Control_config[i].id[CON_KEYBOARD] < 0)
			continue;

		if (Control_config[i].id[CON_KEYBOARD] & KEY_ALTED)
			Alt_is_modifier = 1;

		if (Control_config[i].id[CON_KEYBOARD] & KEY_SHIFTED)
			Shift_is_modifier = 1;

		if (Control_config[i].id[CON_KEYBOARD] & KEY_CTRLED) {
			Assert(0);  // get Alan
			Ctrl_is_modifier = 1;
		}
	}
}

int translate_key_to_index(const char *key, bool find_override)
{
	int i, index = -1, alt = 0, shift = 0, max_scan_codes;

	max_scan_codes = sizeof(Scan_code_text_english) / sizeof(char *);

	// look for modifiers
	Assert(key);
	if (!strnicmp(key, "Alt", 3)) {
		alt = 1;
		key += 3;
		if (*key)
			key++;
	}

	if (!strnicmp(key, "Shift", 5)) {
		shift = 1;
		key += 5;
		if (*key)
			key++;
	}

	// look up index for default key
	if (*key) {
		for (i=0; i<max_scan_codes; i++)
			if (!stricmp(key, Scan_code_text_english[i])) {
				index = i;
				break;
			}

		if (i == max_scan_codes)
			return -1;

		if (shift)
			index |= KEY_SHIFTED;
		if (alt)
			index |= KEY_ALTED;

		// convert scancode to Control_config index
		if (find_override) {
			for (i=0; i<CCFG_MAX; i++) {
				if (!Control_config[i].disabled && Control_config[i].id[CON_KEYBOARD] == index) {
					index = i;
					break;
				}
			}
		} else {
			for (i=0; i<CCFG_MAX; i++) {
				if (!Control_config[i].disabled && Control_config[i].default[CON_KEYBOARD] == index) {
					index = i;
					break;
				}
			}
		}

		if (i == CCFG_MAX)
			return -1;

		return index;
	}

	return -1;
}

char *translate_key(char *key)
{
	int index = -1, key_code = -1, joy_code = -1;
	char *key_text = NULL;
	char *joy_text = NULL;

	static char text[40] = {"None"};

	index = translate_key_to_index(key, false);
	if (index < 0) {
		return NULL;
	}

	key_code = Control_config[index].id[CON_KEYBOARD];
	joy_code = Control_config[index].id[CON_JOY];

	Failed_key_index = index;

	if (key_code >= 0) {
		key_text = textify_scancode(key_code);
	}

	if (joy_code >= 0) {
		joy_text = Joy_button_text[joy_code];
	}

	// both key and joystick button are mapped to this control
	if ((key_code >= 0 ) && (joy_code >= 0) ) {
		strcpy_s(text, key_text);
		strcat_s(text, " or ");
		strcat_s(text, joy_text);
	}
	// if we only have one
	else if (key_code >= 0 ) {
		strcpy_s(text, key_text);
	}
	else if (joy_code >= 0) {
		strcpy_s(text, joy_text);
	}
	else {
		strcpy_s(text, "None");
	}

	return text;
}

char *textify_scancode(int code)
{
	static char text[40];

	if (code < 0)
		return "None";

	int keycode = code & KEY_MASK;

	*text = 0;
	if (code & KEY_ALTED && !(keycode == KEY_LALT || keycode == KEY_RALT)) {
		if(Lcl_gr){		
			strcat_s(text, "Alt-");
		} else if(Lcl_fr){		
			strcat_s(text, "Alt-");
		} else {		
			strcat_s(text, "Alt-");
		}		
	}

	if (code & KEY_SHIFTED && !(keycode == KEY_LSHIFT || keycode == KEY_RSHIFT)) {		
		if(Lcl_gr){
			strcat_s(text, "Shift-");
		} else if(Lcl_fr){		
			strcat_s(text, "Maj.-");
		} else {		
			strcat_s(text, "Shift-");
		}
	}

	strcat_s(text, Scan_code_text[keycode]);
	return text;
}
//XSTR:ON

void control_config_common_load_overrides();

// initialize common control config stuff - call at game startup after localization has been initialized
void control_config_common_init()
{
	config_item_builder Build(&Control_config);
	Build
		// targeting a ship
//  ( <---Keyboard-Default-Key(s)--->,  M,  J, "This is a control",                     Menu Tab,   CC_TYPE,         XSTR )
	(                           KEY_T, -1, -1, "Target next Ship",                      TARGET_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_T, -1, -1, "Target Previous Ship",                  TARGET_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_H,  2,  2, "Target Next Closest Hostile Ship",      TARGET_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_H, -1, -1, "Target Previous Closest Hostile Ship",  TARGET_TAB, CC_TYPE_TRIGGER, true )
	( KEY_ALTED |               KEY_H, -1, -1, "Toggle Auto Targeting",                 TARGET_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_F, -1, -1, "Target Next Closest Friendly Ship",     TARGET_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_F, -1, -1, "Target Previous Closest Friendly Ship", TARGET_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_Y,  4,  4, "Target Ship in Reticle",                TARGET_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_G, -1, -1, "Target Target's Nearest Attacker",      TARGET_TAB, CC_TYPE_TRIGGER, true )
	( KEY_ALTED |               KEY_Y, -1, -1, "Target Last Ship to Send Transmission", TARGET_TAB, CC_TYPE_TRIGGER, true )
	( KEY_ALTED |               KEY_T, -1, -1, "Turn Off Targeting",                    TARGET_TAB, CC_TYPE_TRIGGER, true )

	// targeting a ship's subsystem
	(                           KEY_V, -1, -1, "Target Subsystem in Reticle",           TARGET_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_S, -1, -1, "Target Next Subsystem",                 TARGET_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_S, -1, -1, "Target Previous Subsystem",             TARGET_TAB, CC_TYPE_TRIGGER, true )
	( KEY_ALTED |               KEY_S, -1, -1, "Turn Off Targeting of Subsystems",      TARGET_TAB, CC_TYPE_TRIGGER, true )

	//  matching speed
	(                           KEY_M, -1, -1, "Match Target Speed",                    COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	( KEY_ALTED |               KEY_M, -1, -1, "Toggle Auto Speed Matching",            COMPUTER_TAB, CC_TYPE_TRIGGER, true )

	//  weapons
	(                           KEY_LCTRL,     0,  0, "Fire Primary Weapon",                WEAPON_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_SPACEBAR,  1,  1, "Fire Secondary Weapon",              WEAPON_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PERIOD,   -1, -1, "Cycle Forward Primary Weapon",       WEAPON_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_COMMA,    -1, -1, "Cycle Backward Primary Weapon",      WEAPON_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_DIVIDE,   -1, -1, "Cycle Secondary Weapon Bank",        WEAPON_TAB, CC_TYPE_TRIGGER,    true )
	(             KEY_SHIFTED | KEY_DIVIDE,   -1, -1, "Cycle Secondary Weapon Firing Rate", WEAPON_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_X,         3,  3, "Launch Countermeasure",              WEAPON_TAB, CC_TYPE_TRIGGER,    true )

	//  controls
	(                           KEY_A,    -1, -1, "Forward Thrust", SHIP_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_Z,    -1, -1, "Reverse Thrust", SHIP_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PAD7, -1, -1, "Bank Left",      SHIP_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PAD9, -1, -1, "Bank Right",     SHIP_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PAD8, -1, -1, "Pitch Forward",  SHIP_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PAD2, -1, -1, "Pitch Backward", SHIP_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PAD4, -1, -1, "Turn Left",      SHIP_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PAD6, -1, -1, "Turn Right",     SHIP_TAB, CC_TYPE_CONTINUOUS, true )

	//  throttle controls
	(                           KEY_BACKSP,   -1, -1, "Set Throttle to Zero",        SHIP_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_SLASH,    -1, -1, "Set Throttle to Max",         SHIP_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_LBRACKET, -1, -1, "Set Throttle to One-Third",   SHIP_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_RBRACKET, -1, -1, "Set Throttle to Two-Thirds",  SHIP_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_EQUAL,    -1, -1, "Increase Throttle 5 Percent", SHIP_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_MINUS,    -1, -1, "Decrease Throttle 5 Percent", SHIP_TAB, CC_TYPE_TRIGGER, true )

	//{  squadmate messaging
	(             KEY_SHIFTED | KEY_A, -1, -1, "Attack My Target",    COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_Z, -1, -1, "Disarm My Target",    COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_D, -1, -1, "Disable My Target",   COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_V, -1, -1, "Attack My Subsystem", COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_X, -1, -1, "Capture My Target",   COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_E, -1, -1, "Engage Enemy",        COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_W, -1, -1, "Form on My Wing",     COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_I, -1, -1, "Ignore My Target",    COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_P, -1, -1, "Protect My Target",   COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_C, -1, -1, "Cover Me",            COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_J, -1, -1, "Return to Base",      COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_R, -1, -1, "Rearm Me",            COMPUTER_TAB, CC_TYPE_TRIGGER, true )

	(                           KEY_R, -1,  6, "Target Closest Attacking Ship", TARGET_TAB, CC_TYPE_TRIGGER, true )

	//  Views
	(                           KEY_PADMULTIPLY,    -1, -1, "Chase View",                  COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_PADPERIOD,      -1, -1, "External View",               COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_PADENTER,       -1, -1, "Toggle External Camera Lock", COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_PAD0,           -1, -1, "Free Look View",              COMPUTER_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PADDIVIDE,      -1, -1, "Current Target View",         COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_PADPLUS,        -1, -1, "Increase View Distance",      COMPUTER_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PADMINUS,       -1, -1, "Decrease View Distance",      COMPUTER_TAB, CC_TYPE_CONTINUOUS, true )
	(                           KEY_PAD5,           -1, -1, "Center View",                 COMPUTER_TAB, CC_TYPE_CONTINUOUS, true )
	(                         -1,     -1,   JOY_HATFORWARD, "View Up",                     COMPUTER_TAB, CC_TYPE_CONTINUOUS, true )
	(                         -1,     -1,   JOY_HATBACK,    "View Rear",                   COMPUTER_TAB, CC_TYPE_CONTINUOUS, true )
	(                         -1,     -1,   JOY_HATLEFT,    "View Left",                   COMPUTER_TAB, CC_TYPE_CONTINUOUS, true )
	(                         -1,     -1,   JOY_HATRIGHT,   "View Right",                  COMPUTER_TAB, CC_TYPE_CONTINUOUS, true )

	(                           KEY_RAPOSTRO, -1, -1, "Cycle Radar Range",            COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_C,        -1, -1, "Communications Menu",          COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(                           -1,           -1, -1, "Show Objectives",              -1,           CC_TYPE_TRIGGER,    true )
	( KEY_ALTED |               KEY_J,        -1, -1, "Enter Subspace (End Mission)", COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_J,        -1, -1, "Target Target's Target",       TARGET_TAB,   CC_TYPE_TRIGGER,    true )
	(                           KEY_TAB,      -1,  5, "Afterburner",                  SHIP_TAB,     CC_TYPE_CONTINUOUS, true )

	(                           KEY_INSERT,   -1, -1, "Increase Weapon Energy",   COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_DELETE,   -1, -1, "Decrease Weapon Energy",   COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_HOME,     -1, -1, "Increase Shield Energy",   COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_END,      -1, -1, "Decrease Shield Energy",   COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_PAGEUP,   -1, -1, "Increase Engine Energy",   COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_PAGEDOWN, -1, -1, "Decrease Engine Energy",   COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	( KEY_ALTED |               KEY_D,        -1, -1, "Equalize Energy Settings", COMPUTER_TAB, CC_TYPE_TRIGGER, true )

	(                           KEY_Q,         -1,  7, "Equalize Shields",              COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_UP,        -1, -1, "Augment Forward Shield",        COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_DOWN,      -1, -1, "Augment Rear Shield",           COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_LEFT,      -1, -1, "Augment Left Shield",           COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_RIGHT,     -1, -1, "Augment Right Shield",          COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_SCROLLOCK, -1, -1, "Transfer Energy Laser->Shield", COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_SCROLLOCK, -1, -1, "Transfer Energy Shield->Laser", COMPUTER_TAB, CC_TYPE_TRIGGER, true )
//	(                           -1,            -1, -1, "Show Damage Popup Window" },    -1,

	(                           -1,    -1, -1, "Glide When Pressed",           SHIP_TAB, CC_TYPE_CONTINUOUS, false )
//Back slash -- this was a convenient place for Glide When Pressed, as Show Damage Popup isn't used
	(                           -1,    -1, -1, "Bank When Pressed",          SHIP_TAB,     CC_TYPE_CONTINUOUS, true )
	(                           -1,    -1, -1, "Show Nav Map",               -1,           CC_TYPE_TRIGGER,    true )
	( KEY_ALTED |               KEY_E, -1, -1, "Add or Remove Escort",       COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	( KEY_ALTED | KEY_SHIFTED | KEY_E, -1, -1, "Clear Escort List",          COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(                           KEY_E, -1, -1, "Target Next Escort Ship",    TARGET_TAB,   CC_TYPE_TRIGGER,    true )
	( KEY_ALTED |               KEY_R, -1, -1, "Target Closest Repair Ship", TARGET_TAB,   CC_TYPE_TRIGGER,    true )

	(                           KEY_U, -1, -1, "Target Next Uninspected Cargo",     TARGET_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_U, -1, -1, "Target Previous Uninspected Cargo", TARGET_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_N, -1, -1, "Target Newest Ship in Area",        TARGET_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_K, -1, -1, "Target Next Live Turret",           TARGET_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_K, -1, -1, "Target Previous Live Turret",       TARGET_TAB, CC_TYPE_TRIGGER, true )

	(                           KEY_B, -1, -1, "Target Next Hostile Bomb or Bomber",     TARGET_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_B, -1, -1, "Target Previous Hostile Bomb or Bomber", TARGET_TAB, CC_TYPE_TRIGGER, true )

	//  multiplayer messaging keys
	(                           KEY_1,      -1, -1, "(Multiplayer) Message All",             COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_2,      -1, -1, "(Multiplayer) Message Friendly",        COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_3,      -1, -1, "(Multiplayer) Message Hostile",         COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_4,      -1, -1, "(Multiplayer) Message Target",          COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	( KEY_ALTED |               KEY_X,      -1, -1, "(Multiplayer) Observer Zoom to Target", COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_PERIOD, -1, -1, "Increase Time Compression",             COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_COMMA,  -1, -1, "Decrease Time Compression",             COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(                           KEY_L,      -1, -1, "Toggle High HUD Contrast",              COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_N,      -1, -1, "(Multiplayer) Toggle Network Info",     COMPUTER_TAB, CC_TYPE_TRIGGER, true )
	(             KEY_SHIFTED | KEY_END,    -1, -1, "(Multiplayer) Self Destruct",           COMPUTER_TAB, CC_TYPE_TRIGGER, true )

	//  Misc
	(             KEY_SHIFTED | KEY_O,        -1, -1, "Toggle HUD",                       COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(             KEY_SHIFTED | KEY_3,        -1, -1, "Right Thrust",                     SHIP_TAB,     CC_TYPE_CONTINUOUS, true )
	(             KEY_SHIFTED | KEY_1,        -1, -1, "Left Thrust",                      SHIP_TAB,     CC_TYPE_CONTINUOUS, true )
	(             KEY_SHIFTED | KEY_PADPLUS,  -1, -1, "Up Thrust",                        SHIP_TAB,     CC_TYPE_CONTINUOUS, true )
	(             KEY_SHIFTED | KEY_PADENTER, -1, -1, "Down Thrust",                      SHIP_TAB,     CC_TYPE_CONTINUOUS, true )
	( KEY_ALTED | KEY_SHIFTED | KEY_Q,        -1, -1, "Toggle HUD Wireframe Target View", COMPUTER_TAB, CC_TYPE_TRIGGER,    true )
	(                           -1,           -1, -1, "Top-Down View",                    COMPUTER_TAB, CC_TYPE_TRIGGER,    false )
	(                           -1,           -1, -1, "Target Padlock View",              COMPUTER_TAB, CC_TYPE_TRIGGER,    false ) // (Swifty) Toggle for VM_TRACK
	//  Auto Navigation Systen
	( KEY_ALTED |               KEY_A,        -1, -1, "Toggle Auto Pilot",                COMPUTER_TAB, CC_TYPE_TRIGGER,    false )
	( KEY_ALTED |               KEY_N,        -1, -1, "Cycle Nav Points",                 COMPUTER_TAB, CC_TYPE_TRIGGER,    false )
	( KEY_ALTED |               KEY_G,        -1, -1, "Toggle Gliding",                   SHIP_TAB,     CC_TYPE_TRIGGER,    false )
	(                           KEY_O,        -1, -1, "Cycle Primary Weapon Firing Rate", WEAPON_TAB,   CC_TYPE_TRIGGER,    false )
	;

	control_config_common_load_overrides();
	if(Lcl_gr){
		Scan_code_text = Scan_code_text_german;
		Joy_button_text = Joy_button_text_german;
	} else if(Lcl_fr){
		Scan_code_text = Scan_code_text_french;
		Joy_button_text = Joy_button_text_french;
	} else if(Lcl_pl){
		Scan_code_text = Scan_code_text_polish;
		Joy_button_text = Joy_button_text_polish;
	} else {
		Scan_code_text = Scan_code_text_english;
		Joy_button_text = Joy_button_text_english;
	}
}



#include <map>
#include <string>
SCP_map<SCP_string, int> mKeyNameToVal;
SCP_map<SCP_string, CC_type> mCCTypeNameToVal;
SCP_map<SCP_string, char> mCCTabNameToVal;

/*! Helper function to LoadEnumsIntoMaps(), Loads the Keyboard definitions/enumerations into mKeyNameToVal
*/
void LoadEnumsIntoKeyMap(void) {
	// Dirty macro hack :D
#define ADD_ENUM_TO_KEY_MAP(Enum) mKeyNameToVal[#Enum] = (Enum);

	ADD_ENUM_TO_KEY_MAP(KEY_SHIFTED)
		/*
		ADD_ENUM_TO_KEY_MAP(KEY_ALTED)
		ADD_ENUM_TO_KEY_MAP(KEY_CTRLED)
		ADD_ENUM_TO_KEY_MAP(KEY_DEBUGGED)
		ADD_ENUM_TO_KEY_MAP(KEY_DEBUGGED1)
		ADD_ENUM_TO_KEY_MAP(KEY_MASK)

		ADD_ENUM_TO_KEY_MAP(KEY_DEBUG_KEY)
		*/
		ADD_ENUM_TO_KEY_MAP(KEY_0)
		ADD_ENUM_TO_KEY_MAP(KEY_1)
		ADD_ENUM_TO_KEY_MAP(KEY_2)
		ADD_ENUM_TO_KEY_MAP(KEY_3)
		ADD_ENUM_TO_KEY_MAP(KEY_4)
		ADD_ENUM_TO_KEY_MAP(KEY_5)
		ADD_ENUM_TO_KEY_MAP(KEY_6)
		ADD_ENUM_TO_KEY_MAP(KEY_7)
		ADD_ENUM_TO_KEY_MAP(KEY_8)
		ADD_ENUM_TO_KEY_MAP(KEY_9)

		ADD_ENUM_TO_KEY_MAP(KEY_A)
		ADD_ENUM_TO_KEY_MAP(KEY_B)
		ADD_ENUM_TO_KEY_MAP(KEY_C)
		ADD_ENUM_TO_KEY_MAP(KEY_D)
		ADD_ENUM_TO_KEY_MAP(KEY_E)
		ADD_ENUM_TO_KEY_MAP(KEY_F)
		ADD_ENUM_TO_KEY_MAP(KEY_G)
		ADD_ENUM_TO_KEY_MAP(KEY_H)
		ADD_ENUM_TO_KEY_MAP(KEY_I)
		ADD_ENUM_TO_KEY_MAP(KEY_J)
		ADD_ENUM_TO_KEY_MAP(KEY_K)
		ADD_ENUM_TO_KEY_MAP(KEY_L)
		ADD_ENUM_TO_KEY_MAP(KEY_M)
		ADD_ENUM_TO_KEY_MAP(KEY_N)
		ADD_ENUM_TO_KEY_MAP(KEY_O)
		ADD_ENUM_TO_KEY_MAP(KEY_P)
		ADD_ENUM_TO_KEY_MAP(KEY_Q)
		ADD_ENUM_TO_KEY_MAP(KEY_R)
		ADD_ENUM_TO_KEY_MAP(KEY_S)
		ADD_ENUM_TO_KEY_MAP(KEY_T)
		ADD_ENUM_TO_KEY_MAP(KEY_U)
		ADD_ENUM_TO_KEY_MAP(KEY_V)
		ADD_ENUM_TO_KEY_MAP(KEY_W)
		ADD_ENUM_TO_KEY_MAP(KEY_X)
		ADD_ENUM_TO_KEY_MAP(KEY_Y)
		ADD_ENUM_TO_KEY_MAP(KEY_Z)

		ADD_ENUM_TO_KEY_MAP(KEY_MINUS)
		ADD_ENUM_TO_KEY_MAP(KEY_EQUAL)
		ADD_ENUM_TO_KEY_MAP(KEY_DIVIDE)
		ADD_ENUM_TO_KEY_MAP(KEY_SLASH)
		ADD_ENUM_TO_KEY_MAP(KEY_SLASH_UK)
		ADD_ENUM_TO_KEY_MAP(KEY_COMMA)
		ADD_ENUM_TO_KEY_MAP(KEY_PERIOD)
		ADD_ENUM_TO_KEY_MAP(KEY_SEMICOL)

		ADD_ENUM_TO_KEY_MAP(KEY_LBRACKET)
		ADD_ENUM_TO_KEY_MAP(KEY_RBRACKET)

		ADD_ENUM_TO_KEY_MAP(KEY_RAPOSTRO)
		ADD_ENUM_TO_KEY_MAP(KEY_LAPOSTRO)

		ADD_ENUM_TO_KEY_MAP(KEY_ESC)
		ADD_ENUM_TO_KEY_MAP(KEY_ENTER)
		ADD_ENUM_TO_KEY_MAP(KEY_BACKSP)
		ADD_ENUM_TO_KEY_MAP(KEY_TAB)
		ADD_ENUM_TO_KEY_MAP(KEY_SPACEBAR)

		ADD_ENUM_TO_KEY_MAP(KEY_NUMLOCK)
		ADD_ENUM_TO_KEY_MAP(KEY_SCROLLOCK)
		ADD_ENUM_TO_KEY_MAP(KEY_CAPSLOCK)

		ADD_ENUM_TO_KEY_MAP(KEY_LSHIFT)
		ADD_ENUM_TO_KEY_MAP(KEY_RSHIFT)

		ADD_ENUM_TO_KEY_MAP(KEY_LALT)
		ADD_ENUM_TO_KEY_MAP(KEY_RALT)

		ADD_ENUM_TO_KEY_MAP(KEY_LCTRL)
		ADD_ENUM_TO_KEY_MAP(KEY_RCTRL)

		ADD_ENUM_TO_KEY_MAP(KEY_F1)
		ADD_ENUM_TO_KEY_MAP(KEY_F2)
		ADD_ENUM_TO_KEY_MAP(KEY_F3)
		ADD_ENUM_TO_KEY_MAP(KEY_F4)
		ADD_ENUM_TO_KEY_MAP(KEY_F5)
		ADD_ENUM_TO_KEY_MAP(KEY_F6)
		ADD_ENUM_TO_KEY_MAP(KEY_F7)
		ADD_ENUM_TO_KEY_MAP(KEY_F8)
		ADD_ENUM_TO_KEY_MAP(KEY_F9)
		ADD_ENUM_TO_KEY_MAP(KEY_F10)
		ADD_ENUM_TO_KEY_MAP(KEY_F11)
		ADD_ENUM_TO_KEY_MAP(KEY_F12)

		ADD_ENUM_TO_KEY_MAP(KEY_PAD0)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD1)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD2)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD3)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD4)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD5)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD6)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD7)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD8)
		ADD_ENUM_TO_KEY_MAP(KEY_PAD9)
		ADD_ENUM_TO_KEY_MAP(KEY_PADMINUS)
		ADD_ENUM_TO_KEY_MAP(KEY_PADPLUS)
		ADD_ENUM_TO_KEY_MAP(KEY_PADPERIOD)
		ADD_ENUM_TO_KEY_MAP(KEY_PADDIVIDE)
		ADD_ENUM_TO_KEY_MAP(KEY_PADMULTIPLY)
		ADD_ENUM_TO_KEY_MAP(KEY_PADENTER)

		ADD_ENUM_TO_KEY_MAP(KEY_INSERT)
		ADD_ENUM_TO_KEY_MAP(KEY_HOME)
		ADD_ENUM_TO_KEY_MAP(KEY_PAGEUP)
		ADD_ENUM_TO_KEY_MAP(KEY_DELETE)
		ADD_ENUM_TO_KEY_MAP(KEY_END)
		ADD_ENUM_TO_KEY_MAP(KEY_PAGEDOWN)
		ADD_ENUM_TO_KEY_MAP(KEY_UP)
		ADD_ENUM_TO_KEY_MAP(KEY_DOWN)
		ADD_ENUM_TO_KEY_MAP(KEY_LEFT)
		ADD_ENUM_TO_KEY_MAP(KEY_RIGHT)

		ADD_ENUM_TO_KEY_MAP(KEY_PRINT_SCRN)
		ADD_ENUM_TO_KEY_MAP(KEY_PAUSE)
		ADD_ENUM_TO_KEY_MAP(KEY_BREAK)

#undef ADD_ENUM_TO_KEY_MAP
}

/*! Helper function to LoadEnumsIntoMaps(), Loads the Control Types enumerations into mCCTypeNameToVal
 */
void LoadEnumsIntoCCTypeMap(void) {
	// Dirty macro hack :D
#define ADD_ENUM_TO_CCTYPE_MAP(Enum) mCCTypeNameToVal[#Enum] = (Enum);

	ADD_ENUM_TO_CCTYPE_MAP(CC_TYPE_TRIGGER)
		ADD_ENUM_TO_CCTYPE_MAP(CC_TYPE_CONTINUOUS)

#undef ADD_ENUM_TO_CCTYPE_MAP
}

/*! Helper function to LoadEnumsIntoMaps(), Loads the Control Tabs enumerations into mCCTabNameToVal
 */
void LoadEnumsIntoCCTabMap(void) {
	// Dirty macro hack :D
#define ADD_ENUM_TO_CCTAB_MAP(Enum) mCCTabNameToVal[#Enum] = (Enum);

	ADD_ENUM_TO_CCTAB_MAP(TARGET_TAB)
	ADD_ENUM_TO_CCTAB_MAP(SHIP_TAB)
	ADD_ENUM_TO_CCTAB_MAP(WEAPON_TAB)
	ADD_ENUM_TO_CCTAB_MAP(COMPUTER_TAB)

#undef ADD_ENUM_TO_CCTAB_MAP
}

/*! Loads the various control configuration maps to allow the parsing functions to appropriately map string tokns to
* their associated enumerations. The string tokens in the controlconfigdefaults.tbl match directly to their names in
* the C++ code, such as "KEY_5" in the .tbl mapping to the #define KEY_5 value
*/
void LoadEnumsIntoMaps() {
	LoadEnumsIntoKeyMap();
	LoadEnumsIntoCCTypeMap();
	LoadEnumsIntoCCTabMap();
}

/**
 * @breif Parses a CCFG override bind group
 */
void control_config_common_parse_bind(Ccfg_vector &cfg_preset) {
	SCP_string szTempBuffer;
	SCP_vector<config_item>::iterator p_ccConfig;

	required_string("$Bind Name:");
	stuff_string(szTempBuffer, F_NAME);

	// Search for item with text
	for (p_ccConfig = cfg_preset.begin(); p_ccConfig != cfg_preset.end(); ++p_ccConfig) {
		if (p_ccConfig->text == szTempBuffer) {
			break;
		}
	};

	if (p_ccConfig != cfg_preset.end()) {
		// Found our item
		int iTemp;

		if (optional_string("$Key Default:")) {
			if (optional_string("NONE")) {
				p_ccConfig->default[CON_KEYBOARD] = (short)-1;
			} else {
				stuff_string(szTempBuffer, F_NAME);
				p_ccConfig->default[CON_KEYBOARD] = (short)mKeyNameToVal[szTempBuffer];
			}
		}

		if (optional_string("$Joy Default:")) {
			stuff_int(&iTemp);
			p_ccConfig->default[CON_JOY] = (short)iTemp;
		}

		if (optional_string("$Key Mod Shift:")) {
			stuff_int(&iTemp);
			p_ccConfig->default[CON_KEYBOARD] |= (iTemp == 1) ? KEY_SHIFTED : 0;
		}

		if (optional_string("$Key Mod Alt:")) {
			stuff_int(&iTemp);
			p_ccConfig->default[CON_KEYBOARD] |= (iTemp == 1) ? KEY_ALTED : 0;
		}

		if (optional_string("$Key Mod Ctrl:")) {
			stuff_int(&iTemp);
			p_ccConfig->default[CON_KEYBOARD] |= (iTemp == 1) ? KEY_CTRLED : 0;
		}

		if (optional_string("$Category:")) {
			stuff_string(szTempBuffer, F_NAME);
			p_ccConfig->tab = (char)mCCTabNameToVal[szTempBuffer];
		}

		if (optional_string("$Has XStr:")) {
			stuff_int(&iTemp);
			p_ccConfig->hasXSTR = (iTemp == 1);
		}

		if (optional_string("$Type:")) {
			stuff_string(szTempBuffer, F_NAME);
			p_ccConfig->type = (char)mCCTypeNameToVal[szTempBuffer];
		}

		if (optional_string("+Disable")) {
			p_ccConfig->disabled = true;
		};

	} else {
		// Couldn't find item
		error_display(1, "Bind Name not found: %s\n", szTempBuffer);
		advance_to_eoln(NULL);
		ignore_white_space();
	}
}

/**
 * @brief Parses controlconfigdefault.tbl, and overrides the default control configuration for each valid entry in the .tbl
 */
void control_config_common_load_overrides()
{
	LoadEnumsIntoMaps();

	if (cf_exists_full("controlconfigdefaults.tbl", CF_TYPE_TABLES)) {
		read_file_text("controlconfigdefaults.tbl", CF_TYPE_TABLES);
	} else {
		read_file_text_from_array(defaults_get_file("controlconfigdefaults.tbl"));
	}

	reset_parse();

	// start parsing
	while(optional_string("#ControlConfigOverride")) {
		Ccfg_vector cfg_preset(Control_config);

		SCP_string preset_name;
		if (optional_string("$Name:")) {
			stuff_string_line(preset_name);
		} else {
			preset_name = "<unnamed preset>";
		}

		// Parse all bindings
		while (required_string_either("#End","$Bind Name:")) {
			control_config_common_parse_bind(cfg_preset);
		}

		// Push back our presets
		Control_config_preset_names.push_back(preset_name);
		Control_config_presets.push_back(cfg_preset);

		required_string("#End");
	}
	
	// Overwrite the control config with the first preset that was found
	if (!Control_config_presets.empty()) {
		auto first_preset = Control_config_presets.begin();

		Control_config.assign(first_preset->begin(), first_preset->end());
	}
}

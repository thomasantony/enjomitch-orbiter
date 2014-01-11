// ==============================================================
//                 ORBITER MODULE: LaunchMFD
//                  Part of the ORBITER SDK
//
// Copyright (C) 2004      rjcroy                      - robust time based pitch autopilot (borrowed code)
// Copyright (C) 2004      Dave "Daver" Rowbotham      - conversion of rjcroy's autopolot to C++ (borrowed code)
// Copyright (C) 2004      Erik H. "Sputnik" Anderson  - conversion of the autopilot to energy based (borrowed code)
// Copyright (C) 2007      "Vanguard"                  - dressing up azimuth calcualtions into an MFD (author)
// Copyright (C) 2007      Pawel "She'da'Lier" Stiasny - yaw error visual representation (contributor)
// Copyright (C) 2008      Mohd "Computerex" Ali       - borrowed his code (multiple vessels support) (borrowed code)
// Copyright (C) 2008      Chris "Kwan" Jeppesen       - borrowed his code (peg guidance) (borrowed code)
// Copyright (C) 2008      Steve "agentgonzo" Arch     - peg integration, offplane correction, compass, hud display (co-developer)
// Copyright (C) 2007-2012 Szymon "Enjo" Ender         - everything else ;> (author and maintainer)
//                         All rights reserved
//
// localisation.h
// Authors - Szymon "Enjo" Ender (localisation), Steve "agentgonzo" Arch (code organisation)
//
// This module calculates the appropriate launch azimuth given
// desired orbital inclination and desired orbit altitude. This
// MFD takes the planets rotation into account, which provides a
// much more accurate azimuth. The calculations are performed
// 'on the fly' (technically and methaphorically), meaning that
// you get info about necessary course corrections.
//
// This file is part of LaunchMFD.
//
// LaunchMFD is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// LaunchMFD is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with LaunchMFD.  If not, see <http://www.gnu.org/licenses/>.
// ==============================================================


#pragma once

// undefine existing definitions to prevent compiler warnings
#undef ERROR
#undef FAILED

#ifdef LANG_PL
// Polish text
// LaunchMFD
#define TITLE_AZIMUTH				"LaunchMFD 1/3         Standard"
#define TITLE_COMPASS				"LaunchMFD 2/3         Kompas startu"
#define TITLE_DIRECT_ASCENT			"LaunchMFD 3/3         Wejscie bezposr."
#define MANUAL						"Reczny"
#define PROBE						"LaunchMFD-Probe"
#define INCREASE_INCLINATION		"Zwieksz  inklin."
#define DECREASE_INCLINATION		"Zmniejsz inklin."
#define INCREASE_INCLINATION_FACTOR	"+ wsp. zmian inkl."
#define DECREASE_INCLINATION_FACTOR	"- wsp. zmian inkl."
#define INCREASE_ALTITUDE			"Zwieksz  wysokosc"
#define DECREASE_ALTITUDE			"Zmniejsz wysokosc"
#define ADJUST_ALTITUDE_FACTOR		"Zmien wsp. zmian wys."
#define SELECT_TARGET				"Wybierz cel"
#define ENTER_ALT					"Wprowadz wysokosci"
#define ENTER_FINAL_DIST            "Wprowadz finalny dystans [m]. Wart. dodatnie: ponad celem; ujemne: poni¿ej"
#define AUTOMATE_ALTITUDE			"Zautomat. wysokosc"
#define SWITCH_HUD					"Przelacz HUD"
#define SWITCH_SOUNDS				"Przelacz dzwieki"
#define OFF_PLANE_CORRECTION		"Korekta plaszczyzny"
#define AUTOPILOT				" Autopilot PID"
#define AUTOPILOT_DA			" Autopilot PID - DA"
#define SPACE                       " Kosm."
#define ATMO                        " Atmo."
#define OPERATION_MODE				"Tryb dzialania"
#define CALCULATE_DA				"Oblicz DA"
#define CHANGE_VIEW					"Zmien widok"
#define DEFAULT_ACTION				"Domyslna akcja"
#define FAILED						"Porazka"
#define ENTER_ALTITUDE				"Dla PEG. Wprowadz: PeA (dla kolowej) -lub- PeA ApA -lub- a (automatyczna) -lub- t (celu) -lub- TransX -lub- TX"
#define TARGET						"Wprowadz: Cel -lub- TransX -lub- TX -lub- Inklinacje (wzgl. rownika) -lub- Inkl. LAN q (wzgl. rownika) -lub- Inkl. LAN c (wzgl. ekliptyki)"
#define PID_TUNE_ATMO_XY		  	"Strojenie Atmo. PID XY. Wprowadz Kp Kd Ki dla statku "
#define PID_TUNE_ATMO_BANK		   	"Strojenie Atmo. PID Bank. Wprowadz Kp Kd Ki dla statku "
#define PID_TUNE_SPACE_XY		  	"Strojenie Kosm. PID XY. Wprowadz Kp Kd Ki dla statku "
#define PID_TUNE_SPACE_BANK		   	"Strojenie Kosm. PID Bank. Wprowadz Kp Kd Ki dla statku "
#define CUT_YOUR_ENGINES			"          Wylacz       Silniki! "
#define REQUIRED_HEADING			" Wymagany azymut: "
#define NORTHERN					" polnocny:   "
#define SOUTHERN					" poludniowy: "
#define CORRECTION					" korekta:    "
#define HEADING_ERROR				" Blad azym.: "
#define PITCH_ERROR					" Blad poch.: "
#define CURRENT						" Akt.: "
#define	INCLINATION_ADJ_FACTOR		" Wsp. zmian inklinacji: "
#define ALTITUDE_ADJ_FACTOR			" Wsp. zmian wysokosci: "
#define CURRENT_LATITUDE			" Szerokosc geogr.:   "
#define SELECTED_TARGET						" Wybrany cel: "
#define DESIRED_INCLINATION			" Wybrana inklinacja: "
#define REFERENCE_ALTITUDE			" Wysokosc odniesienia: "
#define PEA							" Docelowa wys. peryc.: "
#define APA							" Docelowa wys. apoc.: "
#define INTERSECTION_TIME			" Czas do przeciecia: "
#define NEXT_LAUNCH_TIME			" Czas polowy V orbit.: "
#define DIRECT_ASCENT_VIEW			"Widok wejscia bezposr."
#define TIME_TO_MECO				" Czas do MECO: "
#define RINC                        "Relat. inkl.:"
#define SWITCH_BUTTONS_PAGE         "Przelacz przyciski"
#define GREAT_CIRCLE                "Wielki Okrag"
#define GREAT_CIRCLE_SWITCH         "Przelacz WO"
#define GREAT_CIRCLE_TRACK          "Sledz WO"
#define GREAT_CIRCLE_ZOOM_IN        "Przybliz WO"
#define GREAT_CIRCLE_ZOOM_OUT       "Oddal WO"
#define GREAT_CIRCLE_PREC_INCR      "+ precyzja WO"
#define GREAT_CIRCLE_PREC_DECR      "- precyzja WO"
#define GREAT_CIRCLE_LINES          "Linie WO"
#define PID_ADJUST_XY               "Stroj. PID XY"
#define PID_ADJUST_BANK             "Stroj. PID Bank"
#define SWITCH_PITCH_GUIDANCE       "Przelacz ster. pitch"
#define SWITCH_AZIMUTH              "Przelacz azymut"
#define MANUAL_TGT_ONLY             "Tylko cel reczny"


// Launch Compass
#define NORTH			"N"
#define SOUTH			"S"
#define EAST			"W"
#define WEST			"Z"
#define MODE			"Tryb:"
#define NORTH_UP		"Polnoc gora"
#define HEAD_UP			"Glowa gora"
#define HEADING_UP		"Azymut gora"
#define TARGET_ASC		"Cel-polnocny:"
#define TARGET_DSC		"Cel-polodn.:"
#define PITCH			"Pochyl:"
#define HEAD_BEARING	"Glowa-azym.:"
#define FOOT_BEARING	"Stopa-azym.:"
#define ALTITUDE		"Wysokosc:"
#define VERTICAL_SPEED	"V. Pionowa:"
#define SET_TARGET_MODE	"Set Target Mode"
// Direct Ascent
#define DIRECT_ASCENT_STR   "Wejscie bezposrednie"
#define SYNCHRO_ASCENT  "Wejscie synchroniczne"
#define TIME_TO_NODE    "Czas do wezla:"
#define DV_NEEDED       "potrzebne DV:"
#define T_TO_PERIAPSIS  "T do perycentrum:"
#define T_TO_BURN       "T do odpalenia:"
#define SATELLITE_T     "T satelity:"
#define SHIP_TMECO      "TMECO statku:"
#define APPROACHING     "Podchodzenie do "
#define GUIDANCE_HAS_CONVERGED  "Naprowadzanie zbiezne"

#elif defined LANG_EN
// English
// LaunchMFD
#define TITLE_AZIMUTH				"LaunchMFD 1/3         Standard"
#define TITLE_COMPASS				"LaunchMFD 2/3         Launch Compass"
#define TITLE_DIRECT_ASCENT			"LaunchMFD 3/3         Direct Ascent"
#define MANUAL						"Manual"
#define PROBE						"LaunchMFD-Probe"
#define INCREASE_INCLINATION		"Increase Incl."
#define DECREASE_INCLINATION		"Decrease Incl."
#define INCREASE_INCLINATION_FACTOR	"Incr. Incl. Factor"
#define DECREASE_INCLINATION_FACTOR	"Decr. Incl. Factor"
#define INCREASE_ALTITUDE			"Increase Altitude"
#define DECREASE_ALTITUDE			"Decrease Altitude"
#define ADJUST_ALTITUDE_FACTOR		"Adjust Alt. Factor"
#define SELECT_TARGET				"Select Target"
#define ENTER_ALT					"Enter Altitudes"
#define ENTER_FINAL_DIST            "Enter Final Distance [m]. Positive values: above target; negative: below"
#define AUTOMATE_ALTITUDE			"Automate Altitude"
#define SWITCH_HUD					"Switch HUD"
#define SWITCH_SOUNDS				"Switch Sounds"
#define OFF_PLANE_CORRECTION		"Off Plane Correction"
#define AUTOPILOT				" PID Autopilot"
#define AUTOPILOT_DA			" PID Autopilot - DA"
#define SPACE                       " Space"
#define ATMO                        " Atmo"
#define OPERATION_MODE				"Operation Mode"
#define CALCULATE_DA				"Calculate DA"
#define CHANGE_VIEW					"Change View"
#define DEFAULT_ACTION				"Default Action"
#define FAILED						"Failed"
#define ENTER_ALTITUDE				"For PEG. Enter: PeA (for circular) -or- PeA ApA -or- a (for automatic) -or- t (target's) -or- TransX -or- TX"
#define TARGET						"Enter: Target -or- TransX -or- TX -or- Inclination (equ. frame) -or- Incl. LAN q (equ. frame) -or- Incl. LAN c (ecl. frame)"
#define PID_TUNE_ATMO_XY		  	"Tuning Atmo PID XY. Enter Kp Kd Ki for vessel "
#define PID_TUNE_ATMO_BANK		   	"Tuning Atmo PID Bank. Enter Kp Kd Ki for vessel "
#define PID_TUNE_SPACE_XY		  	"Tuning Space PID XY. Enter Kp Kd Ki for vessel "
#define PID_TUNE_SPACE_BANK		   	"Tuning Space PID Bank. Enter Kp Kd Ki for vessel "
#define CUT_YOUR_ENGINES			"        Cut Your       Engines! "
#define REQUIRED_HEADING			" Req. Heading: "
#define NORTHERN					" northern:   "
#define SOUTHERN					" southern:   "
#define CORRECTION					" correction: "
#define HEADING_ERROR				" Head. Error: "
#define PITCH_ERROR					" Pitch Error: "
#define CURRENT						" Curr.: "
#define	INCLINATION_ADJ_FACTOR		" Inclination Adj. factor: "
#define ALTITUDE_ADJ_FACTOR			" Altitude Adj. factor: "
#define CURRENT_LATITUDE			" Current Latitude: "
#define SELECTED_TARGET						" Selected Target: "
#define DESIRED_INCLINATION			" Desired Inclination: "
#define REFERENCE_ALTITUDE			" Reference Altitude: "
#define PEA							" Target Periapsis Alt: "
#define APA							" Target Apoapsis Alt: "
#define INTERSECTION_TIME			" Time to Intersection: "
#define NEXT_LAUNCH_TIME			" Half of Orb.V. Time: "
#define DIRECT_ASCENT_VIEW			"Direct Ascent view"
#define TIME_TO_MECO				" Time To MECO: "
#define RINC                        "Relat. incl.: "
#define SWITCH_BUTTONS_PAGE         "Switch buttons page"
#define GREAT_CIRCLE                "Great Circle"
#define GREAT_CIRCLE_SWITCH         "Switch GC"
#define GREAT_CIRCLE_TRACK          "Track GC"
#define GREAT_CIRCLE_ZOOM_IN        "Zoom in GC"
#define GREAT_CIRCLE_ZOOM_OUT       "Zoom out GC"
#define GREAT_CIRCLE_PREC_INCR      "Incr. GC precision"
#define GREAT_CIRCLE_PREC_DECR      "Decr. GC precision"
#define GREAT_CIRCLE_LINES          "GC Lines"
#define PID_ADJUST_XY               "Adj. XY PID"
#define PID_ADJUST_BANK             "Adj. Bank PID"
#define SWITCH_PITCH_GUIDANCE       "Switch pitch guidance"
#define SWITCH_AZIMUTH              "Switch azimuth"
#define MANUAL_TGT_ONLY             "Manual target only"

// Launch Compass
#define NORTH			"N"
#define SOUTH			"S"
#define EAST			"E"
#define WEST			"W"
#define MODE			"Mode:"
#define NORTH_UP		"North Up"
#define HEAD_UP			"Head Up"
#define HEADING_UP		"Heading Up"
#define TARGET_ASC		"Target(asc):"
#define TARGET_DSC		"Target(dsc):"
#define PITCH			"Pitch:"
#define HEAD_BEARING	"Head-Brg:"
#define FOOT_BEARING	"Foot-Brg:"
#define ALTITUDE		"Altitude:"
#define VERTICAL_SPEED	"V. Speed:"
#define SET_TARGET_MODE	"Set Target Mode"
// Direct Ascent
#define DIRECT_ASCENT_STR   "Direct ascent"
#define SYNCHRO_ASCENT  "Synchro ascent"
#define TIME_TO_NODE    "Time to node:"
#define DV_NEEDED       "DV needed:"
#define T_TO_PERIAPSIS  "T to periapsis:"
#define T_TO_BURN       "T to burn:"
#define SATELLITE_T     "Satellite T:"
#define SHIP_TMECO      "Ship TMECO:"
#define APPROACHING     "Approaching "
#define GUIDANCE_HAS_CONVERGED  "Guidance has converged"
#endif

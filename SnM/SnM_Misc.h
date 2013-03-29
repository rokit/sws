/******************************************************************************
/ SnM_Misc.h
/
/ Copyright (c) 2012-2013 Jeffos
/ http://www.standingwaterstudios.com/reaper
/
/ Permission is hereby granted, free of charge, to any person obtaining a copy
/ of this software and associated documentation files (the "Software"), to deal
/ in the Software without restriction, including without limitation the rights to
/ use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
/ of the Software, and to permit persons to whom the Software is furnished to
/ do so, subject to the following conditions:
/ 
/ The above copyright notice and this permission notice shall be included in all
/ copies or substantial portions of the Software.
/ 
/ THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
/ EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
/ OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
/ NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
/ HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
/ WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
/ FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
/ OTHER DEALINGS IN THE SOFTWARE.
/
******************************************************************************/

//#pragma once

#ifndef _SNM_MISC_H_
#define _SNM_MISC_H_

// reascript export
WDL_FastString* SNM_CreateFastString(const char* _str);
void SNM_DeleteFastString(WDL_FastString* _str);
const char* SNM_GetFastString(WDL_FastString* _str);
int SNM_GetFastStringLength(WDL_FastString* _str);
WDL_FastString* SNM_SetFastString(WDL_FastString* _str, const char* _newStr);
MediaItem_Take* SNM_GetMediaItemTakeByGUID(ReaProject* _project, const char* _guid);
bool SNM_GetSourceType(MediaItem_Take* _tk, WDL_FastString* _type);
bool SNM_GetSetSourceState(MediaItem* _item, int takeIdx, WDL_FastString* _state, bool _setnewvalue);
bool SNM_GetSetSourceState2(MediaItem_Take* _tk, WDL_FastString* _state, bool _setnewvalue);
bool SNM_GetSetObjectState(void* _obj, WDL_FastString* _state, bool _setnewvalue, bool _minstate);
int SNM_GetIntConfigVar(const char* _varName, int _errVal);
bool SNM_SetIntConfigVar(const char* _varName, int _newVal);
double SNM_GetDoubleConfigVar(const char* _varName, double _errVal);
bool SNM_SetDoubleConfigVar(const char* _varName, double _newVal);

// exotic resources slots
#ifdef _WIN32
void LoadThemeSlot(int _slotType, const char* _title, int _slot);
void LoadThemeSlot(COMMAND_T*);
#endif
void ShowImageSlot(int _slotType, const char* _title, int _slot);
void ShowImageSlot(COMMAND_T*);
void ShowNextPreviousImageSlot(COMMAND_T*);
void SetSelTrackIconSlot(int _slotType, const char* _title, int _slot);
void SetSelTrackIconSlot(COMMAND_T*);

extern int g_SNM_LastImgSlot;

// toolbar auto refresh
void EnableToolbarsAutoRefesh(COMMAND_T*);
int IsToolbarsAutoRefeshEnabled(COMMAND_T*);
void RefreshToolbars();

extern bool g_SNM_ToolbarRefresh;
extern int g_SNM_ToolbarRefreshFreq;

// misc actions
void ChangeMetronomeVolume(COMMAND_T*);
void SimulateMouseClick(COMMAND_T*);

#endif

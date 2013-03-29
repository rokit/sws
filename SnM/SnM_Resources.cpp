/******************************************************************************
/ SnM_Resources.cpp
/
/ Copyright (c) 2009-2013 Jeffos
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

#include "stdafx.h"
#include "SnM.h"
#include "SnM_FXChain.h"
#include "SnM_Item.h"
#include "SnM_Misc.h"
#include "SnM_Project.h"
#include "SnM_Track.h"
#include "SnM_Util.h"
#include "../Prompt.h"
#ifdef _WIN32
#include "../DragDrop.h"
#endif
#include "../reaper/localize.h"


#define RES_INI_SEC					"Resources"

// common cmds
#define AUTOFILL_MSG				0xF000
#define AUTOFILL_DIR_MSG			0xF001
#define AUTOFILL_PRJ_MSG			0xF002
#define AUTOFILL_DEFAULT_MSG		0xF003
#define AUTOFILL_SYNC_MSG			0xF004
#define CLEAR_SLOTS_MSG				0xF005
#define DEL_SLOTS_MSG				0xF006
#define DEL_FILES_MSG				0xF007
#define ADD_SLOT_MSG				0xF008
#define INSERT_SLOT_MSG				0xF009
#define EDIT_MSG					0xF00A
#ifdef _WIN32
#define DIFF_MSG					0xF00B
#endif
#define EXPLORE_MSG					0xF00C
#define LOAD_MSG					0xF00D
#define AUTOSAVE_MSG				0xF00E
#define AUTOSAVE_DIR_MSG			0xF00F
#define AUTOSAVE_DIR_PRJ_MSG		0xF010
#define AUTOSAVE_DIR_DEFAULT_MSG	0xF011
#define AUTOSAVE_SYNC_MSG			0xF012
#define FILTER_BY_NAME_MSG			0xF013
#define FILTER_BY_PATH_MSG			0xF014
#define FILTER_BY_COMMENT_MSG		0xF015
#define RENAME_MSG					0xF016
#define TIE_BOOKMARK_MSG			0xF017
#define COPY_BOOKMARK_MSG			0xF018
#define DEL_BOOKMARK_MSG			0xF019
#define REN_BOOKMARK_MSG			0xF01A
#define NEW_BOOKMARK_FXC_MSG		0xF01B
#define NEW_BOOKMARK_TR_MSG			0xF01C
#define NEW_BOOKMARK_PRJ_MSG		0xF01D
#define NEW_BOOKMARK_MED_MSG		0xF01E
#define NEW_BOOKMARK_IMG_MSG		0xF01F
#define NEW_BOOKMARK_THM_MSG		0xF020 // leave some room here, at least as much as default slot types) -->
#define NEW_BOOKMARK_END_MSG		0xF02F // <--
// fx chains cmds
#define FXC_APPLY_TR_MSG			0xF030
#define FXC_APPLY_TAKE_MSG			0xF031
#define FXC_APPLY_ALLTAKES_MSG		0xF032
#define FXC_COPY_MSG				0xF033
#define FXC_PASTE_TR_MSG			0xF034
#define FXC_PASTE_TAKE_MSG			0xF035
#define FXC_PASTE_ALLTAKES_MSG		0xF036
#define FXC_AUTOSAVE_INPUTFX_MSG	0xF037
#define FXC_AUTOSAVE_TR_MSG			0xF038
#define FXC_AUTOSAVE_ITEM_MSG		0xF039
#define FXC_AUTOSAVE_DEFNAME_MSG	0xF03A
#define FXC_AUTOSAVE_FX1NAME_MSG	0xF03B
// track template cmds
#define TRT_APPLY_MSG				0xF040
#define TRT_APPLY_WITH_ENV_ITEM_MSG	0xF041
#define TRT_IMPORT_MSG				0xF042
#define TRT_REPLACE_ITEMS_MSG		0xF043
#define TRT_PASTE_ITEMS_MSG			0xF044
#define TRT_AUTOSAVE_WITEMS_MSG		0xF045
#define TRT_AUTOSAVE_WENVS_MSG		0xF046
// project template cmds
#define PRJ_OPEN_SELECT_MSG			0xF050
#define PRJ_OPEN_SELECT_TAB_MSG		0xF051
#define PRJ_AUTOFILL_RECENTS_MSG	0xF052
#define PRJ_LOADER_CONF_MSG			0xF053
#define PRJ_LOADER_SET_MSG			0xF054
#define PRJ_LOADER_CLEAR_MSG		0xF055
// media file cmds
#define MED_PLAY_MSG				0xF060
#define MED_LOOP_MSG				0xF061
#define MED_ADD_CURTR_MSG			0xF062
#define MED_ADD_NEWTR_MSG			0xF063
#define MED_ADD_TAKES_MSG			0xF064
#define MED_AUTOSAVE_MSG			0xF065
// image file cmds
#define IMG_SHOW_MSG				0xF070 
#define IMG_TRICON_MSG				0xF071
#ifdef _WIN32
// theme file cmds
#define THM_LOAD_MSG				0xF080 
#endif

// labels for undo points and popup menu items
#define FXC_APPLY_TR_STR			__LOCALIZE("Paste (replace) FX chain to selected tracks","sws_DLG_150")
#define FXCIN_APPLY_TR_STR			__LOCALIZE("Paste (replace) input FX chain to selected tracks","sws_DLG_150")
#define FXC_APPLY_TAKE_STR			__LOCALIZE("Paste (replace) FX chain to selected items","sws_DLG_150")
#define FXC_APPLY_ALLTAKES_STR		__LOCALIZE("Paste (replace) FX chain to selected items, all takes","sws_DLG_150")
#define FXC_PASTE_TR_STR			__LOCALIZE("Paste FX chain to selected tracks","sws_DLG_150")
#define FXCIN_PASTE_TR_STR			__LOCALIZE("Paste input FX chain to selected tracks","sws_DLG_150")
#define FXC_PASTE_TAKE_STR			__LOCALIZE("Paste FX chain to selected items","sws_DLG_150")
#define FXC_PASTE_ALLTAKES_STR		__LOCALIZE("Paste FX chain to selected items, all takes","sws_DLG_150")
#define TRT_APPLY_STR				__LOCALIZE("Apply track template to selected tracks","sws_DLG_150")
#define TRT_APPLY_WITH_ENV_ITEM_STR	__LOCALIZE("Apply track template (+items/envelopes) to selected tracks","sws_DLG_150")
#define TRT_IMPORT_STR				__LOCALIZE("Import tracks from track template","sws_DLG_150")
#define TRT_REPLACE_ITEMS_STR		__LOCALIZE("Paste (replace) template items to selected tracks","sws_DLG_150")
#define TRT_PASTE_ITEMS_STR			__LOCALIZE("Paste template items to selected tracks","sws_DLG_150")
#define PRJ_SELECT_LOAD_STR			__LOCALIZE("Open/select project","sws_DLG_150")
#define PRJ_SELECT_LOAD_TAB_STR		__LOCALIZE("Open/select project (new tab)","sws_DLG_150")
#define MED_PLAY_STR				__LOCALIZE("Play media file in selected tracks (toggle)","sws_DLG_150")
#define MED_LOOP_STR				__LOCALIZE("Loop media file in selected tracks (toggle)","sws_DLG_150")
#define MED_PLAYLOOP_STR			__LOCALIZE("Play/loop media file","sws_DLG_150")
#define MED_ADD_STR					__LOCALIZE("Insert media","sws_DLG_150")
#define IMG_SHOW_STR				__LOCALIZE("Show image","sws_DLG_150")
#define IMG_TRICON_STR				__LOCALIZE("Set track icon for selected tracks","sws_DLG_150")
#ifdef _WIN32
#define THM_LOAD_STR				__LOCALIZE("Load theme","sws_DLG_150")
#endif

// no default filter text on OSX (cannot catch EN_SETFOCUS/EN_KILLFOCUS)
#ifndef _SNM_SWELL_ISSUES
#define FILTER_DEFAULT_STR		__LOCALIZE("Filter","sws_DLG_150")
#else
#define FILTER_DEFAULT_STR		""
#endif
#define DRAGDROP_EMPTY_SLOT		">Empty<" // no localization: internal stuff
#define AUTOSAVE_ERR_STR		__LOCALIZE("Probable cause: no selection, nothing to save, cannot write file, file in use, invalid filename, etc...","sws_DLG_150")
#define AUTOFILL_ERR_STR		__LOCALIZE("Probable cause: all files are already present, empty directory, directory not found, etc...","sws_DLG_150")
#define NO_SLOT_ERR_STR			__LOCALIZE("No %s slot defined in the Resources view!","sws_DLG_150")

enum {
  BTNID_AUTOFILL=2000, //JFB would be great to have _APS_NEXT_CONTROL_VALUE *always* defined
  BTNID_AUTOSAVE,
  TXTID_TYPE,
  CMBID_TYPE,
  WNDID_ADD_DEL,
  BTNID_ADD_BOOKMARK,
  BTNID_DEL_BOOKMARK,
#ifdef _SNM_MISC // moved to context menu
  BTNID_TIED_ACTIONS,
#endif
  BTNID_OFFSET_TR_TEMPLATE,
  TXTID_DBL_TYPE,
  CMBID_DBLCLICK_TYPE,
  TXTID_DBL_TO,
  CMBID_DBLCLICK_TO
};

enum {
  COL_SLOT=0,
  COL_NAME,
  COL_PATH,
  COL_COMMENT,
  COL_COUNT
};

SNM_ResourceWnd* g_SNM_ResourcesWnd = NULL;

// JFB important notes:
// all global WDL_PtrList vars used to be WDL_PtrList_DeleteOnDestroy ones but
// something weird could occur when REAPER unloads the extension: hang or crash 
// (e.g. issues 292 & 380) on Windows 7 while saving ini files (those lists were 
// already unallocated..)
// slots lists are allocated on the heap for the same reason..
// anyway, no prob here because application exit will destroy the entire runtime 
// context regardless.

WDL_PtrList<PathSlotItem> g_dragPathSlotItems; 
WDL_PtrList<FileSlotList> g_SNM_ResSlots;

// prefs
int g_resViewType = -1;
int g_SNM_TiedSlotActions[SNM_NUM_DEFAULT_SLOTS]; // slot actions of default type/idx are tied to type/value
int g_dblClickPrefs[SNM_MAX_SLOT_TYPES]; // flags, loword: dbl-click type, hiword: dbl-click to (only for fx chains, atm)
WDL_FastString g_filter; // see init + localization in ResourcesInit()
int g_filterPref = 1; // bitmask: &1 = filter by name, &2 = filter by path, &4 = filter by comment
WDL_PtrList<WDL_FastString> g_autoSaveDirs;
WDL_PtrList<WDL_FastString> g_autoFillDirs;
bool g_syncAutoDirPrefs[SNM_MAX_SLOT_TYPES];
int g_SNM_PrjLoaderStartPref = -1; // 1-based
int g_SNM_PrjLoaderEndPref = -1; // 1-based

// auto-save prefs
int g_asTrTmpltPref = 3; // &1: with items, &2: with envs
int g_asFXChainPref = FXC_AUTOSAVE_PREF_TRACK;
int g_asFXChainNamePref = 0;

// helper funcs
FileSlotList* GetSlotList(int _type = -1) {
	if (_type < 0) _type = g_resViewType;
	return g_SNM_ResSlots.Get(_type);
}

const char* GetAutoSaveDir(int _type = -1) {
	if (_type < 0) _type = g_resViewType;
	return g_autoSaveDirs.Get(_type)->Get();
}

void SetAutoSaveDir(const char* _path, int _type = -1) {
	if (_type < 0) _type = g_resViewType;
	g_autoSaveDirs.Get(_type)->Set(_path);
	if (g_syncAutoDirPrefs[_type])
		g_autoFillDirs.Get(_type)->Set(_path);
}

const char* GetAutoFillDir(int _type = -1) {
	if (_type < 0) _type = g_resViewType;
	return g_autoFillDirs.Get(_type)->Get();
}

void SetAutoFillDir(const char* _path, int _type = -1) {
	if (_type < 0) _type = g_resViewType;
	g_autoFillDirs.Get(_type)->Set(_path);
	if (g_syncAutoDirPrefs[_type])
		g_autoSaveDirs.Get(_type)->Set(_path);
}

int GetTypeForUser(int _type = -1) {
	if (_type < 0) _type = g_resViewType;
	if (_type >= SNM_NUM_DEFAULT_SLOTS)
		for (int i=0; i < SNM_NUM_DEFAULT_SLOTS; i++)
			if (!_stricmp(g_SNM_ResSlots.Get(_type)->GetFileExt(), g_SNM_ResSlots.Get(i)->GetFileExt()))
				return i;
	return _type;
}

int GetTypeFromSlotList(FileSlotList* _l) {
	for (int i=0; i < g_SNM_ResSlots.GetSize(); i++)
		if (g_SNM_ResSlots.Get(i) == _l)
			return i;
	return -1;
}

bool IsMultiType(int _type = -1) {
	if (_type < 0) _type = g_resViewType;
	for (int i=0; i < g_SNM_ResSlots.GetSize(); i++)
		if (i != _type && GetTypeForUser(i) == GetTypeForUser(_type))
			return true;
	return false;
}

void GetIniSectionName(int _type, char* _bufOut, size_t _bufOutSz) {
	if (_type >= SNM_NUM_DEFAULT_SLOTS) {
		*_bufOut = '\0';
		_snprintfSafe(_bufOut, _bufOutSz, "CustomSlotType%d", _type-SNM_NUM_DEFAULT_SLOTS+1);
		return;
	}
	// relative path needed, for ex: data\track_icons
	lstrcpyn(_bufOut, GetFileRelativePath(g_SNM_ResSlots.Get(_type)->GetResourceDir()), _bufOutSz);
	*_bufOut = toupper(*_bufOut);
}

// it is up to the caller to unalloc.. 
void GetIniSectionNames(WDL_PtrList<WDL_FastString>* _iniSections) {
	char iniSec[64]="";
	for (int i=0; i < g_SNM_ResSlots.GetSize(); i++) {
		GetIniSectionName(i, iniSec, sizeof(iniSec));
		_iniSections->Add(new WDL_FastString(iniSec));
	}
}

bool IsFiltered() {
	return (g_filter.GetLength() && strcmp(g_filter.Get(), FILTER_DEFAULT_STR));
}


///////////////////////////////////////////////////////////////////////////////
// FileSlotList
///////////////////////////////////////////////////////////////////////////////

// _path: short resource path or full path
PathSlotItem* FileSlotList::AddSlot(const char* _path, const char* _desc) {
	char shortPath[SNM_MAX_PATH] = "";
	GetShortResourcePath(m_resDir.Get(), _path, shortPath, sizeof(shortPath));
	return Add(new PathSlotItem(shortPath, _desc));
}

// _path: short resource path or full path
PathSlotItem* FileSlotList::InsertSlot(int _slot, const char* _path, const char* _desc) {
	PathSlotItem* item = NULL;
	char shortPath[SNM_MAX_PATH] = "";
	GetShortResourcePath(m_resDir.Get(), _path, shortPath, sizeof(shortPath));
	if (_slot >=0 && _slot < GetSize()) item = Insert(_slot, new PathSlotItem(shortPath, _desc));
	else item = Add(new PathSlotItem(shortPath, _desc));
	return item;
}

int FileSlotList::FindByResFulltPath(const char* _resFullPath) {
	if (_resFullPath)
		for (int i=0; i < GetSize(); i++)
			if (PathSlotItem* item = Get(i))
				if (item->m_shortPath.GetLength() && strstr(_resFullPath, item->m_shortPath.Get())) // no stristr: osx + utf-8
					return i;
	return -1;
}

bool FileSlotList::GetFullPath(int _slot, char* _fullFn, int _fullFnSz) {
	if (PathSlotItem* item = Get(_slot)) {
		GetFullResourcePath(m_resDir.Get(), item->m_shortPath.Get(), _fullFn, _fullFnSz);
		return true;
	}
	return false;
}

bool FileSlotList::SetFromFullPath(int _slot, const char* _fullPath) {
	if (PathSlotItem* item = Get(_slot)) {
		char shortPath[SNM_MAX_PATH] = "";
		GetShortResourcePath(m_resDir.Get(), _fullPath, shortPath, sizeof(shortPath));
		item->m_shortPath.Set(shortPath);
		return true;
	}
	return false;
}

bool FileSlotList::IsValidFileExt(const char* _ext) {
	if (!_ext) return false;
	if (!m_ext.GetLength()) return IsMediaExtension(_ext, false);
	return (_stricmp(_ext, m_ext.Get()) == 0);
}

void FileSlotList::GetFileFilter(char* _filter, size_t _filterSz) {
	if (!_filter) return;
	if (m_ext.GetLength()) {
		if (_snprintfStrict(_filter, _filterSz, "REAPER %s (*.%s)X*.%s", m_desc.Get(), m_ext.Get(), m_ext.Get()) > 0) {
			if (char* p = strchr(_filter, ')')) // special code for multiple null terminated strings ('X' -> '\0')
				*(p+1) = '\0';
		}
		else
			lstrcpyn(_filter, "\0\0", _filterSz);
	}
	else
		memcpy(_filter, plugin_getFilterList(), _filterSz); // memcpy because of '\0'
}

void FileSlotList::ClearSlot(int _slot, bool _guiUpdate) {
	if (_slot >=0 && _slot < GetSize())	{
		Get(_slot)->Clear();
		if (_guiUpdate && g_SNM_ResourcesWnd)
			g_SNM_ResourcesWnd->Update();
	}
}

void FileSlotList::ClearSlotPrompt(COMMAND_T* _ct)
{
	if (!GetSize()) {
		WDL_FastString msg; msg.SetFormatted(128, NO_SLOT_ERR_STR, g_SNM_ResSlots.Get(GetTypeForUser(GetTypeFromSlotList(this)))->GetDesc());
		MessageBox(GetMainHwnd(), msg.Get(), __LOCALIZE("S&M - Error","sws_DLG_150"), MB_OK);
		return;
	}
	int slot = PromptForInteger(SWS_CMD_SHORTNAME(_ct), "Slot", 1, GetSize()); //loops on err
	if (slot == -1) return; // user has cancelled
	else ClearSlot(slot);
}

// returns false if cancelled
bool FileSlotList::BrowseSlot(int _slot, char* _fn, int _fnSz)
{
	bool ok = false;
	if (_slot >= 0 && _slot < GetSize())
	{
		char title[128]="", fileFilter[512]="", defaultPath[SNM_MAX_PATH]="";
		_snprintfSafe(title, sizeof(title), __LOCALIZE_VERFMT("S&M - Load %s (slot %d)","sws_DLG_150"), m_desc.Get(), _slot+1);
		if (_snprintfStrict(defaultPath, sizeof(defaultPath), "%s%c%s", GetResourcePath(), PATH_SLASH_CHAR, m_resDir.Get()) <= 0)
			*defaultPath = '\0';
		GetFileFilter(fileFilter, sizeof(fileFilter));

		int type = GetTypeFromSlotList(this);
		const char* path = type>0 ? GetAutoFillDir(type) : "";
		if (char* fn = BrowseForFiles(title, m_lastBrowsedFn.GetLength()?m_lastBrowsedFn.Get():(*path?path:defaultPath), NULL, false, fileFilter))
		{
			if (_fn)
				lstrcpyn(_fn, fn, _fnSz);
			if (SetFromFullPath(_slot, fn)) {
				if (g_SNM_ResourcesWnd)
					g_SNM_ResourcesWnd->Update();
				ok = true;
			}
			m_lastBrowsedFn.Set(fn);
			free(fn);
		}
	}
	return ok;
}

bool FileSlotList::GetOrBrowseSlot(int _slot, char* _fn, int _fnSz, bool _errMsg)
{
	bool ok = false;
	if (_slot >= 0 && _slot < GetSize())
	{
		if (Get(_slot)->IsDefault())
			ok = BrowseSlot(_slot, _fn, _fnSz);
		else if (GetFullPath(_slot, _fn, _fnSz))
			ok = FileOrDirExistsErrMsg(_fn, _errMsg);
	}
	return ok;
}

// returns NULL if failed, otherwise it's up to the caller to free the returned string
// _slot: in/out param
WDL_FastString* FileSlotList::GetOrPromptOrBrowseSlot(const char* _title, int* _slot)
{
	// prompt for slot if needed
	if (*_slot == -1)
	{
		if (GetSize())
			*_slot = PromptForInteger(_title, "Slot", 1, GetSize()); // loops on err
		else {
			WDL_FastString msg; msg.SetFormatted(128, NO_SLOT_ERR_STR, g_SNM_ResSlots.Get(GetTypeForUser(GetTypeFromSlotList(this)))->GetDesc());
			MessageBox(GetMainHwnd(), msg.Get(), __LOCALIZE("S&M - Error","sws_DLG_150"), MB_OK);
		}
	}
	if (*_slot == -1) // user has cancelled or empty
		return NULL; 

	// adds the needed number of slots (more macro friendly)
	if (*_slot >= GetSize())
	{
		for (int i=GetSize(); i <= *_slot; i++)
			Add(new PathSlotItem());
		if (g_SNM_ResourcesWnd) {
			g_SNM_ResourcesWnd->Update();
			g_SNM_ResourcesWnd->SelectBySlot(*_slot);
		}
	}

	if (*_slot < GetSize())
	{
		char fn[SNM_MAX_PATH]="";
		if (GetOrBrowseSlot(*_slot, fn, sizeof(fn), *_slot < 0 || !Get(*_slot)->IsDefault()))
			return new WDL_FastString(fn);
	}
	*_slot = -1;
	return NULL;
}

void FileSlotList::EditSlot(int _slot)
{
	if (_slot >= 0 && _slot < GetSize())
	{
		char fullPath[SNM_MAX_PATH] = "";
		if (GetFullPath(_slot, fullPath, sizeof(fullPath)) && FileOrDirExistsErrMsg(fullPath))
		{
#ifdef _WIN32
			WinSpawnNotepad(fullPath);
#else
			WDL_FastString chain;
			if (LoadChunk(fullPath, &chain, false))
			{
				char title[64] = "";
				_snprintfSafe(title, sizeof(title), __LOCALIZE_VERFMT("S&M - %s (slot %d)","sws_DLG_150"), m_desc.Get(), _slot+1);
				SNM_ShowMsg(chain.Get(), title);
			}
#endif
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
// SNM_ResourceView
///////////////////////////////////////////////////////////////////////////////

// !WANT_LOCALIZE_STRINGS_BEGIN:sws_DLG_150
static SWS_LVColumn s_resListCols[] = { {65,2,"Slot"}, {100,1,"Name"}, {250,2,"Path"}, {200,1,"Comment"} };
// !WANT_LOCALIZE_STRINGS_END

SNM_ResourceView::SNM_ResourceView(HWND hwndList, HWND hwndEdit)
	: SWS_ListView(hwndList, hwndEdit, COL_COUNT, s_resListCols, "ResourcesViewState", false, "sws_DLG_150")
{
}

void SNM_ResourceView::GetItemText(SWS_ListItem* item, int iCol, char* str, int iStrMax)
{
	if (str) *str = '\0';
	if (PathSlotItem* pItem = (PathSlotItem*)item)
	{
		switch (iCol)
		{
			case COL_SLOT:
			{
				int slot = GetSlotList()->Find(pItem);
				if (slot >= 0)
				{
					slot++;
					if (g_resViewType == SNM_SLOT_PRJ && IsProjectLoaderConfValid()) // no GetTypeForUser() here: only one loader/selecter config atm..
						_snprintfSafe(str, iStrMax, "%5.d %s", slot, 
							slot<g_SNM_PrjLoaderStartPref || slot>g_SNM_PrjLoaderEndPref ? "  " : 
							g_SNM_PrjLoaderStartPref==slot ? "->" : g_SNM_PrjLoaderEndPref==slot ? "<-" :  "--");
					else
						_snprintfSafe(str, iStrMax, "%5.d", slot);
				}
				break;
			}
			case COL_NAME:
				GetFilenameNoExt(pItem->m_shortPath.Get(), str, iStrMax);
				break;
			case COL_PATH:
				lstrcpyn(str, pItem->m_shortPath.Get(), iStrMax);
				break;
			case COL_COMMENT:
				lstrcpyn(str, pItem->m_comment.Get(), iStrMax);
				break;
		}
	}
}

bool SNM_ResourceView::IsEditListItemAllowed(SWS_ListItem* item, int iCol)
{
	if (PathSlotItem* pItem = (PathSlotItem*)item)
	{
		if (!pItem->IsDefault())
		{
			int slot = GetSlotList()->Find(pItem);
			if (slot >= 0)
			{
				switch (iCol) {
					case COL_NAME: { // file renaming
						char fn[SNM_MAX_PATH] = "";
						return (GetSlotList()->GetFullPath(slot, fn, sizeof(fn)) && FileOrDirExists(fn));
					}
					case COL_COMMENT:
						return true;
				}
			}
		}
	}
	return false;
}

void SNM_ResourceView::SetItemText(SWS_ListItem* item, int iCol, const char* str)
{
	PathSlotItem* pItem = (PathSlotItem*)item;
	int slot = GetSlotList()->Find(pItem);
	if (pItem && slot >=0)
	{
		switch (iCol)
		{
			case COL_NAME: // file renaming
			{
				if (!IsValidFilenameErrMsg(str, true))
					return;

				char fn[SNM_MAX_PATH] = "";
				if (GetSlotList()->GetFullPath(slot, fn, sizeof(fn)) && !pItem->IsDefault() && FileOrDirExistsErrMsg(fn))
				{
					const char* ext = GetFileExtension(fn);
					char newFn[SNM_MAX_PATH]="", path[SNM_MAX_PATH]="";
					lstrcpyn(path, fn, sizeof(path));
					if (char* p = strrchr(path, PATH_SLASH_CHAR))
						*p = '\0';
					else
						break; // safety

					if (_snprintfStrict(newFn, sizeof(newFn), "%s%c%s.%s", path, PATH_SLASH_CHAR, str, ext) > 0)
					{
						if (FileOrDirExists(newFn)) 
						{
							char msg[SNM_MAX_PATH]="";
							_snprintfSafe(msg, sizeof(msg), __LOCALIZE_VERFMT("File already exists. Overwrite ?\n%s","sws_mbox"), newFn);
							int res = MessageBox(g_SNM_ResourcesWnd?g_SNM_ResourcesWnd->GetHWND():GetMainHwnd(), msg, __LOCALIZE("S&M - Warning","sws_DLG_150"), MB_YESNO);
							if (res == IDYES) {
								if (!SNM_DeleteFile(newFn, false))
									break;
							}
							else
								break;
						}

						if (MoveFile(fn, newFn) && GetSlotList()->SetFromFullPath(slot, newFn))
							ListView_SetItemText(m_hwndList, GetEditingItem(), DisplayToDataCol(2), (LPSTR)pItem->m_shortPath.Get());
							// ^^ direct GUI update 'cause Update() is no-op when editing
					}
				}
				break;
			}
			case COL_COMMENT:
				pItem->m_comment.Set(str);
				pItem->m_comment.Ellipsize(128,128);
				Update();
				break;
		}
	}
}

void SNM_ResourceView::OnItemDblClk(SWS_ListItem* item, int iCol) {
	Perform();
}

void SNM_ResourceView::GetItemList(SWS_ListItemList* pList)
{
	if (IsFiltered())
	{
		char buf[SNM_MAX_PATH] = "";
		LineParser lp(false);
		if (!lp.parse(g_filter.Get()))
		{
			for (int i=0; i < GetSlotList()->GetSize(); i++)
			{
				if (PathSlotItem* item = GetSlotList()->Get(i))
				{
					bool match = false;
					for (int j=0; !match && j < lp.getnumtokens(); j++)
					{
						if (g_filterPref&1) // name
						{
							GetFilenameNoExt(item->m_shortPath.Get(), buf, sizeof(buf));
							match |= (stristr(buf, lp.gettoken_str(j)) != NULL);
						}
						if (!match && (g_filterPref&2)) // path
						{
							if (GetSlotList()->GetFullPath(i, buf, sizeof(buf)))
								if (char* p = strrchr(buf, PATH_SLASH_CHAR)) {
									*p = '\0';
									match |= (stristr(buf, lp.gettoken_str(j)) != NULL);
								}
						}
						if (!match && (g_filterPref&4)) // comment
							match |= (stristr(item->m_comment.Get(), lp.gettoken_str(j)) != NULL);
					}
					if (match)
						pList->Add((SWS_ListItem*)item);
				}
			}
		}
	}
	else
	{
		for (int i=0; i < GetSlotList()->GetSize(); i++)
			pList->Add((SWS_ListItem*)GetSlotList()->Get(i));
	}
}

void SNM_ResourceView::OnBeginDrag(SWS_ListItem* _item)
{
#ifdef _WIN32
	g_dragPathSlotItems.Empty(false);

	// store dragged items (for internal d'n'd) + full paths + get the amount of memory needed
	int iMemNeeded = 0, x=0;
	WDL_PtrList_DeleteOnDestroy<WDL_FastString> fullPaths;
	while (PathSlotItem* pItem = (PathSlotItem*)EnumSelected(&x))
	{
		int slot = GetSlotList()->Find(pItem);
		char fullPath[SNM_MAX_PATH] = "";
		if (GetSlotList()->GetFullPath(slot, fullPath, sizeof(fullPath)))
		{
			bool empty = (pItem->IsDefault() || *fullPath == '\0');
			iMemNeeded += (int)((empty ? strlen(DRAGDROP_EMPTY_SLOT) : strlen(fullPath)) + 1);
			fullPaths.Add(new WDL_FastString(empty ? DRAGDROP_EMPTY_SLOT : fullPath));
			g_dragPathSlotItems.Add(pItem);
		}
	}
	if (!iMemNeeded)
		return;

	iMemNeeded += sizeof(DROPFILES) + 1;

	HGLOBAL hgDrop = GlobalAlloc (GHND | GMEM_SHARE, iMemNeeded);
	DROPFILES* pDrop = NULL;
	if (hgDrop)
		pDrop = (DROPFILES*)GlobalLock(hgDrop); // 'spose should do some error checking...
	
	// for safety..
	if (!hgDrop || !pDrop)
		return;

	pDrop->pFiles = sizeof(DROPFILES);
	pDrop->fWide = false;
	char* pBuf = (char*)pDrop + pDrop->pFiles;

	// add files to the DROPFILES struct, double-NULL terminated
	for (int i=0; i < fullPaths.GetSize(); i++) {
		strcpy(pBuf, fullPaths.Get(i)->Get());
		pBuf += strlen(pBuf) + 1;
	}
	*pBuf = 0;

	GlobalUnlock(hgDrop);
	FORMATETC etc = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
	STGMEDIUM stgmed = { TYMED_HGLOBAL, { 0 }, 0 };
	stgmed.hGlobal = hgDrop;

	SWS_IDataObject* dataObj = new SWS_IDataObject(&etc, &stgmed);
	SWS_IDropSource* dropSrc = new SWS_IDropSource;
	DWORD effect;

	DoDragDrop(dataObj, dropSrc, DROPEFFECT_COPY, &effect);
#endif
}

void SNM_ResourceView::Perform()
{
	int x=0;
	PathSlotItem* pItem = (PathSlotItem*)EnumSelected(&x);
	int slot = GetSlotList()->Find(pItem);

	if (pItem && slot >= 0) 
	{
		bool wasDefaultSlot = pItem->IsDefault();
		int dblClickType = LOWORD(g_dblClickPrefs[g_resViewType]);
		int dblClickTo = HIWORD(g_dblClickPrefs[g_resViewType]);
		switch(GetTypeForUser())
		{
			case SNM_SLOT_FXC:
				switch(dblClickTo)
				{
					case 0:
						ApplyTracksFXChainSlot(g_resViewType, !dblClickType?FXC_APPLY_TR_STR:FXC_PASTE_TR_STR, slot, !dblClickType, false);
						break;
					case 1:
						ApplyTracksFXChainSlot(g_resViewType, !dblClickType?FXCIN_APPLY_TR_STR:FXCIN_PASTE_TR_STR, slot, !dblClickType, true);
						break;
					case 2:
						ApplyTakesFXChainSlot(g_resViewType, !dblClickType?FXC_APPLY_TAKE_STR:FXC_PASTE_TAKE_STR, slot, true, !dblClickType);
						break;
					case 3:
						ApplyTakesFXChainSlot(g_resViewType, !dblClickType?FXC_APPLY_ALLTAKES_STR:FXC_PASTE_ALLTAKES_STR, slot, false, !dblClickType);
						break;
				}
				break;
			case SNM_SLOT_TR:
				switch(dblClickType)
				{
					case 0:
						ImportTrackTemplateSlot(g_resViewType, TRT_IMPORT_STR, slot);
						break;
					case 1:
						ApplyTrackTemplateSlot(g_resViewType, TRT_APPLY_STR, slot, false, false);
						break;
					case 2:
						ApplyTrackTemplateSlot(g_resViewType, TRT_APPLY_WITH_ENV_ITEM_STR, slot, true, true);
						break;
					case 3:
						ReplacePasteItemsTrackTemplateSlot(g_resViewType, TRT_PASTE_ITEMS_STR, slot, true);
						break;
					case 4:
						ReplacePasteItemsTrackTemplateSlot(g_resViewType, TRT_REPLACE_ITEMS_STR, slot, false);
						break;
				}
				break;
			case SNM_SLOT_PRJ:
				switch(dblClickType)
				{
					case 0:
						LoadOrSelectProjectSlot(g_resViewType, PRJ_SELECT_LOAD_STR, slot, false);
						break;
					case 1:
						LoadOrSelectProjectSlot(g_resViewType, PRJ_SELECT_LOAD_TAB_STR, slot, true);
						break;
				}
				break;
			case SNM_SLOT_MEDIA:
			{
				int insertMode = -1;
				switch(dblClickType) 
				{
					case 0: TogglePlaySelTrackMediaSlot(g_resViewType, MED_PLAY_STR, slot, false, false); break;
					case 1: TogglePlaySelTrackMediaSlot(g_resViewType, MED_LOOP_STR, slot, false, true); break;
					case 2: insertMode = 0; break;
					case 3: insertMode = 1; break;
					case 4: insertMode = 3; break;
				}
				if (insertMode >= 0)
					InsertMediaSlot(g_resViewType, MED_ADD_STR, slot, insertMode);
				break;
			}
			case SNM_SLOT_IMG:
				switch(dblClickType) {
					case 0: ShowImageSlot(g_resViewType, IMG_SHOW_STR, slot); break;
					case 1: SetSelTrackIconSlot(g_resViewType, IMG_TRICON_STR, slot); break;
					case 2: InsertMediaSlot(g_resViewType, MED_ADD_STR, slot, 0); break;
				}
				break;
#ifdef _WIN32
	 		case SNM_SLOT_THM:
				LoadThemeSlot(g_resViewType, THM_LOAD_STR, slot);
				break;
#endif
		}

		// update if the slot changed
		if (wasDefaultSlot && !pItem->IsDefault())
			Update();
	}
}


///////////////////////////////////////////////////////////////////////////////
// SNM_ResourceWnd
///////////////////////////////////////////////////////////////////////////////

SNM_ResourceWnd::SNM_ResourceWnd()
	: SWS_DockWnd(IDD_SNM_RESOURCES, __LOCALIZE("Resources","sws_DLG_150"), "SnMResources", SWSGetCommandID(OpenResources))
{
	// Must call SWS_DockWnd::Init() to restore parameters and open the window if necessary
	Init();
}

void SNM_ResourceWnd::OnInitDlg()
{
	m_resize.init_item(IDC_LIST, 0.0, 0.0, 1.0, 1.0);
	m_resize.init_item(IDC_FILTER, 1.0, 1.0, 1.0, 1.0);

	SetWindowLongPtr(GetDlgItem(m_hwnd, IDC_FILTER), GWLP_USERDATA, 0xdeadf00b);
	SetWindowLongPtr(GetDlgItem(m_hwnd, IDC_EDIT), GWLP_USERDATA, 0xdeadf00b);

	m_pLists.Add(new SNM_ResourceView(GetDlgItem(m_hwnd, IDC_LIST), GetDlgItem(m_hwnd, IDC_EDIT)));

	m_vwnd_painter.SetGSC(WDL_STYLE_GetSysColor);
	m_parentVwnd.SetRealParent(m_hwnd);

	m_txtSlotsType.SetID(TXTID_TYPE);
	m_txtSlotsType.SetText(__LOCALIZE("Slots type:","sws_DLG_150"));
	m_parentVwnd.AddChild(&m_txtSlotsType);

	m_cbType.SetID(CMBID_TYPE);
	FillTypeCombo();
	m_parentVwnd.AddChild(&m_cbType);

	m_cbDblClickType.SetID(CMBID_DBLCLICK_TYPE);
	m_parentVwnd.AddChild(&m_cbDblClickType);
	m_cbDblClickTo.SetID(CMBID_DBLCLICK_TO);
	m_parentVwnd.AddChild(&m_cbDblClickTo);
	FillDblClickCombos();

	m_btnAutoFill.SetID(BTNID_AUTOFILL);
	m_parentVwnd.AddChild(&m_btnAutoFill);

	m_btnAutoSave.SetID(BTNID_AUTOSAVE);
	m_parentVwnd.AddChild(&m_btnAutoSave);

	m_txtDblClickType.SetID(TXTID_DBL_TYPE);
	m_txtDblClickType.SetText(__LOCALIZE("Dbl-click to:","sws_DLG_150"));
	m_parentVwnd.AddChild(&m_txtDblClickType);

	m_btnAdd.SetID(BTNID_ADD_BOOKMARK);
	m_btnsAddDel.AddChild(&m_btnAdd);
	m_btnDel.SetID(BTNID_DEL_BOOKMARK);
	m_btnsAddDel.AddChild(&m_btnDel);
	m_btnsAddDel.SetID(WNDID_ADD_DEL);
	m_parentVwnd.AddChild(&m_btnsAddDel);

#ifdef _SNM_MISC // moved to context menu
	m_btnTiedActions.SetID(BTNID_TIED_ACTIONS);
	m_parentVwnd.AddChild(&m_btnTiedActions);
#endif

	m_txtDblClickTo.SetID(TXTID_DBL_TO);
	m_txtDblClickTo.SetText(__LOCALIZE("To sel.:","sws_DLG_150"));
	m_parentVwnd.AddChild(&m_txtDblClickTo);

	m_btnOffsetTrTemplate.SetID(BTNID_OFFSET_TR_TEMPLATE);
	m_parentVwnd.AddChild(&m_btnOffsetTrTemplate);

	// restores the text filter when docking/undocking + indirect call to Update()
	SetDlgItemText(m_hwnd, IDC_FILTER, g_filter.Get());

/* see above comment
	Update();
*/
}

void SNM_ResourceWnd::OnDestroy() 
{
	m_cbType.Empty();
	m_cbDblClickType.Empty();
	m_cbDblClickTo.Empty();
	m_btnsAddDel.RemoveAllChildren(false);
}

void SNM_ResourceWnd::SetType(int _type)
{
	int prevType = g_resViewType;
	g_resViewType = _type;
	m_cbType.SetCurSel(_type);
	if (prevType != g_resViewType) {
		FillDblClickCombos();
		Update();
	}
}

int SNM_ResourceWnd::SetType(const char* _name)
{
	if (_name)
		for (int i=0; i<m_cbType.GetCount(); i++)
			if (!_stricmp(_name, m_cbType.GetItem(i))) {
				SetType(i);
				return i;
			}
	return -1;
}

void SNM_ResourceWnd::Update()
{
	if (m_pLists.GetSize())
		m_pLists.Get(0)->Update();
	m_parentVwnd.RequestRedraw(NULL);
}

void SNM_ResourceWnd::FillTypeCombo()
{
	m_cbType.Empty();
	for (int i=0; i < g_SNM_ResSlots.GetSize(); i++)
		if (char* p = _strdup(g_SNM_ResSlots.Get(i)->GetDesc())) {
			*p = toupper(*p); // 1st char to upper
			m_cbType.AddItem(p);
			free(p);
		}
	m_cbType.SetCurSel((g_resViewType>0 && g_resViewType<g_SNM_ResSlots.GetSize()) ? g_resViewType : SNM_SLOT_FXC);
}

void SNM_ResourceWnd::FillDblClickCombos()
{
	int typeForUser = GetTypeForUser();
	m_cbDblClickType.Empty();
	m_cbDblClickTo.Empty();
	switch(typeForUser)
	{
		case SNM_SLOT_FXC:
			m_cbDblClickType.AddItem(__LOCALIZE("Paste (replace)","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Paste","sws_DLG_150"));

			m_cbDblClickTo.AddItem(__LOCALIZE("Tracks","sws_DLG_150"));
			m_cbDblClickTo.AddItem(__LOCALIZE("Tracks (input FX)","sws_DLG_150"));
			m_cbDblClickTo.AddItem(__LOCALIZE("Items","sws_DLG_150"));
			m_cbDblClickTo.AddItem(__LOCALIZE("Items (all takes)","sws_DLG_150"));
			break;
		case SNM_SLOT_TR:
			m_cbDblClickType.AddItem(__LOCALIZE("Import tracks from track template","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Apply template to sel tracks","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Apply template to sel tracks (+items/envs)","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Paste template items to sel tracks","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Paste (replace) template items to sel tracks","sws_DLG_150"));
			break;
		case SNM_SLOT_PRJ:
			m_cbDblClickType.AddItem(__LOCALIZE("Open/select project","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Open/select project tab","sws_DLG_150"));
			break;
		case SNM_SLOT_MEDIA:
			m_cbDblClickType.AddItem(__LOCALIZE("Play in sel tracks (toggle)","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Loop in sel tracks (toggle)","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Add to current track","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Add to new track","sws_DLG_150"));
			m_cbDblClickType.AddItem(__LOCALIZE("Add to sel items as takes","sws_DLG_150"));
			break;
		case SNM_SLOT_IMG:
			m_cbDblClickType.AddItem(IMG_SHOW_STR);
			m_cbDblClickType.AddItem(IMG_TRICON_STR);
			m_cbDblClickType.AddItem(__LOCALIZE("Add to current track as item","sws_DLG_150"));
			break;
#ifdef _WIN32
		case SNM_SLOT_THM:
			m_cbDblClickType.AddItem(__LOCALIZE("Load theme","sws_DLG_150"));
			break;
#endif
	}
	m_cbDblClickType.SetCurSel(LOWORD(g_dblClickPrefs[g_resViewType]));
	m_cbDblClickTo.SetCurSel(HIWORD(g_dblClickPrefs[g_resViewType]));
}

void SNM_ResourceWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	int x=0;
	PathSlotItem* item = (PathSlotItem*)m_pLists.Get(0)->EnumSelected(&x);
	int slot = GetSlotList()->Find(item);
	bool wasDefaultSlot = item ? item->IsDefault() : false;

	switch(LOWORD(wParam))
	{
		case IDC_FILTER:
			if (HIWORD(wParam)==EN_CHANGE)
			{
				char filter[128]="";
				GetWindowText(GetDlgItem(m_hwnd, IDC_FILTER), filter, sizeof(filter));
				g_filter.Set(filter);
				Update();
			}
#ifndef _SNM_SWELL_ISSUES // EN_SETFOCUS, EN_KILLFOCUS not supported
			else if (HIWORD(wParam)==EN_SETFOCUS)
			{
				HWND hFilt = GetDlgItem(m_hwnd, IDC_FILTER);
				char filter[128]="";
				GetWindowText(hFilt, filter, sizeof(filter));
				if (!strcmp(filter, FILTER_DEFAULT_STR))
					SetWindowText(hFilt, "");
			}
			else if (HIWORD(wParam)==EN_KILLFOCUS)
			{
				HWND hFilt = GetDlgItem(m_hwnd, IDC_FILTER);
				char filter[128]="";
				GetWindowText(hFilt, filter, sizeof(filter));
				if (*filter == '\0') 
					SetWindowText(hFilt, FILTER_DEFAULT_STR);
			}
#endif
			break;

		// ***** Common *****
		case ADD_SLOT_MSG:
			AddSlot(true);
			break;
		case INSERT_SLOT_MSG:
			InsertAtSelectedSlot(true);
			break;
		case CLEAR_SLOTS_MSG:
			ClearDeleteSlots(0, true);
			break;
		case DEL_SLOTS_MSG:
			ClearDeleteSlots(1, true);
			break;
		case DEL_FILES_MSG:
			ClearDeleteSlots(4|2, true);
			break;
		case LOAD_MSG:
			if (item) GetSlotList()->BrowseSlot(slot);
			break;
		case EDIT_MSG:
			if (item) GetSlotList()->EditSlot(slot);
			break;
#ifdef _WIN32
		case DIFF_MSG:
		{
			PathSlotItem* item2 = (PathSlotItem*)m_pLists.Get(0)->EnumSelected(&x);
			if (item && item2 && g_SNM_DiffToolFn.GetLength())
			{
				char fn1[SNM_MAX_PATH]="", fn2[SNM_MAX_PATH]="";
				if (GetSlotList()->GetFullPath(GetSlotList()->Find(item), fn1, sizeof(fn1)) &&
					GetSlotList()->GetFullPath(GetSlotList()->Find(item2), fn2, sizeof(fn2)))
				{
					WDL_FastString prmStr;
					prmStr.SetFormatted(sizeof(fn1)*3, " \"%s\" \"%s\"", fn1, fn2);
					_spawnl(_P_NOWAIT, g_SNM_DiffToolFn.Get(), prmStr.Get(), NULL);
				}
			}
			break;
		}
#endif
		case EXPLORE_MSG:
		{
			char fullPath[SNM_MAX_PATH] = "";
			if (GetSlotList()->GetFullPath(slot, fullPath, sizeof(fullPath)))
				if (char* p = strrchr(fullPath, PATH_SLASH_CHAR)) {
					*(p+1) = '\0'; // ShellExecute() is KO otherwie..
					ShellExecute(NULL, "open", fullPath, NULL, NULL, SW_SHOWNORMAL);
				}
			break;
		}
		// auto-fill
		case BTNID_AUTOFILL:
		case AUTOFILL_MSG:
			AutoFill(g_resViewType);
			break;
		case AUTOFILL_DIR_MSG:
		{
			char path[SNM_MAX_PATH] = "";
			if (BrowseForDirectory(__LOCALIZE("Set auto-fill directory","sws_DLG_150"), GetAutoFillDir(), path, sizeof(path)))
				SetAutoFillDir(path);
			break;
		}
		case AUTOFILL_PRJ_MSG:
		{
			char path[SNM_MAX_PATH] = "";
			GetProjectPath(path, sizeof(path));
			SetAutoFillDir(path);
			break;
		}
		case AUTOFILL_DEFAULT_MSG:
		{
			char path[SNM_MAX_PATH] = "";
			if (_snprintfStrict(path, sizeof(path), "%s%c%s", GetResourcePath(), PATH_SLASH_CHAR, GetSlotList()->GetResourceDir()) > 0) {
				if (!FileOrDirExists(path))
					CreateDirectory(path, NULL);
				SetAutoFillDir(path);
			}
			break;
		}
		case AUTOFILL_SYNC_MSG: // i.e. sync w/ auto-save
			g_syncAutoDirPrefs[g_resViewType] = !g_syncAutoDirPrefs[g_resViewType];
			if (g_syncAutoDirPrefs[g_resViewType])
				g_autoFillDirs.Get(g_resViewType)->Set(GetAutoSaveDir());  // do not use SetAutoFillDir() here!
			break;
		// auto-save
		case BTNID_AUTOSAVE:
		case AUTOSAVE_MSG:
			AutoSave(g_resViewType, true, GetTypeForUser()==SNM_SLOT_TR ? g_asTrTmpltPref : GetTypeForUser()==SNM_SLOT_FXC ? g_asFXChainPref : 0);
			break;
		case AUTOSAVE_DIR_MSG:
		{
			char path[SNM_MAX_PATH] = "";
			if (BrowseForDirectory(__LOCALIZE("Set auto-save directory","sws_DLG_150"), GetAutoSaveDir(), path, sizeof(path)))
				SetAutoSaveDir(path);
			break;
		}
		case AUTOSAVE_DIR_PRJ_MSG:
		{
			char prjPath[SNM_MAX_PATH] = "", path[SNM_MAX_PATH] = "";
			GetProjectPath(prjPath, sizeof(prjPath));
			// see GetFileRelativePath..
			if (_snprintfStrict(path, sizeof(path), "%s%c%s", prjPath, PATH_SLASH_CHAR, GetFileRelativePath(GetSlotList()->GetResourceDir())) > 0) {
				if (!FileOrDirExists(path))
					CreateDirectory(path, NULL);
				SetAutoSaveDir(path);
			}
			break;
		}
		case AUTOSAVE_DIR_DEFAULT_MSG:
		{
			char path[SNM_MAX_PATH] = "";
			if (_snprintfStrict(path, sizeof(path), "%s%c%s", GetResourcePath(), PATH_SLASH_CHAR, GetSlotList()->GetResourceDir()) > 0) {
				if (!FileOrDirExists(path))
					CreateDirectory(path, NULL);
				SetAutoSaveDir(path);
			}
			break;
		}
		case AUTOSAVE_SYNC_MSG: // i.e. sync w/ auto-fill
			g_syncAutoDirPrefs[g_resViewType] = !g_syncAutoDirPrefs[g_resViewType];
			if (g_syncAutoDirPrefs[g_resViewType])
				g_autoSaveDirs.Get(g_resViewType)->Set(GetAutoFillDir()); // do not use SetAutoSaveDir() here!
			break;
		// text filter mode
		case FILTER_BY_NAME_MSG:
			if (g_filterPref&1) g_filterPref &= 6; // 110
			else g_filterPref |= 1;
			Update();
//			SetFocus(GetDlgItem(m_hwnd, IDC_FILTER));
			break;
		case FILTER_BY_PATH_MSG:
			if (g_filterPref&2) g_filterPref &= 5; // 101
			else g_filterPref |= 2;
			Update();
//			SetFocus(GetDlgItem(m_hwnd, IDC_FILTER));
			break;
		case FILTER_BY_COMMENT_MSG:
			if (g_filterPref&4) g_filterPref &= 3; // 011
			else g_filterPref |= 4;
			Update();
//			SetFocus(GetDlgItem(m_hwnd, IDC_FILTER));
			break;
		case RENAME_MSG:
			if (item) {
				char fullPath[SNM_MAX_PATH] = "";
				if (GetSlotList()->GetFullPath(slot, fullPath, sizeof(fullPath)) && FileOrDirExistsErrMsg(fullPath))
					m_pLists.Get(0)->EditListItem((SWS_ListItem*)item, COL_NAME);
			}
			break;
		case REN_BOOKMARK_MSG:
			RenameBookmark(g_resViewType);
			break;
		case BTNID_ADD_BOOKMARK:
			NewBookmark(g_resViewType, false);
			break;
		case COPY_BOOKMARK_MSG:
			NewBookmark(g_resViewType, true);
			break;
		case BTNID_DEL_BOOKMARK:
		case DEL_BOOKMARK_MSG:
			DeleteBookmark(g_resViewType);
			break;

		// new bookmark cmds are in an interval of cmds: see "default" case below..

#ifdef _SNM_MISC // moved to context menu
		case BTNID_TIED_ACTIONS:
			if (!HIWORD(wParam) || HIWORD(wParam)==600)
				g_SNM_TiedSlotActions[GetTypeForUser()] = g_resViewType;
			break;
#endif
		case TIE_BOOKMARK_MSG:
			g_SNM_TiedSlotActions[GetTypeForUser()] = g_resViewType;
			break;

		// ***** FX chain *****
		case FXC_COPY_MSG:
			if (item) CopyFXChainSlotToClipBoard(slot);
			break;
		case FXC_APPLY_TR_MSG:
		case FXC_PASTE_TR_MSG:
			if (item && slot >= 0) {
				ApplyTracksFXChainSlot(g_resViewType, LOWORD(wParam)==FXC_APPLY_TR_MSG?FXC_APPLY_TR_STR:FXC_PASTE_TR_STR, slot, LOWORD(wParam)==FXC_APPLY_TR_MSG, false);
				if (wasDefaultSlot && !item->IsDefault())
					Update();
			}
			break;
		case FXC_APPLY_TAKE_MSG:
		case FXC_PASTE_TAKE_MSG:
			if (item && slot >= 0) {
				ApplyTakesFXChainSlot(g_resViewType, LOWORD(wParam)==FXC_APPLY_TAKE_MSG?FXC_APPLY_TAKE_STR:FXC_PASTE_TAKE_STR, slot, true, LOWORD(wParam)==FXC_APPLY_TAKE_MSG);
				if (wasDefaultSlot && !item->IsDefault())
					Update();
			}
			break;
		case FXC_APPLY_ALLTAKES_MSG:
		case FXC_PASTE_ALLTAKES_MSG:
			if (item && slot >= 0) {
				ApplyTakesFXChainSlot(g_resViewType, LOWORD(wParam)==FXC_APPLY_ALLTAKES_MSG?FXC_APPLY_ALLTAKES_STR:FXC_PASTE_ALLTAKES_STR, slot, false, LOWORD(wParam)==FXC_APPLY_ALLTAKES_MSG);
				if (wasDefaultSlot && !item->IsDefault())
					Update();
			}
			break;
		case FXC_AUTOSAVE_INPUTFX_MSG:
			g_asFXChainPref = FXC_AUTOSAVE_PREF_INPUT_FX;
			break;
		case FXC_AUTOSAVE_TR_MSG:
			g_asFXChainPref = FXC_AUTOSAVE_PREF_TRACK;
			break;
		case FXC_AUTOSAVE_ITEM_MSG:
			g_asFXChainPref = FXC_AUTOSAVE_PREF_ITEM;
			break;
		case FXC_AUTOSAVE_DEFNAME_MSG:
		case FXC_AUTOSAVE_FX1NAME_MSG:
			g_asFXChainNamePref = g_asFXChainNamePref ? 0:1;
			break;

		// ***** Track template *****
		case TRT_APPLY_MSG:
			if (item && slot >= 0) {
				ApplyTrackTemplateSlot(g_resViewType, TRT_APPLY_STR, slot, false, false);
				if (wasDefaultSlot && !item->IsDefault())
					Update();
			}
			break;
		case TRT_APPLY_WITH_ENV_ITEM_MSG:
			if (item && slot >= 0) {
				ApplyTrackTemplateSlot(g_resViewType, TRT_APPLY_WITH_ENV_ITEM_STR, slot, true, true);
				if (wasDefaultSlot && !item->IsDefault())
					Update();
			}
			break;
		case TRT_IMPORT_MSG:
			if (item && slot >= 0) {
				ImportTrackTemplateSlot(g_resViewType, TRT_IMPORT_STR, slot);
				if (wasDefaultSlot && !item->IsDefault())
					Update();
			}
			break;
		case TRT_REPLACE_ITEMS_MSG:
		case TRT_PASTE_ITEMS_MSG:
			if (item && slot >= 0) {
				ReplacePasteItemsTrackTemplateSlot(g_resViewType, LOWORD(wParam)==TRT_REPLACE_ITEMS_MSG?TRT_REPLACE_ITEMS_STR:TRT_PASTE_ITEMS_STR, slot, LOWORD(wParam)==TRT_PASTE_ITEMS_MSG);
				if (wasDefaultSlot && !item->IsDefault())
					Update();
			}
			break;
		case BTNID_OFFSET_TR_TEMPLATE:
			if (int* offsPref = (int*)GetConfigVar("templateditcursor")) { // >= REAPER v4.15
				if (*offsPref) *offsPref = 0;
				else *offsPref = 1;
			}
			break;
		case TRT_AUTOSAVE_WITEMS_MSG:
			if (g_asTrTmpltPref & 1) g_asTrTmpltPref &= 0xE;
			else g_asTrTmpltPref |= 1;
			break;
		case TRT_AUTOSAVE_WENVS_MSG:
			if (g_asTrTmpltPref & 2) g_asTrTmpltPref &= 0xD;
			else g_asTrTmpltPref |= 2;
			break;

		// ***** Project template *****
		case PRJ_OPEN_SELECT_MSG:
			if (item && slot >= 0) {
				LoadOrSelectProjectSlot(g_resViewType, PRJ_SELECT_LOAD_STR, slot, false);
				if (wasDefaultSlot && !item->IsDefault())
					Update();
			}
			break;
		case PRJ_OPEN_SELECT_TAB_MSG:
		{
			bool updt = false;
			while(item) {
				slot = GetSlotList()->Find(item);
				wasDefaultSlot = item->IsDefault();
				LoadOrSelectProjectSlot(g_resViewType, PRJ_SELECT_LOAD_TAB_STR, slot, true);
				updt |= (wasDefaultSlot && !item->IsDefault());
				item = (PathSlotItem*)m_pLists.Get(0)->EnumSelected(&x);
			}
			if (updt) Update();
			break;
		}
		case PRJ_AUTOFILL_RECENTS_MSG:
		{
			int startSlot = GetSlotList()->GetSize();
			int nbRecents = GetPrivateProfileInt("REAPER", "numrecent", 0, get_ini_file());
			nbRecents = min(98, nbRecents); // just in case: 2 digits max..
			if (nbRecents)
			{
				char key[16], path[SNM_MAX_PATH];
				WDL_PtrList_DeleteOnDestroy<WDL_FastString> prjs;
				for (int i=0; i < nbRecents; i++) {
					if (_snprintfStrict(key, sizeof(key), "recent%02d", i+1) > 0) {
						GetPrivateProfileString("Recent", key, "", path, sizeof(path), get_ini_file());
						if (*path)
							prjs.Add(new WDL_FastString(path));
					}
				}
				for (int i=0; i < prjs.GetSize(); i++)
					if (GetSlotList()->FindByResFulltPath(prjs.Get(i)->Get()) < 0) // skip if already present
						GetSlotList()->AddSlot(prjs.Get(i)->Get());
			}
			if (startSlot != GetSlotList()->GetSize()) {
				Update();
				SelectBySlot(startSlot, GetSlotList()->GetSize());
			}
			else {
				char msg[SNM_MAX_PATH] = "";
				_snprintfSafe(msg, sizeof(msg), __LOCALIZE_VERFMT("No new recent projects found!\n%s","sws_DLG_150"), AUTOFILL_ERR_STR);
				MessageBox(m_hwnd, msg, __LOCALIZE("S&M - Warning","sws_DLG_150"), MB_OK);
			}
			break;
		}
		case PRJ_LOADER_CONF_MSG:
			ProjectLoaderConf(NULL);
			break;
		case PRJ_LOADER_SET_MSG:
		{
			int min=GetSlotList()->GetSize(), max=0;
			while(item) {
				slot = GetSlotList()->Find(item)+1;
				if (slot>max) max = slot;
				if (slot<min) min = slot;
				item = (PathSlotItem*)m_pLists.Get(0)->EnumSelected(&x);
			}
			if (max>min) {
				g_SNM_PrjLoaderStartPref = min;
				g_SNM_PrjLoaderEndPref = max;
				Update();
			}
			break;
		}
		case PRJ_LOADER_CLEAR_MSG:
			g_SNM_PrjLoaderStartPref = g_SNM_PrjLoaderEndPref = -1;
			Update();
			break;

		// ***** media files *****
		case MED_PLAY_MSG:
		case MED_LOOP_MSG:
			while(item) {
				slot = GetSlotList()->Find(item);
				wasDefaultSlot = item->IsDefault();
				TogglePlaySelTrackMediaSlot(g_resViewType, MED_PLAYLOOP_STR, slot, false, LOWORD(wParam)==MED_LOOP_MSG);
				item = (PathSlotItem*)m_pLists.Get(0)->EnumSelected(&x);
			}
			break;
		case MED_ADD_CURTR_MSG:
		case MED_ADD_NEWTR_MSG:
		case MED_ADD_TAKES_MSG:
			while(item) {
				slot = GetSlotList()->Find(item);
				wasDefaultSlot = item->IsDefault();
				InsertMediaSlot(g_resViewType, MED_ADD_STR, slot, LOWORD(wParam)==MED_ADD_CURTR_MSG ? 0 : LOWORD(wParam)==MED_ADD_NEWTR_MSG ? 1 : 3);
				item = (PathSlotItem*)m_pLists.Get(0)->EnumSelected(&x);
			}
			break;

		// ***** theme *****
#ifdef _WIN32
		case THM_LOAD_MSG:
			LoadThemeSlot(g_resViewType, THM_LOAD_STR, slot);
			break;
#endif

		// ***** image *****
		case IMG_SHOW_MSG:
			ShowImageSlot(g_resViewType, IMG_SHOW_STR, slot);
			break;
		case IMG_TRICON_MSG:
			SetSelTrackIconSlot(g_resViewType, IMG_TRICON_STR, slot);
			break;

			// ***** WDL GUI & others *****
		case CMBID_TYPE:
			if (HIWORD(wParam)==CBN_SELCHANGE)
			{
				// stop cell editing (changing the list content would be ignored otherwise,
				// leading to unsynchronized dropdown box vs list view)
				m_pLists.Get(0)->EditListItemEnd(false);
				SetType(m_cbType.GetCurSel());
			}
			break;
		case CMBID_DBLCLICK_TYPE:
			if (HIWORD(wParam)==CBN_SELCHANGE) {
				int hi = HIWORD(g_dblClickPrefs[g_resViewType]);
				g_dblClickPrefs[g_resViewType] = m_cbDblClickType.GetCurSel();
				g_dblClickPrefs[g_resViewType] |= hi<<16;
				m_parentVwnd.RequestRedraw(NULL); // some options can be hidden for new sel
			}
			break;
		case CMBID_DBLCLICK_TO:
			if (HIWORD(wParam)==CBN_SELCHANGE) {
				g_dblClickPrefs[g_resViewType] = LOWORD(g_dblClickPrefs[g_resViewType]);
				g_dblClickPrefs[g_resViewType] |= m_cbDblClickTo.GetCurSel()<<16;
			}
			break;
		default:
			if (LOWORD(wParam) >= NEW_BOOKMARK_FXC_MSG && LOWORD(wParam) < (NEW_BOOKMARK_FXC_MSG+SNM_NUM_DEFAULT_SLOTS))
				NewBookmark(LOWORD(wParam)-NEW_BOOKMARK_FXC_MSG, false);
			else
				Main_OnCommand((int)wParam, (int)lParam);
			break;
	}
}

void SNM_ResourceWnd::AutoSaveContextMenu(HMENU _menu, bool _saveItem)
{
	int typeForUser = GetTypeForUser();
	char autoPath[SNM_MAX_PATH] = "";
	_snprintfSafe(autoPath, sizeof(autoPath), __LOCALIZE_VERFMT("[Current auto-save path: %s]","sws_DLG_150"), *GetAutoSaveDir() ? GetAutoSaveDir() : __LOCALIZE("undefined","sws_DLG_150"));
	AddToMenu(_menu, autoPath, 0, -1, false, MF_GRAYED);
	AddToMenu(_menu, __LOCALIZE("Sync auto-save and auto-fill paths","sws_DLG_150"), AUTOFILL_SYNC_MSG, -1, false, g_syncAutoDirPrefs[g_resViewType] ? MFS_CHECKED : MFS_UNCHECKED);

	if (_saveItem) {
		AddToMenu(_menu, SWS_SEPARATOR, 0);
		AddToMenu(_menu, __LOCALIZE("Auto-save","sws_DLG_150"), AUTOSAVE_MSG, -1, false, !IsFiltered() ? MF_ENABLED : MF_GRAYED);
	}

	AddToMenu(_menu, SWS_SEPARATOR, 0);
	AddToMenu(_menu, __LOCALIZE("Set auto-save directory...","sws_DLG_150"), AUTOSAVE_DIR_MSG);
	AddToMenu(_menu, __LOCALIZE("Set auto-save directory to default resource path","sws_DLG_150"), AUTOSAVE_DIR_DEFAULT_MSG);
	switch(typeForUser)
	{
		case SNM_SLOT_FXC:
			AddToMenu(_menu, __LOCALIZE("Set auto-save directory to project path (/FXChains)","sws_DLG_150"), AUTOSAVE_DIR_PRJ_MSG);
			AddToMenu(_menu, SWS_SEPARATOR, 0);
			AddToMenu(_menu, __LOCALIZE("Auto-save FX chains from track selection","sws_DLG_150"), FXC_AUTOSAVE_TR_MSG, -1, false, g_asFXChainPref == FXC_AUTOSAVE_PREF_TRACK ? MFS_CHECKED : MFS_UNCHECKED);
			AddToMenu(_menu, __LOCALIZE("Auto-save FX chains from item selection","sws_DLG_150"), FXC_AUTOSAVE_ITEM_MSG, -1, false, g_asFXChainPref == FXC_AUTOSAVE_PREF_ITEM ? MFS_CHECKED : MFS_UNCHECKED);
			AddToMenu(_menu, __LOCALIZE("Auto-save input FX chains from track selection","sws_DLG_150"), FXC_AUTOSAVE_INPUTFX_MSG, -1, false, g_asFXChainPref == FXC_AUTOSAVE_PREF_INPUT_FX ? MFS_CHECKED : MFS_UNCHECKED);
			AddToMenu(_menu, SWS_SEPARATOR, 0);
			AddToMenu(_menu, __LOCALIZE("Generate filename from track/item name","sws_DLG_150"), FXC_AUTOSAVE_DEFNAME_MSG, -1, false, !g_asFXChainNamePref ? MFS_CHECKED : MFS_UNCHECKED);
			AddToMenu(_menu, __LOCALIZE("Generate filename from first FX name","sws_DLG_150"), FXC_AUTOSAVE_FX1NAME_MSG, -1, false, g_asFXChainNamePref ? MFS_CHECKED : MFS_UNCHECKED);
			break;
		case SNM_SLOT_TR:
			AddToMenu(_menu, __LOCALIZE("Set auto-save directory to project path (/TrackTemplates)","sws_DLG_150"), AUTOSAVE_DIR_PRJ_MSG);
			AddToMenu(_menu, SWS_SEPARATOR, 0);
			AddToMenu(_menu, __LOCALIZE("Include track items in templates","sws_DLG_150"), TRT_AUTOSAVE_WITEMS_MSG, -1, false, (g_asTrTmpltPref&1) ? MFS_CHECKED : MFS_UNCHECKED);
			AddToMenu(_menu, __LOCALIZE("Include envelopes in templates","sws_DLG_150"), TRT_AUTOSAVE_WENVS_MSG, -1, false, (g_asTrTmpltPref&2) ? MFS_CHECKED : MFS_UNCHECKED);
			break;
		case SNM_SLOT_PRJ:
			AddToMenu(_menu, __LOCALIZE("Set auto-save directory to project path (/ProjectTemplates)","sws_DLG_150"), AUTOSAVE_DIR_PRJ_MSG);
			break;
	}
}

void SNM_ResourceWnd::AutoFillContextMenu(HMENU _menu, bool _fillItem)
{
	int typeForUser = GetTypeForUser();
	char autoPath[SNM_MAX_PATH] = "";
	_snprintfSafe(autoPath, sizeof(autoPath), __LOCALIZE_VERFMT("[Current auto-fill path: %s]","sws_DLG_150"), *GetAutoFillDir() ? GetAutoFillDir() : __LOCALIZE("undefined","sws_DLG_150"));
	AddToMenu(_menu, autoPath, 0, -1, false, MF_GRAYED);
	if (GetSlotList()->IsAutoSave())
		AddToMenu(_menu, __LOCALIZE("Sync auto-save and auto-fill paths","sws_DLG_150"), AUTOSAVE_SYNC_MSG, -1, false, g_syncAutoDirPrefs[g_resViewType] ? MFS_CHECKED : MFS_UNCHECKED);

	if (_fillItem) {
		AddToMenu(_menu, SWS_SEPARATOR, 0);
		AddToMenu(_menu, __LOCALIZE("Auto-fill","sws_DLG_150"), AUTOFILL_MSG, -1, false, !IsFiltered() ? MF_ENABLED : MF_GRAYED);
	}
	if (typeForUser == SNM_SLOT_PRJ) {
		if (!_fillItem) AddToMenu(_menu, SWS_SEPARATOR, 0);
		AddToMenu(_menu, __LOCALIZE("Auto-fill with recent projects","sws_DLG_150"), PRJ_AUTOFILL_RECENTS_MSG, -1, false, !IsFiltered() ? MF_ENABLED : MF_GRAYED);
	}
	AddToMenu(_menu, SWS_SEPARATOR, 0);
	AddToMenu(_menu, __LOCALIZE("Set auto-fill directory...","sws_DLG_150"), AUTOFILL_DIR_MSG, -1, false, !IsFiltered() ? MF_ENABLED : MF_GRAYED);
	AddToMenu(_menu, __LOCALIZE("Set auto-fill directory to default resource path","sws_DLG_150"), AUTOFILL_DEFAULT_MSG, -1, false, !IsFiltered() ? MF_ENABLED : MF_GRAYED);
	AddToMenu(_menu, __LOCALIZE("Set auto-fill directory to project path","sws_DLG_150"), AUTOFILL_PRJ_MSG, -1, false, !IsFiltered() ? MF_ENABLED : MF_GRAYED);
}

HMENU SNM_ResourceWnd::OnContextMenu(int _x, int _y, bool* _wantDefaultItems)
{
	HMENU hMenu = CreatePopupMenu();

	// specific context menu for auto-fill/auto-save buttons
	POINT p; GetCursorPos(&p);
	ScreenToClient(m_hwnd,&p);
	if (WDL_VWnd* v = m_parentVwnd.VirtWndFromPoint(p.x,p.y,1))
	{
		switch (v->GetID())
		{
			case BTNID_AUTOFILL:
				*_wantDefaultItems = false;
				AutoFillContextMenu(hMenu, false);
				return hMenu;
			case BTNID_AUTOSAVE:
				if (GetSlotList()->IsAutoSave()) {
					*_wantDefaultItems = false;
					AutoSaveContextMenu(hMenu, false);
					return hMenu;
				}
				break;
		}
	}

	// general context menu
	int iCol;
	PathSlotItem* pItem = (PathSlotItem*)m_pLists.Get(0)->GetHitItem(_x, _y, &iCol);
	UINT enabled = (pItem && !pItem->IsDefault()) ? MF_ENABLED : MF_GRAYED;
	int typeForUser = GetTypeForUser();
	if (pItem && iCol >= 0)
	{
		*_wantDefaultItems = false;
		switch(typeForUser)
		{
			case SNM_SLOT_FXC:
				AddToMenu(hMenu, FXC_PASTE_TR_STR, FXC_PASTE_TR_MSG, -1, false, enabled);
				AddToMenu(hMenu, FXC_APPLY_TR_STR, FXC_APPLY_TR_MSG, -1, false, enabled);
				AddToMenu(hMenu, SWS_SEPARATOR, 0);
				AddToMenu(hMenu, FXC_PASTE_TAKE_STR, FXC_PASTE_TAKE_MSG, -1, false, enabled);
				AddToMenu(hMenu, FXC_PASTE_ALLTAKES_STR, FXC_PASTE_ALLTAKES_MSG, -1, false, enabled);
				AddToMenu(hMenu, FXC_APPLY_TAKE_STR, FXC_APPLY_TAKE_MSG, -1, false, enabled);
				AddToMenu(hMenu, FXC_APPLY_ALLTAKES_STR, FXC_APPLY_ALLTAKES_MSG, -1, false, enabled);
/*JFB seems confusing, commented: it is not a "copy slot" thingy!
				AddToMenu(hMenu, SWS_SEPARATOR, 0);
				AddToMenu(hMenu, __LOCALIZE("Copy","sws_DLG_150"), FXC_COPY_MSG, -1, false, enabled);
*/
				break;
			case SNM_SLOT_TR:
				AddToMenu(hMenu, TRT_IMPORT_STR, TRT_IMPORT_MSG, -1, false, enabled);
				AddToMenu(hMenu, TRT_APPLY_STR, TRT_APPLY_MSG, -1, false, enabled);
				AddToMenu(hMenu, TRT_APPLY_WITH_ENV_ITEM_STR, TRT_APPLY_WITH_ENV_ITEM_MSG, -1, false, enabled);
				AddToMenu(hMenu, SWS_SEPARATOR, 0);
				AddToMenu(hMenu, TRT_PASTE_ITEMS_STR, TRT_PASTE_ITEMS_MSG, -1, false, enabled);
				AddToMenu(hMenu, TRT_REPLACE_ITEMS_STR, TRT_REPLACE_ITEMS_MSG, -1, false, enabled);
				break;
			case SNM_SLOT_PRJ:
			{
				AddToMenu(hMenu, PRJ_SELECT_LOAD_STR, PRJ_OPEN_SELECT_MSG, -1, false, enabled);
				AddToMenu(hMenu, PRJ_SELECT_LOAD_TAB_STR, PRJ_OPEN_SELECT_TAB_MSG, -1, false, enabled);
				if (g_resViewType == typeForUser)  // no GetTypeForUser() here: only one loader/selecter config atm..
				{
					AddToMenu(hMenu, SWS_SEPARATOR, 0);
					int x=0, nbsel=0; while(m_pLists.Get(0)->EnumSelected(&x)) nbsel++;
					AddToMenu(hMenu, __LOCALIZE("Project loader/selecter configuration...","sws_DLG_150"), PRJ_LOADER_CONF_MSG);
					AddToMenu(hMenu, __LOCALIZE("Set project loader/selecter from selection","sws_DLG_150"), PRJ_LOADER_SET_MSG, -1, false, nbsel>1 ? MF_ENABLED : MF_GRAYED);
					AddToMenu(hMenu, __LOCALIZE("Clear project loader/selecter configuration","sws_DLG_150"), PRJ_LOADER_CLEAR_MSG, -1, false, IsProjectLoaderConfValid() ? MF_ENABLED : MF_GRAYED);
				}
				break;
			}
			case SNM_SLOT_MEDIA:
				AddToMenu(hMenu, MED_PLAY_STR, MED_PLAY_MSG, -1, false, enabled);
				AddToMenu(hMenu, MED_LOOP_STR, MED_LOOP_MSG, -1, false, enabled);
				AddToMenu(hMenu, SWS_SEPARATOR, 0);
				AddToMenu(hMenu, __LOCALIZE("Add to current track","sws_DLG_150"), MED_ADD_CURTR_MSG, -1, false, enabled);
				AddToMenu(hMenu, __LOCALIZE("Add to new tracks","sws_DLG_150"), MED_ADD_NEWTR_MSG, -1, false, enabled);
				AddToMenu(hMenu, __LOCALIZE("Add to selected items as takes","sws_DLG_150"), MED_ADD_TAKES_MSG, -1, false, enabled);
				break;
			case SNM_SLOT_IMG:
				AddToMenu(hMenu, IMG_SHOW_STR, IMG_SHOW_MSG, -1, false, enabled);
				AddToMenu(hMenu, IMG_TRICON_STR, IMG_TRICON_MSG, -1, false, enabled);
				AddToMenu(hMenu, __LOCALIZE("Add to current track as item","sws_DLG_150"), MED_ADD_CURTR_MSG, -1, false, enabled);
				break;
#ifdef _WIN32
			case SNM_SLOT_THM:
				AddToMenu(hMenu, THM_LOAD_STR, THM_LOAD_MSG, -1, false, enabled);
				break;
#endif
		}
	}

	if (GetMenuItemCount(hMenu))
		AddToMenu(hMenu, SWS_SEPARATOR, 0);

	// always displayed, even if the list is empty
	AddToMenu(hMenu, __LOCALIZE("Add slot","sws_DLG_150"), ADD_SLOT_MSG, -1, false, !IsFiltered() ? MF_ENABLED : MF_GRAYED);

	if (pItem && iCol >= 0)
	{
		AddToMenu(hMenu, __LOCALIZE("Insert slot","sws_DLG_150"), INSERT_SLOT_MSG, -1, false, !IsFiltered() ? MF_ENABLED : MF_GRAYED);
		AddToMenu(hMenu, __LOCALIZE("Clear slots","sws_DLG_150"), CLEAR_SLOTS_MSG, -1, false, enabled);
		AddToMenu(hMenu, __LOCALIZE("Delete slots","sws_DLG_150"), DEL_SLOTS_MSG);

		AddToMenu(hMenu, SWS_SEPARATOR, 0);
		AddToMenu(hMenu, __LOCALIZE("Load slot/file...","sws_DLG_150"), LOAD_MSG);
		AddToMenu(hMenu, __LOCALIZE("Delete files","sws_DLG_150"), DEL_FILES_MSG, -1, false, enabled);
		AddToMenu(hMenu, __LOCALIZE("Rename file","sws_DLG_150"), RENAME_MSG, -1, false, enabled);
		if (GetSlotList()->IsText())
#ifndef _WIN32
			AddToMenu(hMenu, __LOCALIZE("Display file...","sws_DLG_150"), EDIT_MSG, -1, false, enabled);
#else
		{
			int x=0, nbsel=0; while(m_pLists.Get(0)->EnumSelected(&x)) nbsel++;
			AddToMenu(hMenu, __LOCALIZE("Diff files...","sws_DLG_150"), DIFF_MSG, -1, false, nbsel==2 && enabled==MF_ENABLED && g_SNM_DiffToolFn.GetLength() ? MF_ENABLED:MF_GRAYED);
			AddToMenu(hMenu, __LOCALIZE("Edit file...","sws_DLG_150"), EDIT_MSG, -1, false, enabled);
		}	
#endif
		AddToMenu(hMenu, __LOCALIZE("Show path in explorer/finder...","sws_DLG_150"), EXPLORE_MSG, -1, false, enabled);
	}
	else
	{
		// auto-fill
		AddToMenu(hMenu, SWS_SEPARATOR, 0);
		HMENU hAutoFillSubMenu = CreatePopupMenu();
		AddSubMenu(hMenu, hAutoFillSubMenu, __LOCALIZE("Auto-fill","sws_DLG_150"));
		AutoFillContextMenu(hAutoFillSubMenu, true);

		// auto-save
		if (GetSlotList()->IsAutoSave())
		{
			HMENU hAutoSaveSubMenu = CreatePopupMenu();
			AddSubMenu(hMenu, hAutoSaveSubMenu, __LOCALIZE("Auto-save","sws_DLG_150"));
			AutoSaveContextMenu(hAutoSaveSubMenu, true);
		}

		// bookmarks
//		AddToMenu(hMenu, SWS_SEPARATOR, 0);
		HMENU hBookmarkSubMenu = CreatePopupMenu();
		AddSubMenu(hMenu, hBookmarkSubMenu, __LOCALIZE("Bookmarks","sws_DLG_150"));
		HMENU hNewBookmarkSubMenu = CreatePopupMenu();
		AddSubMenu(hBookmarkSubMenu, hNewBookmarkSubMenu, __LOCALIZE("New bookmark","sws_DLG_150"));
		for (int i=0; i < SNM_NUM_DEFAULT_SLOTS; i++)
			if (char* p = _strdup(g_SNM_ResSlots.Get(i)->GetDesc())) {
				*p = toupper(*p); // 1st char to upper
				AddToMenu(hNewBookmarkSubMenu, p, NEW_BOOKMARK_FXC_MSG+i);
				free(p);
			}
		AddToMenu(hBookmarkSubMenu, __LOCALIZE("Copy bookmark...","sws_DLG_150"), COPY_BOOKMARK_MSG);
		AddToMenu(hBookmarkSubMenu, __LOCALIZE("Rename...","sws_DLG_150"), REN_BOOKMARK_MSG, -1, false, g_resViewType >= SNM_NUM_DEFAULT_SLOTS ? MF_ENABLED : MF_GRAYED);
		AddToMenu(hBookmarkSubMenu, __LOCALIZE("Delete","sws_DLG_150"), DEL_BOOKMARK_MSG, -1, false, g_resViewType >= SNM_NUM_DEFAULT_SLOTS ? MF_ENABLED : MF_GRAYED);

		// filter prefs
		AddToMenu(hMenu, SWS_SEPARATOR, 0);
		HMENU hFilterSubMenu = CreatePopupMenu();
		AddSubMenu(hMenu, hFilterSubMenu, __LOCALIZE("Filter on","sws_DLG_150"));
		AddToMenu(hFilterSubMenu, __LOCALIZE("Name","sws_DLG_150"), FILTER_BY_NAME_MSG, -1, false, (g_filterPref&1) ? MFS_CHECKED : MFS_UNCHECKED);
		AddToMenu(hFilterSubMenu, __LOCALIZE("Path","sws_DLG_150"), FILTER_BY_PATH_MSG, -1, false, (g_filterPref&2) ? MFS_CHECKED : MFS_UNCHECKED);
		AddToMenu(hFilterSubMenu, __LOCALIZE("Comment","sws_DLG_150"), FILTER_BY_COMMENT_MSG, -1, false, (g_filterPref&4) ? MFS_CHECKED : MFS_UNCHECKED);

		// tie slot actions pref
		if (IsMultiType())
		{
			AddToMenu(hMenu, SWS_SEPARATOR, 0);
			char buf[128] = "";
			_snprintfSafe(buf, sizeof(buf), __LOCALIZE_VERFMT("Tie %s slot actions to this bookmark","sws_DLG_150"), g_SNM_ResSlots.Get(typeForUser)->GetDesc());
			AddToMenu(hMenu, buf, TIE_BOOKMARK_MSG, -1, false, g_SNM_TiedSlotActions[typeForUser] == g_resViewType ? MFS_CHECKED : MFS_UNCHECKED);
		}
	}
	return hMenu;
}

int SNM_ResourceWnd::OnKey(MSG* _msg, int _iKeyState) 
{
	if (_msg->message == WM_KEYDOWN)
	{
		if (!_iKeyState)
		{
			switch(_msg->wParam)
			{
				case VK_F2:
					OnCommand(RENAME_MSG, 0);
					return 1;
				case VK_DELETE:
					ClearDeleteSlots(1, true);
					return 1;
				case VK_INSERT:
					InsertAtSelectedSlot(true);
					return 1;
				case VK_RETURN:
					((SNM_ResourceView*)m_pLists.Get(0))->Perform();
					return 1;
			}
		}
		// ctrl+A => select all
		else if (_iKeyState == LVKF_CONTROL && _msg->wParam == 'A')
		{
			HWND h = GetDlgItem(m_hwnd, IDC_FILTER);
#ifdef _WIN32
			if (_msg->hwnd == h)
#else
			if (GetFocus() == h)
#endif
			{
				SetFocus(h);
				SendMessage(h, EM_SETSEL, 0, -1);
				return 1; // eat
			}
		}
	}
	return 0;
}

int SNM_ResourceWnd::GetValidDroppedFilesCount(HDROP _h)
{
	int validCnt=0;
	int iFiles = DragQueryFile(_h, 0xFFFFFFFF, NULL, 0);
	char fn[SNM_MAX_PATH] = "";
	for (int i=0; i < iFiles; i++) 
	{
		DragQueryFile(_h, i, fn, sizeof(fn));
		if (!strcmp(fn, DRAGDROP_EMPTY_SLOT) || g_SNM_ResSlots.Get(GetTypeForUser())->IsValidFileExt(GetFileExtension(fn)))
			validCnt++;
	}
	return validCnt;
}

void SNM_ResourceWnd::OnDroppedFiles(HDROP _h)
{
	int dropped = 0; //nb of successfully dropped files
	int iFiles = DragQueryFile(_h, 0xFFFFFFFF, NULL, 0);
	int iValidFiles = GetValidDroppedFilesCount(_h);

	// Check to see if we dropped on a group
	POINT pt;
	DragQueryPoint(_h, &pt);

	RECT r; // ClientToScreen doesn't work right, wtf?
	GetWindowRect(m_hwnd, &r);
	pt.x += r.left;
	pt.y += r.top;

	PathSlotItem* pItem = (PathSlotItem*)m_pLists.Get(0)->GetHitItem(pt.x, pt.y, NULL);
	int dropSlot = GetSlotList()->Find(pItem);

	// internal d'n'd ?
	if (g_dragPathSlotItems.GetSize())
	{
		int srcSlot = GetSlotList()->Find(g_dragPathSlotItems.Get(0));
		// drag'n'drop slot to the bottom
		if (srcSlot >= 0 && srcSlot < dropSlot)
			dropSlot++; // d'n'd will be more "natural"
	}

	// drop but not on a slot => create slots
	if (!pItem || dropSlot < 0 || dropSlot >= GetSlotList()->GetSize()) 
	{
		dropSlot = GetSlotList()->GetSize();
		for (int i=0; i < iValidFiles; i++)
			GetSlotList()->Add(new PathSlotItem());
	}
	// drop on a slot => insert slots at drop point
	else 
	{
		for (int i=0; i < iValidFiles; i++)
			GetSlotList()->InsertSlot(dropSlot);
	}

	// re-sync pItem 
	pItem = GetSlotList()->Get(dropSlot); 

	// Patch added/inserted slots from dropped data
	char fn[SNM_MAX_PATH] = "";
	int slot;
	for (int i=0; pItem && i < iFiles; i++)
	{
		slot = GetSlotList()->Find(pItem);
		DragQueryFile(_h, i, fn, sizeof(fn));

		// internal d'n'd ?
		if (g_dragPathSlotItems.GetSize())
		{
			if (PathSlotItem* item = GetSlotList()->Get(slot))
			{
				item->m_shortPath.Set(g_dragPathSlotItems.Get(i)->m_shortPath.Get());
				item->m_comment.Set(g_dragPathSlotItems.Get(i)->m_comment.Get());
				dropped++;
				pItem = GetSlotList()->Get(slot+1); 
			}
		}
		// .rfxchain? .rTrackTemplate? etc..
		else if (g_SNM_ResSlots.Get(GetTypeForUser())->IsValidFileExt(GetFileExtension(fn)))
		{
			if (GetSlotList()->SetFromFullPath(slot, fn)) {
				dropped++;
				pItem = GetSlotList()->Get(slot+1); 
			}
		}
	}

	if (dropped)
	{
		// internal drag'n'drop: move (=> delete previous slots)
		for (int i=0; i < g_dragPathSlotItems.GetSize(); i++)
			for (int j=GetSlotList()->GetSize()-1; j >= 0; j--)
				if (GetSlotList()->Get(j) == g_dragPathSlotItems.Get(i)) {
					if (j < dropSlot) dropSlot--;
					GetSlotList()->Delete(j, false);
				}

		Update();

		// Select item at drop point
		if (dropSlot >= 0)
			SelectBySlot(dropSlot);
	}

	g_dragPathSlotItems.Empty(false);
	DragFinish(_h);
}

void SNM_ResourceWnd::DrawControls(LICE_IBitmap* _bm, const RECT* _r, int* _tooltipHeight)
{
	// 1st row of controls (top)

	int x0=_r->left+SNM_GUI_X_MARGIN, h=SNM_GUI_TOP_H;
	if (_tooltipHeight)
		*_tooltipHeight = h;

	LICE_CachedFont* font = SNM_GetThemeFont();
	IconTheme* it = SNM_GetIconTheme();
	int typeForUser = GetTypeForUser();

	// "auto-fill" button
	SNM_SkinButton(&m_btnAutoFill, it ? &(it->toolbar_open) : NULL, __LOCALIZE("Auto-fill","sws_DLG_150"));
	if (SNM_AutoVWndPosition(DT_LEFT, &m_btnAutoFill, NULL, _r, &x0, _r->top, h, 0))
	{
		// "auto-save" button
		m_btnAutoSave.SetGrayed(!GetSlotList()->IsAutoSave());
		SNM_SkinButton(&m_btnAutoSave, it ? &(it->toolbar_save) : NULL, __LOCALIZE("Auto-save","sws_DLG_150"));
		if (SNM_AutoVWndPosition(DT_LEFT, &m_btnAutoSave, NULL, _r, &x0, _r->top, h))
		{
			// type: txt & dropdown
//			m_txtSlotsType.SetFont(font);
//			if (SNM_AutoVWndPosition(DT_LEFT, &m_txtSlotsType, NULL, _r, &x0, _r->top, h, 5))
			{
				m_cbType.SetFont(font);
//				if (SNM_AutoVWndPosition(DT_LEFT, &m_cbType, &m_txtSlotsType, _r, &x0, _r->top, h, 4))
				if (SNM_AutoVWndPosition(DT_LEFT, &m_cbType, NULL, _r, &x0, _r->top, h, 4))
				{
					// add & del bookmark buttons
					m_btnDel.SetEnabled(g_resViewType >= SNM_NUM_DEFAULT_SLOTS);
					if (SNM_AutoVWndPosition(DT_LEFT, &m_btnsAddDel, NULL, _r, &x0, _r->top, h))
					{
#ifdef _SNM_MISC // moved to context menu
						if (IsMultiType())
						{
							m_btnTiedActions.SetCheckState(g_SNM_TiedSlotActions[typeForUser] == g_resViewType);
							char buf[64] = "";
							_snprintfSafe(buf, sizeof(buf), __LOCALIZE_VERFMT("Tie %s slot actions to this bookmark","sws_DLG_150"), g_SNM_ResSlots.Get(typeForUser)->GetDesc());
							m_btnTiedActions.SetTextLabel(buf, -1, font);
							if (SNM_AutoVWndPosition(DT_LEFT, &m_btnTiedActions, NULL, _r, &x0, _r->top, h, 5))
								SNM_AddLogo(_bm, _r, x0, h);
						}
						else
#endif
							SNM_AddLogo(_bm, _r, x0, h);
					}
				}
			}
		}
	}

	// 2nd row of controls (bottom)

	x0 = _r->left+SNM_GUI_X_MARGIN; h=SNM_GUI_BOT_H;
	int y0 = _r->bottom-h;

	// defines a new rect r that takes the filter edit box into account (contrary to _r)
	RECT r;
	GetWindowRect(GetDlgItem(m_hwnd, IDC_FILTER), &r);
	ScreenToClient(m_hwnd, (LPPOINT)&r);
	ScreenToClient(m_hwnd, ((LPPOINT)&r)+1);
	r.top = _r->top;
	r.bottom = _r->bottom;
	r.right = r.left;
	r.left = _r->left;

	// "dbl-click to"
	if (GetSlotList()->IsDblClick())
	{
		m_txtDblClickType.SetFont(font);
		if (!SNM_AutoVWndPosition(DT_LEFT, &m_txtDblClickType, NULL, &r, &x0, y0, h, 5))
			return;
		m_cbDblClickType.SetFont(font);
		if (!SNM_AutoVWndPosition(DT_LEFT, &m_cbDblClickType, &m_txtDblClickType, &r, &x0, y0, h))
			return;

		switch (typeForUser)
		{
			// "to selected" (fx chain only)
			case SNM_SLOT_FXC:
				m_txtDblClickTo.SetFont(font);
				if (!SNM_AutoVWndPosition(DT_LEFT, &m_txtDblClickTo, NULL, &r, &x0, y0, h, 5))
					return;
				m_cbDblClickTo.SetFont(font);
				if (!SNM_AutoVWndPosition(DT_LEFT, &m_cbDblClickTo, &m_txtDblClickTo, &r, &x0, y0, h))
					return;
				break;
			// offset items & envs (tr template only)
			case SNM_SLOT_TR:
				if (int* offsPref = (int*)GetConfigVar("templateditcursor")) // >= REAPER v4.15
				{
					int dblClickPrefs = LOWORD(g_dblClickPrefs[g_resViewType]);
					bool showOffsOption = true;
					switch(dblClickPrefs) {
						case 1: // apply to sel tracks
							showOffsOption = false; // no offset option
							break;
						case 3: // paste template items
						case 4: // paste (replace) template items
							m_btnOffsetTrTemplate.SetTextLabel(__LOCALIZE("Offset template items by edit cursor","sws_DLG_150"), -1, font);
							break;
						default: // other dbl-click prefs
							m_btnOffsetTrTemplate.SetTextLabel(__LOCALIZE("Offset template items/envs by edit cursor","sws_DLG_150"), -1, font);
							break;
					}
					if (showOffsOption) {
						m_btnOffsetTrTemplate.SetCheckState(*offsPref);
						if (!SNM_AutoVWndPosition(DT_LEFT, &m_btnOffsetTrTemplate, NULL, &r, &x0, y0, h, 5))
							return;
					}
				}
				break;
		}
	}
}

bool SNM_ResourceWnd::GetToolTipString(int _xpos, int _ypos, char* _bufOut, int _bufOutSz)
{
	if (WDL_VWnd* v = m_parentVwnd.VirtWndFromPoint(_xpos,_ypos,1))
	{
		int typeForUser = GetTypeForUser();
		switch (v->GetID())
		{
			case BTNID_AUTOFILL:
				return (_snprintfStrict(_bufOut, _bufOutSz, __LOCALIZE_VERFMT("Auto-fill %s slots (right-click for options)\nfrom %s","sws_DLG_150"), 
					g_SNM_ResSlots.Get(typeForUser)->GetDesc(), 
					*GetAutoFillDir() ? GetAutoFillDir() : __LOCALIZE("undefined","sws_DLG_150")) > 0);
			case BTNID_AUTOSAVE:
				if (g_SNM_ResSlots.Get(g_resViewType)->IsAutoSave())
				{
					switch (typeForUser)
					{
						case SNM_SLOT_FXC:
							return (_snprintfStrict(_bufOut, _bufOutSz, __LOCALIZE_VERFMT("%s (right-click for options)\nto %s","sws_DLG_150"),
								g_asFXChainPref == FXC_AUTOSAVE_PREF_TRACK ? __LOCALIZE("Auto-save FX chains for selected tracks","sws_DLG_150") :
								g_asFXChainPref == FXC_AUTOSAVE_PREF_ITEM ? __LOCALIZE("Auto-save FX chains for selected items","sws_DLG_150") :
								g_asFXChainPref == FXC_AUTOSAVE_PREF_INPUT_FX ? __LOCALIZE("Auto-save input FX chains for selected tracks","sws_DLG_150")
									: __LOCALIZE("Auto-save FX chain slots","sws_DLG_150"),
								*GetAutoSaveDir() ? GetAutoSaveDir() : __LOCALIZE("undefined","sws_DLG_150")) > 0);
						case SNM_SLOT_TR:
							return (_snprintfStrict(_bufOut, _bufOutSz, __LOCALIZE_VERFMT("Auto-save track templates%s%s for selected tracks (right-click for options)\nto %s","sws_DLG_150"),
								(g_asTrTmpltPref&1) ? __LOCALIZE(" w/ items","sws_DLG_150") : "",
								(g_asTrTmpltPref&2) ? __LOCALIZE(" w/ envs","sws_DLG_150") : "",
								*GetAutoSaveDir() ? GetAutoSaveDir() : __LOCALIZE("undefined","sws_DLG_150")) > 0);
						default:
							return (_snprintfStrict(_bufOut, _bufOutSz, __LOCALIZE_VERFMT("Auto-save %s slots (right-click for options)\nto %s","sws_DLG_150"), 
								g_SNM_ResSlots.Get(typeForUser)->GetDesc(), 
								*GetAutoSaveDir() ? GetAutoSaveDir() : __LOCALIZE("undefined","sws_DLG_150")) > 0);
					}
				}
				break;
			case CMBID_TYPE:
				return (lstrcpyn(_bufOut, __LOCALIZE("Resource/slot type","sws_DLG_150"), _bufOutSz) != NULL);
			case BTNID_ADD_BOOKMARK:
				return (_snprintfStrict(_bufOut, _bufOutSz, __LOCALIZE_VERFMT("New %s bookmark","sws_DLG_150"), g_SNM_ResSlots.Get(typeForUser)->GetDesc()) > 0);
			case BTNID_DEL_BOOKMARK:
				return (lstrcpyn(_bufOut, __LOCALIZE("Delete bookmark (and files, if confirmed)","sws_DLG_150"), _bufOutSz) != NULL);
		}
	}
	return false;
}

void SNM_ResourceWnd::ClearListSelection()
{
	SWS_ListView* lv = m_pLists.Get(0);
	HWND hList = lv ? lv->GetHWND() : NULL;
	if (hList) // can be called when the view is closed!
		ListView_SetItemState(hList, -1, 0, LVIS_SELECTED);
}

// select a range of slots (contiguous, when the list view is not sorted) or a single slot (when _slot2 < 0)
void SNM_ResourceWnd::SelectBySlot(int _slot1, int _slot2, bool _selectOnly)
{
	SWS_ListView* lv = m_pLists.Get(0);
	HWND hList = lv ? lv->GetHWND() : NULL;
	if (lv && hList) // can be called when the view is closed!
	{
		int slot1=_slot1, slot2=_slot2;

		// check params
		if (_slot1 < 0)
			return;
		if (_slot2 < 0)
			slot2 = slot1;
		else if (_slot2 < _slot1) {
			slot1 = _slot2;
			slot2 = _slot1;
		}

		if (_selectOnly)
			ListView_SetItemState(hList, -1, 0, LVIS_SELECTED);

		int firstSel = -1;
		for (int i=0; i < lv->GetListItemCount(); i++)
		{
			PathSlotItem* item = (PathSlotItem*)lv->GetListItem(i);
			int slot = GetSlotList()->Find(item);
			if (item && slot >= _slot1 && slot <= slot2) 
			{
				if (firstSel < 0)
					firstSel = i;
				ListView_SetItemState(hList, i, LVIS_SELECTED, LVIS_SELECTED);
				if (_slot2 < 0) // one slot to be selected?
					break;
			}
		}
		if (firstSel >= 0)
			ListView_EnsureVisible(hList, firstSel, true);
	}
}

// gets selected slots and returns the number of non empty slots among them
void SNM_ResourceWnd::GetSelectedSlots(WDL_PtrList<PathSlotItem>* _selSlots, WDL_PtrList<PathSlotItem>* _selEmptySlots)
{
	int x=0;
	while (PathSlotItem* pItem = (PathSlotItem*)m_pLists.Get(0)->EnumSelected(&x))
	{
		if (_selEmptySlots && pItem->IsDefault())
			_selEmptySlots->Add(pItem);
		else
			_selSlots->Add(pItem);
	}
}

void SNM_ResourceWnd::AddSlot(bool _update)
{
	int idx = GetSlotList()->GetSize();
	if (GetSlotList()->Add(new PathSlotItem()) && _update) {
		Update();
		SelectBySlot(idx);
	}
}

void SNM_ResourceWnd::InsertAtSelectedSlot(bool _update)
{
	if (GetSlotList()->GetSize())
	{
		bool updt = false;
		if (PathSlotItem* item = (PathSlotItem*)m_pLists.Get(0)->EnumSelected(NULL))
		{
			int slot = GetSlotList()->Find(item);
			if (slot >= 0)
				updt = (GetSlotList()->InsertSlot(slot) != NULL);
			if (_update && updt) 
			{
				Update();
				SelectBySlot(slot);
				return; // <-- !!
			}
		}
	}
	AddSlot(_update); // empty list, no selection, etc.. => add
}

// _mode&1 = delete sel. slots, clear sel. slots otherwise
// _mode&2 = clear sel. slots or delete if empty & last sel. slots
// _mode&4 = delete files
// _mode&8 = delete all slots (exclusive with _mode&1 and _mode&2)
void SNM_ResourceWnd::ClearDeleteSlots(int _mode, bool _update)
{
	bool updt = false;
	int oldSz = GetSlotList()->GetSize();

	WDL_PtrList<int> slots;
	while (slots.GetSize() != GetSlotList()->GetSize())
		slots.Add(_mode&8 ? &g_i1 : &g_i0);

	int x=0;
	if ((_mode&8) != 8)
	{
		while(PathSlotItem* item = (PathSlotItem*)m_pLists.Get(0)->EnumSelected(&x)) {
			int slot = GetSlotList()->Find(item);
			slots.Delete(slot);
			slots.Insert(slot, &g_i1);
		}
	}

	int endSelSlot=-1;
	x=GetSlotList()->GetSize();
	while(x >= 0 && slots.Get(--x) == &g_i1) endSelSlot=x;

	char fullPath[SNM_MAX_PATH] = "";
	WDL_PtrList_DeleteOnDestroy<PathSlotItem> delItems; // DeleteOnDestroy: keep (displayed!) pointers until the list view is updated
	for (int slot=slots.GetSize()-1; slot >=0 ; slot--)
	{
		if (*slots.Get(slot)) // avoids new Find(item) calls
		{
			if (PathSlotItem* item = GetSlotList()->Get(slot))
			{
				if (_mode&4) {
					GetFullResourcePath(GetSlotList()->GetResourceDir(), item->m_shortPath.Get(), fullPath, sizeof(fullPath));
					SNM_DeleteFile(fullPath, true);
				}

				if (_mode&1 || _mode&8 || (_mode&2 && endSelSlot>=0 && slot>=endSelSlot)) 
				{
					slots.Delete(slot, false); // keep the sel list "in sync"
					GetSlotList()->Delete(slot, false); // remove slot - but no deleted pointer yet
					delItems.Add(item); // for later pointer deletion..
				}
				else
					GetSlotList()->ClearSlot(slot, false);
				updt = true;
			}
		}
	}

	if (_update && updt)
	{
		Update();
		if (oldSz != GetSlotList()->GetSize()) // deletion?
			ClearListSelection();
	}

} // + delItems auto clean-up !


///////////////////////////////////////////////////////////////////////////////

bool CheckSetAutoDirectory(const char* _title, int _type, bool _autoSave)
{
	WDL_FastString* dir = _autoSave ? g_autoSaveDirs.Get(_type) : g_autoFillDirs.Get(_type);
	if (!FileOrDirExists(dir->Get()))
	{
		char buf[SNM_MAX_PATH] = "";
		_snprintfSafe(buf, sizeof(buf), __LOCALIZE_VERFMT("%s directory not found!\n%s%sDo you want to define one ?","sws_DLG_150"), _title, dir->Get(), dir->GetLength()?"\n":"");
		if (IDYES == MessageBox(g_SNM_ResourcesWnd?g_SNM_ResourcesWnd->GetHWND():GetMainHwnd(), buf, __LOCALIZE("S&M - Warning","sws_DLG_150"), MB_YESNO)) {
			if (BrowseForDirectory(_autoSave ? __LOCALIZE("Set auto-save directory","sws_DLG_150") : __LOCALIZE("Set auto-fill directory","sws_DLG_150"), GetResourcePath(), buf, sizeof(buf))) {
				if (_autoSave) SetAutoSaveDir(buf, _type);
				else SetAutoFillDir(buf, _type);
			}
		}
		else
			return false;
		return FileOrDirExists(dir->Get()); // re-check (browse cancelled, etc..)
	}
	return true;
}

bool AutoSaveChunkSlot(const void* _obj, const char* _fn) {
	return SaveChunk(_fn, (WDL_FastString*)_obj, true);
}

// overwitres a slot -or- genenerates a new filename + adds or replaces a slot
// just a helper func for AutoSaveTrackSlots(), AutoSaveMediaSlot(), etc..
// _owSlots: slots to overwrite, if any
bool AutoSaveSlot(int _slotType, const char* _dirPath, 
				  const char* _name, const char* _ext,
				  WDL_PtrList<PathSlotItem>* _owSlots, int* _owIdx, 
				  bool (*SaveSlot)(const void*, const char*), const void* _obj) // see AutoSaveChunkSlot() example
{
	bool saved = false;
	char fn[SNM_MAX_PATH] = "";
	int todo = 1; // 0: nothing, 1: gen filename + add slot, 2: gen filename + replace slot

	if (*_owIdx < _owSlots->GetSize() && _owSlots->Get(*_owIdx))
	{
		// gets the filename to overwrite
		GetFullResourcePath(g_SNM_ResSlots.Get(_slotType)->GetResourceDir(), _owSlots->Get(*_owIdx)->m_shortPath.Get(), fn, sizeof(fn));
		if (!*fn)
			todo=2;
		else
		{
			todo=0;
			if (_stricmp(_ext, GetFileExtension(fn))) // corner case, ex: try to write .mid over .wav
				if (char* p = strrchr(fn, '.'))
				{
					strcpy(p+1, _ext);
					char shortPath[SNM_MAX_PATH] = "";
					GetShortResourcePath(g_SNM_ResSlots.Get(_slotType)->GetResourceDir(), fn, shortPath, sizeof(shortPath));
					_owSlots->Get(*_owIdx)->m_shortPath.Set(shortPath);
				}
			saved = (SaveSlot ? SaveSlot(_obj, fn) : SNM_CopyFile(fn, _name));
		}
		*_owIdx = *_owIdx + 1;
	}

	if (todo)
	{
		char fnNoExt[SNM_MAX_PATH] = "";
		GetFilenameNoExt(_name, fnNoExt, sizeof(fnNoExt));
		Filenamize(fnNoExt, sizeof(fnNoExt));
		if (GenerateFilename(_dirPath, fnNoExt, _ext, fn, sizeof(fn)) && (SaveSlot ? SaveSlot(_obj, fn) : SNM_CopyFile(fn, _name)))
		{
			saved = true;
			if (todo==1)
				g_SNM_ResSlots.Get(_slotType)->AddSlot(fn);
			else  {
				char shortPath[SNM_MAX_PATH] = "";
				GetShortResourcePath(g_SNM_ResSlots.Get(_slotType)->GetResourceDir(), fn, shortPath, sizeof(shortPath));
				_owSlots->Get(*_owIdx-1)->m_shortPath.Set(shortPath);
			}
		}
	}
	return saved;
}

// _promptOverwrite:
//    - e.g. false for auto-save *actions*, true for auto-save *button*
// _flags:
//    - for track templates: _flags&1 save template with items, _flags&2 save template with envs
//    - for fx chains: enum FXC_AUTOSAVE_PREF_INPUT_FX, FXC_AUTOSAVE_PREF_TRACK and FXC_AUTOSAVE_PREF_ITEM
//    - n/a otherwise
void AutoSave(int _type, bool _promptOverwrite, int _flags)
{
	WDL_PtrList<PathSlotItem> owSlots; // slots to overwrite
	WDL_PtrList<PathSlotItem> selFilledSlots;

	// overwrite non empty slots?
	int ow = IDNO;
	if (g_SNM_ResourcesWnd)
		g_SNM_ResourcesWnd->GetSelectedSlots(&selFilledSlots, &owSlots); // &owSlots: empty slots are systematically overwritten

	if (selFilledSlots.GetSize() && _promptOverwrite)
		ow = MessageBox(g_SNM_ResourcesWnd?g_SNM_ResourcesWnd->GetHWND():GetMainHwnd(), __LOCALIZE("Some selected slots are already filled, do you want to overwrite them?\n\nReplying \"Yes\" will overwite filled slots/files.\nReplying \"No\" will add new slots/files.","sws_DLG_150"), __LOCALIZE("S&M - Confirmation","sws_DLG_150"), MB_YESNOCANCEL);

	if (ow == IDYES)
	{
		for (int i=0; i<selFilledSlots.GetSize(); i++)
			owSlots.Add(selFilledSlots.Get(i));
	}
	else if (ow == IDCANCEL)
		return;

	if (!CheckSetAutoDirectory(__LOCALIZE("Auto-save","sws_DLG_150"), _type, true))
		return;

	bool saved = false;
	int oldNbSlots = g_SNM_ResSlots.Get(_type)->GetSize();
	switch(GetTypeForUser(_type))
	{
		case SNM_SLOT_FXC:
			switch (_flags) {
				case FXC_AUTOSAVE_PREF_INPUT_FX:
				case FXC_AUTOSAVE_PREF_TRACK:
					saved = AutoSaveTrackFXChainSlots(_type, g_autoSaveDirs.Get(_type)->Get(), &owSlots, g_asFXChainNamePref==1, _flags==FXC_AUTOSAVE_PREF_INPUT_FX);
					break;
				case FXC_AUTOSAVE_PREF_ITEM:
					saved = AutoSaveItemFXChainSlots(_type, g_autoSaveDirs.Get(_type)->Get(), &owSlots, g_asFXChainNamePref==1);
					break;
			}
			break;
		case SNM_SLOT_TR:
			saved = AutoSaveTrackSlots(_type, g_autoSaveDirs.Get(_type)->Get(), &owSlots, (_flags&1)?false:true, (_flags&2)?false:true);
			break;
		case SNM_SLOT_PRJ:
			saved = AutoSaveProjectSlot(_type, g_autoSaveDirs.Get(_type)->Get(), &owSlots, true);
			break;
		case SNM_SLOT_MEDIA:
			saved = AutoSaveMediaSlots(_type, g_autoSaveDirs.Get(_type)->Get(), &owSlots);
			break;
	}

	if (saved)
	{
		if (g_SNM_ResourcesWnd && g_resViewType==_type )
		{
			g_SNM_ResourcesWnd->Update();

			// sel new slots
			bool first = true;
			if (oldNbSlots != g_SNM_ResSlots.Get(_type)->GetSize()) {
				g_SNM_ResourcesWnd->SelectBySlot(oldNbSlots, g_SNM_ResSlots.Get(_type)->GetSize(), first);
				first = false;
			}
			// sel overwriten slots
			for (int i=0; i<owSlots.GetSize(); i++)
			{
				int slot = g_SNM_ResSlots.Get(_type)->Find(owSlots.Get(i));
				if (slot >= 0) {
					g_SNM_ResourcesWnd->SelectBySlot(slot, slot, first);
					first = false;
				}
			}
		}
	}
	else
	{
		char msg[SNM_MAX_PATH];
		_snprintfSafe(msg, sizeof(msg), __LOCALIZE_VERFMT("Auto-save failed!\n%s","sws_DLG_150"), AUTOSAVE_ERR_STR);
		MessageBox(g_SNM_ResourcesWnd?g_SNM_ResourcesWnd->GetHWND():GetMainHwnd(), msg, __LOCALIZE("S&M - Error","sws_DLG_150"), MB_OK);
	}
}

// recursive from _path
void AutoFill(int _type)
{
	if (!CheckSetAutoDirectory(__LOCALIZE("Auto-fill","sws_DLG_150"), _type, false))
		return;

	int startSlot = g_SNM_ResSlots.Get(_type)->GetSize();
	WDL_PtrList_DeleteOnDestroy<WDL_String> files; 
	ScanFiles(&files, g_autoFillDirs.Get(_type)->Get(), g_SNM_ResSlots.Get(_type)->GetFileExt(), true);
	if (int sz = files.GetSize())
		for (int i=0; i < sz; i++)
			if (g_SNM_ResSlots.Get(_type)->FindByResFulltPath(files.Get(i)->Get()) < 0) // skip if already present
				g_SNM_ResSlots.Get(_type)->AddSlot(files.Get(i)->Get());

	if (startSlot != g_SNM_ResSlots.Get(_type)->GetSize())
	{
		if (g_SNM_ResourcesWnd && g_resViewType==_type) {
			g_SNM_ResourcesWnd->Update();
			g_SNM_ResourcesWnd->SelectBySlot(startSlot, g_SNM_ResSlots.Get(_type)->GetSize());
		}
	}
	else
	{
		const char* path = g_autoFillDirs.Get(_type)->Get();
		char msg[SNM_MAX_PATH]="";
		if (path && *path) _snprintfSafe(msg, sizeof(msg), __LOCALIZE_VERFMT("No slot added from: %s\n%s","sws_DLG_150"), path, AUTOFILL_ERR_STR);
		else _snprintfSafe(msg, sizeof(msg), __LOCALIZE_VERFMT("No slot added!\n%s","sws_DLG_150"), AUTOFILL_ERR_STR);
		MessageBox(g_SNM_ResourcesWnd?g_SNM_ResourcesWnd->GetHWND():GetMainHwnd(), msg, __LOCALIZE("S&M - Warning","sws_DLG_150"), MB_OK);
	}
}


///////////////////////////////////////////////////////////////////////////////

void NewBookmark(int _type, bool _copyCurrent)
{
	if (g_SNM_ResSlots.GetSize() < SNM_MAX_SLOT_TYPES)
	{
		// consistency update (just in case..)
		_type = (_copyCurrent ? g_resViewType : GetTypeForUser(_type));

		char name[64] = "";
		_snprintfSafe(name, sizeof(name), __LOCALIZE_VERFMT("My %s slots","sws_DLG_150"), g_SNM_ResSlots.Get(GetTypeForUser(_type))->GetDesc());
		if (PromptUserForString(g_SNM_ResourcesWnd?g_SNM_ResourcesWnd->GetHWND():GetMainHwnd(), __LOCALIZE("S&M - Add bookmark","sws_DLG_150"), name, 64, true) && *name)
		{
			int newType = g_SNM_ResSlots.GetSize();
			g_SNM_ResSlots.Add(new FileSlotList(
				g_SNM_ResSlots.Get(_type)->GetResourceDir(), name, g_SNM_ResSlots.Get(_type)->GetFileExt(), 
				g_SNM_ResSlots.Get(_type)->GetFlags()));

			if (_copyCurrent)
			{
				g_autoSaveDirs.Add(new WDL_FastString(GetAutoSaveDir(_type)));
				g_autoFillDirs.Add(new WDL_FastString(GetAutoFillDir(_type)));
			}
			else
			{
				char path[SNM_MAX_PATH] = "";
				if (_snprintfStrict(path, sizeof(path), "%s%c%s", GetResourcePath(), PATH_SLASH_CHAR, g_SNM_ResSlots.Get(_type)->GetResourceDir()) > 0) {
					if (!FileOrDirExists(path))
						CreateDirectory(path, NULL);
				}
				else *path = '\0';
				g_autoSaveDirs.Add(new WDL_FastString(path));
				g_autoFillDirs.Add(new WDL_FastString(path));
			}
			g_syncAutoDirPrefs[newType] = _copyCurrent ? g_syncAutoDirPrefs[_type] : true;
			g_dblClickPrefs[newType] = _copyCurrent ? g_dblClickPrefs[_type] : 0;

			if (_copyCurrent)
				for (int i=0; i < GetSlotList()->GetSize(); i++)
					if (PathSlotItem* slotItem = GetSlotList()->Get(i))
						g_SNM_ResSlots.Get(newType)->AddSlot(slotItem->m_shortPath.Get(), slotItem->m_comment.Get());

			if (g_SNM_ResourcesWnd) {
				g_SNM_ResourcesWnd->FillTypeCombo();
				g_SNM_ResourcesWnd->SetType(newType); // + GUI update
			}

			// ... custom slot type definitions are saved when exiting
		}
	}
	else
		MessageBox(g_SNM_ResourcesWnd?g_SNM_ResourcesWnd->GetHWND():GetMainHwnd(), __LOCALIZE("Too many resource types!","sws_DLG_150"), __LOCALIZE("S&M - Error","sws_DLG_150"), MB_OK);
}

void DeleteBookmark(int _bookmarkType)
{
	if (_bookmarkType >= SNM_NUM_DEFAULT_SLOTS)
	{
		int reply = IDNO;
		if (g_SNM_ResSlots.Get(_bookmarkType)->GetSize()) // do not ask if empty
		{
			char title[128] = "";
			_snprintfSafe(title, sizeof(title), __LOCALIZE_VERFMT("S&M - Delete bookmark \"%s\"","sws_DLG_150"), g_SNM_ResSlots.Get(_bookmarkType)->GetDesc());
			reply = MessageBox(g_SNM_ResourcesWnd?g_SNM_ResourcesWnd->GetHWND():GetMainHwnd(), __LOCALIZE("Delete files too?","sws_DLG_150"), title, MB_YESNOCANCEL);
		}
		if (reply != IDCANCEL)
		{
			// cleanup ini file (types may not be contiguous anymore..)
			FlushCustomTypesIniFile();
			if (g_SNM_ResourcesWnd)
				g_SNM_ResourcesWnd->ClearDeleteSlots(8 | (reply==IDYES?4:0), true); // true so that deleted items are not displayed anymore

			// remove current slot type
			int oldType = _bookmarkType;
			int oldTypeForUser = GetTypeForUser(oldType);
			g_dblClickPrefs[oldType] = 0;
			if (g_SNM_TiedSlotActions[oldTypeForUser] == oldType)
				g_SNM_TiedSlotActions[oldTypeForUser] = oldTypeForUser;
			g_autoSaveDirs.Delete(oldType, true);
			g_autoFillDirs.Delete(oldType, true);
			g_syncAutoDirPrefs[oldType] = true;
			g_SNM_ResSlots.Delete(oldType, true);

			if (g_SNM_ResourcesWnd) {
				g_SNM_ResourcesWnd->FillTypeCombo();
				g_SNM_ResourcesWnd->SetType(oldType-1); // + GUI update
			}
		}
	}
}

void RenameBookmark(int _bookmarkType)
{
	if (_bookmarkType >= SNM_NUM_DEFAULT_SLOTS)
	{
		char newName[64] = "";
		lstrcpyn(newName, g_SNM_ResSlots.Get(_bookmarkType)->GetDesc(), 64);
		if (PromptUserForString(g_SNM_ResourcesWnd?g_SNM_ResourcesWnd->GetHWND():GetMainHwnd(), __LOCALIZE("S&M - Rename bookmark","sws_DLG_150"), newName, 64, true) && *newName)
		{
			g_SNM_ResSlots.Get(_bookmarkType)->SetDesc(newName);
			if (g_SNM_ResourcesWnd) {
				g_SNM_ResourcesWnd->FillTypeCombo();
				g_SNM_ResourcesWnd->Update();
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////

void FlushCustomTypesIniFile()
{
	char iniSec[64]="";
	for (int i=SNM_NUM_DEFAULT_SLOTS; i < g_SNM_ResSlots.GetSize(); i++)
	{
		if (g_SNM_ResSlots.Get(i))
		{
			GetIniSectionName(i, iniSec, sizeof(iniSec));
			WritePrivateProfileStruct(iniSec, NULL, NULL, 0, g_SNM_IniFn.Get()); // flush section
			WritePrivateProfileString(RES_INI_SEC, iniSec, NULL, g_SNM_IniFn.Get());
			WDL_FastString str;
			str.SetFormatted(64, "AutoFillDir%s", iniSec);
			WritePrivateProfileString(RES_INI_SEC, str.Get(), NULL, g_SNM_IniFn.Get());
			str.SetFormatted(64, "AutoSaveDir%s", iniSec);
			WritePrivateProfileString(RES_INI_SEC, str.Get(), NULL, g_SNM_IniFn.Get());
			str.SetFormatted(64, "SyncAutoDirs%s", iniSec);
			WritePrivateProfileString(RES_INI_SEC, str.Get(), NULL, g_SNM_IniFn.Get());
			str.SetFormatted(64, "TiedActions%s", iniSec);
			WritePrivateProfileString(RES_INI_SEC, str.Get(), NULL, g_SNM_IniFn.Get());
			str.SetFormatted(64, "DblClick%s", iniSec);
			WritePrivateProfileString(RES_INI_SEC, str.Get(), NULL, g_SNM_IniFn.Get());
		}
	}
}

void ReadSlotIniFile(const char* _key, int _slot, char* _path, int _pathSize, char* _desc, int _descSize)
{
	char buf[32];
	if (_snprintfStrict(buf, sizeof(buf), "Slot%d", _slot+1) > 0)
		GetPrivateProfileString(_key, buf, "", _path, _pathSize, g_SNM_IniFn.Get());
	if (_snprintfStrict(buf, sizeof(buf), "Desc%d", _slot+1) > 0)
		GetPrivateProfileString(_key, buf, "", _desc, _descSize, g_SNM_IniFn.Get());
}

// adds bookmarks and custom slot types from the S&M.ini file, example: 
// CustomSlotType1="Data\track_icons,track icons,png"
// CustomSlotType2="TextFiles,text file,txt"
void AddCustomTypesFromIniFile()
{
	int i=0;
	char buf[SNM_MAX_PATH]=""; // can be used with paths..
	WDL_String iniKeyStr("CustomSlotType1"), tokenStrs[2];
	GetPrivateProfileString(RES_INI_SEC, iniKeyStr.Get(), "", buf, sizeof(buf), g_SNM_IniFn.Get());
	while (*buf && i < SNM_MAX_SLOT_TYPES)
	{
		if (char* tok = strtok(buf, ","))
		{
			if (tok[strlen(tok)-1] == PATH_SLASH_CHAR) tok[strlen(tok)-1] = '\0';
			tokenStrs[0].Set(tok);
			if (tok = strtok(NULL, ","))
			{
				tokenStrs[1].Set(tok);
				tok = strtok(NULL, ","); // no NULL test here (special case for media files..)
				g_SNM_ResSlots.Add(new FileSlotList(
					tokenStrs[0].Get(), 
					tokenStrs[1].Get(), 
					tok?tok:"", 
					0));

				// known slot type?
				int newType = g_SNM_ResSlots.GetSize()-1;
				int typeForUser = GetTypeForUser(newType);
				if (newType != typeForUser) // this is a known type => enable some stuff
					g_SNM_ResSlots.Get(newType)->SetFlags(g_SNM_ResSlots.Get(typeForUser)->GetFlags());
			}
		}
		iniKeyStr.SetFormatted(32, "CustomSlotType%d", (++i)+1);
		GetPrivateProfileString(RES_INI_SEC, iniKeyStr.Get(), "", buf, sizeof(buf), g_SNM_IniFn.Get());
	}
}

int ResourcesInit()
{
	// localization
	g_filter.Set(FILTER_DEFAULT_STR);

	g_SNM_ResSlots.Empty(true);

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// slot definitions
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	g_SNM_ResSlots.Add(new FileSlotList("FXChains",			__LOCALIZE("FX chain","sws_DLG_150"),		"RfxChain",			MASK_AUTOFILL|MASK_AUTOSAVE|MASK_DBLCLIK|MASK_TEXT));
	g_SNM_ResSlots.Add(new FileSlotList("TrackTemplates",		__LOCALIZE("track template","sws_DLG_150"),	"RTrackTemplate",	MASK_AUTOFILL|MASK_AUTOSAVE|MASK_DBLCLIK|MASK_TEXT));
	g_SNM_ResSlots.Add(new FileSlotList("ProjectTemplates",	__LOCALIZE("project","sws_DLG_150"),		"RPP",				MASK_AUTOFILL|MASK_AUTOSAVE|MASK_DBLCLIK|MASK_TEXT));
	g_SNM_ResSlots.Add(new FileSlotList("MediaFiles",			__LOCALIZE("media file","sws_DLG_150"),		"",					MASK_AUTOFILL|MASK_AUTOSAVE|MASK_DBLCLIK));	// "" means 'all supported media file extensions'
#ifdef _WIN32
	g_SNM_ResSlots.Add(new FileSlotList("Data\\track_icons",	__LOCALIZE("PNG image","sws_DLG_150"),		"png",				MASK_AUTOFILL|MASK_DBLCLIK));
#else
	g_SNM_ResSlots.Add(new FileSlotList("Data/track_icons",	__LOCALIZE("PNG image","sws_DLG_150"),		"png",				MASK_AUTOFILL|MASK_DBLCLIK));
#endif
	/////////////////////////////////////////////////////////
	// -> add new resource types here..
	//    + related enum on top of the .h file
	// note: the descs MUST be singular and in lower case
	/////////////////////////////////////////////////////////
#ifdef _WIN32
	// keep this item as the last one! (due to win only..)
	g_SNM_ResSlots.Add(new FileSlotList("ColorThemes",			__LOCALIZE("theme","sws_DLG_150"),			"ReaperthemeZip",	MASK_AUTOFILL|MASK_DBLCLIK));
#endif
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	AddCustomTypesFromIniFile();

	// load general prefs
	g_resViewType = BOUNDED((int)GetPrivateProfileInt(
		RES_INI_SEC, "Type", SNM_SLOT_FXC, g_SNM_IniFn.Get()), SNM_SLOT_FXC, g_SNM_ResSlots.GetSize()-1); // bounded for safety (some custom slot types may have been removed..)

	g_filterPref = GetPrivateProfileInt(RES_INI_SEC, "Filter", 1, g_SNM_IniFn.Get());
	g_asFXChainPref = GetPrivateProfileInt(RES_INI_SEC, "AutoSaveFXChain", FXC_AUTOSAVE_PREF_TRACK, g_SNM_IniFn.Get());
	g_asFXChainNamePref = GetPrivateProfileInt(RES_INI_SEC, "AutoSaveFXChainName", 0, g_SNM_IniFn.Get());
	g_asTrTmpltPref = GetPrivateProfileInt(RES_INI_SEC, "AutoSaveTrTemplate", 3, g_SNM_IniFn.Get());
	g_SNM_PrjLoaderStartPref = GetPrivateProfileInt(RES_INI_SEC, "ProjectLoaderStartSlot", -1, g_SNM_IniFn.Get());
	g_SNM_PrjLoaderEndPref = GetPrivateProfileInt(RES_INI_SEC, "ProjectLoaderEndSlot", -1, g_SNM_IniFn.Get());

	// auto-save, auto-fill directories, etc..
	g_autoSaveDirs.Empty(true);
	g_autoFillDirs.Empty(true);
	memset(g_dblClickPrefs, 0, SNM_MAX_SLOT_TYPES*sizeof(int));
//	for (int i=0; i < SNM_NUM_DEFAULT_SLOTS; i++) g_SNM_TiedSlotActions[i]=i;

	char defaultPath[SNM_MAX_PATH]="", path[SNM_MAX_PATH]="", iniSec[64]="", iniKey[64]="";
	for (int i=0; i < g_SNM_ResSlots.GetSize(); i++)
	{
		GetIniSectionName(i, iniSec, sizeof(iniSec));
		if (_snprintfStrict(defaultPath, sizeof(defaultPath), "%s%c%s", GetResourcePath(), PATH_SLASH_CHAR, g_SNM_ResSlots.Get(i)->GetResourceDir()) < 0)
			*defaultPath = '\0';

		// g_autoFillDirs & g_autoSaveDirs must always be filled (even if auto-save is grayed for the user)
		WDL_FastString *fillPath, *savePath;
		if (_snprintfStrict(iniKey, sizeof(iniKey), "AutoSaveDir%s", iniSec) > 0) {
			GetPrivateProfileString(RES_INI_SEC, iniKey, defaultPath, path, sizeof(path), g_SNM_IniFn.Get());
			savePath = new WDL_FastString(path);
		}
		else
			savePath = new WDL_FastString(defaultPath);
		g_autoSaveDirs.Add(savePath);

		if (_snprintfStrict(iniKey, sizeof(iniKey), "AutoFillDir%s", iniSec) > 0) {
			GetPrivateProfileString(RES_INI_SEC, iniKey, defaultPath, path, sizeof(path), g_SNM_IniFn.Get());
			fillPath = new WDL_FastString(path);
		}
		else
			fillPath = new WDL_FastString(defaultPath);
		g_autoFillDirs.Add(fillPath);

		if (_snprintfStrict(iniKey, sizeof(iniKey), "SyncAutoDirs%s", iniSec) > 0)
			g_syncAutoDirPrefs[i] = (GetPrivateProfileInt(RES_INI_SEC, iniKey, 0, g_SNM_IniFn.Get()) == 1);
		else
			g_syncAutoDirPrefs[i] = false;
		if (g_syncAutoDirPrefs[i]) // consistency check (e.g. after sws upgrade)
			g_syncAutoDirPrefs[i] = (strcmp(savePath->Get(), fillPath->Get()) == 0);

		if (g_SNM_ResSlots.Get(i)->IsDblClick()) {
			if (_snprintfStrict(iniKey, sizeof(iniKey), "DblClick%s", iniSec) > 0)
				g_dblClickPrefs[i] = GetPrivateProfileInt(RES_INI_SEC, iniKey, 0, g_SNM_IniFn.Get());
			else
				g_dblClickPrefs[i] = 0;
		}
		// load tied actions for default types (fx chains, track templates, etc...)
		if (i < SNM_NUM_DEFAULT_SLOTS) {
			if (_snprintfStrict(iniKey, sizeof(iniKey), "TiedActions%s", iniSec) > 0)
				g_SNM_TiedSlotActions[i] = GetPrivateProfileInt(RES_INI_SEC, iniKey, i, g_SNM_IniFn.Get());
			else
				g_SNM_TiedSlotActions[i] = i;
		}
	}

	// read slots from ini file
	char desc[128]="", maxSlotCount[16]="";
	for (int i=0; i < g_SNM_ResSlots.GetSize(); i++)
	{
		if (FileSlotList* list = g_SNM_ResSlots.Get(i))
		{
			GetIniSectionName(i, iniSec, sizeof(iniSec));
			GetPrivateProfileString(iniSec, "Max_slot", "0", maxSlotCount, sizeof(maxSlotCount), g_SNM_IniFn.Get()); 
			list->EmptySafe(true);
			int cnt = atoi(maxSlotCount);
			for (int j=0; j<cnt; j++) {
				ReadSlotIniFile(iniSec, j, path, sizeof(path), desc, sizeof(desc));
				list->Add(new PathSlotItem(path, desc));
			}
		}
	}

	// instanciate the window, if needed
	if (SWS_LoadDockWndState("SnMResources"))
		g_SNM_ResourcesWnd = new SNM_ResourceWnd();

	return 1;
}

void ResourcesExit()
{
	WDL_FastString iniStr, escapedStr;
	WDL_PtrList_DeleteOnDestroy<WDL_FastString> iniSections;
	GetIniSectionNames(&iniSections);


	// *** save the main ini section

	iniStr.Set("");

	// save custom slot type definitions
	for (int i=0; i < g_SNM_ResSlots.GetSize(); i++)
	{
		if (i >= SNM_NUM_DEFAULT_SLOTS)
		{
			WDL_FastString str;
			str.SetFormatted(SNM_MAX_PATH, "%s,%s,%s", g_SNM_ResSlots.Get(i)->GetResourceDir(), g_SNM_ResSlots.Get(i)->GetDesc(), g_SNM_ResSlots.Get(i)->GetFileExt());
			makeEscapedConfigString(str.Get(), &escapedStr);
			iniStr.AppendFormatted(SNM_MAX_PATH, "%s=%s\n", iniSections.Get(i)->Get(), escapedStr.Get());
		}
	}
	iniStr.AppendFormatted(256, "Type=%d\n", g_resViewType);
	iniStr.AppendFormatted(256, "Filter=%d\n", g_filterPref);

	// save slot type prefs
	for (int i=0; i < g_SNM_ResSlots.GetSize(); i++)
	{
		// save auto-fill & auto-save paths
		if (g_autoFillDirs.Get(i)->GetLength()) {
			makeEscapedConfigString(g_autoFillDirs.Get(i)->Get(), &escapedStr);
			iniStr.AppendFormatted(SNM_MAX_PATH, "AutoFillDir%s=%s\n", iniSections.Get(i)->Get(), escapedStr.Get());
		}
		if (g_autoSaveDirs.Get(i)->GetLength() && g_SNM_ResSlots.Get(i)->IsAutoSave()) {
			makeEscapedConfigString(g_autoSaveDirs.Get(i)->Get(), &escapedStr);
			iniStr.AppendFormatted(SNM_MAX_PATH, "AutoSaveDir%s=%s\n", iniSections.Get(i)->Get(), escapedStr.Get());
		}
		iniStr.AppendFormatted(256, "SyncAutoDirs%s=%d\n", iniSections.Get(i)->Get(), g_syncAutoDirPrefs[i]);

		// save tied actions for default types (fx chains, track templates, etc...)
		if (i < SNM_NUM_DEFAULT_SLOTS)
			iniStr.AppendFormatted(256, "TiedActions%s=%d\n", iniSections.Get(i)->Get(), g_SNM_TiedSlotActions[i]);

		// dbl-click pref
		if (g_SNM_ResSlots.Get(i)->IsDblClick())
			iniStr.AppendFormatted(256, "DblClick%s=%d\n", iniSections.Get(i)->Get(), g_dblClickPrefs[i]);

		// specific options (saved here for the ini file ordering..)
		switch (i) {
			case SNM_SLOT_FXC:
				iniStr.AppendFormatted(256, "AutoSaveFXChain=%d\n", g_asFXChainPref);
				iniStr.AppendFormatted(256, "AutoSaveFXChainName=%d\n", g_asFXChainNamePref);
				break;
			case SNM_SLOT_TR:
				iniStr.AppendFormatted(256, "AutoSaveTrTemplate=%d\n", g_asTrTmpltPref);
				break;
			case SNM_SLOT_PRJ:
				iniStr.AppendFormatted(256, "ProjectLoaderStartSlot=%d\n", g_SNM_PrjLoaderStartPref);
				iniStr.AppendFormatted(256, "ProjectLoaderEndSlot=%d\n", g_SNM_PrjLoaderEndPref);
				break;
		}
	}
	SaveIniSection(RES_INI_SEC, &iniStr, g_SNM_IniFn.Get());


	// *** save slots ini sections

	for (int i=0; i < g_SNM_ResSlots.GetSize(); i++)
	{
		iniStr.SetFormatted(256, "Max_slot=%d\n", g_SNM_ResSlots.Get(i)->GetSize());
		for (int j=0; j < g_SNM_ResSlots.Get(i)->GetSize(); j++) {
			if (PathSlotItem* item = g_SNM_ResSlots.Get(i)->Get(j)) {
				if (item->m_shortPath.GetLength()) {
					makeEscapedConfigString(item->m_shortPath.Get(), &escapedStr);
					iniStr.AppendFormatted(SNM_MAX_PATH, "Slot%d=%s\n", j+1, escapedStr.Get());
				}
				if (item->m_comment.GetLength()) {
					makeEscapedConfigString(item->m_comment.Get(), &escapedStr);
					iniStr.AppendFormatted(256, "Desc%d=%s\n", j+1, escapedStr.Get());
				}
			}
		}
		// write things in one go (avoid to slow down REAPER shutdown)
		SaveIniSection(iniSections.Get(i)->Get(), &iniStr, g_SNM_IniFn.Get());
	}

	DELETE_NULL(g_SNM_ResourcesWnd);
}

void OpenResources(COMMAND_T* _ct)
{
	if (!g_SNM_ResourcesWnd)
		g_SNM_ResourcesWnd = new SNM_ResourceWnd();
	if (g_SNM_ResourcesWnd) 
	{
		int newType = (int)_ct->user; // -1 means toggle current type
		if (newType == -1)
			newType = g_resViewType;
		g_SNM_ResourcesWnd->Show((g_resViewType == newType) /* i.e toggle */, true);
		g_SNM_ResourcesWnd->SetType(newType);
//		SetFocus(GetDlgItem(g_SNM_ResourcesWnd->GetHWND(), IDC_FILTER));
	}
}

int IsResourcesDisplayed(COMMAND_T* _ct) {
	return (g_SNM_ResourcesWnd && g_SNM_ResourcesWnd->IsValidWindow());
}

void ResourcesDeleteAllSlots(COMMAND_T* _ct)
{
	FileSlotList* fl = g_SNM_ResSlots.Get(g_SNM_TiedSlotActions[(int)_ct->user]);
	WDL_PtrList_DeleteOnDestroy<PathSlotItem> delItems; // keep (displayed!) pointers until the list view is updated
	for (int i=fl->GetSize()-1; i >= 0; i--) {
		delItems.Add(fl->Get(i));
		fl->Delete(i, false);
	}
	if (g_SNM_ResourcesWnd && delItems.GetSize())
		g_SNM_ResourcesWnd->Update();
} // + delItems auto clean-up !

void ResourcesClearSlotPrompt(COMMAND_T* _ct) {
	g_SNM_ResSlots.Get(g_SNM_TiedSlotActions[(int)_ct->user])->ClearSlotPrompt(_ct);
}

void ResourcesClearFXChainSlot(COMMAND_T* _ct) {
	g_SNM_ResSlots.Get(g_SNM_TiedSlotActions[SNM_SLOT_FXC])->ClearSlot((int)_ct->user);
}

void ResourcesClearTrTemplateSlot(COMMAND_T* _ct) {
	g_SNM_ResSlots.Get(g_SNM_TiedSlotActions[SNM_SLOT_TR])->ClearSlot((int)_ct->user);
}

void ResourcesClearPrjTemplateSlot(COMMAND_T* _ct) {
	g_SNM_ResSlots.Get(g_SNM_TiedSlotActions[SNM_SLOT_PRJ])->ClearSlot((int)_ct->user);
}

void ResourcesClearMediaSlot(COMMAND_T* _ct) {
	//JFB TODO? stop sound?
	g_SNM_ResSlots.Get(g_SNM_TiedSlotActions[SNM_SLOT_MEDIA])->ClearSlot((int)_ct->user);
}

void ResourcesClearImageSlot(COMMAND_T* _ct) {
	g_SNM_ResSlots.Get(g_SNM_TiedSlotActions[SNM_SLOT_IMG])->ClearSlot((int)_ct->user);
}

#ifdef _WIN32
void ResourcesClearThemeSlot(COMMAND_T* _ct) {
	g_SNM_ResSlots.Get(g_SNM_TiedSlotActions[SNM_SLOT_THM])->ClearSlot((int)_ct->user);
}
#endif

// specific auto-save for fx chains
void ResourcesAutoSaveFXChain(COMMAND_T* _ct) {
	AutoSave(g_SNM_TiedSlotActions[SNM_SLOT_FXC], false, (int)_ct->user);
}

// specific auto-save for track templates
void ResourcesAutoSaveTrTemplate(COMMAND_T* _ct) {
	AutoSave(g_SNM_TiedSlotActions[SNM_SLOT_TR], false, (int)_ct->user);
}

// auto-save for all other types..
void ResourcesAutoSave(COMMAND_T* _ct) {
	int type = (int)_ct->user;
	if (g_SNM_ResSlots.Get(type)->IsAutoSave()) {
		AutoSave(g_SNM_TiedSlotActions[type], false, 0);
	}
}


///////////////////////////////////////////////////////////////////////////////
// ReaScript export
///////////////////////////////////////////////////////////////////////////////

int SNM_SelectResourceBookmark(const char* _name) {
		return g_SNM_ResourcesWnd ? g_SNM_ResourcesWnd->SetType(_name) : -1;
}

void SNM_TieResourceSlotActions(int _bookmarkId) {
	int typeForUser = _bookmarkId>=0 ? GetTypeForUser(_bookmarkId) : -1;
	if (typeForUser>=0 && typeForUser<SNM_NUM_DEFAULT_SLOTS)
		g_SNM_TiedSlotActions[typeForUser] = _bookmarkId;
}


///////////////////////////////////////////////////////////////////////////////
// SNM_ImageWnd
///////////////////////////////////////////////////////////////////////////////

#define IMGID	2000 //JFB would be great to have _APS_NEXT_CONTROL_VALUE *always* defined

SNM_ImageWnd* g_pImageWnd = NULL;

SNM_ImageWnd::SNM_ImageWnd()
	: SWS_DockWnd(IDD_SNM_IMAGE, __LOCALIZE("Image","sws_DLG_162"), "SnMImage", SWSGetCommandID(OpenImageWnd))
{
	m_stretch = false;

	// Must call SWS_DockWnd::Init() to restore parameters and open the window if necessary
	Init();
}

void SNM_ImageWnd::OnInitDlg()
{
	m_vwnd_painter.SetGSC(WDL_STYLE_GetSysColor);
	m_parentVwnd.SetRealParent(m_hwnd);
	
	m_img.SetID(IMGID);
	m_parentVwnd.AddChild(&m_img);
	RequestRedraw();
}

void SNM_ImageWnd::OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch(LOWORD(wParam))
	{
		case 0xF001:
			m_stretch = !m_stretch;
			RequestRedraw();
			break;
		default:
			Main_OnCommand((int)wParam, (int)lParam);
			break;
	}
}

HMENU SNM_ImageWnd::OnContextMenu(int x, int y, bool* wantDefaultItems)
{
	HMENU hMenu = CreatePopupMenu();
	AddToMenu(hMenu, __LOCALIZE("Stretch to fit","sws_DLG_162"), 0xF001, -1, false, m_stretch?MFS_CHECKED:MFS_UNCHECKED);
	return hMenu;
}

void SNM_ImageWnd::DrawControls(LICE_IBitmap* _bm, const RECT* _r, int* _tooltipHeight)
{
	int x0=_r->left+10, h=35;
	if (_tooltipHeight)
		*_tooltipHeight = h;

	m_img.SetVisible(true);
	if (!m_stretch)
	{
		int x = _r->left + int((_r->right-_r->left)/2 - m_img.GetWidth()/2 + 0.5);
		int y = _r->top + int((_r->bottom-_r->top)/2 - m_img.GetHeight()/2 + 0.5);
		RECT tr = {x, y, x+m_img.GetWidth(), y+m_img.GetHeight()};
		m_img.SetPosition(&tr);
		SNM_AddLogo(_bm, _r, x0, h);
	}
	else
		m_img.SetPosition(_r);
}

bool SNM_ImageWnd::GetToolTipString(int _xpos, int _ypos, char* _bufOut, int _bufOutSz)
{
	if (WDL_VWnd* v = m_parentVwnd.VirtWndFromPoint(_xpos,_ypos,1))
		if (v->GetID()==IMGID && g_SNM_LastImgSlot>=0 && *GetFilename())
			return (_snprintfStrict(_bufOut, _bufOutSz, __LOCALIZE_VERFMT("Image slot %d:\n%s","sws_DLG_162"), g_SNM_LastImgSlot+1, GetFilename()) > 0);
	return false;
}

int ImageInit()
{
	g_pImageWnd = new SNM_ImageWnd();
	if (!g_pImageWnd)
		return 0;

	// load prefs
	g_pImageWnd->SetStretch(!!GetPrivateProfileInt("ImageView", "Stretch", 0, g_SNM_IniFn.Get()));

	return 1;
}

void ImageExit()
{
	// save prefs
	if (g_pImageWnd)
		WritePrivateProfileString("ImageView", "Stretch", g_pImageWnd->IsStretched()?"1":"0", g_SNM_IniFn.Get()); 
	DELETE_NULL(g_pImageWnd);
}

void OpenImageWnd(COMMAND_T* _ct) {
	if (g_pImageWnd) {
		g_pImageWnd->Show(true, true);
		g_pImageWnd->RequestRedraw();
	}
}

bool OpenImageWnd(const char* _fn)
{
	bool ok = false;
	if (g_pImageWnd)
	{
		ok = true;
		WDL_FastString prevFn(g_pImageWnd->GetFilename());
		g_pImageWnd->SetImage(_fn && *_fn ? _fn : NULL); // NULL clears the current image
		g_pImageWnd->Show(!strcmp(prevFn.Get(), _fn) /* i.e toggle */, true);
		g_pImageWnd->RequestRedraw();
	}
	return ok;
}

void ClearImageWnd(COMMAND_T*) {
	if (g_pImageWnd) {
		g_pImageWnd->SetImage(NULL);
		g_pImageWnd->RequestRedraw();
	}
}

bool IsImageWndDisplayed(COMMAND_T*){
	return (g_pImageWnd && g_pImageWnd->IsValidWindow());
}

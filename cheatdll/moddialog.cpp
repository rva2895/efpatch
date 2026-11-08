#include "stdafx.h"
#include "moddialog.h"
#include <CommCtrl.h>
#include <fstream>
#include <set>

HWND hModList = NULL;

HWND CreateListViewInDialog(HWND hwndParent, int ctrlID)
{
    // 2. Get the static control placeholder
    HWND hStatic = GetDlgItem(hwndParent, ctrlID);
    if (!hStatic) return NULL;

    // 3. Get the static control's position and size in the PARENT's client coordinates
    RECT rcStatic;
    GetWindowRect(hStatic, &rcStatic);

    // Convert screen coordinates to parent dialog's client coordinates
    POINT ptTopLeft = { rcStatic.left, rcStatic.top };
    POINT ptBottomRight = { rcStatic.right, rcStatic.bottom };

    ScreenToClient(hwndParent, &ptTopLeft);
    ScreenToClient(hwndParent, &ptBottomRight);

    int x = ptTopLeft.x;
    int y = ptTopLeft.y;
    int w = ptBottomRight.x - ptTopLeft.x;
    int h = ptBottomRight.y - ptTopLeft.y;

    // 4. Create the ListView as a child of the dialog
    HINSTANCE hInst = GetModuleHandle(NULL);
    HWND hListView = CreateWindowEx(
        WS_EX_CLIENTEDGE, // Optional: adds a border
        WC_LISTVIEW,
        L"",
        WS_VISIBLE | WS_CHILD | WS_TABSTOP | LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SINGLESEL,
        x, y, w, h,
        hwndParent,
        (HMENU)(INT_PTR)ctrlID,
        hInst,
        NULL);

    if (!hListView) return NULL;

    // 5. Enable extended features
    ListView_SetExtendedListViewStyle(hListView, LVS_EX_CHECKBOXES | LVS_EX_FULLROWSELECT);

    // 6. Destroy the placeholder static control
    DestroyWindow(hStatic);

    return hListView;
}

// ---------------------------------------------------------------------------
// Mod list population
// ---------------------------------------------------------------------------

static std::wstring ModVersionPath(const std::wstring& modName)
{
    return L"Mods\\" + modName + L"\\version.ini";
}

static std::wstring ModDescriptionPath(const std::wstring& modName)
{
    return L"Mods\\" + modName + L"\\readme.txt";
}

static void __cdecl ModDirCallback(const wchar_t* filename, void* param)
{
    std::vector<std::wstring>* scanResult = (std::vector<std::wstring>*)param;

    if (wcscmp(filename, L".") == 0 || wcscmp(filename, L"..") == 0)
        return;

    // Valid mod: "Mods\<name>\version.ini" exists
    if (file_exists(ModVersionPath(filename).c_str()))
        scanResult->push_back(filename);
}

std::vector<std::wstring> EnumerateValidMods()
{
    std::vector<std::wstring> scanResult;

    findfirst_callback(L"Mods\\*", ModDirCallback, &scanResult);
    std::sort(scanResult.begin(), scanResult.end());
    return scanResult;
}

// Reads a whole file into a std::string. Returns empty on failure.
static std::string ReadFileBytes(const std::wstring& path)
{
    std::ifstream file(path.c_str(), std::ios::binary);
    if (!file)
        return std::string();
    return std::string((std::istreambuf_iterator<char>(file)),
        std::istreambuf_iterator<char>());
}

// Trims leading/trailing whitespace (including CR/LF) from a string.
static std::wstring TrimW(const std::wstring& s)
{
    size_t a = s.find_first_not_of(L" \t\r\n");
    if (a == std::wstring::npos)
        return std::wstring();
    size_t b = s.find_last_not_of(L" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Strip an optional UTF-8 BOM from the raw bytes.
static std::string StripUtf8Bom(const std::string& s)
{
    if (s.size() >= 3 &&
        (unsigned char)s[0] == 0xEF &&
        (unsigned char)s[1] == 0xBB &&
        (unsigned char)s[2] == 0xBF)
        return s.substr(3);
    return s;
}

static std::wstring ReadFileTrimmed(const std::wstring& modName, std::wstring(*ModFilePath)(const std::wstring& modName))
{
    std::string bytes = StripUtf8Bom(ReadFileBytes(ModFilePath(modName)));
    if (bytes.empty())
        return std::wstring();
    return TrimW(UTF8ToWide(bytes));
}

// version.ini contains just the version string (e.g. "1.0").
static std::wstring ReadModVersion(const std::wstring& modName)
{
    return ReadFileTrimmed(modName, ModVersionPath);
}

// Description comes from readme.txt (whole file, trimmed).
static std::wstring ReadModDescription(const std::wstring& modName)
{
    return ReadFileTrimmed(modName, ModDescriptionPath);
}

// Builds the info text: "<description>\r\nVersion: <version>".
// Each line is omitted if its source is missing/empty.
static std::wstring BuildModInfoText(const std::wstring& modName)
{
    std::wstring desc = ReadModDescription(modName);
    std::wstring ver = ReadModVersion(modName);

    std::wstring info;
    if (!desc.empty())
        info = desc;
    if (!ver.empty())
    {
        if (!info.empty())
            info += L"\r\n\r\n";
        info += L"Version: ";
        info += ver;
    }
    return info;
}

// Updates IDC_STATIC_MOD_INFO based on the current selection.
// Uses the first selected item; clears the control when nothing is selected.
static void UpdateModInfo(HWND hWndDlg, HWND hList)
{
    HWND hInfo = GetDlgItem(hWndDlg, IDC_STATIC_MOD_INFO);
    if (!hInfo)
        return;

    int sel = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
    if (sel < 0)
    {
        SetWindowTextW(hInfo, L"");
        return;
    }

    wchar_t name[512] = { 0 };
    ListView_GetItemText(hList, sel, 0, name, _countof(name));
    if (name[0] == L'\0')
    {
        SetWindowTextW(hInfo, L"");
        return;
    }

    std::wstring info = BuildModInfoText(name);
    SetWindowTextW(hInfo, info.c_str());
}

// Enables/disables the Move Up / Move Down buttons based on current selection.
// Up is enabled only when an item is selected and isn't first.
// Down is enabled only when an item is selected and isn't last.
static void UpdateMoveButtons(HWND hWndDlg, HWND hList)
{
    HWND hUp = GetDlgItem(hWndDlg, IDC_BUTTON_MOVE_UP);
    HWND hDown = GetDlgItem(hWndDlg, IDC_BUTTON_MOVE_DOWN);
    if (!hUp || !hDown)
        return;

    int sel = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
    int count = ListView_GetItemCount(hList);

    EnableWindow(hUp, sel > 0);
    EnableWindow(hDown, sel >= 0 && sel < count - 1);
}

static void InitModListColumns(HWND hList)
{
    LVCOLUMNW col = { 0 };
    col.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
    col.iSubItem = 0;
    col.cx = 410;
    col.pszText = const_cast<LPWSTR>(L"Mod Name");
    ListView_InsertColumn(hList, 0, &col);

    // Size the column to fill the ListView client width (no horizontal scroll).
    RECT rcClient;
    GetClientRect(hList, &rcClient);
    int width = rcClient.right - rcClient.left;
    // Subtract the vertical scrollbar width if present.
    if (GetWindowLongW(hList, GWL_STYLE) & WS_VSCROLL)
        width -= GetSystemMetrics(SM_CXVSCROLL);
    if (width < 0)
        width = 0;
    ListView_SetColumnWidth(hList, 0, width);
}

static void PopulateModList(HWND hList)
{
    ListView_DeleteAllItems(hList);

    std::vector<std::wstring> allMods = EnumerateValidMods();
    std::set<std::wstring> validSet(allMods.begin(), allMods.end());
    std::set<std::wstring> checkedSet(cd.mods.begin(), cd.mods.end());
    std::set<std::wstring> placed;

    // Order: mods listed in cd.mods come first (in cd.mods order),
    // then any remaining valid mods in alphabetical order.
    // Duplicates and stale entries in cd.mods are ignored.
    std::vector<std::wstring> orderedMods;
    orderedMods.reserve(allMods.size());
    for (const std::wstring& name : cd.mods)
    {
        if (validSet.count(name) && placed.insert(name).second)
            orderedMods.push_back(name);
    }
    for (const std::wstring& name : allMods)
    {
        if (placed.insert(name).second)
            orderedMods.push_back(name);
    }

    int i = 0;
    for (const std::wstring& name : orderedMods)
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.pszText = const_cast<LPWSTR>(name.c_str());
        int idx = ListView_InsertItem(hList, &item);
        if (idx < 0) { ++i; continue; }

        ListView_SetCheckState(hList, idx, checkedSet.count(name) ? TRUE : FALSE);
        ++i;
    }
}

// Moves the currently selected item by `delta` positions (-1 = up, +1 = down),
// preserving check states and re-selecting the moved item.
static void MoveSelectedItem(HWND hList, int delta)
{
    if (!hList)
        return;

    int sel = ListView_GetNextItem(hList, -1, LVNI_SELECTED);
    if (sel < 0)
        return;

    int count = ListView_GetItemCount(hList);
    int target = sel + delta;
    if (target < 0 || target >= count)
        return;

    // Snapshot all item names and check states.
    struct ItemState { std::wstring name; bool checked; };
    std::vector<ItemState> items;
    items.reserve(count);
    for (int i = 0; i < count; ++i)
    {
        wchar_t buf[512] = { 0 };
        ListView_GetItemText(hList, i, 0, buf, _countof(buf));
        ItemState s;
        s.name = buf;
        s.checked = ListView_GetCheckState(hList, i) != FALSE;
        items.push_back(std::move(s));
    }

    std::swap(items[sel], items[target]);

    // Rebuild the list in the new order.
    ListView_DeleteAllItems(hList);
    for (int i = 0; i < (int)items.size(); ++i)
    {
        LVITEMW item = { 0 };
        item.mask = LVIF_TEXT;
        item.iItem = i;
        item.pszText = const_cast<LPWSTR>(items[i].name.c_str());
        int idx = ListView_InsertItem(hList, &item);
        if (idx < 0)
            continue;
        ListView_SetCheckState(hList, idx, items[i].checked ? TRUE : FALSE);
    }

    // Restore selection/focus on the moved item and scroll it into view.
    ListView_SetItemState(hList, target,
        LVIS_SELECTED | LVIS_FOCUSED,
        LVIS_SELECTED | LVIS_FOCUSED);
    ListView_EnsureVisible(hList, target, FALSE);
}

static void SaveCheckedMods(HWND hList)
{
    cd.mods.clear();

    int count = ListView_GetItemCount(hList);
    for (int i = 0; i < count; ++i)
    {
        if (!ListView_GetCheckState(hList, i))
            continue;

        wchar_t buf[512] = { 0 };
        ListView_GetItemText(hList, i, 0, buf, _countof(buf));
        if (buf[0] != L'\0')
            cd.mods.push_back(buf);
    }
}

// ---------------------------------------------------------------------------
// Dialog
// ---------------------------------------------------------------------------

BOOL CALLBACK ModDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_CLOSE:
        EndDialog(hWndDlg, 0);
        break;

    case WM_INITDIALOG:
        hModList = CreateListViewInDialog(hWndDlg, IDC_STATIC_MODS_LIST);
        if (hModList)
        {
            InitModListColumns(hModList);
            PopulateModList(hModList);
            UpdateMoveButtons(hWndDlg, hModList);
        }
        SetDlgItemTextW(hWndDlg, IDC_STATIC_MOD_INFO, L"");
        break;

    case WM_NOTIFY:
    {
        LPNMHDR hdr = (LPNMHDR)lParam;
        if (hModList && hdr->hwndFrom == hModList)
        {
            if (hdr->code == LVN_ITEMCHANGED)
            {
                LPNMLISTVIEW nmlv = (LPNMLISTVIEW)lParam;
                // Only react to selection state changes (not check/uncheck).
                if ((nmlv->uChanged & LVIF_STATE) &&
                    ((nmlv->uOldState & LVIS_SELECTED) != (nmlv->uNewState & LVIS_SELECTED)))
                {
                    UpdateModInfo(hWndDlg, hModList);
                    UpdateMoveButtons(hWndDlg, hModList);
                }
            }
        }
    }
    break;

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            switch (LOWORD(wParam))
            {
            case IDOK:
                if (hModList)
                    SaveCheckedMods(hModList);

                PostMessage(GetParent(hWndDlg), WM_APP_UPDATE_SETTINGS, 0, 0);
                EndDialog(hWndDlg, 0);
                break;

            case IDCANCEL:
                EndDialog(hWndDlg, 0);
                break;

            case IDC_BUTTON_MOVE_UP:
                MoveSelectedItem(hModList, -1);
                break;

            case IDC_BUTTON_MOVE_DOWN:
                MoveSelectedItem(hModList, +1);
                break;

            default:
                break;
            }
        }
        break;

    default:
        return FALSE;
    }
    return TRUE;
}

void __stdcall launchModDialog(HWND hWnd)
{
    DialogBox(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDD_DIALOG_MODS), hWnd, ModDlgProc);
    hModList = NULL;
}

#include "stdafx.h"
#include "palette.h"
#include "slp.h"
#include "drsfile.h"
#include "resource.h"
#include "commctrl.h"
#include <process.h>

__declspec(naked) void onSelectionBox() //00473C90
{
    __asm
    {
        mov     eax, [esp + 14h]
        cmp     al, 243
        jnz     _box_end
        mov     al, 68
_box_end:
        mov     [esp + 14h], eax
        mov     eax, 00473C95h
        push    ebx
        mov     ebx, [esp + 10h]
        jmp     eax
    }
}

const uint8_t colors_to_replace[] =
{
    1, 131,
    2, 132,
    3, 133,
    4, 134,
    5, 134,
    6, 135,
    7, 135,
    10, 0,
    12, 192,
    30, 64,
    48, 0,
    49, 48,
    50, 49,
    51, 50,
    52, 51,
    53, 52,
    54, 53,
    55, 54,
    128, 1,
    129, 2,
    130, 3,
    131, 4,
    132, 5,
    133, 6,
    134, 7,
    135, 255,
    180, 179,
    206, 0,
    207, 209,
    212, 176,
    235, 52,
    237, 55,
    243, 68,
    253, 215,
    254, 217
};

//0x1308
//int complete_slp_counter = 0;
HWND hWndPaletteDlg;
std::vector<std::wstring>* slp_parallel;
int nProc;
HANDLE* slp_optimize_event;

bool slp_interfac(int id)
{
    if ((id < 50028) || (id > 53291))
        return true;
    if ((id > 53240) && (id < 53271))
        return true;
    switch (id)
    {
    case 50127:        //scen bkg
    case 50150:        //tooltip bkg
    case 50212:
    case 50221:
    case 50222:
    case 50223:
    case 50224:
    case 50225:
    case 50226:
    case 50339:
    case 50340:
    case 50341:
    case 50342:
    case 50343:
    case 50403:
    case 50604:
    case 50606:
    case 50689:
    case 50690:
    case 50691:
    case 50692:
    case 50693:
    case 50694:
    case 50695:
    case 50705:
    case 50706:
    case 50707:
    case 50708:
    case 50709:
    case 50710:
    case 50712:        //NEW
    case 50714:        //???
    case 50715:        //???
    case 50719:        //NEW
    //case 50713:        //
    //case 50716:        //
    //case 50725:    //
    //case 50726:    //
    //case 50727:    //
    //case 50728:    //

    case 50721:
    case 50731:        //obj panel item icons
    case 50732:        //sp icons
    case 50734:
    case 50735:
    case 50736:
    case 50737:
    case 50738:
    case 50739:
    case 50745:        //NEW
    case 50746:        //VERY NEW
    case 50749:        //NEW
    case 50751:
    case 50754:
    case 50760:        //NEW
    case 50764:
    case 50788:
    case 50789:
    case 50790:
    case 50791:
    case 50792:        //VERY NEW
    case 51000:        //cursor
    case 51001:
    case 51002:
    case 51003:
    case 51101:
    case 51102:
    case 51103:
    case 51104:
    case 51105:
    case 51106:
    case 51107:
    case 51108:
    case 51121:
    case 51122:
    case 51123:
    case 51124:
    case 51125:
    case 51126:
    case 51127:
    case 51128:
    case 51141:
    case 51142:
    case 51143:
    case 51144:
    case 51145:
    case 51146:
    case 51147:
    case 51148:
    case 53002:        //NEW
    case 53003:        //icon outline?
    case 53004:        //VERY NEW
    case 53008:        //rec interface
    case 53009:        //VERY NEW, maybe wrong?
    case 53011:        //mp stat text indicators
    case 53201:
    case 53203:
    case 53206:        //tech tree?
    case 53208:
    case 53211:
        return true;
    default:
        return false;
    }
}

unsigned int __stdcall slp_optimize_thread(void* p)
{
    for (int i = (int)p; i < slp_parallel->size(); i += nProc)
    {
        int id;
        swscanf_s((*slp_parallel)[i].c_str(), L"%d", &id);
        if (slp_interfac(id))
        {
            SLP slp;
            FILE* g = _wfopen((*slp_parallel)[i].c_str(), L"rb");
            fseek(g, 0, SEEK_END);
            int size = ftell(g);
            fseek(g, 0, SEEK_SET);
            void* data = malloc(size);
            fread(data, size, 1, g);
            fclose(g);
            slp.load((uint8_t*)data);
            free(data);
            slp.color_replace(colors_to_replace, sizeof(colors_to_replace) / sizeof(colors_to_replace[0]) / 2);
            PostMessage(hWndPaletteDlg, WM_APP_STATUS_SLP, (WPARAM)id, NULL);
            data = slp.write(&size, (id < 15000) || (id > 16000));
            g = _wfopen((*slp_parallel)[i].c_str(), L"wb");
            fwrite(data, size, 1, g);
            fclose(g);
            free(data);
        }
    }
    SetEvent(slp_optimize_event[(int)p]);
    return 0;
}

unsigned int getAffinityCount()
{
    DWORD dwProcessAffinity, dwSystemAffinity;
    GetProcessAffinityMask(GetCurrentProcess(), &dwProcessAffinity, &dwSystemAffinity);
    unsigned int v = dwProcessAffinity;
    v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
    unsigned int c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
    return c;
}

void patch_drs_palette(const wchar_t* filename, const wchar_t* main_dir)
{
    log("Loading %s...", filename);
    wchar_t err[0x100];
    DRS* drs;
    drs = new DRS();
    SetCurrentDirectory(main_dir);
    SetCurrentDirectory(L"data\\");
    if (!drs->loadDRS(filename))
    {
        _snwprintf(err, _countof(err), L"Cannot load %s.\nCheck installation integrity.", filename);
        log(WideToUTF8_c_str(err));
        MessageBox(NULL, err, L"Error", MB_ICONERROR);
        exit(0);
    }
    SetCurrentDirectory(_wgetenv(L"temp"));
    CreateDirectory(filename, NULL);
    SetCurrentDirectory(filename);
    std::wstring* wparam_filename = new std::wstring(filename);
    PostMessage(hWndPaletteDlg, WM_APP_STATUS_DRS, (WPARAM)wparam_filename, 0);
    log("Extracting files...");
    if (drs->extractFiles() == 0)
    {
        _snwprintf(err, _countof(err), L"Cannot create temporary file.\nMake sure there is at least 1 GB free in TEMP folder");
        log(WideToUTF8_c_str(err));
        MessageBox(NULL, err, L"Error", MB_ICONERROR);
        exit(0);
    }
    delete drs;

    log("Scanning folder...");

    slp_parallel = new std::vector<std::wstring>;

    struct palette_slp_callback_param
    {
        int nSLPInFolder;
        std::vector<std::wstring>* slp_parallel;
    } param;
    param.nSLPInFolder = 0;
    param.slp_parallel = slp_parallel;

    auto palette_slp_callback = [](const wchar_t* filename, void* param)
        {
            palette_slp_callback_param* p = (palette_slp_callback_param*)param;
            const wchar_t* s = filename;
            if (s[0] == L'"')
                s++;
            int id;
            swscanf_s(s, L"%d", &id);
            if (id != 50230)
            {
                p->slp_parallel->emplace_back(filename);
                p->nSLPInFolder++;
            }
        };

    findfirst_callback(L"*.slp", palette_slp_callback, &param);

    log("Found %d SLPs, starting %d SLP optimize threads...", param.nSLPInFolder, nProc);
    
    for (int i = 0; i < nProc; i++)
    {
        slp_optimize_event[i] = CreateEvent(NULL, TRUE, FALSE, NULL);
        unsigned int slp_tid;
        HANDLE hThread = (HANDLE)_beginthreadex(NULL, 0, slp_optimize_thread, (void*)i, 0, &slp_tid);
        if (hThread)
            CloseHandle(hThread);
    }
    WaitForMultipleObjects(nProc, slp_optimize_event, TRUE, INFINITE);
    for (int i = 0; i < nProc; i++)
        CloseHandle(slp_optimize_event[i]);
    ///
    delete slp_parallel;

    drs = new DRS();
    wchar_t newfilename[MAX_PATH];
    wcscpy(newfilename, filename);
//#ifndef VOOBLY_EF
    newfilename[wcslen(newfilename) - 4] = 0;
    wcscat(newfilename, L"_p1.drs");
//#endif
    SetCurrentDirectory(main_dir);
    SetCurrentDirectory(DATA_FOLDER_PREFIX_FROM_ROOT);
    drs->setFileName(newfilename);

    log("SLP optimize threads terminated, creating %s...", newfilename);
    //
    std::wstring* wparam_newfilename = new std::wstring(newfilename);
    PostMessage(hWndPaletteDlg, WM_APP_STATUS_DRS, (WPARAM)wparam_newfilename, 1);
    //
    SetCurrentDirectory(_wgetenv(L"temp"));
    SetCurrentDirectory(filename);
    //log("DRS header set up, adding files...");

    struct palette_drs_callback_param
    {
        std::vector<std::wstring> files;
        int nDrsFiles;
        DRS* drs;
    } param_drs;
    param_drs.nDrsFiles = 0;
    param_drs.drs = drs;

    auto palette_drs_callback = [](const wchar_t* filename, void* param)
        {
            palette_drs_callback_param* p = (palette_drs_callback_param*)param;
            unsigned long table;
            const wchar_t* s = filename + wcslen(filename) - 3;
            if (filename[wcslen(filename) - 1] == L'"')
                s--;
            if (!wcsncmp(s, L"slp", 3))
                table = 0x736C7020;
            else if (!wcsncmp(s, L"bin", 3))
                table = 0x62696E61;
            else if (!wcsncmp(s, L"wav", 3))
                table = 0x77617620;
            else
                table = 0;
            if (table != 0)
            {
                p->files.emplace_back(filename);
                s = filename;
                if (s[0] == '"')
                    s++;
                int id;
                swscanf_s(s, L"%d", &id);
                FILE* g = _wfopen(filename, L"rb");
                fseek(g, 0, SEEK_END);
                int size = ftell(g);
                fseek(g, 0, SEEK_SET);
                void* data = malloc(size);
                fread(data, size, 1, g);
                fclose(g);
                p->drs->addFile(data, size, id, table);
                free(data);
                p->nDrsFiles++;
            }
        };

    findfirst_callback(L"*.*", palette_drs_callback, &param_drs);

    for (auto i = param_drs.files.begin(); i != param_drs.files.end(); ++i)
        DeleteFile((*i).c_str());
    SetCurrentDirectory(_wgetenv(L"temp"));
    RemoveDirectory(filename);
    SetCurrentDirectory(main_dir);
    SetCurrentDirectory(DATA_FOLDER_PREFIX_FROM_ROOT);
    log("Added %d files to DRS, writing...", param_drs.nDrsFiles);
    if (!drs->writeDRS())
    {
        _snwprintf(err, _countof(err), L"Cannot create DRS file.\nMake sure there is at least 1 GB free in Game folder, and you have write permissions");
        log(WideToUTF8_c_str(err));
        MessageBox(NULL, err, L"Error", MB_ICONERROR);
        exit(0);
    }
    delete drs;
    log("%s patched", WideToUTF8_c_str(filename));
}

unsigned int __stdcall patch_palette(void*)
{
    nProc = getAffinityCount();
    slp_optimize_event = (HANDLE*)malloc(nProc * sizeof(HANDLE));
    wchar_t dir[MAX_PATH];
    GetCurrentDirectory(_countof(dir), dir);
    patch_drs_palette(L"interfac.drs", dir);
    patch_drs_palette(L"interfac_x1.drs", dir);
    patch_drs_palette(L"graphics.drs", dir);
    patch_drs_palette(L"graphics_x1.drs", dir);
    patch_drs_palette(L"terrain.drs", dir);
    patch_drs_palette(L"terrain_x1.drs", dir);
    SetCurrentDirectory(dir);
    PostMessage(hWndPaletteDlg, WM_CLOSE, NULL, NULL);
    free(slp_optimize_event);
    return 0;
}

int slp_counter;

BOOL CALLBACK PaletteDlgProc(HWND hWndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    wchar_t s[0x100];
    wchar_t* st;
    unsigned int palette_tid;
    HANDLE hThread;
    switch (message)
    {
    case WM_CLOSE:
        EndDialog(hWndDlg, 0);
        return TRUE;
    case WM_INITDIALOG:
        slp_counter = 0;
        hWndPaletteDlg = hWndDlg;
        SetDlgItemText(hWndDlg, IDC_STATIC_PALETTE_CURRENT, L"Ready");
        SendMessage(GetDlgItem(hWndDlg, IDC_PROGRESS_PALETTE), PBM_SETRANGE, 0, MAKELPARAM(0, 0x1280));
        hThread = (HANDLE)_beginthreadex(NULL, 0, patch_palette, NULL, 0, &palette_tid);
        if (hThread)
            CloseHandle(hThread);
        return TRUE;
    case WM_APP_STATUS_DRS: //status update DRS
        switch (lParam)
        {
        case 0:
            st = L"Extracting";
            break;
        case 1:
            st = L"Writing";
            break;
        default:
            st = L"";
            break;
        }
        _snwprintf(s, _countof(s), L"%s %s...", st, ((std::wstring*)wParam)->c_str());
        delete (std::wstring*)wParam;
        SetDlgItemText(hWndDlg, IDC_STATIC_PALETTE_CURRENT, s);
        return TRUE;
    case WM_APP_STATUS_SLP: //status update SLP
        _snwprintf(s, _countof(s), L"Processing %d.slp...", (int)wParam);
        SetDlgItemText(hWndDlg, IDC_STATIC_PALETTE_CURRENT, s);
        SendMessage(GetDlgItem(hWndDlg, IDC_PROGRESS_PALETTE), PBM_SETPOS, slp_counter++, 0);
        return TRUE;
    default:
        break;
    }
    return FALSE;
}

const char creating_new_p1_drs[] = ", creating new p1 DRS files";

void installPalette()
{
    FILE* f = _wfopen(DATA_FOLDER_PREFIX_FROM_ROOT L"palette", L"rb");
    char ver = 0;
    if (f)
    {
        fread(&ver, sizeof(char), 1, f);
        fclose(f);
        if (ver == '5')
        {
            log("Palette already created");
            return;
        }
        else
            log("Wrong palette version%s", creating_new_p1_drs);
    }
    else
        log("Palette not found%s", creating_new_p1_drs);
    DialogBox(GetModuleHandle(DLL_NAME), MAKEINTRESOURCE(IDD_DIALOG_PALETTE), NULL, PaletteDlgProc);
    f = _wfopen(DATA_FOLDER_PREFIX_FROM_ROOT L"palette", L"wb");
    ver = '5';
    fwrite(&ver, sizeof(char), 1, f);
    fclose(f);
}

__declspec(naked) void tech_tree_bkg_fix() //00462680
{
    __asm
    {
        mov     eax, 0FFh
        mov     [esi + 1D88h], eax
        mov     eax, 00462686h
        jmp     eax
    }
}

#pragma optimize( "s", on )
void setPaletteHooks()
{
    setHook((void*)0x00473C90, onSelectionBox);
    writeByte(0x004F3833, 48); //new black? don't know
    writeByte(0x004F38EF, 48);
    writeByte(0x004F39B4, 48);
    writeByte(0x004F3A72, 48);
    writeByte(0x004F3B34, 48);
    writeByte(0x005D9E42, 68); //new yellow 68
    writeByte(0x005DD2DD, 68);
    writeByte(0x005DD9B3, 68);
    writeByte(0x004F95FF, 68);
    writeByte(0x004F96AB, 6);
    writeByte(0x004F9770, 68);
    writeByte(0x004F97A0, 6);
    writeByte(0x004F9B5F, 68);
    writeByte(0x004F9BFA, 68);
    //0054CF0B ??
    //005F8B2C minimap?
    writeByte(0x0069B555, 68); //more here !!
    writeByte(0x0069B551, 6);

    //text color
    writeDword(0x004F9FE6, 68); 
    writeDword(0x004FB58C, 68);
    writeDword(0x004FB619, 68);
    writeDword(0x004FB6E0, 68);
    writeDword(0x004FBBA4, 68);
    writeDword(0x004FBC70, 68);
    writeDword(0x004FBCCD, 68);
    writeDword(0x004FBD2A, 68);
    writeDword(0x004FBD92, 68);
    writeDword(0x004FBE71, 68);
    writeDword(0x004FBF4E, 68);
    writeDword(0x004FBFAB, 68);
    writeDword(0x004FC008, 68);
    writeDword(0x004FC090, 68);
    writeDword(0x004FC5B8, 68);
    writeDword(0x004FC910, 68);
    writeDword(0x005013AF, 68);
    writeDword(0x005016C1, 68);
    writeDword(0x005017A1, 68);
    writeDword(0x00502641, 68);

    //tech tree lines
    writeDword(0x00462844, 7);
    //tech tree bkg
    setHook((void*)0x00462680, tech_tree_bkg_fix);

    //rec interface lines
    writeDword(0x004F3833, 0);
    writeDword(0x004F3838, 1);
    writeDword(0x004F383D, 1);
    writeDword(0x004F3842, 4);
    writeDword(0x004F3847, 4);
    writeDword(0x004F384C, 6);
    writeByte(0x004F3851, 133);

    writeDword(0x004F38EF, 0);
    writeDword(0x004F38F4, 1);
    writeDword(0x004F38F9, 1);
    writeDword(0x004F3904, 4);
    writeDword(0x004F3909, 4);
    writeDword(0x004F390E, 6);
    writeByte(0x004F3913, 133);

    writeDword(0x004F39B4, 0);
    writeDword(0x004F39B9, 1);
    writeDword(0x004F39BE, 1);
    writeDword(0x004F39C3, 4);
    writeDword(0x004F39C8, 4);
    writeDword(0x004F39CD, 6);
    writeByte(0x004F39D2, 133);

    writeDword(0x004F3A72, 0);
    writeDword(0x004F3A77, 1);
    writeDword(0x004F3A7C, 1);
    writeDword(0x004F3A81, 4);
    writeDword(0x004F3A86, 4);
    writeDword(0x004F3A8B, 6);
    writeByte(0x004F3A90, 133);

    writeDword(0x004F3B34, 0);
    writeDword(0x004F3B39, 1);
    writeDword(0x004F3B3E, 1);
    writeDword(0x004F3B43, 4);
    writeDword(0x004F3B48, 4);
    writeDword(0x004F3B4D, 6);
    writeByte(0x004F3B52, 133);

    //mouse cursor
    writeByte(0x0049F08F, 252);
    writeByte(0x0049F824, 252);
    writeByte(0x0049FE02, 252);
    writeByte(0x0049F0C0, 252);
}
#pragma optimize( "", on )

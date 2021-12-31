#include "stdafx.h"
#include "legacypatch.h"
#include "zlib.h"

#ifdef TARGET_VOOBLY
const char legacy_patch_error1[] = "Error decompressing legacy patch data.";
const char legacy_patch_error2[] = "Error writing legacy patch data.";

uint8_t legacy_patch_data_compressed[] =
{
    0xED, 0x57, 0x6F, 0x4C, 0x1B, 0x65, 0x18, 0x7F, 0x8B, 0xB7, 0xAD, 0xD1, 0xC2, 0xCE, 0xD9, 0x66,
    0x97, 0xA8, 0xF1, 0xA6, 0x30, 0x56, 0x10, 0xA4, 0xF3, 0x2F, 0xD2, 0x2D, 0xD0, 0x63, 0x2C, 0x26,
    0x1B, 0x34, 0xB0, 0xA1, 0x2E, 0x86, 0xE3, 0x68, 0x6F, 0xDC, 0xD5, 0x72, 0x47, 0xCE, 0x83, 0x4C,
    0x21, 0x71, 0x5A, 0x59, 0x0E, 0x2E, 0x2A, 0x59, 0xA2, 0x19, 0x9B, 0x59, 0x20, 0xE5, 0x83, 0x1F,
    0xF6, 0x61, 0xC6, 0x2F, 0x18, 0x4D, 0x2C, 0x2D, 0x1A, 0x4C, 0x4C, 0x64, 0xF1, 0x4B, 0xE7, 0x62,
    0xE2, 0x12, 0x3E, 0x5C, 0xC5, 0x0F, 0x73, 0x59, 0xC8, 0xA6, 0xC6, 0xF3, 0x79, 0xAF, 0xED, 0xFA,
    0x07, 0x04, 0x3E, 0xB8, 0x4F, 0xF6, 0x97, 0xFC, 0xDE, 0xDE, 0xF3, 0xBC, 0xCF, 0xF3, 0x7B, 0x9F,
    0xF7, 0x47, 0x68, 0x73, 0xDD, 0xE3, 0x8B, 0x9E, 0xD5, 0x49, 0xDD, 0x85, 0xC8, 0x11, 0xE4, 0x8F,
    0xFC, 0x61, 0x53, 0xB7, 0x19, 0x6E, 0x84, 0xD0, 0x2B, 0x9E, 0x55, 0xDD, 0xE5, 0x80, 0x5C, 0x63,
    0x99, 0x5A, 0x11, 0x89, 0xDB, 0xDB, 0x9A, 0x5C, 0x24, 0x44, 0xA3, 0x37, 0x7A, 0x12, 0xBA, 0x8F,
    0xE8, 0x49, 0x4C, 0x66, 0x21, 0x24, 0xE9, 0x11, 0x24, 0xFC, 0x02, 0x8B, 0xD1, 0x47, 0x1A, 0x66,
    0xD4, 0x0E, 0x55, 0x73, 0x4E, 0x58, 0xA2, 0xB8, 0x7B, 0x0E, 0x37, 0x45, 0x09, 0x58, 0x66, 0xF0,
    0x12, 0x89, 0xD9, 0xFD, 0x11, 0x83, 0xF0, 0x47, 0x62, 0x84, 0x95, 0x14, 0x96, 0xA0, 0x6F, 0x06,
    0xB7, 0xF8, 0x8D, 0xB3, 0xD5, 0x86, 0x19, 0xB9, 0x5D, 0x36, 0xE4, 0x0C, 0xD9, 0x42, 0x65, 0x99,
    0xDC, 0xF7, 0x07, 0x20, 0xB7, 0xF0, 0xC8, 0x4A, 0x3C, 0x13, 0x9B, 0x15, 0x86, 0xA9, 0xBB, 0x2C,
    0xA5, 0x5F, 0x89, 0x71, 0x6B, 0x22, 0x4F, 0xAC, 0x39, 0x8A, 0x67, 0x87, 0x32, 0x6B, 0xA8, 0xAA,
    0x60, 0x5D, 0x55, 0x10, 0x2E, 0xA0, 0xF4, 0x21, 0x0B, 0xAD, 0x9C, 0xCA, 0xBD, 0x16, 0xE2, 0x83,
    0x62, 0x9D, 0x20, 0x87, 0xE5, 0x7A, 0xF5, 0x94, 0x8A, 0x0A, 0x60, 0xED, 0xF7, 0x2B, 0xF2, 0x90,
    0x14, 0xAC, 0x53, 0xE5, 0x3A, 0x4E, 0x54, 0xD2, 0x35, 0x96, 0xC8, 0x3A, 0xE8, 0x5E, 0xEB, 0xD7,
    0x9D, 0xBF, 0x4D, 0xF3, 0xAE, 0x5F, 0x91, 0x38, 0xB1, 0x9E, 0x65, 0x7F, 0x65, 0x2D, 0xEB, 0x9C,
    0x41, 0x14, 0x54, 0xDD, 0xC6, 0x8D, 0x0D, 0xA0, 0x77, 0x62, 0xC6, 0x01, 0xF1, 0xC9, 0x46, 0x72,
    0xE7, 0x7B, 0x51, 0x08, 0xC1, 0x99, 0xA6, 0x6D, 0x24, 0x64, 0x46, 0x7F, 0x4B, 0xB9, 0x66, 0xBF,
    0x35, 0x27, 0x8B, 0x91, 0xF6, 0x1B, 0xED, 0x01, 0xBF, 0x6F, 0x56, 0x60, 0xBF, 0x29, 0xEC, 0x37,
    0x85, 0xFD, 0xC6, 0x4F, 0xB8, 0x35, 0x4A, 0x50, 0xD8, 0x6F, 0x7C, 0x4C, 0x8C, 0xF0, 0x5B, 0x91,
    0x40, 0xED, 0xC1, 0x46, 0x53, 0xD8, 0x44, 0xF7, 0x5E, 0x6C, 0xB4, 0x2D, 0x6B, 0xB4, 0x95, 0x0B,
    0x78, 0xB1, 0xD1, 0xD4, 0xCA, 0x77, 0x99, 0xF8, 0xE3, 0x72, 0xCB, 0x68, 0x78, 0x06, 0x97, 0xA9,
    0x8C, 0xCB, 0x58, 0x70, 0x81, 0x4A, 0xB0, 0x73, 0x36, 0x18, 0xB4, 0x27, 0x4E, 0x58, 0x7E, 0x1C,
    0x79, 0x8A, 0x45, 0x22, 0xF0, 0x60, 0x03, 0x8B, 0x6C, 0x1E, 0x16, 0x5D, 0x00, 0xFE, 0x00, 0x44,
    0xFB, 0x59, 0xB4, 0x1F, 0x38, 0x05, 0xFC, 0x13, 0x48, 0x3E, 0xCD, 0xA2, 0x77, 0x9E, 0x63, 0xD1,
    0x8F, 0xC0, 0xEB, 0x40, 0xD7, 0xF3, 0x2C, 0x7A, 0x09, 0x38, 0x0F, 0xFC, 0xE8, 0x05, 0x16, 0xED,
    0x6D, 0x64, 0x51, 0x3F, 0xF0, 0x73, 0xE0, 0x75, 0xA0, 0xEF, 0x45, 0x16, 0x9D, 0x03, 0x1E, 0x69,
    0x62, 0xD1, 0x4D, 0xE0, 0x69, 0x2F, 0x8B, 0x6E, 0x01, 0xC3, 0x07, 0x58, 0x34, 0x00, 0x3D, 0x32,
    0xEC, 0xBD, 0x0B, 0xE7, 0x7E, 0x01, 0xBC, 0x01, 0xDC, 0x0D, 0xE7, 0xBF, 0x55, 0xCB, 0xA2, 0x13,
    0xC0, 0x10, 0x10, 0x35, 0x8C, 0xA0, 0x69, 0x20, 0xF2, 0x8C, 0xA0, 0x5E, 0xE0, 0xBD, 0x82, 0xFE,
    0x61, 0x65, 0x05, 0x42, 0xA1, 0x6A, 0x61, 0xBA, 0x12, 0xFE, 0x24, 0xED, 0xEC, 0x15, 0x33, 0x93,
    0xD9, 0x27, 0xD0, 0x55, 0x90, 0xF1, 0xE6, 0x32, 0x6E, 0xA1, 0x19, 0x67, 0x6A, 0x72, 0x99, 0x1A,
    0xA1, 0x17, 0x67, 0x28, 0xC8, 0xAC, 0x3C, 0x6E, 0x65, 0x6A, 0xD3, 0x5D, 0xDB, 0x73, 0x35, 0x4F,
    0xA6, 0xBB, 0x6E, 0xF5, 0x64, 0x33, 0xA9, 0xF1, 0xA9, 0xA4, 0x79, 0xF7, 0x7C, 0x81, 0x86, 0x73,
    0x4D, 0x57, 0xB2, 0x85, 0x47, 0x63, 0x31, 0x75, 0xB7, 0x70, 0x1A, 0x42, 0xBF, 0xE9, 0xEA, 0x4D,
    0xC7, 0x3B, 0xCC, 0xA5, 0x54, 0xCA, 0x67, 0x98, 0x21, 0x52, 0x18, 0x84, 0x0D, 0xA1, 0x01, 0x96,
    0x10, 0x94, 0xCF, 0x32, 0xBC, 0xF5, 0x79, 0xCA, 0xC7, 0x6F, 0x72, 0x3F, 0xFE, 0xE4, 0x20, 0xA7,
    0x06, 0x84, 0xFA, 0x60, 0x38, 0x6C, 0xC5, 0x0C, 0x27, 0x49, 0xB2, 0x4A, 0x87, 0x65, 0x2E, 0x48,
    0xE7, 0xED, 0xD5, 0xD3, 0x8C, 0xC0, 0x07, 0x5E, 0xA7, 0x45, 0xE9, 0x0D, 0x95, 0x0B, 0x87, 0x39,
    0x55, 0x94, 0x25, 0x08, 0x54, 0xBE, 0x5F, 0x11, 0xD5, 0x37, 0xEB, 0x73, 0x7A, 0x87, 0x14, 0x45,
    0x56, 0xF2, 0xF4, 0x5F, 0x16, 0xA5, 0xA3, 0x9C, 0x28, 0xB1, 0x59, 0xFD, 0x62, 0x30, 0x02, 0x27,
    0xF5, 0xF3, 0x74, 0x47, 0x5F, 0x88, 0x0F, 0xA8, 0x74, 0xD7, 0x20, 0xCF, 0x17, 0xFE, 0x63, 0x7A,
    0x5B, 0x3B, 0xDA, 0xAB, 0x8F, 0xD1, 0xC7, 0xBB, 0x0E, 0x1D, 0x44, 0x2A, 0xDD, 0x89, 0x8B, 0x37,
    0xD8, 0x6F, 0x97, 0x95, 0x01, 0x2E, 0x4C, 0xB7, 0x28, 0x03, 0xD9, 0x21, 0x0A, 0xF7, 0xFD, 0x22,
    0xAF, 0x04, 0xF8, 0xFC, 0xFD, 0xC3, 0xE2, 0x30, 0xC4, 0x7D, 0x62, 0x18, 0xAE, 0xB1, 0xDE, 0x7C,
    0x9D, 0xFC, 0x80, 0xBC, 0x51, 0x45, 0x66, 0xFE, 0xE3, 0x92, 0xA8, 0xD2, 0xF8, 0xBB, 0x86, 0xDE,
    0xD7, 0x12, 0x1C, 0xE6, 0xA4, 0x00, 0x1F, 0x74, 0x6F, 0xE0, 0x7A, 0x09, 0x25, 0x94, 0x50, 0xC2,
    0xBD, 0xC7, 0xE5, 0x32, 0xBD, 0xDB, 0xA1, 0x9F, 0xFF, 0x04, 0x1E, 0x35, 0xC6, 0xA6, 0x31, 0x84,
    0xC6, 0x6C, 0xD7, 0x98, 0x1D, 0x1A, 0x43, 0x6A, 0xCC, 0x83, 0x1A, 0xB3, 0x4B, 0x63, 0x1E, 0xD2,
    0x18, 0xA7, 0xC6, 0xB8, 0xF4, 0xF3, 0xD3, 0x5B, 0xA8, 0x99, 0xDD, 0x42, 0xCD, 0xA7, 0x56, 0xCD,
    0xFD, 0x1B, 0xD6, 0x5C, 0xDA, 0x82, 0xCE, 0xE5, 0xCD, 0x6B, 0xDA, 0xEC, 0x9F, 0xD9, 0x52, 0x53,
    0x35, 0xCB, 0xE6, 0xBF, 0x1B, 0x50, 0x42, 0x09, 0x25, 0x94, 0xF0, 0x3F, 0x85, 0xDE, 0x5A, 0x49,
    0x7A, 0xAE, 0x8D, 0xC5, 0x76, 0x7E, 0x10, 0x2D, 0x5F, 0x36, 0xDF, 0xBF, 0x54, 0xF9, 0x0C, 0xBC,
    0xDD, 0xE9, 0x43, 0x68, 0x6C, 0x55, 0xF5, 0xE8, 0x6D, 0xCE, 0x89, 0x66, 0xC2, 0xDB, 0xA6, 0x12,
    0x5E, 0xFF, 0xD0, 0x13, 0xE3, 0xBF, 0x1B, 0x8F, 0x25, 0x7E, 0x32, 0xF5, 0x61, 0xE7, 0xB5, 0xC3,
    0x64, 0xF2, 0xD9, 0x5E, 0x78, 0xB9, 0xBA, 0x7A, 0x66, 0x09, 0x24, 0x92, 0xF3, 0x57, 0x2F, 0xE2,
    0xCF, 0x9F, 0x93, 0x58, 0x8C, 0x89, 0x7C, 0x43, 0x34, 0x26, 0x46, 0xBF, 0x4A, 0x75, 0x95, 0xA7,
    0xBF, 0x76, 0x17, 0x7D, 0xFF, 0xCD, 0x7C, 0xF7, 0x15, 0xCF, 0xE7, 0x2E, 0x98, 0xEF, 0x61, 0x98,
    0xEF, 0xED, 0xB8, 0x35, 0xDF, 0xC4, 0x19, 0xFC, 0xF3, 0x31, 0xD1, 0x41, 0xDA, 0x17, 0x35, 0xEB,
    0x31, 0x6F, 0xAE, 0x85, 0xD4, 0x97, 0x8E, 0xFC, 0x9F, 0x83, 0xBC, 0xFB, 0x3B, 0x8A, 0xF4, 0x6B,
    0x0B, 0xF4, 0x1F, 0xC5, 0xF7, 0x2F, 0xD2, 0x5F, 0xBD, 0x42, 0xAF, 0x3D, 0x21, 0x9E, 0x7A, 0x35,
    0xEF, 0x84, 0x9C, 0x3E, 0xB3, 0xB9, 0xFE, 0xD7, 0xF3, 0x96, 0xBE, 0xB1, 0xE6, 0xAD, 0x7F, 0xB2,
    0x40, 0xFF, 0xCE, 0x03, 0xCB, 0xE6, 0x3F
};

const char jedi_holo_txt_file[] = DATA_FOLDER_PREFIX_FROM_ROOT"jedi-holo.txt";
const char ground_to_air_txt_file[] = DATA_FOLDER_PREFIX_FROM_ROOT"ground-to-air.txt";

void install_legacy_patch()
{
    log("Attempting to write legacy patch data");
    z_stream_s st;
    memset(&st, 0, sizeof(st));

    void* dst = malloc(0x1400);
    memset(dst, 0, 0x1400);

    int r = inflateInit2(&st, -MAX_WBITS);

    st.avail_in = sizeof(legacy_patch_data_compressed);
    st.next_in = (Bytef*)legacy_patch_data_compressed;
    st.avail_out = 0x1400;
    st.next_out = (Bytef*)dst;

    r = inflate(&st, Z_NO_FLUSH);
    inflateEnd(&st);

    if (r == Z_STREAM_END)
    {
        MEMORY_BASIC_INFORMATION mi;
        SIZE_T size = 0;
        do
        {
            VirtualQueryEx(GetCurrentProcess(), (BYTE*)0x007B1000 + size, &mi, sizeof(mi));
            if (mi.AllocationBase == (void*)0x00400000)
                size += mi.RegionSize;
        } while (mi.AllocationBase == (void*)0x00400000 && size < 0x3000);
        if (size >= 0x3000)
        {
            void* zero_mem = malloc(0x1000);
            memset(zero_mem, 0, 0x1000);
            writeData(0x007B1000, zero_mem, 0x1000);
            writeData(0x007B2000, dst, st.total_out);
            free(zero_mem);

            //writeDword(0x005F2B56, 0x007B2240);
            //setHook((void*)0x0053BD3A, (void*)0x007B2340);
            //setHook((void*)0x005F557B, (void*)0x007B2A00);

#ifdef VOOBLY_EF
            setHook((void*)0x005E3EE0, (void*)0x007B2000);
            setHook((void*)0x005E3F10, (void*)0x007B2130);
            setHook((void*)0x0041C593, (void*)0x007B2170);

            writeDword(0x007B2046, (DWORD)jedi_holo_txt_file);
            writeDword(0x007B21B6, (DWORD)ground_to_air_txt_file);

            writeByte(0x006910F2, 0x32);
            writeByte(0x006910FA, 0x32);
            writeByte(0x00691101, 0x32);
            writeByte(0x00691109, 0x32);
            writeByte(0x00691113, 0x32);
            writeByte(0x0069111F, 0x32);
#endif

            log("Legacy patch data successfully installed");
            free(dst);
        }
        else
        {
            log(legacy_patch_error2);
            free(dst);
            MessageBox(NULL, legacy_patch_error2, "Error", MB_ICONERROR);
            exit(0);
        }
    }
    else
    {
        log(legacy_patch_error1);
        free(dst);
        MessageBox(NULL, legacy_patch_error1, "Error", MB_ICONERROR);
        exit(0);
    }
}
#endif

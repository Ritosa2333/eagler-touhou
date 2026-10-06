// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x446560; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct WIN32_FIND_DATAA {
    unsigned int dwFileAttributes;
    FILETIME ftCreationTime;
    FILETIME ftLastAccessTime;
    FILETIME ftLastWriteTime;
    unsigned int nFileSizeHigh;
    unsigned int nFileSizeLow;
    unsigned int dwReserved0;
    unsigned int dwReserved1;
    CHAR cFileName[260];
    CHAR cAlternateFileName[14];
} WIN32_FIND_DATAA;

typedef struct FILETIME {
    unsigned int dwLowDateTime;
    unsigned int dwHighDateTime;
} FILETIME;

extern unsigned int g_4ad138;
extern void* g_4b4530;

void sub_446560(void)
{
    unsigned int v10;  // ebx
    unsigned int v11;  // esi
    unsigned int v12;  // edi
    int v13;  // esi
    unsigned int iter;  // edi
    HANDLE v15;  // eax
    int v16;  // edi
    unsigned int node;  // esi
    HANDLE v0;  // [bp-0x1a0]
    WIN32_FIND_DATAA v1;  // [bp-0x19c]
    unsigned int v2;  // [bp-0x19c]
    HANDLE v3;  // [bp-0x198], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x194]
    WIN32_FIND_DATAA v5;  // [bp-0x194]
    char v6;  // [bp-0x190]
    char v7;  // [bp-0x50]
    unsigned int v8;  // [bp-0x8]

    v8 = g_4ad138 ^ &v6;
    v4 = v10;
    v3 = v11;
    v2 = v12;
    v13 = 1;
    iter = g_4b4530 + 23168;
    do
    {
        sub_46cbbb(&v7, "th12_%.2d.rpy", v13);
        *((unsigned int *)iter) = sub_43b6f0(&v7);
    } while (!((char)g_4b4530[23576] & 4) && (v13 = (int)(v13 + 1), iter += 4, v13 <= 25));
    sub_46d585("replay");
    sub_46ddb7("replay");
    v15 = FindFirstFileA("th12_ud????.rpy", &v5);
    v3 = v15;
    if (v15 != 0xffffffff)
    {
        v16 = 25;
        node = g_4b4530 + 23268;
        do
        {
            sub_46ddb7("../");
            *((unsigned int *)node) = sub_43b6f0(&v5.dwReserved1);
            sub_46ddb7("replay");
        } while (!((char)g_4b4530[23576] & 4) && FindNextFileA(v0, &v1) && (v16 = (int)(v16 + 1), node += 4, v16 < 75));
        v15 = v0;
    }
    FindClose(v15);
    sub_46ddb7("../");
    *((unsigned int *)&g_4b4530[23576]) = (int)g_4b4530[23576] & 0xfffffffb | 8;
    _security_check_cookie();
    return;
}

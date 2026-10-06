// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4647f0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct OSVERSIONINFOA {
    unsigned int dwOSVersionInfoSize;
    unsigned int dwMajorVersion;
    unsigned int dwMinorVersion;
    unsigned int dwBuildNumber;
    unsigned int dwPlatformId;
    CHAR szCSDVersion[128];
} OSVERSIONINFOA;

int sub_4647f0(void)
{
    unsigned int v3;  // eax
    unsigned int v4;  // eax
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    unsigned int v7;  // eax
    unsigned int v8;  // eax
    OSVERSIONINFOA v0;  // [bp-0x98]
    OSVERSIONINFOA v1;  // [bp-0x98]

    sub_477420(&v0, 0, 148);
    v1 = (OSVERSIONINFOA)_INSERT(CONCAT(v0, 0), 0, 148);
    GetVersionExA(&v1);
    if (!v1.dwPlatformId)
    {
        return v8;
    }
    else if (v1.dwPlatformId != 1)
    {
        if (v1.dwPlatformId != 2)
        {
            return v3;
        }
        else if (v1.dwMajorVersion == 5)
        {
            return v4;
        }
        else
        {
            return v5;
        }
    }
    else
    {
        if (v1.dwMajorVersion == 4)
            return v6;
        return v7;
    }
}

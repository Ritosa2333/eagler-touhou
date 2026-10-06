// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x482e18; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b437c;

unsigned int * sub_482e18(unsigned int *a0)
{
    int v4;  // edi
    int v5;  // esi
    int v6;  // ecx
    unsigned int *v7;  // edi
    unsigned int v8;  // ebx
    unsigned int *v9;  // eax
    unsigned int **v10;  // eax
    unsigned int v11;  // eax
    int v12;  // eax
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    HMODULE v2;  // [bp-0x8]
    char v3;  // [bp+0x0]

    v7 = sub_4751de(g_4b437c, v4, v5, v6, &v3);
    if (!a0)
    {
        v1 = 22;
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return 0x16;
    }
    v1 = v8;
    *(a0) = 0;
    if (!v7)
    {
        v2 = LoadLibraryA("ADVAPI32.DLL");
        if (!LoadLibraryA("ADVAPI32.DLL"))
        {
            v0 = 22;
            *((unsigned int *)sub_46e96f()) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
            v9 = 0x16;
        }
        else
        {
            v7 = GetProcAddress(LoadLibraryA("ADVAPI32.DLL"), "SystemFunction036");
            if (!v7)
            {
                v10 = sub_46e96f();
                *(v10) = sub_46e92d(GetLastError());
                sub_470f2d(0, 0, 0, 0, 0);
                v9 = sub_46e92d(GetLastError());
            }
            else
            {
                v11 = sub_475163(v7);
                v12 = sub_4751d5();
                if (InterlockedExchange(&g_4b437c, v11) != v12)
                    FreeLibrary(LoadLibraryA("ADVAPI32.DLL"));
                goto LABEL_482efa;
            }
        }
    }
    else
    {
LABEL_482efa:
        if (!v7(a0, 4))
        {
            *((unsigned int *)sub_46e96f()) = 12;
            v9 = *((int *)sub_46e96f());
        }
        else
        {
            v9 = NULL;
        }
    }
    return v9;
}

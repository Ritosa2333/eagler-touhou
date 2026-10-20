// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x482f20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern int g_4b4380;
extern unsigned int g_4b4384;
extern unsigned int g_4b4388;
extern unsigned int g_4b438c;
extern unsigned int g_4b4390;

unsigned int sub_482f20(unsigned int a0, unsigned int a1, unsigned int a2)
{
    unsigned int v7;  // ebx
    unsigned int v8;  // esi
    unsigned int *v17;  // eax
    unsigned int *v18;  // eax
    unsigned int v9;  // edi
    int v10;  // ebx
    HMODULE v11;  // edi
    unsigned int v12;  // eax
    unsigned int *v13;  // esi
    unsigned int *v14;  // edi
    unsigned int v15;  // eax
    unsigned int *v16;  // eax
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    unsigned int v2;  // [bp-0x1c]
    char v3;  // [bp-0x18]
    char v4;  // [bp-0x10]
    char v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]

    v2 = v7;
    v1 = v8;
    v0 = v9;
    v6 = 0;
    v10 = sub_4751d5();
    if (!g_4b4380)
    {
        v11 = LoadLibraryA("USER32.DLL");
        if (!v11)
            return 0;
        v12 = GetProcAddress(v11, "MessageBoxA");
        if (!v12)
            return 0;
        g_4b4380 = sub_475163(v12);
        g_4b4384 = sub_475163(GetProcAddress(v11, "GetActiveWindow"));
        g_4b4388 = sub_475163(GetProcAddress(v11, "GetLastActivePopup"));
        g_4b4390 = sub_475163(GetProcAddress(v11, "GetUserObjectInformationA"));
        if (g_4b4390)
            g_4b438c = sub_475163(GetProcAddress(v11, "GetProcessWindowStation"));
    }
    if (g_4b438c != v10 && g_4b4390 != v10)
    {
        v13 = sub_4751de(g_4b438c);
        v14 = sub_4751de(g_4b4390);
        if (!v13 || !v14 || (v15 = v13(), v15 && v14(v15, 1, &v3, 12, &v5) && v4 & 1))
            goto LABEL_483029;
        a2 |= 0x200000;
    }
    else
    {
LABEL_483029:
        if (g_4b4384 != v10)
        {
            v16 = sub_4751de(g_4b4384);
            if (v16)
            {
                v6 = v16();
                if (v6 && g_4b4388 != v10)
                {
                    v17 = sub_4751de(g_4b4388);
                    if (v17)
                        v6 = v17(v6);
                }
            }
        }
    }
    v18 = sub_4751de(g_4b4380);
    if (!v18)
        return 0;
    return v18(v6, a0, a1, a2);
}

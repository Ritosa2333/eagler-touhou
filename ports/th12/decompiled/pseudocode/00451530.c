// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x451530; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct STARTUPINFOA {
    unsigned int cb;
    PSTR lpReserved;
    PSTR lpDesktop;
    PSTR lpTitle;
    unsigned int dwX;
    unsigned int dwY;
    unsigned int dwXSize;
    unsigned int dwYSize;
    unsigned int dwXCountChars;
    unsigned int dwYCountChars;
    unsigned int dwFillAttribute;
    enum STARTUPINFOW_FLAGS dwFlags;
    unsigned short wShowWindow;
    unsigned short cbReserved2;
    Byte *lpReserved2;
    HANDLE hStdInput;
    HANDLE hStdOutput;
    HANDLE hStdError;
} STARTUPINFOA;

extern unsigned int g_4ad138;
extern unsigned int g_4cee78;
extern char g_4cf418;

int sub_451530(void)
{
    int v10;  // edi
    int v11;  // esi
    unsigned int v20;  // cc_dep2
    char v21;  // 4103
    unsigned int v22;  // eax
    unsigned int v23;  // 4111
    unsigned int v24;  // 4120
    unsigned int v25;  // eax
    char *v12;  // eax
    int v13;  // esi
    char *node;  // eax
    char i;  // 4102
    char *v16;  // edx
    char *iter;  // ecx
    char v18;  // 4101
    unsigned int v19;  // cc_dep1
    char v0;  // [bp-0x264]
    STARTUPINFOA v1;  // [bp-0x260]
    STARTUPINFOA v2;  // [bp-0x260]
    char v3;  // [bp-0x25c]
    char v4;  // [bp-0x21c]
    Byte v5[252];  // [bp-0x218]
    char v6;  // [bp-0x11c]
    Byte v7[268];  // [bp-0x110]
    unsigned int v8;  // [bp-0x4]

    v8 = g_4ad138 ^ &v0;
    v1 = (STARTUPINFOA)0x44;
    sub_477420(&v3, 0, 64, v10, v11);
    GetModuleFileNameA(NULL, v7, 261);
    GetConsoleTitleA(v5, 261);
    GetStartupInfoA(&v2);
    v12 = v2.lpTitle;
    if (v12)
    {
        v13 = sub_46d260(v12, 46);
        if (sub_463df0(v2.lpTitle) && v13)
        {
            if (!sub_46e3f8(v13, ".lnk"))
            {
                do
                {
                    sub_4517a0(v2.lpReserved);
                } while (!sub_46e3f8(sub_46d260(&v2.hStdError, 46), ".lnk"));
            }
            else
            {
                node = v2.lpDesktop;
                do
                {
                    i = *(node);
                    *(&v4 - v2.lpDesktop + node) = *(node);
                    node += 1;
                } while (i);
            }
            v16 = &v2.hStdOutput;
            iter = &v6;
            while (1)
            {
                v18 = *(iter);
                v19 = v18;
                v20 = *(v16);
                if (v18 != *(v16))
                    break;
                if (v18)
                {
                    v21 = iter[1];
                    v19 = v21;
                    v20 = v16[1];
                    if (v21 != v16[1])
                        break;
                    iter += 2;
                    v16 += 2;
                    if (!v21)
                        goto LABEL_451641;
                }
                else
                {
LABEL_451641:
                    v22 = 0;
                    goto LABEL_45164a;
                }
            }
            v23 = _ccall(4, v19, v20, 0);
            v24 = _ccall(12, 0, v23 & 1, v23 & 1);
            v22 = -(v23 & 1) + 1 - (v24 & 1);
LABEL_45164a:
            if (v22)
                g_4cf418 = 1;
        }
        g_4cee78 = g_4cee78 & 0xffffffbf;
    }
    else
    {
        g_4cee78 = g_4cee78 | 64;
    }
    _security_check_cookie();
    return v25;
}

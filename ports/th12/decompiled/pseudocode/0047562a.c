// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47562a; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47562a_0 {
    unsigned int field_0;
    unsigned int field_4;
} st_47562a_0;

extern char g_49d51c;
extern unsigned int g_4adac8;
extern unsigned int g_4adacc;
extern void g_4b4100;
extern void g_4b4104;
extern void g_4b4108;
extern int g_4b410c;

unsigned int sub_47562a(void)
{
    unsigned int v2;  // edi
    HMODULE v3;  // eax
    HMODULE v4;  // edi
    void* v5;  // eax
    unsigned int v6;  // 4098
    st_47562a_0 *v7;  // esi
    unsigned int *v8;  // eax
    unsigned int ch;  // eax
    unsigned int v0;  // [bp-0x8]

    v0 = v2;
    v3 = GetModuleHandleW(&g_49d51c);
    if (!v3)
        v3 = sub_472bdd(&g_49d51c);
    v4 = v3;
    if (v4)
    {
        *((IntPtr (**)(void))&g_4b4100) = GetProcAddress(v4, "FlsAlloc");
        *((IntPtr (**)(void))&g_4b4104) = GetProcAddress(v4, "FlsGetValue");
        *((IntPtr (**)(void))&g_4b4108) = GetProcAddress(v4, "FlsSetValue");
        v5 = GetProcAddress(v4, "FlsFree");
        v6 = *((int *)&g_4b4100);
        g_4b410c = v5;
        if (!v6 || !*((int *)&g_4b4104) || !*((int *)&g_4b4108) || !g_4b410c)
        {
            *((void* *)&g_4b4104) = TlsGetValue;
            *((void* *)&g_4b4100) = sub_475250;
            *((void* *)&g_4b4108) = TlsSetValue;
            g_4b410c = TlsFree;
        }
        g_4adacc = TlsAlloc();
        if (g_4adacc == 0xffffffff)
        {
            return 0;
        }
        else if (TlsSetValue(g_4adacc, *((int *)&g_4b4104)))
        {
            sub_472f3f();
            *((unsigned int *)&g_4b4100) = sub_475163(*((int *)&g_4b4100));
            *((unsigned int *)&g_4b4104) = sub_475163(*((int *)&g_4b4104));
            *((unsigned int *)&g_4b4108) = sub_475163(*((int *)&g_4b4108));
            g_4b410c = sub_475163(g_4b410c);
            if (sub_46eb06())
            {
                g_4adac8 = sub_4751de(*((int *)&g_4b4100))(sub_475481);
                if (g_4adac8 != 0xffffffff)
                {
                    v7 = sub_477813(1, 532);
                    if (v7)
                    {
                        v8 = sub_4751de(*((int *)&g_4b4108));
                        if (v8(g_4adac8, v7))
                        {
                            sub_475307(v7, 0);
                            ch = GetCurrentThreadId();
                            v7->field_4 = 0xffffffff;
                            v7->field_0 = ch;
                            return 1;
                        }
                    }
                }
            }
        }
        else
        {
            return 0;
        }
    }
    sub_4752ca();
    return 0;
}

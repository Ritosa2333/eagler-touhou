// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x475163; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_475163_1 {
    unsigned int field_0;
} st_475163_1;

typedef struct st_475163_0 {
    char padding_0[504];
    struct st_475163_1 *field_1f8;
} st_475163_0;

extern WCHAR g_49d51c;
extern unsigned int g_4adac8;
extern unsigned int g_4adacc;

unsigned int sub_475163(unsigned int a0)
{
    unsigned int *v1;  // eax
    st_475163_0 *v2;  // eax
    unsigned int *v3;  // eax
    HMODULE v4;  // eax
    unsigned int v0;  // [bp-0x8]

    if (TlsGetValue(g_4adacc) && g_4adac8 != 0xffffffff)
    {
        v1 = TlsGetValue(g_4adacc);
        v2 = v1(g_4adac8, v0);
        if (!v2)
            goto LABEL_47519c;
        v3 = &v2->field_1f8->field_0;
    }
    else
    {
LABEL_47519c:
        v4 = GetModuleHandleW(&g_49d51c);
        if (!v4)
        {
            v4 = sub_472bdd(&g_49d51c);
            if (!v4)
                return a0;
        }
        v3 = GetProcAddress(v4, "EncodePointer");
    }
    if (!v3)
        return a0;
    a0 = v3(a0);
    return a0;
}

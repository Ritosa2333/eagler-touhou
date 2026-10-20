// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4751de; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4751de_1 {
    unsigned int field_0;
} st_4751de_1;

typedef struct st_4751de_0 {
    char padding_0[508];
    struct st_4751de_1 *field_1fc;
} st_4751de_0;

extern char g_49d51c;
extern unsigned int g_4adac8;
extern unsigned int g_4adacc;

unsigned int sub_4751de(unsigned int a0)
{
    unsigned int *v1;  // eax
    st_4751de_0 *v2;  // eax
    unsigned int *v3;  // eax
    HMODULE v4;  // eax
    unsigned int v0;  // [bp-0x8]

    if (TlsGetValue(g_4adacc) && g_4adac8 != 0xffffffff)
    {
        v1 = TlsGetValue(g_4adacc);
        v2 = v1(g_4adac8, v0);
        if (!v2)
            goto LABEL_475217;
        v3 = &v2->field_1fc->field_0;
    }
    else
    {
LABEL_475217:
        v4 = GetModuleHandleW(&g_49d51c);
        if (!v4)
        {
            v4 = sub_472bdd(&g_49d51c);
            if (!v4)
                return a0;
        }
        v3 = GetProcAddress(v4, "DecodePointer");
    }
    if (!v3)
        return a0;
    a0 = v3(a0);
    return a0;
}

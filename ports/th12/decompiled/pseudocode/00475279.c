// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x475279; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4adacc;
extern int g_4b4104;

void* sub_475279(void)
{
    void* v1;  // esi
    void* v2;  // esi

    v1 = TlsGetValue(g_4adacc);
    if (v1)
        return v1;
    v2 = sub_4751de(g_4b4104);
    TlsSetValue(g_4adacc, v2);
    return v2;
}

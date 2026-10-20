// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x472bdd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

HMODULE sub_472bdd(PWSTR a0)
{
    unsigned int v1;  // edi
    unsigned int v2;  // edi
    HMODULE v3;  // eax
    unsigned int v0;  // [bp-0x8]

    v0 = v1;
    v2 = 1000;
    while (1)
    {
        Sleep(v2);
        v3 = GetModuleHandleW(a0);
        v2 += 1000;
        if (v2 > 60000)
        {
            return v3;
        }
        else if (v3)
        {
            return v3;
        }
    }
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49132c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_49132c(unsigned int a0)
{
    unsigned int v2;  // esi
    unsigned int v3;  // edi
    unsigned int v5;  // esi
    unsigned int v6;  // edi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]

    if (!a0)
        return 0;
    v1 = v2;
    v0 = v3;
    v5 = sub_475860(a0) + 1;
    v6 = sub_46d04a(v5);
    if (!v6)
    {
        v6 = 0;
    }
    else if (sub_47a2b9(v6, v5, a0))
    {
        sub_470dc6(0, 0, 0, 0, 0);
    }
    return v6;
}

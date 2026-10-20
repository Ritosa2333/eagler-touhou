// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d62f; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_48d62f(unsigned int a0)
{
    unsigned int v2;  // eax
    unsigned int v3;  // eax
    Byte v0[6];  // [bp-0x10]
    char v1;  // [bp-0xa]

    v1 = 0;
    if (GetLocaleInfoA(a0, 4100, v0, 6))
    {
        sub_46d114(v0);
        return v3;
    }
    return v2;
}

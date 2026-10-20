// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485c2b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_485c2b(void)
{
    int *v4;  // ecx
    int v5;  // edi
    unsigned int v6;  // eax
    char v0[120];  // [bp-0x80]
    unsigned int v1;  // [bp+0x4]
    unsigned int v2;  // [bp+0x8]

    if (GetLocaleInfoA(v1 & 0x3ff | 0x400, 1, v0, 120) && (v1 == sub_485b44(v4, v0) || !v2 || !(v6 = sub_485b78(v5), sub_485b78(v5) == sub_475860(*(v4)))))
        return;
    return;
}

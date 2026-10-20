// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x423750; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_423750(void)
{
    int v1;  // edi
    int v2;  // esi
    int v4;  // edi

    v4 = (!sub_46c9ea(424, v1) ? 0 : sub_423380(v2));
    if (!sub_423480())
        return v4;
    if (!v4)
        return 0;
    sub_423580(v4);
    sub_46ca4f(v4);
    return 0;
}

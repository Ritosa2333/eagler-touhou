// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44c110; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_44c110(void)
{
    int v1;  // esi
    int v2;  // eax

    v2 = sub_46d1a0(v1, 47);
    if (!v2)
        return v1;
    return v2 + 1;
}

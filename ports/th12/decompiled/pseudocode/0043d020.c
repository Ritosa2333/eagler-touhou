// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43d020; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

int sub_43d020(void)
{
    int *idx;  // esi
    int v2;  // eax
    int v3;  // eax

    if (*(idx))
    {
        sub_46c941(*(idx));
        *(idx) = 0;
    }
    v2 = idx[1];
    if (!v2)
        return v2;
    v3 = sub_46c941(v2);
    idx[1] = 0;
    return v3;
}

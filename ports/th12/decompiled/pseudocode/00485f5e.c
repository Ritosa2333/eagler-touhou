// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485f5e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

void sub_485f5e(void)
{
    void* idx;  // esi
    void* v3;  // eax
    unsigned int v4;  // 4115
    unsigned int v5;  // eax
    unsigned int v6;  // eax
    unsigned int v0;  // [bp-0x4]

    *((unsigned int *)&idx[16]) = -(NULL < sub_475860(*((int *)idx)) - 3) + 1;
    v3 = sub_475860((int)idx[4]);
    *((unsigned int *)&idx[24]) = 0;
    v4 = (int)idx[16];
    *((unsigned int *)&idx[20]) = -(NULL < v3 - 3) + 1;
    if (v4)
    {
        v0 = 2;
        v5 = 2;
    }
    else
    {
        v5 = sub_485b78(*((int *)idx), *((int *)idx));
    }
    *((unsigned int *)&idx[12]) = v5;
    EnumSystemLocalesA(sub_485c9f, 1);
    v6 = (int)idx[8];
    if (v6 & 0x100 && v6 & 0x200 && (char)v6 & 7)
        return;
    *((unsigned int *)&idx[8]) = 0;
    return;
}

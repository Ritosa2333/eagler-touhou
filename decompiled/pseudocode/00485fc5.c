// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485fc5; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


void sub_485fc5(void)
{
    void* idx;  // esi
    unsigned int v3;  // eax
    unsigned int v4;  // eax
    unsigned int v5;  // 4140
    unsigned int v6;  // 4141
    unsigned int v7;  // eax
    unsigned int v0;  // [bp-0x4]

    v3 = sub_475860(*((int *)idx)) - 3;
    v4 = -(0 < v3);
    v5 = _ccall(12, 0, 0 < v3, 0 < v3);
    *((unsigned int *)&idx[16]) = v4 + 1;
    v6 = _ccall(4, 18, v4 + 1, 0, v5);
    if (!(v6 & 1))
    {
        v0 = 2;
        v7 = 2;
    }
    else
    {
        v7 = sub_485b78();
    }
    *((unsigned int *)&idx[12]) = v7;
    EnumSystemLocalesA(sub_485e71, 1);
    if (!((char)idx[8] & 4))
        *((unsigned int *)&idx[8]) = 0;
    return;
}

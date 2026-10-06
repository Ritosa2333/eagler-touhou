// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x485f32; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_485f32(void)
{
    void* idx;  // esi
    int v2;  // eax

    *((unsigned int *)&idx[20]) = -(0 < sub_475860((int)idx[4]) - 3) + 1;
    v2 = EnumSystemLocalesA(sub_485b93, 1);
    if (!((char)idx[8] & 4))
        *((unsigned int *)&idx[8]) = 0;
    return v2;
}

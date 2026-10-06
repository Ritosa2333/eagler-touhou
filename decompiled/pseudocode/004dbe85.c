// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dbe85; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


unsigned int sub_4dbe85(unsigned int a0, unsigned int a1, unsigned int a2, unsigned int a3, unsigned int a4, unsigned int a5, unsigned int a6, unsigned int a7, unsigned int a8)
{
    void* idx;  // ebp
    unsigned int v2;  // 4106
    unsigned int v3;  // eax
    void* v4;  // edi
    void* v5;  // edi
    void* v6;  // edi
    void* v0;  // [bp+0x0], Other Possible Types: unsigned int

    idx = v0;
    v2 = (int)idx[1160];
    *((void* *)&idx[1160]) = 0xffffffed + idx - 0x6000;
    if (v2)
    {
        v0 = 5345;
        *((unsigned int *)&idx[1038]) = 5345 + (int)idx[1160];
        return a7;
    }
    v3 = (int)idx[4009](idx + 1172);
    *((unsigned int *)&idx[1164]) = v3;
    v4 = idx + 81;
    do
    {
        *((unsigned int *)v4) = (int)idx[4005](v3, v4);
        v5 = v4 + 4;
        do
        {
            v6 = v5;
            v4 = v6 + 1;
            v5 = v4;
        } while (*((char *)v6));
    } while (*((char *)v4));
    goto *((void *)(idx + 122));
}

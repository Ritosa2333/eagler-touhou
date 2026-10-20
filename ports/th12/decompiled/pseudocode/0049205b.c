// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x49205b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_49205b_1 {
    char padding_0[144];
    unsigned int field_90;
} st_49205b_1;

typedef struct st_49205b_0 {
    int field_0;
    unsigned int field_4;
} st_49205b_0;

extern int g_4ab3d0;

void sub_49205b(void)
{
    void* idx;  // ebp
    void* v2;  // edi
    void* idx1;  // ebx
    unsigned int i;  // esi
    st_49205b_1 *index;  // eax
    unsigned int v6;  // eax
    st_49205b_0 *v7;  // ecx

    sub_46fcec(&g_4ab3d0, 16);
    v2 = (int)idx[16];
    idx1 = (int)idx[8];
    *((int *)((char *)idx - 28)) = ((int)v2[4] <= 128 ? (char)idx1[8] : (int)idx1[8]);
    index = sub_475467();
    index->field_90 = index->field_90 + 1;
    for (*((unsigned int *)((char *)idx - 4)) = 0; i != (int)idx[20]; *((unsigned int *)((char *)idx - 28)) = i)
    {
        if (i <= 0xffffffff || i >= (int)v2[4])
            sub_472b94(); /* do not return */
        v6 = i * 8;
        v7 = (int)v2[8] + v6;
        i = v7->field_0;
        *((unsigned int *)((char *)idx - 32)) = i;
        *((unsigned int *)((char *)idx - 4)) = 1;
        if (v7->field_4)
        {
            *((unsigned int *)&idx1[8]) = i;
            sub_493150(*((int *)((int)v2[8] + v6 + 4)), idx1, 259);
        }
        *((unsigned int *)((char *)idx - 4)) = 0;
    }
    *((unsigned int *)((char *)idx - 4)) = 0xfffffffe;
    sub_492121();
    if (i == (int)idx[20])
    {
        *((unsigned int *)&idx1[8]) = i;
        sub_46fd31();
        return;
    }
    sub_472b94(); /* do not return */
}

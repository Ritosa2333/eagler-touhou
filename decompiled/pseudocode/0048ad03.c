// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ad03; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48ad03_0 {
    char padding_0[8];
    unsigned int field_8;
} st_48ad03_0;


void sub_48ad03(unsigned int *idx, st_48ad03_0 *index)
{
    unsigned int v1;  // edi
    unsigned int v2;  // esi
    unsigned int v3;  // ebx
    unsigned int v4;  // edx
    unsigned int v5;  // esi
    unsigned int v6;  // esi
    unsigned int v7;  // ebx
    unsigned int v0;  // [bp-0x10]

    v0 = v1;
    v2 = *(idx) + *((int *)&index->padding_0[0]);
    v3 = 0;
    if (v2 < *(idx) || v2 < *((int *)&index->padding_0[0]))
        v3 = 1;
    *(idx) = v2;
    if (v3)
    {
        v4 = idx[1] + 1;
        v5 = 0;
        if (v4 < idx[1] || v4 < 1)
            v5 = 1;
        idx[1] = v4;
        if (v5)
            idx[2] = idx[2] + 1;
    }
    v6 = idx[1] + *((int *)&index->padding_0[4]);
    v7 = 0;
    if (v6 < idx[1] || v6 < *((int *)&index->padding_0[4]))
        v7 = 1;
    idx[1] = v6;
    if (v7)
        idx[2] = idx[2] + 1;
    idx[2] = idx[2] + index->field_8;
    return;
}

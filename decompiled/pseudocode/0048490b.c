// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48490b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48490b_0 {
    char padding_0[212];
    unsigned int field_d4;
} st_48490b_0;

typedef struct st_48490b_1 {
    char padding_0[112];
    unsigned int field_70;
} st_48490b_1;


unsigned int sub_48490b(int a0)
{
    int v5;  // ebx
    int v6;  // edi
    int v7;  // esi
    int v8;  // ebx
    unsigned int v9;  // edi
    unsigned int v10;  // esi
    unsigned int v11;  // esi
    unsigned int v12;  // esi
    unsigned int v13;  // esi
    unsigned int v14;  // esi
    st_48490b_0 *v0;  // [bp-0x1c]
    st_48490b_1 *idx;  // [bp-0x14]
    char v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int iter;  // [bp-0x8]

    v5 = 0;
    sub_46d3b4(a0, v6, v7, v8);
    v9 = v0->field_d4;
    iter = 0;
    do
    {
        v10 = iter * 4;
        v3 = sub_475860(*((int *)(v10 + v9 + 28)));
        iter += 1;
        v5 = sub_475860(*((int *)(v10 + v9))) + v5 + v3 + 2;
    } while (iter < 7);
    v11 = sub_4777ce(v5 + 1);
    v3 = v11;
    if (v11)
    {
        iter = 0;
        do
        {
            *((char *)v11) = 58;
            v12 = v11 + 1;
            if (sub_47a2b9(v12, v3 - v12 + v5 + 1, *((int *)(v9 + iter * 4))))
                sub_470dc6(0, 0, 0, 0, 0);
            v13 = v12 + sub_475860(v12);
            *((char *)v13) = 58;
            v14 = v13 + 1;
            if (sub_47a2b9(v14, v3 - v14 + v5 + 1, *((int *)(v9 + iter * 4 + 28))))
                sub_470dc6(0, 0, 0, 0, 0);
            v11 = v14 + sub_475860(v14);
            iter += 1;
        } while (iter < 7);
        *((char *)v11) = 0;
    }
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v3;
}

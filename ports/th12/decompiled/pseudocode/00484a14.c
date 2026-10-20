// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x484a14; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_484a14_0 {
    char padding_0[212];
    void* field_d4;
} st_484a14_0;

typedef struct st_484a14_1 {
    char padding_0[112];
    unsigned int field_70;
} st_484a14_1;

unsigned int sub_484a14(int a0)
{
    int v6;  // ebx
    int v7;  // edi
    unsigned int v16;  // esi
    int v8;  // esi
    int v9;  // ebx
    unsigned int v10;  // esi
    unsigned int v11;  // esi
    unsigned int v12;  // esi
    unsigned int v13;  // edi
    unsigned int v14;  // esi
    unsigned int v15;  // esi
    st_484a14_0 *index;  // [bp-0x20]
    st_484a14_1 *idx;  // [bp-0x18]
    char v2;  // [bp-0x14]
    unsigned int v3;  // [bp-0x10]
    unsigned int iter;  // [bp-0xc]
    unsigned int v5;  // [bp-0x8]

    v6 = 0;
    sub_46d3b4(a0, v7, v8, v9);
    v10 = index->field_d4 + 56;
    v5 = v10;
    iter = 12;
    do
    {
        v3 = sub_475860(*((int *)(v10 + 48)));
        v11 = v5 + 4;
        iter -= 1;
        v6 = sub_475860(*((int *)v10)) + v6 + v3 + 2;
        v5 = v11;
        v10 = v11;
    } while (iter != 1);
    v12 = sub_4777ce(v6 + 1);
    v5 = v12;
    if (v12)
    {
        v13 = index->field_d4 + 104;
        iter = 12;
        do
        {
            *((char *)v12) = 58;
            v14 = v12 + 1;
            if (sub_47a2b9(v14, v5 - v14 + v6 + 1, *((int *)(v13 - 48))))
                sub_470dc6(0, 0, 0, 0, 0);
            v15 = v14 + sub_475860(v14);
            *((char *)v15) = 58;
            v16 = v15 + 1;
            if (sub_47a2b9(v16, v5 - v16 + v6 + 1, *((int *)v13)))
                sub_470dc6(0, 0, 0, 0, 0);
            v12 = v16 + sub_475860(v16);
            v13 += 4;
            iter -= 1;
        } while (iter != 1);
        *((char *)v12) = 0;
    }
    if (v2)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return v5;
}

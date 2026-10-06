// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x404620; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_404620_2 {
    char padding_0[3];
    char field_3;
    char padding_4[24];
    char field_1c;
    char padding_1d[1];
    unsigned short field_1e;
    char padding_20[2];
    unsigned short field_22;
} st_404620_2;

typedef struct st_404620_0 {
    char padding_0[16];
    void* field_10;
    unsigned int field_14;
    char padding_18[432];
    unsigned int field_1c8;
} st_404620_0;

typedef struct st_404620_1 {
    char padding_0[1008];
    unsigned int field_3f0;
} st_404620_1;

unsigned int sub_404620(st_404620_0 *index)
{
    int v4;  // ebx
    int idx;  // eax
    unsigned int v6;  // esi
    unsigned int v7;  // edi
    st_404620_2 *idx1;  // ebx
    unsigned int v9;  // edi
    st_404620_1 *v10;  // esi
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    int v2;  // [bp-0xc]
    unsigned int v3;  // [bp-0x4]

    v2 = v4;
    idx = 0;
    v1 = v6;
    v0 = v7;
    v3 = 0;
    if (0 >= *((short *)index->field_10))
        return 0;
    do
    {
        idx1 = *((int *)(index->field_14 + idx * 4));
        if (!(idx1->field_3 & 1))
            continue;
        v9 = &idx1->field_1c;
        v3 = 0;
        if (*((short *)&idx1->field_1c) >= 0)
        {
            do
            {
                v10 = *((short *)(v9 + 6)) * 1204 + index->field_1c8;
                sub_455630(v10);
                if (v10->field_3f0)
                    v3 += 1;
            } while ((v9 += (int)*((short *)(v9 + 2)), *((short *)v9) >= 0));
            if (v3)
                continue;
        }
        idx1->field_3 = idx1->field_3 & 254;
    } while ((idx = (int)(v2 + 1), v2 = idx, idx < (int)*((short *)index->field_10)));
    return 0;
}

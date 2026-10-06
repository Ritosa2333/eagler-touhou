// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4056e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4056e0_0 {
    char padding_0[4];
    unsigned int field_4;
} st_4056e0_0;

typedef struct st_4056e0_3 {
    char padding_0[8];
    struct st_4056e0_0 *field_8;
    struct st_4056e0_0 *field_c;
    void* field_10;
    unsigned int field_14;
    char padding_18[4];
    unsigned int field_1c;
    char padding_20[44];
    unsigned int field_4c;
    char padding_50[372];
    unsigned int field_1c4;
    unsigned int field_1c8;
    char padding_1cc[13364];
    struct st_4056e0_0 *field_3600;
} st_4056e0_3;

extern st_4056e0_3 *g_4b43c0;

unsigned int sub_4056e0(void)
{
    unsigned int idx;  // eax
    unsigned int iter;  // edi
    unsigned int v7;  // ebx
    int index;  // esi
    unsigned int v9;  // edx
    unsigned int v10;  // eax
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]

    g_4b43c0->field_8->field_4 = g_4b43c0->field_8->field_4 | 2;
    g_4b43c0->field_c->field_4 = g_4b43c0->field_c->field_4 | 2;
    g_4b43c0->field_3600->field_4 = g_4b43c0->field_3600->field_4 | 2;
    idx = 0;
    v2 = 0;
    v3 = 0;
    if (0 >= *((short *)g_4b43c0->field_10))
    {
        g_4b43c0->field_4c = g_4b43c0->field_1c;
        return 0;
    }
    do
    {
        *((char *)(*((int *)(g_4b43c0->field_14 + idx * 4)) + 3)) = 1;
        iter = *((int *)(g_4b43c0->field_14 + idx * 4)) + 28;
        if (*((short *)iter) >= 0)
        {
            v7 = v0 * 1204;
            do
            {
                index = g_4b43c0->field_1c8 + v7;
                v2 = *((short *)(iter + 4));
                v3 = g_4b43c0->field_1c4;
                sub_402520();
                *((char *)(index + 1181)) = 16;
                *((char *)(index + 1180)) = 16;
                sub_454d10(index, v2);
                v9 = *((short *)(iter + 2));
                *((unsigned short *)(iter + 6)) = v0;
                iter += v9;
                v7 += 1204;
                v0 += 1;
            } while (*((short *)iter) >= 0);
            idx = v1;
        }
    } while ((idx += 1, v1 = idx, idx < (int)*((short *)g_4b43c0->field_10)));
    v10 = g_4b43c0->field_1c;
    g_4b43c0->field_4c = v10;
    return v10;
}

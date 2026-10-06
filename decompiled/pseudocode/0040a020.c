// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40a020; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40a020_0 {
    char padding_0[92];
    unsigned int field_5c;
    char padding_60[1256];
    unsigned int field_548;
    unsigned int field_54c;
    unsigned int field_550;
    struct st_40a020_1 *field_554;
    char padding_558[62];
    unsigned short field_596;
    char padding_598[4];
    unsigned int field_59c;
    char padding_5a0[16];
    unsigned int field_5b0;
} st_40a020_0;

typedef struct st_40a020_1 {
    unsigned int field_0;
} st_40a020_1;

typedef struct st_40a020_2 {
    char padding_0[96];
    unsigned int field_60;
} st_40a020_2;

extern st_40a020_2 *g_4b44e8;

unsigned int sub_40a020(unsigned int a0)
{
    unsigned int v4;  // ebx
    st_40a020_0 *index;  // esi
    unsigned int v14;  // ftop
    unsigned int v15;  // ftop
    st_40a020_0 *v6;  // ebx
    unsigned int v7;  // edi
    st_40a020_0 *iter;  // edi
    unsigned int v9;  // ftop
    unsigned int v10;  // eax
    unsigned int v11;  // edx
    unsigned int *v12;  // ecx
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]

    v1 = v4;
    v6 = &index->padding_60[4];
    v0 = v7;
    index->field_5c = 0;
    memset(&index->padding_0[20], 0, 48);
    iter = &v6->padding_60[1164];
    v3 = 2000;
    do
    {
        v2 = v1;
        if (!*((short *)&iter->padding_0[70]) || (!g_4b44e8 || !((char)g_4b44e8->field_60 & 2) || !(g_4b44e8->field_60 & 0x400)) && sub_4099e0(v6))
            continue;
        v10 = *((int *)&iter->padding_60[0]);
        if (*((int *)&index->padding_0[20 + 4 * v10]))
            *((st_40a020_0 **)(*((int *)&index->padding_0[44 + 4 * v10]) + 1336)) = v6;
        else
            *((st_40a020_0 **)&index->padding_0[20 + 4 * v10]) = v6;
        *((st_40a020_0 **)&index->padding_0[44 + 4 * *((int *)&iter->padding_60[0])]) = v6;
        *((unsigned int *)&iter->padding_0[76]) = 0;
        index->field_5c = index->field_5c + 1;
        v11 = *((int *)(&iter->padding_0[0] - 4));
        v12 = *((int *)&iter->padding_0[4]);
        *((unsigned int *)(&iter->padding_0[0] - 8)) = v11;
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)((((unsigned short)v9 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3fefae1480000000) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            v14 = v9 - 1;
            if (/* unsupported instruction */)
            {
                v15 = v14 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            else
            {
                v15 = v14 - 1;
                /* unsupported instruction */
                /* unsupported instruction */
            }
            /* unsupported instruction */
            v9 = v15 + 1;
            if (!((char)((((unsigned short)v9 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *(v12)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
            {
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                /* unsupported instruction */
                *((unsigned int *)(&iter->padding_0[0] - 4)) = v11 + 1;
                *((int *)&iter->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
                /* unsupported instruction */
                goto LABEL_40a105;
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v9 -= 1;
        /* unsupported instruction */
        /* unsupported instruction */
        *((int *)&iter->padding_0[0]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)(&iter->padding_0[0] - 4)) = sub_4931e0();
LABEL_40a105:
    } while ((v6 += 2552, iter += 2552, v1 = v2 - 1, v2 != 1));
    return 1;
}

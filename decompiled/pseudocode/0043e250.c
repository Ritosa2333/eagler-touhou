// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43e250; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43e250_1 {
    char padding_0[1228];
    char field_4cc;
    char padding_4cd[11];
    unsigned int field_4d8;
    unsigned int field_4dc;
    unsigned int field_4e0;
    unsigned int field_4e4;
    char padding_4e8[8];
    unsigned int field_4f0;
    char padding_4f4[4];
    unsigned int field_4f8;
    char padding_4fc[8];
    char field_504;
    char field_505;
} st_43e250_1;

typedef struct st_43e250_0 {
    char padding_0[20];
    int field_14;
} st_43e250_0;

extern char g_4b2ed0;
extern st_43e250_0 *g_4b4524;

int sub_43e250(void)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    unsigned int v7;  // edi
    int v8;  // eax
    int i;  // ebx
    st_43e250_1 *idx;  // esi
    int v11;  // ecx
    unsigned int v12;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]
    unsigned int *index;  // [bp+0x4], Other Possible Types: unsigned int
    unsigned int v4;  // [bp+0x8]

    v2 = v5;
    v1 = v6;
    v0 = v7;
    i = v8;
    if (g_4b4524->field_14 >= 10)
        g_4b4524->field_14 = 0;
    idx = 64 * g_4b4524->field_14 + (char *)g_4b4524 + 1228;
    idx->padding_0[56] = 1;
    index = 0;
    if (i >= 0)
    {
        if (i)
        {
            do
            {
                v11 = ((unsigned int)((int)(1717986919 * i >> 0x22)) >> 31) + (int)(1717986919 * i >> 0x22);
                idx->padding_0[index] = (char)i - *(&(char)v11 * 10);
                i = v11;
                index += 1;
            } while (i);
            if (index)
                goto LABEL_43e2ca;
        }
        idx->padding_0[0] = 0;
    }
    else
    {
        idx->padding_0[0] = 10;
    }
    index = 1;
LABEL_43e2ca:
    /* unsupported instruction */
    /* unsupported instruction */
    idx->padding_0[57] = index;
    *((unsigned int *)&idx->padding_0[24]) = v4;
    v12 = *((int *)&idx->padding_0[44]);
    if (!((char)v12 & 1))
    {
        if (/* unsupported instruction */)
            *((int *)&idx->padding_0[36]) = /* unsupported instruction */;
        else
            *((unsigned int *)&idx->padding_0[36]) = nan;
        *((unsigned int *)&idx->padding_0[32]) = 0;
        *((unsigned int *)&idx->padding_0[28]) = 0xfff0bdc1;
        *((char **)&idx->padding_0[40]) = &g_4b2ed0;
        *((unsigned int *)&idx->padding_0[44]) = v12 | 1;
    }
    if (/* unsupported instruction */)
    {
        *((int *)&idx->padding_0[36]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned int *)&idx->padding_0[36]) = nan;
        /* unsupported instruction */
    }
    *((unsigned int *)&idx->padding_0[32]) = 0;
    *((unsigned int *)&idx->padding_0[28]) = 0xffffffff;
    *((unsigned int *)&idx->padding_0[12]) = *(index);
    *((unsigned int *)&idx->padding_0[16]) = index[1];
    *((unsigned int *)&idx->padding_0[20]) = index[2];
    g_4b4524->field_14 = g_4b4524->field_14 + 1;
    return;
}

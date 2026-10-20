// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47eaac; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e26b_0 {
    int field_0;
    char field_4;
} st_47e26b_0;

typedef struct st_47eaac_0 {
    char field_0;
} st_47eaac_0;

extern unsigned int *g_4b430c;
extern st_47eaac_0 *g_4b4318;

int sub_47eaac(void)
{
    unsigned int v10;  // ebx
    unsigned int v11;  // esi
    unsigned int v12;  // edi
    unsigned int v13;  // edx
    unsigned int v14;  // edx
    char *v15;  // edi
    unsigned int v16;  // eax
    unsigned int v0;  // [bp-0x2c]
    unsigned int v1;  // [bp-0x28]
    unsigned int v2;  // [bp-0x24]
    char v3;  // [bp-0x20]
    unsigned int v4[2];  // [bp-0x18]
    unsigned int v5;  // [bp-0x10]
    unsigned int v6;  // [bp-0x10]
    unsigned int v7;  // [bp-0xc]
    unsigned int v8;  // [bp-0x8]
    st_47e26b_0 *idx;  // [bp+0x4]

    v2 = v10;
    v1 = v11;
    idx->field_4 = 0;
    *((unsigned int *)&idx->field_4) = *((int *)&idx->field_4) & 0xffff00ff;
    v8 = 1;
    idx->field_0 = 0;
    if (idx->field_4)
        return;
    v0 = v12;
    do
    {
        if (g_4b4318->field_0 == 64 || g_4b4318->field_0 == 90)
            break;
        if (v8)
            v8 = 0;
        else
            sub_47e9f6(44);
        v15 = &g_4b4318->field_0;
        if (!*(v15))
        {
            sub_47e4af(idx, v14, 1);
            break;
        }
        v16 = *(v15) - 48;
        if (v16 <= 9)
        {
            g_4b4318 = v15 + 1;
            sub_47e7bf(sub_47e378(&v3, v16));
        }
        else
        {
            v7 &= 0xffff0000;
            v6 = 0;
            sub_4826a7(v4, &v6);
            if (g_4b4318 - v15 > 1 && *(g_4b430c) != 9)
                sub_47e32e(g_4b430c, v13, v4);
            sub_47e7bf(v4);
            v5 = v6;
            if (g_4b4318 == v15)
            {
                sub_47e2f7(2);
                v5 = v6;
            }
        }
    } while (!idx->field_4);
    return;
}

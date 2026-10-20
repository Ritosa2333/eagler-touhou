// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477b33; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_477b33_0 {
    unsigned int field_0;
    unsigned int field_4;
    struct st_477b33_1 *field_8;
    char field_c;
} st_477b33_0;

typedef struct st_477b33_1 {
    unsigned int field_0;
} st_477b33_1;

typedef struct st_477b33_2 {
    char padding_0[92];
    struct st_477b33_0 *field_5c;
    unsigned int field_60;
    unsigned int field_64;
} st_477b33_2;

extern int g_4adb90;
extern unsigned int g_4adb94;
extern unsigned int g_4adb9c;

int sub_477b33(void)
{
    int v8;  // ecx
    st_477b33_2 *idx;  // esi
    unsigned int v18;  // eax
    unsigned int v19;  // edi
    st_477b33_0 *v10;  // edx
    unsigned int v11;  // edi
    unsigned int v12;  // ebx
    st_477b33_0 *iter;  // ecx
    unsigned int *v14;  // ebx
    unsigned int v15;  // ecx
    int i;  // edx
    unsigned int v17;  // ecx
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    int v2;  // [bp-0x10]
    unsigned int v3;  // [bp-0xc]
    unsigned int *v4;  // [bp-0x8]
    char v5;  // [bp+0x0]
    unsigned int v6;  // [bp+0x4]
    unsigned int v7;  // [bp+0x8]

    idx = sub_4753ee(v2, v8, v8, &v5);
    if (!idx)
        return;
    v10 = idx->field_5c;
    v1 = v11;
    v0 = v12;
    iter = v10;
    do
    {
    } while (iter->field_0 != v6 && (iter += 12, iter < g_4adb9c * 12 + v10));
    if (iter >= &(&v10->field_0)[3 * g_4adb9c] || iter->field_0 != v6)
        iter = NULL;
    if (!iter || (v14 = (unsigned int *)iter->field_8, v4 = v14, !v14))
        return;
    if (v14 == 0x5)
    {
        iter->field_8 = NULL;
    }
    else if (v14 != 0x1)
    {
        v3 = idx->field_60;
        idx->field_60 = v7;
        v15 = iter->field_4;
        if (v15 == 8)
        {
            i = g_4adb90;
            if (g_4adb90 < g_4adb94 + g_4adb90)
            {
                v17 = g_4adb90 * 12;
                do
                {
                    *((unsigned int *)(v17 + (char *)idx->field_5c + 8)) = 0;
                    i += 1;
                    v17 += 12;
                } while (i < g_4adb94 + g_4adb90);
            }
            v18 = iter->field_0;
            v19 = idx->field_64;
            if (v18 == 0xc000008e)
            {
                idx->field_64 = 131;
            }
            else if (v18 == 0xc0000090)
            {
                idx->field_64 = 129;
            }
            else if (v18 == 0xc0000091)
            {
                idx->field_64 = 132;
            }
            else if (v18 == 0xc0000093)
            {
                idx->field_64 = 133;
            }
            else if (v18 == 0xc000008d)
            {
                idx->field_64 = 130;
            }
            else if (v18 == 0xc000008f)
            {
                idx->field_64 = 134;
            }
            else if (v18 == 0xc0000092)
            {
                idx->field_64 = 138;
            }
            v14(8, idx->field_64);
            idx->field_64 = v19;
        }
        else
        {
            iter->field_8 = NULL;
            v14(v15);
        }
        idx->field_60 = idx->field_60;
    }
    return;
}

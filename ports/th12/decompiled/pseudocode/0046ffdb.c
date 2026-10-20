// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46ffdb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46ffdb_0 {
    struct st_46ffdb_1 *field_0;
    unsigned int field_4;
    struct st_46ffdb_1 *field_8;
    unsigned int field_c;
    char padding_10[8];
    unsigned int field_18;
} st_46ffdb_0;

typedef struct st_46ffdb_1 {
    char field_0;
} st_46ffdb_1;

extern void g_4adbb0;
extern void g_4d6320;

int sub_46ffdb(void)
{
    st_46ffdb_0 *idx;  // esi
    int v8;  // ecx
    void* v17;  // eax
    unsigned int v18;  // eax
    unsigned int v19;  // edx
    unsigned int v9;  // eax
    unsigned int v10;  // ebx
    unsigned int v11;  // eax
    unsigned int v12;  // edi
    char *v13;  // eax
    char *v14;  // edi
    int v15;  // edi
    int v16;  // ecx
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    int v2;  // [bp-0xc]
    int v3;  // [bp-0x8]
    char v4;  // [bp+0x0]
    char v5;  // [bp+0x4], Other Possible Types: unsigned int
    st_46ffdb_0 *v6;  // [bp+0x8], Other Possible Types: int

    idx = v6;
    v6 = sub_47c1da(idx, v2, v8, &v4);
    v9 = idx->field_c;
    if (!((char)v9 & 130))
    {
        *((unsigned int *)sub_46e96f()) = 9;
    }
    else if ((char)v9 & 64)
    {
        *((unsigned int *)sub_46e96f()) = 0x22;
    }
    else
    {
        v1 = v10;
        if ((char)v9 & 1)
        {
            idx->field_4 = 0;
            if ((char)v9 & 16)
            {
                idx->field_0 = idx->field_8;
                idx->field_c = v9 & 0xfffffffe;
                goto LABEL_47003b;
            }
            else
            {
                idx->field_c = v9 | 32;
            }
        }
        else
        {
LABEL_47003b:
            v11 = idx->field_c & 0xffffffef | 2;
            idx->field_c = v11;
            idx->field_4 = 0;
            v3 = 0;
            if (!(v11 & 268) && (idx != sub_47c025() + 32 && idx != sub_47c025() + 64 || !sub_47bfc1(v6)))
                sub_47bf78(idx);
            v0 = v12;
            if (idx->field_c & 264)
            {
                v13 = &idx->field_8->field_0;
                v14 = &idx->field_0->field_0;
                idx->field_0 = v13 + 1;
                v15 = v14 - v13;
                idx->field_4 = idx->field_18 - 1;
                if (v15 > 0)
                {
                    v3 = sub_47be9c(v6, v13, v15);
                    v3 = v3;
                }
                else
                {
                    v16 = v6;
                    if (v16 != 0xffffffff && v16 != -0x2)
                        v17 = (v16 & 31) * 64 + *((int *)&(&g_4d6320)[4 * (v16 >> 5)]);
                    else
                        v17 = &g_4adbb0;
                    if ((char)v17[4] & 32 && !(v18 = sub_47b650(v16, 0, 0, 2), (sub_47b650(v16, 0, 0, 2) & v19) != 0xffffffff))
                        goto LABEL_470129;
                }
                idx->field_8->field_0 = v5;
            }
            else
            {
                v15 = 1;
                v3 = sub_47be9c(v6, &v5, 1);
                v3 = v3;
            }
            if (v3 != v15)
            {
LABEL_470129:
                idx->field_c = idx->field_c | 32;
            }
        }
        return;
    }
    idx->field_c = idx->field_c | 32;
    return;
}

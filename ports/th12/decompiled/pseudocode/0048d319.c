// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d319; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48d319_0 {
    struct st_48d319_0 *field_0;
    unsigned int field_4;
    struct st_48d319_0 *field_8;
    unsigned int field_c;
    char padding_10[8];
    unsigned int field_18;
} st_48d319_0;

extern unsigned int g_4b42f0;
extern unsigned int g_4b43a4[4];

unsigned int sub_48d319(st_48d319_0 *idx)
{
    unsigned int v5;  // eax
    unsigned int v6;  // ebx
    unsigned int v7;  // edi
    st_48d319_0 **v8;  // edi
    unsigned int v9;  // eax
    st_48d319_0 *v10;  // eax
    st_48d319_0 *v11;  // edi
    unsigned int v0;  // [bp-0x14]
    unsigned int v1;  // [bp-0x10]
    unsigned int v2;  // [bp-0xc]
    int v3;  // [bp-0x8]
    char v4;  // [bp+0x0]

    if (!sub_47bfc1(sub_47c1da(idx, v3, &v4)))
        return 0;
    if (idx == sub_47c025() + 32)
    {
        v5 = 0;
        goto LABEL_48d353;
    }
    else if (idx == sub_47c025() + 64)
    {
        v5 = 1;
LABEL_48d353:
        g_4b42f0 = g_4b42f0 + 1;
        if (!(idx->field_c & 268))
        {
            v2 = v6;
            v1 = v7;
            v8 = &g_4b43a4[v5];
            if (!*(v8) && !(v9 = sub_4777ce(0x1000), *(v8) = (st_48d319_0 *)v9, v9))
            {
                v10 = &idx->padding_10[4];
                v0 = 2;
                idx->field_8 = v10;
                idx->field_0 = v10;
                idx->field_18 = 2;
                idx->field_4 = 2;
            }
            else
            {
                v11 = *(v8);
                idx->field_8 = v11;
                idx->field_0 = v11;
                idx->field_18 = 0x1000;
                idx->field_4 = 0x1000;
            }
            idx->field_c = idx->field_c | 4354;
            return 1;
        }
    }
    return 0;
}

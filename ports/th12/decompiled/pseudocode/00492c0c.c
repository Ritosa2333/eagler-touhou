// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x492c0c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_492c0c_0 {
    char padding_0[128];
    unsigned int field_80;
} st_492c0c_0;

typedef struct st_492c0c_1 {
    char padding_0[12];
    unsigned int field_c;
} st_492c0c_1;


int sub_492c0c(void)
{
    unsigned int v11;  // ecx
    unsigned int v12;  // edi
    st_492c0c_0 *v13;  // eax
    unsigned int v14;  // eax
    int v15;  // esi
    unsigned int *iter;  // eax
    unsigned int v17;  // ebx
    void* v18;  // eax
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x8]
    unsigned int *v3;  // [bp+0x4]
    int v4;  // [bp+0x8]
    int v5;  // [bp+0xc]
    int v6;  // [bp+0x10]
    st_492c0c_1 *v7;  // [bp+0x14]
    int v8;  // [bp+0x18]
    int v9;  // [bp+0x1c]
    int v10;  // [bp+0x20]

    v2 = v11;
    if (*(v3) == 0x80000003)
        return;
    v1 = v12;
    if (*((int *)(sub_475467() + 128)))
    {
        v13 = sub_475467();
        v14 = sub_4751d5();
        if (v13->field_80 != v14 && *(v3) != 3762507597 && sub_491b69(v3, v4, v5, v6, v7, v9, v10))
            goto LABEL_492cfc;
    }
    if (!v7->field_c)
        sub_472b94(); /* do not return */
    v15 = v8;
    iter = sub_491cdf(v7, v9, v15, &v2, &v11);
    if (v2 < v11)
    {
        v0 = v17;
        do
        {
            if (v15 >= *(iter) && v15 <= iter[1])
            {
                v18 = iter[3] * 16 + iter[4];
                if ((!*((int *)((char *)v18 - 12)) || !*((char *)(*((int *)((char *)v18 - 12)) + 8))) && !(*((char *)v18 - 16) & 64))
                {
                    sub_492b9e(v3, v5, v6, v7, 0, v9, v10);
                    v15 = v8;
                }
            }
        } while ((v2 += 1, iter += 20, v2 < v11));
    }
LABEL_492cfc:
    return;
}

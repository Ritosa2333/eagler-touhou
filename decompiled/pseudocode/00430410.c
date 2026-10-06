// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x430410; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_430410_0 {
    char padding_0[2464];
    unsigned int field_9a0;
    int field_9a4;
} st_430410_0;

int sub_430410(void)
{
    st_430410_0 *idx;  // eax
    int v6;  // esi
    unsigned int v15;  // eax
    unsigned int v16;  // 4111
    unsigned int v17;  // 4120
    unsigned int v18;  // edi
    unsigned int v19;  // ebx
    int v20;  // esi
    unsigned int v21;  // edi
    unsigned int v7;  // ebx
    int v8;  // ebx
    char *v9;  // ecx
    char *iter;  // eax
    char v11;  // 4101
    unsigned int v12;  // cc_dep1
    unsigned int v13;  // cc_dep2
    char v14;  // 4103
    unsigned int v0;  // [bp-0x14]
    char v1;  // [bp-0x4], Other Possible Types: unsigned int
    unsigned int v2;  // [bp+0x4]
    unsigned int v3;  // [bp+0x8]
    unsigned int v4;  // [bp+0xc]

    v6 = idx->field_9a4;
    if (!idx->field_9a4)
        return;
    v7 = idx->field_9a0;
    if (!sub_46dc03(v2, "debug", 5, v8))
        return;
    v9 = "debug";
    iter = "0100b";
    while (1)
    {
        v11 = *(iter);
        v12 = v11;
        v13 = *(v9);
        if (v11 != *(v9))
            break;
        if (v11)
        {
            v14 = iter[1];
            v12 = v14;
            v13 = v9[1];
            if (v14 != v9[1])
                break;
            iter += 2;
            v9 += 2;
            if (!v14)
                goto LABEL_43046c;
        }
        else
        {
LABEL_43046c:
            v15 = 0;
            goto LABEL_430475;
        }
    }
    v16 = _ccall(4, v12, v13, 0);
    v17 = _ccall(12, 0, v16 & 1, v16 & 1);
    v15 = -(v16 & 1) + 1 - (v17 & 1);
LABEL_430475:
    if (!v15)
        return;
    v0 = v18;
    if (!idx->field_9a0)
        return;
    while (1)
    {
        v19 = v7;
        if (!sub_46dc03(v2, v6, 5))
        {
            v6 += 6;
            sub_46dd2d(v6, "%d %d", &v2, &v1);
            if (v2 == v3 && v1 == v4)
                return;
        }
        v20 = v6;
        v6 = sub_46d1a0(v20, 10) + 1;
        v21 = v20 - v6;
        v7 = v19 + v21;
        if (!(v19 + v21))
            return;
    }
}

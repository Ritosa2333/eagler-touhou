// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48ad74; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48ad74_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    char field_a;
} st_48ad74_0;


void sub_48ad74(char *a0, unsigned int i, st_48ad74_0 *ptr)
{
    unsigned int v6;  // edi
    st_48ad74_0 *v7;  // esi
    unsigned int v16;  // ebx
    unsigned int v17;  // esi
    unsigned int v18;  // ecx
    unsigned int v19;  // edi
    unsigned int v20;  // ebx
    unsigned int v21;  // edx
    unsigned int v22;  // esi
    unsigned int v23;  // ecx
    unsigned int v24;  // edx
    unsigned int v25;  // ecx
    unsigned int v8;  // edi
    unsigned int v26;  // edx
    unsigned int v27;  // esi
    unsigned int v28;  // edi
    unsigned int v29;  // ecx
    unsigned int v9;  // edx
    unsigned int v10;  // ecx
    unsigned int v11;  // esi
    unsigned int v12;  // edx
    unsigned int v13;  // edi
    unsigned int v14;  // ebx
    unsigned int v15;  // esi
    unsigned int v0;  // [bp-0x28]
    unsigned int iter;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]

    v0 = v6;
    iter = 16462;
    memset(ptr, 0, 12);
    if (i > 0)
    {
        do
        {
            v3 = ptr->field_0;
            v7 = &ptr->field_4;
            v4 = v7->field_0;
            v5 = v7->field_4;
            v8 = ptr->field_0 * 2;
            v9 = ptr->field_4 * 2 | ptr->field_0 >> 31;
            v2 = v8;
            v2 = 0;
            v10 = (ptr->field_8 * 2 | ptr->field_4 >> 31) * 2 | v9 >> 31;
            v11 = v8 * 2;
            v12 = v9 * 2 | v8 >> 31;
            v13 = v11 + v3;
            ptr->field_0 = v11;
            ptr->field_4 = v12;
            ptr->field_8 = v10;
            if (v13 < v11 || v13 < v3)
                v2 = 1;
            v14 = 0;
            ptr->field_0 = v13;
            if (v2)
            {
                v15 = v12 + 1;
                if (v15 < v12 || v15 < 1)
                    v14 = 1;
                ptr->field_4 = v15;
                if (v14)
                    ptr->field_8 = v10 + 1;
            }
            v16 = ptr->field_4 + v4;
            v17 = 0;
            if (v16 < ptr->field_4 || v16 < v4)
                v17 = 1;
            ptr->field_4 = v16;
            if (v17)
                ptr->field_8 = ptr->field_8 + 1;
            ptr->field_8 = ptr->field_8 + v5;
            v2 = 0;
            v18 = v13 * 2;
            v19 = v16 * 2 | v13 >> 31;
            v20 = ptr->field_8 * 2 | v16 >> 31;
            ptr->field_0 = v18;
            ptr->field_4 = v19;
            ptr->field_8 = v20;
            v21 = *(a0);
            v22 = v18 + v21;
            v3 = v21;
            if (v22 < v18 || v22 < v3)
                v2 = 1;
            ptr->field_0 = v22;
            if (v2)
            {
                v23 = v19 + 1;
                v24 = 0;
                if (v23 < v19 || v23 < 1)
                    v24 = 1;
                ptr->field_4 = v23;
                if (v24)
                    ptr->field_8 = v20 + 1;
            }
            i -= 1;
            a0 += 1;
        } while (i > 0);
    }
    for (; !ptr->field_8; ptr->field_0 = v26)
    {
        v25 = ptr->field_4;
        ptr->field_8 = v25 >> 16;
        v26 = ptr->field_0 * 0x10000;
        iter += 0xfff0;
        ptr->field_4 = v25 * 0x10000 | ptr->field_0 >> 16;
    }
    if (!(ptr->field_8 & 0x8000))
    {
        do
        {
            v27 = ptr->field_0;
            v28 = ptr->field_4;
            iter += 0xffff;
            ptr->field_0 = v27 * 2;
            v29 = ptr->field_8 * 2 | v28 >> 31;
            ptr->field_4 = v28 * 2 | v27 >> 31;
            ptr->field_8 = v29;
        } while (!(v29 & 0x8000));
    }
    *((unsigned short *)((char *)&ptr->field_8 + 2)) = iter;
    return;
}

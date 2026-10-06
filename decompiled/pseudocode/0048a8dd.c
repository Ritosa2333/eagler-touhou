// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48a8dd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48a8dd_3 {
    char padding_0[188];
    struct st_48a8dd_4 *field_bc;
} st_48a8dd_3;

typedef struct st_48a8dd_4 {
    struct st_48a8dd_2 *field_0;
} st_48a8dd_4;

typedef struct st_48a8dd_0 {
    unsigned int field_0;
    int field_4;
} st_48a8dd_0;

typedef struct st_48a8dd_2 {
    char field_0;
} st_48a8dd_2;

typedef struct st_48a8dd_1 {
    char padding_0[112];
    unsigned int field_70;
} st_48a8dd_1;

int sub_48a8dd(void)
{
    st_48a8dd_0 *index;  // eax
    unsigned int v9;  // esi
    int v10;  // edi
    int v11;  // esi
    int v12;  // ebx
    char *v13;  // ecx
    char *v14;  // eax
    char *v15;  // esi
    char *v16;  // esi
    unsigned int v0;  // [bp-0x28]
    st_48a8dd_3 *v1;  // [bp-0x14]
    st_48a8dd_1 *idx;  // [bp-0xc]
    char v3;  // [bp-0x8]
    unsigned int v4;  // [bp+0x4]
    unsigned int v5;  // [bp+0x8]
    char v6;  // [bp+0xc]
    int v7;  // [bp+0x10]

    v9 = index->field_4 - 1;
    sub_46d3b4(v7, v10, v11, v12);
    if (v13 && v4 > NULL)
    {
        if (v6 && v9 == v5)
        {
            v14 = &v13[v9 + (index->field_0 == 45)];
            *(v14) = 48;
            v14[1] = 0;
        }
        v15 = v13;
        if (index->field_0 == 45)
        {
            *(v13) = 45;
            v15 = v13 + 1;
        }
        if (index->field_4 <= NULL)
        {
            sub_48a2a6();
            *(v15) = 48;
            v16 = v15 + 1;
        }
        else
        {
            v16 = &v15[index->field_4];
        }
        if (v5 > NULL)
        {
            sub_48a2a6();
            *(v16) = v1->field_bc->field_0->field_0;
            if (index->field_4 < NULL)
            {
                if (v6 || v5 >= v5)
                    v5 = -(index->field_4);
                sub_48a2a6();
                sub_477420(v16 + 1, 48, v5);
            }
        }
        if (!v3)
            return;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return;
    }
    v0 = 22;
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    if (v3)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return;
}

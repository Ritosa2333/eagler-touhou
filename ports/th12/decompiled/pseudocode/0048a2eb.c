// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48a2eb; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48a2eb_1 {
    unsigned int field_0;
    unsigned int field_4;
    char padding_8[4];
    struct st_48a2eb_2 *field_c;
} st_48a2eb_1;

typedef struct st_48a2eb_4 {
    char padding_0[188];
    struct st_48a2eb_5 *field_bc;
} st_48a2eb_4;

typedef struct st_48a2eb_5 {
    struct st_48a2eb_2 *field_0;
} st_48a2eb_5;

typedef struct st_48a2eb_2 {
    char field_0;
} st_48a2eb_2;

typedef struct st_48a2eb_0 {
    char padding_0[112];
    unsigned int field_70;
} st_48a2eb_0;

extern char g_4b43ac;

int sub_48a2eb(void)
{
    int v10;  // edi
    int v11;  // esi
    char *v20;  // esi
    int v21;  // eax
    char *v22;  // esi
    int v23;  // edx
    char *index;  // esi
    int v25;  // edx
    int v12;  // ebx
    char *v13;  // eax
    unsigned int *v14;  // eax
    int v15;  // eax
    char *v16;  // esi
    char *v17;  // eax
    char *v18;  // esi
    char *v19;  // ecx
    unsigned int v0;  // [bp-0x28]
    st_48a2eb_4 *v1;  // [bp-0x14]
    st_48a2eb_0 *idx;  // [bp-0xc]
    char v3;  // [bp-0x8]
    int v4;  // [bp+0x4]
    int v5;  // [bp+0x8]
    unsigned int v6;  // [bp+0xc]
    st_48a2eb_1 *v7;  // [bp+0x10]
    char v8;  // [bp+0x14]
    int v9;  // [bp+0x18]

    sub_46d3b4(v9, v10, v11, v12);
    if (!v13 || v4 <= 0)
    {
        v14 = sub_46e96f();
        v0 = 22;
    }
    else
    {
        if (v5 > 0)
            v15 = v5;
        else
            v15 = 0;
        if (v4 <= v15 + 9)
        {
            v14 = sub_46e96f();
            v0 = 0x22;
        }
        else
        {
            if (v8)
                sub_48a2a6();
            v16 = v13;
            if (v7->field_0 == 45)
            {
                *(v13) = 45;
                v16 = v13 + 1;
            }
            if (v5 > 0)
            {
                v17 = v16 + 1;
                *(v16) = *(v17);
                v16 = v17;
                *(v16) = v1->field_bc->field_0->field_0;
            }
            v18 = &v16[v5 + (!v8)];
            if (sub_47a2b9(v18, (v4 == -0x1 ? 0xffffffff : v13 - v18 + v4), "e+000"))
                sub_470dc6(0, 0, 0, 0, 0);
            v19 = v18 + 2;
            if (v6)
                *(v18) = 69;
            v20 = v18 + 1;
            if (v7->field_c->field_0 != 48)
            {
                v21 = v7->field_4 - 1;
                if (v7->field_4 - 1 < 0)
                {
                    v21 = -(v21);
                    *(v20) = 45;
                }
                v22 = v20 + 1;
                if (v21 >= 100)
                {
                    v23 = v21 >> 31;
                    v0 = 100;
                    *(v22) = *(v22) + (char)(CONCAT(v23, v21) / 100);
                    v21 = CONCAT(v23, v21) % 100;
                }
                index = v22 + 1;
                if (v21 >= 10)
                {
                    v25 = v21 >> 31;
                    v0 = 10;
                    *(index) = *(index) + (char)(CONCAT(v25, v21) / 10);
                    v21 = CONCAT(v25, v21) % 10;
                }
                index[1] = index[1] + (char)v21;
            }
            if (g_4b43ac & 1 && *(v19) == 48)
                sub_47a6a0(v19, v19 + 1, 3);
            if (!v3)
                return;
            idx->field_70 = idx->field_70 & 0xfffffffd;
            return;
        }
    }
    *(v14) = v0;
    sub_470f2d(0, 0, 0, 0, 0);
    if (v3)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return;
}

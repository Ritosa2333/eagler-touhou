// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c167; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48c167_0 {
    char field_0;
    char field_1;
} st_48c167_0;

typedef struct st_48c167_1 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[12];
    unsigned int field_14;
} st_48c167_1;

typedef struct st_48c167_2 {
    char padding_0[112];
    unsigned int field_70;
} st_48c167_2;


int sub_48c167(unsigned short *a0, st_48c167_0 *a1, int a2, int a3)
{
    unsigned int v5;  // ebx
    unsigned int v6;  // esi
    st_48c167_1 *v7;  // eax
    int v8;  // ecx
    st_48c167_1 *v9;  // eax
    int v10;  // eax
    unsigned int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    st_48c167_1 *v2;  // [bp-0x14]
    st_48c167_2 *idx;  // [bp-0xc]
    char v4;  // [bp-0x8]

    v1 = v5;
    v0 = v6;
    if (!a1 || !a2)
        return 0;
    if (a1->field_0)
    {
        sub_46d3b4(a3);
        if (!v2->field_14)
        {
            if (a0)
                *(a0) = a1->field_0;
LABEL_48c1b6:
            if (!v4)
                return 1;
            idx->field_70 = idx->field_70 & 0xfffffffd;
            return 1;
        }
        else
        {
            if (sub_47c5a3(a1->field_0, &v2))
            {
                v7 = v2;
                v8 = v7[7].field_4;
                v9 = v7;
                if (v8 > 1 && !(v9 = v7, a2 < v8 || (v9 = v2, !MultiByteToWideChar(v7->field_4, 9, a1, v8, a0, (unsigned int)(char)(a0)))) || a2 >= v9[7].field_4 && a1->field_1)
                {
                    v10 = v9[7].field_4;
                    if (!v4)
                        return v10;
                    idx->field_70 = idx->field_70 & 0xfffffffd;
                    return v10;
                }
            }
            else
            {
                if (MultiByteToWideChar(v2->field_4, 9, a1, 1, a0, a0))
                    goto LABEL_48c1b6;
            }
            *((unsigned int *)sub_46e96f()) = 42;
            if (!v4)
                return -0x1;
            idx->field_70 = idx->field_70 & 0xfffffffd;
            return -0x1;
        }
    }
    else if (a0)
    {
        *(a0) = 0;
    }
    return 0;
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465fe0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465fe0_12 {
    char padding_0[8];
    int field_8;
    char padding_c[116];
    unsigned int field_80;
    unsigned int field_84;
    int field_88;
    char padding_8c[4];
    struct st_465fe0_11 *field_90;
} st_465fe0_12;

typedef struct st_465fe0_13 {
    char padding_0[12];
    struct st_465fe0_12 *field_c;
} st_465fe0_13;

typedef struct st_465fe0_11 {
    char padding_0[16];
    unsigned int field_10;
} st_465fe0_11;

typedef struct st_465fe0_1 {
    unsigned int field_0;
} st_465fe0_1;

typedef struct st_465fe0_0 {
    char padding_0[36];
    struct st_465fe0_1 *field_24;
    char padding_28[4];
    struct st_465fe0_1 *field_2c;
    char padding_30[28];
    struct st_465fe0_1 *field_4c;
    struct st_465fe0_1 *field_50;
} st_465fe0_0;

extern unsigned int g_4d4760;

int sub_465fe0(st_465fe0_0 **a0)
{
    int v9;  // eax
    unsigned int v10;  // esi
    unsigned int v20;  // edx, Other Possible Types: int
    st_465fe0_13 *idx;  // ebx
    int v12;  // edx
    int v13;  // eax
    st_465fe0_12 *idx1;  // esi
    st_465fe0_11 *v15;  // edx
    int v17;  // edi
    int v0;  // [bp-0x5c], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0x58]
    st_465fe0_0 **v2;  // [bp-0x48]
    char *v3;  // [bp-0x38]
    unsigned int v4;  // [bp-0x20]
    st_465fe0_0 **v5;  // [bp-0x1c]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]
    char v8;  // [bp-0x4]

    v6 = 0;
    a0 = 0;
    v7 = 0;
    if (!a0)
        return -2147221008;
    v5 = a0;
    v9 = *(a0)->field_24(a0, &v8);
    if (v9 < NULL)
        return v9;
    v4 = v10;
    if ((char)v6 & 2)
    {
        do
        {
            if (*(a0)->field_50(a0) == 2289565846)
                Sleep(10);
        } while (*(a0)->field_50(a0));
    }
    v20 = 0;
    v3 = &v6;
    v12 = *((int *)&idx->padding_0[8]);
    v2 = a0;
    v13 = *(a0)->field_2c(a0, 0, v12, &v5, &v6, 0, 0, 0);
    if (v13 >= NULL)
    {
        idx1 = idx->field_c;
        if (*((int *)&idx1->padding_c[112]))
        {
            v15 = idx1->field_90;
            idx1->field_84 = idx1->field_80;
            if (*((int *)&v15[1].padding_0[8]) > NULL)
                idx1->field_88 = *((int *)&v15[1].padding_0[8]);
        }
        else if (idx1->padding_8c)
        {
            v20 = idx1->field_90->field_10 + g_4d4760;
            SetFilePointer(idx1->padding_8c, v20, NULL, FILE_BEGIN);
            idx1->field_8 = *((int *)&idx1->field_90[1].padding_0[8]);
        }
        v13 = sub_466d70(&v5, &v3);
        if (v13 >= NULL)
        {
            v17 = v12;
            if (!v17)
            {
                v17 = 0;
                v20 = 0;
                v1 = 0;
                v0 = 0;
            }
            else
            {
                v20 = v20;
                if (v17 >= 0)
                    goto LABEL_4661f8;
                v20 = -(v17);
                v1 = (unsigned int)((*((short *)&idx->field_c->field_90[2].padding_0[6]) != 8) + 1) & 128;
                v0 = v17;
            }
            sub_477420(v17, v1, v20);
LABEL_4661f8:
            *(a0)->field_4c(a0, v20, 0, 0, 0);
            v13 = 0;
        }
    }
    return v13;
}

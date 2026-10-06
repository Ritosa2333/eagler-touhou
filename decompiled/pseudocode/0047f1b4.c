// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47f1b4; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47f1b4_1 {
    unsigned int field_0;
    unsigned int field_4;
} st_47f1b4_1;

typedef struct st_47f1b4_0 {
    char field_0;
} st_47f1b4_0;

extern st_47f1b4_0 *g_4b4318;

int sub_47f1b4(void)
{
    unsigned int v16;  // ebx
    char *v17;  // eax
    unsigned int v26;  // edx
    unsigned int v18;  // esi
    unsigned int v19;  // edi
    unsigned int *v20;  // edi
    unsigned int *v23;  // eax
    unsigned int *v24;  // eax
    char *v0;  // [bp-0x4c], Other Possible Types: int
    char *v1;  // [bp-0x48]
    int v2;  // [bp-0x44]
    char *v3;  // [bp-0x40], Other Possible Types: unsigned int
    unsigned int v4;  // [bp-0x3c]
    unsigned int v5;  // [bp-0x38]
    char v6;  // [bp-0x34]
    char v7;  // [bp-0x2c]
    unsigned int v8[2];  // [bp-0x24]
    char v9;  // [bp-0x1c], Other Possible Types: unsigned int
    unsigned int v10;  // [bp-0x18]
    char v11;  // [bp-0x14], Other Possible Types: unsigned int
    unsigned int v12;  // [bp-0x10]
    unsigned int iter;  // [bp-0x8]
    st_47f1b4_1 *v14;  // [bp+0x4]
    unsigned int *v15;  // [bp+0x8]

    v5 = v16;
    if (g_4b4318->field_0)
    {
        iter = sub_47d5e8();
        if (iter < 0)
            iter = 0;
        if (!iter)
        {
            v4 = 93;
            v3 = &v9;
            v17 = &v11;
            goto LABEL_47f305;
        }
        else
        {
            v12 &= 0xffff0000;
            v4 = v18;
            v3 = v19;
            v20 = v15;
            v11 = 0;
            if (v20[1] & 0x800)
                sub_47ea48("[]");
            for (; (char)v12 <= 1 && (iter -= 1, iter && g_4b4318->field_0); iter = iter)
            {
                sub_47ec02(&v6, 91, sub_47ecb6(&v7, 0, v8, 93));
                sub_47e7bf(sub_47ec6e());
            }
            if (*(v20))
            {
                v1 = &v11;
                v0 = &v6;
                if (!(v20[1] & 0x800))
                {
                    sub_47ec02(v8, 40, v20, &v7, 41);
                    sub_47ec6e(&v7, 41);
                }
                v23 = sub_47e9ae();
                v11 = *(v23);
                v12 = v23[1];
            }
            sub_4826a7(&v9, &v11);
            v14->field_0 = v9;
            v14->field_4 = v10 | 0x800;
            return;
        }
    }
    else
    {
        v24 = v15;
        v4 = 93;
        if (*(v24))
        {
            v2 = 1;
            v17 = &v7;
            v0 = ")[";
            sub_47ec92(sub_47ec02(&v9, 40, v24, v8, ")[", &v7, 1, &v6), v26, v8, ")[");
        }
        else
        {
            v3 = &v6;
            v17 = &v7;
LABEL_47f305:
            v2 = 1;
            v0 = 91;
            sub_47e56d(91, v17, 1);
        }
        sub_47e79b(v0, v17, v2);
        sub_4822f0(v14, sub_47ec6e());
        return;
    }
}

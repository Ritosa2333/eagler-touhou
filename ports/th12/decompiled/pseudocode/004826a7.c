// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4826a7; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4826a7_0 {
    char field_0;
    char field_1;
    char field_2;
    char field_3;
} st_4826a7_0;

extern st_4826a7_0 *g_4b4318;

int sub_4826a7(int a0, unsigned int *a1)
{
    st_4826a7_0 *v8;  // edx
    unsigned int v9;  // eax
    unsigned int v18;  // eax
    unsigned int v19;  // eax
    unsigned int v10;  // ebx
    unsigned int v11;  // eax
    unsigned int v12;  // esi
    unsigned int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v15;  // esi
    st_4826a7_0 *v16;  // edx
    unsigned int v17;  // eax
    unsigned int v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x20]
    char v2;  // [bp-0x1c]
    unsigned int v3;  // [bp-0x14]
    unsigned int v4;  // [bp-0x10]
    unsigned int v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]

    v8 = g_4b4318;
    v9 = v8->field_0;
    v1 = v10;
    v7 &= 0xffff0000;
    v11 = v9 - 0;
    v0 = v12;
    v5 = 0;
    if (v9)
    {
        v13 = v11 - 36;
        if (v11 != 36)
        {
            if (v13 != 29)
            {
                if (v13 != 30)
                {
                    sub_4822f0(a0, a1);
                    return a0;
                }
                sub_47e896(&v5, v8, "volatile");
                if (*(a1))
                    sub_47e9f6(32);
            }
            v14 = *(a1);
            v15 = a1[1];
            g_4b4318 = &g_4b4318->field_1;
            v3 = v14;
            v4 = v15 | 0x100;
            sub_48219f(a0, &v5, &v3);
        }
        else if (v8->field_1 != 36)
        {
            if (v8->field_1)
            {
                sub_47e1ce(2);
                return a0;
            }
            goto LABEL_4827c8;
        }
        else
        {
            v16 = &v8->field_2;
            g_4b4318 = v16;
            v17 = v16->field_0;
            v18 = v17 - 0;
            if (!v17)
                goto LABEL_4827c8;
            v19 = v18 - 65;
            if (v18 == 65)
            {
                g_4b4318 = &v16->field_1;
                sub_481720(a0, a1);
                return a0;
            }
            if (v19 != 1)
            {
                if (v19 != 2)
                {
                    sub_47e1ce(2);
                    return a0;
                }
                v7 &= 0xffff0000;
                g_4b4318 = &v16->field_1;
                v6 = 0;
                sub_4822f0(a0, sub_481a74(&v2, a1, 0, &v6, 0));
                return a0;
            }
            g_4b4318 = &v16->field_1;
            sub_47f89d(a0, a1, 1);
        }
    }
    else
    {
LABEL_4827c8:
        sub_47ec26(a0, 1, a1);
    }
    return a0;
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48206c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48206c_0 {
    char field_0;
} st_48206c_0;

extern st_48206c_0 *g_4b4318;

int sub_48206c(void)
{
    char v7;  // al
    unsigned int v8;  // ebx
    unsigned int v17;  // ecx
    unsigned int v9;  // esi
    unsigned int v10;  // edx
    unsigned int *v11;  // esi
    unsigned int v12;  // edx
    unsigned int v13;  // edx
    unsigned int v14;  // edx
    unsigned int v15;  // edi
    unsigned int *v16;  // edi
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x10]
    unsigned int *v3;  // [bp+0x4]
    unsigned int *v4;  // [bp+0x8]
    unsigned int *v5;  // [bp+0xc]
    int v6;  // [bp+0x10]

    v7 = g_4b4318->field_0;
    v2 = v8;
    v1 = v9;
    if (g_4b4318->field_0)
    {
        if (v7 >= 54 && v7 <= 57 || v7 == 95)
        {
            sub_47e56d(&v17, v10, v6);
            v11 = v5;
            if (*(v4) && (!*(v11) || !(v11[1] & 0x100)))
                sub_47e7bf(&v17, v10, v4);
            if (*(v11))
                sub_47e7bf(&v17, v12, v11);
            sub_481720(v3, &v17);
        }
        else
        {
            sub_481a74(&v17, v5, v6, v4, 0);
            sub_47f89d(v3, &v17, (char)v6 == 42);
        }
        return;
    }
    else
    {
        sub_47e1ce(1);
        sub_47e9f6(v6);
        if (*(v4))
            sub_47e7bf(&v17, v13, v4);
        v0 = v15;
        v16 = v5;
        if (*(v16))
        {
            if (*(v4))
                sub_47e9f6(32);
            sub_47e7bf(&v17, v14, v16);
        }
        *(v3) = v17;
        v3[1] = v17;
        return;
    }
}

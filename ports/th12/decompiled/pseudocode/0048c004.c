// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48c004; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48c004_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    char padding_10[20];
    char field_24;
} st_48c004_0;

extern st_48c004_0 g_4adbb0;
extern st_48c004_0 g_4d6320;

unsigned int sub_48c004(unsigned int a0, void* idx)
{
    unsigned int v3;  // esi
    unsigned int v4;  // edi
    int v5;  // eax
    st_48c004_0 *v6;  // ecx
    st_48c004_0 *v7;  // eax
    unsigned int v8;  // ebx
    char v9;  // al
    char *v10;  // eax
    unsigned int v11;  // eax
    unsigned int v12;  // eax
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    v2 = v3;
    v1 = v4;
    if (!((char)idx[12] & 64))
    {
        v5 = sub_47c1da(idx);
        if (v5 != 0xffffffff && v5 != -0x2)
            v6 = (v5 & 31) * 64 + (&g_4d6320.field_0)[v5 >> 5];
        else
            v6 = &g_4adbb0.field_0;
        if (!(v6->field_24 & 127))
        {
            if (v5 != 0xffffffff && v5 != -0x2)
                v7 = (v5 & 31) * 64 + (&g_4d6320.field_0)[v5 >> 5];
            else
                v7 = &g_4adbb0.field_0;
            if (!(v7->field_24 & 128))
                goto LABEL_48c08d;
        }
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return 0xffffffff;
    }
    else
    {
LABEL_48c08d:
        v0 = v8;
        if (a0 != 0xffffffff && !(v9 = (char)(int)idx[12], !(v9 & 1) && (v9 >= 0 || v9 & 2)))
        {
            if (!(int)idx[8])
                sub_47bf78(idx);
            if (*((int *)idx) == (int)idx[8] && !(int)idx[4])
                *((unsigned int *)idx) = *((int *)idx) + 1;
            *((unsigned int *)idx) = *((int *)idx) - 1;
            v10 = *((int *)idx);
            if ((char)idx[12] & 64)
            {
                if (*(v10) == (char)a0)
                    goto LABEL_48c0dd;
                *((char **)idx) = v10 + 1;
                goto LABEL_48c0d3;
            }
            else
            {
                *(v10) = a0;
LABEL_48c0dd:
                v11 = (int)idx[12];
                *((unsigned int *)&idx[4]) = (int)idx[4] + 1;
                *((unsigned int *)&idx[12]) = v11 & 0xffffffef | 1;
                v12 = a0 & 0xff;
            }
        }
        else
        {
LABEL_48c0d3:
            v12 = 0xffffffff;
        }
        return v12;
    }
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c397; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47c397_0 {
    char padding_0[4];
    unsigned int field_4;
    char padding_8[12];
    unsigned int field_14;
} st_47c397_0;

typedef struct st_47c397_1 {
    char padding_0[112];
    unsigned int field_70;
} st_47c397_1;


unsigned int sub_47c397(int *a0, char *a1, int a2, unsigned short a3, int a4)
{
    unsigned int v9;  // ebx
    unsigned int v10;  // esi
    unsigned int v11;  // edi
    int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    unsigned int v3;  // [bp-0x1c]
    unsigned int v4;  // [bp-0x18]
    st_47c397_0 *v5;  // [bp-0x14]
    st_47c397_1 *idx;  // [bp-0xc]
    char v7;  // [bp-0x8]
    unsigned int v8;  // [bp+0x8]

    v4 = v9;
    v3 = v10;
    v2 = v11;
    if (!a1 && a2 > NULL)
    {
        if (!a0)
            return 0;
        *(a0) = 0;
        return 0;
    }
    if (a0)
        *(a0) = -0x1;
    if (a2 > 0x7fffffff)
    {
        v1 = 22;
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        return 22;
    }
    sub_46d3b4(a4);
    if (v5->field_14)
    {
        v8 = 0;
        v13 = WideCharToMultiByte(v5->field_4, 0, &a3, 1, a1, a2, NULL, &a1);
        if (v13)
        {
            if (!v8)
            {
                if (a0)
                    *(a0) = v13;
LABEL_47c48d:
                if (!v7)
                    return 0;
                idx->field_70 = idx->field_70 & 0xfffffffd;
                return 0;
            }
        }
        else
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
            {
                if (a1 && a2 > NULL)
                    sub_477420(a1, 0, a2);
LABEL_47c452:
                v0 = 0x22;
                *((unsigned int *)sub_46e96f()) = 0x22;
                sub_470f2d(0, 0, 0, 0, 0);
                if (v7)
                {
                    idx->field_70 = idx->field_70 & 0xfffffffd;
                    return 0x22;
                }
                return 0x22;
            }
        }
    }
    else if (a3 <= 0xff)
    {
        if (a1)
        {
            if (a2 <= 0)
                goto LABEL_47c452;
            *(a1) = a3;
        }
        if (a0)
        {
            *(a0) = 1;
            goto LABEL_47c48d;
        }
    }
    else if (a1 && a2 > NULL)
    {
        sub_477420(a1, 0, a2);
    }
    *((unsigned int *)sub_46e96f()) = 42;
    v14 = *((int *)sub_46e96f());
    if (!v7)
        return v14;
    idx->field_70 = idx->field_70 & 0xfffffffd;
    return v14;
}

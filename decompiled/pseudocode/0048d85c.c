// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48d85c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48d85c_1 {
    char padding_0[20];
    unsigned int field_14;
} st_48d85c_1;

typedef struct st_48d85c_0 {
    char padding_0[112];
    unsigned int field_70;
} st_48d85c_0;

unsigned int sub_48d85c(char *a0, char *a1, int a2, int a3)
{
    unsigned int v6;  // ebx
    unsigned int v7;  // esi
    unsigned int v8;  // edi
    char *v9;  // edi
    unsigned int v10;  // eax
    unsigned int v12;  // esi
    unsigned int v13;  // eax
    unsigned int v0;  // [bp-0x20]
    unsigned int v1;  // [bp-0x1c]
    unsigned int v2;  // [bp-0x18]
    st_48d85c_1 *v3;  // [bp-0x14]
    st_48d85c_0 *idx;  // [bp-0xc]
    char v5;  // [bp-0x8]

    v2 = v6;
    v1 = v7;
    v0 = v8;
    if (!a2)
        return 0;
    sub_46d3b4(a3);
    if (a0)
    {
        v9 = a1;
        if (v9)
        {
            if (a2 > 0x7fffffff)
            {
                *((unsigned int *)sub_46e96f()) = 22;
                sub_470f2d(0, 0, 0, 0, 0);
                if (!v5)
                    return 0x7fffffff;
                idx->field_70 = idx->field_70 & 0xfffffffd;
                return 0x7fffffff;
            }
            else
            {
                if (!v3->field_14)
                {
                    v10 = sub_48edb0(a0, v9, a2);
                }
                else
                {
                    do
                    {
                        a0 += 1;
                        v12 = sub_47aa12(*(a0), &v3);
                        v13 = sub_47aa12(*(v9), &v3);
                        v9 += 1;
                        a2 -= 1;
                    } while (a2 != 1 && v12 && (a2 = a2, v12 == v13));
                    v10 = v12 - v13;
                }
                if (!v5)
                    return v10;
                idx->field_70 = idx->field_70 & 0xfffffffd;
                return v10;
            }
        }
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    if (!v5)
        return 0x7fffffff;
    idx->field_70 = idx->field_70 & 0xfffffffd;
    return 0x7fffffff;
}

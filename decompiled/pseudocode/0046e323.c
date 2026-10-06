// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x46e323; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46e323_1 {
    char padding_0[20];
    unsigned int field_14;
} st_46e323_1;

typedef struct st_46e323_0 {
    char padding_0[112];
    unsigned int field_70;
} st_46e323_0;

unsigned int sub_46e323(char *a0, char *a1, int a2)
{
    int v4;  // ebx
    char *v5;  // edi
    int v6;  // edi
    unsigned int v7;  // eax
    unsigned int v8;  // esi
    unsigned int v10;  // esi
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0x24]
    st_46e323_1 *v1;  // [bp-0x14]
    st_46e323_0 *idx;  // [bp-0xc]
    char v3;  // [bp-0x8]

    sub_46d3b4(a2, v4);
    if (!a0)
    {
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
        if (!v3)
            return 0x7fffffff;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return 0x7fffffff;
    }
    else
    {
        v5 = a1;
        if (!v5)
        {
            *((unsigned int *)sub_46e96f(v6)) = 22;
            sub_470f2d(0, 0, 0, 0, 0);
            if (v3)
                idx->field_70 = idx->field_70 & 0xfffffffd;
            v7 = 0x7fffffff;
        }
        else
        {
            if (!v1->field_14)
            {
                v7 = sub_46e2ea(a0, v5);
            }
            else
            {
                v0 = v8;
                do
                {
                    a0 += 1;
                    v10 = sub_47aa12(*(a0), &v1);
                    v11 = sub_47aa12(*(v5), &v1);
                    v5 += 1;
                } while (v10 && v10 == v11);
                v7 = v10 - v11;
            }
            if (v3)
                idx->field_70 = idx->field_70 & 0xfffffffd;
        }
        return v7;
    }
}

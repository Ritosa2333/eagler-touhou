// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473197; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_473197_2 {
    char padding_0[172];
    int field_ac;
    char padding_b0[24];
    unsigned int field_c8;
} st_473197_2;

typedef struct st_473197_0 {
    char field_0;
    char padding_1[1];
    char field_2;
} st_473197_0;

typedef struct st_473197_1 {
    char padding_0[112];
    unsigned int field_70;
} st_473197_1;

int sub_473197(void)
{
    unsigned int v12;  // edi
    unsigned int v13;  // edx
    unsigned int v22;  // ecx
    unsigned int v23;  // ecx
    unsigned int v24;  // ecx
    void* v25;  // edi
    void* v14;  // esi
    unsigned int v15;  // ebx
    char v16;  // bl
    void* iter;  // edi
    st_473197_2 *v18;  // ecx
    unsigned int v19;  // eax
    void* v20;  // edi
    unsigned int v21;  // eax
    unsigned int v0;  // [bp-0x28]
    unsigned int v1;  // [bp-0x24]
    unsigned int v2;  // [bp-0x20]
    st_46d3b4_0 v3;  // [bp-0x18]
    st_473197_1 *idx;  // [bp-0x10]
    char v5;  // [bp-0xc]
    unsigned int v6;  // [bp-0x8]
    unsigned int *v7;  // [bp+0x4]
    void* v8;  // [bp+0x8]
    st_473197_0 **v9;  // [bp+0xc]
    unsigned int v10;  // [bp+0x10]
    unsigned int v11;  // [bp+0x14]

    v2 = v12;
    sub_46d3b4(&v3, v13, v7);
    v14 = v8;
    if (v9)
        *(v9) = v14;
    if (v14 && (!v10 || v10 >= 2 && v10 <= 36))
    {
        v1 = v15;
        v16 = *((char *)v14);
        v6 = 0;
        iter = v14 + 1;
        v18 = (st_473197_2 *)v3;
        while (1)
        {
            if (v18->field_ac > 1)
            {
                v19 = sub_4758eb(v16, 8, &v3);
                v18 = (st_473197_2 *)v3;
            }
            else
            {
                v19 = *((short *)(v18->field_c8 + v16 * 2)) & 8;
            }
            if (!v19)
                break;
            v16 = *((char *)iter);
            iter += 1;
        }
        if (v16 == 45)
        {
            v11 |= 2;
        }
        else if (!(v16 == 43))
        {
            goto LABEL_473253;
        }
        v16 = *((char *)iter);
        iter += 1;
LABEL_473253:
        if (v10 >= NULL && v10 != 1 && v10 <= 36)
        {
            if (!v10)
            {
                if (v16 != 48)
                {
                    v10 = 10;
                }
                else if (*((char *)iter) != 120 && *((char *)iter) != 88)
                {
                    v10 = 8;
                }
                else
                {
                    v10 = 16;
LABEL_4732a8:
                    if (*((char *)iter) == 120 || *((char *)iter) == 88)
                    {
                        v20 = iter + 1;
                        v16 = *((char *)v20);
                        iter = v20 + 1;
                    }
                }
            }
            else
            {
                if (v10 == 16 && v16 == 48)
                    goto LABEL_4732a8;
            }
            v21 = 0xffffffff / v10;
            while (1)
            {
                v22 = *((short *)(v18->field_c8 + v16 * 2));
                if ((char)v22 & 4)
                {
                    v23 = v16 - 48;
                }
                else
                {
                    if (!(v22 & 259))
                        break;
                    v24 = v16;
                    if (v16 - 97 <= 25)
                        v24 -= 32;
                    v23 = v24 - 55;
                }
                if (v23 >= v10)
                    break;
                v11 |= 8;
                if (v6 >= v21 && (v6 != v21 || v23 > (unsigned int)(0xffffffff % v10)))
                {
                    v11 |= 4;
                    if (!v9)
                        break;
                }
                else
                {
                    v6 = v10 * v6 + v23;
                }
                v16 = *((char *)iter);
                iter += 1;
            }
            v25 = iter - 1;
            if (!((char)v11 & 8))
            {
                if (v9)
                    v25 = v8;
                v6 = 0;
            }
            else if ((char)v11 & 4 || !((char)v11 & 1) && (v11 & 2 && v6 > 0x80000000 || !(v11 & 2) && v6 > 0x7fffffff))
            {
                *((unsigned int *)sub_46e96f()) = 0x22;
                if ((char)v11 & 1)
                {
                    v6 = 0xffffffff;
                }
                else
                {
                    v0 = 0;
                    v6 = (unsigned int)(((char)v11 & 2) + 0x7fffffff);
                }
            }
            if (v9)
                *(v9) = v25;
            if ((char)v11 & 2)
                v6 = -(v6);
            if (v5)
                idx->field_70 = idx->field_70 & 0xfffffffd;
        }
        else
        {
            if (v9)
                *(v9) = v14;
            if (v5)
                idx->field_70 = idx->field_70 & 0xfffffffd;
        }
        return;
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    if (v5)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return;
}

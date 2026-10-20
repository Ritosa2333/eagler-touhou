// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x473457; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_473457_2 {
    char padding_0[172];
    int field_ac;
    char padding_b0[24];
    unsigned int field_c8;
} st_473457_2;

typedef struct st_473457_0 {
    char field_0;
    char field_1;
    char field_2;
} st_473457_0;

typedef struct st_473457_1 {
    char padding_0[112];
    unsigned int field_70;
} st_473457_1;


int sub_473457(int a0, char *a1, st_473457_0 **a2, unsigned int a3, unsigned int a4)
{
    int v15;  // edi
    int v16;  // ebx
    unsigned int v26;  // ecx
    int v27;  // edx
    char v28;  // cl
    unsigned int v29;  // eax
    unsigned int v30;  // esi
    unsigned int v31;  // eax
    unsigned int v32;  // eax
    char *v17;  // edi
    unsigned int v18;  // esi
    st_473457_2 *v19;  // esi
    char *v20;  // edi
    unsigned int v21;  // eax
    char *v22;  // edi
    char *v23;  // edi
    int v24;  // edi
    unsigned int v0;  // [bp-0x50]
    unsigned int v1;  // [bp-0x4c]
    st_473457_2 *v2;  // [bp-0x3c]
    st_473457_1 *idx;  // [bp-0x34]
    char v4;  // [bp-0x30]
    int v5;  // [bp-0x28]
    unsigned int v6;  // [bp-0x24]
    unsigned int v7;  // [bp-0x20]
    int v8;  // [bp-0x1c]
    int v9;  // [bp-0x18]
    int v10;  // [bp-0x14]
    int v11;  // [bp-0x14]
    int v12;  // [bp-0x10], Other Possible Types: unsigned int
    char *iter;  // [bp-0xc]
    char v14;  // [bp-0x5]

    sub_46d3b4(a0, v15, v16);
    v17 = a1;
    if (a2)
        *(a2) = v17;
    if (v17 && (!a3 || a3 >= 2 && a3 <= 36))
    {
        v1 = v18;
        v11 = 0;
        v12 = 0;
        v19 = v2;
        do
        {
            v20 = v17 + 1;
            v14 = *(v17);
            if (v19->field_ac > 1)
            {
                v21 = sub_4758eb(v14, 8, &v2);
                v19 = v2;
            }
            else
            {
                v21 = *((short *)(v19->field_c8 + v14 * 2)) & 8;
            }
        } while ((v17 = v20, v21));
        iter = v20;
        if (v14 == 45)
        {
            a4 |= 2;
        }
        else if (!(v14 == 43))
        {
            goto LABEL_473520;
        }
        v22 = v20 + 1;
        iter = v22;
        v14 = *(v20);
        v20 = v22;
LABEL_473520:
        v0 = 16;
        if (a3)
        {
LABEL_47354d:
            if (a3 == 16 && v14 == 48 && (*(v20) == 120 || *(v20) == 88))
            {
                v23 = v20 + 1;
                v14 = *(v23);
                iter = v23 + 1;
            }
        }
        else if (v14 != 48)
        {
            a3 = 10;
        }
        else if (*(v20) != 120 && *(v20) != 88)
        {
            a3 = 8;
        }
        else
        {
            a3 = 16;
            goto LABEL_47354d;
        }
        v24 = a3;
        v5 = v24 >> 31;
        v7 = 0;
        v6 = v26;
        v8 = sub_47c890(-0x1, -0x1, v24, v5);
        v9 = v27;
        while (1)
        {
            v28 = v14;
            v29 = *((short *)(v19->field_c8 + v14 * 2));
            if ((char)v29 & 4)
            {
                v30 = v28 - 48;
            }
            else
            {
                if (!(v29 & 259))
                    break;
                v31 = v28;
                if (v28 - 97 <= 25)
                    v31 -= 32;
                v30 = v31 - 55;
            }
            if (v30 >= a3)
                break;
            a4 |= 8;
            if (v12 >= v9 && (v12 > v9 || v11 >= v8) && (v11 != v8 || v12 != v9 || NULL >= v7 && (NULL > v7 || v30 > v6)))
            {
                a4 |= 4;
                if (!a2)
                    break;
            }
            else
            {
                v32 = sub_483220(v24, v5, v11, v12);
                v11 = v32 + v30;
                v12 = v27 + (v32 + v30 < v32);
            }
            iter += 1;
            v14 = *(iter);
        }
        iter -= 1;
        if (!((char)a4 & 8))
        {
            if (a2)
                iter = a1;
            v11 = 0;
            v12 = 0;
        }
        else if ((char)a4 & 4 || !((char)a4 & 1) && (a4 & 2 && (v12 > -0x80000000 || v12 >= -0x80000000 && v11 > NULL) || !(a4 & 2) && v12 >= 0x7fffffff && (v12 > 0x7fffffff || v11 > -0x1)))
        {
            *((unsigned int *)sub_46e96f()) = 0x22;
            if ((char)a4 & 1)
            {
                v11 = -0x1;
                v12 = -0x1;
            }
            else if ((char)a4 & 2)
            {
                v11 = 0;
                v12 = -0x80000000;
            }
            else
            {
                v11 = -0x1;
                v12 = 0x7fffffff;
            }
        }
        if (a2)
            *(a2) = iter;
        if ((char)a4 & 2)
        {
            v10 = -(v11);
            v12 = -(v12 + (0 < v11));
            v11 = v10;
        }
        if (!v4)
            return v11;
        idx->field_70 = idx->field_70 & 0xfffffffd;
        return v11;
    }
    *((unsigned int *)sub_46e96f()) = 22;
    sub_470f2d(0, 0, 0, 0, 0);
    if (v4)
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return 0;
}

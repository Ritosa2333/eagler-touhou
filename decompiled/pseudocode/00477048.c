// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477048; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_477048_0 {
    unsigned int field_0;
    unsigned int field_4;
    int field_8;
    unsigned int field_c;
    unsigned int field_10;
    int field_14;
    unsigned int field_18;
    int field_1c;
    unsigned int field_20;
} st_477048_0;

typedef struct st_477048_1 {
    unsigned int field_0;
    int field_4;
} st_477048_1;

extern unsigned int g_4ae29c;
extern unsigned int g_4ae2d0;

unsigned int sub_477048(st_477048_0 *idx, st_477048_1 *a1)
{
    int v8;  // edi
    int v9;  // esi
    int v18;  // eax
    int v19;  // edi
    int v20;  // ecx
    unsigned int v21;  // 4101
    unsigned int v22;  // 4101
    int v23;  // eax
    unsigned int v24;  // eax
    int v25;  // edi
    unsigned int *v26;  // edx
    int i;  // eax
    int v10;  // ebx
    unsigned int v28;  // ecx
    unsigned int index;  // ecx
    int v30;  // eax
    unsigned int v31;  // eax
    int v32;  // edi
    unsigned int v33;  // eax
    int v11;  // ecx
    int v12;  // eax
    int v13;  // eax
    unsigned int v14;  // edx
    unsigned int v15;  // eax
    unsigned int v16;  // edi
    int v17;  // edi
    unsigned int v0;  // [bp-0x84]
    unsigned int v1;  // [bp-0x54]
    unsigned int v2;  // [bp-0x34]
    unsigned int v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x14]
    int iter;  // [bp-0x10]
    int v6;  // [bp-0xc]
    unsigned int v7;  // [bp-0x8]

    v7 = 0;
    if (!idx || (sub_477420(idx, 0xff, 36), !a1))
    {
        v3 = 22;
        *((unsigned int *)sub_46e96f(v8, v9)) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        v4 = a1->field_0;
        iter = a1->field_4;
        if ((iter > -0x1 || iter >= -0x1 && v4 >= 0xffff5740) && (iter < 7 || iter <= 7 && v4 <= 2470520527))
        {
            v11 = sub_4774a0(v4, iter, 31536000, 0, v10) + 70;
            v2 = 100;
            v6 = v11 - 1;
            idx = v11;
            v12 = (int)((v11 + 299) / 400) - v6 / 100 + ((int)(v6 + (v6 >> 31 & 3)) >> 2) - 0x11;
            v13 = sub_483220(v11 - 70, (int)(v11 - 70) >> 31, -0x16d, -0x1);
            v15 = sub_483220(v13 - v12, v14 - (v12 >> 31) - (v13 < v12), 86400, 0);
            v16 = v4;
            v17 = v16 + v15;
            iter = iter + v14 + (v16 + v15 < v16);
            if (iter <= NULL && (iter < NULL || v17 < NULL))
            {
                v18 = v6;
                v19 = v17 + 31536000;
                iter += v17 + 31536000 < v17;
                v20 = v18;
                idx = v18;
                if ((v20 & 0x80000003) < NULL)
                {
                    v21 = _ccall(4, 18, ((v20 & 0x80000003) - 1 | 0xfffffffc) + 1, 0, 0);
                    if (v21 & 1)
                        goto LABEL_47716d;
                }
                else if (!(v20 & 0x80000003))
                {
LABEL_47716d:
                    v1 = 100;
                    if (v18 % 100)
                        goto LABEL_47718b;
                    v18 = idx;
                }
                v17 = v19;
                if ((int)((v18 + 1900) % 400))
                    goto LABEL_4771cc;
LABEL_47718b:
                v17 = v19 + 86400;
                iter += v19 + 86400 < v19;
                goto LABEL_4771c5;
            }
            else
            {
                if ((idx & 0x80000003) < NULL)
                    v22 = _ccall(4, 18, ((idx & 0x80000003) - 1 | 0xfffffffc) + 1, 0, 0);
                if (!(idx & 0x80000003) && !(v1 = 100, !(idx % 100)) || !((idx + 1900) % 400))
                {
LABEL_4771c5:
                    v7 = 1;
                }
            }
LABEL_4771cc:
            idx->field_14 = idx;
            v23 = sub_4774a0(v17, iter, 86400, 0);
            idx->field_1c = v23;
            v24 = sub_483220(v23, v23 >> 31, -0x15180, -0x1);
            v25 = v17 + v24;
            iter = iter + v14 + (v17 + v24 < v17);
            v26 = &g_4ae29c;
            if (!v7)
                v26 = &g_4ae2d0;
            i = idx->field_1c;
            v28 = 1;
            if (v26[1] < i)
            {
                do
                {
                    v28 += 1;
                } while (v26[v28] < i);
            }
            index = v28 - 1;
            idx->field_10 = index;
            idx->field_c = i - v26[index];
            v0 = 7;
            idx->field_18 = (sub_4774a0(a1->field_0, a1->field_4, 86400, 0) + 4) % 7;
            v30 = sub_4774a0(v25, iter, 3600, 0);
            idx->field_8 = v30;
            v31 = sub_483220(v30, v30 >> 31, -0xe10, -0x1);
            v32 = v25 + v31;
            iter = iter + v14 + (v25 + v31 < v25);
            v33 = sub_4774a0(v32, iter, 60, 0);
            idx->field_4 = v33;
            idx->field_20 = 0;
            idx->field_0 = v32 - v33 * 60;
            return 0;
        }
        v3 = 22;
        *((unsigned int *)sub_46e96f()) = 22;
    }
    return 22;
}

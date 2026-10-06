// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x477602; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_477602_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
    unsigned int field_10;
    unsigned int field_14;
    unsigned int field_18;
    int field_1c;
    unsigned int field_20;
} st_477602_0;

extern unsigned int g_4ae29c;
extern unsigned int g_4ae2d0;

unsigned int sub_477602(st_477602_0 *idx, int *a1)
{
    unsigned int v3;  // ecx
    int v4;  // ecx
    unsigned int v13;  // edx
    unsigned int index;  // edx
    int v15;  // eax
    unsigned int v16;  // eax
    int v17;  // ecx
    unsigned int v18;  // eax
    unsigned int v5;  // edi
    unsigned int v6;  // eax
    int iter;  // ecx
    unsigned int node;  // edx
    int v9;  // eax
    unsigned int *v10;  // edi
    int v11;  // ecx
    int i;  // eax
    unsigned int v0;  // [bp-0x18]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x8]

    v2 = v3;
    v2 = 0;
    if (!idx || (sub_477420(idx, 0xff, 36), !a1))
    {
        v1 = 22;
        *((unsigned int *)sub_46e96f()) = 22;
        sub_470f2d(0, 0, 0, 0, 0);
    }
    else
    {
        v4 = *(a1);
        if (v4 < -0xa8c0)
        {
            v1 = 22;
            *((unsigned int *)sub_46e96f()) = 22;
        }
        else
        {
            v1 = v5;
            v6 = v4 / 126230400;
            iter = v4 + v6 * 4168736896;
            node = v6 * 4 + 70;
            if (iter >= 31536000)
            {
                iter -= 31536000;
                node += 1;
                if (iter >= 31536000)
                {
                    iter -= 31536000;
                    node += 1;
                    if (iter >= 0x1e28500)
                    {
                        node += 1;
                        iter -= 0x1e28500;
                    }
                    else
                    {
                        v2 = 1;
                    }
                }
            }
            idx->field_14 = node;
            v9 = iter / 86400;
            v10 = &g_4ae29c;
            idx->field_1c = v9;
            v11 = iter + v9 * 0xfffeae80;
            if (!v2)
                v10 = &g_4ae2d0;
            i = idx->field_1c;
            v13 = 1;
            if (v10[1] < i)
            {
                do
                {
                    v13 += 1;
                } while (v10[v13] < i);
            }
            index = v13 - 1;
            idx->field_10 = index;
            idx->field_c = i - v10[index];
            v15 = *(a1);
            v0 = 7;
            v0 = 60;
            idx->field_20 = 0;
            idx->field_18 = ((int)(v15 / 86400) + 4) % 7;
            v16 = v11 / 3600;
            idx->field_8 = v16;
            v17 = v11 + v16 * 0xfffff1f0;
            v18 = v17 / 60;
            idx->field_4 = v18;
            idx->field_0 = v17 - v18 * 60;
            return 0;
        }
    }
    return 22;
}

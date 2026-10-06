// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47e5d3; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47e5d3_0 {
    char field_0;
    char field_1;
} st_47e5d3_0;

typedef struct st_47e4f1_1 {
    struct st_47e4f1_0 *field_0;
    char field_4;
} st_47e4f1_1;

typedef struct st_47e4f1_0 {
    char padding_0[4];
    char field_4;
} st_47e4f1_0;

extern unsigned int g_4b4328;

st_47e4f1_1 * sub_47e5d3(st_47e4f1_1 *idx, unsigned int a1, st_47e5d3_0 **a2, char a3)
{
    unsigned int v1;  // edx
    unsigned int v2;  // edi
    st_47e5d3_0 *v3;  // eax
    st_47e5d3_0 *v4;  // ecx
    char v5;  // al
    char *v6;  // ecx
    st_47e5d3_0 *v7;  // eax
    char v8;  // cl
    unsigned int v0;  // [bp-0xc]

    v1 = 0;
    v0 = v2;
    idx->field_4 = 0;
    *((unsigned int *)&idx->field_4) = *((int *)&idx->field_4) & 0xffff00ff;
    idx->field_0 = 0;
    v3 = *(a2);
    if (v3)
    {
        if (v3->field_0)
        {
            a2 = v3;
            while (1)
            {
                v4 = *(a2);
                v5 = v4->field_0;
                if (v4->field_0 == a3)
                {
LABEL_47e656:
                    sub_47e4f1(idx, v1, a2, v1);
                    v7 = *(a2);
                    v8 = v7->field_0;
                    if (v7->field_0)
                    {
                        *(a2) = &v7->field_1;
                        if (v8 == a3)
                            return idx;
                        idx->field_0 = 0;
                        idx->field_4 = 3;
                        return idx;
                    }
                    else if (!idx->field_4)
                    {
                        idx->field_4 = 1;
                        return idx;
                    }
                    else
                    {
                        return idx;
                    }
                }
                if (v5 != 95 && v5 != 36 && v5 != 60 && v5 != 62 && v5 != 45 && (v5 < 97 || v5 > 122) && (v5 < 65 || v5 > 90) && (v5 < 48 || v5 > 57) && (v5 < 128 || v5 > 254) && !(g_4b4328 & 0x10000))
                    break;
                v1 += 1;
                v6 = &v4->field_1;
                *(a2) = v6;
                if (!*(v6))
                    goto LABEL_47e656;
            }
        }
        else
        {
            idx->field_4 = 1;
            return idx;
        }
    }
    idx->field_4 = 2;
    return idx;
}

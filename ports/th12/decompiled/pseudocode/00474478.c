// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x474478; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_474478_0 {
    char padding_0[143];
    char field_8f;
} st_474478_0;

unsigned int sub_474478(st_474478_0 *a0, char *a1)
{
    st_474478_0 *v5;  // edi
    int v6;  // edi
    char *v13;  // esi
    int v14;  // eax
    char *v7;  // eax
    unsigned int v8;  // cc_op
    unsigned int v10;  // 4107
    char *v11;  // edi
    char v12;  // bl
    st_474478_0 *v0;  // [bp-0x20]
    int v1;  // [bp-0x1c]
    int v2;  // [bp-0xc]
    int v3;  // [bp-0x8]
    char v4;  // [bp+0x0]

    v5 = a0;
    sub_477420(v5, 0, 144, v6, v2, v3, &v4);
    v13 = a1;
    if (!*(v13))
        return 0;
    if (*(v13) == 46 && !(v7 = v13 + 1, !*(v7)))
    {
        if (sub_4830fd(&v5->padding_0[128], 16, v7, 15))
            sub_470dc6(0, 0, 0, 0, 0);
        v5->field_8f = 0;
    }
    else
    {
        a1 = 0;
        v14 = sub_4859c0(v13, "_.,");
        v8 = 6;
        while (1)
        {
            v10 = _ccall(4, v8, v14, 0, 0);
            if (v10 & 1)
            {
LABEL_474599:
                return 0xffffffff;
            }
            v11 = &v13[v14];
            v12 = *(v11);
            if (!a1 && v14 < 64 && v12 != 46)
            {
                v1 = 64;
                v0 = a0;
                v0 = a0;
                goto LABEL_474552;
            }
            if (a1 == 1 && v14 < 64 && v12 != 95)
            {
                v1 = 64;
                v0 = &a0->padding_0[64];
                goto LABEL_474551;
            }
            if (a1 != 2)
                return 0xffffffff;
            if (v14 >= 16)
                return 0xffffffff;
            if (v12 && v12 != 44)
                goto LABEL_474599;
            v1 = 16;
            v0 = &a0->padding_0[128];
LABEL_474551:
LABEL_474552:
            if (sub_4830fd(v0, v1, v13, v14))
                sub_470dc6(0, 0, 0, 0, 0);
            if (v12 == 44 || !v12)
                break;
            a1 += 1;
            v13 = v11 + 1;
            v14 = sub_4859c0(v13, "_.,");
            v8 = 15;
        }
    }
    return 0;
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47478b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_474478_0 {
    char padding_0[143];
    char field_8f;
} st_474478_0;

typedef struct st_47478b_0 {
    char padding_0[192];
    unsigned short field_c0;
    char padding_c2[2];
    unsigned int field_c4;
} st_47478b_0;

extern char g_49fe19[2];

int sub_47478b(char *a0, int a1, int a2, unsigned int a3, unsigned int *a4)
{
    char *v8;  // esi
    int v9;  // edi
    st_47478b_0 *v10;  // eax
    st_47478b_0 *v11;  // ebx
    unsigned int v12;  // eax
    int v13;  // eax
    unsigned int v14;  // eax
    unsigned int v15;  // eax
    int v0;  // [bp-0xc0]
    int v1;  // [bp-0xbc]
    int v2;  // [bp-0xb8]
    int v3;  // [bp-0xb4]
    st_47478b_0 *v4;  // [bp-0xb0]
    st_47478b_0 *v5;  // [bp-0xac]
    st_47478b_0 *v6;  // [bp-0x9c]
    st_474478_0 v7;  // [bp-0x98]

    v8 = a0;
    v10 = sub_475467(v9, v0, v1) + 156;
    v4 = &v10->padding_0[40];
    v11 = &v10->padding_0[32];
    v5 = &v10->padding_0[44];
    v6 = &v10->padding_0[175];
    if (!v8 || !a1 || !a2)
        return v15;
    if (*(v8) == 67 && !v8[1])
    {
        if (sub_47a2b9(a1, a2, "C"))
            sub_470dc6(0, 0, 0, 0, 0);
        if (a3)
            memset(a3, 0, 6);
        if (!a4)
            return v12;
        *(a4) = 0;
        return v12;
    }
    v3 = sub_475860(v8);
    if (v3 >= 131 || sub_472ac0(v6, v8) && sub_472ac0(v5, v8))
    {
        v2 = 0;
        if (sub_474478(&v7, v8))
            return v15;
        if (!sub_486001(&v7, v11, &v7))
            return v15;
        *((unsigned int *)&v4->padding_0[0]) = *((short *)&v11->padding_0[4]);
        sub_4745a1(v6, 131, &v7);
        if (!*(v8) || (v13 = v3, v13 >= 131))
        {
            v13 = v2;
            v8 = &g_49fe19[0];
        }
        if (!sub_4830fd(v5, 131, v8, v13 + 1))
            goto LABEL_474942;
        sub_470dc6(0, 0, 0, 0, 0);
    }
    else
    {
LABEL_474942:
    }
    if (a3)
        sub_47a330(a3, v11, 6);
    if (a4)
        sub_47a330(a4, v4, 4);
    if (!sub_47a2b9(a1, a2, v6))
        return v14;
    sub_470dc6(0, 0, 0, 0, 0);
    return v14;
    return v15;
}

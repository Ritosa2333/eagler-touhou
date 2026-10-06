// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48625c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48625c_0 {
    char field_0[4];
    char field_1;
    char field_2;
    char field_3;
} st_48625c_0;

unsigned int sub_48625c(char *a0)
{
    st_48625c_0 *v1;  // esi
    unsigned int v2;  // edx
    unsigned int v11;  // eax
    unsigned int v12;  // ecx
    unsigned int v13;  // 4112
    unsigned int v3;  // eax
    unsigned int v4;  // 4126
    unsigned int v5;  // eax
    unsigned int v6;  // edx
    unsigned int v7;  // 4126
    unsigned int v8;  // eax
    unsigned int v9;  // edx
    unsigned int v10;  // 4126

    if (v1->field_0 == *((int *)&a0))
        return 0;
    v2 = *(a0);
    v3 = *(&v1->field_0[0]);
    if (v3 != v2)
    {
        v4 = _ccall(15, 15, v3 - v2, 0, 0);
        if ((v4 & 1) * 2 != 1)
            return (v4 & 1) * 2 - 1;
    }
    v5 = v1->field_0[1];
    v6 = a0[1];
    if (v5 != v6)
    {
        v7 = _ccall(15, 15, v5 - v6, 0, 0);
        if ((v7 & 1) * 2 != 1)
            return (v7 & 1) * 2 - 1;
    }
    v8 = v1->field_0[2];
    v9 = a0[2];
    if (v8 != v9)
    {
        v10 = _ccall(15, 15, v8 - v9, 0, 0);
        if ((v10 & 1) * 2 != 1)
            return (v10 & 1) * 2 - 1;
    }
    v11 = v1->field_0[3];
    v12 = a0[3];
    if (v11 == v12)
        return v11 - v12;
    v13 = _ccall(15, 15, v11 - v12, 0, 0);
    return (v13 & 1) * 2 - 1;
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48621c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48621c_0 {
    char field_0[2];
    char field_1;
} st_48621c_0;


unsigned int sub_48621c(char *a0)
{
    st_48621c_0 *v1;  // esi
    unsigned int v2;  // edx
    unsigned int v3;  // eax
    unsigned int v4;  // 4126
    unsigned int v5;  // eax
    unsigned int v6;  // ecx
    unsigned int v7;  // 4112

    if (v1->field_0 == *((short *)&a0))
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
    if (v5 == v6)
        return v5 - v6;
    v7 = _ccall(15, 15, v5 - v6, 0, 0);
    return (v7 & 1) * 2 - 1;
}

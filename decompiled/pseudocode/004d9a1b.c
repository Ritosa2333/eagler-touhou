// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d9a1b; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4d9a1b_0 {
    char padding_0[101];
    char field_65;
} st_4d9a1b_0;


int sub_4d9a1b(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned int v12;  // edi
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4123
    unsigned int idx;  // ebp
    st_4d9a1b_0 *v8;  // edx
    void* v9;  // eax
    unsigned int v10;  // eax
    unsigned int v11;  // 4101
    st_4d9a1b_0 *v0;  // [bp-0x4]

    v6 = _ccall(v2, v3, v4, v5);
    *((char *)(idx + 0x73fc0337)) = *((char *)(idx + 0x73fc0337)) - 48 + ((char)v6 & 1);
    v8->field_65 = 0x66;
    *((int *)((char *)v9 - 25)) = *((int *)((char *)v9 - 25)) >> 2;
    v10 = syscall();
    v11 = _ccall(4, v2, v3, v4, v5);
    v0 = v8;
    if ((((char)v10 & 126) - 114 + (v12 + v9 + (v10 < 2130631212) < v9) ^ 97) >= 0)
        goto LABEL_0x4d9a5a;
}

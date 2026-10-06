// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4d927e; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4d927e_0 {
    char padding_0[4];
    unsigned int field_4;
} st_4d927e_0;


int sub_4d927e(void)
{
    void* v3;  // eax
    char v4;  // bh
    unsigned int v13;  // 4109
    unsigned int v14;  // 4101
    st_4d927e_0 *idx;  // esi
    char v5;  // 4177
    unsigned int v6;  // 4238
    unsigned int v7;  // ecx
    unsigned short v8;  // cs
    unsigned int v9;  // cc_op
    unsigned int v10;  // cc_dep1
    unsigned int v11;  // cc_dep2
    unsigned int v12;  // cc_ndep
    char v0[4];  // [bp-0x8]
    unsigned short v1;  // [bp-0x4]

    *((unsigned int *)v3) = *((int *)v3) + 1;
    *((char *)v3) = *((char *)v3) + v4;
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    v5 = *((char *)v3);
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    v6 = _ccall(5, 1, v5, *((char *)&v3), 0);
    if (!(v7 != 1 & v6 & 1))
        goto LABEL_4d92af;
LABEL_4d92af:
    *((char *)v3) = *((char *)v3) + *((char *)&v3);
    v1 = v8;
    syscall();
    strncpy(v0, "is p", 4);
    v13 = _ccall(2, v9, v10, v11, v12);
    if (v13 & 1)
        goto LABEL_0x4d9336;
    v14 = _ccall(2, v9, v10, v11, v12);
    if (!(v14 & 1))
        goto LABEL_0x4d92ca;
    idx->field_4 = idx->field_4 * 2;
}

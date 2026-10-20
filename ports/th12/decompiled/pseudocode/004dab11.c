// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dab11; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.


int sub_4dab11(void)
{
    unsigned int v1;  // edx
    unsigned int v2;  // eax
    unsigned int v11;  // cc_dep2
    unsigned int v12;  // cc_dep1
    unsigned int v13;  // ecx
    char v14;  // cl
    int v15;  // 4096
    int v16;  // 4096
    unsigned int v17;  // 4104
    unsigned int v18;  // 4101
    int *v3;  // ebx
    void* v4;  // eax
    unsigned int v5;  // cc_op
    unsigned int v6;  // cc_dep1
    unsigned int v7;  // cc_dep2
    unsigned int v8;  // cc_ndep
    unsigned int v9;  // 4132
    unsigned int v10;  // cc_op

    v4 = (int)(CONCAT(v1, v2) / *(v3));
    v9 = _ccall(v5, v6, v7, v8);
    v10 = 0;
    v11 = 0;
    v12 = v9 | 1;
    v14 = v13 - 1;
    if (!(v13 != 1 & !((v9 | 1) >> 6 & 1)))
    {
        if (v14 & 31)
        {
            v15 = *((int *)((char *)v4 - 14));
            v10 = 27;
            v12 = (v14 & 31 ? v15 >> (v14 & 31) : v12);
            v11 = (v14 & 31 ? v15 >> ((v14 & 31) - 1 & 31) : 0);
            *((int *)((char *)v4 - 14)) = v15 >> (v14 & 31);
        }
        else
        {
            v16 = *((int *)((char *)v4 - 14));
            v12 = (v14 & 31 ? v16 >> (v14 & 31) : v12);
            v11 = (v14 & 31 ? v16 >> ((v14 & 31) - 1 & 31) : 0);
            *((int *)((char *)v4 - 14)) = v16 >> (v14 & 31);
        }
    }
    v17 = _ccall(10, v10, v12, v11, 0);
    if (!(v17 & 1))
        goto LABEL_0x4dab95;
    v18 = _ccall(2, v10, v12, v11, 0);
    if (v18 & 1)
        goto LABEL_0x4daaf0;
    if (((*((int *)0x18aa3acd) & 2261) >> 6 & 1) != 1)
        goto LABEL_0x4daada;
}

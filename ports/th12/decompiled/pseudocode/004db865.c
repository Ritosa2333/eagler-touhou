// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4db865; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4db865_0 {
    char padding_0[59];
    unsigned int field_3b;
} st_4db865_0;

typedef struct st_4db865_1 {
    char padding_0[111];
    unsigned short field_6f;
} st_4db865_1;


int sub_4db865(void)
{
    unsigned int v2;  // cc_op
    unsigned int v3;  // cc_dep1
    unsigned short v12;  // es
    st_4db865_0 *idx;  // eax
    unsigned int v14;  // 4113
    unsigned int v15;  // ebp
    unsigned short v16;  // ftop
    unsigned int v17;  // fc3210
    unsigned long v18;  // ldt
    unsigned long v19;  // gdt
    unsigned long long v20;  // 4132
    unsigned int v4;  // cc_dep2
    unsigned int v5;  // cc_ndep
    unsigned int v6;  // 4104
    unsigned int v7;  // ecx
    unsigned int v8;  // edx
    unsigned int v9;  // eax
    unsigned int v10;  // 4110
    st_4db865_1 *v11;  // ebp
    char v0;  // [bp+0x1]

    v6 = _ccall(v2, v3, v4, v5);
    if (!v7)
    {
        v9 = _INSERT(v8, 0, *(0x99f934a));
        v10 = _ccall(4, 18, &v0, 0, v6);
        if (!(v10 & 1))
        {
            *((unsigned short *)&(v11->padding_0)[1]) = v12;
            v14 = *((int *)&(idx->padding_0)[1]);
            *((unsigned int *)&(idx->padding_0)[1]) = *((int *)&(idx->padding_0)[1]) - 27;
            if (v14 == 27)
                return;
        }
        else
        {
            *((unsigned short *)(v15 - 1405369832)) = (v16 & 7) * 0x800 | (unsigned short)v17 & 0x4700;
            *((unsigned int *)(v15 - 1716138650)) = *((int *)(v15 - 1716138650)) - v9;
            v20 = _ccall(v18, v19, 8024, 2783401593);
            goto *((void *)((unsigned int)v20));
        }
    }
}

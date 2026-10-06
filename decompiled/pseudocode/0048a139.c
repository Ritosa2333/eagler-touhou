// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48a139; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_46d3b4_0 {
    unsigned int field_0;
    unsigned int field_4;
    void* field_8;
    char field_c;
} st_46d3b4_0;

typedef struct st_48a139_3 {
    char padding_0[112];
    unsigned int field_70;
} st_48a139_3;

int sub_48a139(void)
{
    unsigned int v6;  // esi
    unsigned int v7;  // edx
    char v16;  // 4103
    void* iter;  // esi
    unsigned int v9;  // eax
    unsigned int v10;  // cc_op
    unsigned int v11;  // cc_dep2
    unsigned int v12;  // 4104
    char v13;  // al
    void* v14;  // esi
    void* v15;  // esi
    unsigned int v0;  // [bp-0x18]
    st_46d3b4_0 v1;  // [bp-0x14]
    st_48a139_3 *idx;  // [bp-0xc]
    char v3;  // [bp-0x8]
    void* v4;  // [bp+0x4]
    unsigned int *v5;  // [bp+0x8]

    v0 = v6;
    sub_46d3b4(&v1, v7, v5);
    iter = v4;
    v9 = sub_47ab27(*((char *)iter));
    v10 = 6;
    v11 = 101;
    while (1)
    {
        v12 = _ccall(4, v10, v9, v11, 0);
        if (v12 & 1)
            break;
        iter += 1;
        v9 = sub_48ba71(*((char *)iter));
        v10 = 15;
        v11 = 0;
    }
    if (sub_47ab27(*((char *)iter)) == 120)
        iter += 2;
    v13 = *((char *)iter);
    *((char *)iter) = *((char *)*((int *)*((int *)(v1 + 188))));
    v14 = iter + 1;
    do
    {
        v15 = v14;
        v16 = *((char *)v15);
        *((char *)v15) = v13;
        v14 = v15 + 1;
        v13 = v16;
    } while (*((char *)v15));
    if (v3 != *((char *)v15))
        idx->field_70 = idx->field_70 & 0xfffffffd;
    return;
}

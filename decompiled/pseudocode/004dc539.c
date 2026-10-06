// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4dc539; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4dc539_0 {
    char padding_0[218348];
    char field_354ec;
} st_4dc539_0;


int sub_4dc539(void)
{
    unsigned int v4;  // cc_op
    unsigned int v5;  // cc_dep1
    unsigned int v6;  // cc_dep2
    unsigned int v7;  // cc_ndep
    unsigned int v8;  // 4114
    st_4dc539_0 *idx;  // ecx
    char v10;  // al
    unsigned int index;  // ebp
    unsigned int v0;  // [bp-0x8]
    unsigned int v2;  // [bp+0x358]
    int v3;  // [bp+0x35c]

    __unsupported_jumpkind_Ijk_NoDecode()
    v8 = _ccall(v4, v5, v6, v7);
    idx->field_354ec = idx->field_354ec + v10 + ((char)v8 & 1);
    *((char *)(index + 1342448716)) = *((char *)(index + 1342448716)) + *((char *)&idx);
    sub_4dc8f5();
    v0 = v2;
    if ((char)sub_4dc973(v2, v3))
        return;
    return;
}

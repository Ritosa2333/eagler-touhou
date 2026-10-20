// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48e03c; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_48e03c_0 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    int field_c;
} st_48e03c_0;

void sub_48e03c(char a0, unsigned int a0, st_48e03c_0 *idx, int a2, int a3)
{
    int v14;  // edi
    unsigned int v0;  // [bp-0x54]
    unsigned short v1;  // [bp-0x50]
    unsigned int v2;  // [bp-0x4c]
    unsigned int v3;  // [bp-0x48]
    char *v4;  // [bp-0x44]
    int v5;  // [bp-0x3c]
    int v6;  // [bp-0x38]
    short v7;  // [bp-0x30]
    char v8;  // [bp-0x2e]
    char v9;  // [bp-0x2c]
    char v10;  // [bp-0x14], Other Possible Types: int
    unsigned int v11;  // [bp-0x10]
    unsigned short v12;  // [bp-0xc]

    sub_48df7f(&v10, &/* unsupported instruction */, v14, v5, v6, a2);
    v4 = &v7;
    v3 = 0;
    v2 = 0x11;
    v0 = v11;
    v1 = v12;
    idx->field_8 = sub_48ee11(v10);
    idx->field_0 = v8;
    idx->field_4 = v7;
    if (sub_47a2b9(a2, a3, &v9))
        sub_470dc6(0, 0, 0, 0, 0);
    idx->field_c = a2;
    return;
}

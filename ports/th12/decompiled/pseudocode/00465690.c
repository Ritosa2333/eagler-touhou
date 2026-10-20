// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x465690; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_465690_0 {
    char padding_0[12];
    struct st_465690_1 *field_c;
} st_465690_0;

typedef struct st_465690_2 {
    struct st_465690_0 *field_0;
} st_465690_2;

typedef struct st_465690_1 {
    unsigned int field_0;
} st_465690_1;

typedef struct st_465690_3 {
    char padding_0[8];
    struct st_465690_1 *field_8;
    char padding_c[44];
    struct st_465690_1 *field_38;
} st_465690_3;


int sub_465690(st_465690_2 **a0)
{
    st_465690_0 **v17;  // ecx
    unsigned int v18;  // eax
    unsigned int v19;  // eax
    unsigned int v20;  // eax
    unsigned int v21;  // eax
    st_465690_3 **v0;  // [bp-0x40]
    unsigned int v1;  // [bp-0x3c], Other Possible Types: unsigned short
    unsigned short v2;  // [bp-0x3a]
    unsigned int v3;  // [bp-0x38]
    unsigned int v4;  // [bp-0x34]
    unsigned int v5;  // [bp-0x30], Other Possible Types: unsigned short
    unsigned short v6;  // [bp-0x2e]
    unsigned short v7;  // [bp-0x2c]
    unsigned int v8;  // [bp-0x28]
    unsigned int v9;  // [bp-0x24]
    unsigned int v10;  // [bp-0x20]
    unsigned int v11;  // [bp-0x1c]
    unsigned int v12;  // [bp-0x18]
    unsigned int v13;  // [bp-0x14]
    unsigned int v14;  // [bp-0x10]
    unsigned int v15;  // [bp-0xc]
    unsigned int v16;  // [bp-0x8]

    v17 = *(a0);
    v0 = NULL;
    if (!v17)
        return v18;
    v8 = 0;
    v9 = 0;
    v10 = 0;
    v12 = 0;
    v11 = 0;
    v13 = 0;
    v14 = 0;
    v15 = 0;
    v16 = 0;
    v10 = 0;
    v12 = 0;
    v8 = 36;
    v9 = 1;
    if (*(v17)->field_c(v17, &v8, &v0, 0) < NULL)
        return v21;
    v1 = 0;
    v5 = 0;
    v3 = 0;
    v4 = 0;
    v7 = 0;
    v1 = 1;
    v6 = 16;
    v5 = 4;
    v2 = 2;
    v3 = 44100;
    v4 = 176400;
    if (*(v0)->field_38(v0, &v1) < NULL)
        return v20;
    if (!v0)
        return v19;
    *(v0)->field_8(v0);
    return v19;
}

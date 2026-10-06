// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x461f74; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_461f74_1 {
    unsigned int field_0;
} st_461f74_1;

typedef struct st_461f74_2 {
    char padding_0[8];
    struct st_461f74_1 *field_8;
    char padding_c[36];
    struct st_461f74_1 *field_30;
    struct st_461f74_1 *field_34;
    struct st_461f74_1 *field_38;
} st_461f74_2;

typedef struct st_461f74_0 {
    char padding_0[72];
    struct st_461f74_1 *field_48;
} st_461f74_0;

int sub_461f74(void)
{
    unsigned int v6;  // eax
    st_461f74_0 *v7;  // ecx
    st_461f74_2 **v0;  // [bp-0x2c], Other Possible Types: char
    char v1;  // [bp-0x28], Other Possible Types: int
    int v2;  // [bp-0x24]
    char v3;  // [bp-0x20]
    unsigned int v4;  // [bp-0x4]

    v7->field_48(v6, 0, &v0);
    *(v0)->field_30(v0, &v3);
    *(v0)->field_34(v0, &v1, 0, 0);
    sub_477420(v2, 0, v1 * v4);
    *(v0)->field_38(v0);
    if (v0)
        *(v0)->field_8(v0);
    return;
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4690b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4690b0_1 {
    char padding_0[16];
    struct st_4690b0_2 *field_10;
} st_4690b0_1;

typedef struct st_4690b0_3 {
    char padding_0[4];
    struct st_4690b0_0 *field_4;
    char padding_8[4100];
    unsigned int field_100c;
    char padding_1010[4];
    struct st_4690b0_4 *field_1014;
} st_4690b0_3;

typedef struct st_4690b0_4 {
    struct st_4690b0_1 *field_0;
} st_4690b0_4;

typedef struct st_4690b0_2 {
    unsigned int field_0;
} st_4690b0_2;

typedef struct st_4690b0_0 {
    char padding_0[8];
    unsigned short field_8;
} st_4690b0_0;

unsigned int sub_4690b0(unsigned int a0, st_4690b0_3 *a1)
{
    unsigned short v2;  // ftop
    unsigned int v3;  // 4160
    unsigned int v4;  // eax
    unsigned int v5;  // eax

    if (!(a1->field_4->field_8 & 1 << ((char)a0 & 31)))
        return 0;
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v3 = _ccall(10, 13, (char)(((v2 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), *((int *)&a1->field_4[1].padding_0[6 + 4 * a0])) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    if (v3 & 1)
    {
        v5 = sub_4931e0();
        return a1->field_1014->field_0->field_10(v5);
    }
    v4 = sub_4931e0();
    return &a1->padding_8[v4 + a1->field_100c];
}

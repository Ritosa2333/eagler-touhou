// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43e240; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43dec0_3 {
    struct st_43dec0_0 *field_0;
    char padding_4[12];
    struct st_43dec0_2 *field_10;
    char padding_14[92];
    unsigned int field_70;
    char padding_74[32];
    unsigned int field_94;
    unsigned int field_98;
    unsigned int field_9c;
    unsigned int field_a0;
    unsigned int field_a4;
    unsigned int field_a8;
    unsigned int field_ac;
    unsigned int field_b0;
    char padding_b4[800];
    char field_3d4[4];
    char field_3d7;
    char padding_3d8[52];
    unsigned int field_40c;
    char padding_410[56];
    unsigned int field_448;
    unsigned int field_44c;
    char padding_450[68];
    unsigned int field_494;
    char padding_498[76];
    unsigned int field_4e4;
    char padding_4e8[4];
    int field_4ec;
    char padding_4f0[20];
    char field_504;
    char field_505;
} st_43dec0_3;

typedef struct st_43dec0_0 {
    char padding_0[228];
    struct st_43dec0_1 *field_e4;
} st_43dec0_0;

typedef struct st_43dec0_2 {
    char padding_0[280];
    unsigned int field_118;
} st_43dec0_2;

typedef struct st_43dec0_1 {
    unsigned int field_0;
} st_43dec0_1;

unsigned int sub_43e240(st_43dec0_3 *a0)
{
    return sub_43dec0(a0);
}

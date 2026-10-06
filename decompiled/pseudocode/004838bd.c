// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4838bd; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4838bd_0 {
    char padding_0[172];
    unsigned int field_ac;
} st_4838bd_0;


int sub_4838bd(void)
{
    unsigned short *v4;  // eax
    st_4838bd_0 *v5;  // esi
    int v6;  // edi
    int v7;  // ebx
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    v2 = v4[33];
    v1 = v4[0x22];
    if (!v5)
        return;
    v0 = 0;
    sub_47a12b(&v4, 1, v2, 49, &v5->padding_0[4], v6, v7);
    sub_47a12b(&v4, 1, v2, 50, &v5->padding_0[8]);
    sub_47a12b(&v4, 1, v2, 0x33, &v5->padding_0[12]);
    sub_47a12b(&v4, 1, v2, 52, &v5->padding_0[16]);
    sub_47a12b(&v4, 1, v2, 53, &v5->padding_0[20]);
    sub_47a12b(&v4, 1, v2, 54, &v5->padding_0[24]);
    sub_47a12b(&v4, 1, v2, 55, v5);
    sub_47a12b(&v4, 1, v2, 42, &v5->padding_0[32]);
    sub_47a12b(&v4, 1, v2, 43, &v5->padding_0[36]);
    sub_47a12b(&v4, 1, v2, 44, &v5->padding_0[40]);
    sub_47a12b(&v4, 1, v2, 45, &v5->padding_0[44]);
    sub_47a12b(&v4, 1, v2, 46, &v5->padding_0[48]);
    sub_47a12b(&v4, 1, v2, 47, &v5->padding_0[52]);
    sub_47a12b(&v4, 1, v2, 48, &v5->padding_0[28]);
    sub_47a12b(&v4, 1, v2, 0x44, &v5->padding_0[56]);
    sub_47a12b(&v4, 1, v2, 69, &v5->padding_0[60]);
    sub_47a12b(&v4, 1, v2, 70, &v5->padding_0[64]);
    sub_47a12b(&v4, 1, v2, 71, &v5->padding_0[0x44]);
    sub_47a12b(&v4, 1, v2, 72, &v5->padding_0[72]);
    sub_47a12b(&v4, 1, v2, 73, &v5->padding_0[76]);
    sub_47a12b(&v4, 1, v2, 74, &v5->padding_0[80]);
    sub_47a12b(&v4, 1, v2, 75, &v5->padding_0[84]);
    sub_47a12b(&v4, 1, v2, 76, &v5->padding_0[88]);
    sub_47a12b(&v4, 1, v2, 77, &v5->padding_0[92]);
    sub_47a12b(&v4, 1, v2, 78, &v5->padding_0[96]);
    sub_47a12b(&v4, 1, v2, 79, &v5->padding_0[100]);
    sub_47a12b(&v4, 1, v2, 56, &v5->padding_0[104]);
    sub_47a12b(&v4, 1, v2, 57, &v5->padding_0[108]);
    sub_47a12b(&v4, 1, v2, 58, &v5->padding_0[112]);
    sub_47a12b(&v4, 1, v2, 59, &v5->padding_0[116]);
    sub_47a12b(&v4, 1, v2, 60, &v5->padding_0[120]);
    sub_47a12b(&v4, 1, v2, 61, &v5->padding_0[124]);
    sub_47a12b(&v4, 1, v2, 62, &v5->padding_0[128]);
    sub_47a12b(&v4, 1, v2, 63, &v5->padding_0[132]);
    sub_47a12b(&v4, 1, v2, 64, &v5->padding_0[0x88]);
    sub_47a12b(&v4, 1, v2, 65, &v5->padding_0[140]);
    sub_47a12b(&v4, 1, v2, 66, &v5->padding_0[144]);
    sub_47a12b(&v4, 1, v2, 67, &v5->padding_0[148]);
    sub_47a12b(&v4, 1, v2, 40, &v5->padding_0[152]);
    sub_47a12b(&v4, 1, v2, 41, &v5->padding_0[156]);
    sub_47a12b(&v4, 1, v1, 31, &v5->padding_0[160]);
    sub_47a12b(&v4, 1, v1, 32, &v5->padding_0[164]);
    sub_47a12b(&v4, 1, v1, 4099, &v5->padding_0[168]);
    sub_47a12b(&v4, 0, v1, 4105, v5 + 1);
    v5->field_ac = v1;
    return;
}

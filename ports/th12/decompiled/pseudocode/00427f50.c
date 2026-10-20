// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x427f50; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_427f50_1 {
    char padding_0[1124];
    struct st_427f50_0 *field_464;
    unsigned int field_468;
} st_427f50_1;

typedef struct st_427f50_0 {
    char padding_0[4];
    struct st_427f50_0 *field_4;
    struct st_427f50_0 *field_8;
} st_427f50_0;

int sub_427f50(void)
{
    st_427f50_1 *idx;  // eax
    st_427f50_0 *v2;  // edx
    st_427f50_0 *v3;  // ecx

    v2 = idx->field_464;
    v3->field_4 = v2;
    v2->field_8 = v3;
    idx->field_468 = idx->field_468 + 1;
    idx->field_464 = v3;
    return;
}

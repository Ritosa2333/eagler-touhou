// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x435900; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_435900_0 {
    char padding_0[33552];
    int field_8310;
    int field_8314;
    char padding_8318[16636];
    unsigned int field_c414;
    unsigned int field_c418;
} st_435900_0;

void sub_435900(st_435900_0 *idx)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // esi
    unsigned int v5;  // edi
    st_435900_0 *v6;  // esi
    unsigned int v7;  // edi
    unsigned int i;  // edi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v2 = v3;
    idx->field_c414 = idx->field_c414 & 0xfffffff7;
    v1 = v4;
    v0 = v5;
    v6 = &idx->field_8314;
    v7 = 8;
    do
    {
        i = v7;
        sub_461970(*((int *)(&v6->padding_0[0] - 4)));
        sub_461970(*((int *)&v6->padding_0[0]));
        v6 = &v6->padding_0[228];
        v7 = i - 1;
    } while (i != 1);
    idx->field_c418 = v7;
    return;
}

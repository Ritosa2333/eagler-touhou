// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408580; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408580_0 {
    char padding_0[60];
    unsigned int field_3c;
    char padding_40[8];
    int field_48;
    char padding_4c[1204];
    int field_500;
} st_408580_0;

unsigned int sub_408580(unsigned int a0, unsigned int a1, st_408580_0 *idx)
{
    unsigned int v3;  // ebx
    unsigned int v4;  // esi
    unsigned int v5;  // edi
    unsigned int v6;  // esi
    unsigned int i;  // esi
    unsigned int *index;  // esi
    unsigned int v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x4]

    v2 = v3;
    v1 = v4;
    v0 = v5;
    v6 = 8;
    do
    {
        i = v6;
        sub_408030(a0);
        v6 = i - 1;
    } while (i != 1);
    sub_461970(idx->field_48);
    if (idx->field_500)
    {
        sub_46c941(idx->field_500);
        idx->field_500 = 0;
    }
    index = sub_46c9ea(0x44);
    if (index)
    {
        index[16] = index[16] & 0xfffffffe;
        sub_477420(index, 0, 0x44);
        *(index) = *(index) | 2;
        sub_452a60(index, 1, 8, 6, 6, 0, 70);
    }
    idx->field_3c = 0;
    return 0;
}

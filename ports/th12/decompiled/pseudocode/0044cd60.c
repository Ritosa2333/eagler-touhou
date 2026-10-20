// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44cd60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44cd60_0 {
    char padding_0[12];
    int field_c;
} st_44cd60_0;

int sub_44cd60(st_44cd60_0 *idx)
{
    int v1;  // eax

    v1 = idx->field_c;
    if (v1)
    {
        v1 = sub_46c941(v1);
        idx->field_c = 0;
    }
    memset(&idx->padding_0[4], 0, 12);
    return v1;
}

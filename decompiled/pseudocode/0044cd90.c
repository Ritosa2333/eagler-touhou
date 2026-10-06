// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44cd90; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44cd90_0 {
    char padding_0[4];
    unsigned int field_4;
    int field_8;
    unsigned int field_c;
} st_44cd90_0;

int sub_44cd90(st_44cd90_0 *idx, unsigned int a1, int a2, int a3)
{
    int v0;  // eax
    int v1;  // esi

    v0 = idx->field_8;
    v1 = idx->field_c + idx->field_4 - v0;
    if (v1 >= a3)
    {
        sub_47a330(a2, v0, a3);
        idx->field_8 = idx->field_8 + a3;
        return a3;
    }
    else if (v1)
    {
        sub_47a330(a2, v0, v1);
        idx->field_8 = idx->field_8 + v1;
        return v1;
    }
    else
    {
        return 0;
    }
}

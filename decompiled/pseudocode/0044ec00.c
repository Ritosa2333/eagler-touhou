// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44ec00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44ec00_0 {
    char padding_0[4];
    int field_4;
    char padding_8[12];
    int field_14;
    unsigned int field_18;
} st_44ec00_0;

void sub_44ec00(st_44ec00_0 *idx, unsigned int a1, char a2, int a3)
{
    st_44ec00_0 *v1;  // eax
    unsigned int v2;  // ebx
    unsigned int v0;  // [bp-0xc]

    if (a2 && idx->field_18 >= 16)
    {
        v1 = &idx->field_4;
        v0 = v2;
        if (a3)
            sub_46e210(v1, 16, v1->padding_0, a3);
        sub_46ca4f(v1->padding_0);
    }
    idx->field_14 = a3;
    idx->field_18 = 15;
    *(&idx->padding_0[a3 + 4]) = 0;
    return;
}

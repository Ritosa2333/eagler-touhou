// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d521; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44d521_0 {
    HFONT field_0;
    int field_4;
    int field_8;
    char padding_c[28];
    int field_28;
} st_44d521_0;

int sub_44d521(void)
{
    st_44d521_0 *v1;  // esi
    PSTR v2;  // eax

    v1->field_0 = CreateFontA(v1->field_8, v1->field_4, 0, 0, v1->field_28, 0, 0, 0, 1, 0, 0, 0, 48, v2);
    return;
}

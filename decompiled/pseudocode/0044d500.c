// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44d500; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_44d500_0 {
    HGDIOBJ field_0;
    char padding_4[32];
    unsigned int field_24;
} st_44d500_0;

int sub_44d500(void)
{
    st_44d500_0 *v1;  // esi

    if (v1->field_0)
    {
        DeleteObject(v1->field_0);
        v1->field_0 = NULL;
    }
    return sub_44d521();
}

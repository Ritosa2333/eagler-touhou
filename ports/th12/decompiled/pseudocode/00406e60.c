// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x406e60; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_406e60_0 {
    char padding_0[60];
    unsigned int field_3c;
} st_406e60_0;

extern unsigned int g_4b0c90;
extern unsigned int g_4b0c94;

int sub_406e60(void)
{
    st_406e60_0 *v2;  // eax
    unsigned int v3;  // ebx
    unsigned int v0;  // [bp-0x4]

    if (!v2->field_3c)
        return;
    switch (g_4b0c94 + g_4b0c90 * 2)
    {
    case 0:
        sub_46abe0();
        return;
    case 1:
        sub_408510();
        return;
    case 2:
        v0 = v3;
        sub_407580();
        return;
    case 3:
        sub_407a30();
        return;
    case 4:
        sub_408c30();
        return;
    case 5:
        sub_409220();
        return;
    default:
        return;
    }
}

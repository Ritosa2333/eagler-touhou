// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x408c10; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_408c10_0 {
    char padding_0[4];
    unsigned int field_4;
} st_408c10_0;

int sub_408c10(void)
{
    st_408c10_0 *v1;  // eax
    unsigned short v3;  // ftop
    unsigned int v4;  // 4150

    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    v4 = _ccall(10, 13, (char)(((v3 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), v1->field_4) * 0x100 & 0x4500 & 0x4700) >> 8) & 65, 0, 0);
    return;
}

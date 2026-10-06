// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4271c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4271c0_0 {
    char padding_0[20];
    int field_14;
    char padding_18[24];
    unsigned int field_30;
} st_4271c0_0;

extern st_4271c0_0 *g_4b4534;

int sub_4271c0(void)
{
    unsigned int *idx;  // eax

    if (g_4b4534->field_14 < 60)
        goto LABEL_427210;
    switch (g_4b4534->field_30)
    {
    case -1: case 1: case 2: case 3:
        if (idx[621] == 1 || idx[621] == 2)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            idx[620] = 7;
            idx[606] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            idx[607] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            idx[623] = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
            /* unsupported instruction */
            return;
        }
    default:
LABEL_427210:
        return;
    }
}

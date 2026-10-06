// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4057e0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4057e0_0 {
    char padding_0[13756];
    unsigned int field_35bc;
    unsigned int field_35c0;
    unsigned int field_35c4;
    unsigned int field_35c8;
    char padding_35cc[4];
    unsigned int field_35d0;
} st_4057e0_0;

extern char g_4b2ed0;
extern st_4057e0_0 *g_4b43bc;

void sub_4057e0(void)
{
    int v1;  // edi
    int v2;  // esi
    unsigned int *idx;  // esi
    unsigned int v4;  // eax

    idx = sub_46c9ea(0x44, v1, v2);
    if (idx)
    {
        idx[16] = idx[16] & 0xfffffffe;
        sub_477420(idx, 0, 0x44);
        *(idx) = *(idx) | 2;
        sub_452a60(idx, 2, 30, 0, 0, 0, 11);
    }
    v4 = g_4b43bc->field_35d0;
    if (!((char)g_4b43bc->field_35d0 & 1))
    {
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b43bc->field_35c8 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        g_4b43bc->field_35c4 = 0;
        g_4b43bc->field_35c0 = 0xfff0bdc1;
        *((char **)&g_4b43bc->padding_35cc[0]) = &g_4b2ed0;
        g_4b43bc->field_35d0 = v4 | 1;
    }
    if (/* unsupported instruction */)
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    else
    {
        /* unsupported instruction */
        /* unsupported instruction */
    }
    g_4b43bc->field_35c4 = 30;
    if (/* unsupported instruction */)
    {
        g_4b43bc->field_35c8 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        g_4b43bc->field_35c8 = nan;
        /* unsupported instruction */
    }
    g_4b43bc->field_35c0 = 29;
    g_4b43bc->field_35bc = g_4b43bc->field_35bc | 2;
    return;
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4544b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4544b0_6 {
    char padding_0[8];
    unsigned short field_8;
    char padding_a[2];
    unsigned int field_c;
} st_4544b0_6;

typedef struct st_4544b0_7 {
    struct st_4544b0_8 *field_0;
    char padding_4[4];
    struct st_4544b0_6 *field_8;
    char padding_c[4];
    unsigned int field_10;
} st_4544b0_7;

typedef struct st_4544b0_8 {
    struct st_4544b0_0 *field_0;
} st_4544b0_8;

typedef struct st_4544b0_1 {
    unsigned int field_0;
} st_4544b0_1;

typedef struct st_4544b0_0 {
    char padding_0[48];
    struct st_4544b0_1 *field_30;
    struct st_4544b0_1 *field_34;
    char padding_38[4];
    struct st_4544b0_1 *field_3c;
    struct st_4544b0_1 *field_40;
    char padding_44[4];
    struct st_4544b0_1 *field_48;
} st_4544b0_0;

extern unsigned int g_4d4780;

int sub_4544b0(void)
{
    st_4544b0_7 *v3;  // esi
    st_4544b0_0 **v4;  // eax
    unsigned int v5;  // eax
    unsigned int v6;  // ebx
    unsigned int v7;  // eax
    unsigned int v0;  // [bp-0xc]
    unsigned int v1;  // [bp-0x4]

    v4 = v3->field_0;
    if (!v4)
        return;
    *(v4)->field_48(v4);
    v3->field_0->field_0->field_34(v3->field_0, 0);
    v3->field_0->field_0->field_40(v3->field_0, v5);
    v3->field_10 = v5;
    if (g_4d4780)
    {
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
        v0 = v6;
        if (/* unsupported instruction */)
        {
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v1 = nan;
            /* unsupported instruction */
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
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v1 = v3->field_8->field_8 + 5000;
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = sub_4931e0();
        v3->field_0->field_0->field_3c(v3->field_0, v7 - 5000);
    }
    else
    {
        v3->field_0->field_0->field_3c(v3->field_0, 0xffffd8f0);
    }
    v3->field_0->field_0->field_30(v3->field_0, 0, 0, v3->field_8->field_c);
    return;
}

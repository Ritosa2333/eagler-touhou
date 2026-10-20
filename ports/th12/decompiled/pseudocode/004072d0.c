// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4072d0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4072d0_0 {
    char padding_0[24];
    int field_18;
    char padding_1c[36];
    int field_40;
    char padding_44[4];
    int field_48;
    char padding_4c[1212];
    unsigned int field_508;
    unsigned int field_50c;
    unsigned int field_510;
} st_4072d0_0;

typedef struct st_4072d0_1 {
    char padding_0[2428];
    unsigned int field_97c;
    unsigned int field_980;
    unsigned int field_984;
    char padding_988[30932];
    unsigned int field_825c;
} st_4072d0_1;

extern st_4072d0_1 *g_4b4514;

unsigned int sub_4072d0(st_4072d0_0 *a0)
{
    st_4072d0_0 *v0;  // edi
    int v1;  // edi
    int v2;  // esi
    int v3;  // ebp
    int v4;  // ebx
    unsigned int v5;  // esi
    unsigned int v6;  // edx
    st_4072d0_0 *idx;  // esi

    v0 = &a0->field_40;
    v5 = sub_461920(a0->field_40, v1, v2, v3, v4);
    if (!v5)
        *((unsigned int *)&v0->padding_0[0]) = v5;
    sub_406f60(40);
    if (!v5)
    {
        sub_461970(a0->field_48);
        return 0xffffffff;
    }
    else if (a0->field_18 > 300)
    {
        sub_461970(*((int *)&v0->padding_0[0]));
        sub_461970(a0->field_48);
        /* unsupported instruction */
        /* unsupported instruction */
        g_4b4514->field_825c = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        return 0xffffffff;
    }
    else
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
        v6 = g_4b4514->field_97c;
        if (/* unsupported instruction */)
        {
            g_4b4514->field_825c = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            g_4b4514->field_825c = nan;
            /* unsupported instruction */
        }
        idx = &a0->field_508;
        *((unsigned int *)&idx->padding_0[0]) = v6;
        *((unsigned int *)&idx->padding_0[4]) = g_4b4514->field_980;
        *((unsigned int *)&idx->padding_0[8]) = g_4b4514->field_984;
        sub_461e30();
        sub_407580();
        return 0;
    }
}

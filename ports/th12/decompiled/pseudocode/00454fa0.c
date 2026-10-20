// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x454fa0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_454fa0_0 {
    unsigned short field_0;
    char padding_2[282];
    unsigned int field_11c;
    char padding_120[4];
    unsigned int field_124;
} st_454fa0_0;

typedef struct st_454fa0_2 {
    char padding_0[160];
    unsigned int field_a0;
} st_454fa0_2;

extern char g_4b2ed0;
extern st_454fa0_2 *g_4ce8cc;

int sub_454fa0(void)
{
    unsigned int v4;  // esi
    unsigned int v5;  // edi
    st_454fa0_0 *v6;  // edx
    unsigned int index;  // ecx
    unsigned int v8;  // esi
    void* idx;  // eax
    unsigned int v10;  // edi
    unsigned short v11;  // ebp
    unsigned int v12;  // edx
    void* v0;  // [bp-0x10]
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]

    v2 = v4;
    v1 = v5;
    v8 = *((int *)(v6->field_11c + index * 4));
    if (!v8)
    {
        return;
    }
    else if (!v6->field_124)
    {
        *((unsigned short *)&idx[1002]) = index;
        v10 = (int)idx[1148];
        *((st_454fa0_0 **)&idx[1016]) = v6;
        if (v10 & 0x400)
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
            *((unsigned int *)&idx[1148]) = (v10 | 8) ^ 0x400;
            if (/* unsupported instruction */)
            {
                *((int *)&idx[64]) = /* unsupported instruction */;
                /* unsupported instruction */
            }
            else
            {
                *((unsigned int *)&idx[64]) = nan;
                /* unsupported instruction */
            }
        }
        /* unsupported instruction */
        /* unsupported instruction */
        *((unsigned short *)&idx[1148]) = 7;
        *((unsigned int *)&idx[956]) = 0xffffffff;
        *((int *)&idx[112]) = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        *((unsigned int *)&idx[108]) = 0;
        *((unsigned int *)&idx[104]) = 0xfff0bdc1;
        *((unsigned int *)&idx[224]) = 0;
        *((unsigned int *)&idx[300]) = 0;
        *((unsigned int *)&idx[344]) = 0;
        *((unsigned int *)&idx[420]) = 0;
        *((unsigned int *)&idx[480]) = 0;
        *((unsigned int *)&idx[540]) = 0;
        *((unsigned int *)&idx[616]) = 0;
        *((unsigned int *)&idx[660]) = 0;
        v11 = v6->field_0;
        *((unsigned int *)&idx[1148]) = (int)idx[1148] & 0xfffff3ff;
        *((unsigned short *)&idx[998]) = v11;
        *((st_454fa0_0 **)&idx[1016]) = v6;
        *((unsigned int *)&idx[1004]) = v8;
        *((unsigned int *)&idx[1008]) = v8;
        v12 = (int)idx[120];
        if (!((char)(int)idx[120] & 1))
        {
            if (/* unsupported instruction */)
                *((int *)&idx[112]) = /* unsupported instruction */;
            else
                *((unsigned int *)&idx[112]) = nan;
            *((unsigned int *)&idx[108]) = 0;
            *((unsigned int *)&idx[104]) = 0xfff0bdc1;
            *((char **)&idx[116]) = &g_4b2ed0;
            *((unsigned int *)&idx[120]) = v12 | 1;
        }
        if (/* unsupported instruction */)
        {
            *((int *)&idx[112]) = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            *((unsigned int *)&idx[112]) = nan;
            /* unsupported instruction */
        }
        *((unsigned int *)&idx[108]) = 0;
        *((unsigned int *)&idx[104]) = 0xffffffff;
        *((unsigned int *)&idx[1148]) = (int)idx[1148] & 0xfffffffe;
        v0 = idx;
        sub_455630();
        g_4ce8cc->field_a0 = g_4ce8cc->field_a0 + 1;
        return;
    }
    else
    {
        return;
    }
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x450720; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_450720_0 {
    char padding_0[64];
    unsigned long long field_40;
} st_450720_0;

typedef struct st_450720_2 {
    unsigned int field_0;
} st_450720_2;

typedef struct st_450720_1 {
    char padding_0[64];
    struct st_450720_2 *field_40;
    struct st_450720_2 *field_44;
} st_450720_1;

typedef struct st_450720_4 {
    struct st_450720_1 *field_0;
} st_450720_4;

extern unsigned int g_4b43cc;
extern unsigned int g_4b43e0;
extern st_450720_4 *g_4ce8f0;
extern char g_4ce9dc;
extern char g_4cead3;
extern unsigned int g_4cee5c;

void sub_450720(void)
{
    st_450720_0 *idx;  // esi
    unsigned short v3;  // ftop
    unsigned short v4;  // ftop
    unsigned int v7;  // 4158
    unsigned int v8;  // eax
    int v0;  // [bp-0x4]

    sub_4508b0(v0);
    if (/* unsupported instruction */)
        idx->field_40 = /* unsupported instruction */;
    else
        idx->field_40 = nan;
    if (g_4cead3 == 1)
    {
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v4 = v3 - 2;
        /* unsupported instruction */
        /* unsupported instruction */
        if (!((char)(((v4 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 65))
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            v7 = _ccall(10, 13, (char)(((v4 & 7) * 0x800 | CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan)) * 0x100 & 0x4500 & 0x4700) >> 8) & 5, 0, 0);
            if (v7 & 1)
                goto LABEL_450778;
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
            v8 = sub_4931e0();
            if (v8 > 0)
            {
                Sleep(v8);
                goto LABEL_45077c;
            }
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
LABEL_450778:
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
        }
    }
    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
LABEL_45077c:
    sub_4508b0();
    if (/* unsupported instruction */)
    {
        *((long long *)&idx[1].padding_0[32]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned long long *)&idx[1].padding_0[32]) = nan;
        /* unsupported instruction */
    }
    sub_450810();
    if (g_4ce8f0->field_0->field_44(g_4ce8f0, 0, 0, 0, 0) < 0)
    {
        sub_431700();
        sub_44f370();
        g_4ce8f0->field_0->field_40(g_4ce8f0, &g_4ce9dc);
        sub_44f400();
        sub_431630();
        sub_451200();
        g_4cee5c = 2;
    }
    sub_4508b0();
    if (/* unsupported instruction */)
    {
        *((long long *)&idx[1].padding_0[24]) = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        *((unsigned long long *)&idx[1].padding_0[24]) = nan;
        /* unsupported instruction */
    }
    if (g_4b43e0)
        sub_41cb70();
    if (g_4b43cc)
        sub_40dd50();
    return;
}

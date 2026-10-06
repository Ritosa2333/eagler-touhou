// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x40f350; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_40f350_1 {
    char padding_0[140];
    unsigned int field_8c;
} st_40f350_1;

typedef struct st_40f350_0 {
    char padding_0[48];
    unsigned int field_30;
    unsigned int field_34;
    unsigned int field_38;
    char padding_3c[216];
    unsigned int field_114;
    char padding_118[212];
    unsigned int field_1ec;
    char padding_1f0[1276];
    unsigned int field_6ec;
    unsigned int field_6f0;
    unsigned int field_6f4;
    char padding_6f8[148];
    char field_78c;
} st_40f350_0;

typedef struct st_40f350_2 {
    char padding_0[100];
    struct st_40f350_1 *field_64;
    char padding_68[8];
    unsigned int field_70;
} st_40f350_2;

extern unsigned int g_4b43b8;
extern st_40f350_2 *g_4b43dc;
extern unsigned int g_4ce8cc;

unsigned int sub_40f350(unsigned int a0)
{
    int v4;  // esi
    int v5;  // ebx
    st_40f350_0 *idx;  // edi
    unsigned int v7;  // edx
    unsigned int v8;  // eax
    int v0;  // [bp-0x1c], Other Possible Types: unsigned int
    unsigned int v1;  // [bp-0xc]
    unsigned int v2;  // [bp-0x8]
    unsigned int v3;  // [bp-0x4]

    /* unsupported instruction */
    /* unsupported instruction */
    /* unsupported instruction */
    sub_401720("SprtView\n", v4, v5, (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan), (/* unsupported instruction */ ? /* unsupported instruction */ : nan));
    switch (idx->field_30)
    {
    case 1:
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
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        if (idx->field_38)
            sub_401720("File %s", *((int *)(idx->field_34 + idx->field_114 * 4)));
        else
            sub_401720("File not found.");
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
        v7 = idx->field_1ec;
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        if (g_4b43dc)
        {
            v7 = *((int *)(g_4b43dc->field_64->field_8c + idx->field_1ec * 8));
            v0 = "Ecl %s";
        }
        else
        {
            v7 = idx->field_1ec;
            v0 = "Ecl %d";
        }
        sub_401720(v0, v7);
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
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        v7 = "Quit";
        sub_401720();
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
        v0 = ">";
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
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        sub_401720();
        *((unsigned int *)(g_4b43b8 + 102272)) = 0xffffffff;
        if (!(idx->field_78c & 2))
            return 1;
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
        idx->field_6ec = v1;
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_6f0 = v2;
        idx->field_6f4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v7 = g_4ce8cc;
        sub_45a9f0(v7);
        return 1;
    case 2:
        if (/* unsupported instruction */)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            break;
        }
        else
        {
            /* unsupported instruction */
            /* unsupported instruction */
            break;
        }
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
        *((unsigned int *)(g_4b43b8 + 102272)) = 0xffff4040;
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
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v7 = *((int *)(idx->field_34 + idx->field_114 * 4));
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        v0 = "Loading %s";
        sub_401720();
        *((unsigned int *)(g_4b43b8 + 102272)) = 0xffffffff;
        return 1;
    case 3:
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
            v1 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v1 = nan;
            /* unsupported instruction */
        }
        *((unsigned int *)(g_4b43b8 + 102272)) = 0xffa0a0a0;
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
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        v8 = sub_4931e0();
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
        v7 = v8;
        sub_401720("Pos %.3d %.3d", sub_4931e0());
        *((unsigned int *)(g_4b43b8 + 102272)) = 0xffffffff;
        if (!(idx->field_78c & 2))
            return 1;
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
        idx->field_6ec = v1;
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        /* unsupported instruction */
        /* unsupported instruction */
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        idx->field_6f0 = v2;
        idx->field_6f4 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        v7 = g_4ce8cc;
        sub_45a9f0(v7);
        return 1;
    case 4:
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
        if (/* unsupported instruction */)
        {
            v2 = /* unsupported instruction */;
            /* unsupported instruction */
        }
        else
        {
            v2 = nan;
            /* unsupported instruction */
        }
        v7 = g_4b43dc->field_70;
        /* unsupported instruction */
        /* unsupported instruction */
        v0 = "Enemy %d";
        v3 = (/* unsupported instruction */ ? /* unsupported instruction */ : nan);
        /* unsupported instruction */
        sub_401720();
        return 1;
    default:
        return 1;
    }
}

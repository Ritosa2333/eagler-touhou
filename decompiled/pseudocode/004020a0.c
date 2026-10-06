// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4020a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ad138;

void sub_4020a0(int a0)
{
    unsigned int v18;  // eax
    char i;  // 4107
    unsigned int *index;  // edi
    unsigned int *idx;  // esi
    int <0x4020a0[is_1]|Stack bp-0x244, 1 B>;  // [bp-0x244]
    unsigned short v0;  // [bp-0x244]
    unsigned int v1;  // [bp-0x144]
    unsigned int v2;  // [bp-0x140]
    unsigned int v3;  // [bp-0x13c]
    unsigned int v4;  // [bp-0x138]
    unsigned int v5;  // [bp-0x134]
    unsigned int v6;  // [bp-0x130]
    unsigned int v7;  // [bp-0x128]
    unsigned int v8;  // [bp-0x124]
    unsigned int v9;  // [bp-0x120]
    unsigned int v10;  // [bp-0x11c]
    unsigned int v11;  // [bp-0x118]
    unsigned int v12;  // [bp-0x114]
    unsigned int v13;  // [bp-0x110]
    char v14;  // [bp-0x10c]
    unsigned int v15;  // [bp-0x10]
    unsigned int v16;  // [bp-0x8]
    char v17;  // [bp+0x8]

    v16 = g_4ad138 ^ &<0x4020a0[is_1]|Stack bp-0x244, 1 B>;
    sub_46cad8(&v14, a0, &v17);
    v18 = 0;
    do
    {
        i = (&v14)[v18];
        *((char *)&v0 + v18) = (&v14)[v18];
        v18 += 1;
    } while (i);
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
        v5 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v5 = nan;
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
    v3 = index[2];
    if (/* unsupported instruction */)
    {
        v6 = /* unsupported instruction */;
        /* unsupported instruction */
    }
    else
    {
        v6 = nan;
        /* unsupported instruction */
    }
    v1 = *(index);
    v2 = index[1];
    v8 = idx[25574];
    v4 = idx[25568];
    v7 = idx[25571];
    v11 = idx[25576];
    v9 = idx[25573];
    v10 = idx[25575];
    v12 = idx[25577];
    v13 = idx[25578];
    sub_4018f0();
    _security_check_cookie(v15 ^ &idx, &v0);
    return;
}

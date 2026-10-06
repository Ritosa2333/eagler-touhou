// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x41a000; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_41a000_0 {
    char padding_0[28];
    struct st_41a000_0 *field_1c;
} st_41a000_0;

typedef struct st_41a000_4 {
    char padding_0[9976];
    unsigned int field_26f8;
} st_41a000_4;

extern st_41a000_0 *g_4b0c94;
extern st_41a000_0 *g_4b43dc;
extern st_41a000_0 *g_4b4514;

st_41a000_0 * sub_41a000(st_41a000_4 *a0, unsigned int a1, st_41a000_0 *a2)
{
    st_41a000_0 *v0;  // eax
    unsigned int v1;  // fc3210
    unsigned int v10;  // 4147
    unsigned int v11;  // fc3210
    unsigned short v12;  // ftop
    st_41a000_0 *v14;  // eax
    unsigned int v15;  // 4147
    unsigned int v16;  // fc3210
    unsigned short v17;  // ftop
    st_41a000_0 *v19;  // eax
    unsigned short v2;  // ftop
    unsigned int v20;  // 4147
    st_41a000_0 *v4;  // eax
    unsigned int v5;  // 4147
    unsigned int v6;  // fc3210
    unsigned short v7;  // ftop
    st_41a000_0 *v9;  // eax

    v0 = &a2[312].padding_0[16];
    switch (v0)
    {
    case 0:
        a2 = sub_464440();
        /* unsupported instruction */
        /* unsupported instruction */
        if (a2 >= NULL)
            return a2;
        /* unsupported instruction */
        /* unsupported instruction */
        return a2;
    case 1:
        return sub_4645b0();
    case 2:
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return sub_4645e0();
    case 3: case 23:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 4: case 24:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 5: case 25:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 6: case 26:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 7: case 27:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 8: case 28:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 9:
        /* unsupported instruction */
        /* unsupported instruction */
        return g_4b4514;
    case 10:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 11:
        return sub_4377a0();
    case 12:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 13:
        return sub_4645e0();
    case 14:
        a2 = a0->field_26f8 >> 23 & 1;
        /* unsupported instruction */
        /* unsupported instruction */
        if (a2 >= NULL)
            return v0;
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 16:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 17:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 19:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 20:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 22:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 29:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 30:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 31:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 32:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 33:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 34:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 35:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 36:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 37:
        if (g_4b43dc->field_1c)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            return g_4b43dc->field_1c;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return NULL;
    case 38:
        if (g_4b43dc->field_1c)
        {
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            /* unsupported instruction */
            return g_4b43dc->field_1c;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        /* unsupported instruction */
        return NULL;
    case 40:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 41:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 42:
        return sub_41c530();
    case 43:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 44:
        return sub_4377a0();
    case 45:
        return sub_4377a0();
    case 46:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 47:
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = 1;
        v1 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0) * 0x100 & 0x4500;
        /* unsupported instruction */
        v4 = _INSERT(_INSERT(v0, 0, ((char)v2 & 7) * 0), 1, ((v2 & 7) * 0x800 | (unsigned short)v1 & 0x4700) >> 8);
        v5 = _ccall(10, 13, (char)(((v2 & 7) * 0x800 | (unsigned short)v1 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v5 & 1)
        {
            a2 = 0;
            break;
        }
        /* unsupported instruction */
        /* unsupported instruction */
        return v4;
    case 48:
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = 1;
        v6 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x3f800000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v9 = _INSERT(_INSERT(v0, 0, ((char)v7 & 7) * 0), 1, ((v7 & 7) * 0x800 | (unsigned short)v6 & 0x4700) >> 8);
        v10 = _ccall(10, 13, (char)(((v7 & 7) * 0x800 | (unsigned short)v6 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v10 & 1)
            a2 = 0;
        /* unsupported instruction */
        /* unsupported instruction */
        return v9;
    case 49:
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = 1;
        v11 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x40000000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v14 = _INSERT(_INSERT(v0, 0, ((char)v12 & 7) * 0), 1, ((v12 & 7) * 0x800 | (unsigned short)v11 & 0x4700) >> 8);
        v15 = _ccall(10, 13, (char)(((v12 & 7) * 0x800 | (unsigned short)v11 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v15 & 1)
            a2 = 0;
        /* unsupported instruction */
        /* unsupported instruction */
        return v14;
    case 50:
        /* unsupported instruction */
        /* unsupported instruction */
        a2 = 1;
        v16 = CmpF((/* unsupported instruction */ ? /* unsupported instruction */ : nan), 0x40400000) * 0x100 & 0x4500;
        /* unsupported instruction */
        v19 = _INSERT(_INSERT(v0, 0, ((char)v17 & 7) * 0), 1, ((v17 & 7) * 0x800 | (unsigned short)v16 & 0x4700) >> 8);
        v20 = _ccall(10, 13, (char)(((v17 & 7) * 0x800 | (unsigned short)v16 & 0x4700) >> 8) & 0x44, 0, 0);
        if (v20 & 1)
            a2 = 0;
        /* unsupported instruction */
        /* unsupported instruction */
        return v19;
    case 51:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 52:
        /* unsupported instruction */
        /* unsupported instruction */
        return g_4b43dc;
    case 53:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 54:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 55:
        /* unsupported instruction */
        /* unsupported instruction */
        return g_4b0c94;
    case 56:
        return sub_41c4c0();
    case 58:
        /* unsupported instruction */
        /* unsupported instruction */
        return g_4b43dc->field_1c;
    case 59:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 61:
        /* unsupported instruction */
        /* unsupported instruction */
        return g_4b43dc->field_1c;
    case 62:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 64:
        /* unsupported instruction */
        /* unsupported instruction */
        return g_4b43dc->field_1c;
    case 65:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 66:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 67:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 68:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 69:
        /* unsupported instruction */
        /* unsupported instruction */
        return g_4b43dc;
    case 70:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 57:
        v0 = g_4b43dc;
    case 15:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 60:
        v0 = g_4b43dc;
    case 18:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    case 63:
        v0 = g_4b43dc;
    case 21:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    default:
        /* unsupported instruction */
        /* unsupported instruction */
        return v0;
    }
}

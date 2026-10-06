// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x45f550; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_45f550_1 {
    unsigned int field_0;
    unsigned int field_4;
    unsigned int field_8;
    unsigned int field_c;
} st_45f550_1;

typedef struct st_45f550_0 {
    char padding_0[4];
    int field_4;
    int field_8;
    struct st_45f550_1 *field_c;
    unsigned int field_10;
} st_45f550_0;

extern unsigned int g_4a2338[4];
extern unsigned int g_4a235c[4];
extern unsigned int g_4ad138;
extern int g_4ce8f0;
extern char g_4ceae8;

int sub_45f550(unsigned int a0, unsigned int a1, int a2)
{
    unsigned int v5;  // ebx
    unsigned int index;  // ebx
    unsigned int *v7;  // ecx
    int v8;  // edi
    st_45f550_0 *idx;  // esi
    int v10;  // edi
    unsigned int v11;  // eax
    unsigned int v0;  // [bp-0x110]
    char v1;  // [bp-0x10c]
    char v2;  // [bp-0x108]
    unsigned int v3;  // [bp-0x4]
    int v4;  // [bp+0x0]

    v3 = g_4ad138 ^ &v1;
    v0 = v5;
    index = a0;
    if (g_4ceae8 & 1)
    {
        v7 = g_4a2338[index];
        if (!(v7 != 0x15 && v7))
        {
            index = 5;
        }
        else if (v7 == 0x14)
        {
            index = 3;
        }
    }
    sub_46cbbb(&v2, "%s", a2);
    v8 = sub_463c10(&v1, 1);
    if (v8)
    {
        idx->field_8 = v10;
        if (D3DXCreateTextureFromFileInMemoryEx(g_4ce8f0, v8, v10, 0, 0, 0, 0, g_4a2338[index], 1, 1, -0x1, v4, 0, 0, idx))
        {
            sub_46c941(v8);
            goto LABEL_45f605;
        }
        else
        {
            idx->field_10 = idx->field_10 & 0xfffffffe;
            sub_45ee00();
            idx->field_c = g_4a235c[index];
            idx->field_4 = v8;
        }
    }
    else
    {
LABEL_45f605:
    }
    _security_check_cookie();
    return v11;
}

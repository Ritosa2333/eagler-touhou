// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4281c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4281c0_0 {
    char padding_0[1132];
    unsigned int field_46c;
} st_4281c0_0;

extern unsigned int g_4b44f4;

st_4281c0_0 * sub_4281c0(void)
{
    int v1;  // edi
    st_4281c0_0 *v2;  // edi
    int v3;  // esi
    int v4;  // ebx

    v2 = sub_46c9ea(1168, v1);
    if (v2)
    {
        sub_427ec0(v3);
        sub_477420(v2, 0, 1168);
        v2->field_46c = 0x10000;
        g_4b44f4 = v2;
    }
    else
    {
        v2 = NULL;
    }
    if (!sub_427fd0(v2))
        return v2;
    if (!v2)
        return NULL;
    sub_4280d0(v4);
    sub_46ca4f(v2);
    return NULL;
}

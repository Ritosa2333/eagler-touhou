// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x468e20; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_468e20_3 {
    char padding_0[4];
    struct st_468e20_0 *field_4;
    char padding_8[4096];
    int field_1008;
    unsigned int field_100c;
    char padding_1010[4];
    struct st_468e20_4 *field_1014;
} st_468e20_3;

typedef struct st_468e20_4 {
    struct st_468e20_1 *field_0;
} st_468e20_4;

typedef struct st_468e20_2 {
    unsigned int field_0;
} st_468e20_2;

typedef struct st_468e20_1 {
    char padding_0[4];
    struct st_468e20_2 *field_4;
} st_468e20_1;

typedef struct st_468e20_0 {
    char padding_0[8];
    unsigned short field_8;
} st_468e20_0;


int sub_468e20(unsigned int a0, st_468e20_3 *idx, unsigned int a2)
{
    st_468e20_0 *v0;  // esi
    unsigned int v1;  // esi
    unsigned int v2;  // esi
    unsigned int v3;  // eax
    unsigned int v4;  // esi

    v0 = idx->field_4;
    if (!(v0->field_8 & 1 << ((char)a2 & 31)))
        return *((int *)&v0[1].padding_0[6 + 4 * a2]);
    v1 = *((int *)&v0[1].padding_0[6 + 4 * a2]);
    if (v1 >= 0)
    {
        return *((int *)&idx->padding_8[v1 + idx->field_100c]);
    }
    else if (v1 == 0xffffffff)
    {
        v2 = idx->field_1008 - 4;
        if (v2 < 0)
            return a2;
        idx->field_1008 = v2;
        v3 = *((int *)&idx->padding_8[v2]);
        v4 = v2 - 4;
        idx->field_1008 = v4;
        a2 = v3;
        if (idx->padding_8[v4] != 0x66)
            return a2;
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
        return sub_4931e0();
    }
    else
    {
        return idx->field_1014->field_0->field_4(v1);
    }
}

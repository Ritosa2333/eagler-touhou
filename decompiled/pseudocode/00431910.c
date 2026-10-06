// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x431910; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_431910_1 {
    unsigned int field_0;
} st_431910_1;

typedef struct st_431910_0 {
    char padding_0[8];
    struct st_431910_1 *field_8;
} st_431910_0;

extern char g_4b564c;
extern unsigned int g_4ce8cc;

st_431910_0 ** sub_431910(void)
{
    st_431910_0 **v1;  // eax
    st_431910_0 **v2;  // eax

    v1 = *((int *)&(&g_4b564c)[g_4ce8cc]);
    if (!v1)
        return v1;
    v2 = *(v1)->field_8(v1);
    *((unsigned int *)&(&g_4b564c)[g_4ce8cc]) = 0;
    return v2;
}

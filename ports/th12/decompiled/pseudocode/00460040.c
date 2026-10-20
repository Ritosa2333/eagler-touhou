// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x460040; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_460040_0 {
    char padding_0[292];
    unsigned int field_124;
} st_460040_0;

int sub_460040(void)
{
    char *v3;  // eax
    char *v4;  // ecx
    unsigned int v6;  // edx
    st_460040_0 *idx;  // edi
    st_460040_0 *v8;  // edi
    char *v1;  // [bp+0xc]
    unsigned int v2;  // [bp+0x10]

    *(v3) = *(v3) + *((char *)&v3);
    *(v4) = *(v4) + *((char *)&v3);
    if (!_INSERT(v3, 0, *((char *)&v3) & 12))
    {
        v8->field_124 = 0;
        return;
    }
    v1 = &v4[v6];
    if (v1 != idx->field_124)
        if (!v2)
            goto LABEL_0x460000;
        else
            goto LABEL_460062;
LABEL_460062:
    idx->field_124 = idx->field_124 + v6;
    return;
}

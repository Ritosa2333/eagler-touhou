// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4305a0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_4305a0_0 {
    char padding_0[1420];
    int field_58c;
    char padding_590[940];
    unsigned int field_93c;
} st_4305a0_0;

extern int g_4b44fc;
extern int g_4b4500;
extern int g_4b4504;

int sub_4305a0(void)
{
    unsigned int v4;  // esi
    st_4305a0_0 *v5;  // eax
    unsigned int *index;  // eax
    unsigned int *idx;  // edi
    unsigned int *idx1;  // eax
    unsigned int *idx2;  // eax
    char *v0;  // [bp-0x24]
    unsigned int v1;  // [bp-0x14]
    unsigned int v2;  // [bp-0x8]

    v2 = v4;
    if (v5->field_93c)
        return;
    v1 = &ecx;
    sub_4615a0(v5->field_58c, &ecx, 0, 0);
    g_4b44fc = &ecx;
    v0 = &v1;
    sub_4615a0(v5->field_58c, &v1, 1, 0);
    g_4b4500 = &v1;
    sub_4615a0(v5->field_58c, &v0, 2, 0);
    g_4b4504 = &v0;
    v5->field_93c = 1;
    index = sub_461920(g_4b44fc);
    if (index)
    {
        index[268] = *(idx);
        index[269] = idx[1];
        index[270] = idx[2];
    }
    idx1 = sub_461920(g_4b4500);
    if (idx1)
    {
        idx1[268] = *(idx);
        idx1[269] = idx[1];
        idx1[270] = idx[2];
    }
    idx2 = sub_461920(g_4b4504);
    if (!idx2)
        return;
    idx2[268] = *(idx);
    idx2[269] = idx[1];
    idx2[270] = idx[2];
    return;
}

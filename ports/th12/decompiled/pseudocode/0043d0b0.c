// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43d0b0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43d0b0_0 {
    int field_0;
    int field_4;
} st_43d0b0_0;

extern st_43d0b0_0 *g_4b451c;

unsigned int sub_43d0b0(void)
{
    int *v1;  // esi
    unsigned int v2;  // eax

    v1 = &g_4b451c->field_0;
    if (v1)
    {
        if (*(v1))
        {
            sub_46c941(*(v1));
            *(v1) = 0;
        }
        if (v1[1])
        {
            sub_46c941(v1[1]);
            v1[1] = 0;
        }
        v2 = sub_46ca4f(v1);
    }
    g_4b451c = 0;
    return v2;
}

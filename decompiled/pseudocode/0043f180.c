// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x43f180; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_43f180_0 {
    char padding_0[125392];
    char field_1e9d0;
    char field_1e9d1;
    char field_1e9d2;
    char field_1e9d3;
    char field_1e9d4;
    char field_1e9d5;
} st_43f180_0;

extern st_43f180_0 *g_4b451c;

unsigned int sub_43f180(void)
{
    if (g_4b451c->field_1e9d0 & 16)
    {
        return 1;
    }
    else if (g_4b451c->field_1e9d1 & 16)
    {
        return 1;
    }
    else if (g_4b451c->field_1e9d2 & 16)
    {
        return 1;
    }
    else if (g_4b451c->field_1e9d3 & 16)
    {
        return 1;
    }
    else if (g_4b451c->field_1e9d4 & 16)
    {
        return 1;
    }
    else if (!(g_4b451c->field_1e9d5 & 16))
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

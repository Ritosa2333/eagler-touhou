// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x47c0fc; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

typedef struct st_47c0fc_0 {
    char padding_0[12];
    unsigned int field_c;
} st_47c0fc_0;

extern st_47c0fc_0 g_4adbf0;
extern st_47c0fc_0 g_4ade50;

void sub_47c0fc(st_47c0fc_0 *idx)
{
    if (idx >= &g_4adbf0.padding_0[0] && idx <= &g_4ade50.padding_0[0])
    {
        sub_46ec9a(((int)(idx - &g_4adbf0.padding_0[0]) >> 5) + 16);
        idx->field_c = idx->field_c | 0x8000;
        return;
    }
    EnterCriticalSection(idx + 2);
    return;
}

// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x464bd0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4a3738;

int sub_464bd0(void)
{
    void* v1;  // eax

    *((char **)v1) = &g_4a3738;
    memset(v1 + 4, 0, 16);
    return;
}

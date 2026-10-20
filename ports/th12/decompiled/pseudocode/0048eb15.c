// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x48eb15; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ae424;

void sub_48eb15(void)
{
    g_4ae424 = CreateFileA("CONOUT$", 0x40000000, 3, NULL, OPEN_EXISTING, SECURITY_ANONYMOUS, NULL);
    return;
}

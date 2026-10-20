// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x4518c0; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern char g_4ceacd;
extern unsigned int g_4ceae8;
extern unsigned int g_4cf428;

unsigned int sub_4518c0(HWND a0, unsigned int a1, unsigned short a2)
{
    unsigned int v4;  // eax
    int v0;  // [bp-0x1c]
    unsigned int v1;  // [bp-0x18]
    WPARAM len;  // [bp-0x14]
    LPARAM v3;  // [bp-0x10]

    v4 = a1 - 16;
    if (a1 != 16)
    {
        if (v4 == 0x100)
        {
            if (g_4ceae8 & 0x100)
                SendMessageA(GetDlgItem(a0, 202), 241, 0x1, NULL);
            switch (g_4ceacd)
            {
            case 0:
                v3 = NULL;
                len = 0x1;
                v1 = 241;
                v0 = 0xcc;
                goto LABEL_451a30;
            case 1:
                v3 = NULL;
                len = 0x1;
                v1 = 241;
                v0 = 205;
                goto LABEL_451a30;
            case 2:
                v3 = NULL;
                len = 0x1;
                v1 = 241;
                v0 = 206;
                goto LABEL_451a30;
            case 3:
                v3 = NULL;
                len = 0x1;
                v1 = 241;
                v0 = 207;
LABEL_451a30:
                SendMessageA(GetDlgItem(a0, v0), v1, len, v3);
            }
            g_4cf428 = g_4cf428 & 0xffffffdf | 64;
            return 0;
        }
        else if (v4 != 0x101)
        {
            return 0;
        }
        else if (a2 == 208)
        {
            if (IsDlgButtonChecked(a0, 202) == 1)
                g_4ceae8 = g_4ceae8 | 0x100;
            else
                g_4ceae8 = g_4ceae8 & 0xfffffeff;
            if (IsDlgButtonChecked(a0, 0xcc) == 1)
            {
                g_4ceacd = 0;
            }
            else if (IsDlgButtonChecked(a0, 205) == 1)
            {
                g_4ceacd = 1;
            }
            else if (IsDlgButtonChecked(a0, 206) == 1)
            {
                g_4ceacd = 2;
            }
            else
            {
                g_4ceacd = (-(0 < IsDlgButtonChecked(a0, 207) - 1) & 253) + 3;
            }
            g_4cf428 = g_4cf428 & 0xffffff9f;
            EndDialog(a0, 0x6);
        }
        else
        {
            return 0;
        }
    }
    if (((char)g_4cf428 & 96) == 64)
        g_4cf428 = g_4cf428 & 0xffffffbf | 32;
    EndDialog(a0, 0x6);
    return 1;
}

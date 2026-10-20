// AUTOMATIC PSEUDOCODE: NOT ORIGINAL SOURCE OR VERIFIED BEHAVIOR
// TH12 1.00b 0x44fe00; EXE 99907258b44ea25be41fb4e607cbe7f64b79021148d9fb95a9a7ebf979095417; angr 10.0.1.post1
// Inferred types/register conventions/x87 expressions require assembly review.

extern unsigned int g_4ce9fc;
extern unsigned int g_4cee78;
extern unsigned int g_4cf3fc;
extern unsigned int g_4cf400;
extern unsigned int g_4cf428;

LRESULT sub_44fe00(HWND a0, unsigned int a1, unsigned int a2, LPARAM a3)
{
    unsigned int v0;  // eax

    if (a1 <= 28)
    {
        if (a1 == 28)
        {
            g_4cf3fc = a2;
            g_4cf400 = !a2;
            return DefWindowProcA(a0, a1, a2, a3);
        }
        else if (a1 != 5)
        {
            if (a1 == 16)
            {
                g_4cee78 = g_4cee78 & 0xfffffeff | 128;
                return 0x1;
            }
            else if (a1 == 20)
            {
                return 0x1;
            }
        }
        else
        {
            if ((char)g_4cf428 & 1 && a2 == 2)
            {
                g_4cf428 = g_4cf428 & 0xfffffff3 | 2;
                return DefWindowProcA(a0, 5, a2, a3);
            }
        }
    }
    else
    {
        if (a1 != 32)
        {
            if (a1 == 274)
            {
                v0 = a2 & 0xfff0;
                if (v0 == 61584)
                    return 0x1;
                if (v0 == 0xf100)
                    return 0x1;
            }
            else if (a1 == 513)
            {
                SetForegroundWindow(a0);
                return DefWindowProcA(a0, 513, a2, a3);
            }
        }
        else
        {
            if (g_4ce9fc)
            {
                SetCursor(LoadCursorA(NULL, 0x7f00));
                ShowCursor(1);
                return 0x1;
            }
            else if (g_4cf400)
            {
                SetCursor(LoadCursorA(NULL, 0x7f00));
                do
                { } while (ShowCursor(1) < 0);
                return 0x1;
            }
            else
            {
                do
                { } while (ShowCursor(0) >= 0);
                SetCursor(NULL);
                return 0x1;
            }
        }
    }
    return DefWindowProcA(a0, a1, a2, a3);
}

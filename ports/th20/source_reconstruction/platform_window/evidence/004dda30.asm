004dda30  55                                 push       ebp
004dda31  8b ec                              mov        ebp, esp
004dda33  51                                 push       ecx
004dda34  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004dda37  0f b6 45 08                        movzx      eax, byte ptr [ebp + 8]
004dda3b  24 03                              and        al, 3
004dda3d  c0 e0 02                           shl        al, 2
004dda40  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004dda43  8a 91 a0 04 00 00                  mov        dl, byte ptr [ecx + 0x4a0]
004dda49  80 e2 f3                           and        dl, 0xf3
004dda4c  0a d0                              or         dl, al
004dda4e  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004dda51  88 90 a0 04 00 00                  mov        byte ptr [eax + 0x4a0], dl
004dda57  8b e5                              mov        esp, ebp
004dda59  5d                                 pop        ebp
004dda5a  c2 04 00                           ret        4

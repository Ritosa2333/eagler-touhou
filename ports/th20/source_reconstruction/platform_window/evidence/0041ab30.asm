0041ab30  55                                 push       ebp
0041ab31  8b ec                              mov        ebp, esp
0041ab33  83 ec 0c                           sub        esp, 0xc
0041ab36  68 ca 00 00 00                     push       0xca
0041ab3b  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ab40  e8 4b 54 ff ff                     call       0x40ff90
0041ab45  50                                 push       eax
0041ab46  ff 15 3c c2 56 00                  call       dword ptr [0x56c23c] ; USER32.dll!IsDlgButtonChecked
0041ab4c  83 f8 01                           cmp        eax, 1
0041ab4f  75 11                              jne        0x41ab62
0041ab51  a1 8c 4f 5c 00                     mov        eax, dword ptr [0x5c4f8c]
0041ab56  0d 80 00 00 00                     or         eax, 0x80
0041ab5b  a3 8c 4f 5c 00                     mov        dword ptr [0x5c4f8c], eax
0041ab60  eb 12                              jmp        0x41ab74
0041ab62  8b 0d 8c 4f 5c 00                  mov        ecx, dword ptr [0x5c4f8c]
0041ab68  81 e1 7f ff ff ff                  and        ecx, 0xffffff7f
0041ab6e  89 0d 8c 4f 5c 00                  mov        dword ptr [0x5c4f8c], ecx
0041ab74  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0041ab7b  eb 09                              jmp        0x41ab86
0041ab7d  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0041ab80  83 c2 01                           add        edx, 1
0041ab83  89 55 fc                           mov        dword ptr [ebp - 4], edx
0041ab86  83 7d fc 0a                        cmp        dword ptr [ebp - 4], 0xa
0041ab8a  7d 49                              jge        0x41abd5
0041ab8c  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041ab8f  8b 0c 85 28 e1 5a 00               mov        ecx, dword ptr [eax*4 + 0x5ae128]
0041ab96  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0041ab99  b9 58 67 5b 00                     mov        ecx, 0x5b6758
0041ab9e  e8 ed 53 ff ff                     call       0x40ff90
0041aba3  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
0041aba6  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
0041aba9  52                                 push       edx
0041abaa  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
0041abad  50                                 push       eax
0041abae  ff 15 3c c2 56 00                  call       dword ptr [0x56c23c] ; USER32.dll!IsDlgButtonChecked
0041abb4  83 f8 01                           cmp        eax, 1
0041abb7  75 1a                              jne        0x41abd3
0041abb9  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041abbc  89 0d 80 4f 5c 00                  mov        dword ptr [0x5c4f80], ecx
0041abc2  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
0041abc5  8a 04 95 d0 e1 5a 00               mov        al, byte ptr [edx*4 + 0x5ae1d0]
0041abcc  a2 8a 4f 5c 00                     mov        byte ptr [0x5c4f8a], al
0041abd1  eb 02                              jmp        0x41abd5
0041abd3  eb a8                              jmp        0x41ab7d
0041abd5  8b e5                              mov        esp, ebp
0041abd7  5d                                 pop        ebp
0041abd8  c3                                 ret        

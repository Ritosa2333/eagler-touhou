004156e0  55                                 push       ebp
004156e1  8b ec                              mov        ebp, esp
004156e3  81 ec 20 02 00 00                  sub        esp, 0x220
004156e9  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
004156ee  33 c5                              xor        eax, ebp
004156f0  89 45 fc                           mov        dword ptr [ebp - 4], eax
004156f3  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
004156f6  50                                 push       eax
004156f7  b9 f0 66 5b 00                     mov        ecx, 0x5b66f0
004156fc  e8 7f ee ff ff                     call       0x414580
00415701  8b 08                              mov        ecx, dword ptr [eax]
00415703  89 8d e8 fd ff ff                  mov        dword ptr [ebp - 0x218], ecx
00415709  68 02 02 00 00                     push       0x202
0041570e  6a 00                              push       0
00415710  8d 95 f8 fd ff ff                  lea        edx, [ebp - 0x208]
00415716  52                                 push       edx
00415717  e8 44 f0 12 00                     call       0x544760
0041571c  83 c4 0c                           add        esp, 0xc
0041571f  68 00 01 00 00                     push       0x100
00415724  8d 85 f8 fd ff ff                  lea        eax, [ebp - 0x208]
0041572a  50                                 push       eax
0041572b  6a ff                              push       -1
0041572d  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00415730  51                                 push       ecx
00415731  6a 00                              push       0
00415733  68 a4 03 00 00                     push       0x3a4
00415738  ff 15 24 c1 56 00                  call       dword ptr [0x56c124] ; KERNEL32.dll!MultiByteToWideChar
0041573e  b9 58 67 5b 00                     mov        ecx, 0x5b6758
00415743  e8 b8 6b ff ff                     call       0x40c300
00415748  50                                 push       eax
00415749  ff 15 ac c2 56 00                  call       dword ptr [0x56c2ac] ; USER32.dll!GetDC
0041574f  89 85 ec fd ff ff                  mov        dword ptr [ebp - 0x214], eax
00415755  8b 95 e8 fd ff ff                  mov        edx, dword ptr [ebp - 0x218]
0041575b  52                                 push       edx
0041575c  8b 85 ec fd ff ff                  mov        eax, dword ptr [ebp - 0x214]
00415762  50                                 push       eax
00415763  ff 15 2c c0 56 00                  call       dword ptr [0x56c02c] ; GDI32.dll!SelectObject
00415769  89 85 e0 fd ff ff                  mov        dword ptr [ebp - 0x220], eax
0041576f  8d 8d f8 fd ff ff                  lea        ecx, [ebp - 0x208]
00415775  51                                 push       ecx
00415776  e8 85 2b 13 00                     call       0x548300
0041577b  83 c4 04                           add        esp, 4
0041577e  89 85 e4 fd ff ff                  mov        dword ptr [ebp - 0x21c], eax
00415784  8d 95 f0 fd ff ff                  lea        edx, [ebp - 0x210]
0041578a  52                                 push       edx
0041578b  8b 85 e4 fd ff ff                  mov        eax, dword ptr [ebp - 0x21c]
00415791  50                                 push       eax
00415792  8d 8d f8 fd ff ff                  lea        ecx, [ebp - 0x208]
00415798  51                                 push       ecx
00415799  8b 95 ec fd ff ff                  mov        edx, dword ptr [ebp - 0x214]
0041579f  52                                 push       edx
004157a0  ff 15 30 c0 56 00                  call       dword ptr [0x56c030] ; GDI32.dll!GetTextExtentPoint32W
004157a6  8b 85 f0 fd ff ff                  mov        eax, dword ptr [ebp - 0x210]
004157ac  83 c0 04                           add        eax, 4
004157af  89 85 f0 fd ff ff                  mov        dword ptr [ebp - 0x210], eax
004157b5  8b 8d f4 fd ff ff                  mov        ecx, dword ptr [ebp - 0x20c]
004157bb  83 c1 04                           add        ecx, 4
004157be  89 8d f4 fd ff ff                  mov        dword ptr [ebp - 0x20c], ecx
004157c4  8b 95 e0 fd ff ff                  mov        edx, dword ptr [ebp - 0x220]
004157ca  52                                 push       edx
004157cb  8b 85 ec fd ff ff                  mov        eax, dword ptr [ebp - 0x214]
004157d1  50                                 push       eax
004157d2  ff 15 2c c0 56 00                  call       dword ptr [0x56c02c] ; GDI32.dll!SelectObject
004157d8  8b 85 f0 fd ff ff                  mov        eax, dword ptr [ebp - 0x210]
004157de  8b 95 f4 fd ff ff                  mov        edx, dword ptr [ebp - 0x20c]
004157e4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004157e7  33 cd                              xor        ecx, ebp
004157e9  e8 00 d2 12 00                     call       0x5429ee
004157ee  8b e5                              mov        esp, ebp
004157f0  5d                                 pop        ebp
004157f1  c3                                 ret        

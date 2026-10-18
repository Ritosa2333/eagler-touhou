0041a1b0  55                                 push       ebp
0041a1b1  8b ec                              mov        ebp, esp
0041a1b3  83 ec 10                           sub        esp, 0x10
0041a1b6  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041a1bb  33 c5                              xor        eax, ebp
0041a1bd  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041a1c0  b8 01 00 00 00                     mov        eax, 1
0041a1c5  66 89 45 f0                        mov        word ptr [ebp - 0x10], ax
0041a1c9  b9 04 00 00 00                     mov        ecx, 4
0041a1ce  66 89 4d f2                        mov        word ptr [ebp - 0xe], cx
0041a1d2  c7 45 f4 00 01 00 00               mov        dword ptr [ebp - 0xc], 0x100
0041a1d9  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0041a1dc  89 55 f8                           mov        dword ptr [ebp - 8], edx
0041a1df  6a 0c                              push       0xc
0041a1e1  6a 01                              push       1
0041a1e3  8d 45 f0                           lea        eax, [ebp - 0x10]
0041a1e6  50                                 push       eax
0041a1e7  ff 15 8c c2 56 00                  call       dword ptr [0x56c28c] ; USER32.dll!RegisterRawInputDevices
0041a1ed  90                                 nop        
0041a1ee  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a1f1  33 cd                              xor        ecx, ebp
0041a1f3  e8 f6 87 12 00                     call       0x5429ee
0041a1f8  8b e5                              mov        esp, ebp
0041a1fa  5d                                 pop        ebp
0041a1fb  c3                                 ret        

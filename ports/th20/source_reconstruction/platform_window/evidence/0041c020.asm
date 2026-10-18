0041c020  55                                 push       ebp
0041c021  8b ec                              mov        ebp, esp
0041c023  81 ec 60 04 00 00                  sub        esp, 0x460
0041c029  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
0041c02e  33 c5                              xor        eax, ebp
0041c030  89 45 fc                           mov        dword ptr [ebp - 4], eax
0041c033  68 0c cd 56 00                     push       0x56cd0c ; string='th20 App'
0041c038  6a 01                              push       1
0041c03a  6a 00                              push       0
0041c03c  ff 15 14 c1 56 00                  call       dword ptr [0x56c114] ; KERNEL32.dll!CreateMutexW
0041c042  a3 90 88 5b 00                     mov        dword ptr [0x5b8890], eax
0041c047  ff 15 30 c1 56 00                  call       dword ptr [0x56c130] ; KERNEL32.dll!GetLastError
0041c04d  3d b7 00 00 00                     cmp        eax, 0xb7
0041c052  75 19                              jne        0x41c06d
0041c054  68 20 cd 56 00                     push       0x56cd20
0041c059  68 78 06 5c 00                     push       0x5c0678
0041c05e  e8 cd 81 03 00                     call       0x454230
0041c063  83 c4 08                           add        esp, 8
0041c066  83 c8 ff                           or         eax, 0xffffffff
0041c069  eb 63                              jmp        0x41c0ce
0041c06d  c7 85 a0 fb ff ff 44 00 00 00      mov        dword ptr [ebp - 0x460], 0x44
0041c077  6a 40                              push       0x40
0041c079  6a 00                              push       0
0041c07b  8d 85 a4 fb ff ff                  lea        eax, [ebp - 0x45c]
0041c081  50                                 push       eax
0041c082  e8 d9 86 12 00                     call       0x544760
0041c087  83 c4 0c                           add        esp, 0xc
0041c08a  68 04 01 00 00                     push       0x104
0041c08f  8d 8d f0 fd ff ff                  lea        ecx, [ebp - 0x210]
0041c095  51                                 push       ecx
0041c096  6a 00                              push       0
0041c098  ff 15 0c c1 56 00                  call       dword ptr [0x56c10c] ; KERNEL32.dll!GetModuleFileNameW
0041c09e  68 04 01 00 00                     push       0x104
0041c0a3  8d 95 e4 fb ff ff                  lea        edx, [ebp - 0x41c]
0041c0a9  52                                 push       edx
0041c0aa  ff 15 4c c0 56 00                  call       dword ptr [0x56c04c] ; KERNEL32.dll!GetConsoleTitleW
0041c0b0  8d 85 a0 fb ff ff                  lea        eax, [ebp - 0x460]
0041c0b6  50                                 push       eax
0041c0b7  ff 15 10 c1 56 00                  call       dword ptr [0x56c110] ; KERNEL32.dll!GetStartupInfoW
0041c0bd  90                                 nop        
0041c0be  83 3d 90 88 5b 00 00               cmp        dword ptr [0x5b8890], 0
0041c0c5  75 05                              jne        0x41c0cc
0041c0c7  83 c8 ff                           or         eax, 0xffffffff
0041c0ca  eb 02                              jmp        0x41c0ce
0041c0cc  33 c0                              xor        eax, eax
0041c0ce  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041c0d1  33 cd                              xor        ecx, ebp
0041c0d3  e8 16 69 12 00                     call       0x5429ee
0041c0d8  8b e5                              mov        esp, ebp
0041c0da  5d                                 pop        ebp
0041c0db  c3                                 ret        

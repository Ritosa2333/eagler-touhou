0041d0c0  55                                 push       ebp
0041d0c1  8b ec                              mov        ebp, esp
0041d0c3  83 ec 08                           sub        esp, 8
0041d0c6  ff 15 a4 c0 56 00                  call       dword ptr [0x56c0a4] ; KERNEL32.dll!GetUserDefaultLCID
0041d0cc  89 45 f8                           mov        dword ptr [ebp - 8], eax
0041d0cf  81 7d f8 11 04 00 00               cmp        dword ptr [ebp - 8], 0x411
0041d0d6  75 09                              jne        0x41d0e1
0041d0d8  c7 45 fc 01 00 00 00               mov        dword ptr [ebp - 4], 1
0041d0df  eb 07                              jmp        0x41d0e8
0041d0e1  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0041d0e8  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041d0eb  8b e5                              mov        esp, ebp
0041d0ed  5d                                 pop        ebp
0041d0ee  c3                                 ret        

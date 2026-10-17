0050fc90  55                                 push       ebp
0050fc91  8b ec                              mov        ebp, esp
0050fc93  83 ec 10                           sub        esp, 0x10
0050fc96  89 4d f4                           mov        dword ptr [ebp - 0xc], ecx
0050fc99  8b 45 f4                           mov        eax, dword ptr [ebp - 0xc]
0050fc9c  83 c0 08                           add        eax, 8
0050fc9f  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0050fca2  c7 45 f8 00 00 00 00               mov        dword ptr [ebp - 8], 0
0050fca9  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
0050fcb0  eb 09                              jmp        0x50fcbb
0050fcb2  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050fcb5  83 c1 01                           add        ecx, 1
0050fcb8  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050fcbb  8b 55 08                           mov        edx, dword ptr [ebp + 8]
0050fcbe  83 ea 08                           sub        edx, 8
0050fcc1  39 55 fc                           cmp        dword ptr [ebp - 4], edx
0050fcc4  7d 11                              jge        0x50fcd7
0050fcc6  8b 45 f0                           mov        eax, dword ptr [ebp - 0x10]
0050fcc9  03 45 fc                           add        eax, dword ptr [ebp - 4]
0050fccc  0f b6 08                           movzx      ecx, byte ptr [eax]
0050fccf  03 4d f8                           add        ecx, dword ptr [ebp - 8]
0050fcd2  89 4d f8                           mov        dword ptr [ebp - 8], ecx
0050fcd5  eb db                              jmp        0x50fcb2
0050fcd7  8b 45 f8                           mov        eax, dword ptr [ebp - 8]
0050fcda  8b e5                              mov        esp, ebp
0050fcdc  5d                                 pop        ebp
0050fcdd  c2 04 00                           ret        4

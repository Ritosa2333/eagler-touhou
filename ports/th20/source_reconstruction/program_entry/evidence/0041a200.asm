; release Direct3D object
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041a200  55                                 push       ebp
0041a201  8b ec                              mov        ebp, esp
0041a203  51                                 push       ecx
0041a204  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041a207  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041a20a  83 78 04 00                        cmp        dword ptr [eax + 4], 0
0041a20e  74 1e                              je         0x41a22e
0041a210  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a213  8b 51 04                           mov        edx, dword ptr [ecx + 4]
0041a216  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041a219  8b 48 04                           mov        ecx, dword ptr [eax + 4]
0041a21c  8b 12                              mov        edx, dword ptr [edx]
0041a21e  51                                 push       ecx
0041a21f  8b 42 08                           mov        eax, dword ptr [edx + 8]
0041a222  ff d0                              call       eax
0041a224  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0041a227  c7 41 04 00 00 00 00               mov        dword ptr [ecx + 4], 0
0041a22e  8b e5                              mov        esp, ebp
0041a230  5d                                 pop        ebp
0041a231  c3                                 ret

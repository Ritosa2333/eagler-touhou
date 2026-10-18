0050e6a0  55                                 push       ebp
0050e6a1  8b ec                              mov        ebp, esp
0050e6a3  51                                 push       ecx
0050e6a4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050e6a7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e6aa  33 c9                              xor        ecx, ecx
0050e6ac  33 d2                              xor        edx, edx
0050e6ae  89 08                              mov        dword ptr [eax], ecx
0050e6b0  89 50 04                           mov        dword ptr [eax + 4], edx
0050e6b3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e6b6  c6 40 08 00                        mov        byte ptr [eax + 8], 0
0050e6ba  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e6bd  c6 41 09 00                        mov        byte ptr [ecx + 9], 0
0050e6c1  33 d2                              xor        edx, edx
0050e6c3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e6c6  83 c0 0a                           add        eax, 0xa
0050e6c9  66 89 10                           mov        word ptr [eax], dx
0050e6cc  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e6cf  8b e5                              mov        esp, ebp
0050e6d1  5d                                 pop        ebp
0050e6d2  c3                                 ret        

0050e4a0  55                                 push       ebp
0050e4a1  8b ec                              mov        ebp, esp
0050e4a3  51                                 push       ecx
0050e4a4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0050e4a7  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e4aa  33 c9                              xor        ecx, ecx
0050e4ac  33 d2                              xor        edx, edx
0050e4ae  89 08                              mov        dword ptr [eax], ecx
0050e4b0  89 50 04                           mov        dword ptr [eax + 4], edx
0050e4b3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e4b6  c6 40 08 00                        mov        byte ptr [eax + 8], 0
0050e4ba  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e4bd  c6 41 09 00                        mov        byte ptr [ecx + 9], 0
0050e4c1  33 d2                              xor        edx, edx
0050e4c3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e4c6  83 c0 0a                           add        eax, 0xa
0050e4c9  89 10                              mov        dword ptr [eax], edx
0050e4cb  89 50 04                           mov        dword ptr [eax + 4], edx
0050e4ce  66 89 50 08                        mov        word ptr [eax + 8], dx
0050e4d2  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e4d5  33 d2                              xor        edx, edx
0050e4d7  33 c0                              xor        eax, eax
0050e4d9  89 51 18                           mov        dword ptr [ecx + 0x18], edx
0050e4dc  89 41 1c                           mov        dword ptr [ecx + 0x1c], eax
0050e4df  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
0050e4e2  0f 57 c0                           xorps      xmm0, xmm0
0050e4e5  f3 0f 11 41 20                     movss      dword ptr [ecx + 0x20], xmm0
0050e4ea  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0050e4ed  8b e5                              mov        esp, ebp
0050e4ef  5d                                 pop        ebp
0050e4f0  c3                                 ret        

00478530 55                       push       ebp
00478531 8bec                     mov        ebp, esp
00478533 51                       push       ecx
00478534 894dfc                   mov        dword ptr [ebp - 4], ecx
00478537 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047853a e8d1a8faff               call       0x422e10
0047853f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478542 83c10c                   add        ecx, 0xc
00478545 e8c6a8faff               call       0x422e10
0047854a 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047854d 0f57c0                   xorps      xmm0, xmm0
00478550 f30f114018               movss      dword ptr [eax + 0x18], xmm0
00478555 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478558 83c11c                   add        ecx, 0x1c
0047855b e880f8fcff               call       0x447de0
00478560 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478563 0f57c0                   xorps      xmm0, xmm0
00478566 f30f114120               movss      dword ptr [ecx + 0x20], xmm0
0047856b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047856e 0f57c0                   xorps      xmm0, xmm0
00478571 f30f114224               movss      dword ptr [edx + 0x24], xmm0
00478576 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00478579 83c128                   add        ecx, 0x28
0047857c e85ff8fcff               call       0x447de0
00478581 8b45fc                   mov        eax, dword ptr [ebp - 4]
00478584 0f57c0                   xorps      xmm0, xmm0
00478587 f30f11402c               movss      dword ptr [eax + 0x2c], xmm0
0047858c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047858f 83c130                   add        ecx, 0x30
00478592 e849f8fcff               call       0x447de0
00478597 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047859a 0f57c0                   xorps      xmm0, xmm0
0047859d f30f114134               movss      dword ptr [ecx + 0x34], xmm0
004785a2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004785a5 83c138                   add        ecx, 0x38
004785a8 e863a8faff               call       0x422e10
004785ad 33d2                     xor        edx, edx
004785af 8b45fc                   mov        eax, dword ptr [ebp - 4]
004785b2 83c044                   add        eax, 0x44
004785b5 8910                     mov        dword ptr [eax], edx
004785b7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004785ba 8be5                     mov        esp, ebp
004785bc 5d                       pop        ebp
004785bd c3                       ret        
004785be cc                       int3       
004785bf cc                       int3       

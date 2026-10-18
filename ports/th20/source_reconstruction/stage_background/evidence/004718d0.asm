004718d0 55                       push       ebp
004718d1 8bec                     mov        ebp, esp
004718d3 51                       push       ecx
004718d4 894dfc                   mov        dword ptr [ebp - 4], ecx
004718d7 8b45fc                   mov        eax, dword ptr [ebp - 4]
004718da f30f104508               movss      xmm0, dword ptr [ebp + 8]
004718df f30f1100                 movss      dword ptr [eax], xmm0
004718e3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004718e6 f30f10450c               movss      xmm0, dword ptr [ebp + 0xc]
004718eb f30f114104               movss      dword ptr [ecx + 4], xmm0
004718f0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004718f3 f30f104510               movss      xmm0, dword ptr [ebp + 0x10]
004718f8 f30f114208               movss      dword ptr [edx + 8], xmm0
004718fd 8b45fc                   mov        eax, dword ptr [ebp - 4]
00471900 f30f104514               movss      xmm0, dword ptr [ebp + 0x14]
00471905 f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0047190a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047190d f30f104518               movss      xmm0, dword ptr [ebp + 0x18]
00471912 f30f114110               movss      dword ptr [ecx + 0x10], xmm0
00471917 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047191a f30f10451c               movss      xmm0, dword ptr [ebp + 0x1c]
0047191f f30f114214               movss      dword ptr [edx + 0x14], xmm0
00471924 8b45fc                   mov        eax, dword ptr [ebp - 4]
00471927 c7401800000000           mov        dword ptr [eax + 0x18], 0
0047192e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00471931 e8ca1a0000               call       0x473400
00471936 90                       nop        
00471937 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047193a 8be5                     mov        esp, ebp
0047193c 5d                       pop        ebp
0047193d c21800                   ret        0x18

00472060 55                       push       ebp
00472061 8bec                     mov        ebp, esp
00472063 51                       push       ecx
00472064 894dfc                   mov        dword ptr [ebp - 4], ecx
00472067 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047206a f30f1000                 movss      xmm0, dword ptr [eax]
0047206e f30f5c4508               subss      xmm0, dword ptr [ebp + 8]
00472073 51                       push       ecx
00472074 f30f110424               movss      dword ptr [esp], xmm0
00472079 e8c264fcff               call       0x438540
0047207e 83c404                   add        esp, 4
00472081 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472084 d919                     fstp       dword ptr [ecx]
00472086 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472089 8be5                     mov        esp, ebp
0047208b 5d                       pop        ebp
0047208c c20400                   ret        4
0047208f cc                       int3       

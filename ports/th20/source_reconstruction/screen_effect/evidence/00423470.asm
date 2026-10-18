00423470 55                       push       ebp
00423471 8bec                     mov        ebp, esp
00423473 51                       push       ecx
00423474 894dfc                   mov        dword ptr [ebp - 4], ecx
00423477 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0042347a e891f9ffff               call       0x422e10
0042347f 8b45fc                   mov        eax, dword ptr [ebp - 4]
00423482 0f57c0                   xorps      xmm0, xmm0
00423485 f30f11400c               movss      dword ptr [eax + 0xc], xmm0
0042348a 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0042348d c7411000000000           mov        dword ptr [ecx + 0x10], 0
00423494 8b45fc                   mov        eax, dword ptr [ebp - 4]
00423497 8be5                     mov        esp, ebp
00423499 5d                       pop        ebp
0042349a c3                       ret        
0042349b cc                       int3       
0042349c cc                       int3       
0042349d cc                       int3       
0042349e cc                       int3       
0042349f cc                       int3       

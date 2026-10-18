00479060 55                       push       ebp
00479061 8bec                     mov        ebp, esp
00479063 51                       push       ecx
00479064 894dfc                   mov        dword ptr [ebp - 4], ecx
00479067 8b4508                   mov        eax, dword ptr [ebp + 8]
0047906a 50                       push       eax
0047906b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047906e 81c150200000             add        ecx, 0x2050
00479074 e8a7a4faff               call       0x423520
00479079 90                       nop        
0047907a 8be5                     mov        esp, ebp
0047907c 5d                       pop        ebp
0047907d c20400                   ret        4

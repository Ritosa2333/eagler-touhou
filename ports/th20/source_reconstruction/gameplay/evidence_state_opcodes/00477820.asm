00477820 55                       push       ebp
00477821 8bec                     mov        ebp, esp
00477823 51                       push       ecx
00477824 56                       push       esi
00477825 57                       push       edi
00477826 894dfc                   mov        dword ptr [ebp - 4], ecx
00477829 b907000000               mov        ecx, 7
0047782e 8b7508                   mov        esi, dword ptr [ebp + 8]
00477831 8b7dfc                   mov        edi, dword ptr [ebp - 4]
00477834 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00477836 5f                       pop        edi
00477837 5e                       pop        esi
00477838 8be5                     mov        esp, ebp
0047783a 5d                       pop        ebp
0047783b c20400                   ret        4
0047783e cc                       int3       
0047783f cc                       int3       

004780a0 55                       push       ebp
004780a1 8bec                     mov        ebp, esp
004780a3 83ec08                   sub        esp, 8
004780a6 894dfc                   mov        dword ptr [ebp - 4], ecx
004780a9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004780ac 8b4838                   mov        ecx, dword ptr [eax + 0x38]
004780af e83c4afaff               call       0x41caf0
004780b4 8bc8                     mov        ecx, eax
004780b6 e835ffffff               call       0x477ff0
004780bb 85c0                     test       eax, eax
004780bd 7e55                     jle        0x478114
004780bf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004780c2 e869000000               call       0x478130
004780c7 0fb6c8                   movzx      ecx, al
004780ca 85c9                     test       ecx, ecx
004780cc 7546                     jne        0x478114
004780ce 833da4065c0000           cmp        dword ptr [0x5c06a4], 0
004780d5 743d                     je         0x478114
004780d7 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004780dd e87e000000               call       0x478160
004780e2 85c0                     test       eax, eax
004780e4 752e                     jne        0x478114
004780e6 8b55fc                   mov        edx, dword ptr [ebp - 4]
004780e9 8b4a38                   mov        ecx, dword ptr [edx + 0x38]
004780ec e83fa6f9ff               call       0x412730
004780f1 85c0                     test       eax, eax
004780f3 741f                     je         0x478114
004780f5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004780f8 8b4838                   mov        ecx, dword ptr [eax + 0x38]
004780fb e830a6f9ff               call       0x412730
00478100 8bc8                     mov        ecx, eax
00478102 e879ffffff               call       0x478080
00478107 85c0                     test       eax, eax
00478109 7409                     je         0x478114
0047810b c745f801000000           mov        dword ptr [ebp - 8], 1
00478112 eb07                     jmp        0x47811b
00478114 c745f800000000           mov        dword ptr [ebp - 8], 0
0047811b 0fb645f8                 movzx      eax, byte ptr [ebp - 8]
0047811f 8be5                     mov        esp, ebp
00478121 5d                       pop        ebp
00478122 c3                       ret        
00478123 cc                       int3       
00478124 cc                       int3       
00478125 cc                       int3       
00478126 cc                       int3       
00478127 cc                       int3       
00478128 cc                       int3       
00478129 cc                       int3       
0047812a cc                       int3       
0047812b cc                       int3       
0047812c cc                       int3       
0047812d cc                       int3       
0047812e cc                       int3       
0047812f cc                       int3       

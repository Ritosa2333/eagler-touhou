00464840 55                       push       ebp
00464841 8bec                     mov        ebp, esp
00464843 83ec0c                   sub        esp, 0xc
00464846 894dfc                   mov        dword ptr [ebp - 4], ecx
00464849 c745f400000000           mov        dword ptr [ebp - 0xc], 0
00464850 c745f800000000           mov        dword ptr [ebp - 8], 0
00464857 eb09                     jmp        0x464862
00464859 8b45f8                   mov        eax, dword ptr [ebp - 8]
0046485c 83c001                   add        eax, 1
0046485f 8945f8                   mov        dword ptr [ebp - 8], eax
00464862 837df804                 cmp        dword ptr [ebp - 8], 4
00464866 7d22                     jge        0x46488a
00464868 694df8e4050000           imul       ecx, dword ptr [ebp - 8], 0x5e4
0046486f 8b55fc                   mov        edx, dword ptr [ebp - 4]
00464872 8d4c0a08                 lea        ecx, [edx + ecx + 8]
00464876 e8556dfcff               call       0x42b5d0
0046487b 85c0                     test       eax, eax
0046487d 7409                     je         0x464888
0046487f 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00464882 83c001                   add        eax, 1
00464885 8945f4                   mov        dword ptr [ebp - 0xc], eax
00464888 ebcf                     jmp        0x464859
0046488a 837df404                 cmp        dword ptr [ebp - 0xc], 4
0046488e 7c07                     jl         0x464897
00464890 b801000000               mov        eax, 1
00464895 eb44                     jmp        0x4648db
00464897 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046489a 81c198170000             add        ecx, 0x1798
004648a0 e82b6dfcff               call       0x42b5d0
004648a5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004648a8 8b91801d0000             mov        edx, dword ptr [ecx + 0x1d80]
004648ae 83c201                   add        edx, 1
004648b1 8b45fc                   mov        eax, dword ptr [ebp - 4]
004648b4 8990801d0000             mov        dword ptr [eax + 0x1d80], edx
004648ba 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004648bd 83b9841d000000           cmp        dword ptr [ecx + 0x1d84], 0
004648c4 7413                     je         0x4648d9
004648c6 8b55fc                   mov        edx, dword ptr [ebp - 4]
004648c9 83ba801d00003c           cmp        dword ptr [edx + 0x1d80], 0x3c
004648d0 7c07                     jl         0x4648d9
004648d2 b801000000               mov        eax, 1
004648d7 eb02                     jmp        0x4648db
004648d9 33c0                     xor        eax, eax
004648db 8be5                     mov        esp, ebp
004648dd 5d                       pop        ebp
004648de c3                       ret        
004648df cc                       int3       

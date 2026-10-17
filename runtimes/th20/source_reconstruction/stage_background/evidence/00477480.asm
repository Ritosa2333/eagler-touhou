00477480 55                       push       ebp
00477481 8bec                     mov        ebp, esp
00477483 83ec14                   sub        esp, 0x14
00477486 894df8                   mov        dword ptr [ebp - 8], ecx
00477489 8b45f8                   mov        eax, dword ptr [ebp - 8]
0047748c 83b82033000000           cmp        dword ptr [eax + 0x3320], 0
00477493 744a                     je         0x4774df
00477495 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00477498 8b9120330000             mov        edx, dword ptr [ecx + 0x3320]
0047749e 8955f0                   mov        dword ptr [ebp - 0x10], edx
004774a1 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004774a8 eb09                     jmp        0x4774b3
004774aa 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004774ad 83c001                   add        eax, 1
004774b0 8945f4                   mov        dword ptr [ebp - 0xc], eax
004774b3 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004774b6 8b9124330000             mov        edx, dword ptr [ecx + 0x3324]
004774bc 0fbf4202                 movsx      eax, word ptr [edx + 2]
004774c0 3945f4                   cmp        dword ptr [ebp - 0xc], eax
004774c3 7d1a                     jge        0x4774df
004774c5 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004774c8 51                       push       ecx
004774c9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004774cc e87fffffff               call       0x477450
004774d1 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004774d4 81c2e4050000             add        edx, 0x5e4
004774da 8955f0                   mov        dword ptr [ebp - 0x10], edx
004774dd ebcb                     jmp        0x4774aa
004774df c745fc00000000           mov        dword ptr [ebp - 4], 0
004774e6 eb09                     jmp        0x4774f1
004774e8 8b45fc                   mov        eax, dword ptr [ebp - 4]
004774eb 83c001                   add        eax, 1
004774ee 8945fc                   mov        dword ptr [ebp - 4], eax
004774f1 837dfc08                 cmp        dword ptr [ebp - 4], 8
004774f5 7d23                     jge        0x47751a
004774f7 694dfce4050000           imul       ecx, dword ptr [ebp - 4], 0x5e4
004774fe 8b55f8                   mov        edx, dword ptr [ebp - 8]
00477501 8d840a84030000           lea        eax, [edx + ecx + 0x384]
00477508 8945ec                   mov        dword ptr [ebp - 0x14], eax
0047750b 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047750e 51                       push       ecx
0047750f 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00477512 e839ffffff               call       0x477450
00477517 90                       nop        
00477518 ebce                     jmp        0x4774e8
0047751a 8be5                     mov        esp, ebp
0047751c 5d                       pop        ebp
0047751d c20400                   ret        4

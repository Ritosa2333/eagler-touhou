004885e0 55                       push       ebp
004885e1 8bec                     mov        ebp, esp
004885e3 83ec14                   sub        esp, 0x14
004885e6 56                       push       esi
004885e7 57                       push       edi
004885e8 894dfc                   mov        dword ptr [ebp - 4], ecx
004885eb 8b45fc                   mov        eax, dword ptr [ebp - 4]
004885ee 8b80a8000000             mov        eax, dword ptr [eax + 0xa8]
004885f4 99                       cdq        
004885f5 b964000000               mov        ecx, 0x64
004885fa f7f9                     idiv       ecx
004885fc 99                       cdq        
004885fd b9e8030000               mov        ecx, 0x3e8
00488602 f7f9                     idiv       ecx
00488604 8955f0                   mov        dword ptr [ebp - 0x10], edx
00488607 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048860a 8b82a8000000             mov        eax, dword ptr [edx + 0xa8]
00488610 99                       cdq        
00488611 b964000000               mov        ecx, 0x64
00488616 f7f9                     idiv       ecx
00488618 8955ec                   mov        dword ptr [ebp - 0x14], edx
0048861b 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048861e 8b82a8000000             mov        eax, dword ptr [edx + 0xa8]
00488624 99                       cdq        
00488625 b9a0860100               mov        ecx, 0x186a0
0048862a f7f9                     idiv       ecx
0048862c 8945f4                   mov        dword ptr [ebp - 0xc], eax
0048862f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00488632 83e916                   sub        ecx, 0x16
00488635 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00488638 05a6030000               add        eax, 0x3a6
0048863d 99                       cdq        
0048863e bee8030000               mov        esi, 0x3e8
00488643 f7fe                     idiv       esi
00488645 8bf2                     mov        esi, edx
00488647 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0048864a 83c043                   add        eax, 0x43
0048864d 99                       cdq        
0048864e bf64000000               mov        edi, 0x64
00488653 f7ff                     idiv       edi
00488655 03f2                     add        esi, edx
00488657 3bce                     cmp        ecx, esi
00488659 7409                     je         0x488664
0048865b c745f801000000           mov        dword ptr [ebp - 8], 1
00488662 eb07                     jmp        0x48866b
00488664 c745f800000000           mov        dword ptr [ebp - 8], 0
0048866b 8b45f8                   mov        eax, dword ptr [ebp - 8]
0048866e 5f                       pop        edi
0048866f 5e                       pop        esi
00488670 8be5                     mov        esp, ebp
00488672 5d                       pop        ebp
00488673 c3                       ret        
00488674 cc                       int3       
00488675 cc                       int3       
00488676 cc                       int3       
00488677 cc                       int3       
00488678 cc                       int3       
00488679 cc                       int3       
0048867a cc                       int3       
0048867b cc                       int3       
0048867c cc                       int3       
0048867d cc                       int3       
0048867e cc                       int3       
0048867f cc                       int3       

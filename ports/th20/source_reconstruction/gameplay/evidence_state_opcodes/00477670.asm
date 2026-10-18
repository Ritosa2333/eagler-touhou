00477670 55                       push       ebp
00477671 8bec                     mov        ebp, esp
00477673 51                       push       ecx
00477674 56                       push       esi
00477675 57                       push       edi
00477676 894dfc                   mov        dword ptr [ebp - 4], ecx
00477679 8b7dfc                   mov        edi, dword ptr [ebp - 4]
0047767c 83c71c                   add        edi, 0x1c
0047767f b907000000               mov        ecx, 7
00477684 8b7508                   mov        esi, dword ptr [ebp + 8]
00477687 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00477689 5f                       pop        edi
0047768a 5e                       pop        esi
0047768b 8be5                     mov        esp, ebp
0047768d 5d                       pop        ebp
0047768e c20400                   ret        4
00477691 cc                       int3       
00477692 cc                       int3       
00477693 cc                       int3       
00477694 cc                       int3       
00477695 cc                       int3       
00477696 cc                       int3       
00477697 cc                       int3       
00477698 cc                       int3       
00477699 cc                       int3       
0047769a cc                       int3       
0047769b cc                       int3       
0047769c cc                       int3       
0047769d cc                       int3       
0047769e cc                       int3       
0047769f cc                       int3       

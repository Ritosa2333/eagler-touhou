00478610 55                       push       ebp
00478611 8bec                     mov        ebp, esp
00478613 51                       push       ecx
00478614 894dfc                   mov        dword ptr [ebp - 4], ecx
00478617 8b4508                   mov        eax, dword ptr [ebp + 8]
0047861a 8b88c8050000             mov        ecx, dword ptr [eax + 0x5c8]
00478620 81c1b8000000             add        ecx, 0xb8
00478626 51                       push       ecx
00478627 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047862a e8f17cfdff               call       0x450320
0047862f 33c0                     xor        eax, eax
00478631 8be5                     mov        esp, ebp
00478633 5d                       pop        ebp
00478634 c20400                   ret        4
00478637 cc                       int3       
00478638 cc                       int3       
00478639 cc                       int3       
0047863a cc                       int3       
0047863b cc                       int3       
0047863c cc                       int3       
0047863d cc                       int3       
0047863e cc                       int3       
0047863f cc                       int3       

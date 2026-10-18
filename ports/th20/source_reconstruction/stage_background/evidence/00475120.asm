00475120 55                       push       ebp
00475121 8bec                     mov        ebp, esp
00475123 51                       push       ecx
00475124 894dfc                   mov        dword ptr [ebp - 4], ecx
00475127 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047512a c780900ed40700000000     mov        dword ptr [eax + 0x7d40e90], 0
00475134 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00475137 c7818c0ed40780808080     mov        dword ptr [ecx + 0x7d40e8c], 0x80808080
00475141 8be5                     mov        esp, ebp
00475143 5d                       pop        ebp
00475144 c3                       ret        
00475145 cc                       int3       
00475146 cc                       int3       
00475147 cc                       int3       
00475148 cc                       int3       
00475149 cc                       int3       
0047514a cc                       int3       
0047514b cc                       int3       
0047514c cc                       int3       
0047514d cc                       int3       
0047514e cc                       int3       
0047514f cc                       int3       

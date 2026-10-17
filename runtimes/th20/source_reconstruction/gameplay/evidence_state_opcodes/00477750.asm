00477750 55                       push       ebp
00477751 8bec                     mov        ebp, esp
00477753 51                       push       ecx
00477754 894dfc                   mov        dword ptr [ebp - 4], ecx
00477757 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047775a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047775d 89889c000000             mov        dword ptr [eax + 0x9c], ecx
00477763 8be5                     mov        esp, ebp
00477765 5d                       pop        ebp
00477766 c20400                   ret        4
00477769 cc                       int3       
0047776a cc                       int3       
0047776b cc                       int3       
0047776c cc                       int3       
0047776d cc                       int3       
0047776e cc                       int3       
0047776f cc                       int3       

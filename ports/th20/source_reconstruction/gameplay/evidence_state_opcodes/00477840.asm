00477840 55                       push       ebp
00477841 8bec                     mov        ebp, esp
00477843 51                       push       ecx
00477844 894dfc                   mov        dword ptr [ebp - 4], ecx
00477847 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047784a 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0047784d 8988a0000000             mov        dword ptr [eax + 0xa0], ecx
00477853 8b4508                   mov        eax, dword ptr [ebp + 8]
00477856 8be5                     mov        esp, ebp
00477858 5d                       pop        ebp
00477859 c20400                   ret        4
0047785c cc                       int3       
0047785d cc                       int3       
0047785e cc                       int3       
0047785f cc                       int3       

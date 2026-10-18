00470ab0 55                       push       ebp
00470ab1 8bec                     mov        ebp, esp
00470ab3 51                       push       ecx
00470ab4 894dfc                   mov        dword ptr [ebp - 4], ecx
00470ab7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00470aba 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00470abd 8988c4a10100             mov        dword ptr [eax + 0x1a1c4], ecx
00470ac3 8be5                     mov        esp, ebp
00470ac5 5d                       pop        ebp
00470ac6 c20400                   ret        4
00470ac9 cc                       int3       
00470aca cc                       int3       
00470acb cc                       int3       
00470acc cc                       int3       
00470acd cc                       int3       
00470ace cc                       int3       
00470acf cc                       int3       

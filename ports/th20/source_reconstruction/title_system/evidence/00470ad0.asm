00470ad0 55                       push       ebp
00470ad1 8bec                     mov        ebp, esp
00470ad3 51                       push       ecx
00470ad4 894dfc                   mov        dword ptr [ebp - 4], ecx
00470ad7 8b45fc                   mov        eax, dword ptr [ebp - 4]
00470ada 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00470add 8988c0a10100             mov        dword ptr [eax + 0x1a1c0], ecx
00470ae3 8be5                     mov        esp, ebp
00470ae5 5d                       pop        ebp
00470ae6 c20400                   ret        4
00470ae9 cc                       int3       
00470aea cc                       int3       
00470aeb cc                       int3       
00470aec cc                       int3       
00470aed cc                       int3       
00470aee cc                       int3       
00470aef cc                       int3       

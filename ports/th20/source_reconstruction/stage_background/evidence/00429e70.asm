00429e70 55                       push       ebp
00429e71 8bec                     mov        ebp, esp
00429e73 51                       push       ecx
00429e74 894dfc                   mov        dword ptr [ebp - 4], ecx
00429e77 8b4508                   mov        eax, dword ptr [ebp + 8]
00429e7a f7d8                     neg        eax
00429e7c 50                       push       eax
00429e7d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00429e80 e81bf9ffff               call       0x4297a0
00429e85 90                       nop        
00429e86 8be5                     mov        esp, ebp
00429e88 5d                       pop        ebp
00429e89 c20400                   ret        4
00429e8c cc                       int3       
00429e8d cc                       int3       
00429e8e cc                       int3       
00429e8f cc                       int3       

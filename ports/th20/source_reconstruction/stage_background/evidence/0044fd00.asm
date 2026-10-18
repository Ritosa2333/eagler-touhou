0044fd00 55                       push       ebp
0044fd01 8bec                     mov        ebp, esp
0044fd03 51                       push       ecx
0044fd04 894dfc                   mov        dword ptr [ebp - 4], ecx
0044fd07 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044fd0a c74028ffffffff           mov        dword ptr [eax + 0x28], 0xffffffff
0044fd11 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0044fd14 8a919a040000             mov        dl, byte ptr [ecx + 0x49a]
0044fd1a 80e2fe                   and        dl, 0xfe
0044fd1d 8b45fc                   mov        eax, dword ptr [ebp - 4]
0044fd20 88909a040000             mov        byte ptr [eax + 0x49a], dl
0044fd26 8be5                     mov        esp, ebp
0044fd28 5d                       pop        ebp
0044fd29 c3                       ret        
0044fd2a cc                       int3       
0044fd2b cc                       int3       
0044fd2c cc                       int3       
0044fd2d cc                       int3       
0044fd2e cc                       int3       
0044fd2f cc                       int3       

00531c10 55                       push       ebp
00531c11 8bec                     mov        ebp, esp
00531c13 51                       push       ecx
00531c14 8b4508                   mov        eax, dword ptr [ebp + 8]
00531c17 50                       push       eax
00531c18 8d4dff                   lea        ecx, [ebp - 1]
00531c1b e810010000               call       0x531d30
00531c20 8be5                     mov        esp, ebp
00531c22 5d                       pop        ebp
00531c23 c3                       ret        
00531c24 cc                       int3       
00531c25 cc                       int3       
00531c26 cc                       int3       
00531c27 cc                       int3       
00531c28 cc                       int3       
00531c29 cc                       int3       
00531c2a cc                       int3       
00531c2b cc                       int3       
00531c2c cc                       int3       
00531c2d cc                       int3       
00531c2e cc                       int3       
00531c2f cc                       int3       

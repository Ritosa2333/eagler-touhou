00422c00 55                       push       ebp
00422c01 8bec                     mov        ebp, esp
00422c03 8b4508                   mov        eax, dword ptr [ebp + 8]
00422c06 33d2                     xor        edx, edx
00422c08 b9ffffff7f               mov        ecx, 0x7fffffff
00422c0d f7f1                     div        ecx
00422c0f 895508                   mov        dword ptr [ebp + 8], edx
00422c12 837d0800                 cmp        dword ptr [ebp + 8], 0
00422c16 7507                     jne        0x422c1f
00422c18 c7450801000000           mov        dword ptr [ebp + 8], 1
00422c1f 8b4508                   mov        eax, dword ptr [ebp + 8]
00422c22 5d                       pop        ebp
00422c23 c3                       ret        
00422c24 cc                       int3       
00422c25 cc                       int3       
00422c26 cc                       int3       
00422c27 cc                       int3       
00422c28 cc                       int3       
00422c29 cc                       int3       
00422c2a cc                       int3       
00422c2b cc                       int3       
00422c2c cc                       int3       
00422c2d cc                       int3       
00422c2e cc                       int3       
00422c2f cc                       int3       

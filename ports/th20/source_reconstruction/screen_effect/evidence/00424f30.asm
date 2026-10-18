00424f30 55                       push       ebp
00424f31 8bec                     mov        ebp, esp
00424f33 83ec14                   sub        esp, 0x14
00424f36 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00424f3b 33c5                     xor        eax, ebp
00424f3d 8945fc                   mov        dword ptr [ebp - 4], eax
00424f40 0f57c0                   xorps      xmm0, xmm0
00424f43 f30f1145ec               movss      dword ptr [ebp - 0x14], xmm0
00424f48 0f57c0                   xorps      xmm0, xmm0
00424f4b f30f1145f0               movss      dword ptr [ebp - 0x10], xmm0
00424f50 f30f2a05f8875b00         cvtsi2ss   xmm0, dword ptr [0x5b87f8]
00424f58 f30f1145f4               movss      dword ptr [ebp - 0xc], xmm0
00424f5d f30f2a05fc875b00         cvtsi2ss   xmm0, dword ptr [0x5b87fc]
00424f65 f30f1145f8               movss      dword ptr [ebp - 8], xmm0
00424f6a 8b4508                   mov        eax, dword ptr [ebp + 8]
00424f6d 8b4818                   mov        ecx, dword ptr [eax + 0x18]
00424f70 c1e118                   shl        ecx, 0x18
00424f73 8b5508                   mov        edx, dword ptr [ebp + 8]
00424f76 0b4a20                   or         ecx, dword ptr [edx + 0x20]
00424f79 51                       push       ecx
00424f7a 8d45ec                   lea        eax, [ebp - 0x14]
00424f7d 50                       push       eax
00424f7e e8cde6ffff               call       0x423650
00424f83 83c408                   add        esp, 8
00424f86 b801000000               mov        eax, 1
00424f8b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00424f8e 33cd                     xor        ecx, ebp
00424f90 e859da1100               call       0x5429ee
00424f95 8be5                     mov        esp, ebp
00424f97 5d                       pop        ebp
00424f98 c3                       ret        
00424f99 cc                       int3       
00424f9a cc                       int3       
00424f9b cc                       int3       
00424f9c cc                       int3       
00424f9d cc                       int3       
00424f9e cc                       int3       
00424f9f cc                       int3       

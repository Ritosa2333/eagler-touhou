00506f10 55                       push       ebp
00506f11 8bec                     mov        ebp, esp
00506f13 83ec0c                   sub        esp, 0xc
00506f16 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00506f1b 33c5                     xor        eax, ebp
00506f1d 8945fc                   mov        dword ptr [ebp - 4], eax
00506f20 894df4                   mov        dword ptr [ebp - 0xc], ecx
00506f23 837d0800                 cmp        dword ptr [ebp + 8], 0
00506f27 7502                     jne        0x506f2b
00506f29 eb36                     jmp        0x506f61
00506f2b 8b4508                   mov        eax, dword ptr [ebp + 8]
00506f2e 50                       push       eax
00506f2f e8ec020000               call       0x507220
00506f34 83c404                   add        esp, 4
00506f37 6a01                     push       1
00506f39 b940025c00               mov        ecx, 0x5c0240
00506f3e e8cd53f0ff               call       0x40c310
00506f43 50                       push       eax
00506f44 8d4df8                   lea        ecx, [ebp - 8]
00506f47 e8e447f0ff               call       0x40b730
00506f4c 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00506f4f 51                       push       ecx
00506f50 e8a7ba0300               call       0x5429fc
00506f55 83c404                   add        esp, 4
00506f58 8d4df8                   lea        ecx, [ebp - 8]
00506f5b e8a049f0ff               call       0x40b900
00506f60 90                       nop        
00506f61 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00506f64 33cd                     xor        ecx, ebp
00506f66 e883ba0300               call       0x5429ee
00506f6b 8be5                     mov        esp, ebp
00506f6d 5d                       pop        ebp
00506f6e c20400                   ret        4
00506f71 cc                       int3       
00506f72 cc                       int3       
00506f73 cc                       int3       
00506f74 cc                       int3       
00506f75 cc                       int3       
00506f76 cc                       int3       
00506f77 cc                       int3       
00506f78 cc                       int3       
00506f79 cc                       int3       
00506f7a cc                       int3       
00506f7b cc                       int3       
00506f7c cc                       int3       
00506f7d cc                       int3       
00506f7e cc                       int3       
00506f7f cc                       int3       

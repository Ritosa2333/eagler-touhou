00535370 55                       push       ebp
00535371 8bec                     mov        ebp, esp
00535373 83ec0c                   sub        esp, 0xc
00535376 894dfc                   mov        dword ptr [ebp - 4], ecx
00535379 e8a2b1ffff               call       0x530520
0053537e 0fb6c0                   movzx      eax, al
00535381 85c0                     test       eax, eax
00535383 753a                     jne        0x5353bf
00535385 6a00                     push       0
00535387 e8a4b4f2ff               call       0x460830
0053538c 83c404                   add        esp, 4
0053538f 8bc8                     mov        ecx, eax
00535391 e81aa3fcff               call       0x4ff6b0
00535396 8945f4                   mov        dword ptr [ebp - 0xc], eax
00535399 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053539c 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
005353a0 8b4510                   mov        eax, dword ptr [ebp + 0x10]
005353a3 8d4c0205                 lea        ecx, [edx + eax + 5]
005353a7 894df8                   mov        dword ptr [ebp - 8], ecx
005353aa 8b55f8                   mov        edx, dword ptr [ebp - 8]
005353ad 52                       push       edx
005353ae 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005353b1 50                       push       eax
005353b2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005353b5 51                       push       ecx
005353b6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005353b9 e882f9fcff               call       0x504d40
005353be 90                       nop        
005353bf 8b5510                   mov        edx, dword ptr [ebp + 0x10]
005353c2 52                       push       edx
005353c3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005353c6 e815000000               call       0x5353e0
005353cb 90                       nop        
005353cc 8be5                     mov        esp, ebp
005353ce 5d                       pop        ebp
005353cf c20c00                   ret        0xc
005353d2 cc                       int3       
005353d3 cc                       int3       
005353d4 cc                       int3       
005353d5 cc                       int3       
005353d6 cc                       int3       
005353d7 cc                       int3       
005353d8 cc                       int3       
005353d9 cc                       int3       
005353da cc                       int3       
005353db cc                       int3       
005353dc cc                       int3       
005353dd cc                       int3       
005353de cc                       int3       
005353df cc                       int3       

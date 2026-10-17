005365a0 55                       push       ebp
005365a1 8bec                     mov        ebp, esp
005365a3 83ec0c                   sub        esp, 0xc
005365a6 894dfc                   mov        dword ptr [ebp - 4], ecx
005365a9 e8729fffff               call       0x530520
005365ae 0fb6c0                   movzx      eax, al
005365b1 85c0                     test       eax, eax
005365b3 753a                     jne        0x5365ef
005365b5 6a00                     push       0
005365b7 e874a2f2ff               call       0x460830
005365bc 83c404                   add        esp, 4
005365bf 8bc8                     mov        ecx, eax
005365c1 e8ea90fcff               call       0x4ff6b0
005365c6 8945f4                   mov        dword ptr [ebp - 0xc], eax
005365c9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005365cc 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
005365d0 8b4510                   mov        eax, dword ptr [ebp + 0x10]
005365d3 8d4c0205                 lea        ecx, [edx + eax + 5]
005365d7 894df8                   mov        dword ptr [ebp - 8], ecx
005365da 8b55f8                   mov        edx, dword ptr [ebp - 8]
005365dd 52                       push       edx
005365de 8b450c                   mov        eax, dword ptr [ebp + 0xc]
005365e1 50                       push       eax
005365e2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005365e5 51                       push       ecx
005365e6 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
005365e9 e852e7fcff               call       0x504d40
005365ee 90                       nop        
005365ef 8b5510                   mov        edx, dword ptr [ebp + 0x10]
005365f2 52                       push       edx
005365f3 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005365f6 e815000000               call       0x536610
005365fb 90                       nop        
005365fc 8be5                     mov        esp, ebp
005365fe 5d                       pop        ebp
005365ff c20c00                   ret        0xc
00536602 cc                       int3       
00536603 cc                       int3       
00536604 cc                       int3       
00536605 cc                       int3       
00536606 cc                       int3       
00536607 cc                       int3       
00536608 cc                       int3       
00536609 cc                       int3       
0053660a cc                       int3       
0053660b cc                       int3       
0053660c cc                       int3       
0053660d cc                       int3       
0053660e cc                       int3       
0053660f cc                       int3       

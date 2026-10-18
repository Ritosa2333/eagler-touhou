005376c0 55                       push       ebp
005376c1 8bec                     mov        ebp, esp
005376c3 83ec0c                   sub        esp, 0xc
005376c6 894dfc                   mov        dword ptr [ebp - 4], ecx
005376c9 e8528effff               call       0x530520
005376ce 0fb6c0                   movzx      eax, al
005376d1 85c0                     test       eax, eax
005376d3 753a                     jne        0x53770f
005376d5 6a00                     push       0
005376d7 e85491f2ff               call       0x460830
005376dc 83c404                   add        esp, 4
005376df 8bc8                     mov        ecx, eax
005376e1 e8ca7ffcff               call       0x4ff6b0
005376e6 8945f4                   mov        dword ptr [ebp - 0xc], eax
005376e9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005376ec 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
005376f0 8b4510                   mov        eax, dword ptr [ebp + 0x10]
005376f3 8d4c020a                 lea        ecx, [edx + eax + 0xa]
005376f7 894df8                   mov        dword ptr [ebp - 8], ecx
005376fa 8b55f8                   mov        edx, dword ptr [ebp - 8]
005376fd 52                       push       edx
005376fe 8b450c                   mov        eax, dword ptr [ebp + 0xc]
00537701 50                       push       eax
00537702 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00537705 51                       push       ecx
00537706 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00537709 e832d6fcff               call       0x504d40
0053770e 90                       nop        
0053770f 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00537712 52                       push       edx
00537713 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537716 e885030000               call       0x537aa0
0053771b 90                       nop        
0053771c 8be5                     mov        esp, ebp
0053771e 5d                       pop        ebp
0053771f c20c00                   ret        0xc
00537722 cc                       int3       
00537723 cc                       int3       
00537724 cc                       int3       
00537725 cc                       int3       
00537726 cc                       int3       
00537727 cc                       int3       
00537728 cc                       int3       
00537729 cc                       int3       
0053772a cc                       int3       
0053772b cc                       int3       
0053772c cc                       int3       
0053772d cc                       int3       
0053772e cc                       int3       
0053772f cc                       int3       

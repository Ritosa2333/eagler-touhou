00451580 55                       push       ebp
00451581 8bec                     mov        ebp, esp
00451583 83ec1c                   sub        esp, 0x1c
00451586 894df8                   mov        dword ptr [ebp - 8], ecx
00451589 c745fc00000000           mov        dword ptr [ebp - 4], 0
00451590 8b45f8                   mov        eax, dword ptr [ebp - 8]
00451593 8945f4                   mov        dword ptr [ebp - 0xc], eax
00451596 6a00                     push       0
00451598 8d4dec                   lea        ecx, [ebp - 0x14]
0045159b 51                       push       ecx
0045159c 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045159f e81cd2fbff               call       0x40e7c0
004515a4 8b5004                   mov        edx, dword ptr [eax + 4]
004515a7 52                       push       edx
004515a8 8b00                     mov        eax, dword ptr [eax]
004515aa 50                       push       eax
004515ab 8d4de4                   lea        ecx, [ebp - 0x1c]
004515ae 51                       push       ecx
004515af e8dca0ffff               call       0x44b690
004515b4 83c40c                   add        esp, 0xc
004515b7 50                       push       eax
004515b8 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004515bb e80068fcff               call       0x417dc0
004515c0 8b55fc                   mov        edx, dword ptr [ebp - 4]
004515c3 83ca01                   or         edx, 1
004515c6 8955fc                   mov        dword ptr [ebp - 4], edx
004515c9 8b4508                   mov        eax, dword ptr [ebp + 8]
004515cc 8be5                     mov        esp, ebp
004515ce 5d                       pop        ebp
004515cf c20400                   ret        4
004515d2 cc                       int3       
004515d3 cc                       int3       
004515d4 cc                       int3       
004515d5 cc                       int3       
004515d6 cc                       int3       
004515d7 cc                       int3       
004515d8 cc                       int3       
004515d9 cc                       int3       
004515da cc                       int3       
004515db cc                       int3       
004515dc cc                       int3       
004515dd cc                       int3       
004515de cc                       int3       
004515df cc                       int3       

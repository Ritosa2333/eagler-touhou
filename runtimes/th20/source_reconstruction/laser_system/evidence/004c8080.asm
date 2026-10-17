004c8080 55                       push       ebp
004c8081 8bec                     mov        ebp, esp
004c8083 83ec0c                   sub        esp, 0xc
004c8086 894df4                   mov        dword ptr [ebp - 0xc], ecx
004c8089 6870130000               push       0x1370
004c808e e8caa60700               call       0x54275d
004c8093 83c404                   add        esp, 4
004c8096 8945fc                   mov        dword ptr [ebp - 4], eax
004c8099 837dfc00                 cmp        dword ptr [ebp - 4], 0
004c809d 742d                     je         0x4c80cc
004c809f 6870130000               push       0x1370
004c80a4 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c80a7 e8d43ff4ff               call       0x40c080
004c80ac 6870130000               push       0x1370
004c80b1 6a00                     push       0
004c80b3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c80b6 50                       push       eax
004c80b7 e8a4c60700               call       0x544760
004c80bc 83c40c                   add        esp, 0xc
004c80bf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c80c2 e849030000               call       0x4c8410
004c80c7 8945f8                   mov        dword ptr [ebp - 8], eax
004c80ca eb07                     jmp        0x4c80d3
004c80cc c745f800000000           mov        dword ptr [ebp - 8], 0
004c80d3 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c80d6 8be5                     mov        esp, ebp
004c80d8 5d                       pop        ebp
004c80d9 c20400                   ret        4
004c80dc cc                       int3       
004c80dd cc                       int3       
004c80de cc                       int3       
004c80df cc                       int3       

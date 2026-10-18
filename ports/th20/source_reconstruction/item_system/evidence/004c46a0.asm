004c46a0 55                       push       ebp
004c46a1 8bec                     mov        ebp, esp
004c46a3 83ec0c                   sub        esp, 0xc
004c46a6 894dfc                   mov        dword ptr [ebp - 4], ecx
004c46a9 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c46ac 8b483c                   mov        ecx, dword ptr [eax + 0x3c]
004c46af 034d08                   add        ecx, dword ptr [ebp + 8]
004c46b2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c46b5 894a3c                   mov        dword ptr [edx + 0x3c], ecx
004c46b8 c745f840420f00           mov        dword ptr [ebp - 8], 0xf4240
004c46bf c745f400000000           mov        dword ptr [ebp - 0xc], 0
004c46c6 8d45f8                   lea        eax, [ebp - 8]
004c46c9 50                       push       eax
004c46ca 8d4df4                   lea        ecx, [ebp - 0xc]
004c46cd 51                       push       ecx
004c46ce 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c46d1 83c23c                   add        edx, 0x3c
004c46d4 52                       push       edx
004c46d5 e896cafaff               call       0x471170
004c46da 83c40c                   add        esp, 0xc
004c46dd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c46e0 8b10                     mov        edx, dword ptr [eax]
004c46e2 89513c                   mov        dword ptr [ecx + 0x3c], edx
004c46e5 8be5                     mov        esp, ebp
004c46e7 5d                       pop        ebp
004c46e8 c20400                   ret        4
004c46eb cc                       int3       
004c46ec cc                       int3       
004c46ed cc                       int3       
004c46ee cc                       int3       
004c46ef cc                       int3       

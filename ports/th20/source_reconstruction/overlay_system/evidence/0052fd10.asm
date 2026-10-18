0052fd10 55                       push       ebp
0052fd11 8bec                     mov        ebp, esp
0052fd13 83ec0c                   sub        esp, 0xc
0052fd16 894dfc                   mov        dword ptr [ebp - 4], ecx
0052fd19 e802080000               call       0x530520
0052fd1e 0fb6c0                   movzx      eax, al
0052fd21 85c0                     test       eax, eax
0052fd23 753a                     jne        0x52fd5f
0052fd25 6a00                     push       0
0052fd27 e8040bf3ff               call       0x460830
0052fd2c 83c404                   add        esp, 4
0052fd2f 8bc8                     mov        ecx, eax
0052fd31 e87af9fcff               call       0x4ff6b0
0052fd36 8945f4                   mov        dword ptr [ebp - 0xc], eax
0052fd39 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052fd3c 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
0052fd40 8b4510                   mov        eax, dword ptr [ebp + 0x10]
0052fd43 8d4c020a                 lea        ecx, [edx + eax + 0xa]
0052fd47 894df8                   mov        dword ptr [ebp - 8], ecx
0052fd4a 8b55f8                   mov        edx, dword ptr [ebp - 8]
0052fd4d 52                       push       edx
0052fd4e 8b450c                   mov        eax, dword ptr [ebp + 0xc]
0052fd51 50                       push       eax
0052fd52 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0052fd55 51                       push       ecx
0052fd56 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052fd59 e8e24ffdff               call       0x504d40
0052fd5e 90                       nop        
0052fd5f 8be5                     mov        esp, ebp
0052fd61 5d                       pop        ebp
0052fd62 c20c00                   ret        0xc
0052fd65 cc                       int3       
0052fd66 cc                       int3       
0052fd67 cc                       int3       
0052fd68 cc                       int3       
0052fd69 cc                       int3       
0052fd6a cc                       int3       
0052fd6b cc                       int3       
0052fd6c cc                       int3       
0052fd6d cc                       int3       
0052fd6e cc                       int3       
0052fd6f cc                       int3       

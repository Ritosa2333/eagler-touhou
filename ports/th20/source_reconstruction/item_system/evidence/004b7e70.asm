004b7e70 55                       push       ebp
004b7e71 8bec                     mov        ebp, esp
004b7e73 83ec0c                   sub        esp, 0xc
004b7e76 894dfc                   mov        dword ptr [ebp - 4], ecx
004b7e79 c745f80a000000           mov        dword ptr [ebp - 8], 0xa
004b7e80 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004b7e87 8d45f8                   lea        eax, [ebp - 8]
004b7e8a 50                       push       eax
004b7e8b 8d4df4                   lea        ecx, [ebp - 0xc]
004b7e8e 51                       push       ecx
004b7e8f 8b55fc                   mov        edx, dword ptr [ebp - 4]
004b7e92 81c2d0000000             add        edx, 0xd0
004b7e98 52                       push       edx
004b7e99 e8d292fbff               call       0x471170
004b7e9e 83c40c                   add        esp, 0xc
004b7ea1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004b7ea4 8b10                     mov        edx, dword ptr [eax]
004b7ea6 8991d0000000             mov        dword ptr [ecx + 0xd0], edx
004b7eac 8b45fc                   mov        eax, dword ptr [ebp - 4]
004b7eaf 8b80d0000000             mov        eax, dword ptr [eax + 0xd0]
004b7eb5 8be5                     mov        esp, ebp
004b7eb7 5d                       pop        ebp
004b7eb8 c3                       ret        
004b7eb9 cc                       int3       
004b7eba cc                       int3       
004b7ebb cc                       int3       
004b7ebc cc                       int3       
004b7ebd cc                       int3       
004b7ebe cc                       int3       
004b7ebf cc                       int3       

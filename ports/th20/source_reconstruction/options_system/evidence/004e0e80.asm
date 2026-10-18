004e0e80 55                       push       ebp
004e0e81 8bec                     mov        ebp, esp
004e0e83 83ec08                   sub        esp, 8
004e0e86 894dfc                   mov        dword ptr [ebp - 4], ecx
004e0e89 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e0e8c 50                       push       eax
004e0e8d 68600f4e00               push       0x4e0f60
004e0e92 6a0a                     push       0xa
004e0e94 e82714f3ff               call       0x4122c0
004e0e99 83c40c                   add        esp, 0xc
004e0e9c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0e9f 894108                   mov        dword ptr [ecx + 8], eax
004e0ea2 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e0ea5 52                       push       edx
004e0ea6 68700f4e00               push       0x4e0f70
004e0eab 6a5e                     push       0x5e
004e0ead e8ae14f3ff               call       0x412360
004e0eb2 83c40c                   add        esp, 0xc
004e0eb5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0eb8 89410c                   mov        dword ptr [ecx + 0xc], eax
004e0ebb 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e0ebe 8b02                     mov        eax, dword ptr [edx]
004e0ec0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0ec3 8b5004                   mov        edx, dword ptr [eax + 4]
004e0ec6 ffd2                     call       edx
004e0ec8 6a00                     push       0
004e0eca 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0ecd 83c114                   add        ecx, 0x14
004e0ed0 e87bdefdff               call       0x4bed50
004e0ed5 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e0ed8 83c014                   add        eax, 0x14
004e0edb 8945f8                   mov        dword ptr [ebp - 8], eax
004e0ede 6a01                     push       1
004e0ee0 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004e0ee3 e8f8ecfdff               call       0x4bfbe0
004e0ee8 6a00                     push       0
004e0eea 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0eed 83c160                   add        ecx, 0x60
004e0ef0 e82b26f4ff               call       0x423520
004e0ef5 6a00                     push       0
004e0ef7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0efa 83c17c                   add        ecx, 0x7c
004e0efd e81e26f4ff               call       0x423520
004e0f02 6a00                     push       0
004e0f04 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0f07 81c18c000000             add        ecx, 0x8c
004e0f0d e80e26f4ff               call       0x423520
004e0f12 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e0f15 83c170                   add        ecx, 0x70
004e0f18 8b5508                   mov        edx, dword ptr [ebp + 8]
004e0f1b 8b02                     mov        eax, dword ptr [edx]
004e0f1d 8901                     mov        dword ptr [ecx], eax
004e0f1f 8b4204                   mov        eax, dword ptr [edx + 4]
004e0f22 894104                   mov        dword ptr [ecx + 4], eax
004e0f25 8b5208                   mov        edx, dword ptr [edx + 8]
004e0f28 895108                   mov        dword ptr [ecx + 8], edx
004e0f2b 33c0                     xor        eax, eax
004e0f2d 8be5                     mov        esp, ebp
004e0f2f 5d                       pop        ebp
004e0f30 c20400                   ret        4
004e0f33 cc                       int3       
004e0f34 cc                       int3       
004e0f35 cc                       int3       
004e0f36 cc                       int3       
004e0f37 cc                       int3       
004e0f38 cc                       int3       
004e0f39 cc                       int3       
004e0f3a cc                       int3       
004e0f3b cc                       int3       
004e0f3c cc                       int3       
004e0f3d cc                       int3       
004e0f3e cc                       int3       
004e0f3f cc                       int3       

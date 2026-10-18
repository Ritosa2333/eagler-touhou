004c0c60 55                       push       ebp
004c0c61 8bec                     mov        ebp, esp
004c0c63 83ec08                   sub        esp, 8
004c0c66 894df8                   mov        dword ptr [ebp - 8], ecx
004c0c69 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c0c6c 8b8810c40000             mov        ecx, dword ptr [eax + 0xc410]
004c0c72 83c101                   add        ecx, 1
004c0c75 81e1ffff0000             and        ecx, 0xffff
004c0c7b 894dfc                   mov        dword ptr [ebp - 4], ecx
004c0c7e 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c0c81 81e2ffff0000             and        edx, 0xffff
004c0c87 8955fc                   mov        dword ptr [ebp - 4], edx
004c0c8a 7507                     jne        0x4c0c93
004c0c8c c745fc01000000           mov        dword ptr [ebp - 4], 1
004c0c93 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c0c96 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0c99 898810c40000             mov        dword ptr [eax + 0xc410], ecx
004c0c9f 8be5                     mov        esp, ebp
004c0ca1 5d                       pop        ebp
004c0ca2 c3                       ret        
004c0ca3 cc                       int3       
004c0ca4 cc                       int3       
004c0ca5 cc                       int3       
004c0ca6 cc                       int3       
004c0ca7 cc                       int3       
004c0ca8 cc                       int3       
004c0ca9 cc                       int3       
004c0caa cc                       int3       
004c0cab cc                       int3       
004c0cac cc                       int3       
004c0cad cc                       int3       
004c0cae cc                       int3       
004c0caf cc                       int3       

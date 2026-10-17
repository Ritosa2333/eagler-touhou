0047dc90 55                       push       ebp
0047dc91 8bec                     mov        ebp, esp
0047dc93 83ec08                   sub        esp, 8
0047dc96 894df8                   mov        dword ptr [ebp - 8], ecx
0047dc99 8b45f8                   mov        eax, dword ptr [ebp - 8]
0047dc9c 8b889c6d2800             mov        ecx, dword ptr [eax + 0x286d9c]
0047dca2 51                       push       ecx
0047dca3 6a0c                     push       0xc
0047dca5 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0047dcab e82017fdff               call       0x44f3d0
0047dcb0 90                       nop        
0047dcb1 c745fc00000000           mov        dword ptr [ebp - 4], 0
0047dcb8 eb09                     jmp        0x47dcc3
0047dcba 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047dcbd 83c201                   add        edx, 1
0047dcc0 8955fc                   mov        dword ptr [ebp - 4], edx
0047dcc3 837dfc06                 cmp        dword ptr [ebp - 4], 6
0047dcc7 7d0f                     jge        0x47dcd8
0047dcc9 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047dccc 50                       push       eax
0047dccd 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047dcd0 e89bfeffff               call       0x47db70
0047dcd5 90                       nop        
0047dcd6 ebe2                     jmp        0x47dcba
0047dcd8 b801000000               mov        eax, 1
0047dcdd 8be5                     mov        esp, ebp
0047dcdf 5d                       pop        ebp
0047dce0 c3                       ret        
0047dce1 cc                       int3       
0047dce2 cc                       int3       
0047dce3 cc                       int3       
0047dce4 cc                       int3       
0047dce5 cc                       int3       
0047dce6 cc                       int3       
0047dce7 cc                       int3       
0047dce8 cc                       int3       
0047dce9 cc                       int3       
0047dcea cc                       int3       
0047dceb cc                       int3       
0047dcec cc                       int3       
0047dced cc                       int3       
0047dcee cc                       int3       
0047dcef cc                       int3       

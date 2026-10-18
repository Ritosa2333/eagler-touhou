00460180 55                       push       ebp
00460181 8bec                     mov        ebp, esp
00460183 51                       push       ecx
00460184 894dfc                   mov        dword ptr [ebp - 4], ecx
00460187 8b4508                   mov        eax, dword ptr [ebp + 8]
0046018a 50                       push       eax
0046018b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046018e e8cd69feff               call       0x446b60
00460193 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00460196 c701c0ee5600             mov        dword ptr [ecx], 0x56eec0
0046019c 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0046019f 83c108                   add        ecx, 8
004601a2 e889ffffff               call       0x460130
004601a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004601aa 81c188020000             add        ecx, 0x288
004601b0 e82bffffff               call       0x4600e0
004601b5 8b55fc                   mov        edx, dword ptr [ebp - 4]
004601b8 0f57c0                   xorps      xmm0, xmm0
004601bb f30f1182c8030000         movss      dword ptr [edx + 0x3c8], xmm0
004601c3 8b45fc                   mov        eax, dword ptr [ebp - 4]
004601c6 0f57c0                   xorps      xmm0, xmm0
004601c9 f30f1180cc030000         movss      dword ptr [eax + 0x3cc], xmm0
004601d1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004601d4 81c1d0030000             add        ecx, 0x3d0
004601da e8b12bfcff               call       0x422d90
004601df 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004601e2 c781e003000000000000     mov        dword ptr [ecx + 0x3e0], 0
004601ec 8b45fc                   mov        eax, dword ptr [ebp - 4]
004601ef 8be5                     mov        esp, ebp
004601f1 5d                       pop        ebp
004601f2 c20400                   ret        4
004601f5 cc                       int3       
004601f6 cc                       int3       
004601f7 cc                       int3       
004601f8 cc                       int3       
004601f9 cc                       int3       
004601fa cc                       int3       
004601fb cc                       int3       
004601fc cc                       int3       
004601fd cc                       int3       
004601fe cc                       int3       
004601ff cc                       int3       

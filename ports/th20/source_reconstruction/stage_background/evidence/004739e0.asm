004739e0 55                       push       ebp
004739e1 8bec                     mov        ebp, esp
004739e3 83ec18                   sub        esp, 0x18
004739e6 894df0                   mov        dword ptr [ebp - 0x10], ecx
004739e9 c745f400000000           mov        dword ptr [ebp - 0xc], 0
004739f0 eb09                     jmp        0x4739fb
004739f2 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
004739f5 83c001                   add        eax, 1
004739f8 8945f4                   mov        dword ptr [ebp - 0xc], eax
004739fb 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004739fe 8b9124330000             mov        edx, dword ptr [ecx + 0x3324]
00473a04 0fbf02                   movsx      eax, word ptr [edx]
00473a07 3945f4                   cmp        dword ptr [ebp - 0xc], eax
00473a0a 0f8dfb000000             jge        0x473b0b
00473a10 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00473a13 8b9128330000             mov        edx, dword ptr [ecx + 0x3328]
00473a19 8b45f4                   mov        eax, dword ptr [ebp - 0xc]
00473a1c 8b0c82                   mov        ecx, dword ptr [edx + eax*4]
00473a1f 894dfc                   mov        dword ptr [ebp - 4], ecx
00473a22 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473a25 0fbe4203                 movsx      eax, byte ptr [edx + 3]
00473a29 83e001                   and        eax, 1
00473a2c 0f848a000000             je         0x473abc
00473a32 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00473a39 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00473a3c 83c130                   add        ecx, 0x30
00473a3f 894df8                   mov        dword ptr [ebp - 8], ecx
00473a42 8b55f8                   mov        edx, dword ptr [ebp - 8]
00473a45 0fbf02                   movsx      eax, word ptr [edx]
00473a48 85c0                     test       eax, eax
00473a4a 7c5a                     jl         0x473aa6
00473a4c 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00473a4f 0fbf5106                 movsx      edx, word ptr [ecx + 6]
00473a53 69c2e4050000             imul       eax, edx, 0x5e4
00473a59 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00473a5c 038120330000             add        eax, dword ptr [ecx + 0x3320]
00473a62 8945e8                   mov        dword ptr [ebp - 0x18], eax
00473a65 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00473a68 52                       push       edx
00473a69 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00473a6f e88c1afcff               call       0x435500
00473a74 90                       nop        
00473a75 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00473a78 e853130000               call       0x474dd0
00473a7d 0fb6c0                   movzx      eax, al
00473a80 85c0                     test       eax, eax
00473a82 7409                     je         0x473a8d
00473a84 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00473a87 83c101                   add        ecx, 1
00473a8a 894dec                   mov        dword ptr [ebp - 0x14], ecx
00473a8d 8b55f8                   mov        edx, dword ptr [ebp - 8]
00473a90 0fbf4202                 movsx      eax, word ptr [edx + 2]
00473a94 50                       push       eax
00473a95 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00473a98 51                       push       ecx
00473a99 e8c2d6ffff               call       0x471160
00473a9e 83c408                   add        esp, 8
00473aa1 8945f8                   mov        dword ptr [ebp - 8], eax
00473aa4 eb9c                     jmp        0x473a42
00473aa6 837dec00                 cmp        dword ptr [ebp - 0x14], 0
00473aaa 7510                     jne        0x473abc
00473aac 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473aaf 0fbe4203                 movsx      eax, byte ptr [edx + 3]
00473ab3 83e0fe                   and        eax, 0xfffffffe
00473ab6 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00473ab9 884103                   mov        byte ptr [ecx + 3], al
00473abc 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473abf f30f104208               movss      xmm0, dword ptr [edx + 8]
00473ac4 0f2e05e8e05600           ucomiss    xmm0, dword ptr [0x56e0e8]
00473acb 9f                       lahf       
00473acc f6c444                   test       ah, 0x44
00473acf 7b35                     jnp        0x473b06
00473ad1 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00473ad4 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00473ada c1e904                   shr        ecx, 4
00473add 83e101                   and        ecx, 1
00473ae0 7524                     jne        0x473b06
00473ae2 8b55fc                   mov        edx, dword ptr [ebp - 4]
00473ae5 8b45fc                   mov        eax, dword ptr [ebp - 4]
00473ae8 f30f10420c               movss      xmm0, dword ptr [edx + 0xc]
00473aed f30f584008               addss      xmm0, dword ptr [eax + 8]
00473af2 51                       push       ecx
00473af3 f30f110424               movss      dword ptr [esp], xmm0
00473af8 e8434afcff               call       0x438540
00473afd 83c404                   add        esp, 4
00473b00 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00473b03 d9590c                   fstp       dword ptr [ecx + 0xc]
00473b06 e9e7feffff               jmp        0x4739f2
00473b0b 33c0                     xor        eax, eax
00473b0d 8be5                     mov        esp, ebp
00473b0f 5d                       pop        ebp
00473b10 c3                       ret        
00473b11 cc                       int3       
00473b12 cc                       int3       
00473b13 cc                       int3       
00473b14 cc                       int3       
00473b15 cc                       int3       
00473b16 cc                       int3       
00473b17 cc                       int3       
00473b18 cc                       int3       
00473b19 cc                       int3       
00473b1a cc                       int3       
00473b1b cc                       int3       
00473b1c cc                       int3       
00473b1d cc                       int3       
00473b1e cc                       int3       
00473b1f cc                       int3       

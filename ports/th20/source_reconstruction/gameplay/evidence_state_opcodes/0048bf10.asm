0048bf10 55                       push       ebp
0048bf11 8bec                     mov        ebp, esp
0048bf13 83ec14                   sub        esp, 0x14
0048bf16 894dfc                   mov        dword ptr [ebp - 4], ecx
0048bf19 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048bf1c 83780800                 cmp        dword ptr [eax + 8], 0
0048bf20 7461                     je         0x48bf83
0048bf22 0fb64d0c                 movzx      ecx, byte ptr [ebp + 0xc]
0048bf26 85c9                     test       ecx, ecx
0048bf28 7559                     jne        0x48bf83
0048bf2a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048bf2d 8b02                     mov        eax, dword ptr [edx]
0048bf2f 50                       push       eax
0048bf30 e89bd00000               call       0x498fd0
0048bf35 83c404                   add        esp, 4
0048bf38 8945f4                   mov        dword ptr [ebp - 0xc], eax
0048bf3b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048bf3e 8b5108                   mov        edx, dword ptr [ecx + 8]
0048bf41 8955f8                   mov        dword ptr [ebp - 8], edx
0048bf44 6aff                     push       -1
0048bf46 6a00                     push       0
0048bf48 6a00                     push       0
0048bf4a 51                       push       ecx
0048bf4b f30f100534fd5600         movss      xmm0, dword ptr [0x56fd34]
0048bf53 f30f110424               movss      dword ptr [esp], xmm0
0048bf58 f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
0048bf60 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048bf68 51                       push       ecx
0048bf69 f30f110424               movss      dword ptr [esp], xmm0
0048bf6e 6aff                     push       -1
0048bf70 8b4508                   mov        eax, dword ptr [ebp + 8]
0048bf73 50                       push       eax
0048bf74 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0048bf77 51                       push       ecx
0048bf78 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0048bf7b e8107d0300               call       0x4c3c90
0048bf80 90                       nop        
0048bf81 eb60                     jmp        0x48bfe3
0048bf83 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048bf86 837a0400                 cmp        dword ptr [edx + 4], 0
0048bf8a 7457                     je         0x48bfe3
0048bf8c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0048bf8f 8b08                     mov        ecx, dword ptr [eax]
0048bf91 51                       push       ecx
0048bf92 e839d00000               call       0x498fd0
0048bf97 83c404                   add        esp, 4
0048bf9a 8945ec                   mov        dword ptr [ebp - 0x14], eax
0048bf9d 8b55fc                   mov        edx, dword ptr [ebp - 4]
0048bfa0 8b4204                   mov        eax, dword ptr [edx + 4]
0048bfa3 8945f0                   mov        dword ptr [ebp - 0x10], eax
0048bfa6 6aff                     push       -1
0048bfa8 6a00                     push       0
0048bfaa 6a00                     push       0
0048bfac 51                       push       ecx
0048bfad f30f100534fd5600         movss      xmm0, dword ptr [0x56fd34]
0048bfb5 f30f110424               movss      dword ptr [esp], xmm0
0048bfba f30f1005f8e05600         movss      xmm0, dword ptr [0x56e0f8]
0048bfc2 f30f5e05d0c85600         divss      xmm0, dword ptr [0x56c8d0]
0048bfca 51                       push       ecx
0048bfcb f30f110424               movss      dword ptr [esp], xmm0
0048bfd0 6aff                     push       -1
0048bfd2 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0048bfd5 51                       push       ecx
0048bfd6 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0048bfd9 52                       push       edx
0048bfda 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0048bfdd e8ae7c0300               call       0x4c3c90
0048bfe2 90                       nop        
0048bfe3 8b4508                   mov        eax, dword ptr [ebp + 8]
0048bfe6 50                       push       eax
0048bfe7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048bfea e861930100               call       0x4a5350
0048bfef 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0048bff2 c7410400000000           mov        dword ptr [ecx + 4], 0
0048bff9 8be5                     mov        esp, ebp
0048bffb 5d                       pop        ebp
0048bffc c20800                   ret        8
0048bfff cc                       int3       
0048c000 cc                       int3       
0048c001 cc                       int3       
0048c002 cc                       int3       
0048c003 cc                       int3       
0048c004 cc                       int3       
0048c005 cc                       int3       
0048c006 cc                       int3       
0048c007 cc                       int3       
0048c008 cc                       int3       
0048c009 cc                       int3       
0048c00a cc                       int3       
0048c00b cc                       int3       
0048c00c cc                       int3       
0048c00d cc                       int3       
0048c00e cc                       int3       
0048c00f cc                       int3       

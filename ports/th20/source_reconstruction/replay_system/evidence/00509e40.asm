00509e40 55                       push       ebp
00509e41 8bec                     mov        ebp, esp
00509e43 83ec0c                   sub        esp, 0xc
00509e46 894dfc                   mov        dword ptr [ebp - 4], ecx
00509e49 8b45fc                   mov        eax, dword ptr [ebp - 4]
00509e4c 8b8860d80300             mov        ecx, dword ptr [eax + 0x3d860]
00509e52 668b5508                 mov        dx, word ptr [ebp + 8]
00509e56 668911                   mov        word ptr [ecx], dx
00509e59 8b45fc                   mov        eax, dword ptr [ebp - 4]
00509e5c 8b8860d80300             mov        ecx, dword ptr [eax + 0x3d860]
00509e62 668b550c                 mov        dx, word ptr [ebp + 0xc]
00509e66 66895102                 mov        word ptr [ecx + 2], dx
00509e6a 8b45fc                   mov        eax, dword ptr [ebp - 4]
00509e6d 8b8860d80300             mov        ecx, dword ptr [eax + 0x3d860]
00509e73 668b5510                 mov        dx, word ptr [ebp + 0x10]
00509e77 66895104                 mov        word ptr [ecx + 4], dx
00509e7b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00509e7e 8b8860d80300             mov        ecx, dword ptr [eax + 0x3d860]
00509e84 83c106                   add        ecx, 6
00509e87 8b55fc                   mov        edx, dword ptr [ebp - 4]
00509e8a 898a60d80300             mov        dword ptr [edx + 0x3d860], ecx
00509e90 8b45fc                   mov        eax, dword ptr [ebp - 4]
00509e93 8945f4                   mov        dword ptr [ebp - 0xc], eax
00509e96 6a00                     push       0
00509e98 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00509e9b e830dcffff               call       0x507ad0
00509ea0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00509ea3 8b9160d80300             mov        edx, dword ptr [ecx + 0x3d860]
00509ea9 2bd0                     sub        edx, eax
00509eab 8bc2                     mov        eax, edx
00509ead 99                       cdq        
00509eae b906000000               mov        ecx, 6
00509eb3 f7f9                     idiv       ecx
00509eb5 3da08c0000               cmp        eax, 0x8ca0
00509eba 7c09                     jl         0x509ec5
00509ebc c745f801000000           mov        dword ptr [ebp - 8], 1
00509ec3 eb07                     jmp        0x509ecc
00509ec5 c745f800000000           mov        dword ptr [ebp - 8], 0
00509ecc 8b45f8                   mov        eax, dword ptr [ebp - 8]
00509ecf 8be5                     mov        esp, ebp
00509ed1 5d                       pop        ebp
00509ed2 c20c00                   ret        0xc
00509ed5 cc                       int3       
00509ed6 cc                       int3       
00509ed7 cc                       int3       
00509ed8 cc                       int3       
00509ed9 cc                       int3       
00509eda cc                       int3       
00509edb cc                       int3       
00509edc cc                       int3       
00509edd cc                       int3       
00509ede cc                       int3       
00509edf cc                       int3       

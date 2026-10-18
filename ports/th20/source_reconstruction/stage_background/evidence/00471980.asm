00471980 55                       push       ebp
00471981 8bec                     mov        ebp, esp
00471983 6aff                     push       -1
00471985 68f0745600               push       0x5674f0
0047198a 64a100000000             mov        eax, dword ptr fs:[0]
00471990 50                       push       eax
00471991 83ec10                   sub        esp, 0x10
00471994 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00471999 33c5                     xor        eax, ebp
0047199b 50                       push       eax
0047199c 8d45f4                   lea        eax, [ebp - 0xc]
0047199f 64a300000000             mov        dword ptr fs:[0], eax
004719a5 894df0                   mov        dword ptr [ebp - 0x10], ecx
004719a8 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004719ab c7000cf85600             mov        dword ptr [eax], 0x56f80c
004719b1 686cf85600               push       0x56f86c
004719b6 e8f5acf9ff               call       0x40c6b0
004719bb 83c404                   add        esp, 4
004719be 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004719c1 8b5108                   mov        edx, dword ptr [ecx + 8]
004719c4 52                       push       edx
004719c5 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004719cb e8e00afaff               call       0x4124b0
004719d0 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004719d3 8b480c                   mov        ecx, dword ptr [eax + 0xc]
004719d6 51                       push       ecx
004719d7 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004719dd e8ce0afaff               call       0x4124b0
004719e2 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004719e5 8b8268330000             mov        eax, dword ptr [edx + 0x3368]
004719eb 50                       push       eax
004719ec 8b0dd8665b00             mov        ecx, dword ptr [0x5b66d8]
004719f2 e8b90afaff               call       0x4124b0
004719f7 90                       nop        
004719f8 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004719fb 83b92033000000           cmp        dword ptr [ecx + 0x3320], 0
00471a02 7474                     je         0x471a78
00471a04 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471a07 8b8220330000             mov        eax, dword ptr [edx + 0x3320]
00471a0d 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00471a10 c745e800000000           mov        dword ptr [ebp - 0x18], 0
00471a17 eb09                     jmp        0x471a22
00471a19 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00471a1c 83c101                   add        ecx, 1
00471a1f 894de8                   mov        dword ptr [ebp - 0x18], ecx
00471a22 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471a25 8b8224330000             mov        eax, dword ptr [edx + 0x3324]
00471a2b 0fbf4802                 movsx      ecx, word ptr [eax + 2]
00471a2f 394de8                   cmp        dword ptr [ebp - 0x18], ecx
00471a32 7d16                     jge        0x471a4a
00471a34 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00471a37 e804a8fdff               call       0x44c240
00471a3c 8b55e4                   mov        edx, dword ptr [ebp - 0x1c]
00471a3f 81c2e4050000             add        edx, 0x5e4
00471a45 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00471a48 ebcf                     jmp        0x471a19
00471a4a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00471a4d 83b82033000000           cmp        dword ptr [eax + 0x3320], 0
00471a54 7422                     je         0x471a78
00471a56 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471a59 8b9120330000             mov        edx, dword ptr [ecx + 0x3320]
00471a5f 52                       push       edx
00471a60 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00471a66 e805dcfaff               call       0x41f670
00471a6b 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00471a6e c7802033000000000000     mov        dword ptr [eax + 0x3320], 0
00471a78 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
00471a7f eb09                     jmp        0x471a8a
00471a81 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00471a84 83c101                   add        ecx, 1
00471a87 894dec                   mov        dword ptr [ebp - 0x14], ecx
00471a8a 837dec08                 cmp        dword ptr [ebp - 0x14], 8
00471a8e 7d19                     jge        0x471aa9
00471a90 6955ece4050000           imul       edx, dword ptr [ebp - 0x14], 0x5e4
00471a97 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00471a9a 8d8c1084030000           lea        ecx, [eax + edx + 0x384]
00471aa1 e89aa7fdff               call       0x44c240
00471aa6 90                       nop        
00471aa7 ebd8                     jmp        0x471a81
00471aa9 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471aac 83b92433000000           cmp        dword ptr [ecx + 0x3324], 0
00471ab3 742e                     je         0x471ae3
00471ab5 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471ab8 83ba2433000000           cmp        dword ptr [edx + 0x3324], 0
00471abf 7422                     je         0x471ae3
00471ac1 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00471ac4 8b8824330000             mov        ecx, dword ptr [eax + 0x3324]
00471aca 51                       push       ecx
00471acb 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00471ad1 e89adbfaff               call       0x41f670
00471ad6 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471ad9 c7822433000000000000     mov        dword ptr [edx + 0x3324], 0
00471ae3 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00471ae6 83b86033000000           cmp        dword ptr [eax + 0x3360], 0
00471aed 742e                     je         0x471b1d
00471aef 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471af2 83b96033000000           cmp        dword ptr [ecx + 0x3360], 0
00471af9 7422                     je         0x471b1d
00471afb 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471afe 8b8260330000             mov        eax, dword ptr [edx + 0x3360]
00471b04 50                       push       eax
00471b05 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00471b0b e860dbfaff               call       0x41f670
00471b10 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471b13 c7816033000000000000     mov        dword ptr [ecx + 0x3360], 0
00471b1d 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471b20 c7826033000000000000     mov        dword ptr [edx + 0x3360], 0
00471b2a b804000000               mov        eax, 4
00471b2f 6bc800                   imul       ecx, eax, 0
00471b32 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471b35 83bc0ac832000000         cmp        dword ptr [edx + ecx + 0x32c8], 0
00471b3d 7434                     je         0x471b73
00471b3f b804000000               mov        eax, 4
00471b44 6bc800                   imul       ecx, eax, 0
00471b47 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471b4a 8d840ac8320000           lea        eax, [edx + ecx + 0x32c8]
00471b51 50                       push       eax
00471b52 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00471b58 e8b3f6ffff               call       0x471210
00471b5d b904000000               mov        ecx, 4
00471b62 6bd100                   imul       edx, ecx, 0
00471b65 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00471b68 c78410c832000000000000   mov        dword ptr [eax + edx + 0x32c8], 0
00471b73 b904000000               mov        ecx, 4
00471b78 c1e100                   shl        ecx, 0
00471b7b 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471b7e 83bc0ac832000000         cmp        dword ptr [edx + ecx + 0x32c8], 0
00471b86 7434                     je         0x471bbc
00471b88 b804000000               mov        eax, 4
00471b8d c1e000                   shl        eax, 0
00471b90 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471b93 8d9401c8320000           lea        edx, [ecx + eax + 0x32c8]
00471b9a 52                       push       edx
00471b9b 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00471ba1 e86af6ffff               call       0x471210
00471ba6 b804000000               mov        eax, 4
00471bab c1e000                   shl        eax, 0
00471bae 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471bb1 c78401c832000000000000   mov        dword ptr [ecx + eax + 0x32c8], 0
00471bbc b968a55b00               mov        ecx, 0x5ba568
00471bc1 e83a320000               call       0x474e00
00471bc6 85c0                     test       eax, eax
00471bc8 751c                     jne        0x471be6
00471bca 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
00471bcd 8b8258330000             mov        eax, dword ptr [edx + 0x3358]
00471bd3 83e001                   and        eax, 1
00471bd6 83c003                   add        eax, 3
00471bd9 50                       push       eax
00471bda 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00471be0 e84ba8fdff               call       0x44c430
00471be5 90                       nop        
00471be6 8b0d9c065c00             mov        ecx, dword ptr [0x5c069c]
00471bec 3b4df0                   cmp        ecx, dword ptr [ebp - 0x10]
00471bef 750a                     jne        0x471bfb
00471bf1 c7059c065c0000000000     mov        dword ptr [0x5c069c], 0
00471bfb 8b15a0065c00             mov        edx, dword ptr [0x5c06a0]
00471c01 3b55f0                   cmp        edx, dword ptr [ebp - 0x10]
00471c04 750a                     jne        0x471c10
00471c06 c705a0065c0000000000     mov        dword ptr [0x5c06a0], 0
00471c10 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471c13 83c110                   add        ecx, 0x10
00471c16 e825000000               call       0x471c40
00471c1b 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00471c1e e8cde1faff               call       0x41fdf0
00471c23 90                       nop        
00471c24 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00471c27 64890d00000000           mov        dword ptr fs:[0], ecx
00471c2e 59                       pop        ecx
00471c2f 8be5                     mov        esp, ebp
00471c31 5d                       pop        ebp
00471c32 c3                       ret        
00471c33 cc                       int3       
00471c34 cc                       int3       
00471c35 cc                       int3       
00471c36 cc                       int3       
00471c37 cc                       int3       
00471c38 cc                       int3       
00471c39 cc                       int3       
00471c3a cc                       int3       
00471c3b cc                       int3       
00471c3c cc                       int3       
00471c3d cc                       int3       
00471c3e cc                       int3       
00471c3f cc                       int3       

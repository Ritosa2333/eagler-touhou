00481780 55                       push       ebp
00481781 8bec                     mov        ebp, esp
00481783 6aff                     push       -1
00481785 685d915600               push       0x56915d
0048178a 64a100000000             mov        eax, dword ptr fs:[0]
00481790 50                       push       eax
00481791 83ec2c                   sub        esp, 0x2c
00481794 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00481799 33c5                     xor        eax, ebp
0048179b 50                       push       eax
0048179c 8d45f4                   lea        eax, [ebp - 0xc]
0048179f 64a300000000             mov        dword ptr fs:[0], eax
004817a5 894de4                   mov        dword ptr [ebp - 0x1c], ecx
004817a8 c745fc00000000           mov        dword ptr [ebp - 4], 0
004817af 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
004817b2 8b88a06d2800             mov        ecx, dword ptr [eax + 0x286da0]
004817b8 e8d3e7f8ff               call       0x40ff90
004817bd 8945e0                   mov        dword ptr [ebp - 0x20], eax
004817c0 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004817c3 83c108                   add        ecx, 8
004817c6 51                       push       ecx
004817c7 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004817ca e881db0700               call       0x4ff350
004817cf d95ddc                   fstp       dword ptr [ebp - 0x24]
004817d2 c745f000000000           mov        dword ptr [ebp - 0x10], 0
004817d9 eb09                     jmp        0x4817e4
004817db 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
004817de 83c201                   add        edx, 1
004817e1 8955f0                   mov        dword ptr [ebp - 0x10], edx
004817e4 8b4508                   mov        eax, dword ptr [ebp + 8]
004817e7 0fbf4826                 movsx      ecx, word ptr [eax + 0x26]
004817eb 394df0                   cmp        dword ptr [ebp - 0x10], ecx
004817ee 7d76                     jge        0x481866
004817f0 c745ec00000000           mov        dword ptr [ebp - 0x14], 0
004817f7 eb09                     jmp        0x481802
004817f9 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
004817fc 83c201                   add        edx, 1
004817ff 8955ec                   mov        dword ptr [ebp - 0x14], edx
00481802 8b4508                   mov        eax, dword ptr [ebp + 8]
00481805 0fbf4824                 movsx      ecx, word ptr [eax + 0x24]
00481809 394dec                   cmp        dword ptr [ebp - 0x14], ecx
0048180c 7d53                     jge        0x481861
0048180e 51                       push       ecx
0048180f f30f1045dc               movss      xmm0, dword ptr [ebp - 0x24]
00481814 f30f110424               movss      dword ptr [esp], xmm0
00481819 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0048181c 52                       push       edx
0048181d 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00481820 50                       push       eax
00481821 83ec08                   sub        esp, 8
00481824 8bcc                     mov        ecx, esp
00481826 8965c8                   mov        dword ptr [ebp - 0x38], esp
00481829 8d550c                   lea        edx, [ebp + 0xc]
0048182c 52                       push       edx
0048182d e81ea0ffff               call       0x47b850
00481832 8b4508                   mov        eax, dword ptr [ebp + 8]
00481835 50                       push       eax
00481836 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00481839 e8a2000000               call       0x4818e0
0048183e 8945d8                   mov        dword ptr [ebp - 0x28], eax
00481841 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00481844 894de8                   mov        dword ptr [ebp - 0x18], ecx
00481847 837de800                 cmp        dword ptr [ebp - 0x18], 0
0048184b 7412                     je         0x48185f
0048184d 837de801                 cmp        dword ptr [ebp - 0x18], 1
00481851 7504                     jne        0x481857
00481853 eb11                     jmp        0x481866
00481855 eb08                     jmp        0x48185f
00481857 837de8ff                 cmp        dword ptr [ebp - 0x18], -1
0048185b 7502                     jne        0x48185f
0048185d eb9a                     jmp        0x4817f9
0048185f eb98                     jmp        0x4817f9
00481861 e975ffffff               jmp        0x4817db
00481866 8d4d0c                   lea        ecx, [ebp + 0xc]
00481869 e892aaf8ff               call       0x40c300
0048186e 83783c00                 cmp        dword ptr [eax + 0x3c], 0
00481872 7c35                     jl         0x4818a9
00481874 8b5508                   mov        edx, dword ptr [ebp + 8]
00481877 f30f104208               movss      xmm0, dword ptr [edx + 8]
0048187c f30f1145d4               movss      dword ptr [ebp - 0x2c], xmm0
00481881 8d4d0c                   lea        ecx, [ebp + 0xc]
00481884 e877aaf8ff               call       0x40c300
00481889 8b403c                   mov        eax, dword ptr [eax + 0x3c]
0048188c 8945d0                   mov        dword ptr [ebp - 0x30], eax
0048188f 51                       push       ecx
00481890 f30f1045d4               movss      xmm0, dword ptr [ebp - 0x2c]
00481895 f30f110424               movss      dword ptr [esp], xmm0
0048189a 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
0048189d 51                       push       ecx
0048189e b930a85b00               mov        ecx, 0x5ba830
004818a3 e80856faff               call       0x426eb0
004818a8 90                       nop        
004818a9 c745cc00000000           mov        dword ptr [ebp - 0x34], 0
004818b0 c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004818b7 8d4d0c                   lea        ecx, [ebp + 0xc]
004818ba e841aaffff               call       0x47c300
004818bf 8b45cc                   mov        eax, dword ptr [ebp - 0x34]
004818c2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004818c5 64890d00000000           mov        dword ptr fs:[0], ecx
004818cc 59                       pop        ecx
004818cd 8be5                     mov        esp, ebp
004818cf 5d                       pop        ebp
004818d0 c20c00                   ret        0xc
004818d3 cc                       int3       
004818d4 cc                       int3       
004818d5 cc                       int3       
004818d6 cc                       int3       
004818d7 cc                       int3       
004818d8 cc                       int3       
004818d9 cc                       int3       
004818da cc                       int3       
004818db cc                       int3       
004818dc cc                       int3       
004818dd cc                       int3       
004818de cc                       int3       
004818df cc                       int3       

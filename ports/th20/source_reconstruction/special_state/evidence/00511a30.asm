00511a30 55                       push       ebp
00511a31 8bec                     mov        ebp, esp
00511a33 6aff                     push       -1
00511a35 68508d5600               push       0x568d50
00511a3a 64a100000000             mov        eax, dword ptr fs:[0]
00511a40 50                       push       eax
00511a41 83ec20                   sub        esp, 0x20
00511a44 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00511a49 33c5                     xor        eax, ebp
00511a4b 8945f0                   mov        dword ptr [ebp - 0x10], eax
00511a4e 50                       push       eax
00511a4f 8d45f4                   lea        eax, [ebp - 0xc]
00511a52 64a300000000             mov        dword ptr fs:[0], eax
00511a58 894de0                   mov        dword ptr [ebp - 0x20], ecx
00511a5b 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00511a5e 8945e4                   mov        dword ptr [ebp - 0x1c], eax
00511a61 6a08                     push       8
00511a63 8d4de8                   lea        ecx, [ebp - 0x18]
00511a66 e815a6efff               call       0x40c080
00511a6b 8d4de8                   lea        ecx, [ebp - 0x18]
00511a6e 51                       push       ecx
00511a6f 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00511a72 e80908f0ff               call       0x412280
00511a77 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
00511a7a e8c10af0ff               call       0x412540
00511a7f 8945dc                   mov        dword ptr [ebp - 0x24], eax
00511a82 eb09                     jmp        0x511a8d
00511a84 8d4de8                   lea        ecx, [ebp - 0x18]
00511a87 e8a401f0ff               call       0x411c30
00511a8c 90                       nop        
00511a8d 8b55dc                   mov        edx, dword ptr [ebp - 0x24]
00511a90 52                       push       edx
00511a91 8d4de8                   lea        ecx, [ebp - 0x18]
00511a94 e81701f0ff               call       0x411bb0
00511a99 0fb6c0                   movzx      eax, al
00511a9c 85c0                     test       eax, eax
00511a9e 7428                     je         0x511ac8
00511aa0 8d4de8                   lea        ecx, [ebp - 0x18]
00511aa3 e858a8efff               call       0x40c300
00511aa8 8945d8                   mov        dword ptr [ebp - 0x28], eax
00511aab 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00511aae e84da8efff               call       0x40c300
00511ab3 8945d4                   mov        dword ptr [ebp - 0x2c], eax
00511ab6 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
00511ab9 51                       push       ecx
00511aba 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
00511ac0 e8ab20f0ff               call       0x413b70
00511ac5 90                       nop        
00511ac6 ebbc                     jmp        0x511a84
00511ac8 8d4de8                   lea        ecx, [ebp - 0x18]
00511acb e83000f0ff               call       0x411b00
00511ad0 90                       nop        
00511ad1 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00511ad4 64890d00000000           mov        dword ptr fs:[0], ecx
00511adb 59                       pop        ecx
00511adc 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00511adf 33cd                     xor        ecx, ebp
00511ae1 e8080f0300               call       0x5429ee
00511ae6 8be5                     mov        esp, ebp
00511ae8 5d                       pop        ebp
00511ae9 c3                       ret        
00511aea cc                       int3       
00511aeb cc                       int3       
00511aec cc                       int3       
00511aed cc                       int3       
00511aee cc                       int3       
00511aef cc                       int3       

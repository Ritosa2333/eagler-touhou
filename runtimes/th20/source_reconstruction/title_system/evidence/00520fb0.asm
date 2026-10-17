00520fb0 55                       push       ebp
00520fb1 8bec                     mov        ebp, esp
00520fb3 83ec18                   sub        esp, 0x18
00520fb6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00520fbb 33c5                     xor        eax, ebp
00520fbd 8945fc                   mov        dword ptr [ebp - 4], eax
00520fc0 894dec                   mov        dword ptr [ebp - 0x14], ecx
00520fc3 51                       push       ecx
00520fc4 0f57c0                   xorps      xmm0, xmm0
00520fc7 f30f110424               movss      dword ptr [esp], xmm0
00520fcc 51                       push       ecx
00520fcd f30f100530fa5600         movss      xmm0, dword ptr [0x56fa30]
00520fd5 f30f110424               movss      dword ptr [esp], xmm0
00520fda 51                       push       ecx
00520fdb f30f100554105700         movss      xmm0, dword ptr [0x571054]
00520fe3 f30f110424               movss      dword ptr [esp], xmm0
00520fe8 8d4df0                   lea        ecx, [ebp - 0x10]
00520feb e8e01df0ff               call       0x422dd0
00520ff0 90                       nop        
00520ff1 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00520ff4 8b4820                   mov        ecx, dword ptr [eax + 0x20]
00520ff7 894de8                   mov        dword ptr [ebp - 0x18], ecx
00520ffa 837de800                 cmp        dword ptr [ebp - 0x18], 0
00520ffe 7408                     je         0x521008
00521000 837de801                 cmp        dword ptr [ebp - 0x18], 1
00521004 742b                     je         0x521031
00521006 eb47                     jmp        0x52104f
00521008 8d55f0                   lea        edx, [ebp - 0x10]
0052100b 52                       push       edx
0052100c e86f00fcff               call       0x4e1080
00521011 83c404                   add        esp, 4
00521014 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
00521017 c7402001000000           mov        dword ptr [eax + 0x20], 1
0052101e 6a00                     push       0
00521020 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00521023 81c154010000             add        ecx, 0x154
00521029 e8f224f0ff               call       0x423520
0052102e 90                       nop        
0052102f eb1e                     jmp        0x52104f
00521031 833db8605c0000           cmp        dword ptr [0x5c60b8], 0
00521038 7515                     jne        0x52104f
0052103a 6a01                     push       1
0052103c 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0052103f e88cbe0000               call       0x52ced0
00521044 6a00                     push       0
00521046 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00521049 e8a2bb0000               call       0x52cbf0
0052104e 90                       nop        
0052104f 33c0                     xor        eax, eax
00521051 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00521054 33cd                     xor        ecx, ebp
00521056 e893190200               call       0x5429ee
0052105b 8be5                     mov        esp, ebp
0052105d 5d                       pop        ebp
0052105e c3                       ret        
0052105f cc                       int3       

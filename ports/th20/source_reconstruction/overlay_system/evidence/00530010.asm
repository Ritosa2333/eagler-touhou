00530010 55                       push       ebp
00530011 8bec                     mov        ebp, esp
00530013 83ec28                   sub        esp, 0x28
00530016 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0053001b 33c5                     xor        eax, ebp
0053001d 8945fc                   mov        dword ptr [ebp - 4], eax
00530020 894dec                   mov        dword ptr [ebp - 0x14], ecx
00530023 8d4df0                   lea        ecx, [ebp - 0x10]
00530026 e8e52defff               call       0x422e10
0053002b 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0053002e 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
00530034 e857ffedff               call       0x40ff90
00530039 8bc8                     mov        ecx, eax
0053003b e8a0f5fcff               call       0x4ff5e0
00530040 0fb6c8                   movzx      ecx, al
00530043 85c9                     test       ecx, ecx
00530045 750f                     jne        0x530056
00530047 f30f100548e05600         movss      xmm0, dword ptr [0x56e048]
0053004f f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00530054 eb0d                     jmp        0x530063
00530056 f30f100594cd5600         movss      xmm0, dword ptr [0x56cd94]
0053005e f30f1145e8               movss      dword ptr [ebp - 0x18], xmm0
00530063 51                       push       ecx
00530064 f30f1045e8               movss      xmm0, dword ptr [ebp - 0x18]
00530069 f30f110424               movss      dword ptr [esp], xmm0
0053006e 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530071 81c1d4000000             add        ecx, 0xd4
00530077 e86492efff               call       0x4292e0
0053007c 51                       push       ecx
0053007d d91c24                   fstp       dword ptr [esp]
00530080 8d55f0                   lea        edx, [ebp - 0x10]
00530083 52                       push       edx
00530084 e8a792f0ff               call       0x439330
00530089 83c40c                   add        esp, 0xc
0053008c 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
0053008f 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
00530095 e8f6feedff               call       0x40ff90
0053009a 8bc8                     mov        ecx, eax
0053009c e8eff5fcff               call       0x4ff690
005300a1 f30f1045f0               movss      xmm0, dword ptr [ebp - 0x10]
005300a6 f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
005300ae f30f2cc8                 cvttss2si  ecx, xmm0
005300b2 0308                     add        ecx, dword ptr [eax]
005300b4 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
005300b7 894a70                   mov        dword ptr [edx + 0x70], ecx
005300ba 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
005300bd 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
005300c3 e8c8feedff               call       0x40ff90
005300c8 8bc8                     mov        ecx, eax
005300ca e8c1f5fcff               call       0x4ff690
005300cf f30f1045f4               movss      xmm0, dword ptr [ebp - 0xc]
005300d4 f30f590598cd5600         mulss      xmm0, dword ptr [0x56cd98]
005300dc f30f2cc8                 cvttss2si  ecx, xmm0
005300e0 034804                   add        ecx, dword ptr [eax + 4]
005300e3 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
005300e6 894a74                   mov        dword ptr [edx + 0x74], ecx
005300e9 8b45ec                   mov        eax, dword ptr [ebp - 0x14]
005300ec 8b8828010000             mov        ecx, dword ptr [eax + 0x128]
005300f2 e899feedff               call       0x40ff90
005300f7 8bc8                     mov        ecx, eax
005300f9 e8e2f4fcff               call       0x4ff5e0
005300fe 0fb6c8                   movzx      ecx, al
00530101 85c9                     test       ecx, ecx
00530103 7552                     jne        0x530157
00530105 8b55ec                   mov        edx, dword ptr [ebp - 0x14]
00530108 81c2d4000000             add        edx, 0xd4
0053010e 8955e4                   mov        dword ptr [ebp - 0x1c], edx
00530111 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
00530119 f30f5e05d00a5700         divss      xmm0, dword ptr [0x570ad0]
00530121 51                       push       ecx
00530122 f30f110424               movss      dword ptr [esp], xmm0
00530127 8d45dc                   lea        eax, [ebp - 0x24]
0053012a 50                       push       eax
0053012b 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0053012e e88d2ef2ff               call       0x452fc0
00530133 8bc8                     mov        ecx, eax
00530135 e8a691efff               call       0x4292e0
0053013a 51                       push       ecx
0053013b d91c24                   fstp       dword ptr [esp]
0053013e e8fd83f0ff               call       0x438540
00530143 d91c24                   fstp       dword ptr [esp]
00530146 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00530149 81c1d4000000             add        ecx, 0xd4
0053014f e8cc2df2ff               call       0x452f20
00530154 90                       nop        
00530155 eb50                     jmp        0x5301a7
00530157 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0053015a 81c1d4000000             add        ecx, 0xd4
00530160 894de0                   mov        dword ptr [ebp - 0x20], ecx
00530163 f30f1005f0e05600         movss      xmm0, dword ptr [0x56e0f0]
0053016b f30f5e0560055700         divss      xmm0, dword ptr [0x570560]
00530173 51                       push       ecx
00530174 f30f110424               movss      dword ptr [esp], xmm0
00530179 8d55d8                   lea        edx, [ebp - 0x28]
0053017c 52                       push       edx
0053017d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
00530180 e83b2ef2ff               call       0x452fc0
00530185 8bc8                     mov        ecx, eax
00530187 e85491efff               call       0x4292e0
0053018c 51                       push       ecx
0053018d d91c24                   fstp       dword ptr [esp]
00530190 e8ab83f0ff               call       0x438540
00530195 d91c24                   fstp       dword ptr [esp]
00530198 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0053019b 81c1d4000000             add        ecx, 0xd4
005301a1 e87a2df2ff               call       0x452f20
005301a6 90                       nop        
005301a7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005301aa 33cd                     xor        ecx, ebp
005301ac e83d280100               call       0x5429ee
005301b1 8be5                     mov        esp, ebp
005301b3 5d                       pop        ebp
005301b4 c3                       ret        
005301b5 cc                       int3       
005301b6 cc                       int3       
005301b7 cc                       int3       
005301b8 cc                       int3       
005301b9 cc                       int3       
005301ba cc                       int3       
005301bb cc                       int3       
005301bc cc                       int3       
005301bd cc                       int3       
005301be cc                       int3       
005301bf cc                       int3       

005081e0 55                       push       ebp
005081e1 8bec                     mov        ebp, esp
005081e3 83ec14                   sub        esp, 0x14
005081e6 894dfc                   mov        dword ptr [ebp - 4], ecx
005081e9 833d28a85b0000           cmp        dword ptr [0x5ba828], 0
005081f0 750a                     jne        0x5081fc
005081f2 b801000000               mov        eax, 1
005081f7 e906010000               jmp        0x508302
005081fc 8b45fc                   mov        eax, dword ptr [ebp - 4]
005081ff 83781001                 cmp        dword ptr [eax + 0x10], 1
00508203 0f85f4000000             jne        0x5082fd
00508209 33c9                     xor        ecx, ecx
0050820b 7534                     jne        0x508241
0050820d ba04000000               mov        edx, 4
00508212 6bc200                   imul       eax, edx, 0
00508215 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
0050821c 7423                     je         0x508241
0050821e b904000000               mov        ecx, 4
00508223 6bd100                   imul       edx, ecx, 0
00508226 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
0050822c 8945f0                   mov        dword ptr [ebp - 0x10], eax
0050822f 6800020000               push       0x200
00508234 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
00508237 e86413f4ff               call       0x4495a0
0050823c 8945f8                   mov        dword ptr [ebp - 8], eax
0050823f eb07                     jmp        0x508248
00508241 c745f800000000           mov        dword ptr [ebp - 8], 0
00508248 837df800                 cmp        dword ptr [ebp - 8], 0
0050824c 7542                     jne        0x508290
0050824e 33c9                     xor        ecx, ecx
00508250 7531                     jne        0x508283
00508252 ba04000000               mov        edx, 4
00508257 6bc200                   imul       eax, edx, 0
0050825a 83b89c885b0000           cmp        dword ptr [eax + 0x5b889c], 0
00508261 7420                     je         0x508283
00508263 b904000000               mov        ecx, 4
00508268 6bd100                   imul       edx, ecx, 0
0050826b 8b829c885b00             mov        eax, dword ptr [edx + 0x5b889c]
00508271 8945ec                   mov        dword ptr [ebp - 0x14], eax
00508274 6a01                     push       1
00508276 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
00508279 e82213f4ff               call       0x4495a0
0050827e 8945f4                   mov        dword ptr [ebp - 0xc], eax
00508281 eb07                     jmp        0x50828a
00508283 c745f400000000           mov        dword ptr [ebp - 0xc], 0
0050828a 837df400                 cmp        dword ptr [ebp - 0xc], 0
0050828e 742c                     je         0x5082bc
00508290 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00508293 c7411401000000           mov        dword ptr [ecx + 0x14], 1
0050829a 8b55fc                   mov        edx, dword ptr [ebp - 4]
0050829d 8b8250020000             mov        eax, dword ptr [edx + 0x250]
005082a3 2507000080               and        eax, 0x80000007
005082a8 7905                     jns        0x5082af
005082aa 48                       dec        eax
005082ab 83c8f8                   or         eax, 0xfffffff8
005082ae 40                       inc        eax
005082af 85c0                     test       eax, eax
005082b1 7407                     je         0x5082ba
005082b3 b806000000               mov        eax, 6
005082b8 eb48                     jmp        0x508302
005082ba eb41                     jmp        0x5082fd
005082bc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005082bf 83791400                 cmp        dword ptr [ecx + 0x14], 0
005082c3 742e                     je         0x5082f3
005082c5 e8760cf9ff               call       0x498f40
005082ca 8bc8                     mov        ecx, eax
005082cc e87f1d0000               call       0x50a050
005082d1 83e808                   sub        eax, 8
005082d4 f20f2ac0                 cvtsi2sd   xmm0, eax
005082d8 f20f5e0570c45600         divsd      xmm0, qword ptr [0x56c470]
005082e0 83ec08                   sub        esp, 8
005082e3 f20f110424               movsd      qword ptr [esp], xmm0
005082e8 b930a85b00               mov        ecx, 0x5ba830
005082ed e8cee5fdff               call       0x4e68c0
005082f2 90                       nop        
005082f3 8b55fc                   mov        edx, dword ptr [ebp - 4]
005082f6 c7421400000000           mov        dword ptr [edx + 0x14], 0
005082fd b801000000               mov        eax, 1
00508302 8be5                     mov        esp, ebp
00508304 5d                       pop        ebp
00508305 c3                       ret        
00508306 cc                       int3       
00508307 cc                       int3       
00508308 cc                       int3       
00508309 cc                       int3       
0050830a cc                       int3       
0050830b cc                       int3       
0050830c cc                       int3       
0050830d cc                       int3       
0050830e cc                       int3       
0050830f cc                       int3       

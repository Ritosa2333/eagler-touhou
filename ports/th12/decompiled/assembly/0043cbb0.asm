
// block 0043cbb0
0043cbb0  push       esi
0043cbb1  push       0x18b0
0043cbb6  call       0x46c9ea

// block 0043cbbb
0043cbbb  mov        esi, eax
0043cbbd  add        esp, 4
0043cbc0  test       esi, esi
0043cbc2  je         0x43cc02

// block 0043cbc4
0043cbc4  push       0x18b0
0043cbc9  push       0
0043cbcb  push       esi
0043cbcc  call       0x477420

// block 0043cbd1
0043cbd1  mov        dword ptr [esi + 0x1518], esi
0043cbd7  lea        eax, [esi + 0x151c]
0043cbdd  mov        dword ptr [esi + 0x18a0], eax
0043cbe3  add        esp, 0xc
0043cbe6  mov        dword ptr [esi + 0x18a4], esi
0043cbec  mov        dword ptr [esi + 0x18a8], 0
0043cbf6  mov        dword ptr [esi + 0x18ac], 0
0043cc00  jmp        0x43cc04

// block 0043cc02
0043cc02  xor        esi, esi

// block 0043cc04
0043cc04  lea        ecx, [edi + edi*2]
0043cc07  cmp        dword ptr [ebx + ecx*4 + 0x44], 0
0043cc0c  lea        ecx, [ebx + ecx*4 + 0x40]
0043cc10  lea        eax, [esi + 0x18a4]
0043cc16  pop        esi
0043cc17  je         0x43cc29

// block 0043cc19
0043cc19  lea        esp, [esp]

// block 0043cc20
0043cc20  mov        ecx, dword ptr [ecx + 4]
0043cc23  cmp        dword ptr [ecx + 4], 0
0043cc27  jne        0x43cc20

// block 0043cc29
0043cc29  mov        edx, dword ptr [ecx + 4]
0043cc2c  test       edx, edx
0043cc2e  je         0x43cc39

// block 0043cc30
0043cc30  mov        dword ptr [eax + 4], edx
0043cc33  mov        edx, dword ptr [ecx + 4]
0043cc36  mov        dword ptr [edx + 8], eax

// block 0043cc39
0043cc39  mov        dword ptr [ecx + 4], eax
0043cc3c  mov        dword ptr [eax + 8], ecx
0043cc3f  inc        dword ptr [ebx + 0xa4]
0043cc45  ret        

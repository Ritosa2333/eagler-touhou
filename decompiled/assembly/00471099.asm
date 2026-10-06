
// block 00471099
00471099  test       byte ptr [ecx + 0xc], 0x40
0047109d  je         0x4710a5

// block 0047109f
0047109f  cmp        dword ptr [ecx + 8], 0
004710a3  je         0x4710c9

// block 004710a5
004710a5  dec        dword ptr [ecx + 4]
004710a8  js         0x4710b5

// block 004710aa
004710aa  mov        edx, dword ptr [ecx]
004710ac  mov        byte ptr [edx], al
004710ae  inc        dword ptr [ecx]
004710b0  movzx      eax, al
004710b3  jmp        0x4710c1

// block 004710b5
004710b5  movsx      eax, al
004710b8  push       ecx
004710b9  push       eax
004710ba  call       0x46ffdb

// block 004710bf
004710bf  pop        ecx
004710c0  pop        ecx

// block 004710c1
004710c1  cmp        eax, -1
004710c4  jne        0x4710c9

// block 004710c6
004710c6  or         dword ptr [esi], eax
004710c8  ret        

// block 004710c9
004710c9  inc        dword ptr [esi]
004710cb  ret        


// block 00464ba0
00464ba0  push       esi
00464ba1  mov        esi, edx
00464ba3  and        esi, 3
00464ba6  sub        edx, esi
00464ba8  xor        eax, eax
00464baa  test       edx, edx
00464bac  jle        0x464bc4

// block 00464bae
00464bae  lea        esi, [edx - 1]
00464bb1  shr        esi, 2
00464bb4  inc        esi

// block 00464bb5
00464bb5  mov        dl, byte ptr [ecx]
00464bb7  add        dl, al
00464bb9  add        ecx, 4
00464bbc  sub        esi, 1
00464bbf  movzx      eax, dl
00464bc2  jne        0x464bb5

// block 00464bc4
00464bc4  pop        esi
00464bc5  ret        

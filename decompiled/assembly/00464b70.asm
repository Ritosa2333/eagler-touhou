
// block 00464b70
00464b70  push       esi
00464b71  mov        esi, edx
00464b73  and        esi, 1
00464b76  sub        edx, esi
00464b78  xor        eax, eax
00464b7a  test       edx, edx
00464b7c  jle        0x464b97

// block 00464b7e
00464b7e  lea        esi, [edx - 1]
00464b81  shr        esi, 1
00464b83  inc        esi

// block 00464b84
00464b84  mov        dl, byte ptr [ecx]
00464b86  add        dl, al
00464b88  movzx      ax, dl
00464b8c  add        ecx, 2
00464b8f  sub        esi, 1
00464b92  movzx      eax, ax
00464b95  jne        0x464b84

// block 00464b97
00464b97  pop        esi
00464b98  ret        

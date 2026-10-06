
// block 00464b60
00464b60  xor        al, al
00464b62  test       ecx, ecx
00464b64  je         0x464b6e

// block 00464b66
00464b66  add        al, byte ptr [edx]
00464b68  dec        ecx
00464b69  inc        edx
00464b6a  test       ecx, ecx
00464b6c  jne        0x464b66

// block 00464b6e
00464b6e  ret        

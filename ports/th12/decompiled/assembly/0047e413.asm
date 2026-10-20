
// block 0047e413
0047e413  mov        eax, dword ptr [ecx + 8]
0047e416  test       eax, eax
0047e418  je         0x47e422

// block 0047e41a
0047e41a  mov        ecx, dword ptr [ecx + 4]
0047e41d  mov        al, byte ptr [ecx + eax - 1]
0047e421  ret        

// block 0047e422
0047e422  xor        al, al
0047e424  ret        


// block 0047e013
0047e013  mov        edi, edi
0047e015  push       esi
0047e016  mov        esi, edx
0047e018  test       esi, esi
0047e01a  jbe        0x47e027

// block 0047e01c
0047e01c  sub        ecx, eax

// block 0047e01e
0047e01e  mov        dl, byte ptr [ecx + eax]
0047e021  mov        byte ptr [eax], dl
0047e023  inc        eax
0047e024  dec        esi
0047e025  jne        0x47e01e

// block 0047e027
0047e027  pop        esi
0047e028  ret        

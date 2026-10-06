
// block 0047deb6
0047deb6  mov        edi, edi
0047deb8  push       ebp
0047deb9  mov        ebp, esp
0047debb  mov        edx, dword ptr [ebp + 8]
0047debe  mov        eax, ecx
0047dec0  mov        dword ptr [eax + 4], edx
0047dec3  dec        edx
0047dec4  neg        edx
0047dec6  sbb        edx, edx
0047dec8  and        edx, 0xfffffffc
0047decb  add        edx, 4
0047dece  mov        dword ptr [eax], 0x49ddd4
0047ded4  mov        dword ptr [eax + 8], edx
0047ded7  pop        ebp
0047ded8  ret        4

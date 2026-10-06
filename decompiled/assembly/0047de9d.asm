
// block 0047de9d
0047de9d  mov        edi, edi
0047de9f  push       ebp
0047dea0  mov        ebp, esp
0047dea2  mov        ecx, dword ptr [ecx + 4]
0047dea5  test       ecx, ecx
0047dea7  je         0x47deaf

// block 0047dea9
0047dea9  pop        ebp
0047deaa  jmp        0x47dda9

// block 0047deaf
0047deaf  mov        eax, dword ptr [ebp + 8]
0047deb2  pop        ebp
0047deb3  ret        8

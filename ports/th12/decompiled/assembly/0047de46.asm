
// block 0047de46
0047de46  mov        edi, edi
0047de48  push       ebp
0047de49  mov        ebp, esp
0047de4b  mov        eax, ecx
0047de4d  mov        ecx, dword ptr [ebp + 8]
0047de50  mov        dword ptr [eax], 0x49ddc8
0047de56  test       ecx, ecx
0047de58  je         0x47de69

// block 0047de5a
0047de5a  mov        dl, byte ptr [ecx + 4]
0047de5d  cmp        dl, 2
0047de60  je         0x47de67

// block 0047de62
0047de62  cmp        dl, 3
0047de65  jne        0x47de69

// block 0047de67
0047de67  xor        ecx, ecx

// block 0047de69
0047de69  mov        dword ptr [eax + 4], ecx
0047de6c  pop        ebp
0047de6d  ret        4

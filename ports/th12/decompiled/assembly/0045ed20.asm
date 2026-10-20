
// block 0045ed20
0045ed20  test       byte ptr [0x4ceae8], 1
0045ed27  je         0x45ed47

// block 0045ed29
0045ed29  mov        ecx, dword ptr [eax*4 + 0x4a2338]
0045ed30  cmp        ecx, 0x15
0045ed33  je         0x45ed42

// block 0045ed35
0045ed35  test       ecx, ecx
0045ed37  je         0x45ed42

// block 0045ed39
0045ed39  cmp        ecx, 0x14
0045ed3c  jne        0x45ed47

// block 0045ed3e
0045ed3e  lea        eax, [ecx - 0x11]
0045ed41  ret        

// block 0045ed42
0045ed42  mov        eax, 5

// block 0045ed47
0045ed47  ret        

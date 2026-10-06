
// block 00421b60
00421b60  mov        eax, dword ptr [0x4b43e4]
00421b65  test       dword ptr [eax + 0x6d18], 0x200
00421b6f  je         0x421b80

// block 00421b71
00421b71  cmp        dword ptr [eax + 0x6d20], 0x78
00421b78  jge        0x421b80

// block 00421b7a
00421b7a  mov        eax, 1
00421b7f  ret        

// block 00421b80
00421b80  xor        eax, eax
00421b82  ret        

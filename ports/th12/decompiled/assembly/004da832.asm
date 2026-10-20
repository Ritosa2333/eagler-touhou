
// block 004da832
004da832  rcl        ebx, 0x14
004da835  mov        eax, dword ptr [0xf4662806]
004da83a  rcr        byte ptr [esi - 0x22], 1
004da83d  xor        eax, 0xb9407c4d
004da842  jo         0x4da818

// block 004da844
004da844  pop        ebp
004da845  adc        dword ptr [ebp + edx*2 + 0x1507ac53], 0x43

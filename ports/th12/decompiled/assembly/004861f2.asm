
// block 004861f2
004861f2  test       eax, eax
004861f4  je         0x486203

// block 004861f6
004861f6  xor        ecx, ecx
004861f8  test       eax, eax
004861fa  setg       cl
004861fd  lea        ecx, [ecx + ecx - 1]
00486201  mov        eax, ecx

// block 00486203
00486203  ret        

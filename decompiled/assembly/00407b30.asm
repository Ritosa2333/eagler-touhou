
// block 00407b30
00407b30  mov        eax, dword ptr [eax + 0x16b8]
00407b36  test       al, 0x21
00407b38  jne        0x407b44

// block 00407b3a
00407b3a  test       eax, 0x6000000
00407b3f  jne        0x407b44

// block 00407b41
00407b41  xor        eax, eax
00407b43  ret        

// block 00407b44
00407b44  mov        eax, 1
00407b49  ret        


// block 00407b60
00407b60  mov        eax, dword ptr [eax + 0x26f8]
00407b66  test       al, 0x21
00407b68  jne        0x407b74

// block 00407b6a
00407b6a  test       eax, 0x6000000
00407b6f  jne        0x407b74

// block 00407b71
00407b71  xor        eax, eax
00407b73  ret        

// block 00407b74
00407b74  mov        eax, 1
00407b79  ret        

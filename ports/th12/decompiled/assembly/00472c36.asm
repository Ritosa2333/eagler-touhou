
// block 00472c36
00472c36  mov        edi, edi
00472c38  push       ebp
00472c39  mov        ebp, esp
00472c3b  push       0x49ce14
00472c40  call       dword ptr [0x498174]

// block 00472c46
00472c46  test       eax, eax
00472c48  je         0x472c5f

// block 00472c4a
00472c4a  push       0x49ce04
00472c4f  push       eax
00472c50  call       dword ptr [0x498170]

// block 00472c56
00472c56  test       eax, eax
00472c58  je         0x472c5f

// block 00472c5a
00472c5a  push       dword ptr [ebp + 8]
00472c5d  call       eax

// block 00472c5f
00472c5f  pop        ebp
00472c60  ret        

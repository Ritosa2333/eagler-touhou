
// block 00431fb0
00431fb0  push       edi
00431fb1  push       0x2e0
00431fb6  call       0x46c9ea

// block 00431fbb
00431fbb  add        esp, 4
00431fbe  test       eax, eax
00431fc0  je         0x431fcf

// block 00431fc2
00431fc2  push       esi
00431fc3  mov        esi, eax
00431fc5  call       0x431d00

// block 00431fca
00431fca  mov        edi, eax
00431fcc  pop        esi
00431fcd  jmp        0x431fd1

// block 00431fcf
00431fcf  xor        edi, edi

// block 00431fd1
00431fd1  call       0x431d70

// block 00431fd6
00431fd6  test       eax, eax
00431fd8  je         0x431ff2

// block 00431fda
00431fda  test       edi, edi
00431fdc  je         0x431fee

// block 00431fde
00431fde  mov        eax, edi
00431fe0  call       0x431ed0

// block 00431fe5
00431fe5  push       edi
00431fe6  call       0x46ca4f

// block 00431feb
00431feb  add        esp, 4

// block 00431fee
00431fee  xor        eax, eax
00431ff0  pop        edi
00431ff1  ret        

// block 00431ff2
00431ff2  mov        eax, edi
00431ff4  pop        edi
00431ff5  ret        

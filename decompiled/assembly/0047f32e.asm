
// block 0047f32e
0047f32e  mov        edi, edi
0047f330  push       ebp
0047f331  mov        ebp, esp
0047f333  sub        esp, 0x10
0047f336  push       0x27
0047f338  push       dword ptr [ebp + 8]
0047f33b  lea        eax, [ebp - 8]
0047f33e  push       0
0047f340  push       eax
0047f341  call       0x47ecb6

// block 0047f346
0047f346  push       eax
0047f347  lea        eax, [ebp - 0x10]
0047f34a  push       0x60
0047f34c  push       eax
0047f34d  call       0x47ec02

// block 0047f352
0047f352  add        esp, 0x14
0047f355  mov        ecx, eax
0047f357  call       0x47ec6e

// block 0047f35c
0047f35c  mov        eax, dword ptr [ebp + 8]
0047f35f  leave      
0047f360  ret        

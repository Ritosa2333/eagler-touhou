
// block 004db54a
004db54a  js         0x4db4f9

// block 004db54c
004db54c  test       eax, 0xfc8dbdfc
004db551  mov        ebp, 0xde3abc40
004db556  xchg       dword ptr [edi + 0x53], edi
004db559  dec        ebp
004db55a  leave      
004db55b  aad        0xb6
004db55d  inc        edi
004db55e  xchg       edi, eax
004db55f  adc        ebx, dword ptr [ecx - 0x2d]
004db562  fist       dword ptr [edi]
004db564  add        ebp, dword ptr [edx - 0x40]
004db567  call       0xc3018391

// block 004db56c
004db56c  stosb      byte ptr es:[edi], al

// block 004db56d
004db56d  inc        eax
004db56e  sbb        byte ptr [ebp + 0x10], bh
004db571  sub        ah, byte ptr [ebp + 0x67]
004db574  je         0x4db508

// block 004db576
004db576  cmp        eax, dword ptr [ecx + 0x1def47e9]
004db57c  rcr        ecx, 0x48
004db57f  xor        eax, dword ptr [eax]
004db581  or         eax, 0xbb9ca2d0
004db586  inc        esi
004db587  add        edi, dword ptr [eax]
004db589  mov        al, byte ptr [0xb5b479a3]
004db58e  idiv       byte ptr [edi]
004db591  aaa        
004db592  pop        ebp
004db593  out        dx, eax
004db594  daa        
004db595  int        0x9d

// block 004db597
004db597  mov        edx, 0xdbce8194
004db59c  dec        edx
004db59d  sub        al, 0x3a
004db59f  xchg       esp, eax
004db5a0  sbb        ebx, dword ptr [edx]
004db5a2  pushfd     

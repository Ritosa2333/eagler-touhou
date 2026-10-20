
// block 004d96a2
004d96a2  cmp        byte ptr [esi + 0x4bb82d78], bl
004d96a8  dec        eax
004d96a9  call       0x266ce625

// block 004d96f4
004d96f4  and        esi, dword ptr [ecx + 8]
004d96f7  div        byte ptr [edi + 0x61]
004d96fa  daa        
004d96fb  sub        eax, dword ptr [esp + ebx*2 - 0x28c6f749]
004d9702  or         dword ptr [ebx], edi
004d9704  cmp        ah, dh
004d9706  test       byte ptr [ecx - 0x40], bh

// block 004d9709
004d9709  xchg       ebx, eax
004d970a  pushfd     
004d970b  shl        byte ptr [edi + edx*4], 9
004d970f  das        
004d9710  add        al, 0x8b
004d9713  jo         0x4d96a2

// block 004d9715
004d9715  push       edi
004d9716  or         dword ptr [edi], edx
004d9718  pushfd     
004d9719  mov        bh, 0x3b
004d971b  das        


// block 0044b710
0044b710  mov        eax, dword ptr [esi + 8]
0044b713  push       edi
0044b714  xor        edi, edi
0044b716  cmp        eax, edi
0044b718  je         0x44b73b

// block 0044b71a
0044b71a  push       eax
0044b71b  push       0x4a21f0
0044b720  call       0x44bcd0

// block 0044b725
0044b725  mov        eax, dword ptr [esi + 8]
0044b728  add        esp, 8
0044b72b  cmp        eax, edi
0044b72d  je         0x44b73b

// block 0044b72f
0044b72f  push       eax
0044b730  call       0x46c941

// block 0044b735
0044b735  add        esp, 4
0044b738  mov        dword ptr [esi + 8], edi

// block 0044b73b
0044b73b  mov        eax, dword ptr [esi]
0044b73d  mov        dword ptr [esi + 8], edi
0044b740  cmp        eax, edi
0044b742  je         0x44b763

// block 0044b744
0044b744  mov        ecx, dword ptr [eax - 4]
0044b747  push       ebx
0044b748  lea        ebx, [eax - 4]
0044b74b  push       0x44bc50
0044b750  push       ecx
0044b751  push       0x10
0044b753  push       eax
0044b754  call       0x46cf1e

// block 0044b759
0044b759  push       ebx
0044b75a  call       0x46ca4f

// block 0044b75f
0044b75f  add        esp, 4
0044b762  pop        ebx

// block 0044b763
0044b763  mov        ecx, dword ptr [esi + 0xc]
0044b766  mov        dword ptr [esi], edi
0044b768  cmp        ecx, edi
0044b76a  je         0x44b775

// block 0044b76c
0044b76c  mov        edx, dword ptr [ecx]
0044b76e  mov        eax, dword ptr [edx + 0x1c]
0044b771  push       1
0044b773  call       eax

// block 0044b775
0044b775  mov        dword ptr [esi + 4], edi
0044b778  mov        dword ptr [esi + 0xc], edi
0044b77b  pop        edi
0044b77c  ret        

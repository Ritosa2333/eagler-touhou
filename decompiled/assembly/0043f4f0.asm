
// block 0043f4f0
0043f4f0  push       -1
0043f4f2  push       0x4974ae
0043f4f7  mov        eax, dword ptr fs:[0]
0043f4fd  push       eax
0043f4fe  push       ecx
0043f4ff  push       ebx
0043f500  push       ebp
0043f501  push       esi
0043f502  push       edi
0043f503  mov        eax, dword ptr [0x4ad138]
0043f508  xor        eax, esp
0043f50a  push       eax
0043f50b  lea        eax, [esp + 0x18]
0043f50f  mov        dword ptr fs:[0], eax
0043f515  mov        ebp, dword ptr [esp + 0x28]
0043f519  mov        dword ptr [ebp], 0x4a20cc
0043f520  lea        esi, [ebp + 0x5c1c]
0043f526  mov        dword ptr [esp + 0x20], 0
0043f52e  mov        dword ptr [esp + 0x14], esi
0043f532  call       0x464c40

// block 0043f537
0043f537  mov        edi, dword ptr [ebp + 0xc]
0043f53a  mov        ebx, dword ptr [0x4ce89c]
0043f540  mov        esi, dword ptr [0x498088]
0043f546  test       edi, edi
0043f548  je         0x43f589

// block 0043f54a
0043f54a  test       dword ptr [0x4cee78], 0x8000
0043f554  je         0x43f563

// block 0043f556
0043f556  push       0x4cf0f8
0043f55b  call       esi

// block 0043f55d
0043f55d  inc        byte ptr [0x4cf218]

// block 0043f563
0043f563  mov        ecx, edi
0043f565  mov        edx, ebx
0043f567  call       0x462890

// block 0043f56c
0043f56c  test       dword ptr [0x4cee78], 0x8000
0043f576  je         0x43f589

// block 0043f578
0043f578  push       0x4cf0f8
0043f57d  call       dword ptr [0x49808c]

// block 0043f583
0043f583  dec        byte ptr [0x4cf218]

// block 0043f589
0043f589  mov        edi, dword ptr [ebp + 0x10]
0043f58c  mov        ebx, dword ptr [0x4ce89c]
0043f592  test       edi, edi
0043f594  je         0x43f5d5

// block 0043f596
0043f596  test       dword ptr [0x4cee78], 0x8000
0043f5a0  je         0x43f5af

// block 0043f5a2
0043f5a2  push       0x4cf0f8
0043f5a7  call       esi

// block 0043f5a9
0043f5a9  inc        byte ptr [0x4cf218]

// block 0043f5af
0043f5af  mov        ecx, edi
0043f5b1  mov        edx, ebx
0043f5b3  call       0x462890

// block 0043f5b8
0043f5b8  test       dword ptr [0x4cee78], 0x8000
0043f5c2  je         0x43f5d5

// block 0043f5c4
0043f5c4  push       0x4cf0f8
0043f5c9  call       dword ptr [0x49808c]

// block 0043f5cf
0043f5cf  dec        byte ptr [0x4cf218]

// block 0043f5d5
0043f5d5  mov        ebx, dword ptr [0x4ce8cc]
0043f5db  mov        edi, dword ptr [ebx + 0x4b5120]
0043f5e1  add        ebx, 0x4b5120
0043f5e7  test       edi, edi
0043f5e9  je         0x43f601

// block 0043f5eb
0043f5eb  call       0x4604e0

// block 0043f5f0
0043f5f0  mov        eax, dword ptr [ebx]
0043f5f2  push       eax
0043f5f3  call       0x46ca4f

// block 0043f5f8
0043f5f8  add        esp, 4
0043f5fb  mov        dword ptr [ebx], 0

// block 0043f601
0043f601  mov        ebx, dword ptr [0x4ce8cc]
0043f607  mov        edi, dword ptr [ebx + 0x4b5124]
0043f60d  add        ebx, 0x4b5124
0043f613  test       edi, edi
0043f615  je         0x43f62d

// block 0043f617
0043f617  call       0x4604e0

// block 0043f61c
0043f61c  mov        ecx, dword ptr [ebx]
0043f61e  push       ecx
0043f61f  call       0x46ca4f

// block 0043f624
0043f624  add        esp, 4
0043f627  mov        dword ptr [ebx], 0

// block 0043f62d
0043f62d  lea        ebx, [ebp + 0x5a80]
0043f633  mov        esi, 0x64

// block 0043f638
0043f638  mov        edi, dword ptr [ebx]
0043f63a  test       edi, edi
0043f63c  je         0x43f64d

// block 0043f63e
0043f63e  push       edi
0043f63f  call       0x43b450

// block 0043f644
0043f644  push       edi
0043f645  call       0x46ca4f

// block 0043f64a
0043f64a  add        esp, 4

// block 0043f64d
0043f64d  add        ebx, 4
0043f650  sub        esi, 1
0043f653  jne        0x43f638

// block 0043f655
0043f655  mov        edx, dword ptr [ebp + 0x66c]
0043f65b  push       edx
0043f65c  lea        ebx, [esi + 1]
0043f65f  call       0x461970

// block 0043f664
0043f664  mov        eax, dword ptr [ebp + 0x5c10]
0043f66a  cmp        eax, esi
0043f66c  je         0x43f67d

// block 0043f66e
0043f66e  push       eax
0043f66f  call       0x46c941

// block 0043f674
0043f674  add        esp, 4
0043f677  mov        dword ptr [ebp + 0x5c10], esi

// block 0043f67d
0043f67d  mov        dword ptr [0x4b4530], esi
0043f683  mov        esi, dword ptr [esp + 0x14]
0043f687  mov        dword ptr [esi], 0x4a3738
0043f68d  call       0x464c40

// block 0043f692
0043f692  mov        ecx, dword ptr [esp + 0x18]
0043f696  mov        dword ptr fs:[0], ecx
0043f69d  pop        ecx
0043f69e  pop        edi
0043f69f  pop        esi
0043f6a0  pop        ebp
0043f6a1  pop        ebx
0043f6a2  add        esp, 0x10
0043f6a5  ret        4

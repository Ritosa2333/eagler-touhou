
// block 004214b0
004214b0  push       ecx
004214b1  mov        eax, dword ptr [0x4b0c4c]
004214b6  push       ebx
004214b7  push       ebp
004214b8  mov        ebp, dword ptr [esp + 0x10]
004214bc  push       esi
004214bd  mov        esi, dword ptr [ebp + 0x6788]
004214c3  inc        esi
004214c4  mov        ecx, esi
004214c6  imul       ecx, ecx, 0x4b4
004214cc  add        eax, 0xa
004214cf  push       edi
004214d0  lea        edi, [ecx + ebp + 0x54b8]
004214d7  movzx      ebx, ax
004214da  mov        eax, dword ptr [edi + 0x494]
004214e0  test       eax, eax
004214e2  je         0x4214eb

// block 004214e4
004214e4  movsx      edx, bx
004214e7  mov        ecx, edi
004214e9  call       eax

// block 004214eb
004214eb  push       edi
004214ec  mov        word ptr [edi + 0x3c4], bx
004214f3  call       0x455630

// block 004214f8
004214f8  cmp        dword ptr [esp + 0x1c], 0
004214fd  jne        0x421524

// block 004214ff
004214ff  mov        eax, dword ptr [edi + 0x494]
00421505  test       eax, eax
00421507  je         0x421512

// block 00421509
00421509  mov        edx, 0x11
0042150e  mov        ecx, edi
00421510  call       eax

// block 00421512
00421512  mov        edx, 0x11
00421517  push       edi
00421518  mov        word ptr [edi + 0x3c4], dx
0042151f  call       0x455630

// block 00421524
00421524  inc        esi
00421525  cmp        esi, 4
00421528  jl         0x42152f

// block 0042152a
0042152a  mov        esi, 1

// block 0042152f
0042152f  mov        eax, dword ptr [0x4b0c50]
00421534  mov        ecx, esi
00421536  imul       ecx, ecx, 0x4b4
0042153c  add        eax, 0xa
0042153f  lea        edi, [ecx + ebp + 0x54b8]
00421546  movzx      ebx, ax
00421549  mov        eax, dword ptr [edi + 0x494]
0042154f  test       eax, eax
00421551  je         0x42155a

// block 00421553
00421553  movsx      edx, bx
00421556  mov        ecx, edi
00421558  call       eax

// block 0042155a
0042155a  push       edi
0042155b  mov        word ptr [edi + 0x3c4], bx
00421562  call       0x455630

// block 00421567
00421567  mov        ebx, dword ptr [esp + 0x1c]
0042156b  cmp        ebx, 1
0042156e  jne        0x421593

// block 00421570
00421570  mov        eax, dword ptr [edi + 0x494]
00421576  test       eax, eax
00421578  je         0x421581

// block 0042157a
0042157a  lea        edx, [ebx + 0x10]
0042157d  mov        ecx, edi
0042157f  call       eax

// block 00421581
00421581  mov        edx, 0x11
00421586  push       edi
00421587  mov        word ptr [edi + 0x3c4], dx
0042158e  call       0x455630

// block 00421593
00421593  inc        esi
00421594  cmp        esi, 4
00421597  jl         0x42159e

// block 00421599
00421599  mov        esi, 1

// block 0042159e
0042159e  mov        eax, dword ptr [0x4b0c54]
004215a3  imul       esi, esi, 0x4b4
004215a9  add        eax, 0xa
004215ac  lea        esi, [esi + ebp + 0x54b8]
004215b3  movzx      edi, ax
004215b6  mov        eax, dword ptr [esi + 0x494]
004215bc  test       eax, eax
004215be  je         0x4215c7

// block 004215c0
004215c0  movsx      edx, di
004215c3  mov        ecx, esi
004215c5  call       eax

// block 004215c7
004215c7  push       esi
004215c8  mov        word ptr [esi + 0x3c4], di
004215cf  call       0x455630

// block 004215d4
004215d4  cmp        ebx, 2
004215d7  jne        0x4215fe

// block 004215d9
004215d9  mov        eax, dword ptr [esi + 0x494]
004215df  test       eax, eax
004215e1  je         0x4215ea

// block 004215e3
004215e3  lea        edx, [ebx + 0xf]
004215e6  mov        ecx, esi
004215e8  call       eax

// block 004215ea
004215ea  mov        ecx, 0x11
004215ef  push       esi
004215f0  mov        word ptr [esi + 0x3c4], cx
004215f7  call       0x455630

// block 004215fc
004215fc  jmp        0x421602

// block 004215fe
004215fe  test       ebx, ebx
00421600  jl         0x421681

// block 00421602
00421602  mov        eax, dword ptr [ebp + 0x6d44]
00421608  push       0
0042160a  push       0x51
0042160c  lea        edx, [esp + 0x24]
00421610  push       edx
00421611  push       eax
00421612  mov        eax, 0x17
00421617  xor        ecx, ecx
00421619  call       0x4615a0

// block 0042161e
0042161e  mov        ecx, dword ptr [esp + 0x1c]
00421622  mov        edx, dword ptr [0x4ce8cc]
00421628  push       ecx
00421629  call       0x461920

// block 0042162e
0042162e  mov        esi, eax
00421630  mov        eax, dword ptr [esi + 0x494]
00421636  lea        edx, [ebx + 7]
00421639  movzx      edi, dx
0042163c  test       eax, eax
0042163e  je         0x421647

// block 00421640
00421640  movsx      edx, di
00421643  mov        ecx, esi
00421645  call       eax

// block 00421647
00421647  push       esi
00421648  mov        word ptr [esi + 0x3c4], di
0042164f  call       0x455630

// block 00421654
00421654  mov        ax, word ptr [ebx*4 + 0x4b0c4c]
0042165c  add        ax, 0xa
00421660  movzx      edi, ax
00421663  mov        eax, dword ptr [esi + 0x494]
00421669  test       eax, eax
0042166b  je         0x421674

// block 0042166d
0042166d  movsx      edx, di
00421670  mov        ecx, esi
00421672  call       eax

// block 00421674
00421674  push       esi
00421675  mov        word ptr [esi + 0x3c4], di
0042167c  call       0x455630

// block 00421681
00421681  pop        edi
00421682  pop        esi
00421683  pop        ebp
00421684  pop        ebx
00421685  pop        ecx
00421686  ret        8

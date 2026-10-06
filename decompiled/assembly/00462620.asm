
// block 00462620
00462620  sub        esp, 8
00462623  test       dword ptr [0x4cee78], 0x8000
0046262d  push       ebp
0046262e  mov        ebp, dword ptr [0x498088]
00462634  push       esi
00462635  mov        esi, dword ptr [0x4ce89c]
0046263b  mov        dword ptr [esp + 0xc], esi
0046263f  je         0x46264e

// block 00462641
00462641  push       0x4cf0f8
00462646  call       ebp

// block 00462648
00462648  inc        byte ptr [0x4cf218]

// block 0046264e
0046264e  mov        eax, dword ptr [esi + 0x3c]
00462651  mov        dword ptr [esp + 8], 0
00462659  test       eax, eax
0046265b  je         0x4626fd

// block 00462661
00462661  push       ebx
00462662  push       edi

// block 00462663
00462663  mov        esi, dword ptr [eax]
00462665  cmp        dword ptr [esi + 8], 0
00462669  mov        ebx, dword ptr [eax + 4]
0046266c  je         0x4626d7

// block 0046266e
0046266e  test       byte ptr [esi + 4], 2
00462672  je         0x4626d3

// block 00462674
00462674  test       dword ptr [0x4cee78], 0x8000
0046267e  je         0x462691

// block 00462680
00462680  push       0x4cf0f8
00462685  call       dword ptr [0x49808c]

// block 0046268b
0046268b  dec        byte ptr [0x4cf218]

// block 00462691
00462691  mov        ecx, dword ptr [esi + 0x20]
00462694  mov        eax, dword ptr [esi + 8]
00462697  call       eax

// block 00462699
00462699  test       dword ptr [0x4cee78], 0x8000
004626a3  mov        edi, eax
004626a5  je         0x4626b4

// block 004626a7
004626a7  push       0x4cf0f8
004626ac  call       ebp

// block 004626ae
004626ae  inc        byte ptr [0x4cf218]

// block 004626b4
004626b4  cmp        edi, 5
004626b7  ja         0x4626d3

// block 004626b9
004626b9  jmp        dword ptr [edi*4 + 0x462724]

// block 004626c0
004626c0  test       byte ptr [esi + 4], 2
004626c4  jne        0x462674

// block 004626c6
004626c6  jmp        0x4626d3

// block 004626c8
004626c8  mov        edx, dword ptr [esp + 0x14]
004626cc  mov        ecx, esi
004626ce  call       0x462890

// block 004626d3
004626d3  inc        dword ptr [esp + 0x10]

// block 004626d7
004626d7  mov        eax, ebx
004626d9  test       ebx, ebx
004626db  jne        0x462663

// block 004626dd
004626dd  jmp        0x4626fb

// block 004626df
004626df  mov        dword ptr [esp + 0x10], 0
004626e7  jmp        0x4626fb

// block 004626e9
004626e9  mov        dword ptr [esp + 0x10], 1
004626f1  jmp        0x4626fb

// block 004626f3
004626f3  mov        dword ptr [esp + 0x10], 0xffffffff

// block 004626fb
004626fb  pop        edi
004626fc  pop        ebx

// block 004626fd
004626fd  test       dword ptr [0x4cee78], 0x8000
00462707  pop        esi
00462708  pop        ebp
00462709  je         0x46271c

// block 0046270b
0046270b  push       0x4cf0f8
00462710  call       dword ptr [0x49808c]

// block 00462716
00462716  dec        byte ptr [0x4cf218]

// block 0046271c
0046271c  mov        eax, dword ptr [esp]
0046271f  add        esp, 8
00462722  ret        


// block 0044a120
0044a120  push       ebp
0044a121  mov        ebp, dword ptr [0x49808c]
0044a127  push       esi
0044a128  mov        esi, dword ptr [ebx + 8]
0044a12b  push       edi
0044a12c  mov        edi, dword ptr [0x4ce89c]
0044a132  test       esi, esi
0044a134  je         0x44a175

// block 0044a136
0044a136  test       dword ptr [0x4cee78], 0x8000
0044a140  je         0x44a153

// block 0044a142
0044a142  push       0x4cf0f8
0044a147  call       dword ptr [0x498088]

// block 0044a14d
0044a14d  inc        byte ptr [0x4cf218]

// block 0044a153
0044a153  mov        ecx, esi
0044a155  mov        edx, edi
0044a157  call       0x462890

// block 0044a15c
0044a15c  test       dword ptr [0x4cee78], 0x8000
0044a166  je         0x44a175

// block 0044a168
0044a168  push       0x4cf0f8
0044a16d  call       ebp

// block 0044a16f
0044a16f  dec        byte ptr [0x4cf218]

// block 0044a175
0044a175  mov        esi, dword ptr [ebx + 0xc]
0044a178  mov        edi, dword ptr [0x4ce89c]
0044a17e  test       esi, esi
0044a180  je         0x44a1c1

// block 0044a182
0044a182  test       dword ptr [0x4cee78], 0x8000
0044a18c  je         0x44a19f

// block 0044a18e
0044a18e  push       0x4cf0f8
0044a193  call       dword ptr [0x498088]

// block 0044a199
0044a199  inc        byte ptr [0x4cf218]

// block 0044a19f
0044a19f  mov        ecx, esi
0044a1a1  mov        edx, edi
0044a1a3  call       0x462890

// block 0044a1a8
0044a1a8  test       dword ptr [0x4cee78], 0x8000
0044a1b2  je         0x44a1c1

// block 0044a1b4
0044a1b4  push       0x4cf0f8
0044a1b9  call       ebp

// block 0044a1bb
0044a1bb  dec        byte ptr [0x4cf218]

// block 0044a1c1
0044a1c1  mov        esi, dword ptr [ebx + 0x24]
0044a1c4  mov        edi, dword ptr [0x4ce89c]
0044a1ca  test       esi, esi
0044a1cc  je         0x44a20d

// block 0044a1ce
0044a1ce  test       dword ptr [0x4cee78], 0x8000
0044a1d8  je         0x44a1eb

// block 0044a1da
0044a1da  push       0x4cf0f8
0044a1df  call       dword ptr [0x498088]

// block 0044a1e5
0044a1e5  inc        byte ptr [0x4cf218]

// block 0044a1eb
0044a1eb  mov        ecx, esi
0044a1ed  mov        edx, edi
0044a1ef  call       0x462890

// block 0044a1f4
0044a1f4  test       dword ptr [0x4cee78], 0x8000
0044a1fe  je         0x44a20d

// block 0044a200
0044a200  push       0x4cf0f8
0044a205  call       ebp

// block 0044a207
0044a207  dec        byte ptr [0x4cf218]

// block 0044a20d
0044a20d  mov        eax, dword ptr [ebx + 0x3c]
0044a210  push       eax
0044a211  mov        dword ptr [0x4b4534], 0
0044a21b  call       0x461a70

// block 0044a220
0044a220  pop        edi
0044a221  pop        esi
0044a222  mov        dword ptr [ebx + 0x3c], 0
0044a229  pop        ebp
0044a22a  ret        

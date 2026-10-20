
// block 0042fbb0
0042fbb0  push       ecx
0042fbb1  push       ebx
0042fbb2  push       esi
0042fbb3  push       edi
0042fbb4  mov        esi, 0x4cefd4
0042fbb9  call       0x463fb0

// block 0042fbbe
0042fbbe  mov        eax, dword ptr [0x4ae590]
0042fbc3  mov        edi, dword ptr [0x49808c]
0042fbc9  mov        ebx, dword ptr [0x4980a4]
0042fbcf  cmp        eax, -1
0042fbd2  je         0x42fc12

// block 0042fbd4
0042fbd4  push       0
0042fbd6  lea        ecx, [esp + 0x10]
0042fbda  push       ecx
0042fbdb  push       0xe
0042fbdd  push       0x4cefbc
0042fbe2  push       eax
0042fbe3  call       dword ptr [0x4980ac]

// block 0042fbe9
0042fbe9  cmp        dword ptr [esp + 0xc], 0xe
0042fbee  je         0x42fc12

// block 0042fbf0
0042fbf0  mov        edx, dword ptr [0x4ae590]
0042fbf6  push       edx
0042fbf7  call       ebx

// block 0042fbf9
0042fbf9  test       dword ptr [0x4cee78], 0x8000
0042fc03  je         0x42fc12

// block 0042fc05
0042fc05  push       0x4cf128
0042fc0a  call       edi

// block 0042fc0c
0042fc0c  dec        byte ptr [0x4cf21a]

// block 0042fc12
0042fc12  mov        edx, dword ptr [0x4cefcc]
0042fc18  mov        esi, 0x28
0042fc1d  call       0x464130

// block 0042fc22
0042fc22  mov        edx, dword ptr [0x4cefd0]
0042fc28  mov        esi, 0xe1000
0042fc2d  call       0x464130

// block 0042fc32
0042fc32  mov        eax, dword ptr [0x4ae590]
0042fc37  cmp        eax, -1
0042fc3a  je         0x42fc58

// block 0042fc3c
0042fc3c  push       eax
0042fc3d  call       ebx

// block 0042fc3f
0042fc3f  test       dword ptr [0x4cee78], 0x8000
0042fc49  je         0x42fc58

// block 0042fc4b
0042fc4b  push       0x4cf128
0042fc50  call       edi

// block 0042fc52
0042fc52  dec        byte ptr [0x4cf21a]

// block 0042fc58
0042fc58  mov        eax, dword ptr [0x4cefcc]
0042fc5d  xor        esi, esi
0042fc5f  cmp        eax, esi
0042fc61  je         0x42fc72

// block 0042fc63
0042fc63  push       eax
0042fc64  call       0x46c941

// block 0042fc69
0042fc69  add        esp, 4
0042fc6c  mov        dword ptr [0x4cefcc], esi

// block 0042fc72
0042fc72  mov        eax, dword ptr [0x4cefd0]
0042fc77  cmp        eax, esi
0042fc79  je         0x42fc8a

// block 0042fc7b
0042fc7b  push       eax
0042fc7c  call       0x46c941

// block 0042fc81
0042fc81  add        esp, 4
0042fc84  mov        dword ptr [0x4cefd0], esi

// block 0042fc8a
0042fc8a  pop        edi
0042fc8b  mov        dword ptr [0x4cefb8], esi
0042fc91  pop        esi
0042fc92  pop        ebx
0042fc93  pop        ecx
0042fc94  ret        


// block 0041ca70
0041ca70  push       esi
0041ca71  mov        esi, dword ptr [eax + 0xc]
0041ca74  push       edi
0041ca75  mov        edi, dword ptr [0x4ce89c]
0041ca7b  test       esi, esi
0041ca7d  je         0x41cac2

// block 0041ca7f
0041ca7f  test       dword ptr [0x4cee78], 0x8000
0041ca89  je         0x41ca9c

// block 0041ca8b
0041ca8b  push       0x4cf0f8
0041ca90  call       dword ptr [0x498088]

// block 0041ca96
0041ca96  inc        byte ptr [0x4cf218]

// block 0041ca9c
0041ca9c  mov        ecx, esi
0041ca9e  mov        edx, edi
0041caa0  call       0x462890

// block 0041caa5
0041caa5  test       dword ptr [0x4cee78], 0x8000
0041caaf  je         0x41cac2

// block 0041cab1
0041cab1  push       0x4cf0f8
0041cab6  call       dword ptr [0x49808c]

// block 0041cabc
0041cabc  dec        byte ptr [0x4cf218]

// block 0041cac2
0041cac2  pop        edi
0041cac3  mov        dword ptr [0x4b43e0], 0
0041cacd  pop        esi
0041cace  ret        

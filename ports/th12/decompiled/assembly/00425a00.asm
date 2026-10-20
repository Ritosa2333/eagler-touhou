
// block 00425a00
00425a00  push       -1
00425a02  push       0x49725c
00425a07  mov        eax, dword ptr fs:[0]
00425a0d  push       eax
00425a0e  push       ebx
00425a0f  push       ebp
00425a10  push       esi
00425a11  push       edi
00425a12  mov        eax, dword ptr [0x4ad138]
00425a17  xor        eax, esp
00425a19  push       eax
00425a1a  lea        eax, [esp + 0x14]
00425a1e  mov        dword ptr fs:[0], eax
00425a24  mov        ebx, dword ptr [esp + 0x24]
00425a28  mov        dword ptr [esp + 0x1c], 0
00425a30  mov        esi, dword ptr [ebx + 8]
00425a33  mov        edi, dword ptr [0x4ce89c]
00425a39  mov        ebp, dword ptr [0x498088]
00425a3f  test       esi, esi
00425a41  je         0x425a82

// block 00425a43
00425a43  test       dword ptr [0x4cee78], 0x8000
00425a4d  je         0x425a5c

// block 00425a4f
00425a4f  push       0x4cf0f8
00425a54  call       ebp

// block 00425a56
00425a56  inc        byte ptr [0x4cf218]

// block 00425a5c
00425a5c  mov        ecx, esi
00425a5e  mov        edx, edi
00425a60  call       0x462890

// block 00425a65
00425a65  test       dword ptr [0x4cee78], 0x8000
00425a6f  je         0x425a82

// block 00425a71
00425a71  push       0x4cf0f8
00425a76  call       dword ptr [0x49808c]

// block 00425a7c
00425a7c  dec        byte ptr [0x4cf218]

// block 00425a82
00425a82  mov        esi, dword ptr [ebx + 0xc]
00425a85  mov        edi, dword ptr [0x4ce89c]
00425a8b  test       esi, esi
00425a8d  je         0x425ace

// block 00425a8f
00425a8f  test       dword ptr [0x4cee78], 0x8000
00425a99  je         0x425aa8

// block 00425a9b
00425a9b  push       0x4cf0f8
00425aa0  call       ebp

// block 00425aa2
00425aa2  inc        byte ptr [0x4cf218]

// block 00425aa8
00425aa8  mov        ecx, esi
00425aaa  mov        edx, edi
00425aac  call       0x462890

// block 00425ab1
00425ab1  test       dword ptr [0x4cee78], 0x8000
00425abb  je         0x425ace

// block 00425abd
00425abd  push       0x4cf0f8
00425ac2  call       dword ptr [0x49808c]

// block 00425ac8
00425ac8  dec        byte ptr [0x4cf218]

// block 00425ace
00425ace  push       0x425900
00425ad3  push       0xa68
00425ad8  push       0x9d8
00425add  add        ebx, 0x14
00425ae0  push       ebx
00425ae1  mov        dword ptr [0x4b44f0], 0
00425aeb  mov        dword ptr [esp + 0x2c], 0xffffffff
00425af3  call       0x46cf1e

// block 00425af8
00425af8  mov        ecx, dword ptr [esp + 0x14]
00425afc  mov        dword ptr fs:[0], ecx
00425b03  pop        ecx
00425b04  pop        edi
00425b05  pop        esi
00425b06  pop        ebp
00425b07  pop        ebx
00425b08  add        esp, 0xc
00425b0b  ret        4

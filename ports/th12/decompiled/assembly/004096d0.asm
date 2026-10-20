
// block 004096d0
004096d0  push       -1
004096d2  push       0x49729c
004096d7  mov        eax, dword ptr fs:[0]
004096dd  push       eax
004096de  push       ebx
004096df  push       ebp
004096e0  push       esi
004096e1  push       edi
004096e2  mov        eax, dword ptr [0x4ad138]
004096e7  xor        eax, esp
004096e9  push       eax
004096ea  lea        eax, [esp + 0x14]
004096ee  mov        dword ptr fs:[0], eax
004096f4  mov        ebx, dword ptr [esp + 0x24]
004096f8  mov        dword ptr [esp + 0x1c], 0
00409700  mov        esi, dword ptr [ebx + 8]
00409703  mov        edi, dword ptr [0x4ce89c]
00409709  mov        ebp, dword ptr [0x498088]
0040970f  test       esi, esi
00409711  je         0x409752

// block 00409713
00409713  test       dword ptr [0x4cee78], 0x8000
0040971d  je         0x40972c

// block 0040971f
0040971f  push       0x4cf0f8
00409724  call       ebp

// block 00409726
00409726  inc        byte ptr [0x4cf218]

// block 0040972c
0040972c  mov        ecx, esi
0040972e  mov        edx, edi
00409730  call       0x462890

// block 00409735
00409735  test       dword ptr [0x4cee78], 0x8000
0040973f  je         0x409752

// block 00409741
00409741  push       0x4cf0f8
00409746  call       dword ptr [0x49808c]

// block 0040974c
0040974c  dec        byte ptr [0x4cf218]

// block 00409752
00409752  mov        esi, dword ptr [ebx + 0xc]
00409755  mov        edi, dword ptr [0x4ce89c]
0040975b  test       esi, esi
0040975d  je         0x40979e

// block 0040975f
0040975f  test       dword ptr [0x4cee78], 0x8000
00409769  je         0x409778

// block 0040976b
0040976b  push       0x4cf0f8
00409770  call       ebp

// block 00409772
00409772  inc        byte ptr [0x4cf218]

// block 00409778
00409778  mov        ecx, esi
0040977a  mov        edx, edi
0040977c  call       0x462890

// block 00409781
00409781  test       dword ptr [0x4cee78], 0x8000
0040978b  je         0x40979e

// block 0040978d
0040978d  push       0x4cf0f8
00409792  call       dword ptr [0x49808c]

// block 00409798
00409798  dec        byte ptr [0x4cf218]

// block 0040979e
0040979e  mov        edx, dword ptr [ebx + 0x4debdc]
004097a4  mov        esi, dword ptr [0x4ce8cc]
004097aa  call       0x461be0

// block 004097af
004097af  push       0x4094f0
004097b4  push       0x7d1
004097b9  push       0x9f8
004097be  add        ebx, 0x64
004097c1  push       ebx
004097c2  mov        dword ptr [0x4b43c8], 0
004097cc  mov        dword ptr [esp + 0x2c], 0xffffffff
004097d4  call       0x46cf1e

// block 004097d9
004097d9  mov        ecx, dword ptr [esp + 0x14]
004097dd  mov        dword ptr fs:[0], ecx
004097e4  pop        ecx
004097e5  pop        edi
004097e6  pop        esi
004097e7  pop        ebp
004097e8  pop        ebx
004097e9  add        esp, 0xc
004097ec  ret        4

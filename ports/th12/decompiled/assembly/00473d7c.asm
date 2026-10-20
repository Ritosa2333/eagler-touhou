
// block 00473d7c
00473d7c  push       ebx
00473d7d  mov        edi, dword ptr [0x498160]
00473d83  call       edi

// block 00473d85
00473d85  test       byte ptr [esi + 0x70], 2
00473d89  jne        0x473e79

// block 00473d8f
00473d8f  test       byte ptr [0x4ad9d4], 1
00473d96  jne        0x473e79

// block 00473d9c
00473d9c  push       0xd
00473d9e  call       0x46ec9a

// block 00473da3
00473da3  pop        ecx
00473da4  and        dword ptr [ebp - 4], 0
00473da8  mov        eax, dword ptr [ebx + 4]
00473dab  mov        dword ptr [0x4b40d0], eax
00473db0  mov        eax, dword ptr [ebx + 8]
00473db3  mov        dword ptr [0x4b40d4], eax
00473db8  mov        eax, dword ptr [ebx + 0xc]
00473dbb  mov        dword ptr [0x4b40d8], eax
00473dc0  xor        eax, eax

// block 00473dc2
00473dc2  mov        dword ptr [ebp - 0x1c], eax
00473dc5  cmp        eax, 5
00473dc8  jge        0x473dda

// block 00473dca
00473dca  mov        cx, word ptr [ebx + eax*2 + 0x10]
00473dcf  mov        word ptr [eax*2 + 0x4b40c4], cx
00473dd7  inc        eax
00473dd8  jmp        0x473dc2

// block 00473dda
00473dda  xor        eax, eax

// block 00473ddc
00473ddc  mov        dword ptr [ebp - 0x1c], eax
00473ddf  cmp        eax, 0x101
00473de4  jge        0x473df3

// block 00473de6
00473de6  mov        cl, byte ptr [eax + ebx + 0x1c]
00473dea  mov        byte ptr [eax + 0x4ad6d0], cl
00473df0  inc        eax
00473df1  jmp        0x473ddc

// block 00473df3
00473df3  xor        eax, eax

// block 00473df5
00473df5  mov        dword ptr [ebp - 0x1c], eax
00473df8  cmp        eax, 0x100
00473dfd  jge        0x473e0f

// block 00473dff
00473dff  mov        cl, byte ptr [eax + ebx + 0x11d]
00473e06  mov        byte ptr [eax + 0x4ad7d8], cl
00473e0c  inc        eax
00473e0d  jmp        0x473df5

// block 00473e0f
00473e0f  push       dword ptr [0x4ad8d8]
00473e15  call       dword ptr [0x49815c]

// block 00473e1b
00473e1b  test       eax, eax
00473e1d  jne        0x473e32

// block 00473e1f
00473e1f  mov        eax, dword ptr [0x4ad8d8]
00473e24  cmp        eax, 0x4ad4b0
00473e29  je         0x473e32

// block 00473e2b
00473e2b  push       eax
00473e2c  call       0x46c941

// block 00473e31
00473e31  pop        ecx

// block 00473e32
00473e32  mov        dword ptr [0x4ad8d8], ebx
00473e38  push       ebx
00473e39  call       edi

// block 00473e3b
00473e3b  mov        dword ptr [ebp - 4], 0xfffffffe
00473e42  call       0x473e49

// block 00473e47
00473e47  jmp        0x473e79


// block 0043ceb0
0043ceb0  push       ebx
0043ceb1  push       ebp
0043ceb2  push       esi
0043ceb3  push       edi
0043ceb4  push       0x448
0043ceb9  mov        esi, eax
0043cebb  push       0
0043cebd  push       esi
0043cebe  call       0x477420

// block 0043cec3
0043cec3  mov        ebx, dword ptr [0x498088]
0043cec9  mov        ebp, dword ptr [0x49808c]
0043cecf  mov        eax, 0x5453
0043ced4  mov        word ptr [esi], ax
0043ced7  mov        ecx, 2
0043cedc  mov        word ptr [esi + 2], cx
0043cee0  mov        dword ptr [esi + 8], 0x448
0043cee7  mov        edx, dword ptr [0x4a0ee4]
0043ceed  mov        dword ptr [esi + 0xc], edx
0043cef0  mov        eax, dword ptr [0x4a0ee8]
0043cef5  mov        dword ptr [esi + 0x10], eax
0043cef8  mov        cl, byte ptr [0x4a0eec]
0043cefe  add        esp, 0xc
0043cf01  mov        byte ptr [esi + 0x14], cl
0043cf04  add        esi, 0x46
0043cf07  mov        edi, 0x200
0043cf0c  lea        esp, [esp]

// block 0043cf10
0043cf10  test       dword ptr [0x4cee78], 0x8000
0043cf1a  je         0x43cf29

// block 0043cf1c
0043cf1c  push       0x4cf1e8
0043cf21  call       ebx

// block 0043cf23
0043cf23  inc        byte ptr [0x4cf222]

// block 0043cf29
0043cf29  mov        edx, dword ptr [0x4ce568]
0043cf2f  inc        dword ptr [0x4ce56c]
0043cf35  xor        edx, 0x9630
0043cf3b  sub        edx, 0x6553
0043cf41  movzx      ecx, dx
0043cf44  mov        ax, cx
0043cf47  add        ecx, ecx
0043cf49  shr        ax, 0xe
0043cf4d  add        ecx, ecx
0043cf4f  add        ax, cx
0043cf52  test       dword ptr [0x4cee78], 0x8000
0043cf5c  mov        word ptr [0x4ce568], ax
0043cf62  je         0x43cf77

// block 0043cf64
0043cf64  push       0x4cf1e8
0043cf69  call       ebp

// block 0043cf6b
0043cf6b  dec        byte ptr [0x4cf222]
0043cf71  mov        ax, word ptr [0x4ce568]

// block 0043cf77
0043cf77  mov        word ptr [esi], ax
0043cf7a  add        esi, 2
0043cf7d  sub        edi, 1
0043cf80  jne        0x43cf10

// block 0043cf82
0043cf82  pop        edi
0043cf83  pop        esi
0043cf84  pop        ebp
0043cf85  pop        ebx
0043cf86  ret        

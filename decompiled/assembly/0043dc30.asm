
// block 0043dc30
0043dc30  push       -1
0043dc32  push       0x49700b
0043dc37  mov        eax, dword ptr fs:[0]
0043dc3d  push       eax
0043dc3e  push       ebx
0043dc3f  push       ebp
0043dc40  push       esi
0043dc41  push       edi
0043dc42  mov        eax, dword ptr [0x4ad138]
0043dc47  xor        eax, esp
0043dc49  push       eax
0043dc4a  lea        eax, [esp + 0x14]
0043dc4e  mov        dword ptr fs:[0], eax
0043dc54  mov        ebx, dword ptr [esp + 0x24]
0043dc58  mov        dword ptr [esp + 0x1c], 0
0043dc60  mov        esi, dword ptr [ebx + 8]
0043dc63  mov        edi, dword ptr [0x4ce89c]
0043dc69  mov        ebp, dword ptr [0x498088]
0043dc6f  test       esi, esi
0043dc71  je         0x43dcb2

// block 0043dc73
0043dc73  test       dword ptr [0x4cee78], 0x8000
0043dc7d  je         0x43dc8c

// block 0043dc7f
0043dc7f  push       0x4cf0f8
0043dc84  call       ebp

// block 0043dc86
0043dc86  inc        byte ptr [0x4cf218]

// block 0043dc8c
0043dc8c  mov        ecx, esi
0043dc8e  mov        edx, edi
0043dc90  call       0x462890

// block 0043dc95
0043dc95  test       dword ptr [0x4cee78], 0x8000
0043dc9f  je         0x43dcb2

// block 0043dca1
0043dca1  push       0x4cf0f8
0043dca6  call       dword ptr [0x49808c]

// block 0043dcac
0043dcac  dec        byte ptr [0x4cf218]

// block 0043dcb2
0043dcb2  mov        esi, dword ptr [ebx + 0xc]
0043dcb5  mov        edi, dword ptr [0x4ce89c]
0043dcbb  test       esi, esi
0043dcbd  je         0x43dcfe

// block 0043dcbf
0043dcbf  test       dword ptr [0x4cee78], 0x8000
0043dcc9  je         0x43dcd8

// block 0043dccb
0043dccb  push       0x4cf0f8
0043dcd0  call       ebp

// block 0043dcd2
0043dcd2  inc        byte ptr [0x4cf218]

// block 0043dcd8
0043dcd8  mov        ecx, esi
0043dcda  mov        edx, edi
0043dcdc  call       0x462890

// block 0043dce1
0043dce1  test       dword ptr [0x4cee78], 0x8000
0043dceb  je         0x43dcfe

// block 0043dced
0043dced  push       0x4cf0f8
0043dcf2  call       dword ptr [0x49808c]

// block 0043dcf8
0043dcf8  dec        byte ptr [0x4cf218]

// block 0043dcfe
0043dcfe  mov        eax, dword ptr [ebx + 0x490]
0043dd04  xor        ecx, ecx
0043dd06  mov        dword ptr [0x4b4524], ecx
0043dd0c  cmp        eax, ecx
0043dd0e  je         0x43dd25

// block 0043dd10
0043dd10  push       eax
0043dd11  call       0x46c941

// block 0043dd16
0043dd16  add        esp, 4
0043dd19  mov        dword ptr [ebx + 0x490], 0
0043dd23  jmp        0x43dd2b

// block 0043dd25
0043dd25  mov        dword ptr [ebx + 0x490], ecx

// block 0043dd2b
0043dd2b  mov        ecx, dword ptr [esp + 0x14]
0043dd2f  mov        dword ptr fs:[0], ecx
0043dd36  pop        ecx
0043dd37  pop        edi
0043dd38  pop        esi
0043dd39  pop        ebp
0043dd3a  pop        ebx
0043dd3b  add        esp, 0xc
0043dd3e  ret        4

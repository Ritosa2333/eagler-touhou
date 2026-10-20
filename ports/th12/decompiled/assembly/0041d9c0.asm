
// block 0041d9c0
0041d9c0  push       ebx
0041d9c1  push       ebp
0041d9c2  mov        ebp, dword ptr [esp + 0xc]
0041d9c6  push       esi
0041d9c7  mov        esi, dword ptr [0x4ce8cc]
0041d9cd  xor        ebx, ebx
0041d9cf  test       byte ptr [0x4b0ce0], 9
0041d9d6  push       edi
0041d9d7  jne        0x41d9fd

// block 0041d9d9
0041d9d9  mov        edi, dword ptr [esi + 0x4b512c]
0041d9df  add        esi, 0x4b512c
0041d9e5  cmp        edi, ebx
0041d9e7  je         0x41da08

// block 0041d9e9
0041d9e9  call       0x4604e0

// block 0041d9ee
0041d9ee  mov        eax, dword ptr [esi]
0041d9f0  push       eax
0041d9f1  call       0x46ca4f

// block 0041d9f6
0041d9f6  add        esp, 4
0041d9f9  mov        dword ptr [esi], ebx
0041d9fb  jmp        0x41da08

// block 0041d9fd
0041d9fd  mov        edx, dword ptr [ebp + 0x6ce4]
0041da03  call       0x461be0

// block 0041da08
0041da08  mov        esi, dword ptr [ebp + 0x6d30]
0041da0e  mov        dword ptr [ebp + 0x6ce4], ebx
0041da14  cmp        esi, ebx
0041da16  je         0x41da2c

// block 0041da18
0041da18  call       0x41cd90

// block 0041da1d
0041da1d  push       esi
0041da1e  call       0x46ca4f

// block 0041da23
0041da23  add        esp, 4
0041da26  mov        dword ptr [ebp + 0x6d30], ebx

// block 0041da2c
0041da2c  test       byte ptr [0x4b0ce0], 9
0041da33  jne        0x41da5c

// block 0041da35
0041da35  mov        eax, dword ptr [ebp + 0x6d34]
0041da3b  cmp        eax, ebx
0041da3d  je         0x41da4e

// block 0041da3f
0041da3f  push       eax
0041da40  call       0x46c941

// block 0041da45
0041da45  add        esp, 4
0041da48  mov        dword ptr [ebp + 0x6d34], ebx

// block 0041da4e
0041da4e  mov        dword ptr [ebp + 0x6d34], ebx
0041da54  mov        dword ptr [0x4ce8a0], ebx
0041da5a  jmp        0x41da68

// block 0041da5c
0041da5c  mov        ecx, dword ptr [ebp + 0x6d34]
0041da62  mov        dword ptr [0x4ce8a0], ecx

// block 0041da68
0041da68  mov        eax, dword ptr [ebp + 8]
0041da6b  cmp        eax, ebx
0041da6d  je         0x41da73

// block 0041da6f
0041da6f  and        dword ptr [eax + 4], 0xfffffffd

// block 0041da73
0041da73  mov        edx, dword ptr [ebp + 0x6c68]
0041da79  push       edx
0041da7a  call       0x461a70

// block 0041da7f
0041da7f  mov        dword ptr [ebp + 0x6c68], ebx
0041da85  mov        eax, dword ptr [ebp + 0x6c6c]
0041da8b  push       eax
0041da8c  call       0x461a70

// block 0041da91
0041da91  mov        dword ptr [ebp + 0x6c6c], ebx
0041da97  mov        ecx, dword ptr [ebp + 0x6c78]
0041da9d  push       ecx
0041da9e  call       0x461a70

// block 0041daa3
0041daa3  xor        eax, eax
0041daa5  mov        dword ptr [ebp + 0x6c78], ebx
0041daab  mov        dword ptr [ebp + 0x6c7c], eax
0041dab1  mov        dword ptr [ebp + 0x6c80], eax
0041dab7  mov        dword ptr [ebp + 0x6c84], eax
0041dabd  mov        dword ptr [ebp + 0x6c88], eax
0041dac3  mov        dword ptr [ebp + 0x6c8c], eax
0041dac9  mov        dword ptr [ebp + 0x6c90], eax
0041dacf  mov        dword ptr [ebp + 0x6c94], eax
0041dad5  mov        dword ptr [ebp + 0x6c98], eax
0041dadb  mov        dword ptr [ebp + 0x6c9c], eax
0041dae1  mov        dword ptr [ebp + 0x6ca0], eax
0041dae7  lea        esi, [ebp + 0x6c40]
0041daed  mov        dword ptr [esp + 0x14], 0xa
0041daf5  mov        edi, 0x10000000
0041dafa  lea        ebx, [ebx]

// block 0041db00
0041db00  mov        ecx, dword ptr [esi]
0041db02  cmp        ecx, ebx
0041db04  je         0x41db71

// block 0041db06
0041db06  mov        edx, dword ptr [0x4ce8cc]
0041db0c  mov        eax, dword ptr [edx + 0x8856b8]
0041db12  cmp        eax, ebx
0041db14  je         0x41db28

// block 0041db16
0041db16  mov        edi, dword ptr [eax]
0041db18  cmp        dword ptr [edi], ecx
0041db1a  je         0x41db41

// block 0041db1c
0041db1c  mov        eax, dword ptr [eax + 4]
0041db1f  cmp        eax, ebx
0041db21  jne        0x41db16

// block 0041db23
0041db23  mov        edi, 0x10000000

// block 0041db28
0041db28  mov        eax, dword ptr [edx + 0x8856c0]
0041db2e  cmp        eax, ebx
0041db30  je         0x41db71

// block 0041db32
0041db32  mov        edx, dword ptr [eax]
0041db34  cmp        dword ptr [edx], ecx
0041db36  je         0x41db4a

// block 0041db38
0041db38  mov        eax, dword ptr [eax + 4]
0041db3b  cmp        eax, ebx
0041db3d  jne        0x41db32

// block 0041db3f
0041db3f  jmp        0x41db71

// block 0041db41
0041db41  mov        eax, edi
0041db43  mov        edi, 0x10000000
0041db48  jmp        0x41db4c

// block 0041db4a
0041db4a  mov        eax, edx

// block 0041db4c
0041db4c  cmp        eax, ebx
0041db4e  je         0x41db71

// block 0041db50
0041db50  or         dword ptr [eax + 0x47c], edi
0041db56  cmp        dword ptr [eax + 0x18], ebx
0041db59  jne        0x41db71

// block 0041db5b
0041db5b  mov        eax, dword ptr [eax + 0x14]
0041db5e  cmp        eax, ebx
0041db60  je         0x41db71

// block 0041db62
0041db62  mov        ecx, dword ptr [eax]
0041db64  or         dword ptr [ecx + 0x47c], edi
0041db6a  mov        eax, dword ptr [eax + 4]
0041db6d  cmp        eax, ebx
0041db6f  jne        0x41db62

// block 0041db71
0041db71  mov        dword ptr [esi], ebx
0041db73  add        esi, 4
0041db76  sub        dword ptr [esp + 0x14], 1
0041db7b  jne        0x41db00

// block 0041db7d
0041db7d  or         dword ptr [ebp + 0x6d18], 0x1c0
0041db87  pop        edi
0041db88  pop        esi
0041db89  mov        dword ptr [ebp + 0x6cf4], ebx
0041db8f  pop        ebp
0041db90  pop        ebx
0041db91  ret        4

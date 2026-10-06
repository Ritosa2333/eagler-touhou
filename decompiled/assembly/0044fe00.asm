
// block 0044fe00
0044fe00  push       ebx
0044fe01  mov        ebx, dword ptr [esp + 8]
0044fe05  push       esi
0044fe06  mov        esi, dword ptr [esp + 0x10]
0044fe0a  push       edi
0044fe0b  mov        edi, dword ptr [esp + 0x18]
0044fe0f  cmp        esi, 0x1c
0044fe12  ja         0x44feb6

// block 0044fe18
0044fe18  je         0x44fe8f

// block 0044fe1a
0044fe1a  cmp        esi, 5
0044fe1d  je         0x44fe55

// block 0044fe1f
0044fe1f  cmp        esi, 0x10
0044fe22  je         0x44fe36

// block 0044fe24
0044fe24  cmp        esi, 0x14
0044fe27  jne        0x44ff01

// block 0044fe2d
0044fe2d  pop        edi
0044fe2e  lea        eax, [esi - 0x13]
0044fe31  pop        esi
0044fe32  pop        ebx
0044fe33  ret        0x10

// block 0044fe36
0044fe36  mov        eax, dword ptr [0x4cee78]
0044fe3b  and        eax, 0xfffffeff
0044fe40  pop        edi
0044fe41  or         eax, 0x80
0044fe46  pop        esi
0044fe47  mov        dword ptr [0x4cee78], eax
0044fe4c  mov        eax, 1
0044fe51  pop        ebx
0044fe52  ret        0x10

// block 0044fe55
0044fe55  mov        ecx, dword ptr [0x4cf428]
0044fe5b  test       cl, 1
0044fe5e  je         0x44ff01

// block 0044fe64
0044fe64  mov        eax, edi
0044fe66  sub        eax, 2
0044fe69  jne        0x44ff01

// block 0044fe6f
0044fe6f  mov        edx, dword ptr [esp + 0x1c]
0044fe73  push       edx
0044fe74  push       edi
0044fe75  and        ecx, 0xfffffff3
0044fe78  push       esi
0044fe79  or         ecx, 2
0044fe7c  push       ebx
0044fe7d  mov        dword ptr [0x4cf428], ecx
0044fe83  call       dword ptr [0x498260]

// block 0044fe89
0044fe89  pop        edi
0044fe8a  pop        esi
0044fe8b  pop        ebx
0044fe8c  ret        0x10

// block 0044fe8f
0044fe8f  mov        edx, dword ptr [esp + 0x1c]
0044fe93  xor        ecx, ecx
0044fe95  test       edi, edi
0044fe97  push       edx
0044fe98  sete       cl
0044fe9b  push       edi
0044fe9c  push       esi
0044fe9d  push       ebx
0044fe9e  mov        dword ptr [0x4cf3fc], edi
0044fea4  mov        dword ptr [0x4cf400], ecx
0044feaa  call       dword ptr [0x498260]

// block 0044feb0
0044feb0  pop        edi
0044feb1  pop        esi
0044feb2  pop        ebx
0044feb3  ret        0x10

// block 0044feb6
0044feb6  cmp        esi, 0x20
0044feb9  je         0x44ff15

// block 0044febb
0044febb  cmp        esi, 0x112
0044fec1  je         0x44fee6

// block 0044fec3
0044fec3  cmp        esi, 0x201
0044fec9  jne        0x44ff01

// block 0044fecb
0044fecb  push       ebx
0044fecc  call       dword ptr [0x498234]

// block 0044fed2
0044fed2  mov        edx, dword ptr [esp + 0x1c]
0044fed6  push       edx
0044fed7  push       edi
0044fed8  push       esi
0044fed9  push       ebx
0044feda  call       dword ptr [0x498260]

// block 0044fee0
0044fee0  pop        edi
0044fee1  pop        esi
0044fee2  pop        ebx
0044fee3  ret        0x10

// block 0044fee6
0044fee6  mov        eax, edi
0044fee8  and        eax, 0xfff0
0044feed  sub        eax, 0xf090
0044fef2  je         0x44ff97

// block 0044fef8
0044fef8  sub        eax, 0x70
0044fefb  je         0x44ff97

// block 0044ff01
0044ff01  mov        edx, dword ptr [esp + 0x1c]
0044ff05  push       edx
0044ff06  push       edi
0044ff07  push       esi
0044ff08  push       ebx
0044ff09  call       dword ptr [0x498260]

// block 0044ff0f
0044ff0f  pop        edi
0044ff10  pop        esi
0044ff11  pop        ebx
0044ff12  ret        0x10

// block 0044ff15
0044ff15  cmp        dword ptr [0x4ce9fc], 0
0044ff1c  jne        0x44ff7b

// block 0044ff1e
0044ff1e  cmp        dword ptr [0x4cf400], 0
0044ff25  je         0x44ff54

// block 0044ff27
0044ff27  push       0x7f00
0044ff2c  push       0
0044ff2e  call       dword ptr [0x49827c]

// block 0044ff34
0044ff34  push       eax
0044ff35  call       dword ptr [0x49822c]

// block 0044ff3b
0044ff3b  mov        esi, dword ptr [0x498240]

// block 0044ff41
0044ff41  push       1
0044ff43  call       esi

// block 0044ff45
0044ff45  test       eax, eax
0044ff47  jl         0x44ff41

// block 0044ff49
0044ff49  pop        edi
0044ff4a  pop        esi
0044ff4b  mov        eax, 1
0044ff50  pop        ebx
0044ff51  ret        0x10

// block 0044ff54
0044ff54  mov        esi, dword ptr [0x498240]
0044ff5a  lea        ebx, [ebx]

// block 0044ff60
0044ff60  push       0
0044ff62  call       esi

// block 0044ff64
0044ff64  test       eax, eax
0044ff66  jge        0x44ff60

// block 0044ff68
0044ff68  push       0
0044ff6a  call       dword ptr [0x49822c]

// block 0044ff70
0044ff70  pop        edi
0044ff71  pop        esi
0044ff72  mov        eax, 1
0044ff77  pop        ebx
0044ff78  ret        0x10

// block 0044ff7b
0044ff7b  push       0x7f00
0044ff80  push       0
0044ff82  call       dword ptr [0x49827c]

// block 0044ff88
0044ff88  push       eax
0044ff89  call       dword ptr [0x49822c]

// block 0044ff8f
0044ff8f  push       1
0044ff91  call       dword ptr [0x498240]

// block 0044ff97
0044ff97  pop        edi
0044ff98  pop        esi
0044ff99  mov        eax, 1
0044ff9e  pop        ebx
0044ff9f  ret        0x10

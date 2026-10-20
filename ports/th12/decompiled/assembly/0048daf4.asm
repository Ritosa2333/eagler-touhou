
// block 0048daf4
0048daf4  mov        edi, edi
0048daf6  push       ebp
0048daf7  mov        ebp, esp
0048daf9  sub        esp, 0xc
0048dafc  push       ebx
0048dafd  xor        ebx, ebx
0048daff  push       esi
0048db00  cmp        edi, ebx
0048db02  jne        0x48db22

// block 0048db04
0048db04  call       0x46e96f

// block 0048db09
0048db09  push       0x16

// block 0048db0b
0048db0b  pop        esi
0048db0c  mov        dword ptr [eax], esi

// block 0048db0e
0048db0e  push       ebx
0048db0f  push       ebx
0048db10  push       ebx
0048db11  push       ebx
0048db12  push       ebx
0048db13  call       0x470f2d

// block 0048db18
0048db18  add        esp, 0x14
0048db1b  mov        eax, esi
0048db1d  jmp        0x48dbe6

// block 0048db22
0048db22  cmp        dword ptr [ebp + 0x10], ebx
0048db25  jbe        0x48db04

// block 0048db27
0048db27  xor        eax, eax
0048db29  cmp        dword ptr [ebp + 0x18], ebx
0048db2c  mov        byte ptr [edi], bl
0048db2e  setne      al
0048db31  inc        eax
0048db32  cmp        dword ptr [ebp + 0x10], eax
0048db35  ja         0x48db40

// block 0048db37
0048db37  call       0x46e96f

// block 0048db3c
0048db3c  push       0x22
0048db3e  jmp        0x48db0b

// block 0048db40
0048db40  mov        eax, dword ptr [ebp + 0x14]
0048db43  add        eax, -2
0048db46  cmp        eax, 0x22
0048db49  ja         0x48db04

// block 0048db4b
0048db4b  mov        edx, dword ptr [ebp + 8]
0048db4e  mov        eax, dword ptr [ebp + 0xc]
0048db51  mov        dword ptr [ebp - 4], ebx
0048db54  mov        esi, edi
0048db56  cmp        dword ptr [ebp + 0x18], ebx
0048db59  je         0x48db6e

// block 0048db5b
0048db5b  neg        edx
0048db5d  adc        eax, ebx
0048db5f  mov        byte ptr [edi], 0x2d
0048db62  lea        esi, [edi + 1]
0048db65  mov        dword ptr [ebp - 4], 1
0048db6c  neg        eax

// block 0048db6e
0048db6e  mov        dword ptr [ebp + 0x18], esi
0048db71  mov        dword ptr [ebp + 0xc], ebx

// block 0048db74
0048db74  push       dword ptr [ebp + 0xc]
0048db77  push       dword ptr [ebp + 0x14]
0048db7a  push       eax
0048db7b  push       edx
0048db7c  call       0x47c890

// block 0048db81
0048db81  mov        dword ptr [ebp - 8], ebx
0048db84  mov        ebx, edx
0048db86  mov        edx, eax
0048db88  mov        eax, ebx
0048db8a  cmp        ecx, 9
0048db8d  jbe        0x48db94

// block 0048db8f
0048db8f  add        cl, 0x57
0048db92  jmp        0x48db97

// block 0048db94
0048db94  add        cl, 0x30

// block 0048db97
0048db97  mov        byte ptr [esi], cl
0048db99  mov        ecx, dword ptr [ebp - 4]
0048db9c  inc        esi
0048db9d  inc        ecx
0048db9e  mov        dword ptr [ebp - 4], ecx
0048dba1  test       eax, eax
0048dba3  ja         0x48dba9

// block 0048dba5
0048dba5  test       edx, edx
0048dba7  jbe        0x48dbb1

// block 0048dba9
0048dba9  cmp        ecx, dword ptr [ebp + 0x10]
0048dbac  jb         0x48db74

// block 0048dbae
0048dbae  mov        ecx, dword ptr [ebp - 4]

// block 0048dbb1
0048dbb1  cmp        ecx, dword ptr [ebp + 0x10]
0048dbb4  jb         0x48dbcc

// block 0048dbb6
0048dbb6  mov        byte ptr [edi], 0
0048dbb9  call       0x46e96f

// block 0048dbbe
0048dbbe  push       0x22
0048dbc0  pop        ecx
0048dbc1  mov        dword ptr [eax], ecx
0048dbc3  mov        esi, ecx
0048dbc5  xor        ebx, ebx
0048dbc7  jmp        0x48db0e

// block 0048dbcc
0048dbcc  mov        byte ptr [esi], 0
0048dbcf  dec        esi

// block 0048dbd0
0048dbd0  mov        eax, dword ptr [ebp + 0x18]
0048dbd3  mov        dl, byte ptr [eax]
0048dbd5  mov        cl, byte ptr [esi]
0048dbd7  mov        byte ptr [esi], dl
0048dbd9  dec        esi
0048dbda  mov        byte ptr [eax], cl
0048dbdc  inc        eax
0048dbdd  mov        dword ptr [ebp + 0x18], eax
0048dbe0  cmp        eax, esi
0048dbe2  jb         0x48dbd0

// block 0048dbe4
0048dbe4  xor        eax, eax

// block 0048dbe6
0048dbe6  pop        esi
0048dbe7  pop        ebx
0048dbe8  leave      
0048dbe9  ret        0x14

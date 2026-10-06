
// block 0048dec0
0048dec0  mov        edi, edi
0048dec2  push       ebp
0048dec3  mov        ebp, esp
0048dec5  mov        ecx, dword ptr [ebp + 0x14]
0048dec8  push       ebx
0048dec9  push       esi
0048deca  mov        esi, dword ptr [ebp + 8]
0048decd  xor        ebx, ebx
0048decf  push       edi
0048ded0  mov        edi, dword ptr [ecx + 0xc]
0048ded3  cmp        esi, ebx
0048ded5  jne        0x48def5

// block 0048ded7
0048ded7  call       0x46e96f

// block 0048dedc
0048dedc  push       0x16
0048dede  pop        esi
0048dedf  mov        dword ptr [eax], esi

// block 0048dee1
0048dee1  push       ebx
0048dee2  push       ebx
0048dee3  push       ebx
0048dee4  push       ebx
0048dee5  push       ebx
0048dee6  call       0x470f2d

// block 0048deeb
0048deeb  add        esp, 0x14
0048deee  mov        eax, esi
0048def0  jmp        0x48df7a

// block 0048def5
0048def5  cmp        dword ptr [ebp + 0xc], ebx
0048def8  jbe        0x48ded7

// block 0048defa
0048defa  mov        edx, dword ptr [ebp + 0x10]
0048defd  cmp        edx, ebx
0048deff  mov        byte ptr [esi], bl
0048df01  jle        0x48df07

// block 0048df03
0048df03  mov        eax, edx
0048df05  jmp        0x48df09

// block 0048df07
0048df07  xor        eax, eax

// block 0048df09
0048df09  inc        eax
0048df0a  cmp        dword ptr [ebp + 0xc], eax
0048df0d  ja         0x48df1d

// block 0048df0f
0048df0f  call       0x46e96f

// block 0048df14
0048df14  push       0x22
0048df16  pop        ecx
0048df17  mov        dword ptr [eax], ecx
0048df19  mov        esi, ecx
0048df1b  jmp        0x48dee1

// block 0048df1d
0048df1d  cmp        edx, ebx
0048df1f  mov        byte ptr [esi], 0x30
0048df22  lea        eax, [esi + 1]
0048df25  jle        0x48df41

// block 0048df27
0048df27  mov        cl, byte ptr [edi]
0048df29  cmp        cl, bl
0048df2b  je         0x48df33

// block 0048df2d
0048df2d  movsx      ecx, cl
0048df30  inc        edi
0048df31  jmp        0x48df36

// block 0048df33
0048df33  push       0x30
0048df35  pop        ecx

// block 0048df36
0048df36  mov        byte ptr [eax], cl
0048df38  inc        eax
0048df39  dec        edx
0048df3a  cmp        edx, ebx
0048df3c  jg         0x48df27

// block 0048df3e
0048df3e  mov        ecx, dword ptr [ebp + 0x14]

// block 0048df41
0048df41  cmp        edx, ebx
0048df43  mov        byte ptr [eax], bl
0048df45  jl         0x48df59

// block 0048df47
0048df47  cmp        byte ptr [edi], 0x35
0048df4a  jl         0x48df59

// block 0048df4c
0048df4c  jmp        0x48df51

// block 0048df4e
0048df4e  mov        byte ptr [eax], 0x30

// block 0048df51
0048df51  dec        eax
0048df52  cmp        byte ptr [eax], 0x39
0048df55  je         0x48df4e

// block 0048df57
0048df57  inc        byte ptr [eax]

// block 0048df59
0048df59  cmp        byte ptr [esi], 0x31
0048df5c  jne        0x48df63

// block 0048df5e
0048df5e  inc        dword ptr [ecx + 4]
0048df61  jmp        0x48df78

// block 0048df63
0048df63  lea        edi, [esi + 1]
0048df66  push       edi
0048df67  call       0x475860

// block 0048df6c
0048df6c  inc        eax
0048df6d  push       eax
0048df6e  push       edi
0048df6f  push       esi
0048df70  call       0x47a6a0

// block 0048df75
0048df75  add        esp, 0x10

// block 0048df78
0048df78  xor        eax, eax

// block 0048df7a
0048df7a  pop        edi
0048df7b  pop        esi
0048df7c  pop        ebx
0048df7d  pop        ebp
0048df7e  ret        

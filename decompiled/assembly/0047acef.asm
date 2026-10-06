
// block 0047acef
0047acef  mov        edi, edi
0047acf1  push       ebp
0047acf2  mov        ebp, esp
0047acf4  push       ecx
0047acf5  mov        ecx, dword ptr [ebp + 0x10]
0047acf8  push       ebx
0047acf9  xor        eax, eax
0047acfb  push       esi
0047acfc  mov        dword ptr [edi], eax
0047acfe  mov        esi, edx
0047ad00  mov        edx, dword ptr [ebp + 0xc]
0047ad03  mov        dword ptr [ecx], 1
0047ad09  cmp        dword ptr [ebp + 8], eax
0047ad0c  je         0x47ad17

// block 0047ad0e
0047ad0e  mov        ebx, dword ptr [ebp + 8]
0047ad11  add        dword ptr [ebp + 8], 4
0047ad15  mov        dword ptr [ebx], edx

// block 0047ad17
0047ad17  mov        dword ptr [ebp - 4], eax

// block 0047ad1a
0047ad1a  cmp        byte ptr [esi], 0x22
0047ad1d  jne        0x47ad2f

// block 0047ad1f
0047ad1f  xor        eax, eax
0047ad21  cmp        dword ptr [ebp - 4], eax
0047ad24  mov        bl, 0x22
0047ad26  sete       al
0047ad29  inc        esi
0047ad2a  mov        dword ptr [ebp - 4], eax
0047ad2d  jmp        0x47ad6b

// block 0047ad2f
0047ad2f  inc        dword ptr [edi]
0047ad31  test       edx, edx
0047ad33  je         0x47ad3d

// block 0047ad35
0047ad35  mov        al, byte ptr [esi]
0047ad37  mov        byte ptr [edx], al
0047ad39  inc        edx
0047ad3a  mov        dword ptr [ebp + 0xc], edx

// block 0047ad3d
0047ad3d  mov        bl, byte ptr [esi]
0047ad3f  movzx      eax, bl
0047ad42  push       eax
0047ad43  inc        esi
0047ad44  call       0x48c7d7

// block 0047ad49
0047ad49  pop        ecx
0047ad4a  test       eax, eax
0047ad4c  je         0x47ad61

// block 0047ad4e
0047ad4e  inc        dword ptr [edi]
0047ad50  cmp        dword ptr [ebp + 0xc], 0
0047ad54  je         0x47ad60

// block 0047ad56
0047ad56  mov        ecx, dword ptr [ebp + 0xc]
0047ad59  mov        al, byte ptr [esi]
0047ad5b  inc        dword ptr [ebp + 0xc]
0047ad5e  mov        byte ptr [ecx], al

// block 0047ad60
0047ad60  inc        esi

// block 0047ad61
0047ad61  mov        edx, dword ptr [ebp + 0xc]
0047ad64  mov        ecx, dword ptr [ebp + 0x10]
0047ad67  test       bl, bl
0047ad69  je         0x47ad9d

// block 0047ad6b
0047ad6b  cmp        dword ptr [ebp - 4], 0
0047ad6f  jne        0x47ad1a

// block 0047ad71
0047ad71  cmp        bl, 0x20
0047ad74  je         0x47ad7b

// block 0047ad76
0047ad76  cmp        bl, 9
0047ad79  jne        0x47ad1a

// block 0047ad7b
0047ad7b  test       edx, edx
0047ad7d  je         0x47ad83

// block 0047ad7f
0047ad7f  mov        byte ptr [edx - 1], 0

// block 0047ad83
0047ad83  and        dword ptr [ebp - 4], 0

// block 0047ad87
0047ad87  cmp        byte ptr [esi], 0
0047ad8a  je         0x47ae79

// block 0047ad90
0047ad90  mov        al, byte ptr [esi]
0047ad92  cmp        al, 0x20
0047ad94  je         0x47ad9a

// block 0047ad96
0047ad96  cmp        al, 9
0047ad98  jne        0x47ada0

// block 0047ad9a
0047ad9a  inc        esi
0047ad9b  jmp        0x47ad90

// block 0047ad9d
0047ad9d  dec        esi
0047ad9e  jmp        0x47ad83

// block 0047ada0
0047ada0  cmp        byte ptr [esi], 0
0047ada3  je         0x47ae79

// block 0047ada9
0047ada9  cmp        dword ptr [ebp + 8], 0
0047adad  je         0x47adb8

// block 0047adaf
0047adaf  mov        eax, dword ptr [ebp + 8]
0047adb2  add        dword ptr [ebp + 8], 4
0047adb6  mov        dword ptr [eax], edx

// block 0047adb8
0047adb8  inc        dword ptr [ecx]

// block 0047adba
0047adba  xor        ebx, ebx
0047adbc  inc        ebx
0047adbd  xor        ecx, ecx
0047adbf  jmp        0x47adc3

// block 0047adc1
0047adc1  inc        esi
0047adc2  inc        ecx

// block 0047adc3
0047adc3  cmp        byte ptr [esi], 0x5c
0047adc6  je         0x47adc1

// block 0047adc8
0047adc8  cmp        byte ptr [esi], 0x22
0047adcb  jne        0x47adf3

// block 0047adcd
0047adcd  test       cl, 1
0047add0  jne        0x47adf1

// block 0047add2
0047add2  cmp        dword ptr [ebp - 4], 0
0047add6  je         0x47ade4

// block 0047add8
0047add8  lea        eax, [esi + 1]
0047addb  cmp        byte ptr [eax], 0x22
0047adde  jne        0x47ade4

// block 0047ade0
0047ade0  mov        esi, eax
0047ade2  jmp        0x47adf1

// block 0047ade4
0047ade4  xor        eax, eax
0047ade6  xor        ebx, ebx
0047ade8  cmp        dword ptr [ebp - 4], eax
0047adeb  sete       al
0047adee  mov        dword ptr [ebp - 4], eax

// block 0047adf1
0047adf1  shr        ecx, 1

// block 0047adf3
0047adf3  test       ecx, ecx
0047adf5  je         0x47ae09

// block 0047adf7
0047adf7  dec        ecx
0047adf8  test       edx, edx
0047adfa  je         0x47ae00

// block 0047adfc
0047adfc  mov        byte ptr [edx], 0x5c
0047adff  inc        edx

// block 0047ae00
0047ae00  inc        dword ptr [edi]
0047ae02  test       ecx, ecx
0047ae04  jne        0x47adf7

// block 0047ae06
0047ae06  mov        dword ptr [ebp + 0xc], edx

// block 0047ae09
0047ae09  mov        al, byte ptr [esi]
0047ae0b  test       al, al
0047ae0d  je         0x47ae64

// block 0047ae0f
0047ae0f  cmp        dword ptr [ebp - 4], 0
0047ae13  jne        0x47ae1d

// block 0047ae15
0047ae15  cmp        al, 0x20
0047ae17  je         0x47ae64

// block 0047ae19
0047ae19  cmp        al, 9
0047ae1b  je         0x47ae64

// block 0047ae1d
0047ae1d  test       ebx, ebx
0047ae1f  je         0x47ae5e

// block 0047ae21
0047ae21  movsx      eax, al
0047ae24  push       eax
0047ae25  test       edx, edx
0047ae27  je         0x47ae4c

// block 0047ae29
0047ae29  call       0x48c7d7

// block 0047ae2e
0047ae2e  pop        ecx
0047ae2f  test       eax, eax
0047ae31  je         0x47ae40

// block 0047ae33
0047ae33  mov        al, byte ptr [esi]
0047ae35  mov        ecx, dword ptr [ebp + 0xc]
0047ae38  inc        dword ptr [ebp + 0xc]
0047ae3b  mov        byte ptr [ecx], al
0047ae3d  inc        esi
0047ae3e  inc        dword ptr [edi]

// block 0047ae40
0047ae40  mov        ecx, dword ptr [ebp + 0xc]
0047ae43  mov        al, byte ptr [esi]
0047ae45  inc        dword ptr [ebp + 0xc]
0047ae48  mov        byte ptr [ecx], al
0047ae4a  jmp        0x47ae59

// block 0047ae4c
0047ae4c  call       0x48c7d7

// block 0047ae51
0047ae51  pop        ecx
0047ae52  test       eax, eax
0047ae54  je         0x47ae59

// block 0047ae56
0047ae56  inc        esi
0047ae57  inc        dword ptr [edi]

// block 0047ae59
0047ae59  inc        dword ptr [edi]
0047ae5b  mov        edx, dword ptr [ebp + 0xc]

// block 0047ae5e
0047ae5e  inc        esi
0047ae5f  jmp        0x47adba

// block 0047ae64
0047ae64  test       edx, edx
0047ae66  je         0x47ae6f

// block 0047ae68
0047ae68  mov        byte ptr [edx], 0
0047ae6b  inc        edx
0047ae6c  mov        dword ptr [ebp + 0xc], edx

// block 0047ae6f
0047ae6f  inc        dword ptr [edi]
0047ae71  mov        ecx, dword ptr [ebp + 0x10]
0047ae74  jmp        0x47ad87

// block 0047ae79
0047ae79  mov        eax, dword ptr [ebp + 8]
0047ae7c  pop        esi
0047ae7d  pop        ebx
0047ae7e  test       eax, eax
0047ae80  je         0x47ae85

// block 0047ae82
0047ae82  and        dword ptr [eax], 0

// block 0047ae85
0047ae85  inc        dword ptr [ecx]
0047ae87  leave      
0047ae88  ret        

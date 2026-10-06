
// block 00465690
00465690  sub        esp, 0x40
00465693  mov        eax, dword ptr [0x4ad138]
00465698  xor        eax, esp
0046569a  mov        dword ptr [esp + 0x3c], eax
0046569e  mov        eax, dword ptr [esp + 0x44]
004656a2  mov        ecx, dword ptr [eax]
004656a4  mov        dword ptr [esp], 0
004656ab  test       ecx, ecx
004656ad  jne        0x4656c5

// block 004656af
004656af  mov        eax, 0x800401f0
004656b4  mov        ecx, dword ptr [esp + 0x3c]
004656b8  xor        ecx, esp
004656ba  call       0x46c932

// block 004656bf
004656bf  add        esp, 0x40
004656c2  ret        4

// block 004656c5
004656c5  xor        eax, eax
004656c7  push       0
004656c9  lea        edx, [esp + 4]
004656cd  push       edx
004656ce  mov        dword ptr [esp + 0x20], eax
004656d2  mov        dword ptr [esp + 0x24], eax
004656d6  mov        dword ptr [esp + 0x28], eax
004656da  mov        dword ptr [esp + 0x30], eax
004656de  mov        dword ptr [esp + 0x2c], eax
004656e2  mov        dword ptr [esp + 0x34], eax
004656e6  mov        dword ptr [esp + 0x38], eax
004656ea  mov        dword ptr [esp + 0x3c], eax
004656ee  mov        dword ptr [esp + 0x40], eax
004656f2  mov        dword ptr [esp + 0x28], eax
004656f6  mov        dword ptr [esp + 0x30], eax
004656fa  lea        edx, [esp + 0x20]
004656fe  mov        dword ptr [esp + 0x20], 0x24
00465706  mov        dword ptr [esp + 0x24], 1
0046570e  mov        eax, dword ptr [ecx]
00465710  mov        eax, dword ptr [eax + 0xc]
00465713  push       edx
00465714  push       ecx
00465715  call       eax

// block 00465717
00465717  test       eax, eax
00465719  jl         0x46578f

// block 0046571b
0046571b  xor        eax, eax
0046571d  mov        dword ptr [esp + 4], eax
00465721  mov        dword ptr [esp + 0x10], eax
00465725  mov        dword ptr [esp + 8], eax
00465729  mov        dword ptr [esp + 0xc], eax
0046572d  mov        word ptr [esp + 0x14], ax
00465732  mov        ecx, 1
00465737  mov        word ptr [esp + 4], cx
0046573c  mov        eax, 0x10
00465741  mov        ecx, 4
00465746  mov        word ptr [esp + 0x12], ax
0046574b  mov        eax, dword ptr [esp]
0046574e  mov        word ptr [esp + 0x10], cx
00465753  mov        edx, 2
00465758  mov        word ptr [esp + 6], dx
0046575d  lea        ecx, [esp + 4]
00465761  mov        dword ptr [esp + 8], 0xac44
00465769  mov        dword ptr [esp + 0xc], 0x2b110
00465771  mov        edx, dword ptr [eax]
00465773  mov        edx, dword ptr [edx + 0x38]
00465776  push       ecx
00465777  push       eax
00465778  call       edx

// block 0046577a
0046577a  test       eax, eax
0046577c  jl         0x46578f

// block 0046577e
0046577e  mov        eax, dword ptr [esp]
00465781  test       eax, eax
00465783  je         0x46578d

// block 00465785
00465785  mov        ecx, dword ptr [eax]
00465787  mov        edx, dword ptr [ecx + 8]
0046578a  push       eax
0046578b  call       edx

// block 0046578d
0046578d  xor        eax, eax

// block 0046578f
0046578f  mov        ecx, dword ptr [esp + 0x3c]
00465793  xor        ecx, esp
00465795  call       0x46c932

// block 0046579a
0046579a  add        esp, 0x40
0046579d  ret        4

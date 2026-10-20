
// block 00450600
00450600  push       ebx
00450601  push       ebp
00450602  mov        ebp, dword ptr [esp + 0xc]
00450606  push       esi
00450607  mov        esi, dword ptr [0x4ce8cc]
0045060d  push       edi
0045060e  call       0x45a3c0

// block 00450613
00450613  mov        edi, 0x4cec04
00450618  mov        dword ptr [0x4cee34], edi
0045061e  call       0x430910

// block 00450623
00450623  mov        edx, dword ptr [0x4cee34]
00450629  mov        eax, dword ptr [0x4ce8f0]
0045062e  mov        ecx, dword ptr [eax]
00450630  add        edx, 0xcc
00450636  push       edx
00450637  push       eax
00450638  mov        eax, dword ptr [ecx + 0xbc]
0045063e  call       eax

// block 00450640
00450640  mov        ebx, dword ptr [0x4ce89c]
00450646  mov        dword ptr [0x4cee38], 1
00450650  call       0x4624c0

// block 00450655
00450655  test       eax, eax
00450657  jne        0x45066f

// block 00450659
00450659  mov        esi, 0x4cf0d8
0045065e  call       0x464c40

// block 00450663
00450663  mov        eax, 1
00450668  pop        edi
00450669  pop        esi
0045066a  pop        ebp
0045066b  pop        ebx
0045066c  ret        4

// block 0045066f
0045066f  cmp        eax, -1
00450672  jne        0x45068a

// block 00450674
00450674  mov        esi, 0x4cf0d8
00450679  call       0x464c40

// block 0045067e
0045067e  mov        eax, 2
00450683  pop        edi
00450684  pop        esi
00450685  pop        ebp
00450686  pop        ebx
00450687  ret        4

// block 0045068a
0045068a  inc        byte ptr [ebp + 0x14]
0045068d  mov        al, byte ptr [ebp + 0x14]
00450690  movzx      ecx, byte ptr [0x4ceace]
00450697  movsx      edx, al
0045069a  inc        ecx
0045069b  cmp        ecx, edx
0045069d  jg         0x450707

// block 0045069f
0045069f  mov        eax, dword ptr [0x4ce8f0]
004506a4  mov        ecx, dword ptr [eax]
004506a6  mov        edx, dword ptr [ecx + 0xa4]
004506ac  push       eax
004506ad  call       edx

// block 004506af
004506af  call       0x45a380

// block 004506b4
004506b4  mov        edi, 0x4ce8e8
004506b9  mov        dword ptr [0x4cf278], 0xff
004506c3  call       0x430300

// block 004506c8
004506c8  call       0x462620

// block 004506cd
004506cd  mov        esi, dword ptr [0x4ce8cc]
004506d3  call       0x45a3c0

// block 004506d8
004506d8  mov        eax, dword ptr [0x4ce8f0]
004506dd  mov        ecx, dword ptr [eax]
004506df  mov        edx, dword ptr [ecx + 0x104]
004506e5  push       0
004506e7  push       0
004506e9  push       eax
004506ea  call       edx

// block 004506ec
004506ec  mov        eax, dword ptr [0x4ce8f0]
004506f1  mov        ecx, dword ptr [eax]
004506f3  mov        edx, dword ptr [ecx + 0xa8]
004506f9  push       eax
004506fa  call       edx

// block 004506fc
004506fc  mov        esi, ebp
004506fe  mov        byte ptr [ebp + 0x14], 0
00450702  call       0x450720

// block 00450707
00450707  call       0x4508b0

// block 0045070c
0045070c  fsub       qword ptr [ebp + 0x40]
0045070f  pop        edi
00450710  pop        esi
00450711  pop        ebp
00450712  fstp       qword ptr [0x4cf2a0]
00450718  xor        eax, eax
0045071a  pop        ebx
0045071b  ret        4

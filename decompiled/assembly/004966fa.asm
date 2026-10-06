
// block 004966fa
004966fa  mov        edi, edi
004966fc  push       ebp
004966fd  mov        ebp, esp
004966ff  sub        esp, 0x20
00496702  xor        eax, eax

// block 00496704
00496704  mov        ecx, dword ptr [eax*8 + 0x4b37a8]
0049670b  cmp        ecx, dword ptr [ebp + 0xc]
0049670e  je         0x496774

// block 00496710
00496710  inc        eax
00496711  cmp        eax, 0x1d
00496714  jl         0x496704

// block 00496716
00496716  xor        eax, eax

// block 00496718
00496718  mov        dword ptr [ebp - 0x1c], eax
0049671b  test       eax, eax
0049671d  je         0x49677d

// block 0049671f
0049671f  mov        eax, dword ptr [ebp + 0x10]
00496722  mov        dword ptr [ebp - 0x18], eax
00496725  mov        eax, dword ptr [ebp + 0x14]
00496728  mov        dword ptr [ebp - 0x14], eax
0049672b  mov        eax, dword ptr [ebp + 0x18]
0049672e  mov        dword ptr [ebp - 0x10], eax
00496731  mov        eax, dword ptr [ebp + 0x1c]
00496734  push       esi
00496735  mov        esi, dword ptr [ebp + 8]
00496738  mov        dword ptr [ebp - 0xc], eax
0049673b  mov        eax, dword ptr [ebp + 0x20]
0049673e  mov        dword ptr [ebp - 8], eax
00496741  mov        eax, dword ptr [ebp + 0x24]
00496744  push       0xffff
00496749  push       dword ptr [ebp + 0x28]
0049674c  mov        dword ptr [ebp - 0x20], esi
0049674f  mov        dword ptr [ebp - 4], eax
00496752  call       0x491181

// block 00496757
00496757  lea        eax, [ebp - 0x20]
0049675a  push       eax
0049675b  call       0x49616c

// block 00496760
00496760  add        esp, 0xc
00496763  test       eax, eax
00496765  jne        0x49676e

// block 00496767
00496767  push       esi
00496768  call       0x496673

// block 0049676d
0049676d  pop        ecx

// block 0049676e
0049676e  fld        qword ptr [ebp - 8]
00496771  pop        esi
00496772  leave      
00496773  ret        

// block 00496774
00496774  mov        eax, dword ptr [eax*8 + 0x4b37ac]
0049677b  jmp        0x496718

// block 0049677d
0049677d  push       0xffff
00496782  push       dword ptr [ebp + 0x28]
00496785  call       0x491181

// block 0049678a
0049678a  push       dword ptr [ebp + 8]
0049678d  call       0x496673

// block 00496792
00496792  fld        qword ptr [ebp + 0x20]
00496795  add        esp, 0xc
00496798  leave      
00496799  ret        

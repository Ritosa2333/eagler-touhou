
// block 0046ac78
0046ac78  or         al, 0
0046ac7a  add        byte ptr [eax], al
0046ac80  cld        
0046ac81  xchg       esp, eax
0046ac82  dec        ecx
0046ac83  add        byte ptr [ecx], al
0046ac85  add        byte ptr [eax], al
0046ac87  add        byte ptr [ecx + eax], cl
0046ac8a  add        byte ptr [eax], al
0046ac90  cld        
0046ac91  xchg       esp, eax
0046ac92  dec        ecx
0046ac93  add        byte ptr [edx], al
0046ac95  add        byte ptr [eax], al
0046ac97  add        byte ptr [edx + eax], cl
0046ac9a  add        byte ptr [eax], al
0046aca0  cld        
0046aca1  xchg       esp, eax
0046aca2  dec        ecx
0046aca3  add        byte ptr [ebx], al
0046aca5  add        byte ptr [eax], al
0046aca7  add        byte ptr [ebx + eax], cl
0046acaa  add        byte ptr [eax], al
0046acb0  cld        
0046acb1  xchg       esp, eax
0046acb2  dec        ecx
0046acb3  add        byte ptr [eax + eax], al
0046acb6  add        byte ptr [eax], al
0046acb8  or         al, 4
0046acba  add        byte ptr [eax], al
0046acc0  cld        
0046acc1  xchg       esp, eax
0046acc2  dec        ecx
0046acc3  add        byte ptr [0xc000000], al
0046acc9  add        eax, 0x8000
0046acce  add        byte ptr [eax], al
0046acd0  cld        
0046acd1  xchg       esp, eax
0046acd2  dec        ecx
0046acd3  add        byte ptr [esi], al
0046acd5  add        byte ptr [eax], al
0046acd7  add        byte ptr [esi + eax], cl
0046acda  add        byte ptr [eax], al
0046ace0  cld        
0046ace1  xchg       esp, eax
0046ace2  dec        ecx
0046ace3  add        byte ptr [edi], al
0046ace5  add        byte ptr [eax], al
0046ace7  add        byte ptr [edi + eax], cl
0046acea  add        byte ptr [eax], al
0046acf0  cld        
0046acf1  xchg       esp, eax
0046acf2  dec        ecx
0046acf3  add        byte ptr [eax], cl
0046acf5  add        byte ptr [eax], al
0046acf7  add        byte ptr [eax + ecx], cl
0046acfa  add        byte ptr [eax], al
0046ad00  cld        
0046ad01  xchg       esp, eax
0046ad02  dec        ecx
0046ad03  add        byte ptr [ecx], cl
0046ad05  add        byte ptr [eax], al
0046ad07  add        byte ptr [ecx + ecx], cl
0046ad0a  add        byte ptr [eax], al
0046ad10  cld        
0046ad11  xchg       esp, eax
0046ad12  dec        ecx
0046ad13  add        byte ptr [edx], cl
0046ad15  add        byte ptr [eax], al
0046ad17  add        byte ptr [edx + ecx], cl
0046ad1a  add        byte ptr [eax], al
0046ad20  cld        
0046ad21  xchg       esp, eax
0046ad22  dec        ecx
0046ad23  add        byte ptr [ebx], cl
0046ad25  add        byte ptr [eax], al
0046ad27  add        byte ptr [ebx + ecx], cl
0046ad2a  add        byte ptr [eax], al
0046ad30  cld        
0046ad31  xchg       esp, eax
0046ad32  dec        ecx
0046ad33  add        byte ptr [eax + eax], cl
0046ad36  add        byte ptr [eax], al
0046ad38  or         al, 0xc
0046ad3a  add        byte ptr [eax], al
0046ad40  cld        
0046ad41  xchg       esp, eax
0046ad42  dec        ecx
0046ad43  add        byte ptr [0xc000000], cl
0046ad49  or         eax, 0x8000
0046ad4e  add        byte ptr [eax], al
0046ad50  cld        
0046ad51  xchg       esp, eax
0046ad52  dec        ecx
0046ad53  add        byte ptr [esi], cl
0046ad55  add        byte ptr [eax], al
0046ad57  add        byte ptr [esi + ecx], cl
0046ad5a  add        byte ptr [eax], al
0046ad60  cld        

// block 0046ad61
0046ad61  xchg       esp, eax
0046ad62  dec        ecx
0046ad63  add        byte ptr [edi], cl
0046ad65  add        byte ptr [eax], al
0046ad67  add        byte ptr [edi + ecx], cl
0046ad6a  add        byte ptr [eax], al
0046ad70  cld        
0046ad71  xchg       esp, eax
0046ad72  dec        ecx
0046ad73  add        byte ptr [eax], dl
0046ad75  add        byte ptr [eax], al
0046ad77  add        byte ptr [eax + edx], cl
0046ad7a  add        byte ptr [eax], al
0046ad80  cld        
0046ad81  xchg       esp, eax
0046ad82  dec        ecx
0046ad83  add        byte ptr [ecx], dl
0046ad85  add        byte ptr [eax], al
0046ad87  add        byte ptr [ecx + edx], cl
0046ad8a  add        byte ptr [eax], al
0046ad90  cld        
0046ad91  xchg       esp, eax
0046ad92  dec        ecx
0046ad93  add        byte ptr [edx], dl
0046ad95  add        byte ptr [eax], al
0046ad97  add        byte ptr [edx + edx], cl
0046ad9a  add        byte ptr [eax], al
0046ada0  cld        
0046ada1  xchg       esp, eax
0046ada2  dec        ecx
0046ada3  add        byte ptr [ebx], dl
0046ada5  add        byte ptr [eax], al
0046ada7  add        byte ptr [ebx + edx], cl
0046adaa  add        byte ptr [eax], al
0046adb0  cld        
0046adb1  xchg       esp, eax
0046adb2  dec        ecx
0046adb3  add        byte ptr [eax + eax], dl
0046adb6  add        byte ptr [eax], al
0046adb8  or         al, 0x14
0046adba  add        byte ptr [eax], al
0046adc0  cld        
0046adc1  xchg       esp, eax
0046adc2  dec        ecx
0046adc3  add        byte ptr [0xc000000], dl
0046adc9  adc        eax, 0x8000
0046adce  add        byte ptr [eax], al
0046add0  cld        
0046add1  xchg       esp, eax
0046add2  dec        ecx
0046add3  add        byte ptr [esi], dl
0046add5  add        byte ptr [eax], al
0046add7  add        byte ptr [esi + edx], cl
0046adda  add        byte ptr [eax], al
0046ade0  cld        
0046ade1  xchg       esp, eax
0046ade2  dec        ecx
0046ade3  add        byte ptr [edi], dl
0046ade5  add        byte ptr [eax], al
0046ade7  add        byte ptr [edi + edx], cl
0046adea  add        byte ptr [eax], al
0046adf0  cld        
0046adf1  xchg       esp, eax
0046adf2  dec        ecx
0046adf3  add        byte ptr [eax], bl
0046adf5  add        byte ptr [eax], al
0046adf7  add        byte ptr [eax + ebx], cl
0046adfa  add        byte ptr [eax], al
0046ae00  cld        
0046ae01  xchg       esp, eax
0046ae02  dec        ecx
0046ae03  add        byte ptr [ecx], bl
0046ae05  add        byte ptr [eax], al
0046ae07  add        byte ptr [ecx + ebx], cl
0046ae0a  add        byte ptr [eax], al
0046ae10  cld        
0046ae11  xchg       esp, eax
0046ae12  dec        ecx
0046ae13  add        byte ptr [edx], bl
0046ae15  add        byte ptr [eax], al
0046ae17  add        byte ptr [edx + ebx], cl
0046ae1a  add        byte ptr [eax], al
0046ae20  cld        
0046ae21  xchg       esp, eax
0046ae22  dec        ecx
0046ae23  add        byte ptr [ebx], bl
0046ae25  add        byte ptr [eax], al
0046ae27  add        byte ptr [ebx + ebx], cl
0046ae2a  add        byte ptr [eax], al
0046ae30  cld        
0046ae31  xchg       esp, eax
0046ae32  dec        ecx
0046ae33  add        byte ptr [eax + eax], bl
0046ae36  add        byte ptr [eax], al
0046ae38  or         al, 0x1c
0046ae3a  add        byte ptr [eax], al
0046ae40  cld        
0046ae41  xchg       esp, eax
0046ae42  dec        ecx

// block 0046ae43
0046ae43  add        byte ptr [0xc000000], bl
0046ae49  sbb        eax, 0x8000
0046ae4e  add        byte ptr [eax], al
0046ae50  cld        
0046ae51  xchg       esp, eax
0046ae52  dec        ecx
0046ae53  add        byte ptr [esi], bl
0046ae55  add        byte ptr [eax], al
0046ae57  add        byte ptr [esi + ebx], cl
0046ae5a  add        byte ptr [eax], al
0046ae60  cld        
0046ae61  xchg       esp, eax
0046ae62  dec        ecx
0046ae63  add        byte ptr [edi], bl
0046ae65  add        byte ptr [eax], al
0046ae67  add        byte ptr [edi + ebx], cl
0046ae6a  add        byte ptr [eax], al
0046ae70  cld        
0046ae71  xchg       esp, eax
0046ae72  dec        ecx
0046ae73  add        byte ptr [eax], ah
0046ae75  add        byte ptr [eax], al
0046ae77  add        byte ptr [eax], cl
0046ae7a  add        byte ptr [eax], al
0046ae80  cld        
0046ae81  xchg       esp, eax
0046ae82  dec        ecx
0046ae83  add        byte ptr [ecx], ah
0046ae85  add        byte ptr [eax], al
0046ae87  add        byte ptr [ecx], cl
0046ae8a  add        byte ptr [eax], al
0046ae90  cld        
0046ae91  xchg       esp, eax
0046ae92  dec        ecx
0046ae93  add        byte ptr [edx], ah
0046ae95  add        byte ptr [eax], al
0046ae97  add        byte ptr [edx], cl
0046ae9a  add        byte ptr [eax], al
0046aea0  cld        
0046aea1  xchg       esp, eax
0046aea2  dec        ecx
0046aea3  add        byte ptr [ebx], ah
0046aea5  add        byte ptr [eax], al
0046aea7  add        byte ptr [ebx], cl
0046aeaa  add        byte ptr [eax], al
0046aeb0  cld        
0046aeb1  xchg       esp, eax
0046aeb2  dec        ecx
0046aeb3  add        byte ptr [eax + eax], ah
0046aeb6  add        byte ptr [eax], al
0046aeb8  or         al, 0x24
0046aeba  add        byte ptr [eax], al
0046aec0  cld        
0046aec1  xchg       esp, eax
0046aec2  dec        ecx
0046aec3  add        byte ptr [0xc000000], ah
0046aec9  and        eax, 0x8000
0046aece  add        byte ptr [eax], al
0046aed0  cld        
0046aed1  xchg       esp, eax
0046aed2  dec        ecx
0046aed3  add        byte ptr [esi], ah
0046aed5  add        byte ptr [eax], al
0046aed7  add        byte ptr [esi], cl
0046aeda  add        byte ptr [eax], al
0046aee0  cld        
0046aee1  xchg       esp, eax
0046aee2  dec        ecx
0046aee3  add        byte ptr [edi], ah
0046aee5  add        byte ptr [eax], al
0046aee7  add        byte ptr [edi], cl
0046aeea  add        byte ptr [eax], al
0046aef0  cld        
0046aef1  xchg       esp, eax
0046aef2  dec        ecx
0046aef3  add        byte ptr [eax], ch
0046aef5  add        byte ptr [eax], al
0046aef7  add        byte ptr [eax + ebp], cl
0046aefa  add        byte ptr [eax], al
0046af00  cld        
0046af01  xchg       esp, eax
0046af02  dec        ecx
0046af03  add        byte ptr [ecx], ch
0046af05  add        byte ptr [eax], al
0046af07  add        byte ptr [ecx + ebp], cl
0046af0a  add        byte ptr [eax], al
0046af10  cld        
0046af11  xchg       esp, eax
0046af12  dec        ecx
0046af13  add        byte ptr [edx], ch
0046af15  add        byte ptr [eax], al
0046af17  add        byte ptr [edx + ebp], cl
0046af1a  add        byte ptr [eax], al
0046af20  cld        
0046af21  xchg       esp, eax
0046af22  dec        ecx
0046af23  add        byte ptr [ebx], ch
0046af25  add        byte ptr [eax], al
0046af27  add        byte ptr [ebx + ebp], cl

// block 0046af2a
0046af2a  add        byte ptr [eax], al
0046af30  cld        
0046af31  xchg       esp, eax
0046af32  dec        ecx
0046af33  add        byte ptr [eax + eax], ch
0046af36  add        byte ptr [eax], al
0046af38  or         al, 0x2c
0046af3a  add        byte ptr [eax], al
0046af40  cld        
0046af41  xchg       esp, eax
0046af42  dec        ecx
0046af43  add        byte ptr [0xc000000], ch
0046af49  sub        eax, 0x8000
0046af4e  add        byte ptr [eax], al
0046af50  cld        
0046af51  xchg       esp, eax
0046af52  dec        ecx
0046af53  add        byte ptr [esi], ch
0046af55  add        byte ptr [eax], al
0046af57  add        byte ptr [esi + ebp], cl
0046af5a  add        byte ptr [eax], al
0046af60  cld        
0046af61  xchg       esp, eax
0046af62  dec        ecx
0046af63  add        byte ptr [edi], ch
0046af65  add        byte ptr [eax], al
0046af67  add        byte ptr [edi + ebp], cl
0046af6a  add        byte ptr [eax], al
0046af70  cld        
0046af71  xchg       esp, eax
0046af72  dec        ecx
0046af73  add        byte ptr [eax], dh
0046af75  add        byte ptr [eax], al
0046af77  add        byte ptr [eax + esi], cl
0046af7a  add        byte ptr [eax], al
0046af80  cld        
0046af81  xchg       esp, eax
0046af82  dec        ecx
0046af83  add        byte ptr [ecx], dh
0046af85  add        byte ptr [eax], al
0046af87  add        byte ptr [ecx + esi], cl
0046af8a  add        byte ptr [eax], al
0046af90  cld        
0046af91  xchg       esp, eax
0046af92  dec        ecx
0046af93  add        byte ptr [edx], dh
0046af95  add        byte ptr [eax], al
0046af97  add        byte ptr [edx + esi], cl
0046af9a  add        byte ptr [eax], al
0046afa0  cld        
0046afa1  xchg       esp, eax
0046afa2  dec        ecx
0046afa3  add        byte ptr [ebx], dh
0046afa5  add        byte ptr [eax], al
0046afa7  add        byte ptr [ebx + esi], cl
0046afaa  add        byte ptr [eax], al
0046afb0  cld        
0046afb1  xchg       esp, eax
0046afb2  dec        ecx
0046afb3  add        byte ptr [eax + eax], dh
0046afb6  add        byte ptr [eax], al
0046afb8  or         al, 0x34
0046afba  add        byte ptr [eax], al
0046afc0  cld        
0046afc1  xchg       esp, eax
0046afc2  dec        ecx
0046afc3  add        byte ptr [0xc000000], dh
0046afc9  xor        eax, 0x8000
0046afce  add        byte ptr [eax], al
0046afd0  cld        
0046afd1  xchg       esp, eax
0046afd2  dec        ecx
0046afd3  add        byte ptr [esi], dh
0046afd5  add        byte ptr [eax], al
0046afd7  add        byte ptr [esi + esi], cl
0046afda  add        byte ptr [eax], al
0046afe0  cld        
0046afe1  xchg       esp, eax
0046afe2  dec        ecx
0046afe3  add        byte ptr [edi], dh
0046afe5  add        byte ptr [eax], al
0046afe7  add        byte ptr [edi + esi], cl
0046afea  add        byte ptr [eax], al
0046aff0  cld        
0046aff1  xchg       esp, eax
0046aff2  dec        ecx
0046aff3  add        byte ptr [eax], bh
0046aff5  add        byte ptr [eax], al
0046aff7  add        byte ptr [eax + edi], cl
0046affa  add        byte ptr [eax], al
0046b000  cld        
0046b001  xchg       esp, eax
0046b002  dec        ecx
0046b003  add        byte ptr [ecx], bh
0046b005  add        byte ptr [eax], al
0046b007  add        byte ptr [ecx + edi], cl
0046b00a  add        byte ptr [eax], al
0046b010  cld        
0046b011  xchg       esp, eax

// block 0046b012
0046b012  dec        ecx
0046b013  add        byte ptr [edx], bh
0046b015  add        byte ptr [eax], al
0046b017  add        byte ptr [edx + edi], cl
0046b01a  add        byte ptr [eax], al
0046b020  cld        
0046b021  xchg       esp, eax
0046b022  dec        ecx
0046b023  add        byte ptr [ebx], bh
0046b025  add        byte ptr [eax], al
0046b027  add        byte ptr [ebx + edi], cl
0046b02a  add        byte ptr [eax], al
0046b030  cld        
0046b031  xchg       esp, eax
0046b032  dec        ecx
0046b033  add        byte ptr [eax + eax], bh
0046b036  add        byte ptr [eax], al
0046b038  or         al, 0x3c
0046b03a  add        byte ptr [eax], al
0046b040  cld        
0046b041  xchg       esp, eax
0046b042  dec        ecx
0046b043  add        byte ptr [0xc000000], bh
0046b049  cmp        eax, 0x8000
0046b04e  add        byte ptr [eax], al
0046b050  cld        
0046b051  xchg       esp, eax
0046b052  dec        ecx
0046b053  add        byte ptr [esi], bh
0046b055  add        byte ptr [eax], al
0046b057  add        byte ptr [esi + edi], cl
0046b05a  add        byte ptr [eax], al
0046b060  cld        
0046b061  xchg       esp, eax
0046b062  dec        ecx
0046b063  add        byte ptr [edi], bh
0046b065  add        byte ptr [eax], al
0046b067  add        byte ptr [edi + edi], cl
0046b06a  add        byte ptr [eax], al
0046b070  cld        
0046b071  xchg       esp, eax
0046b072  dec        ecx
0046b073  add        byte ptr [eax], al
0046b076  add        byte ptr [eax], al
0046b078  or         al, 0x40
0046b07a  add        byte ptr [eax], al
0046b080  cld        
0046b081  xchg       esp, eax
0046b082  dec        ecx
0046b083  add        byte ptr [ecx], al
0046b086  add        byte ptr [eax], al
0046b088  or         al, 0x41
0046b08a  add        byte ptr [eax], al
0046b090  cld        
0046b091  xchg       esp, eax
0046b092  dec        ecx
0046b093  add        byte ptr [edx], al
0046b096  add        byte ptr [eax], al
0046b098  or         al, 0x42
0046b09a  add        byte ptr [eax], al
0046b0a0  cld        
0046b0a1  xchg       esp, eax
0046b0a2  dec        ecx
0046b0a3  add        byte ptr [ebx], al
0046b0a6  add        byte ptr [eax], al
0046b0a8  or         al, 0x43
0046b0aa  add        byte ptr [eax], al
0046b0b0  cld        
0046b0b1  xchg       esp, eax
0046b0b2  dec        ecx
0046b0b3  add        byte ptr [eax + eax], al
0046b0b7  add        byte ptr [esp + eax*2], cl
0046b0ba  add        byte ptr [eax], al
0046b0c0  cld        
0046b0c1  xchg       esp, eax
0046b0c2  dec        ecx
0046b0c3  add        byte ptr [ebp], al
0046b0c6  add        byte ptr [eax], al
0046b0c8  or         al, 0x45
0046b0ca  add        byte ptr [eax], al
0046b0d0  cld        
0046b0d1  xchg       esp, eax
0046b0d2  dec        ecx
0046b0d3  add        byte ptr [esi], al
0046b0d6  add        byte ptr [eax], al
0046b0d8  or         al, 0x46
0046b0da  add        byte ptr [eax], al
0046b0e0  cld        
0046b0e1  xchg       esp, eax
0046b0e2  dec        ecx
0046b0e3  add        byte ptr [edi], al
0046b0e6  add        byte ptr [eax], al
0046b0e8  or         al, 0x47
0046b0ea  add        byte ptr [eax], al
0046b0f0  cld        
0046b0f1  xchg       esp, eax
0046b0f2  dec        ecx
0046b0f3  add        byte ptr [eax], cl
0046b0f6  add        byte ptr [eax], al

// block 0046b0f8
0046b0f8  or         al, 0x48
0046b0fa  add        byte ptr [eax], al
0046b100  cld        
0046b101  xchg       esp, eax
0046b102  dec        ecx
0046b103  add        byte ptr [ecx], cl
0046b106  add        byte ptr [eax], al
0046b108  or         al, 0x49
0046b10a  add        byte ptr [eax], al
0046b110  cld        
0046b111  xchg       esp, eax
0046b112  dec        ecx
0046b113  add        byte ptr [edx], cl
0046b116  add        byte ptr [eax], al
0046b118  or         al, 0x4a
0046b11a  add        byte ptr [eax], al
0046b120  cld        
0046b121  xchg       esp, eax
0046b122  dec        ecx
0046b123  add        byte ptr [ebx], cl
0046b126  add        byte ptr [eax], al
0046b128  or         al, 0x4b
0046b12a  add        byte ptr [eax], al
0046b130  cld        
0046b131  xchg       esp, eax
0046b132  dec        ecx
0046b133  add        byte ptr [eax + eax], cl
0046b137  add        byte ptr [esp + ecx*2], cl
0046b13a  add        byte ptr [eax], al
0046b140  cld        
0046b141  xchg       esp, eax
0046b142  dec        ecx
0046b143  add        byte ptr [ebp], cl
0046b146  add        byte ptr [eax], al
0046b148  or         al, 0x4d
0046b14a  add        byte ptr [eax], al
0046b150  cld        
0046b151  xchg       esp, eax
0046b152  dec        ecx
0046b153  add        byte ptr [esi], cl
0046b156  add        byte ptr [eax], al
0046b158  or         al, 0x4e
0046b15a  add        byte ptr [eax], al
0046b160  cld        
0046b161  xchg       esp, eax
0046b162  dec        ecx
0046b163  add        byte ptr [edi], cl
0046b166  add        byte ptr [eax], al
0046b168  or         al, 0x4f
0046b16a  add        byte ptr [eax], al
0046b170  cld        
0046b171  xchg       esp, eax
0046b172  dec        ecx
0046b173  add        byte ptr [eax], dl
0046b176  add        byte ptr [eax], al
0046b178  or         al, 0x50
0046b17a  add        byte ptr [eax], al
0046b180  cld        
0046b181  xchg       esp, eax
0046b182  dec        ecx
0046b183  add        byte ptr [ecx], dl
0046b186  add        byte ptr [eax], al
0046b188  or         al, 0x51
0046b18a  add        byte ptr [eax], al
0046b190  cld        
0046b191  xchg       esp, eax
0046b192  dec        ecx
0046b193  add        byte ptr [edx], dl
0046b196  add        byte ptr [eax], al
0046b198  or         al, 0x52
0046b19a  add        byte ptr [eax], al
0046b1a0  cld        
0046b1a1  xchg       esp, eax
0046b1a2  dec        ecx
0046b1a3  add        byte ptr [ebx], dl
0046b1a6  add        byte ptr [eax], al
0046b1a8  or         al, 0x53
0046b1aa  add        byte ptr [eax], al
0046b1b0  cld        
0046b1b1  xchg       esp, eax
0046b1b2  dec        ecx
0046b1b3  add        byte ptr [eax + eax], dl
0046b1b7  add        byte ptr [esp + edx*2], cl
0046b1ba  add        byte ptr [eax], al
0046b1c0  cld        
0046b1c1  xchg       esp, eax
0046b1c2  dec        ecx
0046b1c3  add        byte ptr [ebp], dl
0046b1c6  add        byte ptr [eax], al
0046b1c8  or         al, 0x55
0046b1ca  add        byte ptr [eax], al
0046b1d0  cld        
0046b1d1  xchg       esp, eax
0046b1d2  dec        ecx
0046b1d3  add        byte ptr [esi], dl
0046b1d6  add        byte ptr [eax], al
0046b1d8  or         al, 0x56
0046b1da  add        byte ptr [eax], al
0046b1e0  cld        

// block 0046b1e1
0046b1e1  xchg       esp, eax
0046b1e2  dec        ecx
0046b1e3  add        byte ptr [edi], dl
0046b1e6  add        byte ptr [eax], al
0046b1e8  or         al, 0x57
0046b1ea  add        byte ptr [eax], al
0046b1f0  cld        
0046b1f1  xchg       esp, eax
0046b1f2  dec        ecx
0046b1f3  add        byte ptr [eax], bl
0046b1f6  add        byte ptr [eax], al
0046b1f8  or         al, 0x58
0046b1fa  add        byte ptr [eax], al
0046b200  cld        
0046b201  xchg       esp, eax
0046b202  dec        ecx
0046b203  add        byte ptr [ecx], bl
0046b206  add        byte ptr [eax], al
0046b208  or         al, 0x59
0046b20a  add        byte ptr [eax], al
0046b210  cld        
0046b211  xchg       esp, eax
0046b212  dec        ecx
0046b213  add        byte ptr [edx], bl
0046b216  add        byte ptr [eax], al
0046b218  or         al, 0x5a
0046b21a  add        byte ptr [eax], al
0046b220  cld        
0046b221  xchg       esp, eax
0046b222  dec        ecx
0046b223  add        byte ptr [ebx], bl
0046b226  add        byte ptr [eax], al
0046b228  or         al, 0x5b
0046b22a  add        byte ptr [eax], al
0046b230  cld        
0046b231  xchg       esp, eax
0046b232  dec        ecx
0046b233  add        byte ptr [eax + eax], bl
0046b237  add        byte ptr [esp + ebx*2], cl
0046b23a  add        byte ptr [eax], al
0046b240  cld        
0046b241  xchg       esp, eax
0046b242  dec        ecx
0046b243  add        byte ptr [ebp], bl
0046b246  add        byte ptr [eax], al
0046b248  or         al, 0x5d
0046b24a  add        byte ptr [eax], al
0046b250  cld        
0046b251  xchg       esp, eax
0046b252  dec        ecx
0046b253  add        byte ptr [esi], bl
0046b256  add        byte ptr [eax], al
0046b258  or         al, 0x5e
0046b25a  add        byte ptr [eax], al
0046b260  cld        
0046b261  xchg       esp, eax
0046b262  dec        ecx
0046b263  add        byte ptr [edi], bl
0046b266  add        byte ptr [eax], al
0046b268  or         al, 0x5f
0046b26a  add        byte ptr [eax], al
0046b270  cld        
0046b271  xchg       esp, eax
0046b272  dec        ecx
0046b273  add        byte ptr [eax], ah
0046b276  add        byte ptr [eax], al
0046b278  or         al, 0x60
0046b27a  add        byte ptr [eax], al
0046b280  cld        
0046b281  xchg       esp, eax
0046b282  dec        ecx
0046b283  add        byte ptr [ecx], ah
0046b286  add        byte ptr [eax], al
0046b288  or         al, 0x61
0046b28a  add        byte ptr [eax], al
0046b290  cld        
0046b291  xchg       esp, eax
0046b292  dec        ecx
0046b293  add        byte ptr [edx], ah
0046b296  add        byte ptr [eax], al
0046b298  or         al, 0x62
0046b29a  add        byte ptr [eax], al
0046b2a0  cld        
0046b2a1  xchg       esp, eax
0046b2a2  dec        ecx
0046b2a3  add        byte ptr [ebx], ah
0046b2a6  add        byte ptr [eax], al
0046b2a8  or         al, 0x63
0046b2aa  add        byte ptr [eax], al
0046b2b0  cld        
0046b2b1  xchg       esp, eax
0046b2b2  dec        ecx
0046b2b3  add        byte ptr [eax + eax], ah
0046b2b7  add        byte ptr [esp], cl
0046b2ba  add        byte ptr [eax], al
0046b2c0  cld        
0046b2c1  xchg       esp, eax
0046b2c2  dec        ecx
0046b2c3  add        byte ptr [ebp], ah

// block 0046b2c6
0046b2c6  add        byte ptr [eax], al
0046b2c8  or         al, 0x65
0046b2ca  add        byte ptr [eax], al
0046b2d0  cld        
0046b2d1  xchg       esp, eax
0046b2d2  dec        ecx
0046b2d3  add        byte ptr [esi], ah
0046b2d6  add        byte ptr [eax], al
0046b2d8  or         al, 0x66
0046b2da  add        byte ptr [eax], al
0046b2e0  cld        
0046b2e1  xchg       esp, eax
0046b2e2  dec        ecx
0046b2e3  add        byte ptr [edi], ah
0046b2e6  add        byte ptr [eax], al
0046b2e8  or         al, 0x67
0046b2ea  add        byte ptr [eax], al
0046b2f0  cld        
0046b2f1  xchg       esp, eax
0046b2f2  dec        ecx
0046b2f3  add        byte ptr [eax], ch
0046b2f6  add        byte ptr [eax], al
0046b2f8  or         al, 0x68
0046b2fa  add        byte ptr [eax], al
0046b300  cld        
0046b301  xchg       esp, eax
0046b302  dec        ecx
0046b303  add        byte ptr [ecx], ch
0046b306  add        byte ptr [eax], al
0046b308  or         al, 0x69
0046b30a  add        byte ptr [eax], al
0046b310  cld        
0046b311  xchg       esp, eax
0046b312  dec        ecx
0046b313  add        byte ptr [edx], ch
0046b316  add        byte ptr [eax], al
0046b318  or         al, 0x6a
0046b31a  add        byte ptr [eax], al
0046b320  cld        
0046b321  xchg       esp, eax
0046b322  dec        ecx
0046b323  add        byte ptr [ebx], ch
0046b326  add        byte ptr [eax], al
0046b328  or         al, 0x6b
0046b32a  add        byte ptr [eax], al
0046b330  cld        
0046b331  xchg       esp, eax
0046b332  dec        ecx
0046b333  add        byte ptr [eax + eax], ch
0046b337  add        byte ptr [esp + ebp*2], cl
0046b33a  add        byte ptr [eax], al
0046b340  cld        
0046b341  xchg       esp, eax
0046b342  dec        ecx
0046b343  add        byte ptr [ebp], ch
0046b346  add        byte ptr [eax], al
0046b348  or         al, 0x6d
0046b34a  add        byte ptr [eax], al
0046b350  cld        
0046b351  xchg       esp, eax
0046b352  dec        ecx
0046b353  add        byte ptr [esi], ch
0046b356  add        byte ptr [eax], al
0046b358  or         al, 0x6e
0046b35a  add        byte ptr [eax], al
0046b360  cld        
0046b361  xchg       esp, eax
0046b362  dec        ecx
0046b363  add        byte ptr [edi], ch
0046b366  add        byte ptr [eax], al
0046b368  or         al, 0x6f
0046b36a  add        byte ptr [eax], al
0046b370  cld        
0046b371  xchg       esp, eax
0046b372  dec        ecx
0046b373  add        byte ptr [eax], dh
0046b376  add        byte ptr [eax], al
0046b378  or         al, 0x70
0046b37a  add        byte ptr [eax], al
0046b380  cld        
0046b381  xchg       esp, eax
0046b382  dec        ecx
0046b383  add        byte ptr [ecx], dh
0046b386  add        byte ptr [eax], al
0046b388  or         al, 0x71
0046b38a  add        byte ptr [eax], al
0046b390  cld        
0046b391  xchg       esp, eax
0046b392  dec        ecx
0046b393  add        byte ptr [edx], dh
0046b396  add        byte ptr [eax], al
0046b398  or         al, 0x72
0046b39a  add        byte ptr [eax], al
0046b3a0  cld        
0046b3a1  xchg       esp, eax
0046b3a2  dec        ecx
0046b3a3  add        byte ptr [ebx], dh
0046b3a6  add        byte ptr [eax], al
0046b3a8  or         al, 0x73

// block 0046b3aa
0046b3aa  add        byte ptr [eax], al
0046b3b0  cld        
0046b3b1  xchg       esp, eax
0046b3b2  dec        ecx
0046b3b3  add        byte ptr [eax + eax], dh
0046b3b7  add        byte ptr [esp + esi*2], cl
0046b3ba  add        byte ptr [eax], al
0046b3c0  cld        
0046b3c1  xchg       esp, eax
0046b3c2  dec        ecx
0046b3c3  add        byte ptr [ebp], dh
0046b3c6  add        byte ptr [eax], al
0046b3c8  or         al, 0x75
0046b3ca  add        byte ptr [eax], al
0046b3d0  cld        
0046b3d1  xchg       esp, eax
0046b3d2  dec        ecx
0046b3d3  add        byte ptr [esi], dh
0046b3d6  add        byte ptr [eax], al
0046b3d8  or         al, 0x76
0046b3da  add        byte ptr [eax], al
0046b3e0  cld        
0046b3e1  xchg       esp, eax
0046b3e2  dec        ecx
0046b3e3  add        byte ptr [edi], dh
0046b3e6  add        byte ptr [eax], al
0046b3e8  or         al, 0x77
0046b3ea  add        byte ptr [eax], al
0046b3f0  cld        
0046b3f1  xchg       esp, eax
0046b3f2  dec        ecx
0046b3f3  add        byte ptr [eax], bh
0046b3f6  add        byte ptr [eax], al
0046b3f8  or         al, 0x78
0046b3fa  add        byte ptr [eax], al
0046b400  cld        
0046b401  xchg       esp, eax
0046b402  dec        ecx
0046b403  add        byte ptr [ecx], bh
0046b406  add        byte ptr [eax], al
0046b408  or         al, 0x79
0046b40a  add        byte ptr [eax], al
0046b410  cld        
0046b411  xchg       esp, eax
0046b412  dec        ecx
0046b413  add        byte ptr [edx], bh
0046b416  add        byte ptr [eax], al
0046b418  or         al, 0x7a
0046b41a  add        byte ptr [eax], al
0046b420  cld        
0046b421  xchg       esp, eax
0046b422  dec        ecx
0046b423  add        byte ptr [ebx], bh
0046b426  add        byte ptr [eax], al
0046b428  or         al, 0x7b
0046b42a  add        byte ptr [eax], al
0046b430  cld        
0046b431  xchg       esp, eax
0046b432  dec        ecx
0046b433  add        byte ptr [eax + eax], bh
0046b437  add        byte ptr [esp + edi*2], cl
0046b43a  add        byte ptr [eax], al
0046b440  cld        
0046b441  xchg       esp, eax
0046b442  dec        ecx
0046b443  add        byte ptr [ebp], bh
0046b446  add        byte ptr [eax], al
0046b448  or         al, 0x7d
0046b44a  add        byte ptr [eax], al
0046b450  cld        
0046b451  xchg       esp, eax
0046b452  dec        ecx
0046b453  add        byte ptr [esi], bh
0046b456  add        byte ptr [eax], al
0046b458  or         al, 0x7e
0046b45a  add        byte ptr [eax], al
0046b460  cld        
0046b461  xchg       esp, eax
0046b462  dec        ecx
0046b463  add        byte ptr [edi], bh
0046b466  add        byte ptr [eax], al
0046b468  or         al, 0x7f
0046b46a  add        byte ptr [eax], al
0046b470  cld        
0046b471  xchg       esp, eax
0046b472  dec        ecx
0046b473  add        byte ptr [eax + 0xc000000], al
0046b479  add        byte ptr [eax], 0x80
0046b47c  add        byte ptr [eax], al
0046b47e  add        byte ptr [eax], al
0046b480  cld        
0046b481  xchg       esp, eax
0046b482  dec        ecx
0046b483  add        byte ptr [ecx + 0xc000000], al
0046b489  add        dword ptr [eax], 0x80
0046b48f  add        ah, bh
0046b491  xchg       esp, eax
0046b492  dec        ecx
0046b493  add        byte ptr [edx + 0xc000000], al

// block 0046b499
0046b499  add        byte ptr [eax], 0x80
0046b49c  add        byte ptr [eax], al
0046b49e  add        byte ptr [eax], al
0046b4a0  cld        
0046b4a1  xchg       esp, eax
0046b4a2  dec        ecx
0046b4a3  add        byte ptr [ebx + 0xc000000], al
0046b4a9  add        dword ptr [eax], -0x80
0046b4ac  add        byte ptr [eax], al
0046b4ae  add        byte ptr [eax], al
0046b4b0  cld        
0046b4b1  xchg       esp, eax
0046b4b2  dec        ecx
0046b4b3  add        byte ptr [eax + eax - 0x7bf40000], al
0046b4ba  add        byte ptr [eax], al
0046b4c0  cld        
0046b4c1  xchg       esp, eax
0046b4c2  dec        ecx
0046b4c3  add        byte ptr [ebp + 0xc000000], al
0046b4c9  test       dword ptr [eax], eax
0046b4cb  add        byte ptr [eax], 0
0046b4ce  add        byte ptr [eax], al
0046b4d0  cld        
0046b4d1  xchg       esp, eax

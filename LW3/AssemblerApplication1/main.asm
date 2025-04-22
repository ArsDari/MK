.set ARR_SIZE=12

.dseg
arr:.BYTE ARR_SIZE

.cseg 
reset:
  rjmp main
 
main:
  ldi ZH,High(src*2)
  ldi ZL,Low(src*2)
  ldi YH,High(arr)
  ldi YL,Low(arr)
  ;טםטצטאכטחאצטÿטעונאעמנא
  ldi R16,ARR_SIZE
 
copy:
  lpm R18,Z+
  st Y+,R18
  out OCR0A,R18
  dec R16
  brne copy
 
while:
  ldi R17,0x00;פכאדמךמםקאםטÿצטךכא
  ldi R16,2;טעונאעמנהכÿfor
 
for:
  ld R18,Y+;v11
  ld R19,Y+;v12
  ld R20,Y+;v13
  ld R21,Y+;v14
  ld R22,Y+;v21
  ld R23,Y+;v22
  ld R24,Y+;v23
  ld R25,Y;v24
  sbiw YL,3
 
ComMod1:
  cp R21,R25
  breq ComMod2; סכףקאינאגוםסעגאבאיע
  brlt iteration;סכףקאיןונגמדמlongלוםüרוגעמנמדמ(ןונוץמהiteration) 
  rjmp replace; סכףקאיןונגמדמlongבמכüרוגעמנמדמ(ןונוץמהreplace)
 
ComMod2:
  cp R20,R24
  breq ComMod3; סכףקאינאגוםסעגאבאיע
  brlt iteration;סכףקאיןונגמדמlongלוםüרוגעמנמדמ(ןונוץמהiteration) 
  rjmp replace; סכףקאיןונגמדמlongבמכüרוגעמנמדמ(ןונוץמהreplace)
 
ComMod3:
  cp R19,R23
  breq ComMod4; סכףקאינאגוםסעגאבאיע
  brlt iteration;סכףקאיןונגמדמlongלוםüרוגעמנמדמ(ןונוץמהiteration) 
  rjmp replace; סכףקאיןונגמדמlongבמכüרוגעמנמדמ(ןונוץמהreplace)
 
ComMod4:
  cp R18,R22
  breq iteration; סכףקאינאגוםסעגאבאיע
  brlt iteration;סכףקאיןונגמדמlongלוםüרוגעמנמדמ(ןונוץמהiteration) 
  rjmp replace; סכףקאיןונגמדמlongבמכüרוגעמנמדמ(ןונוץמהreplace)
 
replace:
  ldi R17,0x11
  sbiw YL,4
  st Y+,R22
  st Y+,R23
  st Y+,R24
  st Y+,R25
  st Y+,R18
  st Y+,R19
  st Y+,R20
  st Y, R21
  sbiw YL,3
 
iteration:
  dec R16
  brne for
  ldi YH,High(arr)
  ldi YL,Low(arr)
  cpi R17,0x11
  breq while
 
outputlog:
ldi YH,High(arr)
  ldi YL,Low(arr)
ldi R16,0x07

log:
ld R18,Y+
out OCR0B,R18
dec R16
brne log
 
loop:
  nop
  rjmp loop
 
src: .dw 0x2C9B,0x3F7A,0xD08A,0xB4E1,0xF72E,0x5CD3
.model small

;================================================
; A tiny password checker.
; The user should enter password.
; If it is correct, then he is the Chosen One.
; Otherwise, he can go to hell.
; The right password can appear after string:
; "******************** 184 2 9 186 154 2 205 33" (Alt codes)
;---------------
; (c) AQ <alexeyqu@gmail.com>, DCAM MIPT, gr. 376
;================================================

.code

.386

org 100h

Main:   
;		jmp dbg1
;dbg1:		
;		mov byte ptr ss:dbg2, 0C3h
;
;dbg2:		nop	

		mov ax, 0900h
		mov dx, offset WlcmMsg
		int 21h
		
		mov di, offset Maypass
		
		push di
		
		call GetString
		
		pop di
		
		jmp Further
		
Maypass: 	db "********************"

Further:		
		mov si, offset Maypass
		mov di, offset Password

		push si
		push di

		call Strcmp

		pop di
		pop si
		
		cmp dx, 0000h
		je Correct
		
		mov ax, 0900h
		mov dx, offset BadpassMsg
		int 21h
		jmp Error
		
Correct:	
		mov ax, 0900h
		mov dx, offset GoodpassMsg
		int 21h
		
Finish:
		mov ax, 4C00h
		int 21h

BadFinish:
		mov ax, 4C01h
		int 21h

;================================================
;++++++++++++++++++++++++++++++++++++++++++++++++
;---------------FUNCTIONS------------------------
;++++++++++++++++++++++++++++++++++++++++++++++++
;================================================

;================================================
; GetString:	stores string from the input in memory
; Entry:	DI -> adress in memory
; Exit:		DI -> string
; Destr:	
;================================================
GetString	proc
		
		push bp			; making frame
		mov bp, sp
		
		mov di, [bp + 4]	;getting arg
		
		cmp di, 0000h		; if (!di) => Error
		je Error
		
		mov bl, "$"

GetStringNext:
		mov ah, 01h		; getting new char
		int 21h
		
		mov [di], al		; copy it to [di]
		
		inc di
		
		cmp al, bl
		je GetStringExit
		
		jmp GetStringNext
		
GetStringExit:		
		mov sp, bp
		pop bp
		
		ret
endp

;================================================
; Strcmp: 	Compares 2 strings
; Entry:  	SI -> 1st string, DI -> 2nd string
; Exit:   	CX = length, DX = 0 (equal)/1 (s1 > s2)/ 2 (s1 < s2)
; Destr:  	AL
;================================================
Strcmp		proc
 
		push bp
                mov bp, sp		; making frame
                  
		mov si, [bp + 6]
		mov di, [bp + 4]

		mov cx, 0000h    	; counter = 0

		cmp si, 0000h           ; if (!si) => Error
		je Error

		cmp di, 0000h           ; if (!di) => Error
		je Error

		mov al, "$"
		
StrcmpNext:
		cmpsb			; compare [si] with [di]
		ja StrcmpAbove
		jb StrcmpBelow
		
		dec di

		scasb
		je StrcmpEq
            
		loop StrcmpNext

StrcmpEq:
		mov dx, 0000h           ; str1 == str2 <=> si == di
		
		jmp StrcmpExit

StrcmpAbove:                     
		mov dx, 0001h           ; str1 > str2  <=> si >  di
		
		jmp StrcmpExit

StrcmpBelow:
		mov dx, 0002h           ; str1 < str2  <=> si <  di

		jmp StrcmpExit		

StrcmpExit:
		neg cx                  ; get minimal length 
                       
		mov sp, bp		; clearing frame
		pop bp
	
		ret
endp

;================================================
; PrintNum:  	prints decimal number 
; Entry:     	CX = number
; Exit:      	stack = result -> prints
; Destr:     	AX, DX
;================================================
PrintNum	proc
	
	  	mov bl, 0Ah		; decimal base
	
		push dx

		mov ax, '$'		; stop symbol
		push ax
                   
ConvertNextDigit:                                
		mov ax, cx
		
		div bl                  ; then divide, al = cx/bx, ah = cx mod bx

		mov cx, 0000h
		mov cl, al

		mov dx, 0000h
		mov dl, ah

		push dx                 ; CX mod BX

		cmp cl, 0               ; if CX was lesser than BX => one more digit left
		jne ConvertNextDigit

PrintNextDigit:
		pop ax			; pop digit
		
		cmp ax, '$'             ; check stop symbol
		je PrintFinish

		cmp ax, 9               ; if it's a digit
		ja Itsachar

		add ax, '0'		; To ASCII digit

		mov dl, al
		mov ah, 02h     	; print char
		int 21h

		jmp PrintNextDigit 

PrintFinish:	
                pop dx
		
		ret

Itsachar:	
		add ax, 'A'             ; convert to ASCII char digit A-F
		sub ax, 10   		; 10 -> 10 - 10 + 'A' = 'A'

		mov dl, al
		mov ah, 02h     	; print char
		int 21h

		jmp PrintNextDigit 
          
endp     

;================================================
; Getchar: 	getchar
; Entry:
; Exit:
; Destr:   	AX
;================================================
Getchar		proc

		mov ax, 0100h		; getchar()
		int 21h

		ret
endp
            
;================================================
;++++++++++++++++++++++++++++++++++++++++++++++++
;---------------LABLES---------------------------
;++++++++++++++++++++++++++++++++++++++++++++++++
;================================================

Error:		
		mov ax, 0900h                
		mov dx, offset ErrorMsg
		int 21h

		jmp BadFinish

;================================================
;++++++++++++++++++++++++++++++++++++++++++++++++
;---------------MESSAGES-------------------------
;++++++++++++++++++++++++++++++++++++++++++++++++
;================================================

ErrorMsg:	db 0Dh, 0Ah, "Error!$"

WlcmMsg:	db "Hello! Please enter the password here (it should end with dollar symbol): $"

BadpassMsg:	db 0Dh, 0Ah, "Wrong password! Now go to hell!$"

GoodpassMsg:	db 0Dh, 0Ah, "Wake up, Neo.. You're a white mouse.$"

Password:	db "zopapapakarlo$"

end Main
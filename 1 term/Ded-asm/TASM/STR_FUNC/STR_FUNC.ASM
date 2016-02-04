.model small

;================================================
; String and memory functions demo
; --------------
; String: 	1) Strlen
;		2) Strcpy
;		3) Strcmp
; Memory:	1) Memset
;		2) Memcpy
;		3) Memcmp 
;---------------
; (c) AQ <alexeyqu@gmail.com>, DCAM MIPT, gr. 376
;================================================

.code

.386

org 100h

Main:        
;---------------STRLEN--------------------------- 
		mov ah, 09h
		mov dx, offset StrHello
		int 21h
		call Getchar
		
		mov si, offset StrHello
 
		push si
                            
		call Strlen             
		call PrintNum
                      
		pop si

		call Getchar
		
;---------------STRCPY---------------------------
;		mov ax, 0900h
;		mov dx, offset StrEmpty
;		int 21h 
;                     
;		call Getchar
; 
;		mov si, offset StrHello
;		mov di, offset StrEmpty
;
;		push si
;		push di
;	
;		call Strcpy 
;                       
;		pop di
;		pop si
; 
;		mov ax, 0900h
;		mov dx, offset StrEmpty
;		int 21h  
;
;		call Getchar
;		
;---------------STRCMP---------------------------
;		mov ax, 0900h
;		mov dx, offset StrEmpty
;		int 21h
;		call Getchar 
;				
;		mov ax, 0900h
;		mov dx, offset StrHello
;		int 21h 
;		call Getchar
;
;		mov si, offset StrEmpty
;		mov di, offset StrHello
;
;		push si
;		push di
;
;		call Strcmp
;		
;		mov cx, dx
;		call PrintNum
;
;		pop di
;		pop si
;		
;		call Getchar
;
;---------------MEMSET---------------------------
;		mov di, offset MemEmpty
;		mov cx, 0010h
;		mov al, "*"
;
;		push di
;		push cx
;		push ax
;                   
;		call Memset
; 
;		pop di
;		pop cx
;		pop ax
;
;		call Getchar
;
;---------------MEMCMP---------------------------
;		mov si, offset MemBlock1
;		mov di, offset MemBlock2
;		mov cx, 5h
;
;		push si
;		push di
;		push cx
;
;		call Memcmp
;		
;		mov cx, dx
;		call PrintNum
;
;		pop cx
;		pop di
;		pop si
;		
;		call Getchar
;		
;--------------MEMCPY----------------------------
;		mov si, offset MemBlock1
;		mov di, offset MemBlock2
;		mov cx, 5h 
;		
;		push si
;		push di
;		push cx
;
;		call Memcpy
;                    
;		pop cx
;		pop di
;		pop si	                      
;		
;		call Getchar
;		
;---------------FINISH---------------------------
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
;---------------STRING-FUNCTIONS-----------------
;================================================

;================================================
; Strlen: 	Calculates string length
; Entry: 	DI -> string
; Exit: 	CX = length
; Destr: 	AL
;================================================
Strlen		proc

		push bp    
                mov bp, sp		; making frame
                          
		mov di, [bp + 4]	; getting args

		mov cx, 0000h		; cx = 0

		cmp di, 0000h		; if (!bx) => Error!	
		je Error

		mov al, "$"

StrlenNext:	
		scasb			; compare symbol with al, di++
		je StrlenExit

		loop StrlenNext 	; jmp to Next, cx--

StrlenExit: 		
		neg cx			; cx *= -1
		
		mov sp, bp		; clearing frame
		pop bp

		ret
endp

;================================================
; Strcpy: 	copies one string to another
; Entry:  	SI -> source, DI -> destination
; Exit:   	CX = length
; Destr:  	AX
;================================================
Strcpy		proc
  
		push bp
                mov bp, sp		; making frame
                   
		mov si, [bp + 6]	; getting args
		mov di, [bp + 4]

		mov cx, 0000h    	; counter = 0

		cmp si, 0000h           ; if (!si) => Error
		je Error

		cmp di, 0000h           ; if (!di) => Error
		je Error
		
		mov al, "$"

StrcpyNext:	             	
		movsb			; [si] -> [di], si++, di++
		dec di

		scasb  			; end-of-line
		je StrcpyExit

		loop StrcpyNext
		
StrcpyExit:
		neg cx                  ; get length

		sub di, cx              ; set di to start of destination string

		mov sp, bp		; clearing frame
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
;---------------MEMORY-FUNCTIONS-----------------
;================================================

;================================================
; Memset: 	sets memory array with value
; Entry:  	DI -> dest, CX = length, AL = value
; Exit:   	None. //DI -> dest
; Destr:  
;================================================
Memset		proc
		
		push bp
		mov bp, sp		; making frame
		
		mov di, [bp + 8]
		mov cx, [bp + 6]
		mov al, [bp + 4]

		cmp di, 0000h          	; if (!di) => Error
		je Error

		repne stosb		; fill it!

		mov sp, bp		; clearing frame
		pop bp

		ret
endp

;================================================
; Memcmp:	compares two memory arrays
; Entry:  	SI -> 1st array, DI -> 2nd array, CX = length
; Exit:   	DX = 0 (equal)/1 (m1 > m2)/ 2 (m1 < m2)
; Destr:  
;================================================
Memcmp		proc

		push bp
		mov bp, sp		; making frame
		
		mov si, [bp + 8]
		mov di, [bp + 6]
		mov cx, [bp + 4]
                        
		cmp si, 0000h           ; if (!si) => Error
		je Error

		cmp di, 0000h           ; if (!di) => Error
		je Error

MemcmpNext:
		cmpsb			; [si] ?= [di]
		ja MemcmpAbove
		jb MemcmpBelow
                            
		cmp cx, 0               ; if length == 0 => end of memory block
		je MemcmpEq
             
		loop MemcmpNext

MemcmpEq:
		mov dx, 0000h           ; str1 == str2 <=> si == di 
		
		jmp MemcmpExit

MemcmpAbove:                     
		mov dx, 0001h           ; str1 >  str2 <=> si >  di
		
		jmp MemcmpExit

MemcmpBelow:
		mov dx, 0002h           ; str1 <  str2 <=> si <  di

		jmp MemcmpExit		

MemcmpExit:     
		mov sp, bp		; clearing frame
		pop bp
      	
		ret
endp

;================================================
; Memcpy: 	copies one string to another
; Entry:  	SI -> source, CX = length
; Exit:   	DI -> destination
; Destr:  	AL
;================================================
Memcpy		proc
                  
                push bp
		mov bp, sp		; making frame
		
		mov si, [bp + 8]
		mov di, [bp + 6]
		mov cx, [bp + 4] 
                         
		cmp si, 0000h           ; if (!si) => Error
		je Error

		cmp di, 0000h           ; if (!di) => Error
		je Error

		repne movsb
		
		mov sp, bp
		pop bp
                
		ret		
endp

;================================================
;---------------MISC-FUNCTIONS-------------------
;================================================

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
;---------------STRINGS--------------------------
;++++++++++++++++++++++++++++++++++++++++++++++++
;================================================                         

StrEmpty:	db "____________________$"

StrHello:	db "Hello, world!$"

StrGoodBye:	db "Goodbye, world!$"

;------------------------------------------------
           
MemEmpty:	db 128 dup (0)

MemBlock1:      db "My_tiny_memo"

MemBlock2:	db "My_bigger_memo"

;================================================
;++++++++++++++++++++++++++++++++++++++++++++++++
;---------------MESSAGES-------------------------
;++++++++++++++++++++++++++++++++++++++++++++++++
;================================================

ErrorMsg:	db "Error!$"

end Main
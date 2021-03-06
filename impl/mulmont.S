.extern c_debug_print
  .macro debug_print_number isbig=0,char='a', value=%rax
  ## push %rax
  ## push %rbx
  ## push %rcx
  ## push %rdx
  ## push %rdi
  ## push %rsi
  ## push %r8
  ## push %r9
  ## push %r10
  ## push %r11
  ## mov $\isbig, %rdx
  ## mov $\char, %rdi
  ## mov \value, %rsi
  ## call c_debug_print
  ## pop %r11
  ## pop %r10
  ## pop %r9
  ## pop %r8
  ## pop %rsi
  ## pop %rdi
  ## pop %rdx
  ## pop %rcx
  ## pop %rbx
  ## pop %rax
.endm
  
  
  
  .global mulmont_asm
mulmont_asm:      
    .set SIZE, 136
  .set a_ptr, %rdi
  .set b_ptr, %rsi
  .set m_ptr, %r10
  mov %rdx, m_ptr

  .set mp, %rcx
  .set d_ptr,%r8
  .set e_ptr,%r9
  
  ## rax, rdx, scr_1, scr_2 are scratch
  ##   locals
  .set scr_1, %r14
  .set scr_2, %r15
  .set ctr, %rbx
  .set t0, %r12
  .set t1, %r13
  .set a_j, %r11
  .set q, %rbp
  push %rbp
  push %rbx
  push %r12
  push %r13
  push %r14
  push %r15
mulmod_init:
  ##   mov (b_ptr), b_0
  mov $16, ctr
mulmod_outer: 
  mov (a_ptr), a_j
  debug_print_number 0,'j', a_j
  ## Read d[0] into a register
  mov a_j, %rax 
  mulq (b_ptr) #rax = a_j*b_0
  mov (d_ptr), q                # q=d_0
  mov %rax, scr_2                 
  mov %rdx, t0                   # t0:scr_2 = a_j * b_0
  addq q, scr_2                 

  adc $0, t0                   # t0:scr_2 = (fp_word)a->dp[j]*(fp_word) b->dp[0] + (fp_word)d.dp[0]

  addq %rax, q
  sub (e_ptr), q           # q = a_j * b_0 + d_0 - e_0
  debug_print_number 0,'p', q
  imul mp, q
  mov q, %rax


  ##    q = mp * ( b->dp[0] * a->dp[j] + d.dp[0] - e.dp[0])#
  debug_print_number 0,'q', q
  mulq (m_ptr)   ## rax= q *m[0]
  debug_print_number 0,'t', t0
  addq (e_ptr), %rax
  mov %rdx, t1
  adc $0, t1
  debug_print_number 0,'t', t1

  .macro mulmod_inner repeat
    add $8, b_ptr
    add $8, m_ptr   
    add $8, d_ptr
    add $8, e_ptr   
  ## parameters: t0,t1, e_ptr, d_ptr, b_ptr, a_j
###  clobbers scr_1,scr_2,rax,rdx
    movq a_j, %rax
    mulq (b_ptr)                   #rdx:rax = a[j]*b[i]

  	mov %rdx, scr_1
	  mov %rax, scr_2                  #scr_1:scr_2 = a_j *b_i
	  mov q, %rax 
   	mulq (m_ptr)                  #rdx:rax = m_i * q
	  addq t0, scr_2              
	  adc $0, scr_1                #scr_1:scr_2 = a_j*b_i + t0
	  addq t1, %rax
	  adc $0, %rdx
	  addq (d_ptr), scr_2           #scr_1:scr_2 = a_j*b_i+t0+d[j]
	  adc $0, scr_1
	  addq (e_ptr), %rax
	  adc $0, %rdx
	### Ok to read a cacheline we just wrote?
	  movq scr_1, t0
	  movq scr_2, -8(d_ptr)
	  movq %rdx, t1
	  movq %rax, -8(e_ptr)



  
  .if \repeat
  mulmod_inner "(\repeat -1)"
  .endif
.endm
  ## repeat 16 times?
  mulmod_inner 16
mulmod_after:
  movq t0, (d_ptr)
  movq t1, (e_ptr)
 
  sub $SIZE,e_ptr
  sub $SIZE,d_ptr
  sub $SIZE,b_ptr
  sub $SIZE,m_ptr
  
  debug_print_number 1,'d', d_ptr
  debug_print_number 1,'e', e_ptr
  addq $8, a_ptr
  
  dec ctr
  jnz mulmod_outer
out: 
  pop %r15 
  pop %r14
  pop %r13
  pop %r12
  pop %rbx
  pop %rbp
  ret

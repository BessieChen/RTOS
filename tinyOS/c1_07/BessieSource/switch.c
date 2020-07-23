__asm void PendSV_Handler(void) //这里使用的是asm,并且函数名字一定是PendSV_Handler(),pendSV异常发生的时候,就会跳转到这里
{
	//我们的目标是:向register里面存入stack的值
	//所以,先找到stack的地址.
	//方法: 一个blockptr可以指向一大块的block的首地址(包含了stackptr等info),block.stackprt又可以指向一大块的stack的首地址
	IMPORT blockPtr
	LDR R0, =blockPtr
	LDR R0, [R0]
	LDR R0, [R0]
	
	STMDB R0!, {R4-R11}
	
	LDR R1, =blockPtr
	LDR R1, [R1]
	STR R0, [R1]

	ADD R4, R4, #1
	ADD R5, R5, #1
	
	LDMIA R0!, {R4-R11}
	
	BX LR
	
	
	

}
	
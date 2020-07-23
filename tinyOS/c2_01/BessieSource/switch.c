__asm void PendSV_Handler(void) //这里使用的是asm,并且函数名字一定是PendSV_Handler(),pendSV异常发生的时候,就会跳转到这里
{
	//我们的目标是:向register里面存入stack的值
	//所以,先找到stack的地址.
	//方法: 一个blockptr可以指向一大块的block的首地址(包含了stackptr等info),block.stackprt又可以指向一大块的stack的首地址
	IMPORT blockPtr
	LDR R0, =blockPtr	//将blockPtr里面存的值,赋值给r0
	LDR R0, [R0] 			//将r0指向的地址(是block的地址)的值,也就是说[R0]是block的第一个字节的内容,赋给r0(注意block的第一个字节的内容其实也是一个地址,就是stackptr,所以第一个字节就是stack第1024位的地址)
	LDR R0, [R0]			//将r0指向的地址的值,是stack的地址的值,也就是说[R0]是stack的第1024个元素
	
	STMDB R0!, {R4-R11}		//从左往右看,也就是R0的值(也就是stack的值),给R11-R4. STMDB是store给mem的意思, 会先把R0的值给R11,之后给R10,... 所以最后的栈顶是R4
												//其实,我觉得这里不应该这么写,因为我们之前是指向了stack的第1024个元素,按道理stack是空的
	//注意:R0应该是地址, 并且,我发现新的R0比旧的R0小了32,R4-R11一共8个寄存器,刚好一个寄存器存了4个字节
	
	LDR R1, =blockPtr			//这里和line7的思路一样,不过这里是要将新的R0(因为R0存了新的栈顶位置,因为是DB,所以新的R0比旧的小),赋值给stackPtr
	LDR R1, [R1]					//[R1]是block的第一个字节,这个字节是地址,是stack的首地址
	STR R0, [R1]					//[R1]是stack的第1024个元素?我们将新的R0的值赋给这个1024元素
												//其实,我觉得这里也是有点点奇怪,因为最后stack也没有用到,好像更新了stack的值也没什么用

	ADD R4, R4, #1
	ADD R5, R5, #1
	
	LDMIA R0!, {R4-R11}		//这里是将
	
	BX LR
	
	
	

}
	
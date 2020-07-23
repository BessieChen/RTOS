__asm void PendSV_Handler(void) //这里使用的是asm,并且函数名字一定是PendSV_Handler(),pendSV异常发生的时候,就会跳转到这里
{
	//我们的目标是:向register里面存入stack的值
	//所以,先找到stack的地址.
	//方法: 一个blockptr可以指向一大块的block的首地址(包含了stackptr等info),block.stackprt又可以指向一大块的stack的首地址
	
	IMPORT blockPtr
	LDR R0, =blockPtr	//将blockPtr的地址,赋值给r0.等于号相当于&
	LDR R0, [R0] 			//[r0]就是blockPtr里面的数据,这个值就是block的地址
	LDR R0, [R0]			//[r0]这里是block里面的数据,因为block是一个结构体,里面装了block.stackPtr,所以block的值就相当于是block.stackPtr的值, 而stackPtr的值其实是stack的地址(回忆block.stackPtr = &stack[1024]), 所以[r0]就是stack的第1024个元素的地址
	
	STMDB R0!, {R4-R11}		//将r4-r11的值,赋给r0指向的位置, STMDB是store给mem的意思, 会先把R0的值给R11,之后给R10,... 所以最后的栈顶是R4
												//在这之后,stack就有了register里面的数据
												//注意:R0应该是地址, 并且,我发现新的R0比旧的R0小了32,R4-R11一共8个寄存器,刚好一个寄存器存了4个字节
	
												//因为stack存了新的元素,所以stack的栈顶地址需要改变(新的地址是更小的),r0存了这个地址,所以接下来把r0的值,赋给block.stackPtr
	LDR R1, =blockPtr			//这里和line7的思路一样,将blockPtr的地址,赋值给r1
	LDR R1, [R1]					//[R1]就是blockPtr里面的数据,这个值就是block的地址
	STR R0, [R1]					//[R1]就是block里面的数据,因为block是一个结构体,里面装了block.stackPtr,现在把r0的值,赋给block.stackPtr

	ADD R4, R4, #1				//这里对register里面的值修改
	ADD R5, R5, #1
	
	LDMIA R0!, {R4-R11}		//这里是将stack里面存的r4-r11的值,有赋值给r4-r11,所以line21-22对register的修改就无效了
	
	BX LR 								//这里是返回到调用它的父亲函数,也就是返回到triggerPendSV()
	
	
	

}
	
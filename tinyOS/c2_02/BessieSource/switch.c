#include "tinyOS.h" //ÒòÎªĞèÒª¸øtTaskRunFirst()ÔÚswitch.cÖĞ¶¨Òå
#include "ARMCM3.h" //°üº¬ÁËÎÒÃÇÖ®ºóÓÃµ½µÄÖ¸Áî

//ÕâÊÇÎªÁË´¥·¢pendSVÊ¹ÓÃµÄ
#define NVIC_INT_CTRL				0xE000ED04 //Ò»¸ö32Î»¼Ä´æÆ÷,¿ÉÒÔ´¥·¢pendSV
#define NVIC_SYSPRI2				0xE000ED22 //Ò»¸ö8Î»¼Ä´æÆ÷,ÉèÖÃpendsvµÄÓÅÏÈ¼¶

#define NVIC_PENDSVSET  		0x10000000 //´¥·¢pendSVµÄÖµ,32bits
#define NVIC_PENDSV_PRI			0x000000FF //ÉèÖÃpendSVµÄÓÅÏÈ¼¶Îª×îµÍ,8bits

//Ğ´¼Ä´æÆ÷µÄºê
#define MEM32(addr)					*((volatile unsigned long*) addr)
#define MEM8(addr)					*((volatile unsigned char*) addr)
	

__asm void PendSV_Handler(void) //ÕâÀïÊ¹ÓÃµÄÊÇasm,²¢ÇÒº¯ÊıÃû×ÖÒ»¶¨ÊÇPendSV_Handler(),pendSVÒì³£·¢ÉúµÄÊ±ºò,¾Í»áÌø×ªµ½ÕâÀï
{
	//ÎÒÈÏÎª,Ó¦¸ÃÔÚ¸Õ½øÈëÕâ¸öº¯ÊıµÄÊ±ºò,ÒÑ¾­ÊÇ½«xPSP,R11µÈ¼Ä´æÆ÷ÀïÃæµÄÔªËØ,´æµ½PSPÖ¸ÏòµÄstackÖĞÁË, ½ÓÏÂÀ´¾ÍÊÇÎÒÃÇÊÖ¶¯½Ó¹ÜPSP,²¢ÇÒÍùstackÖĞ´æÈë¼Ä´æÆ÷R4-R11µÄÖµ //Èç¹ûPSP==0,ÎÒ¾õµÃ¿ÉÄÜ¾Í²»»á°ÑxPSPµÄ¶«Î÷,´æµ½PSPÖ¸ÏòµÄstackÖĞÁË
	IMPORT currentTask
	IMPORT nextTask
	
	MRS R0, PSP						//½«pspµÄÖµ,´æÈër0 //×¢ÒâÕâÀïÊÇMRS: PSP -> register
	CBZ R0, pendSV_store_only //ÅĞ¶Ïr0 != 0?
	
	//½ÓÏÂÀ´ÊÇ¾üÆÌ¸Ç×ßÈË
	STMDB R0!, {R4-R11}		//ÒòÎªRO¾ÍÖ±½ÓÖ¸ÏòÁËĞèÒª¹ÜÆÌ¸Ç×ßÈËµÄtaskµÄstack,ËùÒÔÖ±½Ó½«R4-T11Ñ¹ÈëÆäÖĞ, ×îºór0³ÉÎªÁËĞÂµÄÕ»¶¥µØÖ·
	
	//¾íÆÌ¸Ç×ßÈËµÄÊ±ºò,¾ÍĞèÒª±£´æĞÂµÄstackÖµÁË,(¶Ô±ÈÏÂÃæµÄ·Å¶«Î÷Ğ¶»õµÄÊ±ºò²»ĞèÒª±£´æĞÂµÄstackÕ»¶¥Ö¸Õë)
	//±£´æµÄĞÂstackÕ»¶¥Ö¸ÕëÊÇ, task->stack = ĞÂstackÕ»¶¥Ö¸Õë
	LDR R1, =currentTask  //R1´æµÄ×îºóÊÇcurrenttaskµÄµØÖ·
	LDR R1, [R1]					//R1×îºó´æµÄÊÇ, currenttaskµÄÄÚÈİ(task BµÄµØÖ·)
	STR R0, [R1]					//[r1]ÊÇtask BµÄÄÚÈİ,Ò²¾ÍÊÇstackµÄÕ»¶¥Ö¸Õë,×îºóÍøstack BÖĞ´æÈëĞÂÕ»¶¥Ö¸Õë·(R0µÄÖµ), »¹¼ÇµÃÂğ,stack BÖ®Ç°´æµÄÊÇ&taskStack[1024], ¼ûtask->stack = stack, ¶østack¾ÍÊÇ&taskStack[1024]
  
	
pendSV_store_only				//½ÓÏÂÀ´ÊÇĞ¶ÏÂ°ü¹ü
	LDR R0, =nextTask			
	LDR R0, [R0]					//Ö´ĞĞÍê,R0´æµÄÊÇnextTaskµÄÄÚÈİ(task AµÄµØÖ·)
	
	LDR R1, =currentTask	//R1´æµÄÊÇcurrentTaskµÄµØÖ·
	STR R0, [R1]					//[R1]:È¥µ½Õâ¸öcurrentTaskµÄÄÚÈİ(task BµÄµØÖ·). ½«[r1]¸Ä³Étask AµÄµØÖ·: ÓÚÊÇcurrentTaskµÄÄÚÈİ¾Í³ÉÁËtask AµÄµØÖ·,Ïàµ±ÓÚcurrentTask = nextTask	
	//ÔÚÕâ¸ö½ÚµãÓ¦¸ÃÑéÖ¤currentTaskµÄÄÚÈİ==nextTaskµÄÄÚÈİ
	
	LDR R0, [R0]					//´ËÊ±R0´æµÄÊÇtask AµÄÄÚÈİ,Õâ¸öÄÚÈİÊÇstack AµÄÕ»¶¥Ö¸Õë
	LDMIA R0!, {R4-R11}		//È¥Õâ¸östack AÕ»¶¥Ö¸ÕëµÄµØ·½,½«stack AÀïÃæµÄÔªËØ,pop³öÀ´,·Ö±ğ¸øR4-R11(ÏÈ¸øR4,×îºó¸øR11)

	//ÏÂÃæÕâÁ½²¿ÆäÊµ²»ĞèÒª,ÎªÊ²Ã´: ÎÒ±¾ÒâÊÇÏë´æR0,¿ÉÊÇÎÒ·¢ÏÖR0ºóÃæ¸³Öµ¸øÁËpsp,²¢ÇÒÖ®ºóĞèÒª¾íÆÕÌ©×ßÈËµÄÊ±ºò,ÊÇ´Ópsp¿ªÊ¼µÄµØ·½·ÅÈëstackµÄ,ËùÒÔÄÇ¸öÊ±ºò·ÅÈëstackµÄÎ»ÖÃ,¾ÍÊÇÎÒ±¾ÒâÏë´æR0µÄÎ»ÖÃ
	//LDR R1, [R1]					////¼û57ĞĞ,R1´æµÄ»¹ÊÇtask BµÄµØÖ·, ËùÒÔÖ´ĞĞÍêÕâÒ»ĞĞ,R1´æµÄÊÇtaskBµÄÄÚÈİ,Ò²¾ÍÊÇstack BµÄµØÖ·
	//STR R0, [R1]					//ÕâÀï[R1]ÒâË¼ÊÇstackµÄÄÚÈİ,stackµÄÄÚÈİ¾ÍÊÇÒ»¸öÖ¸Õë

	MSR PSP, R0						//×¢ÒâÕâÀïÊÇMSR: register -> PSP
	ORR LR, LR, #0x04			//ÕâÒ»¾ä±íÊ¾µÄÊÇ,Ö®ºóÎÒÃÇ½«Ê¹ÓÃPSPÕâ¸öÕ»Ö¸Õë, ÒòÎªPSPÖ¸ÏòµÄÊÇR0,ËùÒÔ,ÎÒÃÇ¿ÉÒÔ´ÓR0¿ªÊ¼pop,Ò²¾ÍÊÇ½«stack BÖĞµÄÊ£ÓàÊı¾İ´æµ½R13,R14µÈ
	BX LR
}

void tTaskRunFirst()
{
	//½«PSPÉèÖÃ³É0,ÒòÎªÔÚµÈ»áÎÒÃÇĞ´µÄasm´úÂëÖĞ,ÎÒÃÇ»áÅĞ¶ÏÈç¹ûPSP==0¾ÍÌø×ªµ½xxx
	__set_PSP(0);//__set_PSP()¾ÍÊÇARMCM3.hÀïÃæ¶¨ÒåµÄ
	
	//´¥·¢pendSVÒì³£, ´ÓcÓïÑÔ->¼Ä´æÆ÷´¥·¢->pendSV_handler()Ò²¾ÍÊÇasmÓïÑÔĞ´µÄ
	//ÉèÖÃÎª×îµÍÓÅÏÈ¼¶,ÄÇÃ´pendSVÒì³£µÄÓÅÏÈ¼¶ < ÖĞ¶ÏÓÅÏÈ¼¶ < SysTickÓÅÏÈ¼¶
	MEM8(NVIC_SYSPRI2) = NVIC_PENDSV_PRI;
	
	//ÔËĞĞÍêÒÔÏÂ´úÂë,pendSV±»´¥·¢
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
	
}

void tTaskSwitch()
{
	//ÔËĞĞÍêÒÔÏÂ´úÂë,pendSV±»´¥·¢
	MEM32(NVIC_INT_CTRL) = NVIC_PENDSVSET;
}

#include "tinyOS.h"
#include "ARMCM3.h" //°üº¬ÁËSysTick

//_BlockType_t,¿ÉÒÔ´æ´¢ºÜ¶àĞÅÏ¢,Ä¿Ç°Ö»ÓĞstackPtr
typedef struct _BlockType_t
{
	unsigned long* stackPtr;
}BlockType_t;


void delay(int count)
{
	while(count)
	{
		count--;
	}
}

//2.2	Ã¿¸ötaskÀïÃæ¶¼ÓĞĞèÒª·´×ªµÄflag
int task1Flag;
int task2Flag;

//2.1 ÉèÖÃÍêÁËtinyOS.hºó,ÎÒÃÇ¾ÍÉèÖÃÁ½¸östack,²¢ÇÒÉèÖÃÁ½¸ötask, ÔÙÉèÖÃÁ½¸ötaskÖ´ĞĞµÄº¯Êı
tTaskStack tTask1Env[1024];		//ÒòÎªÒ»¸ötTaskStackÖ»ÊÇÒ»¸öuint32_t,Ö»ÄÜ´ú±ístackÀïÃæµÄÒ»¸öÔªËØ.ËùÒÔÎÒÃÇÉèÖÃ³ÉÊı×éµÄĞÎÊ½
tTaskStack tTask2Env[1024];

tTask tTask1; //ÉèÖÃÒ»¸öÈÎÎñ,»¹¼ÇµÃÂğ,tTaskÊÇÒ»¸ö½á¹¹Ìå,ÀïÃæ°üº¬Ò»¸ötTaskStack*,Ö®ºóÎÒÃÇ¶ÔÕâ¸ötTaskStack*³õÊ¼»¯,¾Í¿ÉÒÔ½«tTask1ºÍtTask1EnvÁªÏµÆğÀ´ÁË
tTask tTask2;

//2.2 ³õÊ¼»¯ÁËÈÎÎñÖ®ºó,×ÔÈ»»¹ĞèÒªµ÷¶ÈÈÎÎñ,ËùÒÔĞèÒªÉèÖÃcurrentºÍnext task
tTask* currentTask;
tTask* nextTask;

//2.2. ²¢ÇÒ½«Á½¸öÈÎÎñ¶¼·Å½øÒ»¸öarrayÖĞ,Ò×ÓÚ¹ÜÀí
tTask* taskTable[2]; //ÕâÊÇÒ»¸ötTask*µÄÊı×é,Ò²¾ÍÊÇËµ,Êı×éµÄÃ¿Ò»¸öÔªËØ¶¼ÊÇÒ»¸ötTask*, Ò²¾ÍÊÇÃ¿Ò»¸öÔªËØ¶¼´æÒ»¸ötTaskÀàĞÍµÄµØÖ·

//2.2 ÎÒÃÇĞèÒªµ÷¶ÈÈÎÎñ,Ò²¾ÍÊÇËµ,ÎÒÃÇÒª¾ö¶¨ÄÄ¸öÈÎÎñÔÚÏÂÒ»ÂÖ¿ÉÒÔÊ¹ÓÃcpuºÍ×ÊÔ´, ÕâÒ»¸öº¯ÊıĞèÒª·Åµ½task1Entry()Ç°Ãæ,ÒòÎªtask1Entry()µ÷ÓÃÁËËü
void tTaskSched()
{
	if(currentTask == taskTable[0])
		nextTask = taskTable[1]; //ÕâÊÇÎªÁË¸øÔÙ´Î½øÈëpendSV()Òì³£Ò²¾ÍÊÇasm´úÂëÖĞ,nextTaskµÄ¸üĞÂ×öÆÌµæ
	else
		nextTask = taskTable[0];
	tTaskSwitch(); //¾ö¶¨ºÃÁËÏÂÒ»¸öÈÎÎñÊÇÊ²Ã´Ö®ºó,¾ÍÒªÇĞ»»ÁË,Õâ¸ötTaskSwitch()º¯ÊıÀïÃæ°üº¬ÁË´¥·¢pendSVÒì³£,ÒòÎªpendSVÒì³£ÖĞÊ¹ÓÃ»ã±à´úÂë,¿ÉÒÔ¿ØÖÆPC,ËùÒÔ¾Í¿ÉÒÔ×ªÏòĞÂµÄº¯ÊıÁË
}

//ÕâÀïÊÇÅäÖÃÎÒÃÇµÄ¼ÆÊ±Æ÷,´«ÈëµÄ²ÎÊıÊÇÎÒÃÇÏ£ÍûµÄºÁÃë
void tSetSysTickPeriod(uint32_t ms)
{
	//ÖØÖÃ¼ÆÊıÆ÷µÄÊ±ºòµÄÖµ:Ò²¾ÍÊÇËµ,Ã¿´Î¼ÆÊıÆ÷ÖĞ¶ÏµÄÊ±ºò,»á½«(µİ¼õ¼ÆÊıÆ÷)µÄÖµ-1,µ±µİ¼õ¼ÆÊıÆ÷µÄÖµ==0µÄÊ±ºò,»á½«µİ¼õ¼ÆÊıÆ÷µÄÖµÉèÖÃ³ÉÕâ¸öÖØÖÃ¼ÆÊıÆ÷µÄÖµ
	SysTick->LOAD = ms / 1000 * SystemCoreClock - 1; //ÆäÊµÎÒ²»ÊÇºÜÇå³şÎªÊ²Ã´-1, ¿ÉÄÜÊÇÒòÎª¾õµÃÉèÖÃ³ÉÖØÖÃ¼ÆÊıÆ÷µÄÊ±ºò,Ò²ĞèÒª»¨·ÑÊ±¼ä,ËùÒÔ-1Ïàµ±ÓÚÓÃÓÚÕâ¸ö»¨·ÑÊ±¼ä?
	
	//¼ûÏÂ²¹³ä
	NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1);
	
	//µİ¼õ¼ÆÊıÆ÷
	SysTick->VAL = 0;
	
	//ÆäËûÅäÖÃ
	SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | 
									SysTick_CTRL_TICKINT_Msk | //²úÉúÖĞ¶ÏÊ¹ÄÜµÄ±êÖ¾Î»
									SysTick_CTRL_ENABLE_Msk;	//Ê¹ÄÜ¼ÆÊıÆ÷µÄ±êÖ¾Î»
	
	//²¹³ä:
	/*
	1. SysTick_IRQnÊÇÖ¸systickµÄ»ùµØÖ
	2. __NVIC_PRIO_BITSÊÇ4µÄºê¶¨Òå,ÕâÀïÖ¸µÄÊÇÕ¼ÏÈÓÅÏÈ¼¶Îª4,ÒòÎªm3ÄÚºËÖĞÕ¼ÏÈÓÅÏÈ¼¶ºÍÏìÓ¦ÓÉÏØ¼¶Ò»¹²ÊÇ4Î»,ÕâÀïÕ¼ÏÈÓÅÏÈ¼¶ÒÑ¾­ÓĞ4Î»,ËùÒÔÏìÓ¦ÓÅÏÈ¼¶Ö»ÓĞ0Î»
	ÕâÀï´ú±íµÄÊÇ×îµÍÓÅÏÈ¼¶: 1<<4 -1, Ò²¾ÍÊÇ16-1=15,Ò²¾ÍÊÇ1111, ×¢ÒâÓÅÏÈ¼¶ÊÇ0µÄÊ±ºòÊÇ×î¸ßÓÅÏÈ¼¶
	*/
}

//·¢ÉúÊ±ÖÓÖĞ¶ÏÊ±µÄ´¦Àíº¯Êı
void SysTick_Handler()
{
	//ÖĞ¶ÏµÄÊ±ºòÓ¦¸Ã×öÊ²Ã´? Ó¦¸ÃÊÇÇĞ»»ÈÎÎñ: ĞèÒªÏÈµ÷¶ÈÏÂÒ»¸öÈÎÎñ,Ò²¾ÍÊÇÈ·¶¨ºÃnextTaskÊÇÊ²Ã´ -> ½ÓÏÂÀ´¾ÍÊÇ´¥·¢systickÒì³£
	
	//1. µ÷¶ÈÏÂÒ»¸öÈÎÎñ
	tTaskSched();
	
	//2. ½ÓÏÂÀ´¾ÍÊÇ´¥·¢systickÒì³£
	
}

void task1Entry(void * param) //ÉèÖÃtask1ÈÎÎñĞèÒªÖ´ĞĞµÄº¯Êı
{
	//ÒÔÏÂÁ½¾äÊÇÎªÁËÄÜ¹»paramÄÜ¹»±»Ê¹ÓÃ,ÕâÑù¾Í²»»á±»ÓÅ»¯(É¾³ı)
	unsigned long value = *(unsigned long *)param;
	value++;
	
	//ÔÚÕâÀïÌísystickµÄ³õÊ¼»¯º¯Êı,ÒòÎªsystickµÄ³õÊ¼»¯º¯ÊıtSetSysTickPeriod()ÖĞ,ÓĞÊ¹ÄÜ¼ÆÊıÆ÷µÄÖĞ¶Ï,ÎªÁË±ÜÃâÖĞ¶ÏÒı·¢µÄ»ìÂÒ,¾Í½«systickµÄ³õÊ¼»¯¼ÓÔÚÕâÀï
	//³õÊ¼»¯systick,ÉèÖÃÎª10ºÁÃë(²»¹ıÕæÊµµÄÊÇÖ´ĞĞÁË1.4Ãë,ÉñÆæ¶øÇÒ²»¹ÜÎÒ¸ÄÄÄÀï,¸Ğ¾õ¶¼Ã»ÓĞ±ä»¯),²¢ÇÒËûÏÔÊ¾µÄ1s,ÎÒ¸Ğ¾õÏñÊÇ¹ıÁËºÜ³¤Ê±¼ä!
	tSetSysTickPeriod(10);//ÒòÎªÖ®ºó¾Í¶¼ÔÚfor loopÀïÃæÁË,ÕâÀï¾ÍÖ»ÊÇÔËĞĞÁËÒ»´Î
	/*
	ÆäËûÔ­ÒòÎªÊ²Ã´ÔÚÕâÀï³õÊ¼»¯systick:
	1. ÒòÎªÈç¹ûÔÚÆäËûµØ·½³õÊ¼»¯systick,³õÊ¼»¯Ö®ºó,systick¾ÍÁ¢¼´¿ªÊ¼¹¤×÷ÁË,ËùÒÔµÚÒ»´Î·¢ÉúÖĞ¶ÏµÄÊ±ºò,¿ÉÄÜ¾Í»áµ÷ÓÃtTaskSched,µ«ÊÇÀîÂüÒªÅĞ¶ÏcurrentTaskÊÇ²»ÊÇ==0,ÒòÎªÎÒÃÇ¸Õ¿ªÊ¼currentTask²»ÊÇ
	*/
	//²¹³ä:¼ÇµÃÉèÖÃÊ±ÖÓÆµÂÊ:
	/*
	È¥system_ARMCM3.cÎÄ¼ş,½«41ºÍ42ĞĞ¸Ä³É
	#define  XTAL            ( 12000000U)     // Oscillator frequency   
	#define  SYSTEM_CLOCK    (1 * XTAL)	
	ÕâÀïÊÇ¸Ä³ÉÁË12Õ×ÆµÂÊ
	È»ºóÏµÍ³Ê±ÖÓSYSTEM CLock¾ÍÊÇ12Õ×*1
	12Õ×: ÏÖ´úººÓïÖĞ:12Õ×=12°ÙÍò,Ò²¾ÍÊÇ1Õ×==1°ÙÍò==1 million, Ò²¾ÍÊÇ1µÄºóÃæ6¸öÁã,ÕâÀï¾ÍÊÇ12µÄºóÃæ6¸öÁã
	µ«ÊÇ¼ÆËã»úÖĞ,Ò»Õ×ÊÇ2^30
	*/
	
	for(;;){
		task1Flag = 0;
		delay(100);
		task1Flag = 1;
		delay(100);
		
		//tTaskSched();//ÊÇsystickµ÷¶ÈÁË,ÕâÀï¾Í²»ĞèÒªÁË
	}
}
void task2Entry(void * param) 
{
	for(;;){
		task2Flag = 0;
		delay(100);
		task2Flag = 1;
		delay(100);
		
		//tTaskSched();//ÊÇsystickµ÷¶ÈÁË,ÕâÀï¾Í²»ĞèÒªÁË
	}
}

//ÉèÖÃ³õÊ¼»¯ÈÎÎñµÄº¯Êı: ³õÊ¼»¯stack: ½«tTask1ºÍtTask1EnvÁªÏµÆğÀ´, ³õÊ¼»¯taskº¯Êı
void taskInit(tTask* task, void (*func)(void*), void* param, tTaskStack* stack )
{
	//µÚÒ»¸ö²ÎÊı: ĞèÒª³õÊ¼»¯µÄÄÇ¸ötaskµÄµØÖ·
	//#2: taskĞèÒªÖ´ĞĞµÄº¯Êıfunc
	//#3: ¸Ãº¯ÊıfuncµÄ²ÎÊıparam
	//#4: taskĞèÒªµÄstackµÄµØÖ·
	
	//2.2 ¹ı³Ì: ÓÃRunFirst()´¥·¢pendSVÖĞ¶Ï -> ½«taskµÄstackÀïÃæµÄÔªËØ,¸³Öµ¸øregister -> ×îºóÈÃpspÖ¸ÏòstackµÄÕ»¶¥ -> pendSVÀûÓÃpsp×Ô¶¯½«stackÀïÃæÊ£ÓàµÄÔªËØÒÀ´Î´æ·Åµ½registerÖĞ
	//ËùÒÔ,ÎÒÃÇ¾ÍĞèÒªÔÚtask³õÊ¼»¯µÄÕâ¸öº¯ÊıtaskInit()ÖĞ,¾Í°ÑstackÀïÃæµÄÔªËØÌîºÃ
	
	//×îºóÊÇ¸øpendsv×Ô¶¯°ïÎÒÃÇ´ÓstackÀïÃæÈ¡³ö,¸øregisterµÄ.ÒòÎªÊÇºóÃæpop,ËùÒÔÏÈpush
	//×¢Òâ!pendsv×Ô¶¯´æ¸øregister,ÊÇÓĞËüµÄ¹Ì¶¨µÄË³ĞòµÄ,ËùÒÔÎÒÃÇÏòstack´æÖµµÄÊ±ºò,ĞèÒªµÄË³ĞòÒ²ÊÇ¹Ì¶¨µÄ
	//ÏÈstack -= 1, Ò²¾ÍÊÇµØÖ·-4(ÒòÎªstackÊÇtTaskStackÀàĞÍµÄÖ¸Õë,tTaskStackÀàĞÍÊÇuint32_t)
	//ÒòÎªstackÀïÃæµÄÖµ,×îºó¶¼Òª¸øregister,register´æµÄ¶¼ÊÇunsigned long,ËùÒÔ¼ÇµÃÇ¿ÖÆÀàĞÍ×ª»»
	*(--stack) = (unsigned long) 1 << 24; //¸øxPSP
	*(--stack) = (unsigned long) func; //¸øPC,Ò²¾ÍÊÇR15. ÕâÀï²»Ğ´³É&func,ÎÒ¾õµÃÊÇÒòÎª±¾À´func¾ÍÊÇÒ»¸öÖ¸Õë(Ò»¸öº¯ÊıÖ¸Õë),Ïàµ±ÓÚfuncµÄÖµ¾ÍÊÇµØÖ·,Í¬Ê±PCĞèÒªµÄÒ²ÊÇµØÖ·. ×¢Òâ,ÕâÒ»¾ä,¾Í¿ÉÒÔÈÃÖ®ºóµÄPCÖ¸ÏòÈë¿Úº¯Êı,ËùÒÔµ±³öÁËPendSV_HandlerÖ®ºó,¾Í»áÖ´ĞĞfuncÖ¸ÏòµÄº¯Êı(Ò²¾ÍÊÇÈÎÎñÖ´ĞĞµÄº¯Êı)
	*(--stack) = (unsigned long) 0x14; //¸øLR,Ò²¾ÍÊÇR14,µ«ÊÇÕâÀï²¢Ã»ÓĞÓÃµ½,¾ÍËæÒâ¸¶Ò»¸öÖµ,Ò»¸ö16½øÖÆµÄÖµ.×¢ÒâÕâÀï0x14Ö»ÊÇÒ»¸ö×Ö½Ú,µ«ÊÇÇ¿ÖÆÀàĞÍ×ª»»ÁË. todo: ²»ÊÇÎÒÃÇ½«LRÉèÖÃ³ÉÁË0x04Âğ?ÎªÊ²Ã´ÕâÀïÓÖÊÇ0x14?»á²»»á±»¸²¸Ç?
	*(--stack) = (unsigned long) 0x12;
	*(--stack) = (unsigned long) 0x3;
	*(--stack) = (unsigned long) 0x2;
	*(--stack) = (unsigned long) 0x1;
	*(--stack) = (unsigned long) param; //¸øR0¼Ä´æÆ÷,±àÒëÆ÷Ä¬ÈÏ:R0¼Ä´æÆ÷´æµÄ¾ÍÊÇPCÖ¸ÏòµÄº¯ÊıµÄÈë¿Ú²ÎÊıµÄÖµ,ËäÈ»paramÊÇÒ»¸övoid*ÀàĞÍ,µ«ÊÇÕâÀïÇ¿ÖÆÀàĞÍ×ª»»ÁË. ÒòÎªÕâÀïÖ»ÊÇ°ÑparamµÄÖµ´«½øÈ¥,¾ßÌåÔõÃ´´¦ÀíÕâ¸öparamµÄÖµ,ÊÇfuncÀïÃæ×öµÄÊÂÇé
	
	//Ê×ÏÈ,³öÕ»µÄÊÇ¸øR4-R11µÄ,ÒòÎªÊÇ×î¿ªÊ¼pop,ËùÒÔÔÚÕâÀïºópush
	//×¢Òâ,ÒòÎªÎÒÃÇÖ®ºóĞ´µÄasm´úÂëÖĞ,Ê¹ÓÃÁËLDMIA R0!, {R4-R11}, ËùÒÔÊÇÏÈpop¸øR4,ËùÒÔ×îºóÒ»¸öpushµÄÊÇ¸øR4µÄÖµ0x4
	*(--stack) = (unsigned long) 0x11;
	*(--stack) = (unsigned long) 0x10;
	*(--stack) = (unsigned long) 0x9;
	*(--stack) = (unsigned long) 0x8;
	*(--stack) = (unsigned long) 0x7;
	*(--stack) = (unsigned long) 0x6;
	*(--stack) = (unsigned long) 0x5;
	*(--stack) = (unsigned long) 0x4;
	
	//ÌîÂústackÖ®ºó,ÎÒÃÇ¾Í°ÑÖ¸ÏòÕ»¶¥µÄÖ¸Õëstack,¸³Öµ¸øtaskµÄstack³ÉÔ±
	task->stack = stack; //ÒòÎªtask->stackÊÇtTaskStack*,¶øÇÒstackÒ²ÊÇTaskStack*, ÒòÎªÕâÀïµÄtaskÊÇÖ¸Õë(µØÖ·),ËùÒÔÏàµ±ÓÚµØÖ·->stack
}


int main()
{
	//½«tTask°ó¶¨ÉÏÏàÓ¦µÄstackºÍfunc
	taskInit(&tTask1, task1Entry, (void*)0x11111111, &tTask1Env[1024]); //×¢ÒâtTask1EnvÊÇÒ»¸öuint32_tµÄÊı×é,ËùÒÔ²»ÄÜĞ´³É&tTaskEnv,¶øÒªĞ´³É&tTaskEnv[xx]
	taskInit(&tTask2, task2Entry, (void*)0x00000000, &tTask2Env[1024]); 
	
	//ÎªÎÒÃÇµÄtaskTableÉèÖÃ
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	
	nextTask = taskTable[0]; //ÒòÎªÎÒÃÇÊÇ¸Õ¿ªÊ¼,»¹Ã»ÓĞÖ´ĞĞµÄÈÎÎñ,Ò²¾ÍÊÇcurrentTask»¹Ã»ÓĞ±»³õÊ¼»¯,ËùÒÔÕâÀïÓ¦¸ÃÊÇnextTask±»³õÊ¼»¯
	
	//2.2 ¿ªÊ¼Ö´ĞĞµÚÒ»¸öÈÎÎñ: ºÍÒ»°ãµÄÈÎÎñÇĞ»»²»Í¬µÄµØ·½(Ç°Õß:Ö±½Ó°ÑstackÀïÃæµÄ¶«Î÷´æµ½register,ÏëÏóÈ¥¹ñÌ¨µÄÊ±ºòÃ»ÓĞÈË,µÚÒ»¸ö¿ÍÈËÖ±½Ó°Ñ¶«Î÷·Å×ÀÃæ | ºóÕß:µ±Ç°µÄÈËÏÈ°ÑregisterµÄ¶«Î÷´æµ½stackÖĞ,ĞÂÀ´µÄÈÓ°Ñ×Ô¼ºstackÀïÃæµÄ¶«Î÷´æµ½register,ÏëÏóÈ¥¹ñÌ¨µÄÊ±ºòÓĞÈË,Ç°ÃæµÄ¿ÍÈË:¾üÆÌ¸Ç×ßÈË,ºóÃæµÄ¿ÍÈË:ÔÙ·Å¶«Î÷)
	tTaskRunFirst(); //Õâ¸öº¯Êı,Ò²Òª´¥·¢pendSVÒì³£, È»ºóÒòÎªpendSV»áÖ´ĞĞasm´úÂë,¿ÉÒÔ¿ØÖÆpc,ËùÒÔ¿ÉÒÔ¿ØÖÆÏÂÒ»¸öÒªÈ¥µÄº¯Êı
	//×¢Òâ: ÒòÎªtTaskSwitch();tTaskRunFirst(); ¶¼ÒªÓÃasm´úÂëÀ´¹ÜÀíÓ²¼ş,ËùÒÔÎÒÃÇ¾Í°ÑÕâÁ½¸öº¯ÊıµÄ¶¨Òå,·Åµ½switch.cÖĞ,ÒòÎªÕâ¸öcÎÄ¼şÖĞ´¦ÀíÓĞ¹ØÓ²¼şµÄ²¿·Ö.µ«ÊÇÕâ¸öÁ½¸öº¯ÊıµÄÉùÃ÷,ĞèÒª·Åµ½tinyOS.hÖĞ
	
	return 0; //×¢Òâ,ÕâÀï²»»áÖ´ĞĞµ½return 0,ÒòÎªtTaskRunFirst()ÀïÃæ¾Í»áÒ»Ö±×ßµ½for loop
}

#include "tinyOS.h"
#include "ARMCM3.h" //°üº¬ÁËSysTick

//3.4 ¶¨Òå: ÈÎÎñ¾ÍĞ÷±í, taskTable
tBitmap taskPrioBitmap;

//2.2	Ã¿¸ötaskÀïÃæ¶¼ÓĞĞèÒª·´×ªµÄflag
int task1Flag;
int task2Flag;

//3.4 taskTableÏÖÔÚÀïÃæÓĞTINYOS_PRIO_COUNT¸öÔªËØ
tTask* taskTable[TINYOS_PRIO_COUNT]; 

//2.1 ÉèÖÃÍêÁËtinyOS.hºó,ÎÒÃÇ¾ÍÉèÖÃÁ½¸östack,²¢ÇÒÉèÖÃÁ½¸ötask, ÔÙÉèÖÃÁ½¸ötaskÖ´ĞĞµÄº¯Êı
tTaskStack tTask1Env[1024];		//ÒòÎªÒ»¸ötTaskStackÖ»ÊÇÒ»¸öuint32_t,Ö»ÄÜ´ú±ístackÀïÃæµÄÒ»¸öÔªËØ.ËùÒÔÎÒÃÇÉèÖÃ³ÉÊı×éµÄĞÎÊ½
tTaskStack tTask2Env[1024];
tTaskStack idleTaskEnv[1024]; //2.4

tTask tTask1; //ÉèÖÃÒ»¸öÈÎÎñ,»¹¼ÇµÃÂğ,tTaskÊÇÒ»¸ö½á¹¹Ìå,ÀïÃæ°üº¬Ò»¸ötTaskStack*,Ö®ºóÎÒÃÇ¶ÔÕâ¸ötTaskStack*³õÊ¼»¯,¾Í¿ÉÒÔ½«tTask1ºÍtTask1EnvÁªÏµÆğÀ´ÁË
tTask tTask2;
tTask tTaskIdle; //2.4

//2.2 ³õÊ¼»¯ÁËÈÎÎñÖ®ºó,×ÔÈ»»¹ĞèÒªµ÷¶ÈÈÎÎñ,ËùÒÔĞèÒªÉèÖÃcurrentºÍnext task
tTask* currentTask;
tTask* nextTask;
tTask* idleTask; //2.4


//3.2 ¶¨Òåµ÷¶ÈËøµÄ¼ÆÊıÆ÷, ÊÇuint8, Ö»ÓĞµ±µ÷¶ÈËø==0µÄÊ±ºò,²Å¿ÉÒÔ´¥·¢µ÷¶Èº¯ÊıtTaskSched()
uint8_t schedLockCount;

//3.4 ËäÈ»ÊÇ³õÊ¼»¯µ÷¶ÈËø,µ«ÊÇÕâÀïÒ²°üÀ¨ÁËbitmapµÄ³õÊ¼»¯,ÒòÎªmain()µÚÒ»¸öµ÷ÓÃµÄ¾ÍÊÇÕâ¸öº¯Êı.  µ÷¶ÈËøÇø±ğÓÚÁÙ½çÇø:ÁÙ½çÇøÊÇÊ¹ÓÃ__get_primask()À´¹Ø±ÕÖĞ¶Ï,Ò²¾ÍÊÇÊ¹ÓÃÁËARMCM3.hÎÄ¼şµÄº¯Êı,µ«ÊÇµ÷¶ÈËøÊ¹ÎÒÃÇ×Ô¼º¶¨ÒåµÄ
void tTaskSchedInit(void)
{
	schedLockCount = 0;
	tBitmapInit(&taskPrioBitmap);//3.4
}
//3.2 ¿ªÊ¼Ê¹ÓÃµ÷¶ÈËø, Ò²¾ÍÊÇÈÃµ÷¶Èº¯ÊıtTaskSched()Ê§Ğ§
void tTaskSchedDisable(void)//×¢ÒâÕâ¸öÃû×Ö, Õâ¸öÃû×ÖÊÇDisable,ÒòÎªÒªsched()Ê§Ğ§
{
	//ÒòÎªµ÷¶ÈËøÒ²ÊÇÈ«¾Ö±äÁ¿,ÓÖÒòÎªÕâ¸ötTaskSchedDisable()º¯ÊıÒ²»á±»systickÖĞ¶Ï»òÕßÈÎÎñµ÷ÓÃ(ÒòÎªËûÃÇÒªÊ¹ÓÃÕâ¸öº¯ÊıÈ¥±£»¤È«¾Ö±äÁ¿xxx),ËùÒÔÎÒÃÇĞèÒª±£»¤µ÷¶ÈËù,ÓÃÁÙ½çÇø±£»¤
	uint32_t status = tTaskEnterCritical();
	
	//¿ªÊ¼²Ù×÷µ÷¶ÈËø,×¢Òâ,ÒòÎªµ÷¶ÈËøÊÇuint8,×î´óÊÇ255, ËùÒÔÈç¹û==255,ÎÒÃÇ¾Í²»+1, ·ÀÖ¹Òç³ö
	if(schedLockCount <= 254)
	{
		schedLockCount++;
	}
	tTaskExitCritical(status);
}

//3.4 ÏÖÔÚĞèÒªĞŞ¸ÄÒ»ÏÂÎÒÃÇµÄtTaskSched()
void tTaskSched(void)
{
	//3.4 ÏÂÃæĞèÒªtTask*
	tTask* highestPrioTask;
	
	//3.2 ÎªÁË±£ÏÕÆğ¼û, ÎÒÃÇĞèÒªÅĞ¶Ïµ÷¶ÈËøÊÇ²»ÊÇ==0,Èç¹ûÈ·¶¨ÊÇ==0,²Å»áÖ´ĞĞºóÃæµÄÓï¾ä,·ñÔò¾Íreturn
	//Í¬Ñù, ÅĞ¶Ïµ÷¶ÈËøµÄÖµµÄÊ±ºò,ÎªÁË±£ÏÕ,»¹ÊÇÓÃÁÙ½çÇøÀ´±£»¤
	uint32_t status = tTaskEnterCritical();
	if(schedLockCount > 0)
	{
		tTaskExitCritical(status);
		return;
	}
	
	//3.4 ½ÓÏÂÀ´ÊÇĞŞ¸Ä²¿·Ö, Âß¼­ºÜ¼òµ¥, ¿´µ±Ç°µÄÈÎÎñÊÇ²»ÊÇÓÅÏÈ¼¶×î¸ßµÄ,Èç¹û²»ÊÇ,¾ÍÇĞ»»µ½ÓÅÏÈ¼¶×î¸ßµÄÉÏÃæ
	highestPrioTask = tTaskHighestTaskReady();
	if(highestPrioTask == currentTask)
	{
		tTaskExitCritical(status);
		return;
	}
	nextTask = highestPrioTask;
	tTaskSwitch();
	tTaskExitCritical(status);
}




//3.2 ÍË³öµ÷¶ÈËø, Ò²¾ÍÊÇÈÃµ÷¶Èº¯ÊıtTaskSched()ÉúĞ§ : µ±È»,Ö»ÓĞµ±µ÷¶ÈËø==0µÄÊ±ºò,²Å»áÈÃtTaskSched()ÉúĞ§, ÉúĞ§µÄÒâË¼ÊÇ:Á¢¼´Ö´ĞĞtTaskSched(), µ«ÊÇÖ´ĞĞtTaskSched()²»´ú±í¾ÍÈ¥ÕæµÄµ÷¶È,ÒòÎªÊÇ·ñÓ¦¸Ãµ÷¶ÈÒªĞèÒª¿´ÎÒ×Ô¼ºµÄdelayÊÇ²»ÊÇ==0,±ğÈËµÄdelayÊÇ²»ÊÇ==0
//×¢Òâ, Õâ¸öº¯ÊıĞèÒª·Åµ½tTaskSched()ÏÂÃæ,ÒòÎªĞèÒªÊ¹ÓÃµ½tTaskSched()
void tTaskSchedEnable(void)	
{
	//Í¬Àí,ĞèÒªÓÃÁÙ½çÇøÀ´±£»¤µ÷¶ÈËù
	uint32_t status = tTaskEnterCritical();
	
	//°²È«Æğ¼û,ÏÈÅĞ¶Ïµ÷¶ÈËøÊÇ²»ÊÇÒÑ¾­==0ÁË
	if(schedLockCount > 0)
	{
		schedLockCount--;
	}
	//Ö»ÓĞµ±µ÷¶ÈËø==0µÄÊ±ºò,²Å»áÈÃtTaskSched()ÉúĞ§, ÉúĞ§µÄÒâË¼ÊÇ:Á¢¼´Ö´ĞĞtTaskSched(), µ«ÊÇÖ´ĞĞtTaskSched()²»´ú±í¾ÍÈ¥ÕæµÄµ÷¶È,ÒòÎªÊÇ·ñÓ¦¸Ãµ÷¶ÈÒªĞèÒª¿´ÎÒ×Ô¼ºµÄdelayÊÇ²»ÊÇ==0,±ğÈËµÄdelayÊÇ²»ÊÇ==0
	if(schedLockCount == 0)
	{
		tTaskSched();
	}
	tTaskExitCritical(status);
}

//3.1 ÉèÖÃÁÙ½çÇø
uint32_t tTaskEnterCritical(void)
{
	uint32_t primask = __get_PRIMASK(); //ÕâÊÇ"ARMCM3.h"µÄ
	__disable_irq(); //"ARMCM3.h"µÄ,ÊÇ¹Ø±ÕÖĞ¶Ï
	return primask; //·µ»ØÖĞ¶ÏÊ¹ÄÜ
}

//3.1 ÍË³öÁÙ½çÇø
void tTaskExitCritical(uint32_t primask)
{
	__set_PRIMASK(primask);
}

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


//3.4 ĞèÒªĞŞ¸Ä,Ã¿´Îsystick¶¼ĞèÒª°ÑtaskTableÖĞµÄÃ¿Ò»¸öÈÎÎñµÄdelayTicks¼õÒ» // 2.4 ÒòÎªÏÖÔÚÃ¿¸öÈÎÎñ¶¼ÓĞÒ»¸ö¼ÆÊıÆ÷delayTicks, ÏÖÔÚÉè¼ÆÒ»¸öº¯Êı,ÄÜ¹»µİ¼õdelayTicks(Ïàµ±ÓÚÈíÑÓ³Ù)
void tTaskSysTickHandler()
{
	uint32_t status = tTaskEnterCritical();
	int i;
	for(i = 0; i < TINYOS_PRIO_COUNT; i++) //3.4
	{
		if(taskTable[i] == (tTask*)0) //3.4 ¼ÇµÃÒªÇ¿ÖÆÀàĞÍ×ª»»,ÎÒ·¢ÏÖÕâÀï²»ÄÜÓÃnull»òÕßNULL
		{
			continue; //Ê×ÏÈ,ÎÒÒªÅĞ¶ÏtaskTableµÄÄÄ¸öÔªËØÊÇ²»ÊÇÓĞÈÎÎñ
		}
		
		if(taskTable[i]->delayTicks > 0) 
		{
			taskTable[i]->delayTicks--;
		}
		else
		{
			tBitmapSet(&taskPrioBitmap, taskTable[i]->prio); //3.4 ÒòÎªÈç¹ûÄ³¸ötaskµÄdelayTick==0, ËµÃ÷Õâ¸ötaskÒ²Òª½øÈë¾ÍĞ÷×´Ì¬ÁË, Î»Í¼ÖĞµÄ¶ÔÓ¦Î»ÖÃÉèÖÃ³É1
		}
	}
	tTaskExitCritical(status);
	//ÒòÎªÉÏÃæµİ¼õÁËdelayTicks,ËùÒÔÓĞ¿ÉÄÜÓĞÒ»¸öÒÔÉÏµÄÈÎÎñÑÓ³ÙÍê±Ï,¿ÉÒÔ±»¿ªÆôÁË. ÒÔÏÂµÄtTaskSched()ÀïÃæ¾Í¿ÉÒÔÅĞ¶ÏÊÇ·ñdelayTicks==0,ÊÇ·ñ¿ÉÒÔ±»¿ªÆô
	tTaskSched();
}

//2.4 ·¢ÉúÊ±ÖÓÖĞ¶ÏÊ±µÄ´¦Àíº¯Êı, ¾ÍÏñÊÇpendSVÒ»Ñù,ÊÇÏµÍ³»á×Ô¶¯µ÷ÓÃµ½ÕâÀïµÄ,×¢Òâº¯ÊıÃû²»ÒªĞ´´í
void SysTick_Handler()
{
	//ÖĞ¶ÏµÄÊ±ºòÓ¦¸Ã×öÊ²Ã´? Ó¦¸ÃÊÇÇĞ»»ÈÎÎñ: ĞèÒªÏÈµ÷¶ÈÏÂÒ»¸öÈÎÎñ,Ò²¾ÍÊÇÈ·¶¨ºÃnextTaskÊÇÊ²Ã´ -> ½ÓÏÂÀ´¾ÍÊÇ´¥·¢systickÒì³£
	
	//1. µ÷¶ÈÏÂÒ»¸öÈÎÎñ
	//tTaskSched(); //2.4ÖĞ¾Í²»ÄÜÖ±½Óµ÷ÓÃÕâ¸öº¯Êı
	
	//2.4, ĞèÒªÏÈµİ¼õdelayTicks,È»ºóµ÷ÓÃtTaskSched()
	tTaskSysTickHandler(); //Õâ¸öº¯ÊıÀïÃæ»áµ÷ÓÃtTasksched()
	
}

//3.4 ÕâÀïĞèÒª½«ĞŞ¸Ä //2.4 ÊµÏÖÑÓÊ±º¯Êı, ÒòÎªÕâ¸öº¯Êı¿Ï¶¨ÊÇcurrentTaskµ÷ÓÃµÄ,ËùÒÔÀïÃæÖ±½ÓĞ´currentTask->xxx
void tTaskDelay(uint32_t delay)
{
	uint32_t status = tTaskEnterCritical();
	
	currentTask->delayTicks = delay; //Ò²¾ÍÊÇÎªcurrentTaskÉèÖÃĞèÒªÑÓÊ±¶à¾Ã
	tBitmapClear(&taskPrioBitmap, currentTask->prio); //3.4 ÒòÎªÑÓÊ±ËµÃ÷currentTask²»ĞèÒªcpu,¼ÈÈ»²»ĞèÒª,ËµÃ÷¾ÍĞ÷±íÖĞ¶ÔÓ¦µÄÔªËØ¾ÍÉèÖÃ³É0
	//3.4 ×¢Òâ:¼´±ãclear()ÁË,µ«ÊÇcurrentTaskµÄprio»¹ÊÇÃ»ÓĞ±ä»¯, taskTableÖĞÒ²»¹ÊÇÓĞÕâ¸öÈÎÎñ
	
	tTaskExitCritical(status);
	tTaskSched();//ËùÒÔÏÖÔÚcurrentTaskÊÇ·ÅÆúÁËcpu,ÏÖÔÚÓÉtTaskSched()À´ÅĞ¶Ï»¹ÓĞË­ÊÇĞèÒªcpuµÄ
}

//2.4 ÕâÀïÊÇÅäÖÃÎÒÃÇµÄ¼ÆÊ±Æ÷,´«ÈëµÄ²ÎÊıÊÇÎÒÃÇÏ£ÍûµÄºÁÃë
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




//3.3 ÎÒ×Ô¼ºÉè¼ÆµÄ, ¼ì²â±¨´íµÄº¯Êı
void ifError(void)
{
	
}

//2.1 ÉèÖÃtask1ÈÎÎñĞèÒªÖ´ĞĞµÄº¯Êı
void task1Entry(void * param) 
{
	tSetSysTickPeriod(1);
	for(;;){
		task1Flag = 0;
		tTaskDelay(1);
		task1Flag = 1;
		tTaskDelay(1);

	}
}
void task2Entry(void * param) 
{
	for(;;){

		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
	}
}


//2.4 Ìí¼ÓÒ»¸ö¿ÕÏĞÈÎÎñ, ÀïÃæÊ²Ã´¶¼²»×ö, µÈ×ÅÆäËûÈÎÎñµÄ½áÊøÑÓ³Ù
void idleTaskEntry(void* param)
{
	for(;;)
	{}
}



//3.4 ÉèÖÃÓÅÏÈ¼¶
void taskInit(tTask* task, void (*func)(void*), void* param, uint32_t prio, tTaskStack* stack )
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
	task->delayTicks = 0; //2.4, ÒòÎªtinyOS.hÖĞÍùtask½á¹¹ÌåÌí¼ÓÁË³ÉÔ±delayTicks, ËùÒÔÕâÀïÒ²ÒªĞ´ÉÏ
	task->prio = prio; //3.4
	
	//3.4 ³õÊ¼»¯Ò»¸ötaskÖ®ºó, Òª°Ñtask·Åµ½taskTableµÄÏàÓ¦Î»ÖÃ
	taskTable[prio] = task; //×¢Òâ,ÕâÀïµÄtask±¾Éí¾ÍÊÇÒ»¸öµØÖ·,ËùÒÔ²»ÒªĞ´³É = &task;
	
	//3.4 Òª½«¾ÍĞ÷±íÖĞµÄÏàÓ¦Î»ÖÃÉèÖÃ³É1
	tBitmapSet(&taskPrioBitmap, prio);
}

//3.4 ·µ»Øµ±Ç°×î¸ßÓÅÏÈ¼¶µÄÈÎÎñ
tTask* tTaskHighestTaskReady(void) //×¢Òâ,ÕâÀï²»ĞèÒª´«Èë&taskPrioBitmap×÷Îª²ÎÊı,ÒòÎªÄ¬ÈÏÊÇÈ«¾Ö±äÁ¿. µ±È»ÄãÏë¸Ä³ÉÒªÊäÈë²ÎÊıµÄÒ²¿ÉÒÔ
{
	uint32_t prio = tBitmapGetFirstSet(&taskPrioBitmap);
	return taskTable[prio];
}

int main()
{
	
	//3.4 ÒòÎªshareCountÊÇÎÒÃÇ×Ô¼ºµÄ¶¨ÒåµÄµ÷¶ÈËø,ËùÒÔĞèÒª³õÊ¼»¯, Õâ¸öº¯ÊıÀïÃæ°üº¬ÁË ³õÊ¼»¯¾ÍĞ÷±í, Ò²¾ÍÊÇbitmapµÄ³õÊ¼»¯
	tTaskSchedInit();

	
	//½«tTask°ó¶¨ÉÏÏàÓ¦µÄstackºÍfunc
	taskInit(&tTask1, task1Entry, (void*)0x11111111, 0, &tTask1Env[1024]); //3.4 ÉèÖÃ³É×î¸ßÓÅÏÈ¼¶
	taskInit(&tTask2, task2Entry, (void*)0x00000000, 1, &tTask2Env[1024]); 
	taskInit(&tTaskIdle, idleTaskEntry, (void*)0x00000001, TINYOS_PRIO_COUNT - 1, &idleTaskEnv[1024]); //3.4 ÉèÖÃ³É×îµÍÓÅÏÈ¼¶,31. ÕâÀïÊ¹ÓÃµÄÊÇÅäÖÃÎÄ¼ştConfig.hÖĞµÄºê¶¨Òå
	
	//ÎªÎÒÃÇµÄtaskTableÉèÖÃ
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	idleTask = &tTaskIdle; //2.4
	
	//3.4 nextTaskÊÇ×î¸ßÓÅÏÈ¼¶µÄÈÎÎñ
	nextTask = tTaskHighestTaskReady(); //É¾³ı: nextTask = taskTable[0]; //ÒòÎªÎÒÃÇÊÇ¸Õ¿ªÊ¼,»¹Ã»ÓĞÖ´ĞĞµÄÈÎÎñ,Ò²¾ÍÊÇcurrentTask»¹Ã»ÓĞ±»³õÊ¼»¯,ËùÒÔÕâÀïÓ¦¸ÃÊÇnextTask±»³õÊ¼»¯
	
	//2.2 ¿ªÊ¼Ö´ĞĞµÚÒ»¸öÈÎÎñ: ºÍÒ»°ãµÄÈÎÎñÇĞ»»²»Í¬µÄµØ·½(Ç°Õß:Ö±½Ó°ÑstackÀïÃæµÄ¶«Î÷´æµ½register,ÏëÏóÈ¥¹ñÌ¨µÄÊ±ºòÃ»ÓĞÈË,µÚÒ»¸ö¿ÍÈËÖ±½Ó°Ñ¶«Î÷·Å×ÀÃæ | ºóÕß:µ±Ç°µÄÈËÏÈ°ÑregisterµÄ¶«Î÷´æµ½stackÖĞ,ĞÂÀ´µÄÈÓ°Ñ×Ô¼ºstackÀïÃæµÄ¶«Î÷´æµ½register,ÏëÏóÈ¥¹ñÌ¨µÄÊ±ºòÓĞÈË,Ç°ÃæµÄ¿ÍÈË:¾üÆÌ¸Ç×ßÈË,ºóÃæµÄ¿ÍÈË:ÔÙ·Å¶«Î÷)
	tTaskRunFirst(); //Õâ¸öº¯Êı,Ò²Òª´¥·¢pendSVÒì³£, È»ºóÒòÎªpendSV»áÖ´ĞĞasm´úÂë,¿ÉÒÔ¿ØÖÆpc,ËùÒÔ¿ÉÒÔ¿ØÖÆÏÂÒ»¸öÒªÈ¥µÄº¯Êı
	//×¢Òâ: ÒòÎªtTaskSwitch();tTaskRunFirst(); ¶¼ÒªÓÃasm´úÂëÀ´¹ÜÀíÓ²¼ş,ËùÒÔÎÒÃÇ¾Í°ÑÕâÁ½¸öº¯ÊıµÄ¶¨Òå,·Åµ½switch.cÖĞ,ÒòÎªÕâ¸öcÎÄ¼şÖĞ´¦ÀíÓĞ¹ØÓ²¼şµÄ²¿·Ö.µ«ÊÇÕâ¸öÁ½¸öº¯ÊıµÄÉùÃ÷,ĞèÒª·Åµ½tinyOS.hÖĞ
	
	return 0; //×¢Òâ,ÕâÀï²»»áÖ´ĞĞµ½return 0,ÒòÎªtTaskRunFirst()ÀïÃæ¾Í»áÒ»Ö±×ßµ½for loop
}

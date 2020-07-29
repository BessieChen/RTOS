#include "tinyOS.h"
#include "ARMCM3.h" //°üº¬ÁËSysTick

//3.2 Ìí¼ÓÈÎÎñºÍÖĞ¶Ï¶¼ÄÜ·ÃÎÊµÄÈ«¾Ö±äÁ¿
int shareCount;

//2.2	Ã¿¸ötaskÀïÃæ¶¼ÓĞĞèÒª·´×ªµÄflag
int task1Flag;
int task2Flag;

//2.2. ²¢ÇÒ½«Á½¸öÈÎÎñ¶¼·Å½øÒ»¸öarrayÖĞ,Ò×ÓÚ¹ÜÀí
tTask* taskTable[2]; //ÕâÊÇÒ»¸ötTask*µÄÊı×é,Ò²¾ÍÊÇËµ,Êı×éµÄÃ¿Ò»¸öÔªËØ¶¼ÊÇÒ»¸ötTask*, Ò²¾ÍÊÇÃ¿Ò»¸öÔªËØ¶¼´æÒ»¸ötTaskÀàĞÍµÄµØÖ·

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

//3.2 ³õÊ¼»¯µ÷¶ÈËø, Çø±ğÓÚÁÙ½çÇø:ÁÙ½çÇøÊÇÊ¹ÓÃ__get_primask()À´¹Ø±ÕÖĞ¶Ï,Ò²¾ÍÊÇÊ¹ÓÃÁËARMCM3.hÎÄ¼şµÄº¯Êı,µ«ÊÇµ÷¶ÈËøÊ¹ÎÒÃÇ×Ô¼º¶¨ÒåµÄ
void tTaskSchedInit(void)
{
	schedLockCount = 0;
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

//3.2 ÏÖÔÚĞèÒªĞŞ¸ÄÒ»ÏÂÎÒÃÇµÄtTaskSched()
void tTaskSched(void)
{
	//2.4ÒÔÏÂµÄ¶¼ÒªÉ¾µô
	/*
	if(currentTask == taskTable[0])
		nextTask = taskTable[1]; //ÕâÊÇÎªÁË¸øÔÙ´Î½øÈëpendSV()Òì³£Ò²¾ÍÊÇasm´úÂëÖĞ,nextTaskµÄ¸üĞÂ×öÆÌµæ
	else
		nextTask = taskTable[0];
	tTaskSwitch(); //¾ö¶¨ºÃÁËÏÂÒ»¸öÈÎÎñÊÇÊ²Ã´Ö®ºó,¾ÍÒªÇĞ»»ÁË,Õâ¸ötTaskSwitch()º¯ÊıÀïÃæ°üº¬ÁË´¥·¢pendSVÒì³£,ÒòÎªpendSVÒì³£ÖĞÊ¹ÓÃ»ã±à´úÂë,¿ÉÒÔ¿ØÖÆPC,ËùÒÔ¾Í¿ÉÒÔ×ªÏòĞÂµÄº¯ÊıÁË
	*/
	
	//2.4 ²½Öè:
	/*
		1. Èç¹ûµ±Ç°ÈÎÎñÊÇidle
			1. Èç¹ûtask1µÄdelayTicks==0, ÏÂÒ»¸öÈÎÎñ¾ÍÊÇtask1
			2. task2Í¬ÉÏ
			3. ¼ÙÉèÁ½¸ötaskµÄdelayTicks != 0, ÄÇÃ´Ê²Ã´¶¼²»ÓÃ×ö, ÒòÎªÏÂÒ»¸öÈÎÎñ»¹ÒÀ¾ÉÊÇµ±Ç°ÈÎÎñidelTask
			ÎÒµÄÒÉÎÊ:Èç¹ûtask1ºÍtask2¶¼ÊÇdelayTicks==0,µ«ÊÇÕâ¸öº¯ÊıÈ·ÊÇÏÈÅĞ¶ÏÁËtask1
		2. Èç¹ûµ±Ç°ÈÎÎñÊÇtask1
			1. Èç¹ûtask2µÄdT==0, ÏÂÒ»¸öÈÎÎñ¾ÍÊÇtask2
			2. Èç¹ûtask1µÄdT!=0, Ò²¾ÍÊÇËµµ±Ç°task1»¹ÊÇ²»ĞèÒªcpu,ÒòÎª×ßµ½µÚ¶ş²½,ËµÃ÷task2µÄdT!=0, ¼ÈÈ»ËûÃÇÁ©¶¼²»Òªcpu, ´ËÊ±ÏÂÒ»¸öÈÎÎñÊÇidleTask
			3. ¼ÙÉètask1µÄdT == 0, ÄÇÃ´Ê²Ã´¶¼²»ÓÃ×ö, ÒòÎªÏÂÒ»¸öÈÎÎñ»¹ÒÀ¾ÉÊÇµ±Ç°ÈÎÎñtask1
			ÎÒµÄÒÉÎÊ:ÄÇ¾ÍÊÇÈç¹ûtask1ºÍ2µÄdT¶¼==0,ÄÇÒ²ÒÀ¾ÉÓÅÏÈ¸øtask2,ÒòÎªÏÈÅĞ¶Ïtask2
		3. Èç¹ûµ±Ç°ÈÎÎñÊÇtask2
			1.2.3Í¬ÉÏ
	*/
	
	//3.2 ÎªÁË±£ÏÕÆğ¼û, ÎÒÃÇĞèÒªÅĞ¶Ïµ÷¶ÈËøÊÇ²»ÊÇ==0,Èç¹ûÈ·¶¨ÊÇ==0,²Å»áÖ´ĞĞºóÃæµÄÓï¾ä,·ñÔò¾Íreturn
	//Í¬Ñù, ÅĞ¶Ïµ÷¶ÈËøµÄÖµµÄÊ±ºò,ÎªÁË±£ÏÕ,»¹ÊÇÓÃÁÙ½çÇøÀ´±£»¤
	uint32_t status = tTaskEnterCritical();
	if(schedLockCount > 0)
	{
		tTaskExitCritical(status);
		return;
	}
	if(currentTask == idleTask) //×¢ÒâidelTaskÊÇÖ¸Õë, tTask* idleTask = &tTaskIdle; //extern tTask* idleTask; //ĞèÒªÔÚtinyOS.hÀïĞ´ÉÏÕâÒ»¾ä,·ñÔòmain.c»á±¨´í, ÒªÃ´¾ÍÊÇÈ«¾Ö±äÁ¿¶¼Ğ´ÔÚÕâ¸öº¯ÊıÇ°Ãæ
	{
		if(taskTable[0]->delayTicks == 0)
		{
			nextTask = taskTable[0];
		}
		else if(taskTable[1]->delayTicks == 0)
		{
			nextTask = taskTable[1];
		}
		else //Ò²¾ÍÊÇÁ½¸ötaskµÄdT != 0, ËûÁ©¶¼²»Òªcpu
		{
			tTaskExitCritical(status);
			return; //ËùÒÔnextTaskÒÀ¾ÉÊÇcurrentTask,ºÍÖ®Ç°Ò»Ñù
		}
	}
	else if(currentTask == taskTable[0])
	{
		if(taskTable[1]->delayTicks == 0)
		{
			nextTask = taskTable[1]; //Èç¹ûtask1ºÍ2µÄdT¶¼==0,ÄÇÒ²ÒÀ¾ÉÓÅÏÈ¸øtask2,ÒòÎªÏÈÅĞ¶Ïtask2
		}
		else if(taskTable[0]->delayTicks != 0)
		{
			nextTask = idleTask; //ËµÃ÷Ò²¾ÍÊÇÁ½¸ötaskµÄdT != 0, ËûÁ©¶¼²»Òªcpu, ËùÒÔ¸øidle
		}
		else //ËµÃ÷task2µÄdT != 0, µ«ÊÇtask1µÄdT == 0, ËùÒÔnextTaskÒÀ¾ÉÊÇtask1
		{
			tTaskExitCritical(status);
			return;
		}
	}
	else //currentTask == taskTable[1]
	{
		if(taskTable[0]->delayTicks == 0)
		{
			nextTask = taskTable[0]; //Èç¹ûtask1ºÍ2µÄdT¶¼==0,ÄÇÒ²ÒÀ¾ÉÓÅÏÈ¸øtask1,ÒòÎªÏÈÅĞ¶Ïtask1
		}
		else if(taskTable[1]->delayTicks != 0)
		{
			nextTask = idleTask; //ËµÃ÷Ò²¾ÍÊÇÁ½¸ötaskµÄdT != 0, ËûÁ©¶¼²»Òªcpu, ËùÒÔ¸øidle
		}
		else //ËµÃ÷task1µÄdT != 0, µ«ÊÇtask2µÄdT == 0, ËùÒÔnextTaskÒÀ¾ÉÊÇtask2
		{
			tTaskExitCritical(status);
			return;
		}
	}
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


//2.4 ÒòÎªÏÖÔÚÃ¿¸öÈÎÎñ¶¼ÓĞÒ»¸ö¼ÆÊıÆ÷delayTicks, ÏÖÔÚÉè¼ÆÒ»¸öº¯Êı,ÄÜ¹»µİ¼õdelayTicks(Ïàµ±ÓÚÈíÑÓ³Ù)
void tTaskSysTickHandler()
{
	int i;
	for(i = 0; i < 2; i++)
	{
		if(taskTable[i]->delayTicks > 0) //Èç¹û½øÈëif,ËµÃ÷ÒªĞèÒªµİ¼õ
		{
			taskTable[i]->delayTicks--;
		}
	}
	
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

//2.4 ÊµÏÖÑÓÊ±º¯Êı, ÒòÎªÕâ¸öº¯Êı¿Ï¶¨ÊÇcurrentTaskµ÷ÓÃµÄ,ËùÒÔÀïÃæÖ±½ÓĞ´currentTask->xxx
void tTaskDelay(uint32_t delay)
{
	currentTask->delayTicks = delay; //Ò²¾ÍÊÇÎªcurrentTaskÉèÖÃĞèÒªÑÓÊ±¶à¾Ã
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




//ÉèÖÃtask1ÈÎÎñĞèÒªÖ´ĞĞµÄº¯Êı
void task1Entry(void * param) 
{
	
	tSetSysTickPeriod(1);
	
	for(;;){
		
		int var;
		//int i; //×¢Òâ,ÉùÃ÷Ò»¶¨¶¼Òª·ÅÔÚ¿ÉÖ´ĞĞÓï¾äÇ°Ãæ(ÉùÃ÷:int i; int var)(¿ÉÖ´ĞĞÓï¾ä: var = xxx;) ,·ñÔò»á±¨´í:error:  #268: declaration may not appear after executable statement in block
		
		tTaskSchedDisable();
		var = shareCount;
		//for(i = 0; i < 0xf; i++)
	
		task1Flag = 0;
		tTaskDelay(1);
		
		//ÒòÎªÉÏÒ»¾ä»á´¥·¢ÑÓÊ±,ËùÒÔ×ßµ½ÕâÒ»¾äÖ®Ç°,ÒÑ¾­È¥¹ıÒ»´Îtask2ÁË
		var++;
		shareCount = var;
		tTaskSchedEnable();
		
		task1Flag = 1;
		tTaskDelay(1);
		
		//tTaskSched();//ÊÇsystickµ÷¶ÈÁË,ÕâÀï¾Í²»ĞèÒªÁË
	}
}
void task2Entry(void * param) 
{
	for(;;){
		
		tTaskSchedDisable();
		shareCount++;
		tTaskSchedEnable();
		
		task2Flag = 0;
		tTaskDelay(1);
		task2Flag = 1;
		tTaskDelay(1);
		
		//tTaskSched();//ÊÇsystickµ÷¶ÈÁË,ÕâÀï¾Í²»ĞèÒªÁË
	}
}


//2.4 Ìí¼ÓÒ»¸ö¿ÕÏĞÈÎÎñ, ÀïÃæÊ²Ã´¶¼²»×ö, µÈ×ÅÆäËûÈÎÎñµÄ½áÊøÑÓ³Ù
void idleTaskEntry(void* param)
{
	for(;;)
	{}
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
	task->delayTicks = 0; //2.4, ÒòÎªtinyOS.hÖĞÍùtask½á¹¹ÌåÌí¼ÓÁË³ÉÔ±delayTicks, ËùÒÔÕâÀïÒ²ÒªĞ´ÉÏ
}


int main()
{
	
	//3.2 ÒòÎªshareCountÊÇÎÒÃÇ×Ô¼ºµÄ¶¨ÒåµÄµ÷¶ÈËø,ËùÒÔĞèÒª³õÊ¼»¯
	tTaskSchedInit();
	
	//½«tTask°ó¶¨ÉÏÏàÓ¦µÄstackºÍfunc
	taskInit(&tTask1, task1Entry, (void*)0x11111111, &tTask1Env[1024]); //×¢ÒâtTask1EnvÊÇÒ»¸öuint32_tµÄÊı×é,ËùÒÔ²»ÄÜĞ´³É&tTaskEnv,¶øÒªĞ´³É&tTaskEnv[xx]
	taskInit(&tTask2, task2Entry, (void*)0x00000000, &tTask2Env[1024]); 
	taskInit(&tTaskIdle, idleTaskEntry, (void*)0x00000001, &idleTaskEnv[1024]); //2.4, idleTaskµÄ³õÊ¼»¯
	
	//ÎªÎÒÃÇµÄtaskTableÉèÖÃ
	taskTable[0] = &tTask1;
	taskTable[1] = &tTask2;
	idleTask = &tTaskIdle; //2.4
	
	nextTask = taskTable[0]; //ÒòÎªÎÒÃÇÊÇ¸Õ¿ªÊ¼,»¹Ã»ÓĞÖ´ĞĞµÄÈÎÎñ,Ò²¾ÍÊÇcurrentTask»¹Ã»ÓĞ±»³õÊ¼»¯,ËùÒÔÕâÀïÓ¦¸ÃÊÇnextTask±»³õÊ¼»¯
	
	//2.2 ¿ªÊ¼Ö´ĞĞµÚÒ»¸öÈÎÎñ: ºÍÒ»°ãµÄÈÎÎñÇĞ»»²»Í¬µÄµØ·½(Ç°Õß:Ö±½Ó°ÑstackÀïÃæµÄ¶«Î÷´æµ½register,ÏëÏóÈ¥¹ñÌ¨µÄÊ±ºòÃ»ÓĞÈË,µÚÒ»¸ö¿ÍÈËÖ±½Ó°Ñ¶«Î÷·Å×ÀÃæ | ºóÕß:µ±Ç°µÄÈËÏÈ°ÑregisterµÄ¶«Î÷´æµ½stackÖĞ,ĞÂÀ´µÄÈÓ°Ñ×Ô¼ºstackÀïÃæµÄ¶«Î÷´æµ½register,ÏëÏóÈ¥¹ñÌ¨µÄÊ±ºòÓĞÈË,Ç°ÃæµÄ¿ÍÈË:¾üÆÌ¸Ç×ßÈË,ºóÃæµÄ¿ÍÈË:ÔÙ·Å¶«Î÷)
	tTaskRunFirst(); //Õâ¸öº¯Êı,Ò²Òª´¥·¢pendSVÒì³£, È»ºóÒòÎªpendSV»áÖ´ĞĞasm´úÂë,¿ÉÒÔ¿ØÖÆpc,ËùÒÔ¿ÉÒÔ¿ØÖÆÏÂÒ»¸öÒªÈ¥µÄº¯Êı
	//×¢Òâ: ÒòÎªtTaskSwitch();tTaskRunFirst(); ¶¼ÒªÓÃasm´úÂëÀ´¹ÜÀíÓ²¼ş,ËùÒÔÎÒÃÇ¾Í°ÑÕâÁ½¸öº¯ÊıµÄ¶¨Òå,·Åµ½switch.cÖĞ,ÒòÎªÕâ¸öcÎÄ¼şÖĞ´¦ÀíÓĞ¹ØÓ²¼şµÄ²¿·Ö.µ«ÊÇÕâ¸öÁ½¸öº¯ÊıµÄÉùÃ÷,ĞèÒª·Åµ½tinyOS.hÖĞ
	
	return 0; //×¢Òâ,ÕâÀï²»»áÖ´ĞĞµ½return 0,ÒòÎªtTaskRunFirst()ÀïÃæ¾Í»áÒ»Ö±×ßµ½for loop
}

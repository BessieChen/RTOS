//3.4 这个是配置文件
#ifndef TCONFIG_H
#define TCONFIG_H

#define TINYOS_PRIO_COUNT			32 //3.4 设置我们优先级的个数
#define TINYOS_SLICE_MAX			10 //老师写的是10 //3.7 如果是10, 时间片是10*10ms = 100ms, 因为systick是10ms启动一次, 但是我的电脑, 一个systick需要1.39s才会触发, 慢老师的100倍
/*
//有意思的故事, 假设TINYOS_SLICE_MAX设置为1, 如果task2和task3的时间片设置为1, 然后task1调用tTaskDelay(1), 那么每一次systick触发, 也是需要1. 所以每次systick触发: task1只会把flag翻转一次, 然后翻转之后, 就是立即的执行task2或者task3, 假设是task2执行, 因为task2的时间片只有1, 所以systick之后, task2时间片用完, 但是此时task1延迟也结束了, 所以又变成了task1执行
//结果: task1 翻转一次 -> task2 翻转混多次 -> task1再反转一次 -> task3翻转很多次
//3.7 如果我想获得和老师一样的效果, 需要将TINYOS_SLICE_MAX设置为10, 然后task1调用tTaskDelay(1), 然后task2和3需要delay(0xFF)
//3.7 记住, 上面的配置完成后, 需要等待差不多23s才能看到一样的效果, 看上去像是同时进行的, 但实际上放大很多倍,发现是还是一个cpu
//我建议你的调试方法: 
//1. 将task1Flag = 0, 1; task1Flag = 0, 1; task1Flag = 0, 1; 这6个地方设置断点
//2. 将systickHandler()设置断点
//你的发现: 一个systick会让task1从延时变成就绪, 这需要1.39s, 也就是1.39s一个systick. 但是在这个1.39s之间, delay(0xff)可能就只需要0.001s, 所以task2可以翻转1400多次

*/

#define TINYOS_IDELTASK_STACK_SIZE 1024
#endif

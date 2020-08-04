//3.4 ����������ļ�
#ifndef TCONFIG_H
#define TCONFIG_H

#define TINYOS_PRIO_COUNT			32 //3.4 �����������ȼ��ĸ���
#define TINYOS_SLICE_MAX			10 //��ʦд����10 //3.7 �����10, ʱ��Ƭ��10*10ms = 100ms, ��Ϊsystick��10ms����һ��, �����ҵĵ���, һ��systick��Ҫ1.39s�Żᴥ��, ����ʦ��100��
/*
//����˼�Ĺ���, ����TINYOS_SLICE_MAX����Ϊ1, ���task2��task3��ʱ��Ƭ����Ϊ1, Ȼ��task1����tTaskDelay(1), ��ôÿһ��systick����, Ҳ����Ҫ1. ����ÿ��systick����: task1ֻ���flag��תһ��, Ȼ��ת֮��, ����������ִ��task2����task3, ������task2ִ��, ��Ϊtask2��ʱ��Ƭֻ��1, ����systick֮��, task2ʱ��Ƭ����, ���Ǵ�ʱtask1�ӳ�Ҳ������, �����ֱ����task1ִ��
//���: task1 ��תһ�� -> task2 ��ת���� -> task1�ٷ�תһ�� -> task3��ת�ܶ��
//3.7 ��������ú���ʦһ����Ч��, ��Ҫ��TINYOS_SLICE_MAX����Ϊ10, Ȼ��task1����tTaskDelay(1), Ȼ��task2��3��Ҫdelay(0xFF)
//3.7 ��ס, �����������ɺ�, ��Ҫ�ȴ����23s���ܿ���һ����Ч��, ����ȥ����ͬʱ���е�, ��ʵ���ϷŴ�ܶ౶,�����ǻ���һ��cpu
//�ҽ�����ĵ��Է���: 
//1. ��task1Flag = 0, 1; task1Flag = 0, 1; task1Flag = 0, 1; ��6���ط����öϵ�
//2. ��systickHandler()���öϵ�
//��ķ���: һ��systick����task1����ʱ��ɾ���, ����Ҫ1.39s, Ҳ����1.39sһ��systick. ���������1.39s֮��, delay(0xff)���ܾ�ֻ��Ҫ0.001s, ����task2���Է�ת1400���

*/

#define TINYOS_IDELTASK_STACK_SIZE 1024
#endif

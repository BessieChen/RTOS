void delay(int count)
{
	while(count)
	{
		count--;
	}
}

int flag;
int main()
{
	for(;;)
	{
		flag = 1;
		delay(100);
		flag = 0;
		delay(100);
	}
}
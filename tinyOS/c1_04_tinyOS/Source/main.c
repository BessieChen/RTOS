static void delay(int count)
{
		while(--count>0);
}

static int flag;

int main(){
	for(;;)
	{
		flag = 0;
		delay(100);
		flag = 1;
		delay(100);
	}
}
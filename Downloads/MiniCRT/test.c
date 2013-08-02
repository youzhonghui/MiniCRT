#include "minicrt.h"

int main(int argc,char *argv[])
{
	int i;
	FILE* fp;

	fp	= fopen("test.txt","w");
	
	//
	{
	  char* buf = "hello how are you \n";
	  int len = strlen(buf);
	  fwrite(buf ,1,len,fp);
	  printf("%d  %s\n",len,buf);
	}
	
	fclose(fp);

	return 0; 
}


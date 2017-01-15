#include <stdio.h>
typedef struct bit
{
	unsigned char bit: 1;
}bit; 
unsigned char *hash(unsigned char *str,unsigned char *key)
{
        unsigned long hash = 5381;
        int c;
	int sel[]={ 21,18,27,30,4,24,14,7,0,10}; // unsgn long is 4bytes -> select 10bits as the hashed_key
        while (c = *str++)
            hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	bit b;
	for(c=0;c<10;c++)
	{
		b.bit = ( hash >> sel[c] ) & 0x01;
		if (c<8)
			key[0] |= ( b.bit << c );
		else
			key[1] |= ( b.bit << (c%8) );
	}
        return key;
}
unsigned char IP(unsigned char in) // Initial Permutation
{
	int init_perm[]={ 1,5,2,0,3,7,4,6 };
	unsigned char out;
	int i;
	bit b;
	for(i=0;i<8;i++) // LSB is bit 0
	{
		b.bit = (in >> init_perm[i]) & 0x01;
		out |= ( b.bit  << i );
		printf("%d", ( (in >> 7-i) & 0x01) ); // print reversed in
	}
	printf(" -> ");
	for(i=7;i>=0;i--)
	{
		printf("%d", ( (out >> i) & 0x01) ); // print reversed out
	}
	printf(" IP: %d -> %d ",in,out);
	return out;
}
unsigned char IP_1(unsigned char in)
{
	int rev_init_perm[]={ 3,0,2,4,6,1,7,5 }; 
	unsigned char out;
	int i;
	bit b;
	for(i=7;i>=0;i--)
	{
		b.bit = (in >> rev_init_perm[i]) & 0x01;
		out |= ( b.bit  << i );
		printf("%d", ((in >> 7-i) & 0x01)); // print reversed in
	}
	printf(" -> ");
	for(i=7;i>=0;i--)
	{
		printf("%d", ((out >> i) & 0x01));// print reversed out
	}
	printf(" IP-1: %d -> %d ",in,out);
	return out;
	
}
void P10(unsigned char *keyin,unsigned char *keyout)
{
	int p10[]={2,4,1,6,3,9,0,8,7,5};
	int i;
	bit b;
	for(i=0;i<10;i++)
	{
		if(p10[i]<8)
		{
			b.bit = (keyin[0] >> p10[i]) & 0x01;
		}
		else
		{
			b.bit = (keyin[1] >> (p10[i]%8) ) & 0x01;
		}
		if(i<8)
		{
			keyout[0] |= ( b.bit << i);
		}
		else
		{
			keyout[1] |= (b.bit << i%8);
		}
	}
	printf("Hashed key: ");
	printf("%d%d",( (keyin[1] >> 1 ) & 0x01),( (keyin[1] >> 0) & 0x01));
	for(i=0;i<8;i++) printf("%d",( (keyin[0] >> (7-i)) & 0x01));
	printf("\nP10key: ");
	printf("%d%d",( (keyout[1] >> 1 ) & 0x01),( (keyout[1] >> 0) & 0x01));
	for(i=0;i<8;i++) printf("%d",( (keyout[0] >> (7-i)) & 0x01));
	printf("\n");
}
int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		printf("Usage %s infile outfile \n",argv[0]);
		return -1;
	}
	FILE *infd=fopen(argv[1],"rb");
	if(infd == NULL)
	{
		printf("no such file: %s\n",argv[1]);
		return 1;
	}
	else
	{
		printf("file %s opened with fd: %x \n",argv[1],infd);
	}
	FILE *outfd=fopen(argv[2],"wb");
	if(outfd== NULL)
	{
		printf("Failed to create file: %s\n",argv[2]);
		return 2;	
	}
	unsigned char pwd[30],key[2]; // 30 char max password and 8-bit key (declaration)
	printf("Password: ");
	if( fgets(pwd,30,stdin) == NULL)
	{
		printf("Bad password given\n");
		return 3;
	}
	hash(pwd,key);
	unsigned char key10[2];
	P10(key,key10);
	int i,j;
	unsigned char buff;
	fseek(infd,0,SEEK_END);
	long size=ftell(infd);
	printf("size: %ld\n",size);
	fseek(infd,SEEK_SET,0);
	for(j=0;j<size;j++)
	{
	fread(&buff,1,1,infd);
	unsigned char iped=IP(buff);
	printf(" ");
	IP_1(iped);
	printf(" %c",buff);
	printf("\n");
	}
	fclose(infd);	
	return 0;
}

void fprint_bin(FILE *infd)
{
	int i,j;
	char buff;
	fseek(infd,0,SEEK_END);
	long size=ftell(infd);
	printf("size: %ld\n",size);
	fseek(infd,SEEK_SET,0);
	for(j=0;j<size;j++)
	{
	fread(&buff,1,1,infd);
	for(i=7;i>=0;i--)
	{
		printf("%d", (buff >> i) & 0x01);
	}
	if((j+1)%6 == 0) printf("\n");
	else printf(" ");
	}
	printf("\n");
	
}

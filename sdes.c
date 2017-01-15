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
	key[0]=0;key[1]=0;
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
	unsigned char out=0;
	int i;
	bit b;
	for(i=0;i<8;i++) // LSB is bit 0
	{
		b.bit = (in >> init_perm[i]) & 0x01;
		out |= ( b.bit  << i );
		//printf("%d", ( (in >> 7-i) & 0x01) ); // print reversed in
	}
	/*printf(" -> ");
	for(i=7;i>=0;i--)
	{
		printf("%d", ( (out >> i) & 0x01) ); // print reversed out
	}
	printf(" IP: %d -> %d ",in,out);
	printf("\n");*/
	return out;
}
unsigned char IP_1(unsigned char in)
{
	int rev_init_perm[]={ 3,0,2,4,6,1,7,5 }; 
	unsigned char out=0;
	int i;
	bit b;
	for(i=7;i>=0;i--)
	{
		b.bit = (in >> rev_init_perm[i]) & 0x01;
		out |= ( b.bit  << i );
		//printf("%d", ((in >> 7-i) & 0x01)); // print reversed in
	}
	/*printf(" -> ");
	for(i=7;i>=0;i--)
	{
		printf("%d", ((out >> i) & 0x01));// print reversed out
	}
	printf(" IP-1: %d -> %d ",in,out);
	printf("\n");*/
	return out;
	
}
void P10(unsigned char keyin[2],unsigned char keyout[2])
{
	int p10[]={2,4,1,6,3,9,0,8,7,5};
	int i;
	bit b,b1;
	unsigned char keyin0=keyin[0],keyin1=keyin[1];
	unsigned char keyout0=0,keyout1=0;
	printf("Hashed key: ");
	for(i=0;i<10;i++) 
	{
		if(p10[i]<8)    b1.bit = ( ( keyin0 >> p10[i]   ) & 0x01 );
                else            b1.bit = ( ( keyin1 >> (p10[i]%8) ) & 0x01);
		if(i<8) 
		{
			
			b.bit=( (keyin0 >> i) & 0x01);
			keyout0 |= ( b1.bit << i );
		}
		else 
		{
			
			b.bit=( (keyin1 >> (i%8) ) & 0x01);
			keyout1 |= ( b1.bit << (i%8) );
		}
		printf("%d",b.bit);//printf("%d -> %d\n\t",b.bit,b1.bit);
	}
	printf("\nP10:");
	b.bit = (keyout1 >> 0  ) & 0x01;
	b1.bit = (keyout1 >> 1) & 0x01;
	printf("%d%d",b1.bit,b.bit);
	for(i=7;i>=0;i--)
	{
		if(i==4) printf(" ");
		b.bit= ( (keyout0 >> i) & 0x01) ;
		printf("%d",b.bit);
	}
	printf("\n");
	keyout[0]=keyout0;
	keyout[1]=keyout1;
}
unsigned char *LS(unsigned char key10[2],unsigned char key_shifted[2])
{
	int i;
	bit b,msb;
	unsigned char key100=key10[0],key101=key10[1],key_shifted0=0,key_shifted1=0;
	msb.bit = ( (key100 >> 4) & 0x01);
	// 1st half from lsb
	for(i=4;i>=1;i--)
	{
		b.bit = ((key100 >> (i-1)) & 0x01);
		key_shifted0 |= ( b.bit << i );
	}
	key_shifted0 |= (msb.bit << 0) ;
	// 2nd half (to msb)
	bit b1;
	b.bit= ((key100 >> 7) & 0x01) ;
	msb.bit= ((key101 >> 1) & 0x01); 
	for(i=7;i>=6;i--)
	{
		b1.bit = ( (key100 >> (i-1) ) & 0x01) ;
		key_shifted0 |= ( b1.bit << i) ;
	}
	b1.bit=((key101 >> 0) & 0x01) ;
	key_shifted1 |= ( b1.bit << 1);
	key_shifted1 |= (b.bit << 0);
	key_shifted0 |= (msb.bit << 5);
	key_shifted[0]=key_shifted0;
	key_shifted[1]=key_shifted1;
	return key_shifted;
}
unsigned char P8(unsigned char key10[2],unsigned char *key8)
{
	int p8[]={5,2,6,3,7,4,9,8};
	int i;
	bit b;
	printf("P8: ");
	for(i=0;i<8;i++)
	{
		if(p8[i]<8)	b.bit=(key10[0] >> p8[i]) & 0x01;
		else		b.bit=(key10[1] >> (p8[i]%8) ) & 0x01;
		*key8 |= (b.bit << i);
	}
	for(i=0;i<8;i++) printf("%d",(*key8 >> (7-i) & 0x01 ));
	printf("\n");
	return *key8;	
}
int welcome(int argc,char *argv[],FILE *infd,FILE *outfd)
{

	if(argc > 4 || argc < 3 )
	{
		printf("Usage %s infile outfile \n",argv[0]);
		return 1;
	}
	if(infd == NULL)
	{
		printf("no such file: %s\n",argv[1]);
		return 1;
	}
	else
	{
		printf("file %s opened with fd: %x \n",argv[1],infd);
	}
	if(outfd == NULL)
	{
		fclose(infd);
		printf("Failed to create file: %s\n",argv[2]);
		return 1;	
	}
	return 0;
}
void produce_subkeys(unsigned char *pwd,unsigned char *subkey1,unsigned char *subkey2)
{
	unsigned char key[2],key10[2]; //10-bit main key produced by hashed pwd, 10-bit produced by Permutation10
	unsigned char key10s[2],key10ss[2];// 2 10-bit Left-Shifted keys

	hash(pwd,key); // Hash given password to produce 10-bit key
	P10(key,key10); // Permutation10 over 10-bit key produced by hashed password

	// Produce subkey 1
	LS(key10,key10s); // Left-Shift 10-bit key
	int i,j; 
	printf("P10 key shifted: %d%d",( (key10s[1] >> 1 ) & 0x01),( (key10s[1] >> 0) & 0x01)); // Print shifted key
        for(i=0;i<8;i++) 
	{
		if(i==3) printf(" ");
		printf("%d",( (key10s[0] >> (7-i)) & 0x01));
	}
	printf("\n");
	P8(key10s,&(*subkey1)); // Permutation8 over 10-bit left-shifted key to procude subkey1 

	// Produce subkey 2
	LS(key10s,key10ss); // Left-Shift 10-bit key once more
	printf("P10 key shifted: %d%d",( (key10ss[1] >> 1 ) & 0x01),( (key10ss[1] >> 0) & 0x01)); // Printf shifted key
        for(i=0;i<8;i++) 
	{
		if(i==3) printf(" ");
		printf("%d",( (key10ss[0] >> (7-i)) & 0x01));
	}
	printf("\n");
	P8(key10ss,&(*subkey2)); // Permutation8 over 10-bit double left-shifted key to produce subkey2
}
unsigned char F(unsigned char R,unsigned char skey)
{
	int i;
	int EP[]={3,0,1,2,1,2,3,0};// expansion of R
	unsigned char ep=0,xored=0;	
	for(i=0;i<8;i++)
	{
		ep |= ( (R >> EP[i]) & 0x01) << i;
	}
	xored= ep ^ skey;
	int sbox0[4][4]={ {1,0,3,2}, {3,2,1,0}, {0,2,1,3}, {3,1,3,2} };
	int sbox1[4][4]={ {0,1,2,3}, {2,0,1,3}, {3,0,1,0}, {2,1,0,3} };
	unsigned int row,col;
	bit row0,row1,col0,col1;
	unsigned char bits4=0;
	// S-Box 0
	// sbox0[row0row1][col0col1]
	row=0; // reset bits to 0
	//row0= (xored & (1 << 0)) >> 0;
	//row1= (xored >> 3) & 0x01;
	row |= ( (xored & (1 << 0)) >> 0) << 1 ;
	row |= ( (xored & (1 << 3)) >> 3) << 0 ;
	col=0;
	//col0 = (xored >> 1) & 0x01;
	//col1 = (xored >> 2) & 0x01;
	col |= ( (xored & (1 << 1)) >> 1) << 1 ;
	col |= ( (xored & (1 << 2)) >> 2) << 0 ;
//	bits4 |= ( sbox0[row,col] >> 2) & 0x02 ; //set first 2 bits of bits4;
	bits4 |= ( (sbox0[row][col] & (1 << 1)) >> 1) << 1;
	bits4 |= ( (sbox0[row][col] & (1 << 0)) >> 0) << 0;
	// S-Box 1
	row=0;
	//row0= (xored >> 4) & 0x01;
	//row1= (xored >> 7) & 0x01;
	row |= ( (xored & (1 << 4)) >> 4) << 1 ;
        row |= ( (xored & (1 << 7)) >> 7) << 0 ;
	col=0;
	//col0= (xored >> 5) & 0x01;
	//col1= (xored >> 6) & 0x01;
	col |= ( (xored & (1 << 5)) >> 5) << 1 ;
        col |= ( (xored & (1 << 6)) >> 6) << 0 ;
	bits4 |= (( sbox1[row][col] & (1 << 1)) >> 1) << 3 ; 
	bits4 |= (( sbox1[row][col] & (1 << 0)) >> 0) << 2 ;
	printf("bits4: %d\n",bits4);
	unsigned char out=0;
	int p4[]={2,4,3,1};
	for (i=0;i<4;i++)
	{
		out |= ( ( bits4  >> p4[i]) & 0x01 ) << i;
	}
	printf("out: %d\n",out);	
	return out;
}
void split(unsigned char buff,unsigned char *L,unsigned char *R)
{
	*L=0;*R=0; // reset bits
	int i;
	unsigned char l=0,r=0;
	for(i=0;i<4;i++)
	{
		l |= (( buff >> i) & 0x01) << i;
		r |= (( buff >> i+4) & 0x01) << i;
	}
	*L=l;*R=r;
	printf("L: %d R:%d\n",*L,*R);
}
unsigned char join(unsigned char left,unsigned char right)
{
	int i;
	unsigned char joined=0;
        for(i=0;i<4;i++)
        {
                joined |= (( left >> i) & 0x01) << i;
                joined |= (( right >> i) & 0x01) << (i+4);
        }
	return joined;
	printf("joined: %c\n",joined);

}
int main(int argc,char *argv[])
{
	
	FILE *infd=fopen(argv[1],"rb");
	FILE *outfd=fopen(argv[2],"wb");
	if( welcome(argc,argv,infd,outfd) ) return 1;
	unsigned char pwd[30]; // 30 char max password and 8-bit key (declaration)
	unsigned char subkey1=0,subkey2=0; // 10-bit key, 2 8-bit subkeys
	printf("Password: ");
	if( fgets(pwd,30,stdin) == NULL)
	{
		printf("Bad password given\n");
		return 3;
	}
	produce_subkeys(pwd,&subkey1,&subkey2);
	printf("skey1:%d skey2:%d\n",subkey1,subkey2);
	unsigned char buff;
	fseek(infd,0,SEEK_END);
	long size=ftell(infd);
	printf("size: %ld\n",size);
	fseek(infd,SEEK_SET,0);
	int j;
if(argc==4){
	unsigned char left=0,right=0,temp=0;
	unsigned char iped=IP(buff);
	fread(&buff,1,1,infd);
	split(iped,&left,&right);
	//printf("l: %d r: %d\n",left,right);
	for(j=0;j<size-1;j++)
	{		
		temp=F(right,subkey1) ^ left; // alter first 4-bits starting from LSB
		left=temp;	
		temp=F(left,subkey2) ^ right;
		right=temp;
		unsigned char joined=0;
		joined=join(left,right);
		unsigned char data=0;
		data=IP_1(joined);
//		printf("%c ",data);
		fwrite(&data,1,1,outfd);
		iped=0;	iped=IP(buff);
		fread(&buff,1,1,infd);
		split(iped,&left,&right);//printf("l: %d r: %d\n",left,right);
		
	}
	left=F(right,subkey1) ^ left; // alter first 4-bits starting from LSB	
	right=F(left,subkey2) ^ right;
	unsigned char joined=join(left,right);
	unsigned char data=IP_1(joined);
//	printf("%c",data);
	fwrite(&data,1,1,outfd);
	printf("\n");
}
else
{
	unsigned char left=0,right=0,temp=0;
        unsigned char iped=IP(buff);
        fread(&buff,1,1,infd);
        split(iped,&left,&right);
        //printf("l: %d r: %d\n",left,right);
        for(j=0;j<size-1;j++)
        {
                temp=F(right,subkey2) ^ left; // alter first 4-bits starting from LSB
                left=temp;
                temp=F(left,subkey1) ^ right;
                right=temp;
                unsigned char joined=0;
                joined=join(left,right);
                unsigned char data=0;
                data=IP_1(joined);
//              printf("%c ",data);
                fwrite(&data,1,1,outfd);
                iped=0; iped=IP(buff);
                fread(&buff,1,1,infd);
                split(iped,&left,&right);//printf("l: %d r: %d\n",left,right);

        }
        left=F(right,subkey2) ^ left; // alter first 4-bits starting from LSB   
        right=F(left,subkey1) ^ right;
        unsigned char joined=join(left,right);
        unsigned char data=IP_1(joined);
//      printf("%c",data);
        fwrite(&data,1,1,outfd);
        printf("\n");

	printf("bye!\n");
}
	fclose(infd);
	fclose(outfd);	
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

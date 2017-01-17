#include <stdio.h>

/* Defines a signle bit of type unsigned char */
typedef struct bit
{
	unsigned char bit: 1;
}bit; 
/*
*	Hash function that generates a 10-bit key by hashing a given string (the password) and selecting 10 out of 32 bits.
*	This is a modified version of the hash function 'djb2' found on: "http://www.cse.yorku.ca/~oz/hash.html".
*	@param str The password to be hashed.
* 	@param key The resulting 10-bit key after the hash over str.
*	@return the key as unsigned char key[2].
*/
unsigned char *hash(unsigned char *str,unsigned char *key)
{
        unsigned long hash = 5381;
        int c;
	int sel[]={ 21,18,27,30,4,24,14,7,0,10}; // unsigned long size is 4bytes -> select 10bits to create the SDES-key.
        while (c = *str++)
            hash = ((hash << 5) + hash) + c; // Hashing str ( hash * 33 + c )
	bit b;
	key[0]=0;key[1]=0; // setting all bits of the key variable to 0
	for(c=0;c<10;c++)  // selecting bit from the hash
	{
		b.bit = ( hash >> sel[c] ) & 0x01;
		if (c<8)
			key[0] |= ( b.bit << c );
		else
			key[1] |= ( b.bit << (c%8) );
	}
        return key;
}
/*
*	 Initial Permutation function as described on SDES standerd.
*	 Accepts a single unsigned char (8-bits) that represent a (SDES) block of data to be permutated.
*	 Returns an unsigned char of the permutated block.
*	 @param in The 8-bit block of data.
*	 @return The permutated block of data.
*/
unsigned char IP(unsigned char in)
{
	int init_perm[]={ 1,5,2,0,3,7,4,6 };
	unsigned char out=0;
	int i;
	bit b;
	for(i=0;i<8;i++) 				// LSB is bit 0. (Little Endian)
	{
		b.bit = (in >> init_perm[i]) & 0x01; 	// b.bit= the bit in position 'init_perm[i]' of the variable 'in'.
		out |= ( b.bit  << i );			// Set the bit in position 'i' of the varible 'out' as b.bit. 
	}
	return out;
}
/*
*	 Reverse Initial Permutation function as described on SDES standerd.
*	 Accepts a single unsigned char (8-bits) that represent a (SDES) block of data to be permutated.
*	 Returns an unsigned char of the reversed permutated block.
*	 @param in The 8-bit block of data.
*	 @return The reverse permutated block of data.
*/
unsigned char IP_1(unsigned char in)
{
	int rev_init_perm[]={ 3,0,2,4,6,1,7,5 }; 
	unsigned char out=0;
	int i;
	bit b;
	for(i=0;i<8;i++)
	{
		b.bit = (in >> rev_init_perm[i]) & 0x01;// b.bit= the bit in position 'rev_init_perm[i]' of the variable 'in'.
		out |= ( b.bit  << i );			// Set the bit in position 'i' of the varible 'out' as b.bit. 
	}
	return out;
	
}
/*
*	Permutaion 10 function as described on SDES standerd.
*	Accepts 10-bit key and permutates its bits.
*	@param keyin The 10-bit key as input.
*	@parm keyout The 10-bit permutated key. 
*/
void P10(unsigned char keyin[2],unsigned char keyout[2])
{
	int p10[]={2,4,1,6,3,9,0,8,7,5};
	int i;
	bit b,b1;
	keyout[0]=0,keyout[1]=0; // Reset bits
	printf("Hashed key: ");
	for(i=0;i<10;i++) 
	{
		if(p10[i]<8)    b1.bit = ( ( keyin[0] >> p10[i]   ) & 0x01 );
                else            b1.bit = ( ( keyin[1] >> (p10[i]%8) ) & 0x01);
		if(i<8) 
		{
			
			b.bit=( (keyin[0] >> i) & 0x01);
			keyout[0] |= ( b1.bit << i );
		}
		else 
		{
			
			b.bit=( (keyin[1] >> (i%8) ) & 0x01);
			keyout[1] |= ( b1.bit << (i%8) );
		}
		printf("%d",b.bit);
	}
	printf("\nP10: ");
	b.bit = (keyout[1] >> 0  ) & 0x01;
	b1.bit = (keyout[1] >> 1) & 0x01;
	printf("%d%d",b1.bit,b.bit);
	for(i=7;i>=0;i--)
	{
		if(i==4) printf(" ");
		b.bit= ( (keyout[0] >> i) & 0x01) ;
		printf("%d",b.bit);
	}
	printf("\n");
}
/*
*	Left Swift function.
*	Accepts the permutated over P10() key and returns the key shifted as defined at SDES standard.
*	More: Splits the input 10-bit key at two 5-bit parts and then applies bitwise LeftSwift on them.
*	@param key10 The already 'permutated10' imput 10-bit key.
*	@param key_shifted The output shifted key.
*/
void LS(unsigned char key10[2],unsigned char key_shifted[2])
{
	int i;
	bit b,msb;
	key_shifted[0]=0;key_shifted[1]=0; // Reset bits to 0
	msb.bit = ( (key10[0] >> 4) & 0x01);
	// 1st half (starting on lsb)
	for(i=4;i>=1;i--)
	{
		b.bit = ((key10[0] >> (i-1)) & 0x01);
		key_shifted[0] |= ( b.bit << i );
	}
	key_shifted[0] |= (msb.bit << 0) ;
	// 2nd half (to msb)
	bit b1;
	b.bit= ((key10[0] >> 7) & 0x01) ;
	msb.bit= ((key10[1] >> 1) & 0x01); 
	for(i=7;i>=6;i--)
	{
		b1.bit = ( (key10[0] >> (i-1) ) & 0x01) ;
		key_shifted[0] |= ( b1.bit << i) ;
	}
	b1.bit=((key10[1] >> 0) & 0x01) ;
	key_shifted[1] |= ( b1.bit << 1);
	key_shifted[1] |= (b.bit << 0);
	key_shifted[0] |= (msb.bit << 5);
}
/*
*	Permutation 8 function as defined by SDES standard.
*	Accepts 10-bit key and produces an 8-bit key over selected 8 out of 10 bits.
*	@param key10 The 10-bit input key.
*	@param key8 The 8-bit output key.
*/
void P8(unsigned char key10[2],unsigned char *key8)
{
	int p8[]={5,2,6,3,7,4,9,8};
	int i;
	bit b;
	printf("P8: ");
	*key8=0;
	for(i=0;i<8;i++)
	{
		if(p8[i]<8)	b.bit=(key10[0] >> p8[i]) & 0x01;
		else		b.bit=(key10[1] >> (p8[i]%8) ) & 0x01;
		*key8 |= (b.bit << i);
	}
	for(i=0;i<8;i++) printf("%d",(*key8 >> (7-i) & 0x01 ));
	printf("\n");
}
/*
*	This function produces the two subkeys needed for SDES rounds.
*	Accepts the a string (the password) and produces the 2 subkeys that depend on it.
*	@param pwd The string password.
*	@param subkey1 The 1st subkey produced.
*	@param subkey2 The 2nd subkey produced.
*	@see hash
*	@see P10
*	@see P8
*	@see LS
*/
void produce_subkeys(unsigned char *pwd,unsigned char *subkey1,unsigned char *subkey2)
{
	unsigned char key[2],key10[2]; 		//10-bit main key produced by hashed pwd, 10-bit produced by Permutation10
	unsigned char key10s[2],key10ss[2];	// 2 10-bit Left-Shifted keys

	hash(pwd,key); 				// Hash given password to produce 10-bit key
	P10(key,key10); 			// Permutation10 over 10-bit key produced by hashed password

	// Produce subkey 1
	LS(key10,key10s); 			// Left-Shift 10-bit key
	int i,j; 
	printf("P10 key shifted: %d%d",( (key10s[1] >> 1 ) & 0x01),( (key10s[1] >> 0) & 0x01)); // Print shifted key
        for(i=0;i<8;i++) 
	{
		if(i==3) printf(" ");
		printf("%d",( (key10s[0] >> (7-i)) & 0x01));
	}
	printf("\n");
	P8(key10s,&(*subkey1)); 		// Permutation8 over 10-bit left-shifted key to procude subkey1 

	// Produce subkey 2
	LS(key10s,key10ss); 			// Left-Shift 10-bit key once more
	printf("P10 key shifted: %d%d",( (key10ss[1] >> 1 ) & 0x01),( (key10ss[1] >> 0) & 0x01)); // Printf shifted key
        for(i=0;i<8;i++) 
	{
		if(i==3) printf(" ");
		printf("%d",( (key10ss[0] >> (7-i)) & 0x01));
	}
	printf("\n");
	P8(key10ss,&(*subkey2)); 		// Permutation8 over 10-bit double left-shifted key to produce subkey2
}
/*
*	The F mapping as described on SDES standard.
*	Accepts the half of the current block (4 bits) ,the sub-key (8 bits) of the current round as unsigned chars and returns the mapped block.
*	Uses: 
*		Expansion Permutation:  Turns 4-bit to 8-bit block.
*		S-Boxes		     :	Produce a 4-bit block using the xor of (Expansion_Permutation , Subkey).
*		Permutation 4 	     :  Permutates the 4 bits produced by the use of S-Boxes.
*
*	@param R The half of the block.
*	@param skey The round's subkey.
*	@return The mapped block.
*/
unsigned char F(unsigned char R,unsigned char skey)
{
	int i;
	int EP[]={3,0,1,2,1,2,3,0};	// expansion of R
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
	row |= ( (xored & (1 << 0)) >> 0) << 1 ;
	row |= ( (xored & (1 << 3)) >> 3) << 0 ;
	col=0;
	col |= ( (xored & (1 << 1)) >> 1) << 1 ;
	col |= ( (xored & (1 << 2)) >> 2) << 0 ;
	bits4 |= ( (sbox0[row][col] & (1 << 1)) >> 1) << 1;
	bits4 |= ( (sbox0[row][col] & (1 << 0)) >> 0) << 0;
	// S-Box 1
	row=0;
	row |= ( (xored & (1 << 4)) >> 4) << 1 ;
        row |= ( (xored & (1 << 7)) >> 7) << 0 ;
	col=0;
	col |= ( (xored & (1 << 5)) >> 5) << 1 ;
        col |= ( (xored & (1 << 6)) >> 6) << 0 ;
	bits4 |= (( sbox1[row][col] & (1 << 1)) >> 1) << 3 ; 
	bits4 |= (( sbox1[row][col] & (1 << 0)) >> 0) << 2 ;
	unsigned char out=0;
	int p4[]={2,4,3,1};
	for (i=0;i<4;i++)
	{
		out |= ( ( bits4  >> p4[i]) & 0x01 ) << i;
	}
	return out;
}
/*
*	Split function that splits an 8-bit block to left and right blocks of 4-bits.
*	@param buff The 8-bit block.
*	@param L The left 4-bit block.
*	@param R The right 4-bit block.
*/
void split(unsigned char buff,unsigned char *L,unsigned char *R)
{
	*L=0;*R=0; // reset bits
	int i;
	unsigned char l=0,r=0;
	for(i=0;i<4;i++)
	{
		*R |= (( buff >> i) & 0x01) << i;
		*L |= (( buff >> i+4) & 0x01) << i;
	}
}
/*
*	Join function, used for joining two 4-bit blocks to one 8-bit block.
*	Left block contains LSB and Right block MSB.
*	@param left The left block.
*	@param right The right block.
*	@return joined The 8-bit block (r+l).
*/
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

}
/*
	Simple welcome function.
	Parses the user input arguments of the program and returns Error/Warning messages at STDOUT.
	@param argc The number of program's inputs given by the user.
	@param argv The inputs given by the user.
	@param infd The file descriptor of the input file.
	@param outfd The file descriptor of the output file.
	@return If user's parameters are ok returns 0 else 1.
*/
int welcome(int argc,char *argv[],FILE *infd,FILE *outfd)
{

	if(argc !=4 )
	{
		printf("Usage %s infile X outfile \nX:\n\t-e:\t for encrypt\n\t-d:\t for decrypt\n\n",argv[0]);
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
		printf("Failed to create file: %s\n",argv[3]);
		return 1;	
	}
	if(strncmp(argv[2],"-e",2))
	{
		if(strncmp(argv[2],"-d",2))
		{
			printf("Invalid option %s!\n",argv[2]);
			return 1;
		}
	}
	return 0;
}
/*
*	Encryption function
*	@param infd	Descriptor to the input file to encrypt.
*	@param outfd	Descriptor to the output encrypted file.
*	@param size	The size of input file in bytes.
*	@param subkey1	The subkey for round 1 of block enryption.
*	@param subkey2	The subkey for round 2 of block encryption.
*/
void encrypt(FILE *infd,FILE *outfd,long size,unsigned char subkey1,unsigned char subkey2)
{
	unsigned char left=0,right=0,temp=0,iped=0,buff;
	int j;
	fread(&buff,1,1,infd);
	iped=IP(buff);
	split(iped,&left,&right);
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
		fwrite(&data,1,1,outfd);
		fread(&buff,1,1,infd);
		iped=0;	iped=IP(buff);
		split(iped,&left,&right);
	}
	temp=F(right,subkey1) ^ left; // alter first 4-bits starting from LSB	
	left=temp;
	temp=F(left,subkey2) ^ right;
	right=temp;
	unsigned char joined=0;
	joined=join(left,right);
	unsigned char data=0;
	data=IP_1(joined);
	fwrite(&data,1,1,outfd);
	printf("\n");
}
/*
*       Decryption function
*       @param infd     Descriptor to the input encrypted file.
*       @param outfd    Descriptor to the output  decrypted file.
*       @param size     The size of input file in bytes.
*       @param subkey1  The subkey for round 1 of block decryption.
*       @param subkey2  The subkey for round 2 of block decryption.
*/
void decrypt(FILE *infd,FILE *outfd,long size,unsigned char subkey1,unsigned char subkey2)
{
	unsigned char left=0,right=0,temp=0,iped=0,buff;
	int j;
        fread(&buff,1,1,infd);
	iped=IP(buff);
        split(iped,&left,&right);
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
                fwrite(&data,1,1,outfd);
                fread(&buff,1,1,infd);
                iped=0; iped=IP(buff);
                split(iped,&left,&right);

        }
        temp=F(right,subkey2) ^ left; // alter first 4-bits starting from LSB   
	left=temp;
        temp=F(left,subkey1) ^ right;
	right=temp;
        unsigned char joined=0;
	joined=join(left,right);
        unsigned char data=0;
	data=IP_1(joined);
        fwrite(&data,1,1,outfd);
        printf("\n");
}
/*
	Main function
*/
int main(int argc,char *argv[])
{
	
	FILE *infd=fopen(argv[1],"rb");
	FILE *outfd=fopen(argv[3],"wb");
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
	printf("skey1: %d skey2: %d\n",subkey1,subkey2);
	fseek(infd,0,SEEK_END);
	long size=ftell(infd);
	printf("size: %ld\n",size);
	fseek(infd,SEEK_SET,0);
	int j;
	if(strncmp(argv[2],"-e",2))
	{
		encrypt(infd,outfd,size,subkey1,subkey2);
	}
	else
	{
		if(strncmp(argv[2],"-d",2))
		{
			decrypt(infd,outfd,size,subkey1,subkey2);	
		}
		else
		{
			printf("Nothing to do..\nExiting..\n");
		}
	}
	fclose(infd);
	fclose(outfd);	
	return 0;
}

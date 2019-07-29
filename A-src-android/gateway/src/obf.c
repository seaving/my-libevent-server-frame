/*
 * Simple string obfuscation - Quequero 2013
 *
 * Obfuscated string format:
 * 1-byte key. 1-byte module. 1-byte length, <string>
 */

#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

// <search string> <replace string> <string to search>
unsigned char *str_replace(unsigned char *search , unsigned char *replace , unsigned char *subject) {
  unsigned char *p = NULL , *old = NULL , *new_subject = NULL ;
  int c = 0 , search_size;

  search_size = strlen((char *) search);

  for (p = (unsigned char *) strstr((char *) subject , (char *) search) ; 
  		p != NULL ; p = (unsigned char *) strstr((char *) (p + search_size), (char *) search)) {
    c++;
  }

  c = (strlen((char *) replace) - search_size )*c + strlen((char *) subject);
  new_subject = malloc(c);
  strcpy((char *) new_subject , "");
  old = subject;

  for (p = (unsigned char *) strstr((char *) subject , (char *) search) ; 
  		p != NULL ; p = (unsigned char *) strstr((char *) (p + search_size), (char *) search)) {
    strncpy((char *) (new_subject + strlen((char *) new_subject)) , (char *) old , p - old);
    strcpy((char *) (new_subject + strlen((char *) new_subject)), (char *) replace);
    old = p + search_size;
  }

  strcpy((char *) (new_subject + strlen((char *) new_subject)), (char *) old);
  return new_subject;
}

int obfuscate(unsigned char *s, unsigned char *d) {
  unsigned int seed;
  unsigned char key, mod;
  unsigned char *rep = NULL;
  unsigned char nl[] = "\n";
  //unsigned char tb[] = "\t";
  //unsigned char cr[] = "\r";
  int i, j, len;

  srandom(time(0));
  seed = random();

  key = (unsigned char)(seed & 0x000000ff);
  mod = (unsigned char)((seed & 0x0000ff00) >> 8);

  char *replace = "\\n";
  rep = str_replace((unsigned char *) replace, nl, s);
  
  /*rep = str_replace("\\t", tb, rep);
  
  rep = str_replace("\\r", cr, rep);
  printf("LENGTH rep3: %d\n", strlen(rep));*/

  len = strlen((char *) rep);

  /*if (len > 1024) {
    printf("String too long\n");
    return -1;
  }*/
  //printf(" obf len :%d ,%04x\n",len ,len);

  d[0] = key;
  d[1] = mod;
  d[2] = ( ((uint16_t)len & 0xff) ^ key ^ mod);
  d[3] = ( (((uint16_t)len >> 8) & 0xff) ^ key ^ mod);

  for (i = 0, j = 4; i < len; i++, j++) {
    d[j] = rep[i] ^ key;
    d[j] += mod;
    d[j] ^= mod;
  }

  return len;
}

char* deobfuscate(unsigned char *s) {
  unsigned char key, mod;
  int i, j ,len;
  static char d[4069]; // E' zozza ma cosi' non serve la free()

  key = s[0];
  mod = s[1];
  len = ( s[2] ^ key ^ mod ) + (( s[3] ^ key ^ mod ) << 8);

  //printf(" len :%d\n",len);
  // zero terminate the string
  memset(d, 0x00, len + 1);

  for (i = 0, j = 4; i < len; i++, j++) {
    d[i] = s[j] ^ mod;
    d[i] -= mod;
    d[i] ^= key;
  }

  d[len] = 0;
  return d;
}

#if 0
static void append(char* buf,char* sb)
{
	memcpy(sb + strlen(sb),buf,strlen(buf));
}

int main(int argc, char *argv[]) {
	unsigned char buf[2048], *test ,ebuf[2048] ,*pbuf;
	int i, obf_len;
	FILE* fp = NULL ,*fout = NULL;

	if (argc < 2) {
		printf("Usage: %s <file>\n", argv[0]);
		return 0;
	}
	/*
	
	fp = fopen(argv[1],"r+");
	if(fp == NULL){
		printf("open %s failed\n",argv[1]);
		return -1;
	}

	while(fgets(buf,1024,fp) != NULL){
		char* p = buf;
		while(*p != 0x0d) p++;
		*p = '\0';
		
		//printf("===> \"%s\"\n",buf);
		
		obf_len = strlen(buf) + 3;

		//ebuf = (unsigned char *)malloc(obf_len);
		memset(ebuf, 0x00, obf_len);

		obf_len = obfuscate(buf, ebuf);
		obf_len += 3;

		printf("\tunsigned char obf_string[] = \"");
		
		for (i = 0; i < obf_len; i++) {
			printf("\\x");
			printf("%02x", ebuf[i]);
		}

		printf("\"; // \"%s\"\n", buf);
		
		//test = deobfuscate(ebuf);

		//printf("\tDeobfuscated string: \"%s\"\n", test);
		
		//free(ebuf);
	}
	fclose(fp);
	*/
	
	obf_len = strlen(argv[1]) + 3;

	pbuf = (unsigned char *)malloc(obf_len);
	memset(pbuf, 0x00, obf_len);

	obf_len = obfuscate(argv[1], pbuf);
	obf_len += 3;

	printf("\tunsigned char obf_string[] = \"");
	
	for (i = 0; i < obf_len; i++) {
		printf("\\x");
		printf("%02x", pbuf[i]);
	}

	printf("\"; // \"%s\"\n", argv[1]);
	
	//unsigned char* p = argv[1];
	//unsigned char obf_string[] = "\x35\x56\x6d\xc1\xe5\xe4\xca\x3d\xe4\xca\x3d\xfc\x3d\xc1\xfc\xca\xe2";
	//uint16_t obf_string[] = "\x64bf\x6c02\x8b1\xbccf\xbcdb\xbcda\xbccc\xbca3\xbcda\xbccc\xbca3\xbccf\xbcde\xbccc\xbc07"; // "this is test"
	
	test = deobfuscate(pbuf);

	printf("\tDeobfuscated string: \"%s\"\n", test);
	
	free(pbuf); 
	//test = deobfuscate(buf);

	//printf("Deobfuscated string: \"%s\"\n", test);

	//free(buf);
	return 0;
}
#endif


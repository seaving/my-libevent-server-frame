#ifndef __BASE64_H__
#define __BASE64_H__


bool base64encode(const char *data, const long datalen, 
		char *base64Buf, const long bufsize);

bool base64decode(const char *base64Char, 
		const long base64CharSize, 
		char *originChar, long originCharSize);


#endif



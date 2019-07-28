#include "sys_inc.h"
#include "log_trace.h"
#include "base64.h"

static const char *ALPHA_BASE = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

bool base64encode(const char *data, const long datalen, 
		char *base64Buf, const long bufsize)
{
    int a = 0;
    int i = 0;
    while (i < datalen)
    {
    	char b0 = data[i++];
		char b1 = (i < datalen) ? data[i++] : 0;
		char b2 = (i < datalen) ? data[i++] : 0;

		int int63 = 0x3F; //  00111111
		int int255 = 0xFF; // 11111111
		if (bufsize - a <= 4)
		{
			return false;
		}

		base64Buf[a++] = ALPHA_BASE[(b0 >> 2) & int63];
		base64Buf[a++] = ALPHA_BASE[((b0 << 4) | ((b1 & int255) >> 4)) & int63];
		base64Buf[a++] = ALPHA_BASE[((b1 << 2) | ((b2 & int255) >> 6)) & int63];
		base64Buf[a++] = ALPHA_BASE[b2 & int63];
	}
	switch (datalen % 3)
	{
		case 1:
			base64Buf[--a] = '=';
		case 2:
			base64Buf[--a] = '=';
		default:
			break;
    }
	return true;
}

bool base64decode(const char *base64Char, 
		const long base64CharSize, 
		char *originChar, long originCharSize)
{
	int i = 0;
	int toInt[128] = {-1};
	for (i = 0; i < 64; i++)
	{
		toInt[(unsigned char) ALPHA_BASE[i]] = i;
	}
	int int255 = 0xFF;
	int index = 0;

    for (i = 0; i < base64CharSize; i += 4)
    {
		int c0 = toInt[(unsigned char) base64Char[i]];
		int c1 = toInt[(unsigned char) base64Char[i + 1]];
		originChar[index++] = (((c0 << 2) | (c1 >> 4)) & int255);
		if (index >= originCharSize)
		{
			return originChar;
		}
		int c2 = toInt[(unsigned char) base64Char[i + 2]];
		originChar[index++] = (((c1 << 4) | (c2 >> 2)) & int255);
		if (index >= originCharSize)
		{
			return originChar;
		}
		int c3 = toInt[(unsigned char) base64Char[i + 3]];
		originChar[index++] = (((c2 << 6) | c3) & int255);
    }
	return true;
}



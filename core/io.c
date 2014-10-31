#include <core/io.h>

void kprint_int(u32 n)
{
	if (n == 0) {
		kprint_char('0');
		return;
	}

	s32 acc = n;
	char c[32];
	int i = 0;
	while (acc > 0) {
		c[i] = '0' + acc%10;
		acc /= 10;
		i++;
	}
	c[i] = 0;

	char c2[32];
	c2[i--] = 0;

	int j = 0;
	while(i >= 0) {
		c2[i--] = c[j++];
	}

	kprint(c2);
}

void kprint_hex(u32 n)
{
	s32 tmp;

	kprint("0x");

	char noZeroes = 1;

	int i;
	for (i = 28; i > 0; i -= 4)
	{
		tmp = (n >> i) & 0xF;
		if (tmp == 0 && noZeroes != 0)
			continue;

		if (tmp >= 0xA)  {
			noZeroes = 0;
			kprint_char(tmp - 0xA + 'A' );
		} else {
			noZeroes = 0;
			kprint_char(tmp + '0');
		}
	}

	tmp = n & 0xF;
	if (tmp >= 0xA) {
		kprint_char(tmp - 0xA + 'A');
	} else {
		kprint_char(tmp + '0');
	}
}


void kprint_intnl(u32 val)
{
	kprint_int(val);
	kprint("\n");
}


void kprint_hexnl(u32 val)
{
	kprint_hex(val);
	kprint("\n");
}

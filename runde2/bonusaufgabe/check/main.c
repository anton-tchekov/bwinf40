#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

typedef __uint128_t uint128_t;

static char *_print_key(uint128_t key)
{
	static char s[129];
	int i;
	for(i = 0; i < 128; ++i)
	{
		s[127 - i] = ((key >> i) & 1) + '0';
	}

	s[128] = '\0';
	return s;
}


int main(int argc, char **argv)
{
	int i, j;
	uint128_t keys[111] = { 0 };
	char buf[256];
	FILE *fp;
	if(argc != 13)
	{
		fprintf(stderr, "Usage: ./check file a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11\n");
		return 1;
	}

	/* Load file */
	if(!(fp = fopen(argv[1], "r")))
	{
		fprintf(stderr, "Error opening file \"%s\"\n", argv[1]);
		return 1;
	}

	i = 0;
	while(fgets(buf, sizeof(buf), fp))
	{
		for(j = 0; j < 128; ++j)
		{
			if(buf[j] == '1')
			{
				keys[i] |= ((uint128_t)1UL << (uint128_t)(127 - j));
			}
			else if(buf[j] != '0')
			{
				fprintf(stderr, "Invalid binary digit in line %d\n", i + 1);
				fclose(fp);
				return 1;
			}
		}

		++i;
	}

	if(i != 111)
	{
		fprintf(stderr, "Incomplete file\n");
		fclose(fp);
		return 1;
	}

	fclose(fp);

	int idxs[11];

	for(i = 2; i < 13; ++i)
	{
		idxs[i - 2] = atoi(argv[i]);
	}

	uint128_t xor = 0;
	for(i = 0; i < 11; ++i)
	{
		printf("%d\n", idxs[i]);
		xor ^= keys[idxs[i]];
	}
	printf("%s\n", _print_key(xor));

	return 0;
}



#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef __uint128_t uint128_t;

static uint128_t _random_key(void)
{
	static int first = 1;
	uint128_t key;
	int i;
	if(first)
	{
		first = 0;
		srand(time(0));
	}

	key = 0;
	for(i = 0; i < 128; i += 8)
	{
		key |= (uint128_t)(rand() % 256) << (uint128_t)i;
	}

	return key;
}

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

static void _keys_swap(uint128_t *a, uint128_t *b)
{
	uint128_t temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

static void _keys_shuffle(uint128_t *keys, int count)
{
	int i, j;
	for(i = 0; i < count - 1; ++i)
	{
		j = i + rand() / (RAND_MAX / (count - i) + 1);
		_keys_swap(keys + i, keys + j);
	}
}

int main(void)
{
	uint128_t keys[111];
	int i;
	for(i = 0; i < 110; ++i)
	{
		keys[i] = _random_key();
	}

	keys[110] = _xor_keys(keys, 10);

	_keys_shuffle(keys, 111);

	for(i = 0; i < 111; ++i)
	{
		printf("%s\n", _print_key(keys[i]));
	}

	return 0;
}

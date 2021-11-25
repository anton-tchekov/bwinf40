#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEBUG

#define INVALID 1000000

typedef struct INPUT
{
	int Start, End, SE, Count, *Cars, *Across;
} Input;

static int input_get(Input *in, const char *filename)
{
	int i;
	FILE *fp;
	char *p, buf[256];

	/* Open File */
	if(!(fp = fopen(filename, "r")))
	{
		return 1;
	}

	/* Get Start and End */
	if(!fgets(buf, sizeof(buf), fp))
	{
		return 1;
	}

	p = strtok(buf, " ");
	if(p == NULL)
	{
		return 1;
	}

	in->Start = *p;
	p = strtok(NULL, " ");
	if(p == NULL)
	{
		return 1;
	}

	in->End = *p;

	in->SE = in->End - in->Start + 1;

	/* Get Count */
	if(!fgets(buf, sizeof(buf), fp))
	{
		return 1;
	}

	in->Count = atoi(buf);

	/* Allocate Memory for Across */
	if(!(in->Across = malloc(in->SE * sizeof(int))))
	{
		return 1;
	}

	/* Allocate Memory for Cars */
	if(!(in->Cars = malloc(in->Count * sizeof(int))))
	{
		return 1;
	}

	for(i = 0; i < in->SE; ++i)
	{
		in->Across[i] = ' ';
	}

	/* Get Across Cars */
	for(i = 0; i < in->Count; ++i)
	{
		char c;
		int pos;
		if(!fgets(buf, sizeof(buf), fp))
		{
			return 1;
		}

		p = strtok(buf, " ");
		if(p == NULL)
		{
			return 1;
		}

		c = *p;
		p = strtok(NULL, " ");
		if(p == NULL)
		{
			return 1;
		}

		pos = atoi(p);
		in->Across[pos] = c;
		in->Across[pos + 1] = c;

		in->Cars[i] = pos;
	}

	fclose(fp);
	return 0;
}

#ifdef DEBUG

static void input_line(Input *in)
{
	int i;
	for(i = 0; i < in->SE; ++i)
	{
		printf("+---");
	}

	printf("+\n");
}

static void input_test(Input *in)
{
	int i;

	printf("Schiebeparkplatz:\n");

	input_line(in);

	for(i = 0; i < in->SE; ++i)
	{
		printf("| %c ", in->Start + i);
	}

	printf("|\n");

	input_line(in);

	for(i = 0; i < in->SE; ++i)
	{
		printf("| %c ", in->Start + i);
	}

	printf("|\n");

	input_line(in);

	for(i = 0; i < in->SE; ++i)
	{
		printf("| %c ", in->Across[i]);
	}

	printf("|\n");

	input_line(in);

	printf("\n");
}

#endif

static void input_destroy(Input *in)
{
	free(in->Across);
	free(in->Cars);
}

static int test_left(Input *in, int i, int *out, int *across, int *shift)
{
	int j, cnt, sft = 0;
	for(cnt = 0, j = in->Across[i] - in->End - 1; ; --j, ++cnt)
	{
		if(across[j] == i)
		{
			out[cnt] = (in->End + j + 1) << 8 | 2;
			across[j] -= 2;
			sft += 2;
		}
		else
		{
			out[cnt] = (in->End + j + 1) << 8 | 1;
			--across[j];
			++sft;
		}

		/* Check Border */
		if(across[j] < 0)
		{
			return INVALID;
		}

		/* Last Sideways Car */
		if(j == 0)
		{
			break;
		}

		/* Check Overlap */
		if(!((across[j] == (across[j - 1] + 1)) || (across[j] == across[j - 1])))
		{
			break;
		}

		i = across[j];
	}

	*shift = sft;
	return cnt + 1;
}

static int test_right(Input *in, int i, int *out, int *across, int *shift)
{
	int j, cnt, sft = 0;
	for(cnt = 0, j = in->Across[i] - in->End - 1; ; ++j, ++cnt)
	{
		if(across[j] == i)
		{
			out[cnt] = (in->End + j + 1) << 8 | 1;
			++across[j];
			++sft;
		}
		else
		{
			out[cnt] = (in->End + j + 1) << 8 | 2;
			across[j] += 2;
			sft += 2;
		}

		/* Check Border */
		if(across[j] > (in->End - in->Start - 1))
		{
			return INVALID;
		}

		/* Last Sideways Car */
		if(j + 1 >= in->Count)
		{
			break;
		}

		/* Check Overlap */
		if(!(across[j] == across[j + 1] || (across[j] + 1) == across[j + 1]))
		{
			break;
		}

		i = across[j] + 1;
	}

	*shift = sft;
	return cnt + 1;
}

int main(int argc, char **argv)
{
	static int rout[128], lout[128];
	int i, j, r, l, sl, sr, across[128];
	Input in;

	if(argc != 2)
	{
		fprintf(stderr, "Usage: ./a1-schiebeparkplatz input-file\n");
		return 1;
	}

	if(input_get(&in, argv[1]))
	{
		fprintf(stderr, "Failed to read input file \"%s\"\n", argv[1]);
		return 1;
	}

#ifdef DEBUG

	input_test(&in);

#endif

	for(i = 0; i < in.SE; ++i)
	{
		printf("%c: ", in.Start + i);

		if(in.Across[i] != ' ')
		{
			memcpy(across, in.Cars, in.Count * sizeof(int));
			l = test_left(&in, i, lout, across, &sl);

			memcpy(across, in.Cars, in.Count * sizeof(int));
			r = test_right(&in, i, rout, across, &sr);

			if(r == INVALID && l == INVALID)
			{
				printf("Impossible");
			}
			else
			{
				int show = 0, cnt, *out;
				char *str;

				if(r > l)
				{
					show = 1;
				}
				else if(r < l)
				{
					show = 2;
				}
				else /* r == l */
				{
					if(sr > sl)
					{
						show = 1;
					}
					else if(sr < sl)
					{
						show = 2;
					}
					else
					{
						show = 1;
					}
				}

				if(show == 1)
				{
					cnt = l;
					out = lout;
					str = "links";
				}
				else if(show == 2)
				{
					cnt = r;
					out = rout;
					str = "rechts";
				}

				for(j = cnt - 1; j >= 0; --j)
				{
					printf("%c %d %s", out[j] >> 8, out[j] & 0xff, str);
					if(j > 0)
					{
						printf(", ");
					}
				}
			}
		}

		printf("\n");
	}

	printf("\n");
	input_destroy(&in);
	return 0;
}


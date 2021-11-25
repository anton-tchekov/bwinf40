#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define MAX_WEIGHT  10000
#define MAX_STORE   (2 * MAX_WEIGHT)
#define MAX_TOTAL   64
#define MAX_TYPE    32

static uint8_t count_min[MAX_STORE];
static uint8_t set[MAX_STORE];
static int output[MAX_STORE][MAX_TOTAL];

typedef struct INPUT
{
	int Count, *Weights, *Counts;
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

	/* Get Number of Weights */
	fgets(buf, sizeof(buf), fp);
	in->Count = atoi(buf);

	/* Allocate Memory for Weights and Count */
	if(!(in->Weights = malloc(2 * in->Count * sizeof(int))))
	{
		return 1;
	}

	in->Counts = in->Weights + in->Count;

	/* Get Weights and Count */
	for(i = 0; i < in->Count; ++i)
	{
		fgets(buf, sizeof(buf), fp);
		in->Weights[i] = strtol(buf, &p, 10);
		in->Counts[i] = strtol(p, &p, 10);
	}

	fclose(fp);
	return 0;
}

static void input_destroy(Input *in)
{
	free(in->Weights);
}

#ifdef DEBUG

static void input_test(Input *in)
{
	int i;
	printf("Count = %d\n", in->Count);
	for(i = 0; i < in->Count; ++i)
	{
		printf("%3dx %5dg\n", in->Counts[i], in->Weights[i]);
	}
}

#endif

static void balance(int w[][MAX_TYPE], int *count, int n, int k, int sum, int *a)
{
	int i;
	if(sum >= 1 && sum < MAX_STORE)
	{
		int ms = 0;
		set[sum] = 1;
		for(i = 0; i < n; ++i)
		{
			ms += a[i];
		}

		if(ms < count_min[sum])
		{
			count_min[sum] = ms;
			for(i = 0; i < n; ++i)
			{
				output[sum][i] = a[i];
			}
		}
	}

	if(k == n)
	{
		return;
	}

	for(i = 0; i < count[k]; ++i)
	{
		a[k] = i + 1;
		balance(w, count, n, k + 1, sum + w[k][i], a);
	}
}

static int check_result(int weight)
{
	return set[weight];
}

static void print_result(Input *in, int opt[][MAX_TYPE], int weight, int diff)
{
	int j, first = 1;

	printf("%5d | %5d | ", weight, diff);

	/* Foreach Weight */
	for(j = 0; j < in->Count; ++j)
	{
		/* Is weight used? */
		if(output[weight][j] != 0)
		{
			int v = output[weight][j] - 1;
			int w = opt[j][v];

			/* Is weight used? */
			if(w != 0)
			{
				if(first)
				{
					first = 0;
				}
				else
				{
					printf(" + ");
				}

				if(v == 0)
				{

				}
				if(v < in->Counts[j] + 1)
				{
					printf("(-%d * %d)", v, in->Weights[j]);
				}
				else if(v < 2 * in->Counts[j] + 1)
				{
					printf("(%d * %d)", v - in->Counts[j], in->Weights[j]);
				}
			}
		}
	}

	printf("\n");
}

int main(int argc, char **argv)
{
	int i, j, k;
	Input in;

	/* Check Command Line Parameters */
	if(argc != 2)
	{
		fprintf(stderr, "Usage: ./a5-marktwaage input-file\n");
		return 1;
	}

	/* Read Input File */
	if(input_get(&in, argv[1]))
	{
		fprintf(stderr, "Error reading input from \"%s\"\n", argv[1]);
		return 1;
	}

#ifdef DEBUG

	input_test(&in);

#endif

	static int taken[MAX_TYPE];
	int count[in.Count], opt[in.Count][MAX_TYPE];

#ifdef DEBUG

	{
		uint64_t no = 1;
		for(i = 0; i < in.Count; ++i)
		{
			for(j = 0; j < in.Counts[i]; ++j)
			{
				no *= 3;
			}
		}

		printf("Without optimization: %ld\n", no);

		uint64_t opt = 1;
		for(i = 0; i < in.Count; ++i)
		{
			opt *= 2 * in.Counts[i] + 1;
		}

		printf("With optimization: %ld\n", opt);
	}

#endif

	/* Create Array of Weights */
	for(i = 0; i < in.Count; ++i)
	{
		int n = 0;
		opt[i][n] = 0;

		for(j = 0, ++n; j < in.Counts[i]; ++j, ++n)
		{
			opt[i][n] = -in.Weights[i] * (j + 1);
		}

		for(j = 0; j < in.Counts[i]; ++j, ++n)
		{
			opt[i][n] = in.Weights[i] * (j + 1);
		}
	}

	for(i = 0; i < in.Count; ++i)
	{
#ifdef DEBUG
		printf("%3d: ", i);
#endif

		count[i] = 2 * in.Counts[i] + 1;

#ifdef DEBUG
		for(j = 0; j < count[i]; ++j)
		{
			printf("%14d ", opt[i][j]);
		}

		printf("\n");
#endif
	}

	memset(count_min, 255, MAX_STORE);

	balance(opt, count, in.Count, 0, 0, taken);


#ifdef DEBUG

	printf("\n\n\n");

	for(i = 0; i < MAX_STORE; ++i)
	{
		if(set[i])
		{
			printf("%5d: FOUND\n", i);
		}
		else
		{
			printf("%5d:\n", i);
		}
	}

	printf("\n\n\n");

#endif

	/* Print Weights from 10g to 10kg in 10g Intervals */
	for(i = 10; i <= MAX_WEIGHT; i += 10)
	{
		/* Print Weight */
		printf("%5d | ", i);

		if(check_result(i))
		{
			print_result(&in, opt, i, 0);
		}
		else
		{
			for(j = i; j < MAX_STORE; ++j)
			{
				if(check_result(j))
				{
					break;
				}
			}

			for(k = i; k >= 0; --k)
			{
				if(check_result(k))
				{
					break;
				}
			}

			if((j - i) < (i - k))
			{
				print_result(&in, opt, j, j - i);
			}
			else
			{
				print_result(&in, opt, k, i - k);
			}
		}
	}

	input_destroy(&in);
	return 0;
}


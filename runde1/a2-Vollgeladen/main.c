#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* #define DEBUG */

#define MAX_TIME  360 /* 6 Hours * 60 Minutes */
#define MAX_DAYS    5
#define MAX_SCORE  50

/* Input Data Structure */
typedef struct INPUT
{
	int Count, Minutes, *Positions, *Scores;
} Input;

static int input_get(Input *in, const char *filename)
{
	int i;
	FILE *fp;
	char *p, buf[256];
	float score;

	/* Open File */
	if(!(fp = fopen(filename, "r")))
	{
		return 1;
	}

	/* Get Number of Hotels */
	if(!fgets(buf, sizeof(buf), fp))
	{
		return 1;
	}

	in->Count = strtol(buf, &p, 10);

	/* Get Minutes */
	if(!fgets(buf, sizeof(buf), fp))
	{
		return 1;
	}

	in->Minutes = strtol(buf, &p, 10);

	/* Allocate Memory for Hotel Scores */
	if(!(in->Scores = malloc(in->Count * sizeof(int *))))
	{
		return 1;
	}

	/* Allocate Memory for Hotel Positions */
	if(!(in->Positions = malloc(in->Count * sizeof(int *))))
	{
		return 1;
	}

	/* Get Hotel Scores */
	for(i = 0; i < in->Count; ++i)
	{
		if(!fgets(buf, sizeof(buf), fp))
		{
			return 1;
		}

		/* Read Hotel Position (Minute) */
		in->Positions[i] = strtol(buf, &p, 10);

		/* Read Hotel Score */
		score = strtof(p, &p);

		/* Covert Score to Integer by Multiplying with 10 */
		in->Scores[i] = (int)(score * 10.0f);
	}

	fclose(fp);
	return 0;
}

#ifdef DEBUG
static void input_test(Input *in)
{
	int i;
	printf("Number of Hotels: %d\n", in->Count);
	printf("Duration in Minutes: %d\n", in->Minutes);
	for(i = 0; i < in->Count; ++i)
	{
		printf("Hotel %4d: %4d MIN | %2d PT\n", i, in->Positions[i], in->Scores[i]);
	}

	printf("\n");
}
#endif

static void input_destroy(Input *in)
{
	free(in->Positions);
	free(in->Scores);
}

static int try_score(Input *in, int score, int *out)
{
	int i, cnt = 0, hmins[in->Count], hids[in->Count], day = 0, min = 0, n = 0;

#ifdef DEBUG
	printf("Trying Min Score %d\n", score);
#endif

	/* Get Array of Hotels with minimum score */
	for(i = 0; i < in->Count; ++i)
	{
		if(in->Scores[i] < score)
		{
			continue;
		}

#ifdef DEBUG
		printf("Hotel %4d: %4d MIN | %3d PTS\n", i, in->Positions[i], in->Scores[i]);
#endif

		hmins[cnt] = in->Positions[i];
		hids[cnt++] = i;
	}

	/* Try to find path */
	for(i = 0; i < cnt; ++i)
	{
		/* End destination can be reached */
		if(min + MAX_TIME >= in->Minutes)
		{
			return n;
		}

		if(hmins[i] >= min + MAX_TIME || i == cnt - 1)
		{
			if(hmins[i] > min + MAX_TIME)
			{
				--i;

				/* Next Hotel too far away */
				if(i < 0 || hmins[i] == min)
				{
#ifdef DEBUG
					printf("> Too far: %d > %d\n", hmins[i + 1] - min, MAX_TIME);
#endif
					return -1;
				}
			}

			/* Farthest away Hotel that can be reached */
			min = hmins[i];
			out[n++] = hids[i];

#ifdef DEBUG
			printf("Stop at Hotel %d\n", min);
#endif

			/* Too many days */
			if(++day > MAX_DAYS)
			{
#ifdef DEBUG
				printf("> Too many days: %d > %d\n", day, MAX_DAYS);
#endif

				return -1;
			}
		}
	}

	/* End destination can be reached */
	if(min + MAX_TIME >= in->Minutes)
	{
		return n;
	}

	return -1;
}

static void choose_hotels(Input *in)
{
	int i, count[MAX_SCORE + 1], out[64], n;

	memset(count, 0, sizeof(count));

	/* Count how many Hotels have score X */
	for(i = 0; i < in->Count; ++i)
	{
		++count[in->Scores[i]];
	}

#ifdef DEBUG
	printf("Scores Count:\n");
	for(i = 0; i <= MAX_SCORE; ++i)
	{
		if(!count[i])
		{
			continue;
		}

		printf("%2d: %3d\n", i, count[i]);
	}

	printf("\n");
#endif

	/* Try all scores as minimum score starting at the highest */
	for(i = MAX_SCORE; i >= 0; --i)
	{
		/* Skip score that no hotels have */
		if(!count[i])
		{
			continue;
		}

		/* Is the trip with this minimum score possible? */
		if((n = try_score(in, i, out)) != -1)
		{
			int j;

			printf("\nLowest Score: %d.%d\n", i / 10, i % 10);

			for(j = 0; j < n; ++j)
			{
				printf("> Stop at Hotel %d (%d Minutes) - Score: %d.%d\n",
					out[j],
					in->Positions[out[j]],
					in->Scores[out[j]] / 10,
					in->Scores[out[j]] % 10);
			}

			return;
		}

#ifdef DEBUG
		printf("\n\n");
#endif
	}

	printf("\nNot possible!\n");
}

int main(int argc, char **argv)
{
	Input in;

	if(argc != 2)
	{
		fprintf(stderr, "Usage: ./a2-vollgeladen input-file\n");
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

	choose_hotels(&in);

	input_destroy(&in);
	return 0;
}


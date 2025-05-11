#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t sig = 0;

static void sig_handler(int signo)
{
	sig = signo;
}

#define WRAP

int ROW = 24;
int COL = 48;

int *current_world;
int *next_world;

int get_idx(int i, int j)
{
	return i * COL + j;
}

int mod(int x, int j)
{
	return ((x % j) + j) % j;
}

void print_world(int *world)
{
	for(int i = 0; i < ROW; i++)
	{
		for(int j = 0; j < COL; j++)
		{
			printf("%c", ".#"[world[get_idx(i, j)]]);
		}
		printf("\n");
	}
}

void create_next_world()
{
	for(int i = 0; i < ROW; i++)
	{
		for(int j = 0; j < COL; j++)
		{
			int count = 0;
			for(int x = -1; x < 2; x++)
			{
				for(int y = -1; y < 2; y++)
				{
#ifdef WRAP
					count += (x || y) ? current_world[get_idx(mod(x + i, ROW), mod(y + j, COL))] : 0;
#else
					count += (x || y) ? current_world[get_idx(mod(x + i, ROW), mod(y + j, COL))] * (((x + i) >= 0 && (x + i) <= ROW && (y + j) >= 0 && (y + j) <= COL)): 0;
#endif
				}
			}
			next_world[get_idx(i, j)]= current_world[get_idx(i, j)] ? (count == 2 || count == 3) : count == 3;
		}
	}
}

void init_world(char *initial_file_name)
{

	FILE *initial_file = fopen(initial_file_name, "r");
	char line[256];

	if(initial_file != NULL)
	{
		if(fgets(line, sizeof(line), initial_file))
			ROW = atoi(line);
		if(fgets(line, sizeof(line), initial_file))
			COL = atoi(line);

		current_world = malloc(sizeof(int) * COL * ROW);
		next_world = malloc(sizeof(int) * COL * ROW);

		for(int i = 0; fgets(line, sizeof(line), initial_file); i++)
		{
			for(int j = 0; j < COL; j++)
			{
				current_world[get_idx(i, j)] = (line[j] != '#') ? 0 : 1;
			}
		}
	}
	else
	{
		printf("could not find %s\n", initial_file_name);
		current_world = malloc(sizeof(int) * COL * ROW);
		next_world = malloc(sizeof(int) * COL * ROW);
		current_world[get_idx(0, 1)] = 1;
		current_world[get_idx(1, 2)] = 1;
		current_world[get_idx(2, 0)] = 1;
		current_world[get_idx(2, 1)] = 1;
		current_world[get_idx(2, 2)] = 1;
	}
}

int main(int argc, char **argv)
{
	if(signal(SIGINT, sig_handler) == SIG_ERR)
	{
		fputs("could not set signal handler\n", stderr);
		return 1;
	}

	init_world(argc < 2 ? "init.txt" : argv[1]);

	printf("initial world size: %d, %d\n", COL, ROW);

	for(;sig != SIGINT;)
	{
		print_world(current_world);
		create_next_world();
		memcpy(current_world, next_world, sizeof(int) * COL * ROW);
		usleep(1000 * 50);
		printf("\033[%dA\033[%dD", ROW, COL);
	}
	free(current_world);
	free(next_world);
	printf("memory freed\nbye\n");
	return 0;
}

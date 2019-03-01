/*
 *  Merges partial Torto solutions.
 *  Andre R. de la Rocha   --   28/Feb/2019
 */

#include <iostream>
#include <unordered_set>
#include <algorithm>
#include <string>
#include <cstdio>
#include <cstring>

// Board dimensions.
#define NUM_COL 3
#define NUM_ROW 6

char board[NUM_ROW][NUM_COL];

// all unique solutions including mirrored configurations
unsigned long long sol_count;
// determinate solutions (configurations with all matrix elements specified)
unsigned long long dsol_count;
// "essentially unique" considers mirrored solutions as equivalent
unsigned long long esol_count;
// essentially unique and determinate solutions
unsigned long long edsol_count;

// Set to filter out duplicate solutions.
std::unordered_set<std::string> unq_sols;

bool essential_only;
bool determinate_only;
bool single_line;
bool quiet;

bool is_determinate()
{
	for (int i = 0; i < NUM_ROW; ++i)
		for (int j = 0; j < NUM_COL; ++j)
			if (!board[i][j])
				return false;
	return true;
}

std::string board_key(bool flipy, bool flipx)
{
	std::string key;
	for (int i = 0; i < NUM_ROW; ++i)
		for (int j = 0; j < NUM_COL; ++j)
			key += board[flipy ? NUM_ROW-1-i : i][flipx ? NUM_COL-1-j : j];
	return key;
}

void output_board(bool flipy, bool flipx)
{
	for (int i = 0; i < NUM_ROW; ++i) {
		for (int j = 0; j < NUM_COL; ++j) {
			char c = board[flipy ? NUM_ROW-1-i : i][flipx ? NUM_COL-1-j : j];
			printf("%c", c ? c : '.');
		}
		if (!single_line) printf("\n");
	}
	printf("\n");
}

void output_sol_key(std::string key, bool det, bool flipy, bool flipx)
{
	if (unq_sols.find(key) == unq_sols.end()) {
		++sol_count;
		if (det) ++dsol_count;
		unq_sols.insert(key);
		if (!quiet && !essential_only && (!determinate_only || det))
			output_board(flipy, flipx);
	}
}

void output_solution()
{
	bool det = is_determinate();
	
	std::string k1 = board_key(false, false);
	std::string k2 = board_key(false, true);
	std::string k3 = board_key(true, false);
	std::string k4 = board_key(true, true);
	
	if (unq_sols.find(k1) == unq_sols.end()
			&& unq_sols.find(k2) == unq_sols.end()
			&& unq_sols.find(k3) == unq_sols.end()
			&& unq_sols.find(k4) == unq_sols.end()) {
		++esol_count;
		if (det) ++edsol_count;
		if (!quiet && essential_only && (!determinate_only || det))
			output_board(false, false);
	}
	output_sol_key(k1, det, false, false);
	output_sol_key(k2, det, false, true);
	output_sol_key(k3, det, true, false);
	output_sol_key(k4, det, true, true);
}

int main(int argc, char *argv[])
{
	bool no_stats = false;

	for (int i = 1; i < argc; ++i) {
		std::string arg = std::string(argv[i]);
		if (arg == "-e")
			essential_only = true;
		else if (arg == "-d")
			determinate_only = true;
		else if (arg == "-s")
			single_line = true;
		else if (arg == "-q")
			quiet = true;
		else if (arg == "-m")
			no_stats = true;
		else if (arg == "-h") {
			fprintf(stderr, "Usage: %s [-e] [-d] [-u] [-r] [-s] [-q]  <input.txt  >output.txt\n", argv[0]);
			fprintf(stderr, "       -e       : output only essentially unique solutions\n");
			fprintf(stderr, "       -d       : output only determinate solutions\n");
			fprintf(stderr, "       -s       : output each solution using a single line\n");
			fprintf(stderr, "       -q       : quiet mode - do not output solutions\n");
			fprintf(stderr, "       -m       : mute mode - do not output statistics\n");
			return 0;
		}
	}

	char buf[100];
	while (fgets(buf, sizeof(buf), stdin)) {
		if (strlen(buf) >= NUM_COL*NUM_ROW) {
			for (int i = 0; i < NUM_ROW; ++i) {
				for (int j = 0; j < NUM_COL; ++j) {
					char c = buf[i*NUM_COL+j];
					board[i][j] = (c == '.') ? 0 : c;
				}
			}
			output_solution();
		}
	}

	if (!no_stats) {
		fprintf(stderr, "Total solutions: %lld  (%lld determinate)\n", sol_count, dsol_count);
		fprintf(stderr, "Essentially unique solutions: %lld  (%lld determinate)\n", esol_count, edsol_count);
	}
	return 0;
}

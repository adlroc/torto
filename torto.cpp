/*
 *  Solves "Reversed Torto" using a simple recursive algorithm.
 *  Andre R. de la Rocha   --   23/Feb/2019
 */

#include <iostream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <string>

// Board dimensions.
#define NUM_COL 3
#define NUM_ROW 6

// Padding inner array dim. to powers of two makes access slightly faster.
#define PAD_COL 1
#define PAD_ROW 2

#define MAX_WORDS 64

const char *words[MAX_WORDS];

// board characters
int board[NUM_ROW][NUM_COL+PAD_COL];
// counts the number of words simultaneously covering a board site
int occupancy[NUM_ROW][NUM_COL+PAD_COL];
// tracks the steps of a word being placed on the board
int crumbs[MAX_WORDS][NUM_ROW+PAD_ROW][NUM_COL+PAD_COL];

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

// number of partitions and index for partitioned mode
unsigned int par_cnt = 1;
unsigned int par_idx = 0;

// flags
bool essential_only;
bool determinate_only;
bool disable_pre_sort;
bool raw_mode;
bool single_line;
bool quiet;

void output_solution();

void rec_torto(int n, int w, int l, int i, int j)
{
	crumbs[w][i][j] = l+2;
	board[i][j] = words[w][l];
	++occupancy[i][j];

	// next char in word
	char c = words[w][l+1];

	if (c) {
		// north
		if (i > 0 && (!occupancy[i-1][j] || board[i-1][j] == c)
				&& !crumbs[w][i-1][j])
			rec_torto(n, w, l+1, i-1, j);
		if (j < NUM_COL-1) {
			// east
			if ((!occupancy[i][j+1] || board[i][j+1] == c)
					&& !crumbs[w][i][j+1])
				rec_torto(n, w, l+1, i, j+1);
			// northeast
			if (i > 0 && (!occupancy[i-1][j+1] || board[i-1][j+1] == c)
					&& !crumbs[w][i-1][j+1]
					&& abs(crumbs[w][i-1][j] - crumbs[w][i][j+1]) != 1)
				rec_torto(n, w, l+1, i-1, j+1);
			// southeast
			if (i < NUM_ROW-1 && (!occupancy[i+1][j+1] || board[i+1][j+1] == c)
					&& !crumbs[w][i+1][j+1]
					&& abs(crumbs[w][i+1][j] - crumbs[w][i][j+1]) != 1)
				rec_torto(n, w, l+1, i+1, j+1);
		}
		if (j > 0) {
			// west
			if ((!occupancy[i][j-1] || board[i][j-1] == c)
					&& !crumbs[w][i][j-1])
				rec_torto(n, w, l+1, i, j-1);
			// northwest
			if (i > 0 && (!occupancy[i-1][j-1] || board[i-1][j-1] == c)
					&& !crumbs[w][i-1][j-1]
					&& abs(crumbs[w][i-1][j] - crumbs[w][i][j-1]) != 1)
				rec_torto(n, w, l+1, i-1, j-1);
			// southwest
			if (i < NUM_ROW-1 && (!occupancy[i+1][j-1] || board[i+1][j-1] == c)
					&& !crumbs[w][i+1][j-1]
					&& abs(crumbs[w][i+1][j] - crumbs[w][i][j-1]) != 1)
				rec_torto(n, w, l+1, i+1, j-1);
		}
		// south
		if (i < NUM_ROW-1 && (!occupancy[i+1][j] || board[i+1][j] == c)
				&& !crumbs[w][i+1][j])
			rec_torto(n, w, l+1, i+1, j);
	} else {
		// last char
		if (w == n-1) {
			// last word
			output_solution();
		} else {
			// next word
			char nc = words[w+1][0];
			for (int y = 0; y < NUM_ROW; ++y) {
				for (int x = 0; x < NUM_COL; ++x) {
					static unsigned int h;
					if ((!occupancy[y][x] || board[y][x] == nc)
							&& (w > 0 || (h = h * 1103515245U + 12345U) % par_cnt == par_idx))
						rec_torto(n, w+1, 0, y, x);
				}
			}
		}
	}
	// undo changes
	--occupancy[i][j];
	crumbs[w][i][j] = 0;
}

void torto(int n)
{
	// Due to reflection symmetry we only need to
	// explore ~ 1/4 of the configuration space.
	int rows = (NUM_ROW+1)/2;
	int cols = (NUM_COL+1)/2;
	for (int i = 0; i < rows; ++i)
		for (int j = 0; j < cols; ++j)
			rec_torto(n, 0, 0, i, j);
}

bool is_determinate()
{
	for (int i = 0; i < NUM_ROW; ++i)
		for (int j = 0; j < NUM_COL; ++j)
			if (!occupancy[i][j])
				return false;
	return true;
}

std::string board_key(bool flipy, bool flipx)
{
	std::string key;
	for (int i = 0; i < NUM_ROW; ++i) {
		for (int j = 0; j < NUM_COL; ++j) {
			int y = flipy ? NUM_ROW-1-i : i;
			int x = flipx ? NUM_COL-1-j : j;
			key += (occupancy[y][x] ? static_cast<char>(board[y][x]) : '.');
		}
	}
	return key;
}

void output_board(bool flipy, bool flipx)
{
	for (int i = 0; i < NUM_ROW; ++i) {
		for (int j = 0; j < NUM_COL; ++j) {
			int y = flipy ? NUM_ROW-1-i : i;
			int x = flipx ? NUM_COL-1-j : j;
			std::cout << (occupancy[y][x] ? static_cast<char>(board[y][x]) : '.');
		}
		if (!single_line) std::cout << "\n";
	}
	std::cout << "\n";
}

void output_sol_key(std::string key, bool det, bool flipy, bool flipx)
{
	if (raw_mode || unq_sols.find(key) == unq_sols.end()) {
		++sol_count;
		if (det) ++dsol_count;
		if (!raw_mode) unq_sols.insert(key);
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
	
	if (!raw_mode
			&& unq_sols.find(k1) == unq_sols.end()
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

// Used to sort the list from the largest to smallest word,
// to help eliminate large branches of the search tree first.
bool sorter(const std::string &a, const std::string &b)
{
	return a.length() > b.length();
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
		else if (arg == "-u")
			disable_pre_sort = true;
		else if (arg == "-r")
			raw_mode = true;
		else if (arg == "-s")
			single_line = true;
		else if (arg == "-q")
			quiet = true;
		else if (arg == "-m")
			no_stats = true;
		else if (arg == "-p" && i < argc-2) {
			par_cnt = strtoul(argv[++i], NULL, 10);
			par_idx = strtoul(argv[++i], NULL, 10);
			if (par_cnt < 1 || par_idx >= par_cnt) return -1;
		} else {
			std::cerr << "Usage: " << argv[0] << " [flags] <input.txt  >output.txt\n";
			std::cerr << "       -e             : output only essentially unique solutions\n";
			std::cerr << "       -d             : output only determinate solutions\n";
			std::cerr << "       -u             : do not sort word list before searching solutions\n";
			std::cerr << "       -r             : raw mode - do not filter out duplicate solutions\n";
			std::cerr << "       -s             : output each solution using a single line\n";
			std::cerr << "       -q             : quiet mode - do not output solutions\n";
			std::cerr << "       -m             : mute mode - do not output statistics\n";
			std::cerr << "       -p <cnt> <idx> : partitioned mode\n";
			return 0;
		}
	}

	int num_words;
	std::cin >> num_words;
	if (num_words < 1 || num_words > MAX_WORDS) return -1;

	std::vector<std::string> wordvec(num_words);

	for (int i = 0; i < num_words; ++i)
		std::cin >> wordvec[i];

	if (!disable_pre_sort)
		std::sort(wordvec.begin(), wordvec.end(), sorter);

	for (int i = 0; i < num_words; ++i)
		words[i] = wordvec[i].c_str();

	torto(num_words);

	if (!no_stats) {
		std::cerr << "Total solutions: " << sol_count
				  << "  (" << dsol_count << " determinate)\n";
		std::cerr << "Essentially unique solutions: " << esol_count
				  << "  (" << edsol_count << " determinate)\n";
	}
	return 0;
}

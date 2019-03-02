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

// Defined by bitmask type. It can be trivially increased by using
// multiple array elements, but 64 seems good enough for our needs.
#define MAX_WORDS 64

// max top level partitions
#define MAX_PARTS (((NUM_ROW+1)/2)*((NUM_COL+1)/2))

const char *words[MAX_WORDS];
int len[MAX_WORDS];

int parity[MAX_WORDS];

// board characters
char board[NUM_ROW][NUM_COL+PAD_COL];
// bitmask to mark simultaneous covering of a site by multiple words
unsigned long long bitmask[NUM_ROW][NUM_COL+PAD_COL];
// breadcrumbs to enforce "no diagonal crossing" constraint
int crumbs[MAX_WORDS][NUM_ROW+PAD_ROW][NUM_COL+PAD_COL];

// all unique solutions including mirrored configurations
unsigned long long sol_count;
// determinate solutions (configurations with all matrix elements specified)
unsigned long long dsol_count;
// "essentially unique" considers mirrored solutions as equivalent
unsigned long long esol_count;
// essentially unique and determinate solutions
unsigned long long edsol_count;
// number of recursive calls
unsigned long long call_count;

// Set to filter out duplicate solutions.
std::unordered_set<std::string> unq_sols;

bool essential_only;
bool determinate_only;
bool disable_pre_sort;
bool raw_mode;
bool single_line;
bool quiet;
int part_idx = -1;

void output_solution();

void rec_torto(int n, int w, int l, int i, int j)
{
	++call_count;
	board[i][j] = words[w][l];
	bitmask[i][j] |= 1<<w;
	crumbs[w][i][j] = l+2;

	if (l == len[w]-1) {
		// last char
		if (w == n-1) {
			// last word
			output_solution();
		} else {
			// next word
			for (int y = 0; y < NUM_ROW; ++y)
				for (int x = 0; x < NUM_COL; ++x)
					if ((!board[y][x] || board[y][x] == words[w+1][0])
							&& !(bitmask[y][x] & (1<<(w+1)))
							&& (parity[w+1] == -1 || (y*NUM_COL+x)%2 == parity[w+1]))
						rec_torto(n, w+1, 0, y, x);
		}
	} else {
		// next char in word
		char c = words[w][l+1];
		if (j < NUM_COL-1) {
			// east
			if ((!board[i][j+1] || board[i][j+1] == c)
					&& !(bitmask[i][j+1] & (1<<w)))
				rec_torto(n, w, l+1, i, j+1);
			// northeast
			if (i > 0 && (!board[i-1][j+1] || board[i-1][j+1] == c)
					&& !(bitmask[i-1][j+1] & (1<<w))
					&& abs(crumbs[w][i-1][j] - crumbs[w][i][j+1]) != 1)
				rec_torto(n, w, l+1, i-1, j+1);
			// southeast
			if (i < NUM_ROW-1 && (!board[i+1][j+1] || board[i+1][j+1] == c)
					&& !(bitmask[i+1][j+1] & (1<<w))
					&& abs(crumbs[w][i+1][j] - crumbs[w][i][j+1]) != 1)
				rec_torto(n, w, l+1, i+1, j+1);
		}
		if (j > 0) {
			// west
			if ((!board[i][j-1] || board[i][j-1] == c)
					&& !(bitmask[i][j-1] & (1<<w)))
				rec_torto(n, w, l+1, i, j-1);
			// northwest
			if (i > 0 && (!board[i-1][j-1] || board[i-1][j-1] == c)
					&& !(bitmask[i-1][j-1] & (1<<w))
					&& abs(crumbs[w][i-1][j] - crumbs[w][i][j-1]) != 1)
				rec_torto(n, w, l+1, i-1, j-1);
			// southwest
			if (i < NUM_ROW-1 && (!board[i+1][j-1] || board[i+1][j-1] == c)
					&& !(bitmask[i+1][j-1] & (1<<w))
					&& abs(crumbs[w][i+1][j] - crumbs[w][i][j-1]) != 1)
				rec_torto(n, w, l+1, i+1, j-1);
		}
		// north
		if (i > 0 && (!board[i-1][j] || board[i-1][j] == c)
				&& !(bitmask[i-1][j] & (1<<w)))
			rec_torto(n, w, l+1, i-1, j);
		// south
		if (i < NUM_ROW-1 && (!board[i+1][j] || board[i+1][j] == c)
				&& !(bitmask[i+1][j] & (1<<w)))
			rec_torto(n, w, l+1, i+1, j);
	}
	// undo changes
	bitmask[i][j] &= ~(1<<w);
	if (!bitmask[i][j]) board[i][j] = 0;
	crumbs[w][i][j] = 0;
}

void torto(int n)
{
	int rows = (NUM_ROW+1)/2;
	int cols = (NUM_COL+1)/2;
	if (part_idx > -1) {
		rec_torto(n, 0, 0, part_idx / cols, part_idx % cols);
	} else {
		// Due to reflection symmetry we only need to
		// explore ~ 1/4 of the configuration space.
		for (int i = 0; i < rows; ++i)
			for (int j = 0; j < cols; ++j)
				if (parity[0] == -1 || (i*cols+j)%2 == parity[0])
					rec_torto(n, 0, 0, i, j);
	}
}

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
			std::cout << (c ? c : '.');
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

	for (int i = 0; i < MAX_WORDS; ++i)
		parity[i] = -1;

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
		else if (arg == "-i" && i < argc-1) {
			part_idx = atoi(argv[++i]);
			if (part_idx < 0 || part_idx >= MAX_PARTS) return -1;
		} else if (arg == "-p" && i < argc-2) {
			int lvl = atoi(argv[++i]);
			int par = atoi(argv[++i]);
			if (lvl < 0 || lvl >= MAX_WORDS || par < 0 || par > 1) return -1;
			parity[lvl] = par;
		} else if (arg == "-h") {
			std::cerr << "Usage: " << argv[0] << " [flags] <input.txt  >output.txt\n";
			std::cerr << "       -e         : output only essentially unique solutions\n";
			std::cerr << "       -d         : output only determinate solutions\n";
			std::cerr << "       -u         : do not sort word list before searching solutions\n";
			std::cerr << "       -r         : raw mode - do not filter out duplicate solutions\n";
			std::cerr << "       -s         : output each solution using a single line\n";
			std::cerr << "       -q         : quiet mode - do not output solutions\n";
			std::cerr << "       -m         : mute mode - do not output statistics\n";
			std::cerr << "       -i <idx>   : partial mode: top level partition with index <idx>\n";
			std::cerr << "       -p <l> <p> : partial mode: bifurcation level <l> with parity <p>\n";
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

	for (int i = 0; i < num_words; ++i) {
		words[i] = wordvec[i].c_str();
		len[i] = static_cast<int>(wordvec[i].length());
		if (len[i] < 1) return -1;
	}

	torto(num_words);

	if (!no_stats) {
		std::cerr << "Total solutions: " << sol_count
				  << "  (" << dsol_count << " determinate)\n";
		std::cerr << "Essentially unique solutions: " << esol_count
				  << "  (" << edsol_count << " determinate)\n";
		std::cerr << "Call count: " << call_count << "\n";
	}
	return 0;
}

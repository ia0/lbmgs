#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <game/chess.h>

#include <common.h>
#include <client.h>

/* white > 0, black < 0 */
enum piece {
	EMPTY = 0,
	KING = 1,
	QUEEN = 2,
	ROOK = 3,
	KNIGHT = 4,
	BISHOP = 5,
	PAWN = 6,
};
struct chess {
	int8_t board[8][8]; /* [a-h][1-8] */
	int white;
	int black;
	int turn:1; /* white == 0, black == -1 */
};

static void
board_init(int8_t board[8][8])
{
	unsigned char x;
	int y;
	board[0][0] = board[7][0] = ROOK;
	board[0][7] = board[7][7] = -ROOK;
	board[1][0] = board[6][0] = KNIGHT;
	board[1][7] = board[6][7] = -KNIGHT;
	board[2][0] = board[5][0] = BISHOP;
	board[2][7] = board[5][7] = -BISHOP;
	board[3][0] = QUEEN;
	board[3][7] = -QUEEN;
	board[4][0] = KING;
	board[4][7] = -KING;
	for (x = 0; x < 8; x++) {
		board[x][1] = PAWN;
		board[x][6] = -PAWN;
	}
	for (x = 0; x < 8; x++)
		for (y = 2; y < 6; y++)
			board[x][y] = EMPTY;
}

static void
piece_print(int cid, int8_t v)
{
	int black;
	unsigned char r;
	black = v < 0;
	switch (black ? -v : v) {
	case EMPTY: r = '-'; break;
	case KING: r = 'k'; break;
	case QUEEN: r = 'q'; break;
	case ROOK: r = 'r'; break;
	case KNIGHT: r = 'n'; break;
	case BISHOP: r = 'b'; break;
	case PAWN: r = 'p'; break;
	default:
		geprintf(cid, "unexpected piece %d\n", v);
		r = '?'; break;
	}
	cprintf(cid, " %c", black ? toupper(r) : r);
	return;
}

static void
board_print(int cid, struct chess *chess)
{
	unsigned char x;
	int y, start, end, step;

	assert(chess->white == cid || chess->black == cid);

	if (chess->white == cid) {
		start = 7;
		end = -1;
		step = -1;
	} else {
		start = 0;
		end = 8;
		step = 1;
	}

	for (y = start; y != end; y += step) {
		cprintf(cid, "%d ", y + 1);
		for (x = 0; x < 8; x++)
			piece_print(cid, chess->board[x][y]);
		cprintf(cid, "\n");
	}
	cprintf(cid, "   a b c d e f g h\n");

	return;
}

static void *
chess_create(int cid)
{
	struct chess *chess;

	chess = malloc(sizeof(*chess));
	board_init(chess->board);
	chess->white = cid;
	chess->black = -1;
	chess->turn = 0;

	cprintf(cid, "You play white.\n");
	cprintf(cid, "Wait for a player to join.\n");

	return chess;
}

static void
chess_clean(void *data)
{
	assert(data != NULL);
	free(data);
	return;
}

static int
chess_join(int cid, void *data)
{
	struct chess *chess = data;
	int pid;

	assert(valid_cid(cid));
	assert(chess != NULL);

	chess->black = cid;
	pid = chess->white;

	cprintf(pid, "\n");
	board_print(pid, data);
	cprintf(pid, "This is your turn to play.\n");
	cprompt(pid);

	board_print(cid, data);
	cprintf(cid, "You play black.\n");
	cprintf(cid, "Wait for white to play.\n");

	return 1;
}

/* static int */
/* chess_leave(int cid, void *data) */
/* { */
/* 	struct chess *chess = data; */
/* 	int i, last; */

/* 	assert(valid_cid(cid)); */
/* 	assert(chess != NULL); */

/* 	for (i = 0; i < ID_SLOTS; i++) */
/* 		if (chess->ids[i] == cid) */
/* 			break; */
/* 	if (i == ID_SLOTS) { */
/* 		geprintf(cid, "could not find chess slot\n"); */
/* 		return 0; */
/* 	} */

/* 	chess->ids[i] = -1; */

/* 	cprintf(cid, "Ending chess.\n"); */

/* 	for (last = 1, i = 0; i < ID_SLOTS; i++) { */
/* 		int pid = chess->ids[i]; */
/* 		if (pid != -1) { */
/* 			assert(pid != cid); */
/* 			last = 0; */
/* 			cprintf(pid, "\n%d has left.\n", cid); */
/* 			cprompt(pid); */
/* 		} */
/* 	} */
/* 	if (last) { */
/* 		chess_clean(data); */
/* 		return -1; */
/* 	} */

/* 	return 0; */
/* } */

/* static int */
/* chess_process(int cid, void *data, char *line) */
/* { */
/* 	struct chess *chess = data; */
/* 	int i; */

/* 	assert(chess != NULL); */

/* 	if (!*line) */
/* 		return -1; */

/* 	for (i = 0; i < ID_SLOTS; i++) { */
/* 		int pid = chess->ids[i]; */
/* 		if (pid != -1) { */
/* 			if (pid != cid) { */
/* 				cprintf(pid, "\n%d: %s\n", cid, line); */
/* 				cprompt(pid); */
/* 			} else { */
/* 				cprintf(cid, "%d) %s\n", cid, line); */
/* 			} */
/* 		} */
/* 	} */

/* 	return 0; */
/* } */

struct game chess_game = {
	.name = "chess",
	.create = chess_create,
	.clean = chess_clean,
	.join = chess_join,
	/* .leave = chess_leave, */
	/* .process = chess_process, */
};

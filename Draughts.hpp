#include<vector>
#include<iostream>

#define NUM_COLS 8
#define NUM_ROWS 8
#define MAX_LENGTH_MAN 1

inline int sign(int x) {
	return (x > 0) - (x < 0);
}

class Movement;
class Board;

class Position {

	public:
		enum Col {
			A, B, C, D, E, F, G, H, NOT_VALID
		};

		Position(int row, Col col);
		int getRow() const;
		Col getCol() const;
		bool withinBounds();
		std::vector<Movement*> getPossibleEats(Board& board);

	private:
		int row;
		Col col;

};

enum PlayerTurn {
	PLAYER1, PLAYER2, UNKNOWN
};

enum PieceType {
	EMPTY, MAN, KING
};

struct Piece {
	PieceType type = EMPTY;
	PlayerTurn owner = UNKNOWN;
};

class Movement {
	Position origin;
	Piece originalPiece;
	Piece destinyPiece;
	Position destiny;
	Position realDestiny;
	Movement* nextMove = nullptr;
	Board* board;

	public:
		Movement(Position origin, Position destiny, Board* board, bool expand = false);
		~Movement();
		Position getOrigin();
		Position getDestiny();
		bool isDiagonal();
		bool hasValidOrigin(PlayerTurn player);
		bool hasValidDestiny(PlayerTurn player);
		bool canEat();
		bool tryingToEat();
		bool isValid(PlayerTurn player);
		int getLength();
		int pathLength();
		bool clearPath();
		Position getNextPosition();
		Movement* findLongestPath();
		void execute();
		void undo();

		friend std::ostream& operator <<(std::ostream& outputStream, const Movement& m);
};



class Player {
	protected:
		PlayerTurn turn;
		Position::Col getColFromChar(char col);
		Player(PlayerTurn turn);
	public:
		enum Type {
			HUMAN, RANDOM_AI, AB_AI
		};
		static Player& create(PlayerTurn turn, Type type);
		PlayerTurn getTurn();
		virtual Movement getMovement(Board *board) = 0;
		friend std::ostream& operator <<(std::ostream& outputStream, const Player& p);
};

class HumanPlayer: public Player {
		public:
			HumanPlayer(PlayerTurn turn);
			Movement getMovement(Board *board) final;
};

class RandomAIPlayer: public Player {
		public:
			RandomAIPlayer(PlayerTurn turn);
			Movement getMovement(Board *board) final;
};

class AlphaBetaAIPlayer: public Player {
		private:
			Movement *bestMove;
			int search(Board &board, int depth, int a, int b, bool maximizing);
			int heuristic(Board &board);
		public:
			AlphaBetaAIPlayer(PlayerTurn turn);
			Movement getMovement(Board *board) final;
};

class Board {
	public:
		int scoreByDistanceToKings(PlayerTurn player);	//Each piece in the farest row to king's row is worth 1, the closests are worth 8
		int numInvenciblePiecesFor(PlayerTurn player, PieceType type);
		int numUnusablePiecesFor(PlayerTurn player, PieceType type);
		int getPiecesFor(PlayerTurn player, PieceType type);
		PlayerTurn getOwner(Position pos);
		bool playerCanMove(PlayerTurn player);
		bool pieceHasFreedom(Position pos);
		bool pieceCanBeEaten(Position pos);
		Piece getAt(Position pos);
		std::vector<Position> piecesWithFreedomOf(PlayerTurn player);
		std::vector<Position> possibleDestiniesFor(Position origin);

		Piece& operator ()(int row, int col);

		friend std::ostream& operator <<(std::ostream& outputStream, const Board& b);

#define EMP {EMPTY, UNKNOWN}
#define MAN1 {MAN, PLAYER1}
#define MAN2 {MAN, PLAYER2}
#define KING1 {KING, PLAYER1}
#define KING2 {KING, PLAYER2}

	private:
		Piece board[NUM_ROWS][NUM_COLS] = 
						{
						{EMP, MAN2, EMP, MAN2, EMP, MAN2, EMP, MAN2}, 
						{MAN2, EMP, MAN2, EMP, MAN2, EMP, MAN2, EMP},
						{EMP, MAN2, EMP, MAN2, EMP, MAN2, EMP, MAN2},
						{EMP, EMP, EMP, EMP, EMP, EMP, EMP, EMP},
						{EMP, EMP, EMP, EMP, EMP, EMP, EMP, EMP},
						{MAN1, EMP, MAN1, EMP, MAN1, EMP, MAN1, EMP},
						{EMP, MAN1, EMP, MAN1, EMP, MAN1, EMP, MAN1},
						{MAN1, EMP, MAN1, EMP, MAN1, EMP, MAN1, EMP}
						};

};

class Match {
	private:
		Board board;
		Player& player1;
		Player& player2;
		PlayerTurn getRandomTurn();
	
	public:
		Match(Player& player1, Player& player2);
		void start();
};

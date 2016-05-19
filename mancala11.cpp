#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <limits>

static int inf = std::numeric_limits<int>::max();
static int _inf = std::numeric_limits<int>::min();


struct move
{
	int pitNumber;
	int score;
	int board[22];
	move(int n, int s)
	{
		pitNumber = n;
		score = s;
	}
	move(int n, int s, int* b)
	{
		pitNumber = n;
		score = s;
		for (int i = 0; i<22 ; i++)
			board[i] = b[i];
	}
};

bool operator>(const move & lhs, const move & rhs)
{
	if(lhs.score > rhs.score)
		return true;
	else
		return false;
}

std::string check_Val( int val )
{
	if (val == _inf)
		return "-Infinity";
	else if (val == inf)
		return "Infinity";
	else
		return std::to_string(val);
}

class mancala
{
public:
	int whoStarts, cutoffDepth, n_pits, arrSize;
	std::ofstream o_file;//pits keeps count of the number of pits each player has(other than his mancala)
	std::ofstream o_file_state;
	int MAX = _inf; int MIN = inf;
	//int MAX = inf, MIN = _inf;
	int board[22];//everything is stored here!
	void setMancala(std::string str, int n);
	void setState(std::string str, int n);
	mancala(std::string s, std::string p, int q);
	bool makeMove(int* board, move m, int player);//returns board. the getBestMove func has to handle the nextmove based on the state of the board 
	// move minimax(int* board, int player);
	move Max_play(int * b , int count, int player, std::string s, bool re_move, int local);
	move Min_play(int * b , int count, int player, std::string s, bool re_move, int local);
	move abMax_play(int * b , int count, int player, std::string s, bool re_move, int local, int alpha, int beta);
	move abMin_play(int * b , int count, int player, std::string s, bool re_move, int local, int alpha, int beta);
	void minimaxUtil();
	void abminimaxUtil();
	int check_board(int * b);
	void boardBackup(int* a, int* b);
	void endgamemove(int * b);
	int evaluate(int* b);
	// void printBoard(int * b);
};


// this function only changes the board
// pass only legal and optimal(after they go through minimax) moves to this function.
// function returns true if the last stone ended in a mancala. the calling function should check if the board
// state results in another move for the same player.
bool mancala::makeMove(int* b, move m, int player)
{
	int i = m.pitNumber;
	if(player == 1)
	{
		int rocks = b[i];
		b[i] = 0;
		for(i = i+1; rocks!=0 ; i = (i+1) % arrSize)
		{
			if(i==arrSize-1)
				continue;
			b[i] = b[i] + 1;
			rocks--;
		}
		i--;
		if( (i>=0 && i<n_pits) && b[i] == 1)//if the last stone ends up in an empty pit on player's side
		{
			b[n_pits] = b[n_pits] + b[2*n_pits - i] + b[i];
			b[2*n_pits - i] = 0;
			b[i] = 0;
			return false;
		}
		else if( i == n_pits )
			return true;
		else
			return false;
	}
	else
	{
		int rocks = b[i];
		b[i] = 0;
		for(i = i+1; rocks!=0 ; i = (i+1) % arrSize)
		{
			if(i==n_pits)
				continue;
			b[i] = b[i] + 1;
			rocks--;
		}
		if(i!=0)
			i--;
		else
			i = arrSize-1;
 		if( ( i >= n_pits+1 && i < arrSize-1) && b[i] == 1 )
		{
			b[arrSize-1] = b[arrSize-1] + b[2*n_pits - i] + b[i];
			b[2*n_pits - i] = 0;
			b[i] = 0;
			return false;
		}
		else if(i == arrSize-1)
			return true;
		else
			return false;
	}
}

void mancala::boardBackup(int* a, int* b)//creates acopy of board a into b
{
	for(int i = 0 ; i<arrSize ; i++)
	{
		b[i] = a[i];
	}
}

void mancala::minimaxUtil()
{
	//whoStarts is Max player so, he makes the first move
	// std::cout << "Inside the util now\n";
	o_file.open("traverse_log.txt");
	o_file << "Node,Depth,Value\n";
	o_file << "root,0,-Infinity\n";
	move v = Max_play(board, 0, whoStarts, "root", false, _inf);
	// o_file << "best move is " << v.pitNumber << " and it's value is " << v.score << std::endl; 
	// o_file << "final board \n";
	// for(int i =0; i< 22; i++)
	// 	o_file << v.board[i] << " ";
	o_file.close();
	o_file_state.open("next_state.txt");
	// o_file_state << "hell";
	for(int i = arrSize-2; i>n_pits; i--)
	{
		o_file_state << v.board[i];
		if (i!=n_pits+1)
			o_file_state << " ";
	}
	o_file_state << "\n";
	for(int i = 0; i<n_pits ;i++)
	{
		o_file_state << v.board[i];
		if (i!=n_pits-1)
			o_file_state << " ";
	}
	o_file_state << "\n";
	o_file_state << v.board[arrSize-1] << "\n";
	o_file_state << v.board[n_pits] << "\n";

	o_file_state.close();

	// o_file.close();
}

int mancala::evaluate(int* b)
{
	return (whoStarts == 1 ? (b[n_pits] - b[arrSize-1]) : (b[arrSize-1] - b[n_pits]));
}

int mancala::check_board(int * b)
{
	bool flip = true;
	for(int i = 0; i<n_pits; i++ )
	{
		if(b[i] != 0)
			flip = false;
	}
	bool flop = true;
	for(int i = n_pits+1 ; i < arrSize-1 ; i++)
	{
		if(b[i] != 0)
			flop = false;
	}
	//flip or flop == true => either side is empty
	if(flip || flop)
		return 1;//game ends. no further moves
	else
		return 0;//the game continues

}

void mancala::endgamemove(int * b)
{
	int rocks = 0;
	for(int i =0 ; i<n_pits; i++)
	{
		rocks = rocks + b[i];
		b[i] = 0;
	}
	b[n_pits] = b[n_pits] + rocks;
	rocks = 0;
	for( int i = n_pits + 1; i<arrSize-1 ; i++)
	{
		rocks = rocks + b[i];
		b[i] = 0;
	}
	b[arrSize-1] = b[arrSize-1] + rocks;
}

move mancala::Max_play(int * b, int depth, int player, std::string parentMove, bool re_move, int local)
{
	int best_move; 
	int best_board[22] = { };
	int c = depth;
	int local_max = local;
	int board1[22] = { };
	bool flop = false; //to check if a move exists for the current player
	std::string S;
	for(int t = 0 ; t < arrSize ; t++)// saving the board contents before we run any moves
		board1[t] = b[t];
	if(c < cutoffDepth)
	{
		// std::cout << "Depth less than cutoff depth\n";
		if(player == 1)
		{
			for(int i = 0 ; i < n_pits ; i++ )
			{
				if(board1[i] != 0)
				{
					flop = true;
					move m(i ,inf, board1);
					// std::cout << "Max_play Made this move on this board :\n";
					// printBoard(board1); //getchar();
		 			bool is_re_move = makeMove(board1, m, player);
		 			// check board type
		 			int type = check_board(board1);//if type == 1 no further moves. move all rocks to relevant player's mancala. if 0 continue

					// std::cout << "(B" << i+2 << -1000000 << "), " << c+1 << std::endl;
					S = "B" + std::to_string(i+2);// + "," + std::to_string(c);// + "\n";
		 			// std::cout << "After move: \n";
		 			// printBoard(board1);//getchar();
		 			move u(0,0); // the move returned by the recursive functions is stored here2
		 			if(type == 1)
		 			{
		 				endgamemove(board1);// moves all the remaining rocks into the respectvive players mancala
		 				//rocks have been moved off the board into the respective player's mancalas
		 				int util = evaluate(board1);
		 				if (is_re_move)
		 				{
			 				// o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << "-Infinity" << std::endl;
			 				o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << check_Val(util) << std::endl;
 		 				}
 		 				else
 		 				{
 		 					// o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << "Infinity" << std::endl;
 		 					o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << check_Val(util) << std::endl;
 		 				}
 		 				if(util > local_max)
 		 					local_max = util;
 		 				if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << std::endl;
						}
		 				move m(-1100000, evaluate(board1), board1);
		 				// return m;
		 			}
					else if(is_re_move)
					{
						// std::cout << "got a re_move\n calling Max_play again\n with depth value = " << c <<std::endl; //getchar();
						if(c == cutoffDepth-1)//if move was made on a leaf node
						{
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << "-Infinity" << std::endl;
						}
						else
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << "-Infinity" << std::endl;
						u = Max_play(board1, c, player, S, true, _inf);
						if(u.score > local_max)//for now worry about the value, the move part is to be used for stack trace
						{
							boardBackup(u.board , best_board);
							best_move = i;
							local_max = u.score;
						}
						for(int t = 0 ; t < arrSize ; t++) //reset the board for the next move
							board1[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << std::endl;
						}
					}
					else
					{
						// std::cout << "no re_move\n";
						// std::cout << "current depth : " << c << std::endl;
						// std::cout << "calling Min_play now with the following board: \n";
						// printBoard(board1);//getchar();
						if(c == cutoffDepth-1)//leaf node returned
						{
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << check_Val(evaluate(board1)) << std::endl;
						}
						else
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << "Infinity" << std::endl;
						u = Min_play(board1, c+1, 2, S, false, inf);
						if(u.score > local_max)
						{
							boardBackup(board1 , best_board);
							best_move = i;
							local_max = u.score;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board1[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << std::endl;
						}
				    }
				}
			}
		}
		else
		{
			for(int i = arrSize-2 ; i > n_pits ; i--)
			{
				if(board1[i] != 0)
				{
					move m(i , inf);					
					// std::cout << "Max_play Made this move on this board :\n";
					// printBoard(board1); //getchar();
		 			bool is_re_move = makeMove(board1, m, player);
					// std::cout << "(A" << arrSize-i << -1000000 << "), " << c << std::endl;
					S = "A" + std::to_string(arrSize-i);// + "," + std::to_string(c);// + "\n";
		 			// std::cout << "After move: \n";
		 			int type = check_board(board1);//passed by reference. so, the board is changed to the 000 board

		 			// printBoard(board1);//getchar();
		 			move u(0,0);
		 			if(type == 1)
		 			{
		 				endgamemove(board1);// moves all the remaining rocks into the respectvive players mancala
		 				//rocks have been moved off the board into the respective player's mancalas
		 				int util = evaluate(board1);
		 				if (is_re_move)
		 				{
			 				// o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << "-Infinity" << std::endl;
			 				o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << check_Val(util) << std::endl;
 		 				}
 		 				else
 		 				{
 		 					// o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << "Infinity" << std::endl;
 		 					o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << check_Val(util) << std::endl;
 		 				}
 		 				if(util > local_max)
 		 					local_max = util;
 		 				if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << std::endl;
						}
		 				move m(-1100000, evaluate(board1), board1);
		 				// return m;
		 			}
					else if(is_re_move)
					{
						// std::cout << "got a re_move\n calling Max_play again\n with depth value = " << c <<std::endl; //getchar();
						if(c == cutoffDepth-1)//if move was made on a leaf node
						{
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << "-Infinity" << std::endl;
						}
						else
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << "-Infinity" << std::endl;
						u = Max_play(board1 , c, player, S ,true, _inf);
						if(u.score > local_max)//for now worry about the value, the move part is to be used for stack trace
						{
							boardBackup(u.board , best_board);
							best_move = i;
							local_max = u.score;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board1[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << std::endl;
						}
					}
					else
					{
						// std::cout << "no re_move\n";
						// std::cout << "current depth : " << c << std::endl;	
						// std::cout << "calling Min_play now with the following board: \n";
						// printBoard(board1);//getchar();
						if(c == cutoffDepth-1)//leaf node returned
						{
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << check_Val(evaluate(board1)) << std::endl;
						}
						else
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << "Infinity" << std::endl;
						u = Min_play(board1, c+1, 1, S, false, inf);
						if(u.score > local_max)
						{
							boardBackup(board1 , best_board);
							best_move = i;
							local_max = u.score;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board1[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << std::endl;
						}
				    }  
				}					
			}
		}
		move a = move( best_move, local_max, best_board);
		return a; //take care of this in the kickoff function
	}
	else if(c == cutoffDepth)
	{
		// std::cout << "at Cutoff depth in Max_play.\n";
		int util = evaluate(board1);
		// o_file << "," << util  << std::endl;
		// std::cout << "evaluated value = " << util << std::endl;
		// std::cout << "On this board: \n";
		// printBoard(board1);
		move m(-1000000, util, board1);
		return m;
	}
	//depth is more than the cutoff value // dont let the control get here. 
	//don't know how to handle this case
	move p(-44444, -44444);
	return p;
}



move mancala::Min_play(int* b , int count, int player, std::string parentMove, bool re_move, int local)
{
	int best_board[22] = { };
	int local_min = local;
	int least_move;
	int k = count;
	int board2[22] = { };
	bool flip = true;
	std::string Q;
	for(int t = 0 ; t < arrSize ; t++)
		board2[t] = b[t];

	if(k < cutoffDepth)
	{
		if(player == 2)
		{
			for(int i = arrSize-2 ; i > n_pits ; i-- )
			{
				if(board2[i] != 0)
				{
					move n(i , _inf);
					// std::cout << "Min_play Made this move on this board :\n";
					
					// printBoard(board2); //getchar();
					bool is_re_move = makeMove(board2, n, 2);
					// std::cout << "(A" << arrSize-i << -1000000 << "), " << k << std::endl;
					Q = "A" + std::to_string(arrSize-i);// + "," + std::to_string(k);// + "\n";
					// std::cout << "After move: \n";
					// printBoard(board2); //getchar();

					int type = check_board(board2);

					move l(0,0);
					int flip = 9;
					if(type == 1)
		 			{
		 				endgamemove(board2);// moves all the remaining rocks into the respectvive players mancala
		 				//rocks have been moved off the board into the respective player's mancalas
		 				int util = evaluate(board2);
		 				if (is_re_move)
		 				{
			 				// o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << "Infinity" << std::endl;
			 				o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << check_Val(util) << std::endl;
 		 				}
 		 				else
 		 				{
 		 					// o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << "-Infinity" << std::endl;
 		 					o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << check_Val(util) << std::endl;
 		 				}
 		 				if(util < local_min)
 		 					local_min = util;
 		 				if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << std::endl;
						}
		 				move m(-1100000, evaluate(board2), board2);
		 				// return m;
		 			}
					else if(is_re_move)
					{
						// std::cout << "got a re_move\n calling Min_play again\n with depth value = " << k <<std::endl; //getchar();
						if(k == cutoffDepth-1)
						{
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << "Infinity" << std::endl;
						}
						else
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << "Infinity" << std::endl;
						
						l = Min_play(board2, k, player, Q, true, inf);
						if(l.score < local_min)
						{
							boardBackup(l.board , best_board);
							least_move = i;
							local_min = l.score;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board2[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << std::endl;
						}
					}
					else
					{
						// std::cout << "no re_move\n";
						// std::cout << "current depth : " << k << std::endl;	
						// // std::cout << "calling Max_play now with the following board: \n";
						// printBoard(board2); //getchar();
						if(k == cutoffDepth-1)
						{
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << check_Val(evaluate(board2)) << std::endl;
						}
						else
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << "-Infinity" << std::endl;
						l = Max_play(board2, k+1, 1, Q, false, _inf);
						if(l.score < local_min)
						{
							boardBackup(board2 , best_board);
							least_move = i;
							local_min = l.score;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board2[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << std::endl;
						}
					}
				}
			}
		}
		else
		{
			for(int i = 0 ; i < n_pits ; i++ )
			{
				if(board2[i] != 0)
				{
					move n(i ,_inf);
					// std::cout << "Min_play Made this move on this board :\n";
					// printBoard(board2); //getchar();
					bool is_re_move = makeMove(board2, n, 1);
					// std::cout << "(B" << i+2 << -1000000 << "), " << k << std::endl;
					Q = "B" + std::to_string(i+2);// + "," + std::to_string(k);// + "\n";
					// std::cout << "After move: \n";
					int type = check_board(board2);

					// printBoard(board2); //getchar();
					move l(0,0);

					if(type == 1)
		 			{
		 				endgamemove(board2);// moves all the remaining rocks into the respectvive players mancala
		 				//rocks have been moved off the board into the respective player's mancalas
		 				int util = evaluate(board2);
		 				if (is_re_move)
		 				{
			 				// o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << "-Infinity" << std::endl;
			 				o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << check_Val(util) << std::endl;
 		 				}
 		 				else
 		 				{
 		 					// o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << "Infinity" << std::endl;
 		 					o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << check_Val(util) << std::endl;
 		 				}
 		 				if(util < local_min)
 		 					local_min = util;
 		 				if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << std::endl;
						}
		 				move m(-1100000, evaluate(board2), board2);
		 				// return m;
		 			}


					else if(is_re_move)
					{
						flip = false;
						// std::cout << "got a re_move\n calling Min_play again\n with depth value = " << k <<std::endl; //getchar();
						if(k == cutoffDepth-1)
						{
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << "Infinity" << std::endl;
						}
						else
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << "Infinity" << std::endl;
						l = Min_play(board2, k, player, Q, true, inf);
						if(l.score < local_min)
						{
							boardBackup(l.board , best_board);
							least_move = i;
							local_min = l.score;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board2[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << std::endl;
						}
					}
					else
					{
						// std::cout << "no re_move\n";
						// std::cout << "current depth : " << k << std::endl;	
						// std::cout << "calling Max_play now with the following board: \n";
						// printBoard(board2); //getchar();
						if(k == cutoffDepth-1)
						{
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << check_Val(evaluate(board2)) << std::endl;
						}
						else
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << "-Infinity" << std::endl;
						l = Max_play(board2, k+1, 2, Q, false, _inf);
						if(l.score < local_min)
						{
							boardBackup(l.board , best_board);
							least_move = i;
							local_min = l.score;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board2[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << std::endl;
						}
					}
				}
			}
		}
		move b( least_move, local_min, best_board);
		return b;
	}
	else if(k == cutoffDepth)
	{
		int utility = evaluate(board2);
		// o_file << "," << utility << std::endl;
		// std::cout << " evaluated Ulitilty value = " << utility << std::endl;
		// std::cout << "On this board: \n";
		// printBoard(board2); 
		move n(-1000000, utility, board2);
		return n;
	}
	//if the depth is greater than the cutoffvalue // make sure loop never gets here!!!
	//don't know how to handle this case.

	move p(-44444, -44444);
	return p;
}
 ////////////////////////////////////
 ////

void mancala::abminimaxUtil()
{
	//whoStarts is Max player so, he makes the first move
	// std::cout << "Inside the util now\n";
	o_file.open("traverse_log.txt");
	o_file << "Node,Depth,Value,Alpha,Beta\n";
	o_file << "root,0,-Infinity,-Infinity,Infinity\n";
	move v = abMax_play(board, 0, whoStarts, "root", false, _inf, _inf, inf);
	// o_file << "best move is " << v.pitNumber << " and it's value is " << v.score << std::endl; 
	// o_file << "final board \n";
	// for(int i =0; i< 22; i++)
	// 	o_file << v.board[i] << " ";
	o_file.close();
	o_file_state.open("next_state.txt");
	// o_file_state << "hell";
	for(int i = arrSize-2; i>n_pits; i--)
	{
		o_file_state << v.board[i];
		if (i!=n_pits+1)
			o_file_state << " ";
	}
	o_file_state << "\n";
	for(int i = 0; i<n_pits ;i++)
	{
		o_file_state << v.board[i];
		if (i!=n_pits-1)
			o_file_state << " ";
	}
	o_file_state << "\n";
	o_file_state << v.board[arrSize-1] << "\n";
	o_file_state << v.board[n_pits] << "\n";

	o_file_state.close();
}

move mancala::abMax_play(int * b, int depth, int player, std::string parentMove, bool re_move, int local, int alpha, int beta)
{
	int c = depth;
	int best_move;
	int best_board[22] = { };
	int local_max = local;
	int local_alpha = alpha, local_beta = beta;
	int board1[22] = { };
	bool flop = true;
	std::string S;
	for(int t = 0 ; t < arrSize ; t++)// saving the board contents before we run any moves
		board1[t] = b[t];
	if(c < cutoffDepth)
	{
		// std::cout << "Depth less than cutoff depth\n";
		if(player == 1)
		{
			for(int i = 0 ; i < n_pits ; i++ )
			{
				if(board1[i] != 0)
				{
					move m(i ,inf);
					// std::cout << "Max_play Made this move on this board :\n";
					// printBoard(board1); //getchar();
		 			bool is_re_move = makeMove(board1, m, player);
					// std::cout << "(B" << i+2 << -1000000 << "), " << c+1 << std::endl;
					S = "B" + std::to_string(i+2);// + "," + std::to_string(c);// + "\n";
		 			// std::cout << "After move: \n";

		 			int type = check_board(board1);

		 			// printBoard(board1);//getchar();
		 			move u(0,0); // the move returned by the recursive functions is stored here

		 			if(type == 1)
		 			{
		 				int temp;
		 				endgamemove(board1);// moves all the remaining rocks into the respectvive players mancala
		 				//rocks have been moved off the board into the respective player's mancalas
		 				if (is_re_move)
		 				{
			 				// o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << "-Infinity" << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
			 				int util_val = evaluate(board1);
			 				if(util_val > local_max)//for now worry about the value, the move part is to be used for stack trace
							{
								best_move = i;
								boardBackup(board1, best_board);
								local_max = util_val;
							}
							temp = local_alpha;
							if( util_val > local_alpha )//&& !(local_beta <= u.score))
								local_alpha = util_val;
			 				o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << check_Val(evaluate(board1)) << "," << check_Val(alpha)  << "," << check_Val(beta) << std::endl;
 		 				}
 		 				else
 		 				{
			 				// o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << "Infinity" << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
			 				int util_val = evaluate(board1);
			 				if(util_val > local_max)//for now worry about the value, the move part is to be used for stack trace
							{
								best_move = i;
								boardBackup(board1, best_board);
								local_max = util_val;
							}
							temp = local_alpha;
							if( util_val < local_alpha )//&& !(local_beta <= u.score))
								local_alpha = util_val;
			 				o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << check_Val(evaluate(board1)) << "," << check_Val(alpha)  << "," << check_Val(beta) << std::endl;
 		 				}
 		 				if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							// return u;
						}
 		 				else
 		 				{
	 		 				if(re_move)
							{
								o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
							}
						}
		 				move m(-1100000, evaluate(board1), board1);
		 				// return m;
		 			}

					else if(is_re_move)
					{
						// std::cout << "got a re_move\n calling Max_play again\n with depth value = " << c <<std::endl; //getchar();
						if(c == cutoffDepth-1)//if move was made on a leaf node
						{
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << check_Val(_inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << check_Val(_inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						u = abMax_play(board1, c, player, S, true, _inf, local_alpha, local_beta);
						if(u.score > local_max)//for now worry about the value, the move part is to be used for stack trace
						{
							best_move = i;
							boardBackup(u.board, best_board);
							local_max = u.score;
						}
						int temp = local_alpha;
						if( u.score > local_alpha )//&& !(local_beta <= u.score))
							local_alpha = u.score;
						if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							return u;
						}
						for(int t = 0 ; t < arrSize ; t++) //reset the board for the next move
							board1[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
					}
					else
					{
						// std::cout << "no re_move\n";
						// std::cout << "current depth : " << c << std::endl;	
						// std::cout << "calling Min_play now with the following board: \n";
						// printBoard(board1);//getchar();
						if(c == cutoffDepth-1)//leaf node returned
						{
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << check_Val(evaluate(board1)) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(c+1) << "," << check_Val(inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						u = abMin_play(board1, c+1, 2, S, false, inf, local_alpha, local_beta);
						if(u.score > local_max)
						{
							local_max = u.score;
							boardBackup(board1, best_board);
							best_move = i;
						}
						int temp = local_alpha;
						if( u.score > local_alpha )//&& !(local_beta <= u.score))
							local_alpha = u.score;
						if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							return u;
						}

						for(int t = 0 ; t < arrSize ; t++)
							board1[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
				    }
				}
			}
		}
		else
		{
			for(int i = arrSize-2 ; i > n_pits ; i--)
			{
				if(board1[i] != 0)
				{
					move m(i , inf);					
					// std::cout << "Max_play Made this move on this board :\n";
					// printBoard(board1); //getchar();
		 			bool is_re_move = makeMove(board1, m, player);
					// std::cout << "(A" << arrSize-i << -1000000 << "), " << c << std::endl;
					S = "A" + std::to_string(arrSize-i);// + "," + std::to_string(c);// + "\n";
		 			// std::cout << "After move: \n";
		 			// printBoard(board1);//getchar();
		 			int type = check_board(board1);
		 			move u(0,0);

					if(type == 1)
		 			{
		 				int temp;
		 				endgamemove(board1);// moves all the remaining rocks into the respectvive players mancala
		 				//rocks have been moved off the board into the respective player's mancalas
		 				if (is_re_move)
		 				{
			 				// o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << "-Infinity" << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
			 				int util_val = evaluate(board1);
			 				if(util_val > local_max)//for now worry about the value, the move part is to be used for stack trace
							{
								best_move = i;
								boardBackup(board1, best_board);
								local_max = util_val;
							}
							temp = local_alpha;
							if( util_val > local_alpha )//&& !(local_beta <= u.score))
								local_alpha = util_val;
			 				o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << check_Val(evaluate(board1)) << "," << check_Val(alpha)  << "," << check_Val(beta) << std::endl;
 		 				}
 		 				else
 		 				{
			 				// o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << "Infinity" << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
			 				int util_val = evaluate(board1);
			 				if(util_val > local_max)//for now worry about the value, the move part is to be used for stack trace
							{
								best_move = i;
								boardBackup(board1, best_board);
								local_max = util_val;
							}
							temp = local_alpha;
							if( util_val > local_alpha )//&& !(local_beta <= u.score))
								local_alpha = util_val;
			 				o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << check_Val(evaluate(board1)) << "," << check_Val(alpha)  << "," << check_Val(beta) << std::endl;
 		 				}
 		 				if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							// return u;
						}
 		 				else
 		 				{
	 		 				if(re_move)
							{
								o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
							}
						}
		 				move m(-1100000, evaluate(board1), board1);
		 				// return m;
		 			}

					else if(is_re_move)
					{
						// std::cout << "got a re_move\n calling Max_play again\n with depth value = " << c <<std::endl; //getchar();
						if(c == cutoffDepth-1)//if move was made on a leaf node
						{
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << check_Val(_inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << check_Val(_inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						u = abMax_play(board1 , c, player, S ,true, _inf, local_alpha, local_beta);
						if(u.score > local_max)//for now worry about the value, the move part is to be used for stack trace
						{
							local_max = u.score;
							boardBackup(u.board, best_board);
							best_move = i;
						}
						int temp = local_alpha;
						if( u.score > local_alpha)
							local_alpha = u.score;
						if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							return u;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board1[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
					}
					else
					{
						// std::cout << "no re_move\n";
						// std::cout << "current depth : " << c << std::endl;	
						// std::cout << "calling Min_play now with the following board: \n";
						// printBoard(board1);//getchar();
						if(c == cutoffDepth-1)//leaf node returned
						{
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << check_Val(evaluate(board1)) <<  "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(c+1) << "," << check_Val(inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta)<< std::endl;
						u = abMin_play(board1, c+1, 1, S, false, inf, local_alpha, local_beta);
						if(u.score > local_max)
						{
							local_max = u.score;
							best_move = i;
							boardBackup(board1, best_board);
						}
						int temp = local_alpha;
						if( u.score > local_alpha )//&& !(local_beta <= u.score))
							local_alpha = u.score;
						if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max)  << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max)  << "," << check_Val(temp) << "," << check_Val(local_beta) << std::endl;
							}
							return u;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board1[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(c+1) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(c) << "," << check_Val(local_max) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
				    }  
				}					
			}
		}
		move a = move( -999999, local_max, best_board);
		return a; //take care of this in the kickoff function
	}
	else if(c == cutoffDepth)
	{
		// std::cout << "at Cutoff depth in Max_play.\n";
		int util = evaluate(board1);
		// o_file << "," << util  << std::endl;
		// std::cout << "evaluated value = " << util << std::endl;
		// std::cout << "On this board: \n";
		// printBoard(board1);
		move m(-1000000, util, board1);
		return m;
	}
	//depth is more than the cutoff value // dont let the control get here. 
	//don't know how to handle this case
	move p(-44444, -44444);
	return p;
}



move mancala::abMin_play(int* b , int count, int player, std::string parentMove, bool re_move, int local, int alpha, int beta)
{
	int local_min = local;
	int best_move;
	int best_board[22] = { };
	int local_alpha = alpha, local_beta = beta;
	int k = count;
	int board2[22] = { };
	bool flip = true;
	std::string Q;
	for(int t = 0 ; t < arrSize ; t++)
		board2[t] = b[t];

	if(k < cutoffDepth)
	{
		if(player == 2)
		{
			for(int i = arrSize-2 ; i > n_pits ; i-- )
			{
				if(board2[i] != 0)
				{
					move n(i , _inf);
					// std::cout << "Min_play Made this move on this board :\n";
					
					// printBoard(board2); //getchar();
					bool is_re_move = makeMove(board2, n, 2);
					// std::cout << "(A" << arrSize-i << -1000000 << "), " << k << std::endl;
					Q = "A" + std::to_string(arrSize-i);// + "," + std::to_string(k);// + "\n";
					// std::cout << "After move: \n";
					// printBoard(board2); //getchar();
					int type = check_board(board2);
					move l(0,0);
					int flip = 9;

					if(type == 1)
		 			{
		 				int temp;
		 				endgamemove(board2);// moves all the remaining rocks into the respectvive players mancala
		 				//rocks have been moved off the board into the respective player's mancalas
		 				if (is_re_move)
		 				{
			 				// o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << "Infinity" << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
			 				int util_val = evaluate(board2);
			 				if(util_val < local_min)
							{
								local_min = util_val; // -2 -2 -1
								best_move = i;
								boardBackup(board2, best_board);
							}
							temp = local_beta;
							if( util_val < local_beta )//&& !(local_beta <= u.score))
								local_beta = util_val;
			 				o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << check_Val(evaluate(board2)) << "," << check_Val(alpha)  << "," << check_Val(beta) << std::endl;
 		 				}
 		 				else
 		 				{
			 				// o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << "-Infinity" << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
			 				int util_val = evaluate(board2);
			 				if(util_val < local_min)
							{
								local_min = util_val; // -2 -2 -1
								best_move = i;
								boardBackup(board2, best_board);
							}
							temp = local_beta;
							if( util_val < local_beta )//&& !(local_beta <= u.score))
								local_beta = util_val;
			 				o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << check_Val(evaluate(board2)) << "," << check_Val(alpha)  << "," << check_Val(beta) << std::endl;
 		 				}
 		 				if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							// return l;
						}
 		 				else
 		 				{
	 		 				if(re_move)
							{
								o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
							}
						}
		 				move m(-1100000, evaluate(board2), board2);
		 				// return m;
		 			}


					else if(is_re_move)
					{
						// std::cout << "got a re_move\n calling Min_play again\n with depth value = " << k <<std::endl; //getchar();
						if(k == cutoffDepth-1)
						{
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << check_Val(inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << check_Val(inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						
						l = abMin_play(board2, k, player, Q, true, inf, local_alpha, local_beta);// inf , -2 , -1
						if(l.score < local_min)
						{
							local_min = l.score; // -2 -2 -1
							best_move = i;
							boardBackup(l.board, best_board);
						}
						int temp = local_beta;
						if( l.score < local_beta)// && !(l.score <= local_alpha) )
						{
							local_beta = l.score; // -2 -2 -2
						}
						if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							return l;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board2[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
					}
					else
					{
						// std::cout << "no re_move\n";
						// std::cout << "current depth : " << k << std::endl;	
						// std::cout << "calling Max_play now with the following board: \n";
						// printBoard(board2); //getchar();
						if(k == cutoffDepth-1)
						{
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << check_Val(evaluate(board2)) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
							o_file << "A" << std::to_string(arrSize-i) << "," << std::to_string(k+1) << "," << check_Val(_inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						l = abMax_play(board2, k+1, 1, Q, false, _inf, local_alpha, local_beta);
						if( l.score < local_min )
						{
							local_min = l.score;
							best_move = i;
							boardBackup(board2, best_board);
						}
						int temp = local_beta;
						if( l.score < local_beta )//&& !(l.score <= local_alpha) )
							local_beta = l.score;
						if( local_beta <= local_alpha)
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							return l;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board2[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
					}
				}
			}
		}
		else
		{
			for(int i = 0 ; i < n_pits ; i++ )
			{
				if(board2[i] != 0)
				{
					move n(i ,_inf);
					// std::cout << "Min_play Made this move on this board :\n";
					// printBoard(board2); //getchar();
					bool is_re_move = makeMove(board2, n, 1);
					// std::cout << "(B" << i+2 << -1000000 << "), " << k << std::endl;
					Q = "B" + std::to_string(i+2);// + "," + std::to_string(k);// + "\n";
					// std::cout << "After move: \n";
					// printBoard(board2); //getchar();
					int type = check_board(board2);
					move l(0,0);


					if(type == 1)
		 			{
		 				int temp;
		 				endgamemove(board2);// moves all the remaining rocks into the respectvive players mancala
		 				//rocks have been moved off the board into the respective player's mancalas
		 				if (is_re_move)
		 				{
			 				// o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << "-Infinity" << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
			 				int util_val = evaluate(board2);
			 				if(util_val < local_min)
							{
								local_min = util_val; // -2 -2 -1
								best_move = i;
								boardBackup(board2, best_board);
							}
							temp = local_alpha;
							if( util_val < local_beta )//&& !(local_beta <= u.score))
								local_beta = util_val;
			 				o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << check_Val(evaluate(board2)) << "," << check_Val(alpha)  << "," << check_Val(local_beta) << std::endl;
 		 				}
 		 				else
 		 				{
			 				// o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << "Infinity" << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
			 				int util_val = evaluate(board2);
			 				if(util_val < local_min)
							{
								local_min = util_val; // -2 -2 -1
								best_move = i;
								boardBackup(board2, best_board);
							}
							temp = local_alpha;
							if( util_val < local_beta )//&& !(local_beta <= u.score))
								local_beta = util_val;
			 				o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << check_Val(evaluate(board2)) << "," << check_Val(alpha)  << "," << check_Val(local_beta) << std::endl;
 		 				}
 		 				if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							// return l;
						}
 		 				else
 		 				{
	 		 				if(re_move)
							{
								o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
							}
						}
		 				move m(-1100000, evaluate(board2), board2);
		 				// return m;
		 			}



					else if(is_re_move)
					{
						flip = false;
						// std::cout << "got a re_move\n calling Min_play again\n with depth value = " << k <<std::endl; //getchar();
						if(k == cutoffDepth-1)
						{
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << check_Val(inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << check_Val(inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						l = abMin_play(board2, k, player, Q, true, inf, local_alpha, local_beta);
						if(l.score < local_min)
						{
							local_min = l.score;
							best_move = i;
							boardBackup(l.board, best_board);
						}
						int temp = local_beta;
						if( l.score < local_beta)// && !(l.score <= local_alpha) )
						{
							local_beta = l.score; // -2 -2 -2
						}
						if( local_beta <= local_alpha )
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							return l;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board2[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
					}
					else
					{
						// std::cout << "no re_move\n";
						// std::cout << "current depth : " << k << std::endl;	
						// std::cout << "calling Max_play now with the following board: \n";
						// printBoard(board2); //getchar();
						if(k == cutoffDepth-1)
						{
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << check_Val(evaluate(board2)) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
							o_file << "B" << std::to_string(i+2) << "," << std::to_string(k+1) << "," << check_Val(_inf) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						l = abMax_play(board2, k+1, 2, Q, false, _inf, local_alpha, local_beta);
						if(l.score < local_min)
						{
							best_move = i;
							boardBackup(board2, best_board);
							local_min = l.score;
						}
						int temp = local_beta;
						if( l.score < local_beta )//&& !(l.score <= local_alpha) )
							local_beta = l.score;
						if( local_beta <= local_alpha)
						{
							if(re_move)
							{
								o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							else
							{
								o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(temp) << std::endl;
							}
							return l;
						}
						for(int t = 0 ; t < arrSize ; t++)
							board2[t] = b[t];
						if(re_move)
						{
							o_file << parentMove << "," << std::to_string(k+1) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
						else
						{
							o_file << parentMove << "," << std::to_string(k) << "," << check_Val(local_min) << "," << check_Val(local_alpha) << "," << check_Val(local_beta) << std::endl;
						}
					}
				}
			}
		}
		move b( -999999, local_min, best_board);
		return b;
	}
	else if(k == cutoffDepth)
	{
		int utility = evaluate(board2);
		// o_file << "," << utility << std::endl;
		// std::cout << " evaluated Ulitilty value = " << utility << std::endl;
		// std::cout << "On this board: \n";
		// printBoard(board2); 
		move n(-1000000, utility, board2);
		return n;
	}
	//if the depth is greater than the cutoffvalue // make sure loop never gets here!!!
	//don't know how to handle this case.

	move p(-44444, -44444);
	return p;
}

// void mancala::printBoard( int * b)
// {
// 	for(int i = arrSize - 1; i>=n_pits+1; i--)
// 	{
// 		std::cout << b[i] << " | ";
// 	}
// 	std::cout << std::endl;
// 	for(int j=0; j<=n_pits; j++)
// 	{
// 		std::cout << "----";
// 	}
// 	std::cout << std::endl;
// 	for(int i = 0; i<=n_pits; i++)
// 	{
// 		std::cout << b[i] << " | ";
// 	}
// 	std::cout << std::endl;
// 	// std::cout << "\nwhoStarts = Player " << whoStarts;
// 	// std::cout << "\ncutoffDepth = " << cutoffDepth;
// 	// std::cout << "\nn_pits(without the mancala) = " << n_pits;
// 	// std::cout << "\narrSize = " << arrSize << std::endl;
// 	std::cout << "-----------------------------------------\n";
// }

mancala::mancala(std::string startPlayer, std::string cutoff, int size)
{
	whoStarts = atoi(startPlayer.c_str());
	cutoffDepth = atoi(cutoff.c_str());
	n_pits = size + 1;
	arrSize = 2*(n_pits + 1);
	// board = new int[arrSize]; //no longer a dynamic array
}

void mancala::setMancala(std::string str, int n)
{
	if(n==1)
	{
		board[n_pits] = stoi(str);
	}
	if(n==2)
	{
		board[2*n_pits + 1] = stoi(str);
	}
}

void mancala::setState(std::string str, int n)
{
	std::stringstream ss(str);
	if(n==2)
	{
		for(int i = arrSize - 2; i > n_pits; i--)
			ss >> board[i];
	}
	if(n==1)
	{
		for(int i = 0; i < n_pits; i++ )
			ss >> board[i];
	}
}


int main (int argc, char* argv[])
{
	int pitCount;
	std::string STR, alg, toPlay, cutOff;
	std::ifstream i_file;
	// i_file.open("1");
	i_file.open(argv[2]);
	while(!i_file.eof())
	{
		getline(i_file, alg); //reading algorithm 
		getline(i_file, toPlay); //reading start player
		getline(i_file, cutOff); //reading cutoff depth
		getline(i_file, STR); // reading player 2's setup
		pitCount = std::count(STR.begin(), STR.end(), ' '); //to calculate the size of str
		mancala newMancala(toPlay, cutOff, pitCount); //creating a new class of mancala
		newMancala.setState(STR, 2); //setting player 2's setup
		getline(i_file, STR); //reading player 1's setup
		newMancala.setState(STR, 1); //setting player 1's setup
		getline(i_file, STR); //reading player 2's mancala
		newMancala.setMancala(STR, 2); //setting player 2's amancala
		getline(i_file, STR); //reading player 1's mancala
		newMancala.setMancala(STR, 1); //setting player 1's mancala
		int algo = stoi(alg);
		// std::cout << algo << " to be run on the following board\n";
		int* pitss = newMancala.board;
		// newMancala.printBoard(pitss);//board is set now call the correct function
		// getchar();
		if(algo == 1)//greedy
		{
			newMancala.cutoffDepth = 1;
			newMancala.minimaxUtil();
		}
		else if(algo == 2)//minimax
		{
			// std::cout << inf << " \n" << _inf <<"\nRunning MiniMax now: \n";
			newMancala.minimaxUtil();
		}
		else if(algo == 3)//alpha-beta
		{
			// std::cout << inf << " \n" << _inf <<"\nRunning alpha beta now: \n";
			newMancala.abminimaxUtil();
		}
		else //competition
		{

		}

		// bool f = newMancala.makeMove(newMancala.board, move(5 , 2), 2);
		// newMancala.printBoard(newMancala.board);
		getline(i_file, STR); //to get rid of the empty line between cases
		//call alg 
	}
	return 0;
}
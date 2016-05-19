#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>

// #define INF_POS 1000000
// #define INF_NEG -1000000

struct move
{
	int pitNumber;
	int score;
	move(int n, int s)
	{
		pitNumber = n;
		score = s;
	}
};

bool operator>(const move & lhs, const move & rhs)
{
	if(lhs.score > rhs.score)
		return true;
	else
		return false;
}

class mancala
{
public:
	int whoStarts, cutoffDepth, n_pits, arrSize;
	std::ofstream o_file;//pits keeps count of the number of pits each player has(other than his mancala)
	// std::vector <int> board;
	int board[22];//everything is stored here!
	void setMancala(std::string str, int n);
	void setState(std::string str, int n);
	mancala(std::string s, std::string p, int q);
	bool makeMove(int* board, move m, int player);//returns board. the getBestMove func has to handle the nextmove based on the state of the board 
	// move minimax(int* board, int player);
	move Max_play(int * b , int count);
	move Min_play(int * b , int count);
	void minimaxUtil();
	void boardBackup(int* a, int* b);
	int evaluate(int* b);
	void printBoard(int * b);
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
	std::cout << "Inside the util now\n";
	o_file.open("traverse_log.txt");
	o_file << "Node,Depth,Value\n";
	move v = Max_play(board , 0);
	o_file.close();
}

int mancala::evaluate(int* b)
{
	return (b[n_pits] - b[arrSize-1]);
}


move mancala::Max_play(int * b , int depth)
{
	int c = depth;
	// std::cout << "Depth now: " << c << std::endl;
	if(depth == 0)
	{
		o_file << "root," << depth << std::endl;
	}
	int board1[22] = { };
	for(int t = 0 ; t < arrSize ; t++)
		board1[t] = b[t];

	if(c < cutoffDepth)
	{
		// std::cout << "Depth less than cutoff depth\n";
		for(int i = 0 ; i < n_pits ; i++ )
		{
			if(board1[i] != 0)
			{
				move m(i , -1000000);
				// std::cout << "Move Pit number: " << i << std::endl;
				
				std::cout << "Max_play Made this move on this board :\n";
				printBoard(board1); getchar();
	 			bool is_re_move = makeMove(board1, m, 1);
	 			
	 			c++;

				std::cout << "(B" << i+2 << -1000000 << "), " << c << std::endl;
				
				o_file << "B" << i+2 << "," << c << "\n"; 

	 			std::cout << "After move: \n";
	 			printBoard(board1);getchar();
				if(is_re_move)
				{
					//make another move keeping depth the same
					std::cout << "got a re_move\n calling Max_play again\n with depth value = " << c <<std::endl; getchar();
					move l = Max_play(board1 , c-1);
					for(int t = 0 ; t < arrSize ; t++)
						board1[t] = b[t];
					c--;
				}
				else
				{
					std::cout << "no re_move\n";
					std::cout << "current depth : " << c << std::endl;	
					// c++;					std::cout << "depth after increment: " << c << std::endl;	
					std::cout << "calling Min_play now with the following board: \n";
					printBoard(board1);getchar();
					move u = Min_play(board1 , c);
					if(u.pitNumber == -1000000 || u.pitNumber == 999999)//this is a leaf node at cutoffdepth or beyond//reset board to the previous saved board and decrement the depth counter
					{
						for(int t = 0 ; t < arrSize ; t++)
							board1[t] = b[t];
						c--;
					}
			    }
			}
		}
		move a = move( -999999, 1);
		return a;
	}
	else if(c == cutoffDepth)
	{
		std::cout << "at Cutoff depth in Max_play.\n";
		int util = evaluate(board1);
		std::cout << "evaluated value = " << util << std::endl;
		std::cout << "On this board: \n";
		printBoard(board1);
		move m(-1000000, util);
		return m;
	}
	//depth is more than the cutoff value // dont let the control get here. 
	//don't know how to handle thisn case
	move p(-44444, -44444);
	return p;
}



move mancala::Min_play(int* b , int count)
{
	int board2[22] = { };
	for(int t = 0 ; t < arrSize ; t++)
		board2[t] = b[t];

	int k = count;
	if(k < cutoffDepth)
	{
		for(int i = arrSize-2 ; i > n_pits ; i-- )
		{
			if(board2[i] != 0)
			{
				move n(i , +1000000);
				std::cout << "Min_play Made this move on this board :\n";
				
				printBoard(board2); getchar();
				bool is_re_move = makeMove(board2, n, 2);
				k++;

				std::cout << "(A" << arrSize-i << -1000000 << "), " << k << std::endl;
				o_file << "A" << arrSize-i << "," << k << "\n";


				std::cout << "After move: \n";
				printBoard(board2); getchar();
				if(is_re_move)
				{
					std::cout << "got a re_move\n calling Min_play again\n with depth value = " << k <<std::endl; getchar();
					move o = Min_play(board2, k-1);
					for(int t = 0 ; t < arrSize ; t++)
						board2[t] = b[t];
					k--;

					//return o;
				}
				else
				{
					std::cout << "no re_move\n";
					std::cout << "current depth : " << k << std::endl;	
					// k++; 					std::cout << "depth after increment: " << k << std::endl;	
					std::cout << "calling Max_play now with the following board: \n";
					printBoard(board2); getchar();
					move l = Max_play(board2, k);
					if (l.pitNumber == -1000000 || l.pitNumber == 999999)//returned a leaf node //we need to restore the board to the saved board state initially 
					{
						for(int t = 0 ; t < arrSize ; t++)
							board2[t] = b[t];
						k--;
					}
					
				}
			}
		}
		// for(int t = 0 ; t < arrSize ; t++)//no more possible moves to make so, we reset the board, decrement the depth counter and return the move
		// 	board2[t] = b[t];
		// k--;
		move b = move( 999999, 1);
		return b;
	}
	else if(k == cutoffDepth)
	{
		int utility = evaluate(board2);
		std::cout << " evaluated Ulitilty value = " << utility << std::endl;
		std::cout << "On this board: \n";
		printBoard(board2); 
		move n(-1000000, utility);
		return n;
	}
	//if the depth is greater than the cutoffvalue // make sure loop never gets here!!!
	//don't know how to handle this case.

	move p(-44444, -44444);
	return p;
}


void mancala::printBoard( int * b)
{
	for(int i = arrSize - 1; i>=n_pits+1; i--)
	{
		std::cout << b[i] << " | ";
	}
	std::cout << std::endl;
	for(int j=0; j<=n_pits; j++)
	{
		std::cout << "----";
	}
	std::cout << std::endl;
	for(int i = 0; i<=n_pits; i++)
	{
		std::cout << b[i] << " | ";
	}
	std::cout << std::endl;
	// std::cout << "\nwhoStarts = Player " << whoStarts;
	// std::cout << "\ncutoffDepth = " << cutoffDepth;
	// std::cout << "\nn_pits(without the mancala) = " << n_pits;
	// std::cout << "\narrSize = " << arrSize << std::endl;
	std::cout << "-----------------------------------------\n";
}

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
		board[n_pits] = atoi(str.c_str());
	}
	if(n==2)
	{
		board[2*n_pits + 1] = atoi(str.c_str());
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
	i_file.open("input_2.txt");
	// i_file.open(argv[2]);
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
		std::cout << alg << " to be run on the following board\n";
		int* pitss = newMancala.board;
		newMancala.printBoard(pitss);//board is set now call the correct function
		if(alg == "1")//greedy
		{

		}
		else if(alg == "2")//minimax
		{
			std::cout << "Running MiniMax now: \n";
			newMancala.minimaxUtil();
		}
		else if(alg == "3")//alpha-beta
		{

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
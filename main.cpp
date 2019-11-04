// Project 2 - Checkers Game
// Course: CS 5346 - Advanced Artificial Intelligence
// Authors: Aishwarya Baskaran, Philip Fitzgerald, Prajna Saha

/* ------------------------------------------------------------------------------------ */

#define NDEBUG
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <string>


using std::getline;
using std::ifstream;
using std::list;
using std::string;
using std::stringstream;
using std::cin;
using std::cout;
using std::endl;
using std::max;
using std::min;
using std::list;
using std::toupper;



//*****************************************************BOARD*****************************************************//
class jump
{
    //previous jump
    jump* prev;
    
    //the piece jumping
    char jumpingPiece;
    
    //when there are no next moves, noNext is true
    bool noNext;
    
    //used to keep track of how many times jump was inserted into a move,
    //increment every time jump is concatenated to a move
    //prevents double freeing of memory
    int numTimes;
    
    //character jumped over
    char c;
    
    //start point's x and y coordinate
    int xs;
    int ys;
    
    //jumped character's point coordinates
    int x;
    int y;
    
    //end point of the jump
    int xend;
    int yend;
    
    int key;
    
    //constructor for each data value
    jump(char jpingp, char piece, int xs, int ys, int xc, int yc, int xe, int ye, jump* p, int k):
    prev(p), jumpingPiece(jpingp), noNext(true), numTimes(0), c(piece), xs(xs), ys(ys),
    x(xc), y(yc), xend(xe), yend(ye), key(k){}
    
    //---------------------------------------------------------------------------------
    //friend classes:
    //---------------------------------------------------------------------------------
    //pointers to jump are deleted in move's destructor
    friend class move;
    //board's members create jumps
    friend class board;
};

class move
{
    //moving piece
    char mP;
    
    //start point's coordinates
    int xi;
    int yi;
    
    //end point's coordinates
    int xf;
    int yf;
    
    //command used to map a string to the move
    //used for when the player enters a string for a move
    std::string command;
    
    //list storing all the jumps made by the move
    std::list<jump*> jpoints;
    
    //constructor for move
    move(char c, int xs, int ys, int xe, int ye): mP(c), xi(xs), yi(ys), xf(xe), yf(ye) {}
    
    //destructor for move, found in board.cpp
    //frees all the heap allocated memory for the jumps in jpoints
    ~move();
    
    //---------------------------------------------------------------------------------
    //friend classes:
    //---------------------------------------------------------------------------------
    //board creates moves and it accesses many of move's members
    friend class board;
    //move's member: command is accessed in game::outputMessage()
    friend class game;
};

class board
{
    //first coordinate is x, second is y
    //x is vertical down, y is horizontal
    //don't need an 8x8 array since only half the spaces are legal positions for pieces
    char arr[8][4];
    
    //'b' for black, 'r' for red
    //indicates who's turn it is
    char color;
    
    //[0] for black, [1] for red
    //default initialized to false since it's a static array
    static bool isComputer[2];
    
    //---------------------------------------------------------------------------------
    //functions for board creation, found in board.cpp:
    //---------------------------------------------------------------------------------
    //1: constructor for initializing an initial board
    //2: destructor deallocates memory for all the moves in mlist
    //3: copy constructor
    //   copies over all data values except the move list
    //   useful for creating new boards for each move in alpha-beta search
    //4: changeTurn(), called after a move is made
    //   called in game.cpp by alphabeta
    //   called by makeMove, which is found in boardPublic.cpp
    //   is inlined
    //5: converts a command stored in the form 2 3 3 2 -1 to (2,3) -> (3, 2)
    //   called in inputCommand in boardPrint.cpp
    //6: create a list of moves by calling this
    //   is called each time a new board gets created after a move is made
    //   called by evaluate, in boardPublic.cpp
    //   is inlined
    board();
    ~board();
    board(const board&);
    
    void changeTurn()
    {
        if (color == 'r')
            color = 'b';
        else
            color = 'r';
    }
    
    static void convertCommand(const std::string&);
    
    bool terminalTest()
    {
        if (!movesAvailable())
            return true;
        return false;
    }
    
    //---------------------------------------------------------------------------------
    //general functions used for moves and jumps, found in board.cpp
    //---------------------------------------------------------------------------------
    //called in the terminalTest function
    //if there are any jumps, they are added to move list
    //else if there are any moves, they are added to the move list
    //if there are no jumps or moves available, it returns false
    //is inlined
    bool movesAvailable()
    {
        if (jumpsAvailable())
            return true;
        if (listMoves())
            return true;
        return false;
    }
    
    //if a red piece 'r' reaches the red side's end, it becomes a red king 'R'
    //if a black piece 'b' reaches the black side's end, it becomes a black king 'B'
    //called at the end of the makeMove function, which is found in boardPublic.cpp
    //is inlined
    void handleKinging(const int& x, const int& y)
    {
        if (x == 0 && arr[x][y] == 'r')
            arr[x][y] = 'R';
        if (x == 7 && arr[x][y] == 'b')
            arr[x][y] = 'B';
    }
    
    //returns true if the position arr[i][j] is a valid position on the checker board
    //called by jumpConditions, which is found in boardJumps.cpp
    //called by createMove, which is found in boardMoves.cpp
    //is inlined
    bool isValidPos(int i, int j)
    {
        if (i >= 0 && i < 8 && j >= 0 && j < 4)
            return true;
        else return false;
    }
    
    //reset the board after game over
    //called by startup, which is in boardPublic.cpp
    void reset();
    
    //create a board from an input file:
    void modifyBoard(std::ifstream&);
    
    //function for fixing strings obtained via getline
    //eliminate the \r character in a string or the \n character
    //called by modifyBoard in board.cpp
    void remove_carriage_return(std::string&);
    
    //---------------------------------------------------------------------------------
    //functions for jumps: found in boardJumps.cpp
    //---------------------------------------------------------------------------------
    //createkey is inlined
    int createkey(int, int, int, int, int, int);
    int reverse(int);
    void createJump(std::list<jump*>&, char, int, int, int, int, int, int, jump*);
    void createJumpMove(std::list<jump*>&);
    void jumpAvailable(std::list<jump*>&, char c, int, int, jump*);
    bool jumpsAvailable();
    bool jumpConditions(int, int, int, int);
    
    //---------------------------------------------------------------------------------
    //functions for regular moves, found in boardMoves.cpp
    //---------------------------------------------------------------------------------
    void checkNeighbors(int&, int&);
    void createMove(const int&, const int&, int, int);
    bool listMoves();
    
    //---------------------------------------------------------------------------------
    //functions for printing, found in boardPrint.cpp
    //---------------------------------------------------------------------------------
    //converts a point to string form and appends it to command list for a move
    //called by createJumpMove in boardJumps.cpp
    //called by createMove in boardMoves.cpp
    void convert(const int&, const int&, std::string&);
    
    //used for printing out moves, converting the y coordinate in the matrix
    //to the coordinate on the expanded 8x8 board
    //called in printMoves in boardPrint.cpp
    int convertY(const int& x, const int& y);
    
    //prints out a row of the checkers board
    //called by boardPrint in boardPublic.cpp
    void printline(const int&, const std::string&, const std::string&);
    
    //change text color for red and black pieces
    void printcolor(const char&);
    //---------------------------------------------------------------------------------
    
    //modifies who is a computer, called by startup
    static void whoComputer();
    
    //-------------------------------------------------------------------------------------
    // FUNCTIONS AND MEMBERS UTILIZED DIRECTLY IN GAME.H:
    //-------------------------------------------------------------------------------------
    //timer for the computer
    static int timeLimit;
    
    //list of moves for the board:
    std::list<move*> mlist;
    
    //---------------------------------------------------------------------------------
    //functions found in boardPublic.cpp, functions called in game.cpp
    //---------------------------------------------------------------------------------
    //determines whether or not the current turn is a computer's turn
    //called to run alpha-beta search if necessary
    //is inlined
    bool isComputerTurn()
    {
        if (color == 'b' && isComputer[0])
            return true;
        if (color == 'r' && isComputer[1])
            return true;
        return false;
    }
    
    //prints out directions and available moves
    //need to add computer moves to it
    void inputCommand();
    
    //expands and prints board
    void printBoard();
    
    //prints moves in order listed in the list
    //called by inputCommand in boardPrint.cpp
    //called by game::computerTurn in game.cpp
    void printMoves();
    
    //makes the move
    //should be used on a copy of a board when alpha-beta searching
    void makeMove(move*);
    
    //reverses a move
    void undoMove(move*);
    
    //checks double corners and diagonals near end game
    //gives points for occupying a double corner for losing player
    //winning player gets points for occupying a diagonal near losing player's corner
    //called by evaluate() in boardPublic.cpp
    int cornerDiagonal(char, char);
    
    //Evaluation function, need to do
    //will be implemented in alpha-beta search
    int evaluate();
    
    //determines whether or not players will be a computer calls modifyBoard
    void startup();
    
    //gets the current color's turn
    //is inlined
    char getTurn()
    {
        return color;
    }
    
    //---------------------------------------------------------------------------------
    //friend classes:
    //---------------------------------------------------------------------------------
    //game accesses many of move's functions (see above divider)
    friend class game;
    //sptr is a smart pointer class that automatically manages memory for boards created on the heap
    template <class T> friend class sptr;
};

bool board::isComputer[2];

int board::timeLimit = 0;


//frees the memory allocated on the heap for each jump pointer
//avoids double freeing of memory by keeping track of the
//number of times each jump was added to a moves jump list
//first decrements each jump's numTimes
//only deletes the jump if numTimes equals 0
//this is necessary for multiple moves utilizing the same jumps,
//such as in the case of branching jumps:
//            1
//        2
//    3        3'
//        4
// 1 -> 2 would have numTimes equal to 2 since the jump would be utilized twice,
//once for each move
move::~move()
{
    for (list<jump*>::iterator it = jpoints.begin(); it != jpoints.end(); ++it)
    {
        --(*it)->numTimes;
        if ((*it)->numTimes == 0)
            delete (*it);
    }
}

//initializes everything for the checker board
board::board()
{
    reset();
}

//destructor deallocates memory for all the moves in mlist
board::~board()
{
    while (!mlist.empty())
    {
        delete mlist.front();
        mlist.pop_front();
    }
}

//copy constructor: copies over all data values except the move list
//useful for creating new boards for each move in alpha-beta search
board::board(const board& b): color(b.color)
{
    for (int i = 0; i != 8; ++i)
        for (int j = 0; j != 4; ++j)
            arr[i][j] = b.arr[i][j];
}

//resets the board, called by printEBoard in boardPublic.cpp
//create the start board
//first three rows are filled with black pieces
//next two rows are empty
//last three rows are filled with red pieces
void board::reset()
{
    color = 'b';
    for (int i = 0; i != 3; ++i)
        for (int j = 0; j != 4; ++j)
            arr[i][j] = 'b';
    for (int i = 3; i != 5; ++i)
        for (int j = 0; j != 4; ++j)
            arr[i][j] = 'e';
    for (int i = 5; i != 8; ++i)
        for (int j = 0; j != 4; ++j)
            arr[i][j] = 'r';
}

//store it all in a list
//parse each line
//modify the board per line
//last line, the 9th line, will be turn
//a sample line of input would be
//e b b e
//the last line, get the current piece color's turn
//set color equal to it
//make sure the color is valid
void board::modifyBoard(ifstream& fin)
{
    string line;
    int count = 0;
    while (getline(fin, line) && count != 8)
    {
        remove_carriage_return(line);
        stringstream ss(line);
        for (int jIndex = 0; jIndex != 4; ++jIndex)
            ss >> arr[count][jIndex];
        ++count;
    }
    
    getline(fin, line);
    remove_carriage_return(line);
    stringstream ss(line);
    ss >> color;
    color = tolower(color);
    assert(color == 'b' || color == 'r');
}

//eliminate the \r character in a string or the \n character
inline void board::remove_carriage_return(std::string& line)
{
    if (*line.rbegin() == '\r' || *line.rbegin() == '\n')
        line.erase(line.length() - 1);
}





//*****************************************************GAME*****************************************************//
template <class T>
class sptr
{
private:
    T* pData;
public:
    sptr(T* pVal): pData(pVal) {}
    ~sptr()    {delete pData;}
    T& operator*() {return *pData;}
    T* operator->() {return pData;}
};

class game
{
    //black shall be max
    //red shall be min
    
    //maintains a pointer to the current board
    sptr<board> currentB;
    
    //upper bound for maximum depth that can be reached by alpha beta
    //highly unlikely to reach depth 20 within the given time limit
    static const int maxIterDepth;
    
    //best move from last complete alpha-beta search
    move* bestM;
    
    //temporary best move for current alpha-beta search
    move* tempBestM;
    
    //keeps track of the max depth of the current alpha-beta search
    int maxdepth;
    
    //current depth fully explored of alpha-beta search
    int cdepth;
    
    //tells whether or not total search time has elapsed
    //if it has, terminate the current search and use the previous best move
    bool timeUp;
    
    //becomes true if the game is over
    //condition that loops the game when false and creates more turns
    //set this to false when calling playTheGame()
    bool gameOver;
    
    //reached end of game space
    //marker set to true during end game when terminalTest
    //for a child node has been reached
    //disables the "out of time searching" to next depth message
    bool reachedEnd;
    
    //start and end times
    //uses time and diff time
    //to find the time elapsed in calendar time seconds
    time_t startTime;
    time_t endTime;
    
    //start and end times for search to a certain depth
    //if the most recent search took too long, it will not alpha beta search to next depth
    time_t startTimeD;
    time_t endTimeD;
    
    //prints the game board and prompts user for appropriate move
    //if it's a computer's turn, it starts the alpha-beta search and
    //selects the computer's move
    //it is continuously called when gameOver is false
    void printGame();
    
    //for computer's turn
    //prints depth searched up to, whether or not time ran out, and computer's move
    void outputMessage();
    
    //everything related to the computer's turn
    void computerTurn();
    
    //alpha beta searched called by computerTurn
    int alphabeta(sptr<board>&, int, int, int);
    
    //message that prints when the game is over
    void endMessage();
    
public:
    
    //constructor, initializes data members
    game();
    
    //calls startup
    //calls appropriate functions to play checkers
    void playTheGame();
    
};

//highly unlikely to be able to search to depth 20 within maximum time limit
const int game::maxIterDepth = 20;

//game's constructor
game::game(): currentB(sptr<board>(new board())), bestM(NULL), tempBestM(NULL), maxdepth(0),
cdepth(0), timeUp(false), gameOver(false), reachedEnd(false), startTime(0), endTime(0),
startTimeD(0), endTimeD(0) {}

//generates more turns and starts up the game
void game::playTheGame()
{
    gameOver = false;
    currentB->startup();
    while (!gameOver)
        printGame();
}

//message for game over
//prompts user to play again or not
//calls playTheGame if answer is yes
void game::endMessage()
{
    gameOver = true;
    cout << "The game is over." << endl;
    cout << endl;
    if (currentB->getTurn() == 'r')
        cout << "Player 1 wins." << endl;
    else cout << "Player 2 wins." << endl;
    cout << "Do you want to play again? (Y/N):" << endl;
    char answer;
    cin >> answer;
    bool loop = true;
    while (loop)
    {
        if (tolower(answer) == 'y')
        {
            loop = false;
            playTheGame();
        }
        else if (tolower(answer) == 'n')
            loop = false;
        else
        {
            cout << "Do you want to play again? (Y/N):" << endl;
            cin >> answer;
        }
    }
}

//computer's turn
//calls alpha beta search
//if there's only one available move, makes that move immediately
//prints out depth searched to, time searching, and move made afterwards through outputMessage
void game::computerTurn()
{
    //moves for current board are already created in printGame function
    //game over scenario also taken care of in printGame
    currentB->printMoves();
    cout << "The computer will make a move." << endl;
    
    //if there's only one move to make, make it immediately
    //this may occur for jump moves
    if (currentB->mlist.size() == 1)
    {
        bestM = currentB->mlist.front();
        time(&startTime);
        time(&endTime);
        cdepth = 0;
    }
    else
    {
        //start the timer for the search
        time(&startTime);
        for (int i = 1; i != maxIterDepth; ++i)
        {
            //keep track of amount of time searched up to a specific depth
            time(&startTimeD);
            
            //changes maxdepth
            maxdepth = i;
            
            //calls alpha beta search up to depth maxdepth, with alpha = -infinity and beta = infinity
            alphabeta(currentB, i, std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
            time(&endTimeD);
            
            //if the search up to a specific depth took more than half the time limit
            //terminate the search by breaking out of the loop
            if (difftime(endTimeD, startTimeD) >= ((board::timeLimit)/2))
            {
                time(&endTime);
                timeUp = true;
                break;
            }
            
            //break out of loop if time's up
            //if time isn't up, either the remaining game space has been explored
            //or search to maxIterDepth was completed; sets bestM = tempBestM
            if (timeUp)
                break;
            else
                bestM = tempBestM;
            
            //test if alpha beta is done searching remaining game space, no need to go deeper/repeat
            if (reachedEnd)
                break;
        }
    }
    assert(bestM != NULL);
    
    //output appropriate message for computer's search results
    outputMessage();
}

//prints out computer's move
//also resets bestM, tempBestM, timeUp, and reachedEnd
void game::outputMessage()
{
    currentB->makeMove(bestM);
    cout << "Completed search to depth " << cdepth << "." << endl;
    if (timeUp && cdepth != maxIterDepth && !reachedEnd) //or no way to get to maxdepth since gamespace end has been reached
        cout << "Out of time searching to depth " << cdepth + 1 << "." << endl;
    cout << "Searched for a total of " << difftime(endTime, startTime) << " seconds" << endl;
    cout << "The chosen move is: ";
    board::convertCommand(bestM->command);
    cout << endl;
    
    //resets everything used for next alpha-beta search
    bestM = NULL;
    tempBestM = NULL;
    timeUp = false;
    reachedEnd = false;
}

//prints the game board
//outputs game over message if necessary;
//generates moves for current player's turn if there are any
//if the game has not ended, it
//makes the computer make a move
//or prompts user to make a move
void game::printGame()
{
    currentB->printBoard();
    if (currentB->terminalTest())
        endMessage();
    else if (currentB->isComputerTurn())
        computerTurn();
    else
        currentB->inputCommand();
}

//fail-hard alpha beta
//returns alpha / beta instead of value
int game::alphabeta(sptr<board>& b, int depth, int alpha, int beta)
{
    if (depth != maxdepth && b->terminalTest())    //don't need to compute moves for depth 0
    {
        //b->printBoard();
        reachedEnd = true;    //set reached end as true
        cdepth = maxdepth;
        if (b->getTurn() == 'r')
            return std::numeric_limits<int>::max();
        else return std::numeric_limits<int>::min();
    }
    reachedEnd = false;    //set reached end as false, means that remaining game space still isn't fully explored
    if (depth == 0)
        return b->evaluate();
    list<move*>::iterator iter = b->mlist.begin();
    
    int localalpha = std::numeric_limits<int>::min();
    int localbeta = std::numeric_limits<int>::max();
    
    //max's turn
    if (b->getTurn() == 'b')
    {
        for (; iter != b->mlist.end(); ++iter)
        {
            //update the end time
            time(&endTime);
            
            //check if time's up
            //if it is, break out of the loop
            if (difftime(endTime, startTime) >= (board::timeLimit - 1))
            {
                timeUp = true;
                break;
            }
            
            //make the move
            //create a new board using the copy constructor
            //the move has been made for the new board
            //and pass it as a parameter to alpha beta,
            //along with depth reduced by one
            //undo the move and revert the turn
            //this is because undoMove doesn't change turn
            b->makeMove(*iter);
            sptr<board> newB(new board(*b));
            int value = alphabeta(newB, depth-1, alpha, min(localbeta, beta));
            b->undoMove(*iter);
            b->changeTurn();
            
            //found best move
            if (value > alpha)
            {
                alpha = value;
                if (depth == maxdepth)
                    tempBestM = (*iter);
            }
            
            //cut off
            if (alpha >= beta && depth < maxdepth)
                return alpha;
        }
        
        //search has been completed to depth = maxdepth
        //update cdepth to be equal to maxdepth
        if (!timeUp && depth == maxdepth)
            cdepth = depth;
        
        //this is max's best move
        return alpha;
    }
    
    // turn = 'r'
    //min's turn
    //almost identical to max's
    //returns beta instead of alpha
    //tests value < beta instead of if value > alpha
    //for finding best move
    else
    {
        for (; iter != b->mlist.end(); ++iter)
        {
            time(&endTime);
            if (difftime(endTime, startTime) >= (board::timeLimit - 1))
            {
                timeUp = true;
                break;
            }
            b->makeMove(*iter);
            sptr<board> newB(new board(*b));
            int value = alphabeta(newB, depth-1, max(localalpha, alpha), beta);
            b->undoMove(*iter);
            b->changeTurn();
            
            //found best move for min
            if (value < beta)
            {
                beta = value;
                if (depth == maxdepth)
                    tempBestM = (*iter);
            }
            
            //cut off
            if (alpha >= beta)
                return beta;
        }
        
        //search has been completed to depth = maxdepth
        //update cdepth to be equal to maxdepth
        if (!timeUp && depth == maxdepth)
            cdepth = depth;
        
        //this is min's best move
        return beta;
    }
}



//*****************************************************BoardPrint*****************************************************//
void board::convert(const int& x, const int& y, string& s)
{
    //assert(0 <= x && x <= 7 && 0 <= y && y <= 3);
    char c1 = '0' + x;
    char c2;
    if (x % 2 == 0)
    {
        c2 = '0' + (2*y + 1);
    }
    else
    {
        c2 = '0' + (2*y);
    }
    s += c1;
    s += ' ';
    s += c2;
    s += ' ';
}

//used for printing out moves, converting the y coordinate in the matrix
//to the coordinate on the expanded 8x8 board
inline int board::convertY(const int& x, const int& y)
{
    if (x % 2 == 0)
        return (2*y + 1);
    else return (2*y);
}

//prints a line of the board
//that does not contain any pieces
//i.e something like: XXX|   |XXX|   |XXX|   |XXX|
//cases vary by whether or not it's an even or odd row
//called by printBoard in boardPublic.cpp
void board::printline(const int& i, const string& lineEven, const string& lineOdd)
{
    if (i % 2 == 0)
    {
        cout << lineEven << endl;
        cout << "  " << i << " |XXXXX|";
        for (int j = 0; j != 3; ++j)
        {
            cout << "  ";
            printcolor(arr[i][j]);
            cout << "  |XXXXX|";
        }
        cout << "  ";
        printcolor(arr[i][3]);
        cout << "  |" << endl;;
        cout << lineEven << endl;
    }
    else
    {
        cout << lineOdd << endl;
        cout << "  " << i << " |";
        for (int j = 0; j != 3; ++j)
        {
            cout << "  ";
            printcolor(arr[i][j]);
            cout << "  |XXXXX|";
        }
        cout << "  ";
        printcolor(arr[i][3]);
        cout << "  |XXXXX|" << endl;;
        cout << lineOdd << endl;
    }
}

//function for printing a character in a different color
//uses escape sequences to print out a color character
void board::printcolor(const char& c)
{
    if (c == 'e')
        cout << ' ';
    else if (c == 'r' || c == 'R')
    {
        //sets piece as magenta color
        string default_console = "\033";
        string color = "\033";
        cout << color << c;
        cout << default_console;
    }
    else
    {
        //c is 'b' or 'B', sets pieces as cyan color
        string default_console = "\033";
        string color = "\033";
        cout << color << c;
        cout << default_console;
    }
}

//called by startup, which is found in board.h
//prompts user to assign who is/ is not a computer
void board::whoComputer()
{
    bool b = true;
    char c = ' ';
    while (b)
    {
        cout << "Will player # 1 be a computer? (Y/N):" << endl;
        cin >> c;
        if (tolower(c) == 'y')
        {
            board::isComputer[0] = true;
            b = false;
        }
        else if (tolower(c) == 'n')
        {
            board::isComputer[0] = false;
            b = false;
        }
    }
    b = true;
    c = ' ';
    while (b)
    {
        cout << "Will player # 2 be a computer? (Y/N):" << endl;
        cin >> c;
        if (tolower(c) == 'y')
        {
            board::isComputer[1] = true;
            b = false;
        }
        else if (tolower(c) == 'n')
        {
            board::isComputer[1] = false;
            b = false;
        }
    }
    if (board::isComputer[0] == true || board::isComputer[1] == true)
    {
        cout << "Enter a time limit for the computer in seconds (3-60):" << endl;
        cin >> timeLimit;
        if (timeLimit > 60 || timeLimit < 3)
            cin >> timeLimit;
    }
}






//*****************************************************BoardPublic*****************************************************//
void board::convertCommand(const string& s)
{
    string::const_iterator it = s.begin();
    cout << "(" << (*it) << ", ";
    it += 2;
    cout << (*it) << ") ";
    it += 2;
    while (*it != '-')
    {
        cout << "-> (" << (*it) << ", ";
        it += 2;
        cout << (*it) << ") ";
        it += 2;
    }
}

//functions for outputting commands
//for humans, ask for move
//for computer, this function will never be called
//instead another case will run in printEBoard which is found in boardPublic.cpp
void board::inputCommand()
{
    printMoves();
    string m;
    cout << "Enter a sequence of integers indicating a move." << endl;
    cout << "Each set of two integers represents a position." << endl;
    cout << "End the sequence with -1." << endl;
    cout << "For example: 2 3 3 2 -1" << endl;
    cout <<    "    represents (2,3) -> (3,2)" << endl;
    cout << "Enter move: ";
    
    //enter a command
    //try to match the command with one in the list of moves
    //if the end of the list is reached
    //input command again until one is matched
    getline(cin, m);
    list<move*>::iterator it = mlist.begin();
    while (it != mlist.end())
    {
        if ((*it)->command == m)
        {
            cout << "You have chosen the move: ";
            convertCommand((*it)->command);
            cout << endl;
            break;
        }
        ++it;
        if (it == mlist.end())
        {
            getline(cin, m);
            it = mlist.begin();
        }
    }
    makeMove(*it);
}

//print the board
//called by printEBoard
void board::printBoard()
{
    cout << "Current board:" << endl;
    cout << endl;
    cout << "Player 1 is ";
    printcolor('b');
    cout << " (normal piece) and ";
    printcolor('B');
    cout << " (king)" << endl;
    cout << "Player 2 is ";
    printcolor('r');
    cout << " (normal piece) and ";
    printcolor('R');
    cout <<    " (king)" << endl;
    int count = 0;
    cout << "       " << count;
    ++count;
    while (count != 8)
    {
        cout << "     " << count++;
    }
    cout << " " << endl;
    string lineEven = "    |XXXXX|     |XXXXX|     |XXXXX|     |XXXXX|     |";
    string lineOdd = "    |     |XXXXX|     |XXXXX|     |XXXXX|     |XXXXX|";
    string linebreak = "    -------------------------------------------------";
    
    //print the board
    cout << linebreak << endl;
    for (int i = 0; i != 8; ++i)
    {
        printline(i, lineEven, lineOdd);
        if (i != 7)
            cout << linebreak << endl;
    }
    cout << linebreak << endl;
    
    //output a blank line before outputting moves
    cout << endl;
}

//decides whose turn it is to move based on color
//prints out all the legal moves for the current board
void board::printMoves()
{
    if (color == 'b')
        cout << "Player 1 to move." << endl;
    else cout << "Player 2 to move." << endl;
    cout << "The legal moves are:" << endl;
    list<move*>::const_iterator it = mlist.begin();
    for (; it != mlist.end(); ++it)
    {
        cout << "Move: ";
        convertCommand((*it)->command);
        cout << endl;
    }
    cout << endl;
}

//makes a move
//if there's any jumps, they are implemented
//pieces are erased and subtracted from the total count if necessary
//moves the piece from one position to another
void board::makeMove(move* m)
{
    if (!m->jpoints.empty())
    {
        list<jump*>::iterator it = m->jpoints.begin();
        for (; it != m->jpoints.end(); ++it)
            arr[(*it)->x][(*it)->y] = 'e';
    }
    
    //save the piece
    //replace the start position with an empty space
    //add back in the saved piece at the end point
    char c = arr[m->xi][m->yi];
    arr[m->xi][m->yi] = 'e';
    arr[m->xf][m->yf] = c;
    
    //check if the piece should be changed to a king and change player's turn
    handleKinging(m->xf, m->yf);
    changeTurn();
}

//undoes a move
//replaces the starting jump point only if the starting jump hasn't already been replaced
//iterate through its list of jumps
//add back all the characters that were temporarily deleted
//add the jumping piece in the start position of the move
void board::undoMove(move* m)
{
    if (!m->jpoints.empty())
    {
        for (list<jump*>::iterator it = m->jpoints.begin(); it != m->jpoints.end(); ++it)
        {
            arr[(*it)->xs][(*it)->ys] = 'e';
            arr[(*it)->x][(*it)->y] = (*it)->c;
            arr[(*it)->xend][(*it)->yend] = 'e';
        }
    }
    arr[m->xf][m->yf] = 'e';
    arr[m->xi][m->yi] = m->mP;
}

//gives a small bonus to losing player for being in a double corner
//gives a smaller bonus to winning player for being on a diagonal close to the losing piece's corner
//will help winning player force losing player out of a corner
int board::cornerDiagonal(char losing, char winning)
{
    int c = 0;
    if (tolower(arr[0][0]) == losing || tolower(arr[1][0]) == losing)
    {
        c += 9;
        if (tolower(arr[0][0]) == winning)
            c -= 3;
        if (tolower(arr[1][0]) == winning)
            c -= 3;
        if (tolower(arr[1][1]) == winning)
            c -= 1;
        if (tolower(arr[2][0]) == winning)
            c -= 1;
        if (tolower(arr[2][1]) == winning)
            c -= 1;
        if (tolower(arr[3][1]) == winning)
            c -= 1;
    }
    if (tolower(arr[6][3]) == losing || tolower(arr[7][3]) == losing)
    {
        c += 9;
        if (tolower(arr[4][2]) == winning)
            c -= 1;
        if (tolower(arr[5][2]) == winning)
            c -= 1;
        if (tolower(arr[5][3]) == winning)
            c -= 1;
        if (tolower(arr[6][2]) == winning)
            c -= 1;
        if (tolower(arr[6][3]) == winning)
            c -= 3;
        if (tolower(arr[7][3]) == winning)
            c -= 3;
    }
    return c;
}

//black is more positive
//red is more negative
//aabbccddee
//aa is a count of all pieces: king is worth a total of 3, regular pieces are 2 (black - red)
//bb is measuring how close a normal piece is to becoming a king, give last row a bonus
//cc is a piece count difference
//dd is a measurement near end game, double corners add a bonus for losing player
//ee is pseudo-random in the case that multiple moves tie for best
int board::evaluate()
{
    int a1 = 0, a2 = 0, b = 0, c = 0, d = 0;
    for (int i = 0; i != 8; ++i)
        for (int j = 0; j != 4; ++j)
        {
            if (arr[i][j] == 'b')
            {
                a1 += 2;
                if (i == 0)
                    b += 9;
                else b += i;
                c += 1;
            }
            else if (arr[i][j] == 'r')
            {
                a2 -=2;
                if (i == 7)
                    b -= 9;
                else b -= (7 - i);
                c -= 1;
            }
            else if (arr[i][j] == 'B')
            {
                a1 += 3;
                c += 1;
            }
            else if (arr[i][j] == 'R')
            {
                a2 -= 3;
                c -= 1;
            }
        }
    if (c > 0 && a2 >= -8)
        d -= cornerDiagonal('r', 'b');
    else if (c < 0 && a1 <= 8)
        d += cornerDiagonal('b', 'r');
    a1 *= 100000000;
    a2 *= 100000000;
    b *= 1000000;
    c *= 10000;
    d *= 100;
    int e = rand() % 100;
    if (color == 'r')
        e = -e;
    return a1 + a2 + b + c + d + e;
}

//determines whether or not players will be a computer calls modifyBoard
void board::startup()
{
    //reset the board
    reset();
    whoComputer();
    bool b = true;
    cout << "Do you want to load a game from a file? (Y/N):" << endl;
    char c = ' ';
    while (b)
    {
        cin >> c;
        if (tolower(c) == 'y' || tolower(c) == 'n')
            b = false;
    }
    if (tolower(c) == 'y')
    {
        string name;
        cout << "Enter filename: " << endl;
        cin >> name;
        ifstream fin(name.c_str());
        while (!fin.good())
        {
            cout << "Enter filename: " << endl;
            cin >> name;
            fin.open(name.c_str());
        }
        modifyBoard(fin);
    }
}








//*****************************************************BoardMoves*****************************************************//
void board::checkNeighbors(int& x, int& y)
{
    //check down movement
    //cases depend on whether or not it's an even or odd row
    if (tolower(arr[x][y]) == 'b' || arr[x][y] == 'R')
    {
        if (x % 2 == 0)
        {
            createMove(x, y, x+1, y);
            createMove(x, y, x+1, y+1);
        }
        else
        {
            createMove(x, y, x+1, y);
            createMove(x, y, x+1, y-1);
        }
    }
    //check up movement
    if (tolower(arr[x][y]) == 'r' || arr[x][y] == 'B')
    {
        if (x % 2 == 0)
        {
            createMove(x, y, x-1, y);
            createMove(x, y, x-1, y+1);
        }
        else
        {
            createMove(x, y, x-1, y);
            createMove(x, y, x-1, y-1);
        }
    }
}

//creates the move given the beginning and end positions of the move
//also creates the command associated with the move
//check for valid positions and
//check to make sure the adjacent piece is empty
void board::createMove(const int& xi,const int& yi, int xf, int yf)
{
    if (isValidPos(xf, yf) && arr[xf][yf] == 'e')
    {
        move* m = new move(arr[xi][yi], xi, yi, xf, yf);
        mlist.push_back(m);
        convert(xi, yi, m->command);
        convert(xf, yf, m->command);
        m->command += "-1";
    }
}

//returns true if there are any regular moves
//called by movesAvailable in board.h
//called only if jumpsAvailable returns false
bool board::listMoves()
{
    while (!mlist.empty())
    {
        delete mlist.front();
        mlist.pop_front();
    }
    
    //iterate through the matrix
    //check neighboring positions
    //if the piece matches the current turn's color
    if (color == 'b')
    {
        for (int i = 0; i!= 8; ++i)
            for (int j = 0; j != 4; ++j)
                if (arr[i][j] == color || arr[i][j] == toupper(color))
                    checkNeighbors(i, j);
    }
    //color == 'b'
    //order this differently so that the moves are ordered so that
    //pieces closest to being kings get checked first
    //useful for alpha-beta
    else
    {
        for (int i = 7; i!= -1; --i)
            for (int j = 0; j != 4; ++j)
                if (arr[i][j] == color || arr[i][j] == toupper(color))
                    checkNeighbors(i, j);
    }
    
    if (mlist.empty())
        return false;
    return true;
}









//*****************************************************BoardJump*****************************************************//
//create a "code" for each jump, used to prevent repetitions
inline int board::createkey(int xs, int ys, int xj, int yj, int xe, int ye)
{
    return ye + xe*10 + yj*100 + xj*1000 + ys*10000 + xs*100000;
}

//reverses a jump's key
//so 233444 will be broken up into 23 34 44 and reorganized to become 444332
//this is to prevent any repeated jumps
int board::reverse(int i)
{
    int num = 0;
    while (i/100 > 0)
    {
        num += i % 100;
        num *= 100;
        i /= 100;
    }
    num += i;
    return num;
    
}

//creates a new jump given the start position (xs, ys),
//jumped over position (xj, yj), and end position (xe, ye)
//also takes a jump pointer jp which represents the previous jump
//it will be added in when the jumps are connected into a move
//instead the character is passed
//recursively call jumpAvailable, using the new end position
//keep track of what piece is currently making the jumps by passing it as parameter
void board::createJump(list<jump*>& jlist, char c, int xs, int ys, int xj, int yj, int xe, int ye, jump* jp)
{
    arr[xs][ys] = 'e';
    //+1 to each because 0 will mess up the keys and reverse for an edge case such as xs=0 ys =0
    int key = createkey(xs+1, ys+1, xj+1, yj+1, xe+1, ye+1);
    jump* jcheck = jp;
    while (jcheck != NULL)
    {
        if (key == jcheck->key || key == reverse(jcheck->key))
            return;
        jcheck = jcheck->prev;
    }
    jump* j = new jump(c, arr[xj][yj], xs, ys, xj, yj, xe, ye, jp, key);
    if (jp != NULL)
        jp->noNext = false;
    jlist.push_front(j);
    jumpAvailable(jlist, c, xe, ye, j);
}

//iterate through the list of jumps
//create new moves only when encountering the last jump
//add all the appropriate jumps to the move's list of jumps via backtracking
//increment the jump's numTimes
//repeat the loop until the first jump was added to the move's jump list
//add the start point to the move's start position and to command
//undoes each jump, so the starting character is where it began before getting erased
//replaces the 'e's with original characters
void board::createJumpMove(list<jump*>& jlist)
{
    if (!jlist.empty())
    {
        list<jump*>::const_iterator it = jlist.begin();
        for (; it != jlist.end(); ++it)
        {
            if ((*it)->noNext)
            {
                move* m = new move((*it)->jumpingPiece, -1, -1, -1, -1);
                jump* jp = (*it);
                while (jp != NULL)
                {
                    m->jpoints.push_front(jp);
                    ++jp->numTimes;
                    jp = jp->prev;
                }
                m->xi = m->jpoints.front()->xs;
                m->yi = m->jpoints.front()->ys;
                convert(m->jpoints.front()->xs, m->jpoints.front()->ys, m->command);
                for (list<jump*>::iterator it = m->jpoints.begin(); it != m->jpoints.end(); ++it)
                {
                    convert((*it)->xend, (*it)->yend, m->command);
                    if ((*it)->noNext)
                    {
                        m->xf = (*it)->xend;
                        m->yf = (*it)->yend;
                    }
                }
                
                mlist.push_back(m);
                m->command += "-1";
                undoMove(m);
            }
        }
    }
}

//checking for jumping in all four directions
//(x,y) is the start point
void board::jumpAvailable(list<jump*>& jlist, char c, int x, int y, jump* jp= NULL)
{
    if (tolower(c) == 'b' || c == 'R')
    {
        if (x % 2 == 0)    //even x
        {
            if (jumpConditions(x+1, y, x+2, y-1))    //checks left down jump
                createJump(jlist, c, x, y, x+1, y, x+2, y-1, jp);
            if (jumpConditions(x+1, y+1, x+2, y+1))    //checks right down jump
                createJump(jlist, c, x, y, x+1, y+1, x+2, y+1, jp);
        }
        else    //odd x
        {
            if (jumpConditions(x+1, y-1, x+2, y-1))    //checks left down jump
                createJump(jlist, c, x, y, x+1, y-1, x+2, y-1, jp);
            if (jumpConditions(x+1, y, x+2, y+1))    //checks right down jump
                createJump(jlist, c, x, y, x+1, y, x+2, y+1, jp);
        }
    }
    if (tolower(c) == 'r' || c == 'B')
    {
        if (x % 2 == 0)    //even x
        {
            if (jumpConditions(x-1, y+1, x-2, y+1))    //checks right up jump
                createJump(jlist, c, x, y, x-1, y+1, x-2, y+1, jp);
            if (jumpConditions(x-1, y, x-2, y-1))    //checks left up jump
                createJump(jlist, c, x, y, x-1, y, x-2, y-1, jp);
        }
        else    //odd x
        {
            if (jumpConditions(x-1, y-1, x-2, y-1))    //checks left up jump
                createJump(jlist, c, x, y, x-1, y-1, x-2, y-1, jp);
            if (jumpConditions(x-1, y, x-2, y+1))    //checks right up jump
                createJump(jlist, c, x, y, x-1, y, x-2, y+1, jp);
        }
    }
}

//called by movesAvailable in board.h
//clears all moves
//if the piece belongs to the current turn's color
//check it for jumps
//then create jumping moves once the search is finished
bool board::jumpsAvailable()
{
    while (!mlist.empty())
    {
        delete mlist.front();
        mlist.pop_front();
    }
    for (int i = 0; i!= 8; ++i)
    {
        for (int j = 0; j != 4; ++j)
        {
            if (arr[i][j] == color || arr[i][j] == toupper(color))
            {
                list<jump*> jlist;
                jumpAvailable(jlist, arr[i][j], i, j, NULL);
                createJumpMove(jlist);
            }
        }
    }
    
    //if no jumping moves were added, return false, else return true
    if (mlist.empty())
        return false;
    return true;
}

//checks for jumping conditions
//checks if the jumped point is valid and has the enemy's color
//checks if the end point is valid and empty
//returns true if conditions are satisfied
bool board::jumpConditions(int xj, int yj, int xe, int ye)
{
    if (isValidPos(xj, yj) && isValidPos(xe, ye) && arr[xj][yj] != 'e' &&
        arr[xj][yj] != color && arr[xe][ye] == 'e' &&  arr[xj][yj] != std::toupper(color))
        return true;
    return false;
}










int main()
{
    //create the game and play it
    //playTheGame function found in game.cpp
    game g;
    g.playTheGame();
}

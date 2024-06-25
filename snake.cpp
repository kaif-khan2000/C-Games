#include<iostream>
#include<cstdlib>
#include<termios.h>
#include<unistd.h>
#include<fcntl.h>
#include<thread>


int getch(void) 
{ 
    struct termios oldattr, newattr; 
    int ch; 
    tcgetattr(STDIN_FILENO, &oldattr); 
    newattr = oldattr; 
    newattr.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &newattr); 
    ch = getchar(); 
    tcsetattr(STDIN_FILENO, TCSANOW, &oldattr); 
    return ch; 
} 


#define x first
#define y second

using namespace std;

int width;
int height;
bool gameOver;
int score;

pair<int, int> pLoc, fLoc, wallhit;
enum Direction {STOP=0, UP, DOWN, LEFT, RIGHT};
Direction dir;
pair<int, int> tails[1000];
int tailCount;

void setup() {
	cout << "enter game size\n";
	cin >> height;
	cin >> width;
	gameOver = false;
	dir = STOP;
	score = 0;
	pLoc.x = width/2;
	pLoc.y = height/2;
	fLoc.x = (rand()%width-1) + 1;
	fLoc.y = (rand()%height-1) + 1;
	wallhit.x = -2;
	wallhit.y = -2;
	tailCount = 0;
}

void draw() {
	system("clear");
	cout << "#";
	for (int i=1;i<width;i++) {
		if (wallhit.x == i && wallhit.y == -1)
			cout << "X";
		else
			cout << "-";
	}

	cout << "#" << endl;

	for (int i=0;i<height;i++) {
		if (wallhit.y == i && wallhit.x == -1)
			cout << "X";
		else
			cout << "|";
		for (int j=1;j<width;j++){
			if (pLoc.x == j && pLoc.y == i && fLoc.x == j && fLoc.y == i){
				score++;
				fLoc.x = (rand()%width-1) + 1;
				fLoc.y = (rand()%height-1) + 1;
				tails[tailCount].x = tailCount ? tails[tailCount].x+1 : pLoc.x + 1;
				tails[tailCount].y = tailCount ? tails[tailCount].y : pLoc.y;
				tailCount++;
			}
			bool tailPrint = false;
			for (int k=0;k<tailCount;k++)
				if (tails[k].x == j && tails[k].y == i) {
					tailPrint = true;
					cout << "*";
				}

			if (pLoc.x == j && pLoc.y == i) 
				cout << "0";
			else if (fLoc.x == j && fLoc.y == i)
				cout << "@";
			else if (!tailPrint)
				cout << " ";
		}
		if (wallhit.y == i && wallhit.x == width)
			cout << "X" << endl;
		else
			cout << "|" << endl;
	}
	cout << "#";
	for (int i=1;i<width;i++) {	
		if (wallhit.x == i && wallhit.y == height)
			cout << "X";
		else
			cout << "-";
	}
	cout << "#" <<endl;
	cout << "Score: " << score << endl;
	cout << "Food: " << fLoc.x << " " << fLoc.y << endl;
	if (gameOver) cout << "GameOver" << endl;
}

void getInput() {
	while(!gameOver) {

		switch(getch()) {
			case 65:
				dir = tailCount!=0 && dir==DOWN ? dir : UP;
				break;
			case 66:
				dir = tailCount!=0 && dir==UP ? dir : DOWN;
				break;
			case 67:
				dir = tailCount!=0 && dir==LEFT ? dir : RIGHT;
				break;
			case 68:
				dir = tailCount!=0 && dir==RIGHT ? dir : LEFT;
				break;
			case 'q':
				gameOver = true;
				break;
			default:
				dir = STOP;
				break;
		}
	}
}

void logic() {
	if (pLoc.x <= 0 || pLoc.x >= width || pLoc.y < 0 || pLoc.y >= height) {
		wallhit.x = pLoc.x;
		wallhit.y = pLoc.y;
		gameOver = true;
	}
	int prevX = pLoc.x, prevY = pLoc.y;
	switch(dir) {
		case UP:    // key up 65
			pLoc.y--;
        		break;
    		case DOWN:    // key down
			pLoc.y++;
        		break;
    		case RIGHT:    // key right
        		pLoc.x++;
			break;
    		case LEFT:    // key left
        		pLoc.x--;
			break;
		case STOP:
			return;
	}

	for (int i=0;i<tailCount;i++) {
		if (tailCount>1 && tails[i].x == pLoc.x && tails[i].y == pLoc.y)
			gameOver = true;
		swap(tails[i].x, prevX);
		swap(tails[i].y, prevY);
	}
}

int main() {
	setup();

	thread t(getInput);
	while(!gameOver) {
		logic();
		draw();
		unsigned int pause = (150-score) * 1000;
		usleep(pause);
	}
	draw();
	t.join();	
	return 0;
}

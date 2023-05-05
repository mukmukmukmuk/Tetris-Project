#include "tetris.h"

static struct sigaction act, oact;

int main()
{
	int exit = 0;
	createRankList();
	initscr();
	noecho();
	keypad(stdscr, TRUE);

	srand((unsigned int)time(NULL));

	while (!exit)
	{
		clear();
		switch (menu())
		{
		case MENU_PLAY:
			play();
			break;
		case MENU_RANK:
			rank();
			break;
		case MENU_REC:
			recommendedPlay();
			break;
		case MENU_EXIT:
			exit = 1;
			break;
		default:
			break;
		}
	}

	endwin();
	system("clear");
	Node *curr = head;
	while (curr)
	{
		Node *tmp = curr;
		curr = curr->link;
		free(tmp);
	}
	return 0;
}

void InitTetris()
{
	for (int i = 0; i < BLOCK_NUM; i++)
		nextBlock[i] = rand() % 7;
	recRoot = malloc(sizeof(RecNode));
	memorysize += sizeof(RecNode);
	recRoot->lv = 0;
	recRoot->score = 0;
	recRoot->y = 0;
	for (int j = 0; j < HEIGHT; j++)
	{
		for (int i = 0; i < WIDTH; i++)
		{
			field[j][i] = 0;
			recRoot->f[j][i] = 0;
		}
	}
	int tmp = modified_recommend(recRoot);
	free(recRoot);
	if (!tmp)
		gameOver = 1;
	blockRotate = 0;
	blockY = -1;
	blockX = WIDTH / 2 - 2;
	score = 0;
	gameOver = 0;
	timed_out = 0;

	DrawOutline();
	DrawField();
	if (!recOn)
		DrawBlock(blockY, blockX, nextBlock[0], blockRotate, ' ');
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline()
{
	int i, j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0, 0, HEIGHT, WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2, WIDTH + 10);
	printw("NEXT BLOCK");
	DrawBox(3, WIDTH + 10, 4, 8);

	DrawBox(11, WIDTH + 10, 4, 8);

	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(19, WIDTH + 10);
	printw("SCORE");
	DrawBox(20, WIDTH + 10, 1, 8);
}
int GetCommand()
{
	int command;
	command = wgetch(stdscr);
	switch (command)
	{
	case KEY_UP:
		break;
	case KEY_DOWN:
		break;
	case KEY_LEFT:
		break;
	case KEY_RIGHT:
		break;
	case ' ': /* space key*/
		/*fall block*/
		break;
	case 'q':
	case 'Q':
		command = QUIT;
		break;
	default:
		command = NOTHING;
		break;
	}
	if (command != QUIT && recOn)
		command = KEY_DOWN;
	return command;
}

int ProcessCommand(int command)
{
	int ret = 1;
	int drawFlag = 0;
	switch (command)
	{
	case QUIT:
		ret = QUIT;
		break;
	case KEY_UP:
		if ((drawFlag = CheckToMove(field, nextBlock[0], (blockRotate + 1) % 4, blockY, blockX)))
			blockRotate = (blockRotate + 1) % 4;
		break;
	case KEY_DOWN:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX)))
			blockY++;
		break;
	case KEY_RIGHT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX + 1)))
			blockX++;
		break;
	case KEY_LEFT:
		if ((drawFlag = CheckToMove(field, nextBlock[0], blockRotate, blockY, blockX - 1)))
			blockX--;
		break;
	default:
		break;
	}
	if (drawFlag)
		DrawChange(field, command, nextBlock[0], blockRotate, blockY, blockX);
	return ret;
}

void DrawField()
{
	int i, j;
	for (j = 0; j < HEIGHT; j++)
	{
		move(j + 1, 1);
		for (i = 0; i < WIDTH; i++)
		{
			if (field[j][i] == 1)
			{
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(".");
		}
	}
}

void PrintScore(int score)
{
	move(21, WIDTH + 11);
	printw("%8d", score);
}

void DrawNextBlock(int *nextBlock)
{
	int i, j;
	for (i = 0; i < 4; i++)
	{
		move(4 + i, WIDTH + 13);
		for (j = 0; j < 4; j++)
		{
			if (block[nextBlock[1]][0][i][j] == 1)
			{
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(" ");
		}
		move(12 + i, WIDTH + 13);
		for (j = 0; j < 4; j++)
		{
			if (block[nextBlock[2]][0][i][j] == 1)
			{
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(" ");
		}
	}
}

void DrawBlock(int y, int x, int blockID, int blockRotate, char tile)
{
	int i, j;
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
		{
			if (block[blockID][blockRotate][i][j] == 1 && i + y >= 0)
			{
				move(i + y + 1, j + x + 1);
				attron(A_REVERSE);
				printw("%c", tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT, WIDTH + 10);
}

void DrawBox(int y, int x, int height, int width)
{
	int i, j;
	move(y, x);
	addch(ACS_ULCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for (j = 0; j < height; j++)
	{
		move(y + j + 1, x);
		addch(ACS_VLINE);
		move(y + j + 1, x + width + 1);
		addch(ACS_VLINE);
	}
	move(y + j + 1, x);
	addch(ACS_LLCORNER);
	for (i = 0; i < width; i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play()
{
	int command;
	clock_t start, finish;
	long double duration;
	memorysize = 0;
	start = time(NULL);

	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM, &act, &oact);
	InitTetris();
	do
	{
		if (timed_out == 0)
		{
			alarm(1);
			timed_out = 1;
		}

		command = GetCommand();
		if (ProcessCommand(command) == QUIT)
		{

			alarm(0);
			DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
			move(HEIGHT / 2, WIDTH / 2 - 4);
			printw("Good-bye!!");
			refresh();
			getch();
			finish = time(NULL);
			duration = (long double)difftime(finish, start);
			clear();
			printw("play-time : %.2llf sec\n", duration);
			printw("score : %d\n", score);
			printw("space : %lld\n", memorysize);
			printw("time-efficiency : %.8llf\n", (float)score / duration);
			printw("space-efficiency : %.8lf\n", (float)score / memorysize);
			refresh();
			getch();

			return;
		}
	} while (!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT / 2 - 1, WIDTH / 2 - 5, 1, 10);
	move(HEIGHT / 2, WIDTH / 2 - 4);
	printw("GameOver!!");
	refresh();
	getch();
	finish = time(NULL);
	duration = (long double)difftime(finish, start);
	newRank(score);
	writeRankFile();
	clear();
	printw("play-time : %.2llf sec\n", duration);
	printw("score : %d\n", score);
	printw("space : %lld\n", memorysize);
	printw("time-efficiency : %.2llf\n", (float)score / duration);
	printw("space-efficiency : %.8lf\n", (float)score / memorysize);
	refresh();
	getch();
}

char menu()
{
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

/////////////////////////첫주차 실습에서 구현해야 할 함수/////////////////////////

int CheckToMove(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX)
{
	// user code
	for (int i = 0; i <= 3; i++)
	{
		for (int j = 0; j <= 3; j++)
		{
			if (block[currentBlock][blockRotate][i][j])
			{
				int dy = blockY + i, dx = blockX + j;
				if (f[dy][dx] == 1 || dy >= HEIGHT || dy < 0 || dx >= WIDTH || dx < 0)
					return 0;
			}
		}
	}
	return 1;
}

void DrawChange(char f[HEIGHT][WIDTH], int command, int currentBlock, int blockRotate, int blockY, int blockX)
{
	// user code
	int y = blockY, x = blockX, r = blockRotate;
	// 1. 이전 블록 정보를 찾는다. ProcessCommand의 switch문을 참조할 것
	switch (command)
	{
	case KEY_LEFT:
		x++;
		break;
	case KEY_RIGHT:
		x--;
		break;
	case KEY_UP:
		r = (r + 3) % 4;
		break;
	case KEY_DOWN:
		y--;
		break;
	default:
		break;
	}
	int sd = y;
	// 2. 이전 블록 정보를 지운다. DrawBlock함수 참조할 것.
	while (CheckToMove(field, currentBlock, r, sd + 1, x))
		sd++;
	for (int i = 0; i <= 3; i++)
		for (int j = 0; j <= 3; j++)
		{
			if (block[currentBlock][r][i][j] == 1 && i + y >= 0)
			{
				move(i + y + 1, j + x + 1);
				printw("%c", '.');
			}
			if (block[currentBlock][r][i][j] == 1 && i + sd >= 0)
			{
				move(i + sd + 1, j + x + 1);
				printw("%c", '.');
			}
		}
	// 3. 새로운 블록 정보를 그린다.
	DrawBlockWithFeatures(blockY, blockX, currentBlock, blockRotate);
}

void BlockDown(int sig)
{
	if (recOn)
	{
		blockY = recommendY;
		blockX = recommendX;
		blockRotate = recommendR;
	}
	// user code
	if (CheckToMove(field, nextBlock[0], blockRotate, blockY + 1, blockX) == 1)
	{
		blockY++;
		DrawChange(field, KEY_DOWN, nextBlock[0], blockRotate, blockY, blockX);
		timed_out = 0;
		return;
	}

	if (blockY == -1)
		gameOver = 1;

	score += AddBlockToField(field, nextBlock[0], blockRotate, blockY, blockX);

	score += DeleteLine(field);
	PrintScore(score);
	for (int i = 0; i < BLOCK_NUM - 1; i++)
		nextBlock[i] = nextBlock[i + 1];
	nextBlock[BLOCK_NUM - 1] = rand() % 7;

	recRoot = malloc(sizeof(RecNode));
	memorysize += sizeof(RecNode);
	recRoot->lv = 0;
	recRoot->score = 0;
	for (int i = 0; i < HEIGHT; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			recRoot->f[i][j] = field[i][j];
		}
	}
	int tmp = modified_recommend(recRoot);
	free(recRoot);
	if (!tmp)
		gameOver = 1;
	DrawNextBlock(nextBlock);

	blockRotate = 0;
	blockY = -1;
	blockX = (WIDTH / 2) - 2;

	DrawField();
	timed_out = 0;
	//강의자료 p26-27의 플로우차트를 참고한다.
}

int AddBlockToField(char f[HEIGHT][WIDTH], int currentBlock, int blockRotate, int blockY, int blockX)
{
	// user code
	int sc = 0;
	for (int i = 0; i <= 3; i++)
	{
		for (int j = 0; j <= 3; j++)
		{
			if (block[currentBlock][blockRotate][i][j])
			{
				int dy = blockY + i, dx = blockX + j;
				if (f[dy][dx] == 1 || dy >= HEIGHT || dy < 0 || dx >= WIDTH || dx < 0)
					continue;
				else
				{
					if (dy + 1 == HEIGHT || f[dy + 1][dx] == 1)
						sc++;
					f[dy][dx] = 1;
				}
			}
		}
	}
	// Block이 추가된 영역의 필드값을 바꾼다.
	return sc * 10;
}

int DeleteLine(char f[HEIGHT][WIDTH])
{
	// user code
	int count = 0;
	for (int i = 0; i < HEIGHT; i++)
	{
		bool chk = true;
		for (int j = 0; j < WIDTH; j++)
		{
			if (f[i][j] == 0)
			{
				chk = false;
				break;
			}
		}
		if (chk)
		{
			if (i == 0)
				for (int j = 0; j < WIDTH; j++)
					f[0][j] = 0;
			else
			{
				for (int j = i; j >= 1; j--)
				{
					for (int k = 0; k < WIDTH; k++)
					{
						f[j][k] = f[j - 1][k];
					}
				}
				for (int j = 0; j < WIDTH; j++)
					f[0][j] = 0;
				count++;
			}
		}
	}
	return count * count * 100;
	// 1. 필드를 탐색하여, 꽉 찬 구간이 있는지 탐색한다.
	// 2. 꽉찬 구간이 있으면 해당 구간을 지운다.즉, 해당 구간으로 필드값을 한칸씩 내린다.
}

///////////////////////////////////////////////////////////////////////////

void DrawShadow(int y, int x, int blockID, int blockRotate)
{
	while (CheckToMove(field, blockID, blockRotate, y + 1, x))
	{
		y++;
	}
	DrawBlock(y, x, blockID, blockRotate, '/');
}

void createRankList()
{
	// 목적: Input파일인 "rank.txt"에서 랭킹 정보를 읽어들임, 읽어들인 정보로 랭킹 목록 생성
	// 1. "rank.txt"열기
	// 2. 파일에서 랭킹정보 읽어오기
	// 3. LinkedList로 저장
	// 4. 파일 닫기
	FILE *fp;
	int i, j;

	// 1. 파일 열기
	fp = fopen("rank.txt", "r");

	if (fscanf(fp, "%d", &score_num) != EOF)
	{
		Node *curr = head;
		for (int i = 0; i < score_num; i++)
		{
			Node *nd = malloc(sizeof(Node));
			nd->link = NULL;
			fscanf(fp, "%s %d", nd->name, &(nd->score));
			if (!head)
			{
				head = nd;
				curr = head;
			}
			else
			{
				curr->link = nd;
				curr = curr->link;
			}
		}
	}
	else
	{
		score_num = 0;
	}
	// 4. 파일닫기
	fclose(fp);
}

void rank()
{
	//목적: rank 메뉴를 출력하고 점수 순으로 X부터~Y까지 출력함
	// 1. 문자열 초기화
	int X = 1, Y = score_num, ch, i, j;
	clear();

	// 2. printw()로 3개의 메뉴출력
	printw("1. list ranks from X to Y\n");
	printw("2. list ranks by a specific name\n");
	printw("3. delete a specific rank\n");
	// 3. wgetch()를 사용하여 변수 ch에 입력받은 메뉴번호 저장
	ch = wgetch(stdscr);
	// 4. 각 메뉴에 따라 입력받을 값을 변수에 저장
	// 4-1. 메뉴1: X, Y를 입력받고 적절한 input인지 확인 후(X<=Y), X와 Y사이의 rank 출력
	if (ch == '1')
	{
		echo();
		printw("X: ");
		scanw("%d", &X);
		printw("Y: ");
		scanw("%d", &Y);
		if (X > Y || X < 1 || Y > score_num)
		{
			printw("search failure: no rank in the list\n");
		}
		else
		{
			printw("       name       |   score   \n");
			printw("------------------------------\n");
			Node *curr = head;
			for (int i = 1; i <= Y; i++)
			{
				if (i >= X)
				{
					printw(" %-17s| %-10d\n", curr->name, curr->score);
				}
				curr = curr->link;
			}
		}
	}

	// 4-2. 메뉴2: 문자열을 받아 저장된 이름과 비교하고 이름에 해당하는 리스트를 출력
	else if (ch == '2')
	{
		char str[NAMELEN + 1];
		int check = 0;
		Node *curr = head;
		echo();
		printw("Input the name: ");
		scanw("%s", str);

		printw("       name       |   score   \n");
		printw("------------------------------\n");

		while (curr)
		{
			if (strcmp(curr->name, str) == 0)
			{
				printw(" %-17s| %-10d\n", curr->name, curr->score);
				check = 1;
			}
			curr = curr->link;
		}
		if (!check)
			printw("\nsearch failure: no name in the list\n");
	}

	// 4-3. 메뉴3: rank번호를 입력받아 리스트에서 삭제
	else if (ch == '3')
	{
		int num;
		echo();
		printw("Input the rank: ");
		scanw("%d", &num);
		if (num < 1 || num > score_num)
		{
			printw("\nsearch failure: the rank not in the list\n");
		}
		else
		{
			Node *prev = NULL, *curr = head;
			for (int i = 0; i < num - 1; i++)
			{
				prev = curr;
				curr = curr->link;
			}
			if (curr)
			{
				if (prev)
					prev->link = curr->link;
				else
					head = curr->link;
				free(curr);
				score_num--;
				printw("\nresult: the rank deleted\n");
				writeRankFile();
			}
			else
			{
				printw("\nsearch failure: the rank not in the list\n");
			}
		}
		noecho();
	}
	getch();
}

void writeRankFile()
{
	// 목적: 추가된 랭킹 정보가 있으면 새로운 정보를 "rank.txt"에 쓰고 없으면 종료
	int sn, i;
	// 1. "rank.txt" 연다
	FILE *fp = fopen("rank.txt", "w");

	// 2. 랭킹 정보들의 수를 "rank.txt"에 기록
	fprintf(fp, "%d\n", score_num);
	Node *curr = head;
	while (curr)
	{
		fprintf(fp, "%s %d\n", curr->name, curr->score);
		curr = curr->link;
	}

	fclose(fp);
}

void newRank(int score)
{
	// 목적: GameOver시 호출되어 사용자 이름을 입력받고 score와 함께 리스트의 적절한 위치에 저장
	char str[NAMELEN + 1];
	int i, j;
	if (!recOn)
	{
		clear();
		// 1. 사용자 이름을 입력받음
		echo();
		printw("your name: ");
		scanw("%s", str);
		noecho();
	}
	else
	{
		strcpy(str, "RecommendPlay");
	}
	score_num++;
	Node *nd = malloc(sizeof(Node)), *prev = 0, *curr = head;
	strcpy(nd->name, str);
	nd->score = score;
	nd->link = NULL;

	while (curr)
	{
		if (score > curr->score)
			break;
		prev = curr;
		curr = curr->link;
	}
	// 2. 새로운 노드를 생성해 이름과 점수를 저장, score_number가
	if (!prev)
	{
		head = nd;
	}
	else
	{
		prev->link = nd;
		nd->link = curr;
	}
}

void DrawRecommend(int y, int x, int blockID, int blockRotate)
{
	DrawBlock(y, x, blockID, blockRotate, 'R');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate)
{
	if (!recOn)
		DrawShadow(y, x, blockID, blockRotate);
	DrawRecommend(recommendY, recommendX, blockID, recommendR);
	if (!recOn)
		DrawBlock(y, x, blockID, blockRotate, ' ');
}

int recommend(RecNode *root)
{
	int cnt = 0, max = 0;
	if (root->lv < BLOCK_NUM)
	{
		for (int r = 0; r < 4; r++)
		{
			for (int x = -4; x < WIDTH + 3; x++)
			{
				int y = 0;
				// 내릴 수 있는 곳 까지 내림
				int chk = 0;
				if (CheckToMove(root->f, nextBlock[root->lv], r, y, x))
				{
					chk = 1;
					while (CheckToMove(root->f, nextBlock[root->lv], r, y + 1, x))
						y++;
				}

				if (!chk)
					continue;

				root->c[cnt] = malloc(sizeof(RecNode));
				memorysize += sizeof(RecNode);
				// root의 필드 상태 복사
				for (int i = 0; i < HEIGHT; i++)
				{
					for (int j = 0; j < WIDTH; j++)
					{
						root->c[cnt]->f[i][j] = root->f[i][j];
					}
				}

				// lv, f, score 저장
				root->c[cnt]->score = root->score;
				root->c[cnt]->score += AddBlockToField(root->c[cnt]->f, nextBlock[root->lv], r, y, x);
				root->c[cnt]->score += DeleteLine(root->c[cnt]->f);
				root->c[cnt]->lv = root->lv + 1;
				root->c[cnt]->score += recommend(root->c[cnt]);
				if (root->c[cnt]->score > max)
				{
					max = root->c[cnt]->score;
					if (!(root->lv))
					{
						recommendR = r;
						recommendY = y;
						recommendX = x;
					}
				}
				cnt++;
			}
		}

		for (int i = 0; i < cnt; i++)
		{
			free(root->c[i]);
		}
	}
	return max;
}

int modified_recommend(RecNode *root)
{
	int cnt = 0, max = 0, dy = -1;
	if (root->lv < BLOCK_NUM)
	{
		for (int r = 0; r < 4; r++)
		{
			if ((nextBlock[root->lv] == 0 && r >= 2) || (nextBlock[root->lv] == 4 && r >= 1) || (nextBlock[root->lv] == 5 && r >= 2) || (nextBlock[root->lv] == 6 && r >= 2))
				continue;
			for (int x = -4; x < WIDTH + 3; x++)
			{
				int y = 0;
				// 내릴 수 있는 곳 까지 내림
				int chk = 0;
				if (CheckToMove(root->f, nextBlock[root->lv], r, y, x))
				{
					chk = 1;
					while (CheckToMove(root->f, nextBlock[root->lv], r, y + 1, x))
						y++;
				}

				if (!chk)
					continue;

				root->c[cnt] = malloc(sizeof(RecNode));
				memorysize += sizeof(RecNode);
				// root의 필드 상태 복사
				for (int i = 0; i < HEIGHT; i++)
				{
					for (int j = 0; j < WIDTH; j++)
					{
						root->c[cnt]->f[i][j] = root->f[i][j];
					}
				}

				// lv, f, score 저장
				root->c[cnt]->score = root->score;
				root->c[cnt]->score += AddBlockToField(root->c[cnt]->f, nextBlock[root->lv], r, y, x);
				root->c[cnt]->score += DeleteLine(root->c[cnt]->f);
				root->c[cnt]->lv = root->lv + 1;
				root->c[cnt]->score += modified_recommend(root->c[cnt]);
				root->c[cnt]->y = y;
				root->c[cnt]->score += y * 500;
				if (root->c[cnt]->score >= max)
				{
					max = root->c[cnt]->score;
					if ((root->c[cnt]->score == max) && (root->c[cnt]->y > dy))
						dy = root->c[cnt]->y;
					if (!(root->lv))
					{
						recommendR = r;
						recommendY = y;
						recommendX = x;
					}
				}
				cnt++;
			}
		}

		for (int i = 0; i < cnt; i++)
		{
			free(root->c[i]);
		}
	}
	return max;
}

void recommendedPlay()
{
	recOn = 1;
	play();
	recOn = 0;
}

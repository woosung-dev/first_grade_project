#pragma warning (disable:4996) //오류 제거용
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <conio.h>	//키보드 입력 함수 사용을 위해 [인터넷 참조]
#include<MMSystem.h>
#pragma comment(lib,"Winmm.lib")


#define true 1	//참 거짓 값 매크로
#define FALSE 0

#define LEFT 75	 /*키보드 입력 값들(아스키)*/
#define RIGHT 77
#define UP 72
#define DOWN 80
#define SPACE 32

#define BY 2	/*맵 생성을 위한 변수*/
#define BX 3		
#define BH 24		
#define BW 10		
#define MX 44 //멀티 위치를 위한 매크로 값
#define max(a,b) (((a)>(b))?(a):(b))	//둘중 큰값 반환 매크로


	void gotoxy(int x, int y) { //gotoxy함수 [인터넷 참고]
		COORD pos = { x,y };
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
	}

	CONSOLE_CURSOR_INFO cursorInfo = { 0, };//커서 숨기기용 구조체 [인터넷 참고]

	void coursor_N() {			//커서 숨기는 함수[인터넷 참고]
		cursorInfo.bVisible = FALSE;
		cursorInfo.dwSize = 100;
		SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	}
	


	int loginDesign();//로그인 화면 디자인

	int signUp();//회원가입
	int titleDesign(); //게임시작화면 디자인

	void controlsPresent(void);// 컨트롤조작키 설명
	void drawSquare(int left, int top, int right, int bottom);//사각형그리기 좌/상/우/하


	void comboPrint(int x); // 블록 파괴시 콤보 점수를 화면에 출력함수 
	void showBlock(int show);	// 블록 				/*http://yk209.tistory.com/entry/c%EC%96%B8%EC%96%B4-%ED%85%8C%ED%8A%B8%EB%A6%AC%EC%8A%A4-%EA%B2%8C%EC%9E%84-%EC%86%8C%EC%8A%A4 */
	void showNBlock(int show);	// 다음 블록			/*http://kkikkodev.tistory.com/71 */
	void showShadow(int show);	// 그림자				//블록 생성에 대해 이해를 위해 참조함
	int movedown(); // 블록을 한칸 내리는 함수
	int checkAround(int x, int y, int z, int r); // 주변 검사 함수		
	int checkLine(); // 블록 줄 검사 후 판단하여 삭제 [+삭제된 줄수를 반환 하는 함수]	
	int inputKey(); // 키 입력 함수		
	int gameOver();	//게임종료시 화면 띄우기
	void pause(); // 일시 정지 
	void levelupBonus(); // 일정 레벨 도달시 이벤트 발생 함수	
	void gamePage();	// 게임 화면 
	void boardPage(); //  벽 안의 블록 상황 화면에 출력		
	void gameScore(int x, int y); // 게임 점수판 화면에 보여주는 함수 
	void gamelevel(); // 게임 [레벨+ 레벨업 조건] 화면에 보여주는 함수
	
	void arrBlock(int show);	//배열 (생성 / 지움) 함수
	int MultiMain(int,int); //멀티용 함수
	void addLine(int atk);		//라인 추가       
		

	enum { EMPTY, BLOCK, NBLOCK, WALL, SHADOW, LV, ATK };	//빈공간, 움직이는블록, 이동완료블록, 벽, 그림자,이벤트,공격
	
	const char *Tiles[7] = { "  ","■","▩","▦","□" ,"◈", "▣" };	//위 열거형과 순서 같음

	int blocks[7][4][8] = { { { 0,-1,1,-1,-1,0,0,0 },{ -1,-1,-1,0,0,0,0,1 },{ 0,-1,1,-1,-1,0,0,0 },{ -1,-1,-1,0,0,0,0,1 } },

	{ { 0,0,1,0,0,-1,-1,-1 },{ -1,1,-1,0,0,0,0,-1 },{ 0,0,1,0,0,-1,-1,-1 },{ -1,1,-1,0,0,0,0,-1 } },

	{ { -2,1,-1,1,0,1,1,1 },{ -1,2,-1,1,-1,0,-1,-1 },{ -2,1,-1,1,0,1,1,1 },{ -1,2,-1,1,-1,0,-1,-1 } }, // ㅣ 

	{ { -1,1,0,1,1,1,-1,0 },{ -1,1,0,1,0,0,0,-1 },{ 0,1,0,0,-1,0,-2,0 },{ -1,2,-1,1,-1,0,0,0 } }, // ㄴ 

	{ { -1,1,0,1,-1,0,0,0 },{ -1,1,0,1,-1,0,0,0 },{ -1,1,0,1,-1,0,0,0 },{ -1,1,0,1,-1,0,0,0 } }, // ㅁ 

	{ { -2,1,-1,1,0,1,0,0 },{ 0,2,0,1,0,0,-1,0 },{ -1,1,-1,0,0,0,1,0 },{ -1,1,0,1,-1,0,-1,-1 } },

	{ { 0,1,-1,0,0,0,1,0 },{ 0,1,0,0,1,0,0,-1 },{ -1,0,0,0,1,0,0,-1 },{ 0,1,0,0,-1,0,0,-1 } } }; // ㅗ 

	int SingleBoard[BW + 2][BH + 2];			//싱글전체[맵 x,y]
	int MultiBoard[BW + 2][BH + 2];				//멀티전체[맵 x y]
	int Block, Shape, nextBlock, nextShape; // 블록 종류, 모양 
	int nx, ny; // 좌표값 
	int Frame = 20, Stay; // 블록 내려오는 속도
	int score = 0, bestScore=0;		//점수값 ,최고기록 점수
	int level = 1,BreakBlocks = 0; // 레벨, 부서진 블록수 레벨업 조건
	
	int main() {

		coursor_N();	//커서 숨기기 함수
		char ch;
		int i, j;
		int Mod;
		srand((unsigned)time(NULL)); //난수표생성 

		while (loginDesign() != 1) {}

		Mod = titleDesign();
		switch (Mod)
		{
		case 1:
			
			srand((unsigned)time(NULL));
			system("cls");		//초기화

			for (i = 0;i<BW + 2;i++) {	//맵 생성을 위해 배열에 값을 넣는 2중 반복문
				
				for (j = 0;j < BH + 2;j++){

					SingleBoard[i][j] = (i == 0 || i == BW + 1 || j == BH + 1)? WALL : EMPTY;
				}
			}
			
			gamePage();	//[맵+부화면 ex)스코어 ] 화면에 띄우는 함수
			Block = rand() % 7;		//블록 랜덤생성
			Shape = rand() % 4;		// 블록 모양 랜덤 생성

			sndPlaySoundA("A.wav", SND_ASYNC | SND_LOOP);
			
			while (true) 
			{
				nextBlock = rand() % 7;
				nextShape = rand() % 4; // 그다음 블럭 

				nx = BW / 2;	//5 중앙에 생성하기 위해서
				ny = 1;

				showNBlock(TRUE);
				showShadow(TRUE);//그림자 생성
				showBlock(TRUE);//블록 생성

				if (checkAround(nx, ny, Block, Shape) != 0) {
					arrBlock(true);
					Sleep(1000);//의미 있는지는 모르겠음..
					PlaySound(NULL, NULL, NULL);
					gameOver();
					break;
				}//게임 종료조건

				Stay = Frame;
				arrBlock(true);
				
				while(true) {			//블록 이동 하는 반복문 [블록이 땅에 닿으면 빠져나감]	
					
					if (--Stay == 0) {		//게임진행 속도를 위한 if문

						Stay = Frame;
						arrBlock(FALSE);	//검사기를 사용함으로 배열을 잠시 지워주기 위해
						if (movedown()) {
							break;	//블록이 땅에 닿으면 반복문 빠져나감
						}
						
					}

					if (inputKey())break;
					arrBlock(true);
					Sleep(50);
				}

				showNBlock(FALSE);
				Block = nextBlock;
				Shape = nextShape;
			}
			
			break;

		case 2:

			srand((unsigned)time(NULL));
			system("cls");		//초기화

			for (i = 0;i<BW + 2;i++) {	//맵 생성을 위해 배열에 값을 넣는 2중 반복문

				for (j = 0;j < BH + 2;j++) {

					SingleBoard[i][j] = (i == 0 || i == BW + 1 || j == BH + 1) ? WALL : EMPTY;
				}
			}


			gamePage();	//[맵+부화면 ex)스코어 ] 화면에 띄우는 함수
			Block = rand() % 7;		//블록 랜덤생성
			Shape = rand() % 4;		// 블록 모양 랜덤 생성
			PlaySound(TEXT("tg.wav"),NULL, SND_ASYNC | SND_LOOP);
			while (true)
			{
				nextBlock = rand() % 7;
				nextShape = rand() % 4; // 그다음 블럭 

				nx = BW / 2;	//5 중앙에 생성하기 위해서
				ny = 1;

				showNBlock(TRUE);
				showShadow(TRUE);//그림자 생성
				showBlock(TRUE);//블록 생성

				if (checkAround(nx, ny, Block, Shape) != 0) {
					arrBlock(true);
					Sleep(1000);//의미 있는지는 모르겠음..
					PlaySound(NULL, NULL, NULL);
					gameOver();
					break;
				}//게임 종료조건

				Stay = Frame;
				arrBlock(true);
				

				while (true) {			//블록 이동 하는 반복문 [블록이 땅에 닿으면 빠져나감]
					MultiMain(66, 30);
					if (--Stay == 0) {

						Stay = Frame;
						
						arrBlock(FALSE);	//검사기를 사용함으로 배열을 잠시 지워주기 위해
						
						if (movedown()) {
							//*요부분에 멀티 데이터 값 화면에 출력하는 함수*//
							

							break;	//블록이 땅에 닿으면 반복문 빠져나감
						}
					}

					if (inputKey())break;
					arrBlock(true);
					Sleep(50);
				}

				showNBlock(FALSE);
				Block = nextBlock;
				Shape = nextShape;
			}
			//멀티
		default:
			break;
		}
}
	
	
	int loginDesign(void) {

		int x = 10; //타이틀화면이 표시되는 x좌표 
		int y = 5; //타이틀화면이 표시되는 y좌표 
		char Choose;


		while (true) {
			system("cls");
			gotoxy(x, y + 0);printf("■■■■■■  ■■■■■■  ■■■■■■  ■■■■■■  ■■  ■■■■■■     \n");Sleep(100);
			gotoxy(x, y + 1);printf("    ■■      ■■              ■■      ■■      ■  ■■  ■■      ■■   \n");Sleep(100);
			gotoxy(x, y + 2);printf("    ■■      ■■              ■■      ■■    ■■  ■■    ■■           \n");Sleep(100);
			gotoxy(x, y + 3);printf("    ■■      ■■■■■■      ■■      ■■  ■■    ■■      ■■         \n");Sleep(100);
			gotoxy(x, y + 4);printf("    ■■      ■■              ■■      ■■■        ■■         ■■      \n");Sleep(100);
			gotoxy(x, y + 5);printf("    ■■      ■■              ■■      ■■  ■■    ■■  ■■      ■■   \n");Sleep(100);
			gotoxy(x, y + 6);printf("    ■■      ■■■■■■      ■■      ■■    ■■  ■■    ■■■■■■   \n");Sleep(100);
			gotoxy(x + 10, y + 13);printf("Connection complete Welcome...");//접속완료 환영합니다.
			gotoxy(x + 10, y + 20);printf("[1] 회원 가입 [2] 로그인 [3]종료\n\t");

			Choose = getch();
			if (Choose == '1') {
				//SignUp();
			}//회원가입
			else if (Choose == '2') { return 1; }//로그인
			else if (Choose == '3') { exit(-1); }//종료
			else { gotoxy(x + 10, y + 30); printf("잘못입력하였습니다 다시 입력해주세요."); Sleep(1400); }
		}
	}

	int titleDesign(void) {

		int x = 10; //타이틀화면이 표시되는 x좌표 
		int y = 5; //타이틀화면이 표시되는 y좌표 
		int Choose;//무한루프 탈출 조건문

		while (true)
		{
			system("cls");
			gotoxy(x, y + 0);printf("□■■■■■■□■■■■■■□■■■■■■□■■■■■■□■■□■■■■■■□□ \n");Sleep(100);
			gotoxy(x, y + 1);printf("□□□■■□□□■■□□□□□□□■■□□□■■□□□■□■■□■■□□□■■□ \n");Sleep(100);
			gotoxy(x, y + 2);printf("□□□■■□□□■■□□□□□□□■■□□□■■□□■■□■■□□■■□□□□□ \n");Sleep(100);
			gotoxy(x, y + 3);printf("□□□■■□□□■■■■■■□□□■■□□□■■□■■□□■■□□□□■■□□□ \n");Sleep(100);
			gotoxy(x, y + 4);printf("□□□■■□□□■■□□□□□□□■■□□□■■■□□□□■■□□□□□■■□□ \n");Sleep(100);
			gotoxy(x, y + 5);printf("□□□■■□□□■■□□□□□□□■■□□□■■□■■□□■■□■■□□□■■□ \n");Sleep(100);
			gotoxy(x, y + 6);printf("□□□■■□□□■■■■■■□□□■■□□□■■□□■■□■■□□■■■■■■□ \n");Sleep(100);
			gotoxy(x + 10, y + 8);printf("Please Enter want Key to Start..");
			gotoxy(x + 20, y + 13);printf("                                                        "); //welcom글씨 지우기위해

			gotoxy(x + 10, y + 10); printf("□1.SinglePlay □    ");
			gotoxy(x + 10, y + 13); printf("■2.Multi Play ■    ");
			gotoxy(x + 10, y + 16); printf("□3. Controls  □    ");
			gotoxy(x + 10, y + 19); printf("■4.   EXIT    ■\n\n");
			Sleep(10); // 00.1초 딜레이  
			Choose = getch();
			if (Choose == '1') return 1; //키입력이 있으면 무한루프 종료
			else if (Choose == '2')return 2;
			else if (Choose == '3') {
				//조작키 함수
				controlsPresent();
			}
			else if (Choose == '4') { exit(-1); }
			else { printf("\n\t 잘못입력하였습니다 다시 입력해주세요 : ");Sleep(1400); }
		}
	}

	
	void controlsPresent(void) {
		system("cls");
		int x = 25, y = 9;
		char ch;

		drawSquare(x - 1, y - 1, x + 30, y + 7);
		gotoxy(x + 10, y - 1);
		printf("%c", 22);
		gotoxy(x + 10, y + 7);
		printf("%c", 21);
		
		gotoxy(x, y);
		printf(" ←       %c Move Left     ", 5);
		gotoxy(x, y + 1);
		printf(" →       %c Move Right     ", 5);
		gotoxy(x, y + 2);
		printf(" ↓       %c Soft Drop      ", 5);
		gotoxy(x, y + 3);
		printf(" ↑       %c Rotate Right   ", 5);
		gotoxy(x, y + 4);
		printf(" Space    %c Hard Drop      ", 5);
		gotoxy(x, y + 5);
		printf(" s        %c Pause          ", 5);
		gotoxy(x, y + 6);
		printf("          %c            \n\n", 5);
				
		Sleep(2000);	//2초 대기
		ch = getch();		//키입력시 함수빠져나감
		return;

	}


	void drawSquare(int left, int top, int right, int bottom) {
		int i, j;
		for (i = 0; i <= bottom - top; i++) {
			if (i == 0) {
				gotoxy(left, top + i);
				printf("%c", 1);
				for (j = 0; j <= right - left - 2; j++)
					printf("%c", 6);
					printf("%c", 2);
			}
			else if (i == bottom - top) {
				gotoxy(left, top + i);
				printf("%c", 3);
				for (j = 0; j <= right - left - 2; j++)
				printf("%c", 6);
				printf("%c", 4);
			}
			else {
				gotoxy(left, top + i);
				printf("%c", 5);
				gotoxy(right, top + i);
				printf("%c", 5);
			}
		}
		return;
	}

	int signUp(void) {
		char ID[20];

		system("cls");
		printf("회원가입 요청중..");
		while (true) {
			printf("ID를 입력하세요 : ");
			scanf("%s", ID);
			//if ()//아이디 이미 존재 오류 printf("이미 존재하는 아이디 입니다.");
			//ID[0]=NULL;
		}
	}

		
	void showBlock(int show) {

		int i;
		
		for (i = 0;i<8;i += 2) {

			gotoxy(BX + (blocks[Block][Shape][i] + nx) * 2, BY + blocks[Block][Shape][i + 1] + ny);
			puts(Tiles[show ? BLOCK : EMPTY]); // show값이 참이면 블럭, 거짓이면 지움 
		}
	}


	void gamePage() {  // 게임화면 출력 

		int i, j;

		for (i = 0;i<BW + 2;i++) {			//게임화면 맵 그리는 반복문

			for (j = 0;j<BH + 2;j++) {
				gotoxy(BX + i * 2, BY + j);
				puts(Tiles[SingleBoard[i][j]]);
			}
		}

		gotoxy(28, 2);printf("ㅁㅁㅁ Next Block ㅁㅁㅁ\n");
		gotoxy(28, 3);printf("ㅁ                    ㅁ\n");
		gotoxy(28, 4);printf("ㅁ                    ㅁ\n");
		gotoxy(28, 5);printf("ㅁ                    ㅁ\n");
		gotoxy(28, 6);printf("ㅁ                    ㅁ\n");
		gotoxy(28, 7);printf("ㅁ                    ㅁ\n");
		gotoxy(28, 8);printf("ㅁ                    ㅁ\n");
		gotoxy(28, 9);printf("ㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁㅁ\n");

		gameScore(0, 0);
		gamelevel();//레벨 레벨업 조건 블록 클리어 수

	}


	int inputKey() { // 키 누르기 

		int ch;
		arrBlock(FALSE);	//오류를 막기위해 배열 잠시 지움

		if (kbhit()) {

			ch = getch();

			switch (ch) {

			case 'S':

			case 's':

				pause();

				break;

			case LEFT:

				if (checkAround(nx - 1, ny, Block, Shape) == EMPTY) {

					showBlock(FALSE);
					showShadow(FALSE);

					nx--;

					showShadow(TRUE);
					showBlock(TRUE);
					//addLine(2);
				}

				break;

			case RIGHT:

				if (checkAround(nx + 1, ny, Block, Shape) == EMPTY) {

					
					showBlock(FALSE);
					showShadow(FALSE);

					nx++;
					//addLine(2);
					showShadow(TRUE);
					showBlock(TRUE);
				}

				break;

			case UP:
				for (int i = 0; i < 2; i++)
				{
					if (checkAround(nx, ny, Block, (Shape + 1) % 4) == WALL) {
						if (nx < 3) {
							showBlock(FALSE);
							showShadow(FALSE);
							nx++;
						}

						else if (nx > 9) {

							showBlock(FALSE);
							showShadow(FALSE);
							nx--;
						}
					}
				}
				
		
				if (checkAround(nx, ny, Block, (Shape + 1) % 4) == EMPTY) {
					
						showBlock(FALSE);
						showShadow(FALSE);

						Shape = (Shape + 1) % 4;	//모양 돌리기
						
						showShadow(TRUE);
						showBlock(TRUE);
					
				}

				break;

			case DOWN:

				if (movedown()) {	//끝까지 내려가면 실행

					return TRUE;
				}

				break;

			case SPACE:		//스페이스
				
				while (movedown() == FALSE) {}		//다내려갈때까지 반복

				return TRUE;

			}
		}

		return FALSE;
	}


	int checkAround(int x, int y, int z, int r) { // 주변 블럭 검사 

		int i, k = EMPTY;

		for (i = 0;i<8;i += 2) {

			k = max(k, SingleBoard[x + blocks[z][r][i]][y + blocks[z][r][i + 1]]); // 두개를 비교하여 블럭이있다면 양수값 반환 
																			 //a랑 b를 비교하여 큰값 반환
		}
		return k; // 0이면 거짓 그외값이면 참 
	}


	int movedown() { // 아래로 한칸 내리기 

		if (checkAround(nx, ny + 1, Block, Shape) != EMPTY) {	//한칸 내릴수 없으면 실행

			checkLine();

			return TRUE;

		}

		showBlock(FALSE);

		ny++;

		showBlock(TRUE);

		return FALSE;
	}


	int checkLine() { // 줄 검사 + 블록 처리[화면,배열]

		int i, j, x, y, combo = 0, Line=0;

		for (i = 0;i<8;i += 2) {		//블록,배열 nBLOCK처리
			gotoxy(BX + (blocks[Block][Shape][i] + nx) * 2, BY + blocks[Block][Shape][i + 1] + ny);
			SingleBoard[nx + blocks[Block][Shape][i]][ny + blocks[Block][Shape][i + 1]] = NBLOCK;
			puts(Tiles[NBLOCK]);
		}

		for (i = 1;i<BH + 1;i++) {		//세로

			for (j = 1;j<BW + 1;j++) {		//가로

				if (SingleBoard[j][i] != NBLOCK && SingleBoard[j][i] != ATK)break;

			} // 줄 검사 


			if (j == BW + 1) {	//한줄 꽉찼으면 

				comboPrint(++combo);
				Line++;		//부서진 라인 변수
				
				for (y = i;y>1;y--) {		//배열 윗줄 내리기 2중 반복문

					for (x = 1;x<BW + 1;x++) {

						SingleBoard[x][y] = SingleBoard[x][y - 1];	//윗줄 배열 내리기
					}
				}

				BreakBlocks++;
				gamelevel();	//레벨 화면 
				Sleep(500);
				boardPage();	//게임 박스안 블럭들
			}
		}
		combo = 0;
		levelupBonus();//레벨업 조건 + 보너스
		if (Line > 2) { Line *= 2; }
		return Line;
	}

	
	void boardPage() { // 게임 박스 안 블럭들출력 

		int i, j;
		
		for (i = 1;i<BW + 1;i++) {	//맵 안의 블록출력 2중반복문

			for (j = 1;j<BH + 1;j++) {

				gotoxy(BX + i * 2, BY + j);	puts(Tiles[SingleBoard[i][j]]);
				
			}
		}
	}

	
	void showNBlock(int show) { // 다음 블럭 출력 

		int i;

		for (i = 0;i<8;i += 2) {

			gotoxy(40 + blocks[nextBlock][nextShape][i] * 2, 5 + blocks[nextBlock][nextShape][i + 1]);	//지정위치에 블록 출력
			//x*2는 화면상 ㅁ가 [가로 2칸 세로 1칸으로 보여지기때문에]
			puts(Tiles[show ? BLOCK : EMPTY]); // show값이 참이면 블럭, 거짓이면 지움 
		}
	}

	
	void gameScore(int a, int b) { // 게임 점수 

		score += a;
		score += b;
		if (score > bestScore) { bestScore = score; }
		gotoxy(30, 24);printf("Score : %d", score);
		gotoxy(30, 22);printf("bsetScore : %d", bestScore);
	}
	

	void showShadow(int show) {
		int i;
		int yy;

		for (yy = ny;checkAround(nx, yy, Block, Shape) == EMPTY;yy++);		//지금 블록 empty가 아닐때까지 반복

		for (i = 0;i<8;i += 2) {

			gotoxy(BX + (blocks[Block][Shape][i] + nx) * 2, BY + blocks[Block][Shape][i + 1] + yy - 1);//empty상태로 보여주기위해 [yy-1]
			
			puts(Tiles[show ? SHADOW : EMPTY]);

		}
	}
	

	void levelupBonus() {

		int i, j,Check=0;
		
		if (BreakBlocks >= 5) {
						
			if (level < 5) { BreakBlocks %= 5;	//한번에 7~8점되는 상황을 위해
			gotoxy(BX + 8, 8);printf("Leve up!!");
			Check = true;
			}
			
			if (level >= 5) {if (BreakBlocks >= 10){		//레벨이 일정이상 올라가면 이벤트 발생	3칸 지우기
				BreakBlocks %= 10;	
				Check = true;
				gotoxy(BX + 8, 8);	printf("Leve up!!");

					for (i = BH;i > BH - 3;i--) {

						for (j = 1;j < BW + 1;j++) {

							gotoxy(BX + j * 2, BY + i);
							puts(Tiles[LV]);
							SingleBoard[j][i] = LV;
							Sleep(40);
						}
					}

				Sleep(100);

				for (i = BH;i > 3 - 1;i--) {

					for (j = 1;j < BW + 1;j++) {
						SingleBoard[j][i] = SingleBoard[j][i - 3];
					}
				}
			}//레벨	
			}//블록

			Sleep(300);
			boardPage(); //블록안 상황 화면에 출력
			
			if (Check==true)//레벨업 확인
			{
				if (Frame > 4)Frame -= 3;

				else Frame = 4;
				level++;
			}
						
		}
		gamelevel();
		
	}

	void comboPrint(int x) {

		gotoxy(BX + 6, 8);printf("%d combo!", x); // 콤보
		gotoxy(BX + 16, 9);printf("+%d", x*x * 100);
		gotoxy(BX + 16, 10);printf("+%d", level*x * 100);
		
		gameScore(x*x * 100, level*x * 100);
	}
	

	void gamelevel() {	//레벨 + 지워진 블록 표시
		
		gotoxy(30, 26);	printf("Lv : %d", level);
		if (level >= 5) { gotoxy(30, 27); printf("Break block :%2d/10", BreakBlocks); }
		else { gotoxy(30, 27); printf("Break block :%d/5", BreakBlocks); }

	}

	
	void pause() {
		char pause;

		gotoxy(BX + BW , BY + 7);	printf("▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣");
		gotoxy(BX + BW , BY + 8);	printf("▣                              ▣");
		gotoxy(BX + BW , BY + 9);	printf("▣         P  A  U  S  E        ▣");
		gotoxy(BX + BW , BY + 10);	printf("▣                              ▣");
		gotoxy(BX + BW , BY + 11);	printf("▣      아무 키나 누르세요      ▣");
		gotoxy(BX + BW , BY + 12);	printf("▣                              ▣");
		gotoxy(BX + BW , BY + 13);	printf("▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣");
		Sleep(1000);
		pause = getch();
		system("cls");
		boardPage();
		gamePage();

		showNBlock(TRUE);
		showBlock(TRUE);
		showShadow(TRUE);
	}


	int gameOver() {
		char over;

		gotoxy(BX + BW , BY + 7);	printf("▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣");
		gotoxy(BX + BW , BY + 8);	printf("▣                              ▣");
		gotoxy(BX + BW , BY + 9);	printf("▣                              ▣");
		gotoxy(BX + BW , BY + 10);	printf("▣      G A M E    O V E R      ▣");
		gotoxy(BX + BW , BY + 11);	printf("▣                              ▣");
		gotoxy(BX + BW , BY + 12);	printf("▣                              ▣");
		gotoxy(BX + BW , BY + 13);	printf("▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣▣");
		over = getch();
		gotoxy(1, 28);
		return bestScore;
	}
		
	void arrBlock(int show) {

		for (int i = 0;i<8;i += 2) {		

			SingleBoard[nx + blocks[Block][Shape][i]][ny + blocks[Block][Shape][i + 1]] = (show ? BLOCK : EMPTY);

		}
	}
	
	
	int MultiMain(int Mx,int score) {
		
		int i, j;
		//system("cls");		//초기화
		
		for (i = 0;i<BW + 2;i++) {	//맵 생성을 위해 배열에 값을 넣는 2중 반복문

			for (j = 0;j < BH + 2;j++) {

				MultiBoard[i][j] = SingleBoard[i][j];
			}
		}
		
		for (i = 0;i<BW + 2;i++) {	//맵 생성을 위해 배열에 값을 넣는 2중 반복문

			
			for (j = 0;j<BH + 2;j++) {
				
				gotoxy(Mx +BX + i * 2, BY + j);	puts(Tiles[MultiBoard[i][j]]);
				
			}
		}

	
		
		return 1; //성공
	}

	void addLine(int atk)
	{
		int i, j;
		int AT = (rand() % 9) + 1;//랜덤으로 한자리는 비워두기위해

		for (i = atk;i < BH + 1;i++) {
			for (j = 0;j < BW + 1;j++) {
				SingleBoard[j][i - atk] = SingleBoard[j][i];
			}
		}

		for (i = BH;i > BH - atk;i--) {

			for (j = 1;j < BW + 1;j++) {

				gotoxy(BX + j * 2, BY + i);
				if (AT == j)
				{
					SingleBoard[j][i] = EMPTY;	//한칸은 비워주기위해
				}
				else {
					SingleBoard[j][i] = ATK;
					Sleep(40);
				}
			}
			AT = (rand() % 9) + 1;	//랜덤
		}

		Sleep(100);
		boardPage(); //블록안 상황 화면에 출력
		showNBlock(TRUE);
		showBlock(TRUE);	//블록 show
		showShadow(TRUE);	//그림자 show
	}
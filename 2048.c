/*
 * 2048 게임 (컴퓨터 프로그래밍 실습 기말 프로젝트)
 * 소프트웨어학과 202421572 김유진
 *
 * [전체 흐름]
 *   main (시작 메뉴: 로그인 / 회원가입 / 랭킹 / 종료)
 *     └ 로그인 성공 → 게임 메뉴 (게임 시작 / 랭킹 / 내 기록 / 로그아웃)
 *          └ play_game → 끝나면 save_record 로 기록 저장
 *
 * [작업 순서]
 *   PART 1 (게임)       → 혼자 한 판 끝까지 할 수 있으면 완료
 *   PART 2, 3 (회원/기록) → 로그인 → 게임 → 랭킹 흐름이 되면 완료
 *   색 입히기, 이상한 입력 처리, 테스트, 주석 정리
 *
 * 컴파일:  gcc -Wall -o 2048 2048.c
 * 실행:    ./2048
 *
 * 폴더 위치:
 * cd "/Users/yujin/Desktop/SMU/2026/26 3-2/컴프 컴프실/컴프실/기말과제_2048"
 * 깃허브 저장:
 * git add 2048.c
 * git commit -m " ~ 구현"
 * git push
 * 
 * TODO 라고 적힌 곳이 네가 채울 부분이야.
 * 지금은 컴파일만 되도록 return 0; 같은 임시 코드가 들어가 있어.
 */

#include <stdio.h>   // printf, scanf, 파일 입출력(fopen 등)
#include <stdlib.h>  // rand, srand, malloc, free, system
#include <string.h>  // strcmp, strcpy (문자열 비교/복사)
#include <time.h>    // time (랜덤 시드용)

/* ───────────── 상수 ─────────────
 * 숫자를 코드에 직접 쓰지 않고 이름을 붙여두면
 *  1) 코드를 읽을 때 4가 "판 크기"라는 걸 바로 알 수 있고
 *  2) 나중에 바꿀 때 여기 한 곳만 고치면 돼.
 */
#define SIZE 4                    // 판 크기 (4x4)
#define WIN_TILE 2048             // 이 숫자를 만들면 승리
#define NAME_LEN 20               // 아이디/비밀번호 최대 길이
#define USERS_FILE "users.txt"    // 회원 정보 파일
#define RECORDS_FILE "records.txt"// 게임 기록 파일
#define RANK_COUNT 10             // 랭킹에 보여줄 개수

/* ───────────── 구조체 ─────────────
 * 서로 관련된 변수들을 하나로 묶는 것.
 * typedef 를 쓰면 "struct User" 대신 "User" 라고만 써도 돼.
 */
typedef struct {
    char id[NAME_LEN];
    char pw[NAME_LEN];
} User;

typedef struct {
    char id[NAME_LEN];
    int score;
    int max_tile;
} Record;

/* ───────────── 전역 변수 ─────────────
 * 판과 점수는 거의 모든 함수가 쓰기 때문에 전역으로 둠.
 * (매개변수로 넘기는 방식도 있지만, 실습 수준에선 전역이 이해하기 쉬움)
 */
int board[SIZE][SIZE];            // 게임 판. 0 = 빈칸
int score = 0;                    // 현재 점수
int won_shown = 0;                // 2048 축하 메시지를 이미 보여줬는지 (0/1)
char current_user[NAME_LEN] = ""; // 로그인한 사람 아이디

/* ───────────── 함수 원형 ─────────────
 * main 보다 아래에 함수를 만들어도 main 에서 부를 수 있게
 * "이런 함수가 있어" 라고 미리 알려주는 부분.
 */

// PART 0: 도우미
void clear_screen(void);
void clear_input_buffer(void);

// PART 1: 게임 (1일차)
void init_game(void);
void print_board(void);
int  add_random_tile(void);
int  slide_row(int row[SIZE]);
void rotate_board(void);
int  move(char dir);
int  get_max_tile(void);
int  can_move(void);
void play_game(void);
void compress_row(int row[SIZE]);

// PART 2: 회원 (2일차)
int  id_exists(const char *id);
void sign_up(void);
int  login(void);

// PART 3: 기록과 랭킹 (2일차)
void save_record(int final_score, int max_tile);
void show_ranking(void);
void show_my_records(void);

// 메뉴
void game_menu(void);


/* ================================================================
 * main
 * ================================================================ */
int main(void)
{
    srand((unsigned)time(NULL)); // 실행할 때마다 다른 랜덤 결과가 나오게 함 (딱 한 번만 호출!)

    /* TODO (2일차): 시작 메뉴
     *  반복:
     *    메뉴 출력: 1. 로그인  2. 회원가입  3. 랭킹 보기  0. 종료
     *    번호 입력
     *    1 → login() 이 성공(1)하면 game_menu()
     *    2 → sign_up()
     *    3 → show_ranking()
     *    0 → 반복 끝
     *
     *  1일차에는 메뉴 없이 그냥 play_game(); 한 줄만 두고 게임부터 테스트하자.
     */
    board[0][0] = 2;
    board[0][3] = 4;
    print_board();
    rotate_board();
    print_board();

    return 0;
}


/* ================================================================
 * PART 0: 도우미 함수
 * ================================================================ */

/* 화면 지우기 (이건 채워둠)
 * 윈도우는 "cls", 맥/리눅스는 "clear" 명령을 써.
 * #ifdef _WIN32 는 "윈도우에서 컴파일될 때만" 이라는 뜻이라
 * 같은 코드로 두 운영체제에서 다 돌아가게 됨 → 제안서의 "이식성"
 */
void clear_screen(void)
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* 입력 버퍼 비우기
 * scanf 로 숫자를 받을 때 사용자가 "abc" 같은 걸 치면
 * 그 글자들이 버퍼에 남아서 다음 scanf 가 계속 실패해 → 무한 루프.
 * 그래서 줄바꿈('\n')이 나올 때까지 남은 글자를 읽어서 버림.
 *
 * TODO: getchar() 와 while 로 '\n' 이나 EOF 가 나올 때까지 읽어서 버리기
 */
void clear_input_buffer(void)
{
}


/* ================================================================
 * PART 1: 게임 (1일차)
 * ================================================================ */

/* 새 게임 준비
 * TODO:
 *  - board 의 모든 칸을 0 으로
 *  - score, won_shown 을 0 으로
 *  - add_random_tile() 두 번
 */
void init_game(void)
{
    // printf("Score: %d\n\n", score);
    for (int i=0; i<SIZE; i++) // i=행 (위에서 아래로)
    {
        for(int j=0; j<SIZE; j++) // j=열 (왼에서 오로))
        {
            board[i][j] = 0; // 그냥 0을 넣기 (화면상에는 .)
        }
    }
    // 점수랑 축하 메시지 기록 초기화
    score = 0;
    won_shown = 0;

    // 숫자 2개 놓기 - 기본값 세팅
    add_random_tile();
    add_random_tile();

}

/* 판과 점수 출력
 * TODO:
 *  - 위에 점수 출력
 *  - 이중 for 문으로 판 출력
 *      0 인 칸은 "." 처럼 빈칸 표시
 *      숫자는 폭을 맞춰서 (힌트: printf 의 %5d)
 *  - 아래에 조작법 출력 (w/a/s/d 이동, q 종료)
 */
void print_board(void)
{
    printf("Score: %d\n\n", score);
    for (int i=0; i<SIZE; i++) // i=행 (위에서 아래로)
    {
        for(int j=0; j<SIZE; j++) // j=열 (왼에서 오로))
        {
            if (board[i][j] == 0)
                printf("    .");
            else
                printf("%5d", board[i][j]);
        }
        printf("\n");
    }
    printf("\n[w]위 [a]왼쪽 [s]아래 [d]오른쪽  [q]종료\n");
}

/* 빈칸 하나에 2 또는 4 놓기
 * 반환: 놓았으면 1, 빈칸이 없어서 못 놓았으면 0
 *
 * 왜 "빈칸 목록"을 먼저 만들까?
 *  rand() 로 아무 칸이나 찍고 빈칸이 나올 때까지 다시 찍는 방법은
 *  판이 거의 찼을 때 오래 걸리고, 빈칸이 0개면 영원히 안 끝나.
 *
 * TODO:
 *  1) 빈칸의 위치를 배열에 모으기 (힌트: 칸 번호 = i * SIZE + j 로 하나의 정수에 담기)
 *  2) 빈칸 개수가 0 이면 return 0
 *  3) 그중 하나를 rand() % 개수 로 고르기
 *  4) 90% 확률로 2, 10% 확률로 4  (힌트: rand() % 10 == 0 이면 4)
 */
int add_random_tile(void)
{
    int empty[SIZE * SIZE];   // 빈칸 위치들을 담을 배열 (최대 16칸)
    int count = 0;            // 지금까지 찾은 빈칸 개수
    // 1. 빈칸 모으기
    for (int i=0; i<SIZE; i++)
        for (int j=0; j<SIZE; j++)
            {
                if (board[i][j] == 0) // 이 칸이 비어있으면?
                {
                    empty[count] = i * SIZE + j; // 칸 번호 저장
                    count++; // 개수 하나 늘리기
                }
            }
    // 2. 빈칸이 하나도 없으면 못 놓으니까 끝
    if (count == 0)
        return 0;
    
    // 3. 목록에서 하나 뽑기
    int pick = empty[rand() % count];
    
    // 4. 자리 번호 -> (몇번째 줄, 몇번째 칸)으로 바꾸기
    int r = pick / SIZE; // 몫 = 줄
    int c = pick % SIZE; // 나머지 = 칸

    // 5. 10%는 4, 90%는 2
    if (rand() % 10 == 0) // 10번 중 1번
        board[r][c] = 4;
    else                  // 10번 중 9번
        board[r][c] = 2;

    return 1; // 놓는데 성공!

}

/* ★ 핵심 함수 ★  한 줄을 왼쪽으로 밀고 합치기
 * 반환: 이번에 합쳐서 얻은 점수
 *
 * 배열을 매개변수로 받으면 원본 주소가 넘어오기 때문에 (1113 실습 내용!)
 * 여기서 row 를 바꾸면 호출한 쪽 배열도 바뀜.
 *
 * 3단계:
 *  1) 당기기  : 0 이 아닌 숫자만 왼쪽부터 차례로 채우고 나머지는 0
 *  2) 합치기  : 왼쪽부터 보면서 row[k] == row[k+1] 이고 0 이 아니면
 *               row[k] 를 2배, row[k+1] 을 0, 점수에 row[k] 더하기
 *               → 합친 다음엔 k 를 하나 더 건너뛰어야 같은 타일이 두 번 안 합쳐짐!
 *  3) 다시 당기기 : 합치면서 생긴 0 을 메우기
 *
 * 테스트 케이스 (전부 통과해야 함):
 *   [2 2 2 2] → [4 4 0 0]   점수 8
 *   [2 2 4 0] → [4 4 0 0]   점수 4
 *   [0 2 0 2] → [4 0 0 0]   점수 4
 *   [2 4 8 16]→ 그대로       점수 0
 *   [4 4 8 8] → [8 16 0 0]  점수 24
 */
void compress_row(int row[SIZE])
{
    int tmp[SIZE] = {0}; // 전부 0으로 시작하는 새 종이 -> 4칸짜리 배열이고 0으로 채워져있음
    int pos = 0; // tmp에 다음에 쓸 자리

    for (int k=0; k<SIZE; k++)
    {
        if (row[k] != 0) // 읽고 있는 칸이 0이 아니면
        {
            tmp[pos] = row[k]; // 쓸 자리에 옮겨 적고
            pos++; // 쓸 자리를 한 칸 옆으로
        }
    }
    // 칸마다 그대로 복사
    for (int k=0; k<SIZE; k++)
        row[k] = tmp[k];
}

int slide_row(int row[SIZE])
{
    int gained = 0; // 이번에 얻은 점수

    compress_row(row); // 1. 당기기

    for (int k=0; k<SIZE-1; k++) // 2. 합치기
    {
        if (row[k] != 0 && row[k] == row[k+1]) // 0이 아니고, 오른쪽 칸이랑 같으면 
        {
            row[k] = row[k] * 2 ; // 두 배로, row[k] + row[k + 1]
            gained += row[k]; // 합쳐진 값만큼 점수에 더

            row[k+1] = 0; // 오른쪽 칸은 비우기
        }
    }
    compress_row(row); // 3. 다시 당기기

    return gained;
}

/* 판을 시계 방향으로 90도 돌리기
 * 바로 board 에 덮어쓰면 아직 안 옮긴 값이 지워지니까
 * 임시 배열 tmp 에 결과를 만든 다음 board 로 복사해야 해.
 *
 * TODO:
 *  1) board[i][j] 가 tmp 의 어디로 가는지 종이에 4x4 를 그려서 찾기
 *     (힌트: 맨 왼쪽 열이 돌린 뒤 맨 위 행이 됨. 단, 순서가 뒤집혀서)
 *  2) 이중 for 로 tmp 채우기
 *  3) tmp 를 board 로 복사
 */
void rotate_board(void)
{
    int tmp[SIZE][SIZE]; // 돌린 결과를 적을 새 판

    // 1. 새 판 채우기
    for (int r=0; r<SIZE; r++)
        for (int c=0; c<SIZE; c++)
            tmp[r][c] = board[SIZE - 1 -c][r];
    
    // 2. 새 판을 원래 판에 복사
    for (int r=0; r<SIZE; r++)
        for (int c=0; c<SIZE; c++)
            board[r][c] = tmp[r][c];
}

/* 한 방향으로 이동
 * 반환: 판이 바뀌었으면 1, 안 바뀌었으면 0
 *       (안 바뀌었으면 새 타일을 만들면 안 되니까 이 정보가 필요함)
 *
 * 아이디어: 왼쪽 밀기만 만들고, 나머지는 회전으로 해결
 *   'a' 왼쪽 : 회전 0번 → 밀기 → 회전 0번
 *   's' 아래 : 회전 1번 → 밀기 → 회전 3번
 *   'd' 오른쪽: 회전 2번 → 밀기 → 회전 2번
 *   'w' 위   : 회전 3번 → 밀기 → 회전 1번
 *   (앞 회전 + 뒤 회전 = 4번 = 한 바퀴 → 원래 방향으로 돌아옴)
 *
 * TODO:
 *  1) 이동 전 판을 before 배열에 복사
 *  2) dir 에 따라 회전 횟수 정하기
 *  3) 회전 → 모든 행에 slide_row (반환값은 score 에 더하기) → 되돌리는 회전
 *  4) before 와 board 를 비교해서 한 칸이라도 다르면 1, 아니면 0
 */
int move(char dir)
{
    return 0; // 임시
}

/* 판에서 가장 큰 숫자 (기록 저장용, 2048 달성 확인용) */
int get_max_tile(void)
{
    return 0; // 임시
}

/* 아직 움직일 수 있는지 (게임 오버 판정)
 * 반환: 움직일 수 있으면 1
 *
 * TODO: 아래 중 하나라도 있으면 1
 *  - 빈칸(0)이 있다
 *  - 오른쪽 칸과 같은 숫자가 있다
 *  - 아래 칸과 같은 숫자가 있다
 *  (왼쪽, 위는 안 봐도 돼. 왜일까? 생각해 보기)
 *  배열 밖을 보지 않도록 범위 조심!
 */
int can_move(void)
{
    return 0; // 임시
}

/* 게임 한 판
 * TODO:
 *  init_game()
 *  반복:
 *    clear_screen(), print_board()
 *    2048 을 처음 만들었으면 (won_shown 이 0 이고 최대 타일 >= WIN_TILE)
 *        축하 메시지 출력, won_shown = 1   → 게임은 계속!
 *    can_move() 가 0 이면 "게임 오버" 출력하고 반복 끝
 *    키 입력 (힌트: scanf(" %c", &key) — %c 앞 공백이 남은 줄바꿈을 건너뜀)
 *    'q' 면 반복 끝
 *    w/a/s/d 면 move() 하고, 판이 바뀌었으면 add_random_tile()
 *    그 외 글자는 무시
 *
 *  끝나면 최종 점수 출력
 *  (2일차) 로그인한 상태면 save_record(score, get_max_tile())
 */
void play_game(void)
{
}


/* ================================================================
 * PART 2: 회원 (2일차)
 * ================================================================ */

/* 이미 있는 아이디인지
 * 반환: 있으면 1, 없으면 0
 *
 * TODO:
 *  - USERS_FILE 을 "r" 로 열기. NULL 이면(파일이 아직 없음) 회원이 0명이니까 0
 *  - fscanf(fp, "%19s %19s", ...) 가 2 를 반환하는 동안 한 줄씩 읽기
 *    (%19s: 배열 크기 20 중 '\0' 자리 1칸 빼고 19글자까지만 → 넘침 방지)
 *  - strcmp 로 비교해서 같으면 닫고 1
 *  - 파일 닫기 잊지 말기!
 */
int id_exists(const char *id)
{
    return 0; // 임시
}

/* 회원가입
 * TODO:
 *  - 아이디 입력 → id_exists 면 "이미 있는 아이디" 하고 return
 *  - 비밀번호 입력
 *  - USERS_FILE 을 "a"(이어쓰기) 로 열어서 "아이디 비밀번호\n" 저장
 *    ("w" 로 열면 기존 회원이 전부 지워짐!)
 */
void sign_up(void)
{
}

/* 로그인
 * 반환: 성공 1, 실패 0
 * TODO:
 *  - 아이디, 비밀번호 입력
 *  - 파일에서 한 줄씩 읽어 둘 다 같은 줄이 있으면
 *    strcpy(current_user, 아이디) 하고 1
 *  - 없으면 실패 메시지 후 0
 */
int login(void)
{
    return 0; // 임시
}


/* ================================================================
 * PART 3: 기록과 랭킹 (2일차)
 * ================================================================ */

/* 기록 한 줄 추가: "아이디 점수 최대타일\n"
 * TODO: RECORDS_FILE 을 "a" 로 열어서 fprintf, 닫기
 */
void save_record(int final_score, int max_tile)
{
}

/* 랭킹 TOP 10
 * 기록이 몇 개일지 모르니까 배열 크기를 미리 정할 수 없음 → malloc (제안서의 "동적 할당")
 *
 * TODO:
 *  1) 파일을 한 번 다 읽어서 줄 수(count) 세기. 0 이면 "기록 없음"
 *  2) Record *list = malloc(sizeof(Record) * count);  NULL 확인
 *  3) rewind(fp) 로 파일 처음으로 돌아가서 list 에 다 읽어 넣기
 *  4) 점수 내림차순 정렬 (버블 정렬이나 선택 정렬 직접 짜기 — 구조체도 통째로 대입(=) 가능)
 *  5) 앞에서부터 최대 RANK_COUNT 개 출력 (순위, 아이디, 점수, 최대 타일)
 *  6) free(list), fclose(fp)
 */
void show_ranking(void)
{
}

/* 내 기록: 파일을 읽으면서 아이디가 current_user 인 줄만 출력 + 최고 점수 */
void show_my_records(void)
{
}


/* ================================================================
 * 메뉴
 * ================================================================ */

/* 로그인 후 메뉴
 * TODO: 반복
 *   1. 게임 시작 → play_game()
 *   2. 랭킹 보기 → show_ranking()
 *   3. 내 기록  → show_my_records()
 *   0. 로그아웃 → current_user 를 빈 문자열로 하고 return
 */
void game_menu(void)
{
}

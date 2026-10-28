/*
 * DASF004 (2026-2) · 실습#3 — 습관 트래커 ③: 채우고 · 늘리고 · 압축하고
 * ─────────────────────────────────────────────────────────
 * 학번을 붙여 WA3_학번.c 로 저장하고, 제출할 때 확장자를 .txt 로 바꾸세요.
 * 빌드: gcc -Wall -Wextra -std=c11 WA3_학번.c -o wa3
 *
 * [TODO 요구사항 1] ~ [TODO 요구사항 4] — 빈칸 다섯 곳을 채우면 완성입니다.
 *   · 요구사항 2 는 빈칸이 둘입니다 — 2-a 는 파일 맨 위의 매크로, 2-b 는 아래의 함수.
 *   · 파일 스코프 상태·실습#2 승계 함수·main 은 [수정 금지] 입니다.
 *   · 구조체·포인터·동적 메모리는 쓰지 않습니다. (아직 안 배웠습니다)
 *   · 여러분이 만드는 함수의 *매개변수*로 배열을 넘기지 않습니다(W12 내용)
 *     — 상태는 파일 스코프에 있어 어느 함수에서나 그냥 씁니다.
 *     scanf·printf 에 이름(문자열)을 넘기는 것은 6주차에 배운 그대로 씁니다.
 */
#include <stdio.h>
#include <string.h>

#define MAX_HABITS 5
#define MAX_NAME   32
#define MAX_WEEKS  6

/* ── [TODO 요구사항 2-a] 비트 매크로 두 개를 정의한다 ────────────
 * 한 달의 31일을 unsigned int 한 개에 담습니다. d일(1~31)은 (d-1)번 비트입니다.
 *   DAY_BIT(d)     → d일 자리에만 1 이 선 값.   예) DAY_BIT(3) 은 0b100
 *   IS_DONE(m, d)  → m 의 d일 비트가 켜져 있으면 참
 * 함정: 매개변수를 괄호로 감싸세요. 매크로는 「글자 그대로 갈아끼우기」라
 *       괄호가 없으면 DAY_BIT(x+1) 같은 호출에서 엉뚱한 값이 나옵니다.
 * 아래 두 줄의 (0u)·(0) 을 지우고 여러분의 식을 넣으세요. */

#define DAY_BIT(d)      (1u << ((d) - 1))
#define IS_DONE(m, d)   (((m) & DAY_BIT(d)) != 0)

/* ── [수정 금지] 파일 스코프 상태 ─────────────────────────── */
char         habitName[MAX_HABITS][MAX_NAME];
unsigned int doneMask[MAX_HABITS];
int          habitCount = 0;
int          curYear = 2026, curMonth = 1, curDays = 0, curFirstWeekday = 0;

/* ── [수정 금지] 함수 선언 ───────────────────────────────── */
int  readIntInRange(int lo, int hi);
void flushLine(void);
int  isLeapYear(int year);
int  daysInMonth(int year, int month);
int  firstWeekdayOf(int year, int month);
void readHabits(void);
void toggleDay(int h, int d);
void showNameLengthDemo(void);
void printCalendarHeader(int h);
void printCalendar(int h);
void printWeeklyTable(void);

/* ── [수정 금지] 실습#2 의 정답 — 그대로 제공합니다 ───────
 * 실습#2 를 못 하셨어도 이번 실습은 처음부터 하실 수 있습니다. */
/* ── [수정 금지] 입력 줄의 남은 부분을 버린다 ─────────────
 * 이름이 버퍼보다 길면 남은 글자가 입력에 남아 그다음 숫자 읽기를 망친다.
 * (한글은 한 글자가 3바이트라 금방 넘친다)
 * 문자열을 scanf 로 읽은 직후에는 반드시 한 번 부른다. */
void flushLine(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int readIntInRange(int lo, int hi)
{
    int value = 0;
    scanf("%d", &value);
    while (value < lo || value > hi) {
        printf("[오류] %d~%d 사이의 값을 입력하세요.\n> ", lo, hi);
        scanf("%d", &value);
    }
    return value;
}
int isLeapYear(int year)
{
    return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0);
}
int daysInMonth(int year, int month)
{
    if (month == 4 || month == 6 || month == 9 || month == 11) return 30;
    if (month == 2) return isLeapYear(year) ? 29 : 28;
    return 31;
}
int firstWeekdayOf(int year, int month)
{
    long elapsed = 0;
    for (int y = 2000; y < year; y++) elapsed += isLeapYear(y) ? 366 : 365;
    for (int m = 1; m < month; m++)   elapsed += daysInMonth(year, m);
    return (int)((6 + elapsed) % 7);
}

/* ── [수정 금지] W6 문자열 — 길이는 strlen 으로 잽니다 ──────────
 * 고칠 것이 없습니다. 시작할 때 한 번 보여 드리는 참고 출력입니다.
 * strlen 은 <string.h> 에 있고, 널 문자('\0') 앞까지의 **바이트 수**를 셉니다.
 * 한글은 한 글자가 3바이트라 「글자 수」와 「바이트 수」가 다릅니다 —
 * 이름을 %31s 로 받는 이유가 이것입니다(31바이트 + 널 문자 한 칸). */
void showNameLengthDemo(void)
{
    const char *ko = "운동";
    const char *en = "workout";
    printf("--- [참고] 이름의 길이 ---\n");
    printf("  \"%s\"    글자 2개 -> %d바이트\n", ko, (int)strlen(ko));
    printf("  \"%s\" 글자 7개 -> %d바이트\n", en, (int)strlen(en));
    printf("  이름은 %d바이트까지 받습니다.\n", MAX_NAME - 1);
    printf("--------------------------\n");
}

/* ── [수정 금지] 달력 머리 두 줄 — 제목과 요일 헤더 ─────────
 * 한글 요일 헤더는 글자 폭 때문에 손으로 맞추기 까다로워서 드립니다.
 * 요구사항 3 에서 이 함수를 먼저 부르고, 여러분은 격자와 요약만 만들면 됩니다. */
void printCalendarHeader(int h)
{
    printf("\n      %d년 %d월  [%s]\n", curYear, curMonth, habitName[h]);
    printf("  일  월  화  수  목  금  토\n");
}

/* ── [TODO 요구사항 1] 습관 이름을 여러 개 받는다 ─────────────────
 * 1) 아래 프롬프트를 찍고 개수를 1~MAX_HABITS 로 받아 habitCount 에 넣는다.
 *        습관을 몇 개 기록하시겠어요? (1~5):
 * 2) h = 0 부터 habitCount-1 까지 반복하며
 *        %d번 습관 이름 (공백 없이):        ← h+1 을 찍는다
 *    를 찍고 이름을 habitName[h] 에 담는다. 서식은 %31s 를 쓴다.
 * 3) 같은 반복 안에서 doneMask[h] 를 0u 로 초기화한다.
 * 함정: 문자열을 읽은 직후에는 flushLine() 을 한 번 부르세요.
 *       %31s 의 31 은 MAX_NAME-1 입니다. 마지막 한 칸은 널 문자 자리입니다.
 *
 * ⚠️ 반복문 안의 scanf·flushLine 두 줄은 이름을 전부 habitName[0] 에 덮어쓰는 임시 코드입니다.
 *    (없으면 뒤 입력이 어긋나 프로그램이 멈춥니다) — 위 2)·3) 대로 고치세요. */
void readHabits(void)
{
    printf("습관을 몇 개 기록하시겠어요? (1~%d): ", MAX_HABITS);
    habitCount = readIntInRange(1, MAX_HABITS);
    for (int h = 0; h < habitCount; h++) {
        printf("%d번 습관 이름 (공백 없이): ", h + 1);
        scanf("%31s", habitName[h]);
        flushLine();
        doneMask[h] = 0u;  
    }
}

/* ── [TODO 요구사항 2-b] 비트 하나를 켜고 끈다 ────────────────────
 * h 번 습관의 d 일 비트가
 *   · 켜져 있으면  → 끄고   "[%s] %d일 체크를 해제했습니다.\n"
 *   · 꺼져 있으면  → 켜고   "[%s] %d일을 체크했습니다.\n"
 * 9주차에서 배운 플래그 관리 그대로입니다 — 켜기·끄기·확인 셋.
 * 함정: 끄기는 「그 자리만 0 으로」입니다. 나머지 30칸은 그대로 있어야 합니다. */
void toggleDay(int h, int d)
{
    if (IS_DONE(doneMask[h], d)) {
        doneMask[h] &= ~DAY_BIT(d);
        printf("[%s] %d일 체크를 해제했습니다.\n", habitName[h], d);
    } else {
        doneMask[h] |= DAY_BIT(d);
        printf("[%s] %d일을 체크했습니다.\n", habitName[h], d);
    }
}

/* ── [TODO 요구사항 3] 달력 + 체크 표시 + 달성 요약 ───────────────
 * ① 머리 두 줄은 printCalendarHeader(h); 한 줄로 끝냅니다. [수정 금지] 함수입니다.
 * ② 격자 — 실습#2 에서 그린 것과 같은 격자입니다. 두 겹의 반복문으로 그립니다.
 *    · 한 줄은 7칸. 한 줄을 다 그리면 줄을 바꿉니다.
 *    · 날짜가 놓이는 칸:  printf("%3d%c", d, 체크됐으면 '*' 아니면 ' ');   ← 4칸
 *    · 날짜가 없는 칸:    printf("    ");   (공백 4개)
 * ③ 맨 아래 요약 — 달성일수와 함께 달성률을 냅니다.
 *      printf("\n[%s] %d일 / %d일 달성 (%.1f%%)\n", 이름, 달성일수, curDays, 달성률);
 *    달성률은 달성일수를 그 달의 일수로 나눈 백분율이고, 소수점 아래 한 자리입니다.
 * 함정: 달성률은 실수입니다. 정수끼리 나누면 소수점 아래가 통째로 버려져
 *       무엇을 넣든 0.0%% 가 나옵니다. 2주차의 자료형을 떠올리세요. */
void printCalendar(int h)
{
    printCalendarHeader(h);

    int count = 0;                                       
    int numWeeks = (curFirstWeekday + curDays + 6) / 7;
    int d = 1;                                 

    for (int w = 0; w < numWeeks; w++) {
        for (int col = 0; col < 7; col++) {
            int slot = w * 7 + col;
            if (slot < curFirstWeekday || d > curDays) {
                printf("    ");
            } else {
                int done = IS_DONE(doneMask[h], d);
                if (done) count++;
                printf("%3d%c", d, done ? '*' : ' ');
                d++;
            }
        }
        printf("\n");
    }
    double rate = (double)count / curDays * 100.0;
    printf("\n[%s] %d일 / %d일 달성 (%.1f%%)\n", habitName[h], count, curDays, rate);
}

/* ── [TODO 요구사항 4] 습관 × 주차 집계표 ─────────────────────────
 * 「습관 × 주차」 크기의 2차원 배열을 지역 변수로 두고 0 으로 채운 뒤 집계합니다.
 *   · d 일이 몇 주차인지는 요구사항 3 에서 격자의 몇 번째 줄에 놓았는지와 같습니다.
 *   · 이 달이 몇 주인지도 그 격자의 줄 수와 같습니다.
 * 출력 서식:
 *     \n=== 주간 집계 (%d년 %d월) ===\n
 *     %-10s  ← "습관"        그다음 주차마다 %4d주        그리고 개행
 *     %-10s  ← 습관 이름      그다음 주차마다 %5d          그리고 개행
 * 힌트: 집계할 때는 바깥이 습관, 안쪽이 날짜입니다. */
void printWeeklyTable(void)
{
    int numWeeks = (curFirstWeekday + curDays + 6) / 7;
    int weekly[MAX_HABITS][MAX_WEEKS] = {0};

    for (int h = 0; h < habitCount; h++) {
        for (int d = 1; d <= curDays; d++) {
            if (IS_DONE(doneMask[h], d)) {
                int w = (curFirstWeekday + d - 1) / 7;
                weekly[h][w]++;
            }
        }
    }

    printf("\n=== 주간 집계 (%d년 %d월) ===\n", curYear, curMonth);

    printf("%-10s", "습관");
    for (int w = 0; w < numWeeks; w++) {
        printf("%4d주", w + 1);
    }
    printf("\n");

    for (int h = 0; h < habitCount; h++) {
        printf("%-10s", habitName[h]);
        for (int w = 0; w < numWeeks; w++) {
            printf("%5d", weekly[h][w]);
        }
        printf("\n");
    }
}

/* ── [수정 금지] ─────────────────────────────────────────── */
int main(void)
{
    int choice = 0, h = 0, d = 0;

    printf("========== 습관 트래커 ==========\n");
    showNameLengthDemo();
    printf("연도를 입력하세요 (2000~2100): ");
    curYear = readIntInRange(2000, 2100);
    printf("월을 입력하세요 (1~12): ");
    curMonth = readIntInRange(1, 12);
    curDays = daysInMonth(curYear, curMonth);
    curFirstWeekday = firstWeekdayOf(curYear, curMonth);

    readHabits();

    do {
        printf("\n%d년 %d월 | [1]체크 [2]달력 [3]집계 [0]종료 > ", curYear, curMonth);
        choice = readIntInRange(0, 3);

        if (choice == 1) {
            printf("습관 번호 (1~%d): ", habitCount);
            h = readIntInRange(1, habitCount);
            printf("체크할 날짜 (1~%d, 0=취소): ", curDays);
            d = readIntInRange(0, curDays);
            if (d == 0) printf("취소했습니다.\n");
            else         toggleDay(h - 1, d);
        } else if (choice == 2) {
            printf("습관 번호 (1~%d): ", habitCount);
            h = readIntInRange(1, habitCount);
            printCalendar(h - 1);
        } else if (choice == 3) {
            printWeeklyTable();
        } else {
            printf("종료합니다.\n");
        }
    } while (choice != 0);

    return 0;
}

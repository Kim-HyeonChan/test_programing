/*
 * DASF004 (2026-2) · 실습#5(프로젝트) — 습관 트래커 ⑤: 저장하고, 필요한 만큼만
 * ─────────────────────────────────────────────────────────
 * 학번을 붙여 WA5_학번.c 로 저장하고, 제출할 때 확장자를 .txt 로 바꾸세요.
 * 빌드: gcc -Wall -Wextra -std=c11 WA5_학번.c -o wa5
 *
 * [TODO 요구사항 1] ~ [TODO 요구사항 3] 을 채우면 완성입니다.
 *   · 실습#1~#4 에서 만든 함수는 [수정 금지] 로 완성된 채 제공합니다.
 *   · 앞 실습을 못 하셨어도 이번 프로젝트는 처음부터 하실 수 있습니다.
 *
 * 담는 강의: 포인터와 배열 · 파일 입출력 · 동적 메모리
 * 만점 20점 — 프로젝트.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HABITS 5
#define MAX_NAME   32
#define MAX_WEEKS  6
#define MAX_MEMO   200
#define SAVE_PATH  "habits.txt"
#define DAY_BIT(d)      (1u << ((d) - 1))
#define IS_DONE(m, d)   (((m) & DAY_BIT(d)) != 0u)

/* ── [수정 금지] 타입 · 전역 선언 ───────────────────────── */
typedef enum { CAT_HEALTH = 1, CAT_STUDY = 2, CAT_LIFE = 3 } Category;

typedef struct {
    char         name[MAX_NAME];
    unsigned int doneMask;
    Category     cat;
} Habit;

Habit habits[MAX_HABITS];
char *memo[MAX_HABITS];          /* 각 습관의 메모 — 필요한 만큼만 확보한다 */
                                 /* ↑ 동적 메모리 문법입니다. [수정 금지]이고
                                  *   동적 메모리를 배우기 전에는 읽지 않아도 요구사항 1·2 를 끝낼 수 있습니다. */
int   habitCount = 0;
int   curYear = 2026, curMonth = 1, curDays = 0, curFirstWeekday = 0;

int  readIntInRange(int lo, int hi);
void flushLine(void);
int  isLeapYear(int year);
int  daysInMonth(int year, int month);
int  firstWeekdayOf(int year, int month);
const char *categoryName(Category c);
void readHabits(void);
void toggleDay(int h, int d);
void printCalendar(int h);
void printWeeklyTable(void);
void renameHabit(Habit *h);
void habitStats(const Habit *h, int *pDone, int *pBest);
void printSummary(const Habit *arr, int n);
void saveToFile(void);
int  loadFromFile(void);
void printByCategory(void);
void setMemo(int h);
void freeAll(void);

/* ── [수정 금지] 실습#2·#3 에서 만든 함수 ──────────────────── */
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
int isLeapYear(int year) { return ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0); }
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
/* ── [수정 금지] 실습#4 요구사항 4 ─────────────────────────────── */
const char *categoryName(Category c)
{
    switch (c) {
    case CAT_HEALTH: return "건강";
    case CAT_STUDY:  return "학습";
    case CAT_LIFE:   return "생활";
    }
    return "생활";
}

void printByCategory(void)
{
    printf("--- 분류별 집계 ---\n");
    for (int c = CAT_HEALTH; c <= CAT_LIFE; c++) {
        int count = 0, total = 0;
        for (int h = 0; h < habitCount; h++) {
            int done = 0, best = 0;
            if (habits[h].cat != (Category)c) continue;
            habitStats(&habits[h], &done, &best);
            count++;
            total += done;
        }
        printf("  %s  %d개  달성 %d일\n", categoryName((Category)c), count, total);
    }
    printf("-------------------\n");
}
/* ── [수정 금지] 실습#2·#3 에서 만든 함수 — toggleDay · printCalendar · printWeeklyTable ── */
void toggleDay(int h, int d)
{
    if (IS_DONE(habits[h].doneMask, d)) {
        habits[h].doneMask &= ~DAY_BIT(d);
        printf("[%s] %d일 체크를 해제했습니다.\n", habits[h].name, d);
    } else {
        habits[h].doneMask |= DAY_BIT(d);
        printf("[%s] %d일을 체크했습니다.\n", habits[h].name, d);
    }
}
void printCalendar(int h)
{
    int achieved = 0;
    printf("\n      %d년 %d월  [%s] (%s)\n", curYear, curMonth,
           habits[h].name, categoryName(habits[h].cat));
    printf("  일  월  화  수  목  금  토\n");
    int weeks = (curFirstWeekday + curDays + 6) / 7;
    for (int row = 0; row < weeks; row++) {
        for (int col = 0; col < 7; col++) {
            int d = row * 7 + col - curFirstWeekday + 1;
            if (d < 1 || d > curDays) { printf("    "); continue; }
            printf("%3d%c", d, IS_DONE(habits[h].doneMask, d) ? '*' : ' ');
        }
        printf("\n");
    }
    for (int d = 1; d <= curDays; d++) if (IS_DONE(habits[h].doneMask, d)) achieved++;
    printf("\n[%s] %d일 / %d일 달성 (%.1f%%)\n", habits[h].name, achieved, curDays,
           100.0 * achieved / curDays);
}
void printWeeklyTable(void)
{
    int weekCount[MAX_HABITS][MAX_WEEKS] = {{0}};
    int weeks = ((curFirstWeekday + curDays - 1) / 7) + 1;
    for (int h = 0; h < habitCount; h++)
        for (int d = 1; d <= curDays; d++)
            if (IS_DONE(habits[h].doneMask, d))
                weekCount[h][(curFirstWeekday + d - 1) / 7]++;
    printf("\n=== 주간 집계 (%d년 %d월) ===\n", curYear, curMonth);
    printf("%-10s", "습관");
    for (int w = 0; w < weeks; w++) printf("%4d주", w + 1);
    printf("\n");
    for (int h = 0; h < habitCount; h++) {
        printf("%-10s", habits[h].name);
        for (int w = 0; w < weeks; w++) printf("%5d", weekCount[h][w]);
        printf("\n");
    }
}

/* ── [수정 금지] 실습#4 요구사항 1 ─────────────────────────────── */
void readHabits(void)
{
    printf("습관을 몇 개 기록하시겠어요? (1~%d): ", MAX_HABITS);
    habitCount = readIntInRange(1, MAX_HABITS);
    for (int h = 0; h < habitCount; h++) {
        printf("%d번 습관 이름 (공백 없이): ", h + 1);
        scanf("%31s", habits[h].name);
        flushLine();
        printf("   분류 (1=건강 2=학습 3=생활): ");
        habits[h].cat = (Category)readIntInRange(1, 3);
        habits[h].doneMask = 0u;
    }
}

/* ── [수정 금지] 실습#4 요구사항 2·3 ──────────────────────────── */
void renameHabit(Habit *h)
{
    char newName[MAX_NAME];
    printf("새 이름 (공백 없이): ");
    scanf("%31s", newName);
    flushLine();
    printf("[%s] → [%s] 로 바꿨습니다.\n", h->name, newName);
    for (int i = 0; i < MAX_NAME; i++) {
        h->name[i] = newName[i];
        if (newName[i] == '\0') break;
    }
}


void habitStats(const Habit *h, int *pDone, int *pBest)
{
    int done = 0, run = 0, best = 0;
    for (int d = 1; d <= curDays; d++) {
        if (IS_DONE(h->doneMask, d)) {
            done++;
            run++;
            if (run > best) best = run;
        } else {
            run = 0;
        }
    }
    *pDone = done;
    *pBest = best;
}


/* ── [TODO 요구사항 1] 배열을 포인터로 받아 훑는다 (포인터와 배열) ──
 * arr 는 습관 배열의 첫 원소를 가리키고, n 은 개수입니다.
 * 출력 서식:
 *     \n=== 전체 요약 (%d년 %d월) ===\n
 *     %d. %-10s (%s) %3d일\n        ← 번호(1부터) · 이름 · 분류 이름 · 달성일수
 *     합계: %d일\n
 * 🔴 포인터를 한 칸씩 앞으로 옮기며 훑으세요. 첨자로만 쓰면 화면은 같아도 감점입니다.
 *    멤버에는 -> 로 닿습니다.
 * 🔴 함정: 배열을 함수에 넘기면 「개수」가 따라가지 않습니다. 그래서 n 을 같이 받습니다.
 *          sizeof(arr) 로는 원소 수를 알 수 없습니다 — 포인터의 크기가 나옵니다. */
void printSummary(const Habit *arr, int n)
{
    int totalDone = 0;
    const Habit *p = arr; // 첫 번째 습관을 가리키는 포인터

    printf("\n=== 전체 요약 (%d년 %d월) ===\n", curYear, curMonth);

    for (int i = 0; i < n; i++, p++) {
        int done = 0, best = 0;
        habitStats(p, &done, &best); // 포인터 p가 가리키는 습관의 달성 일수 계산
        totalDone += done;

        printf("%d. %-10s (%s) %3d일\n",
               i + 1, p->name, categoryName(p->cat), done);
    }
    printf("합계: %d일\n", totalDone);
}
/* ── [TODO 요구사항 2] 파일로 저장하고 불러온다 (파일 입출력) ──────
 * 저장 (SAVE_PATH 에 쓰기 모드로):
 *     첫 줄        "%d %d %d\n"      ← curYear, curMonth, habitCount
 *     습관마다 한 줄 "%s %d %u\n"     ← name, (int)cat, doneMask
 *     끝나면       "%d개 습관을 저장했습니다.\n"
 *     열기 실패 시  "[오류] 저장에 실패했습니다.\n"
 * 불러오기 (읽기 모드):
 *     첫 줄을 읽어 curYear·curMonth·habitCount 를 채우고
 *     curDays·curFirstWeekday 를 다시 계산한 뒤(daysInMonth·firstWeekdayOf)
 *     습관을 순서대로 읽는다.  끝나면 "%d개 습관을 불러왔습니다.\n" 후 1 을 반환.
 *     파일이 없거나 첫 줄을 못 읽으면 "[오류] 저장된 파일이 없습니다.\n" 후 0 반환.
 * 🔴 메모(요구사항 3)는 파일에 저장하지 않습니다. 불러오기는 연·월·습관 목록만 덮어쓰고
 *    memo[] 는 건드리지 않습니다 — 불러온 뒤에도 메모는 그대로 남습니다.
 * 힌트: fopen / fprintf / fscanf / fclose. 문자열은 "%31s" 로 받습니다.
 * 🔴 함정: fopen 이 NULL 을 돌려줄 수 있습니다. 확인 없이 쓰면 프로그램이 죽습니다.
 *          fclose 를 빠뜨리면 내용이 파일에 안 남을 수 있습니다. */
void saveToFile(void)
{
    FILE *fp = fopen(SAVE_PATH, "w");
    if (fp == NULL) {
        printf("[오류] 저장에 실패했습니다.\n");
        return;
    }

    // 첫 줄: 연도, 월, 습관 개수
    fprintf(fp, "%d %d %d\n", curYear, curMonth, habitCount);

    // 둘째 줄부터: 습관마다 이름, 분류(정수), 달성 비트마스크
    for (int i = 0; i < habitCount; i++) {
        fprintf(fp, "%s %d %u\n",
                habits[i].name, (int)habits[i].cat, habits[i].doneMask);
    }

    fclose(fp);
    printf("%d개 습관을 저장했습니다.\n", habitCount);
}

int loadFromFile(void)
{
    FILE *fp = fopen(SAVE_PATH, "r");
    if (fp == NULL) {
        printf("[오류] 저장된 파일이 없습니다.\n");
        return 0;
    }

    // 첫 줄 읽기: 3개 값을 모두 못 읽으면 실패 처리
    if (fscanf(fp, "%d %d %d", &curYear, &curMonth, &habitCount) != 3) {
        fclose(fp);
        printf("[오류] 저장된 파일이 없습니다.\n");
        return 0;
    }

    // 불러온 연·월에 맞춰 일수와 시작 요일 재계산
    curDays = daysInMonth(curYear, curMonth);
    curFirstWeekday = firstWeekdayOf(curYear, curMonth);

    // 습관 목록 순서대로 읽기
    for (int i = 0; i < habitCount; i++) {
        int catVal = 0;
        if (fscanf(fp, "%31s %d %u",
                   habits[i].name, &catVal, &habits[i].doneMask) != 3) {
            break;
        }
        habits[i].cat = (Category)catVal;
    }

    fclose(fp);
    printf("%d개 습관을 불러왔습니다.\n", habitCount);
    return 1;
}
/* ── [TODO 요구사항 3] 메모 — 필요한 만큼만 확보하고 반납한다 (동적 메모리) ─
 * 1) "메모 (공백 없이): " 를 찍고 지역 버퍼(char buf[MAX_MEMO])로 받는다. flushLine() 도 부른다.
 * 2) 글자 수를 잰다(strlen).
 * 3) 🔴 먼저 free(memo[h]) — 이전 메모가 있으면 반납한다.
 * 4) 그다음 memo[h] 에 그 길이 + 1 바이트를 malloc 한다. 확보 실패면
 *        "[오류] 메모리를 확보하지 못했습니다.\n" 을 내고 돌아간다.
 * 5) 내용을 복사하고(strcpy) 아래를 출력한다.
 *        [%s] 메모를 저장했습니다. (%d바이트)\n     ← 이름, 확보한 바이트 수
 * 힌트: free(NULL) 은 아무 일도 하지 않습니다 — 처음이라도 그냥 부르면 됩니다.
 * 🔴 함정 ①: +1 을 빼먹으면 널 문자가 들어갈 자리가 없습니다.
 * 🔴 함정 ②: 새로 malloc 하기 전에 free 를 안 하면 그만큼이 샙니다.
 * 🔴 순서: free 가 malloc 보다 뒤에 오면 옛 주소를 잃은 뒤에 놓아 주게 되어
 *          이중 해제·use-after-free 가 됩니다. 당장은 돌아도 나중에(종료할 때 등)
 *          죽거나 값이 깨질 수 있습니다. 순서를 지키세요.
 * 💡 한글은 한 글자가 3바이트입니다 — 「아침에만」 4글자면 4 × 3 + 1 = 13바이트.
 *
 * ⚠️ setMemo 안의 임시 코드는 둘뿐입니다 — (void)h; 와 「(아직 미구현 …)」 출력 줄.
 *    구현하면서 이 둘만 지우세요. 버퍼 · scanf · flushLine 은 1) 단계 그대로 씁니다. */
void setMemo(int h)
{
    char buf[MAX_MEMO];
    printf("메모 (공백 없이): ");
    scanf("%199s", buf);
    flushLine();

    int bytes = (int)strlen(buf) + 1; // 문자열 길이 + 널 문자('\0') 1바이트

    free(memo[h]);                    // 1) 기존 메모가 있으면 먼저 반납
    memo[h] = malloc((size_t)bytes);  // 2) 필요한 만큼만 새로 할당
    if (memo[h] == NULL) {            // 3) 할당 실패 확인
        printf("[오류] 메모리를 확보하지 못했습니다.\n");
        return;
    }

    strcpy(memo[h], buf);             // 4) 입력받은 메모 복사
    printf("[%s] 메모를 저장했습니다. (%d바이트)\n", habits[h].name, bytes);
}

/* ── [수정 금지] 종료 시 전량 반납 (동적 메모리 — 손대지 않습니다) ── */
void freeAll(void)
{
    for (int h = 0; h < MAX_HABITS; h++) {
        free(memo[h]);
        memo[h] = NULL;
    }
}

/* ── [수정 금지] ─────────────────────────────────────────── */
int main(void)
{
    int choice = 0, h = 0, d = 0, done = 0, best = 0;

    printf("========== 습관 트래커 ==========\n");
    printf("연도를 입력하세요 (2000~2100): ");
    curYear = readIntInRange(2000, 2100);
    printf("월을 입력하세요 (1~12): ");
    curMonth = readIntInRange(1, 12);
    curDays = daysInMonth(curYear, curMonth);
    curFirstWeekday = firstWeekdayOf(curYear, curMonth);

    readHabits();

    do {
        printf("\n%d년 %d월 | [1]체크 [2]달력 [3]집계 [4]이름 [5]통계\n", curYear, curMonth);
        printf("           | [6]요약 [7]저장 [8]불러오기 [9]메모 [0]종료 > ");
        choice = readIntInRange(0, 9);

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
        } else if (choice == 4) {
            printf("이름을 바꿀 습관 번호 (1~%d): ", habitCount);
            h = readIntInRange(1, habitCount);
            renameHabit(&habits[h - 1]);
        } else if (choice == 5) {
            printf("통계를 볼 습관 번호 (1~%d): ", habitCount);
            h = readIntInRange(1, habitCount);
            habitStats(&habits[h - 1], &done, &best);
            printf("[%s] (%s)\n", habits[h - 1].name, categoryName(habits[h - 1].cat));
            printf("  달성: %d일 / %d일\n", done, curDays);
            printf("  최장 연속: %d일\n", best);
            printByCategory();
            if (memo[h - 1] != NULL) printf("  메모: %s\n", memo[h - 1]);
        } else if (choice == 6) {
            printSummary(habits, habitCount);
        } else if (choice == 7) {
            saveToFile();
        } else if (choice == 8) {
            loadFromFile();
        } else if (choice == 9) {
            printf("메모를 쓸 습관 번호 (1~%d): ", habitCount);
            h = readIntInRange(1, habitCount);
            setMemo(h - 1);
        } else {
            printf("종료합니다.\n");
        }
    } while (choice != 0);

    freeAll();
    return 0;
}

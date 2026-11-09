/*
 * DASF004 (2026-2) · 실습#4 — 습관 트래커 ④: 묶고 넘기기
 * ─────────────────────────────────────────────────────────
 * 학번을 붙여 WA4_학번.c 로 저장하고, 제출할 때 확장자를 .txt 로 바꾸세요.
 * 빌드: gcc -Wall -Wextra -std=c11 WA4_학번.c -o wa4
 *
 * [TODO 요구사항 1] ~ [TODO 요구사항 4] — 빈칸 다섯 곳을 채우면 완성입니다.
 *   · 요구사항 4 는 빈칸이 둘입니다 (4-a categoryName · 4-b printByCategory).
 *   · 구조체·열거형 정의, 승계 함수, main 은 [수정 금지] 입니다.
 *   · 포인터 산술·동적 메모리·파일 입출력은 쓰지 않습니다. (이번 과제의 범위 밖입니다)
 */
#include <stdio.h>

#define MAX_HABITS 5
#define MAX_NAME   32
#define MAX_WEEKS  6
#define DAY_BIT(d)      (1u << ((d) - 1))
#define IS_DONE(m, d)   (((m) & DAY_BIT(d)) != 0u)

/* ── [수정 금지] 열거형과 구조체 ─────────────────────────
 * 실습#3 까지는 상태가 배열 두 개(habitName[h] · doneMask[h])로 흩어져 있었고
 * 분류는 담을 자리조차 없었습니다. 둘을 같은 인덱스로 맞춰 줘야 했죠.
 * 구조체는 그 둘을 하나로 묶고 분류가 들어갈 칸까지 만들어 줍니다. */
typedef enum { CAT_HEALTH = 1, CAT_STUDY = 2, CAT_LIFE = 3 } Category;

typedef struct {
    char         name[MAX_NAME];
    unsigned int doneMask;
    Category     cat;
} Habit;

/* ── [수정 금지] 파일 스코프 상태 ─────────────────────────── */
Habit habits[MAX_HABITS];
int   habitCount = 0;
int   curYear = 2026, curMonth = 1, curDays = 0, curFirstWeekday = 0;

/* ── [수정 금지] 함수 선언 ───────────────────────────────── */
int  readIntInRange(int lo, int hi);
void flushLine(void);
int  isLeapYear(int year);
int  daysInMonth(int year, int month);
int  firstWeekdayOf(int year, int month);
const char *categoryName(Category c);
void showPassingDemo(void);
void readHabits(void);
void toggleDay(int h, int d);
void printCalendar(int h);
void printWeeklyTable(void);
void renameHabit(Habit *h);
void habitStats(const Habit *h, int *pDone, int *pBest);
void printByCategory(void);

/* ── [수정 금지] 이미 완성되어 제공되는 함수들 ───────────── */
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

/* ── [수정 금지] 값 전달 vs 포인터 전달 — 시작할 때 한 번 보여 줍니다 ──
 * 이번 과제의 요점입니다. **같은 일을 하는 두 함수**를 나란히 부릅니다.
 *   · byValue   — 구조체를 통째로 복사해서 받는다  → 원본은 안 바뀐다
 *   · byPointer — 원본의 주소를 받는다             → 원본이 바뀐다
 * 4-a 를 채운 뒤 아래 두 줄의 출력이 다르다는 것이 요구사항 2 를 포인터로 받아야 하는 이유입니다.
 * (직접 실험해 보고 싶으면 이 두 함수를 복사해서 여러분 코드에 써 보세요.
 *  main 과 renameHabit 의 선언은 고치지 않습니다.) */
void demoSetByValue(Habit copy)
{
    copy.cat = CAT_LIFE;
    printf("  값 전달   — 함수 안 : %s\n", categoryName(copy.cat));
}

void demoSetByPointer(Habit *ptr)
{
    ptr->cat = CAT_LIFE;
    printf("  포인터    — 함수 안 : %s\n", categoryName(ptr->cat));
}

void showPassingDemo(void)
{
    Habit sample;
    sample.doneMask = 0u;
    sample.cat = CAT_HEALTH;
    sample.name[0] = '\0';

    printf("--- [참고] 값 전달 vs 포인터 전달 ---\n");
    demoSetByValue(sample);
    printf("  값 전달   — main 은 : %s   <- 안 바뀐다\n", categoryName(sample.cat));
    demoSetByPointer(&sample);
    printf("  포인터    — main 은 : %s   <- 바뀐다\n", categoryName(sample.cat));
    printf("------------------------------------\n");
}
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

/* ── [TODO 요구사항 1] 구조체 배열을 채운다 ──────────────────────
 * 1) 프롬프트를 찍고 개수를 1~MAX_HABITS 로 받아 habitCount 에 넣는다.
 *        습관을 몇 개 기록하시겠어요? (1~5):
 * 2) h = 0 .. habitCount-1 반복하며
 *        %d번 습관 이름 (공백 없이):            ← h+1
 *    을 찍고 이름을 %31s 서식으로 그 습관의 name 에 받는다. 직후에 flushLine().
 *        "   분류 (1=건강 2=학습 3=생활): "     ← 앞에 공백 3칸
 *    을 찍고 1~3 을 받아 그 습관의 cat 에 넣는다.
 *    같은 반복 안에서 그 습관의 doneMask 를 0 으로 초기화한다.
 * 힌트: 배열 원소가 구조체이므로 . 으로 멤버에 닿습니다.
 * 함정: cat 은 Category 형입니다. 받은 정수는 (Category) 로 형을 밝혀서 넣으세요.
 *       형을 안 밝혀도 컴파일은 되지만, 밝혀 두면 어떤 형의 값인지가 코드에 남습니다.
 * 💡 doneMask 는 파일 스코프 배열 안에 있어 처음엔 자동으로 0 입니다.
 *    그래도 명시적으로 0 을 넣어 두세요 — 「어디서 0 이 되는지」가 코드에
 *    보여야 하고, 같은 코드를 함수 안 지역 변수로 옮기면 그때는 쓰레기값입니다.
 *
 * ⚠️ 반복문 안의 scanf·flushLine 두 줄은 이름을 전부 habits[0] 에 덮어쓰고, dummy 는 분류를 버리는 임시 코드입니다.
 *    (없으면 뒤 입력이 어긋나 프로그램이 멈춥니다) — 위 2) 대로 고치세요. */
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

/* ── [TODO 요구사항 2] 포인터로 원본을 고친다 ────────────────────
 * 1) "새 이름 (공백 없이): " 를 찍고 새 이름을 지역 배열에 받는다.
 * 2) 바꾸기 전에 아래를 출력한다.
 *        [%s] → [%s] 로 바꿨습니다.\n      ← 옛 이름, 새 이름 순
 * 3) h 가 가리키는 구조체의 name 을 새 이름으로 덮는다.
 * 힌트: h 는 포인터입니다. 멤버에 닿을 때는 h->name 을 씁니다.
 *       문자열 복사는 반복문으로 한 글자씩 옮기고 \0 을 만나면 멈춥니다.
 *       strcpy 를 쓰려면 맨 위 #include <stdio.h> 아래에 #include <string.h> 를
 *       직접 더하세요 — #include 줄은 [수정 금지]가 아닙니다.
 * 🔴 함정: 매개변수를 Habit h (값)로 받으면 함수 안에서만 바뀌고
 *          main 의 배열은 그대로입니다. 그게 이 요구사항의 요점입니다. */
void renameHabit(Habit *h)
{
    char newName[MAX_NAME];
    printf("새 이름 (공백 없이): ");
    scanf("%31s", newName);
    flushLine();
    printf("[%s] → [%s] 로 바꿨습니다.\n", h->name, newName);
    int i = 0;
    while (newName[i] != '\0') {
        h->name[i] = newName[i];
        i++;
    }
    h->name[i] = '\0';
}

/* ── [TODO 요구사항 3] out-parameter 로 두 값을 돌려준다 ─────────
 * 1일부터 curDays 까지 훑으면서 두 가지를 구한다.
 *   · 달성한 날의 수        → *pDone 에 넣는다
 *   · 가장 긴 연속 달성 일수 → *pBest 에 넣는다
 * 힌트: 연속 길이를 세는 변수를 하나 두고, 체크된 날이면 +1,
 *       아니면 0 으로 되돌립니다. 그 값의 최댓값이 최장 연속입니다.
 * 함정: return 은 값을 하나만 돌려줍니다. 두 개를 돌려주려고 포인터 매개변수를
 *       받는 것이고, 넘어온 포인터가 「가리키는 자리」에 값을 씁니다.
 * 함정: h 는 const Habit * 입니다 — 읽기만 하고 고치지 마세요.
 * 🔴 이 함수는 아무것도 출력하지 않습니다. 화면의 세 줄([이름] (분류) ·
 *    달성 · 최장 연속)은 main 이 [수정 금지] 로 찍습니다. 함수 안에서
 *    또 찍으면 같은 줄이 두 번 나옵니다. */
void habitStats(const Habit *h, int *pDone, int *pBest)
{
    int doneCount = 0;
    int currentStreak = 0;
    int maxStreak = 0;

    for (int d = 1; d <= curDays; d++) {
        if (IS_DONE(h->doneMask, d)) {
            doneCount++;
            currentStreak++;
            if (currentStreak > maxStreak) {
                maxStreak = currentStreak;
            }
        } else {
            currentStreak = 0;
        }
    }

    *pDone = doneCount;
    *pBest = maxStreak;
}

/* ── [TODO 요구사항 4-a] 분류 이름을 돌려준다 ────────────────────
 * Category 값 하나를 받아 "건강" · "학습" · "생활" 중 하나를 돌려줍니다.
 * 세 값이 서로 배타적이고 개수가 정해져 있으니, 값 하나로 갈래를 고르는
 * 문법이 여기 맞습니다.
 * 함정: 숫자 1·2·3 을 그대로 쓰지 말고 열거형이 만들어 둔 이름을 쓰세요.
 *       이름을 쓰면 나중에 분류가 늘어도 고칠 곳이 한 군데입니다. */
const char *categoryName(Category c)
{
    switch (c) {
        case CAT_HEALTH:
            return "건강";
        case CAT_STUDY:
            return "학습";
        case CAT_LIFE:
            return "생활";
        default:
            return "?";
    }
}
/* ── [TODO 요구사항 4-b] 분류별 집계 ─────────────────────────────
 * 세 분류를 차례로 돌면서, 그 분류에 속한 습관이 몇 개이고
 * 달성일수의 합이 얼마인지를 출력합니다. 습관이 없는 분류도 0 으로 찍습니다.
 * 출력 서식:
 *     --- 분류별 집계 ---
 *       %s  %d개  달성 %d일        ← 분류 이름 · 개수 · 달성일수 합
 *     -------------------
 * 🔴 달성일수는 직접 세지 말고 요구사항 3 의 habitStats 를 부르세요.
 *    그 함수는 결과를 반환하지 않고 「받아 둔 자리에 써 주는」 방식입니다 —
 *    어디에 쓸지를 넘겨야 합니다.
 * 함정: 분류 값을 1·2·3 으로 세지 말고 열거형 이름으로 처음과 끝을 잡으세요. */
void printByCategory(void)
{
    printf("--- 분류별 집계 ---\n");
    for (Category cat = CAT_HEALTH; cat <= CAT_LIFE; cat++) {
        int count = 0;     
        int totalDone = 0; 

        for (int h = 0; h < habitCount; h++) {
            if (habits[h].cat == cat) {
                count++;
                
                int done = 0, best = 0;
                habitStats(&habits[h], &done, &best);
                totalDone += done;
            }
        }
        printf("  %s  %d개  달성 %d일\n", categoryName(cat), count, totalDone);
    }

    printf("-------------------\n");
}
/* ── [수정 금지] ─────────────────────────────────────────── */
int main(void)
{
    int choice = 0, h = 0, d = 0, done = 0, best = 0;

    showPassingDemo();
    printf("========== 습관 트래커 ==========\n");
    printf("연도를 입력하세요 (2000~2100): ");
    curYear = readIntInRange(2000, 2100);
    printf("월을 입력하세요 (1~12): ");
    curMonth = readIntInRange(1, 12);
    curDays = daysInMonth(curYear, curMonth);
    curFirstWeekday = firstWeekdayOf(curYear, curMonth);

    readHabits();

    do {
        printf("\n%d년 %d월 | [1]체크 [2]달력 [3]집계 [4]이름 [5]통계 [0]종료 > ", curYear, curMonth);
        choice = readIntInRange(0, 5);

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
        } else {
            printf("종료합니다.\n");
        }
    } while (choice != 0);

    return 0;
}

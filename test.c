/*
 * DASF004 (2026-2) · 실습#2 — 습관 트래커 ②: 빈 달력을 그리고, 함수로 쪼갠다
 * ─────────────────────────────────────────────────────────
 * 학번을 붙여 WA2_학번.c 로 저장하고, 제출할 때 확장자를 .txt 로 바꾸세요.
 * 빌드: gcc -Wall -Wextra -std=c11 WA2_학번.c -o wa2
 *
 * [TODO 요구사항 1] ~ [TODO 요구사항 4] — 함수 다섯 개의 본문을 채우면 완성입니다(요구사항 2 가 두 개).
 *   · main 과 함수 선언부는 [수정 금지] 입니다.
 *   · 배열·문자열·구조체·포인터는 쓰지 않습니다. (아직 안 배웠습니다)
 *   · (void)lo; 같은 줄은 "아직 안 쓰는 값" 표시입니다 — 구현을 시작하면 지우세요.
 */
#include <stdio.h>

/* ── [수정 금지] 함수 선언 ─────────────────────────────── */
int  readIntInRange(int lo, int hi);
int  isLeapYear(int year);
int  daysInMonth(int year, int month);
int  firstWeekdayOf(int year, int month);
void printCalendar(int firstWeekday, int days);

/* ── [TODO 요구사항 1] 범위 안의 값이 들어올 때까지 다시 묻는다 ────
 * 1) 값을 하나 읽는다.
 * 2) lo~hi 밖이면 아래 두 줄을 내고 다시 읽는다. 범위에 들어올 때까지 반복.
 *        [오류] %d~%d 사이의 값을 입력하세요.
 *        >
 *    (둘째 줄은 "> " 로 끝나고 줄바꿈이 없습니다 — 첫 줄 끝에 \n, 그다음 "> ")
 * 3) 범위 안의 값을 반환한다.
 * 함정: 첫 질문은 부르는 쪽(main)이 이미 찍었습니다. 여기서 또 찍지 마세요. */
int readIntInRange(int lo, int hi)
{
    int val = 0;
    scanf("%d", &val);
    while (val<lo || val>hi) {
        printf("[오류] %d~%d 사이의 값을 입력하세요.\n", lo, hi);
        printf("> ");
        scanf("%d", &val);
    }

    return val;
}

/* ── [TODO 요구사항 2-a] 윤년이면 1, 아니면 0 을 반환 ─────────────
 * 실습#1 에서 main 안에 썼던 식을 그대로 옮겨 오면 됩니다. */
int isLeapYear(int year)
{
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)){
        return 1;
    } else {
        return 0;
    }
}

/* ── [TODO 요구사항 2-b] 그 달의 일수를 반환 ──────────────────────
 * 4·6·9·11월 → 30   2월 → 28(윤년이면 29)   나머지 → 31
 * 함정: 2월을 판정할 때 isLeapYear 를 「불러서」 쓰세요.
 *       같은 조건식을 두 번 쓰면 함수로 나눈 의미가 없습니다. */
int daysInMonth(int year, int month)
{
    if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    } else if (month == 2) {
        if (isLeapYear(year) == 1) {
            return 29;
        } else {
            return 28;
        }
    } else {
        return 31;
    }
}

/* ── [TODO 요구사항 3] 그 달 1일의 요일 (0=일 … 6=토) ─────────────
 * 기준일: 2000년 1월 1일은 토요일이고, 요일 코드는 6 입니다.
 * 기준일부터 그 달 1일까지 며칠이 지났는지를 세어 요일을 옮깁니다.
 *   ① 2000년 ~ (year-1)년   해마다 365일, 윤년이면 366일
 *   ② 그 해 1월 ~ (month-1)월  달마다 daysInMonth
 * 함정: 두 단계 모두 「자기 자신」은 포함하지 않습니다.
 *       2000년 1월을 넣으면 경과일이 0 이고 요일이 그대로 토요일이어야 합니다. */
int firstWeekdayOf(int year, int month)
{
    int totalDays = 0;
    int y = 0;
    int m = 0;

    for (y = 2000; y < year; y++) {
        if (isLeapYear(y) == 1) {
            totalDays += 366;
        } else {
            totalDays += 365;
        }
    }
    for (m = 1; m < month; m++) {
        totalDays += daysInMonth(year, m);
    }
    return (6 + totalDays) % 7;
}

/* ── [TODO 요구사항 4] 달력 격자 출력 ─────────────────────────────
 * 달력은 「주」가 세로로 쌓이고 「요일」이 가로로 늘어선 격자입니다.
 * 그 격자를 그대로 두 겹의 반복문으로 그리세요 — 바깥이 주, 안쪽이 요일.
 *   · 한 줄은 7칸이고, 한 줄을 다 그리면 줄을 바꿉니다.
 *   · 날짜가 놓이는 칸은 printf("%3d ", d)
 *   · 날짜가 없는 칸은 공백 4개("    ")   ← 1일 앞과 말일 뒤
 * 함정: 안쪽 반복은 7칸을 끝까지 돕니다. 날짜가 없다고 건너뛰면 칸이 밀립니다.
 *       ─ 빈 칸을 찍고 「그 칸은 여기까지」로 넘어가는 방법이 있습니다. */
void printCalendar(int firstWeekday, int days)
{
    int totalWeeks = (firstWeekday + days + 6) / 7;
    int week = 0;
    int dow = 0;

    for (week = 0; week < totalWeeks; week++) {
        for (dow = 0; dow < 7; dow++) {
            int dayNum = week * 7 + dow - firstWeekday + 1;

            if (dayNum >= 1 && dayNum <= days) {
                printf("%3d ", dayNum);
            } else {
                printf("    ");
            }
        }
        printf("\n");
    }
}

/* ── [수정 금지] ─────────────────────────────────────────── */
int main(void)
{
    int year = 0, month = 0, days = 0, firstWeekday = 0;

    printf("=== 습관 트래커 ===\n");
    printf("연도를 입력하세요 (2000~2100): ");
    year = readIntInRange(2000, 2100);
    printf("월을 입력하세요 (1~12): ");
    month = readIntInRange(1, 12);

    days = daysInMonth(year, month);
    firstWeekday = firstWeekdayOf(year, month);

    printf("\n         %d년 %d월\n", year, month);
    printf("  일  월  화  수  목  금  토\n");
    printCalendar(firstWeekday, days);

    return 0;
}

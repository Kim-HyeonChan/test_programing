#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define TOTAL_ROUNDS 5   // 총 진행할 게임 판수
#define DICE_COUNT 5     // 사용하는 주사위 개수

// 각 판의 기록을 저장하기 위한 구조체
typedef struct {
    int round;              // 판 번호
    int dice[DICE_COUNT];   // 굴려 나온 주사위 5개의 눈
    char category[30];      // 판정된 족보 이름
    int score;              // 해당 판에서 얻은 점수
} RoundRecord;

// 주사위 5개를 무작위(1~6)로 굴리는 함수
void roll_dice(int dice[]) {
    for (int i = 0; i < DICE_COUNT; i++) {
        dice[i] = (rand() % 6) + 1;
    }
}

// 주사위 조합을 분석하여 가장 높은 점수의 족보를 판정하는 함수
void evaluate_hand(const int dice[], char *out_category, int *out_score) {
    int counts[7] = {0}; // 1~6 번 눈이 각각 몇 개 나왔는지 카운트하는 배열

    for (int i = 0; i < DICE_COUNT; i++) {
        counts[dice[i]]++;
    }

    int max_score = 0;
    char best_category[30] = "족보 없음";

    // 1. 요트 (Yacht): 5개 모두 동일한 눈 -> 50점
    for (int i = 1; i <= 6; i++) {
        if (counts[i] == 5) {
            if (50 > max_score) {
                max_score = 50;
                strcpy(best_category, "요트");
            }
        }
    }

    // 2. 풀하우스 (Full House): 같은 눈 3개 + 같은 눈 2개 -> 25점
    int has_3 = 0, has_2 = 0;
    for (int i = 1; i <= 6; i++) {
        if (counts[i] == 3) has_3 = 1;
        if (counts[i] == 2) has_2 = 1;
    }
    if (has_3 && has_2) {
        if (25 > max_score) {
            max_score = 25;
            strcpy(best_category, "풀하우스");
        }
    }

    // 3. 스트레이트 (Straight): 연속된 눈 4개 이상 -> 30점
    if ((counts[1] >= 1 && counts[2] >= 1 && counts[3] >= 1 && counts[4] >= 1) ||
        (counts[2] >= 1 && counts[3] >= 1 && counts[4] >= 1 && counts[5] >= 1) ||
        (counts[3] >= 1 && counts[4] >= 1 && counts[5] >= 1 && counts[6] >= 1)) {
        if (30 > max_score) {
            max_score = 30;
            strcpy(best_category, "스트레이트");
        }
    }

    // 4. 포카드 (Four of a Kind): 같은 눈 4개 이상 -> 해당 눈 * 4
    for (int i = 1; i <= 6; i++) {
        if (counts[i] >= 4) {
            int score = i * 4;
            if (score > max_score) {
                max_score = score;
                strcpy(best_category, "포카드");
            }
        }
    }

    // 5. 트리플 (Three of a Kind): 같은 눈 3개 이상 -> 해당 눈 * 3
    for (int i = 1; i <= 6; i++) {
        if (counts[i] >= 3) {
            int score = i * 3;
            if (score > max_score) {
                max_score = score;
                strcpy(best_category, "트리플");
            }
        }
    }

    // 6. 원페어 (One Pair): 같은 눈 2개 이상 -> 해당 눈 * 2
    for (int i = 1; i <= 6; i++) {
        if (counts[i] >= 2) {
            int score = i * 2;
            if (score > max_score) {
                max_score = score;
                strcpy(best_category, "원페어");
            }
        }
    }

    *out_score = max_score;
    strcpy(out_category, best_category);
}

int main() {
    srand((unsigned int)time(NULL));

    RoundRecord records[TOTAL_ROUNDS];
    int total_score = 0;

    printf("=========================================\n");
    printf("           주사위 요트 게임 시작          \n");
    printf("=========================================\n\n");

    for (int r = 0; r < TOTAL_ROUNDS; r++) {
        records[r].round = r + 1;

        roll_dice(records[r].dice);
        evaluate_hand(records[r].dice, records[r].category, &records[r].score);
        total_score += records[r].score;

        printf("[%d판] 주사위: [ %d %d %d %d %d ] -> 판정: %s (%d점)\n",
               records[r].round,
               records[r].dice[0], records[r].dice[1], records[r].dice[2],
               records[r].dice[3], records[r].dice[4],
               records[r].category, records[r].score);
    }

    printf("\n=========================================\n");
    printf("               최종 판별 기록             \n");
    printf("=========================================\n");
    printf(" 판수 |   주사위 결과   |  판정 족보  | 점수 \n");
    printf("------+-----------------+-------------+------\n");

    for (int r = 0; r < TOTAL_ROUNDS; r++) {
        printf("  %d   |  %d  %d  %d  %d  %d  | %-11s | %2d점\n",
               records[r].round,
               records[r].dice[0], records[r].dice[1], records[r].dice[2],
               records[r].dice[3], records[r].dice[4],
               records[r].category,
               records[r].score);
    }

    printf("-----------------------------------------\n");
    printf(" 총점: %d점\n", total_score);
    printf("=========================================\n");

    return 0;
}
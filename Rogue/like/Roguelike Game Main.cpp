#pragma once
#include "DxLib.h"
#include <cmath>
#include <vector>

// �\���̒�`
struct Enemy {
    float x, y;
    int hp;
    bool isAlive;

    float knockbackX = 0.0f;
    float knockbackY = 0.0f;
    int knockbackTimer = 0;
};

struct Boss {
    float x, y;
    int hp;
    int MaxHP;
    bool isAlive;
    int phase;
};

struct HealItem {
    float x, y;
    bool isActive;
};

int main()
{
    // �E�B���h�E������
    ChangeWindowMode(TRUE);
    SetGraphMode(800, 640, 32);
    if (DxLib_Init() == -1) return -1;
    SetDrawScreen(DX_SCREEN_BACK);

    // �v���C���[�ݒ�
    float playerX = 400, playerY = 300;
    const float playerSpeed = 2.0f;
    int playerHP = 100;
    const int maxHP = 100;

    // ���G����
    int invincibleTimer = 0;
    const int invincibleDuration = 60;

    // �G�̏�����
    std::vector<Enemy> enemies = {
        {100, 100, 100, true},
        {700, 150, 100, true},
        {300, 500, 100, true},
    };
    const int damageFromEnemy = 10;

    // �񕜃A�C�e��
    std::vector<HealItem> heals = {
        {200, 200, true},
        {600, 400, true},
    };
    const int healAmount = 20;

    // �{�X������
    Boss boss = { 400, 100, 300, 300, true, 1 };

    // �U�����
    bool isAttacking = false;
    int attackTimer = 0;
    const int attackDuration = 10;
    const float attackRange = 40.0f;
    const int attackDamage = 50;

    bool isGameOver = false;

    // ���C�����[�v
    while (ProcessMessage() == 0) {
        // ���͏���
        if (!isGameOver) {
            if (CheckHitKey(KEY_INPUT_UP))    playerY -= playerSpeed;
            if (CheckHitKey(KEY_INPUT_DOWN))  playerY += playerSpeed;
            if (CheckHitKey(KEY_INPUT_LEFT))  playerX -= playerSpeed;
            if (CheckHitKey(KEY_INPUT_RIGHT)) playerX += playerSpeed;
        }

        if (invincibleTimer > 0) invincibleTimer--;

        // �U������
        if (!isGameOver && CheckHitKey(KEY_INPUT_SPACE) && !isAttacking) {
            isAttacking = true;
            attackTimer = attackDuration;
        }
        if (isAttacking) {
            attackTimer--;
            if (attackTimer <= 0) isAttacking = false;
        }

        // �G�̏���
        for (auto& enemy : enemies) {
            if (!enemy.isAlive) continue;

            float hpRate = (float)enemy.hp / 100;
            float speed = (hpRate > 0.5f) ? 1.0f : (hpRate > 0.25f) ? 2.0f : 2.5f;

            // �m�b�N�o�b�N����
            if (enemy.knockbackTimer > 0) {
                enemy.x += enemy.knockbackX;
                enemy.y += enemy.knockbackY;
                enemy.knockbackTimer--;
            }
            else {
                float dx = playerX - enemy.x;
                float dy = playerY - enemy.y;
                float dist = sqrtf(dx * dx + dy * dy);
                if (dist != 0.0f) {
                    enemy.x += (dx / dist) * speed;
                    enemy.y += (dy / dist) * speed;
                }
            }

            // �U������
            if (isAttacking) {
                float edx = enemy.x - playerX;
                float edy = enemy.y - playerY;
                float edist = sqrtf(edx * edx + edy * edy);
                if (edist < attackRange) {
                    enemy.hp -= attackDamage;
                    if (enemy.hp <= 0) {
                        enemy.isAlive = false;
                    }

                    // �m�b�N�o�b�N�ݒ�
                    float dx = enemy.x - playerX;
                    float dy = enemy.y - playerY;
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist != 0.0f) {
                        enemy.knockbackX = (dx / dist) * 5.0f;
                        enemy.knockbackY = (dy / dist) * 5.0f;
                        enemy.knockbackTimer = 10;
                    }
                }
            }

            // �ڐG�ɂ��_���[�W�i���G���Ԓ��͐H���Ȃ��j
            float dx = playerX - enemy.x;
            float dy = playerY - enemy.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (!isGameOver && dist < 32.0f && invincibleTimer <= 0) {
                playerHP -= damageFromEnemy;
                invincibleTimer = invincibleDuration;
                if (playerHP <= 0) {
                    playerHP = 0;
                    isGameOver = true;
                }
            }
        }

        // �{�X�̏���
        if (boss.isAlive) {
            float bossHpRate = (float)boss.hp / boss.MaxHP;
            boss.phase = (bossHpRate > 0.66f) ? 1 : (bossHpRate > 0.33f) ? 2 : 3;
            float bossSpeed = (boss.phase == 1) ? 1.2f : (boss.phase == 2) ? 1.8f : 2.5f;

            float dx = playerX - boss.x;
            float dy = playerY - boss.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist > 0) {
                boss.x += (dx / dist) * bossSpeed;
                boss.y += (dy / dist) * bossSpeed;
            }

            // �U������i�m�b�N�o�b�N�t���j
            if (isAttacking && dist < attackRange + 10) {
                boss.hp -= attackDamage;

                // �m�b�N�o�b�N�i�����Z�߁j
                float knockX = (dx / dist) * 2.0f;
                float knockY = (dy / dist) * 2.0f;
                boss.x += knockX;
                boss.y += knockY;

                if (boss.hp <= 0) {
                    boss.hp = 0;
                    boss.isAlive = false;
                }
            }

            // �ڐG�Ń_���[�W�i���G���͐H���Ȃ��j
            if (!isGameOver && dist < 40.0f && invincibleTimer <= 0) {
                playerHP -= damageFromEnemy * 2;
                invincibleTimer = invincibleDuration;
                if (playerHP <= 0) {
                    playerHP = 0;
                    isGameOver = true;
                }
            }
        }

        // �񕜃A�C�e���̏���
        for (auto& heal : heals) {
            if (!heal.isActive) continue;
            float dx = playerX - heal.x;
            float dy = playerY - heal.y;
            float dist = sqrtf(dx * dx + dy * dy);
            if (dist < 32.0f) {
                playerHP += healAmount;
                if (playerHP > maxHP) playerHP = maxHP;
                heal.isActive = false;
            }
        }

        // �`�揈��
        ClearDrawScreen();

        // �v���C���[ ���G���͔���������
        int playerColor = (invincibleTimer > 0) ? GetColor(255, 128, 128) : GetColor(255, 0, 0);
        DrawBox((int)(playerX - 16), (int)(playerY - 16), (int)(playerX + 16), (int)(playerY + 16), playerColor, TRUE);

        if (isAttacking) {
            DrawCircle((int)playerX, (int)playerY, (int)attackRange, GetColor(255, 200, 200), FALSE);
        }

        // �G
        for (const auto& enemy : enemies) {
            if (!enemy.isAlive) continue;
            float hpRate = (float)enemy.hp / 100.0f;
            int color = (hpRate > 0.5f) ? GetColor(0, 0, 255) :
                (hpRate > 0.25f) ? GetColor(150, 0, 200) :
                GetColor(255, 0, 0);
            DrawBox((int)(enemy.x - 16), (int)(enemy.y - 16), (int)(enemy.x + 16), (int)(enemy.y + 16), color, TRUE);

            // HP�o�[
            int eBarW = 32, eBarH = 6;
            int eBarX = (int)enemy.x - eBarW / 2;
            int eBarY = (int)enemy.y - 24;
            int eFilled = (int)(eBarW * hpRate);

            DrawBox(eBarX, eBarY, eBarX + eBarW, eBarY + eBarH, GetColor(100, 100, 100), TRUE);
            DrawBox(eBarX, eBarY, eBarX + eFilled, eBarY + eBarH, GetColor(255, 80, 80), TRUE);
            DrawBox(eBarX, eBarY, eBarX + eBarW, eBarY + eBarH, GetColor(255, 255, 255), FALSE);
        }

        // �{�X
        if (boss.isAlive) {
            int bossColor = (boss.phase == 1) ? GetColor(0, 100, 255) :
                (boss.phase == 2) ? GetColor(200, 50, 200) :
                GetColor(255, 50, 50);
            DrawBox((int)(boss.x - 32), (int)(boss.y - 32), (int)(boss.x + 32), (int)(boss.y + 32), bossColor, TRUE);
        }

        // �{�XHP�o�[
        if (boss.isAlive) {
            int screenW = 800;
            int barWidth = 400;
            int barHeight = 20;

            int barX = (screenW - barWidth) / 2;
            int barY = 560;

            float hpRate = (float)boss.hp / boss.MaxHP;
            int filled = (int)(barWidth * hpRate);

            DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(100, 100, 100), TRUE);
            DrawBox(barX, barY, barX + filled, barY + barHeight, GetColor(255, 0, 0), TRUE);
            DrawBox(barX, barY, barX + barWidth, barY + barHeight, GetColor(255, 255, 255), FALSE);
            DrawFormatString(barX + 5, barY - 18, GetColor(255, 255, 255), "BOSS HP");
        }

        // �񕜃A�C�e��
        for (const auto& heal : heals) {
            if (heal.isActive) {
                DrawCircle((int)heal.x, (int)heal.y, 12, GetColor(0, 255, 0), TRUE);
            }
        }

        // �v���C���[HP�o�[
        int uiX = 10, uiY = 10, uiW = 200, uiH = 20;
        int filled = (int)((float)playerHP / maxHP * uiW);
        DrawBox(uiX, uiY, uiX + uiW, uiY + uiH, GetColor(100, 100, 100), TRUE);
        DrawBox(uiX, uiY, uiX + filled, uiY + uiH, GetColor(255, 50, 50), TRUE);
        DrawBox(uiX, uiY, uiX + uiW, uiY + uiH, GetColor(255, 255, 255), FALSE);

        if (isGameOver) {
            DrawFormatString(300, 280, GetColor(255, 0, 0), "GAME OVER");
            DrawFormatString(260, 320, GetColor(255, 255, 255), "Press SPACE to Retry");
        }

        // ���g���C����
        if (isGameOver && CheckHitKey(KEY_INPUT_SPACE)) {
            playerX = 400; playerY = 300;
            playerHP = maxHP;
            for (auto& e : enemies) {
                e.x = rand() % 700 + 50;
                e.y = rand() % 500 + 50;
                e.hp = 100;
                e.isAlive = true;
                e.knockbackX = 0;
                e.knockbackY = 0;
                e.knockbackTimer = 0;
            }
            boss = { 400, 100, 300, 300, true, 1 };
            for (auto& h : heals) h.isActive = true;
            isGameOver = false;
            isAttacking = false;
            invincibleTimer = 0;
        }

        ScreenFlip();
    }

    DxLib_End();
    return 0;
}
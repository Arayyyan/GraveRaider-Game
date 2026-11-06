
// -----------------------------------------------------------

#include "GamesEngineeringBase.h"

// -----------------------------------------------------------

#include <iostream>

#include <string>
#include <stdexcept>
#include <random>
#include <memory>

#include "World.h"

#include "Entity.hpp"
#include "GameEntity.hpp"
#include "GraveRaider.hpp"
#include "Camera.hpp"
#include "TitleScreen.hpp"


// -----------------------------------------------------------

int main()
{
    std::string example = "All systems nominal";
    std::cout << example << std::endl;

    GamesEngineeringBase::Timer timer;

    GamesEngineeringBase::Window window;
    window.create(1024, 800, "GraveRaider", false);

    GameState current_state = GameState::TitleScreen;
    TitleScreen titleScreen;

    // BackgroundEntity background({0, 0});
    //SpriteEntity sprite({ 100, 100 }, "./Resources/player_front.png");
    HeroEntity hero(Vec2f(100.0f, 100.0f));
    WorldEntity world({ 64,64 }, window.getHeight());

    if (!world.load("./Resources/level.txt")) {
        std::cout << "Failed to load level file" << std::endl;
    }

    static constexpr int kMaxEnemies = 4;
    EnemyEntity enemies[kMaxEnemies];
    
    float enemy_spawn_timer = 3.0f;


    //projectile container
    static constexpr int kMaxProjectiles = 10;
    ProjectileEntity projectiles[kMaxProjectiles];
    float attack_cooldown = 0.0f;

    //damage cooldown tracker
    float damage_cooldown = 0.0f;



    Camera camera(static_cast<float>(window.getWidth()), static_cast<float>(window.getHeight()));
    //world.load("./Resources/level.txt");

    float delta_time = timer.dt();
    bool running = true;
    while (running) {

        delta_time = timer.dt();

        window.clear();

        if (window.keyPressed(VK_ESCAPE)) break;

        if (current_state == GameState::TitleScreen) {
            titleScreen.Draw(window);

            // Check for button click or Enter key
            if (window.keyPressed(VK_RETURN) || titleScreen.isPlayButtonClicked(window)) {
                std::cout << "SUCCESS: Switching to Playing state!" << std::endl;
                current_state = GameState::Playing;
            }

            window.present();
            running = !window.keyPressed(VK_ESCAPE);

            continue;
        }

        Vec2f input{ 0, 0 };
        if (window.keyPressed('W')) input.y_ = -1.0f;
        if (window.keyPressed('S')) input.y_ = 1.0f;
        if (window.keyPressed('A')) input.x_ = -1.0f;
        if (window.keyPressed('D')) input.x_ = 1.0f;

        if (window.keyPressed(VK_LSHIFT))
        {
            world.SetScrollSpeed(5.0f);
        }
        else
        {
            world.SetScrollSpeed(1.0f);
        }

        if (window.keyPressed(VK_SPACE)) {
            world.SetShowALphas(true);
        }
        else
        {
            world.SetShowALphas(false);
        }

        // background.Update(delta_time);
        //sprite.Update(delta_time);
        hero.SetInputAxis(input);

        Vec2f current_pos = hero.GetPosition();
        Vec2f velocity = hero.GetInputAxis() * 250.0f * delta_time;


        //x movement test
        Vec2f next_x_pos = current_pos + Vec2f{ velocity.x_, 0.0f };

        float cx = next_x_pos.x_ + hero.GetImageWidth() / 2.0f;
        float cy = current_pos.y_ + hero.GetImageHeight() / 2.0f;

        if (!world.IsSolid(cx, cy)) {
            current_pos.x_ = next_x_pos.x_;
        }
        //y movement test
        Vec2f next_y_pos = current_pos + Vec2f{ 0.0f, velocity.y_ };

        cx = current_pos.x_ + hero.GetImageWidth() / 2.0f;
        cy = next_y_pos.y_ + hero.GetImageHeight() / 2.0f;

        if (!world.IsSolid(cx, cy)) {
            current_pos.y_ = next_y_pos.y_;
        }
        hero.SetPosition(current_pos);


        //first projectile on 'E' key
        if (attack_cooldown > 0.0f) attack_cooldown -= delta_time;

        if (window.keyPressed('E') && attack_cooldown <= 0.0f) {
            Vec2f shoot_dir = hero.GetFacingDirection();
            Vec2f spawn_pos = hero.GetCollisionPoint();

            for (int i = 0; i < kMaxProjectiles; ++i) {
                if (!projectiles[i].isActive()) {
                    projectiles[i].Activate(spawn_pos, shoot_dir * 500.0f);
                    attack_cooldown = 0.3f;
                    break;
                }
            }

        }

        for (int i = 0; i < kMaxProjectiles; ++i) {
            if (!projectiles[i].isActive()) continue;

            projectiles[i].Update(delta_time);

            Vec2f p_pos = projectiles[i].GetPosition();
            Vec2f h_pos = hero.GetPosition();
            float dx_hero = p_pos.x_ - h_pos.x_;
            float dy_hero = p_pos.y_ - h_pos.y_;
            float dist_from_hero = sqrtf((dx_hero * dx_hero) + (dy_hero * dy_hero));

            if (dist_from_hero > 860.0f) {
                projectiles[i].Deactivate();
            }

            for (int e = 0; e < kMaxEnemies; ++e) {
                if (enemies[e].isAlive()) {
                    Vec2f e_center = enemies[e].GetCollisionPoint();

                    float dx = p_pos.x_ - e_center.x_;
                    float dy = p_pos.y_ - e_center.y_;
                    float dist = sqrtf((dx * dx) + (dy * dy));

                    if (dist < 25.0f) {
                        enemies[e].takeDamage(25);
                        projectiles[i].Deactivate();
                        break;
                    }
                }
            }

        }

        bool any_enemy_alive = false;
        for (int e = 0; e < kMaxEnemies; ++e) {
            if (enemies[e].isAlive()) {
                any_enemy_alive = true;

                Vec2f enemy_pos = enemies[e].GetPosition();
                Vec2f player_pos = hero.GetPosition();
                Vec2f direction = player_pos - enemy_pos;

                for (int other = 0; other < kMaxEnemies; ++other) {
                    if (e != other && enemies[other].isAlive()) {
                        Vec2f other_pos = enemies[other].GetPosition();
                        Vec2f diff = enemy_pos - other_pos;
                        float dist = sqrtf((diff.x_ * diff.x_) + (diff.y_ * diff.y_));
                        if (dist > 0.0f && dist < 45.0f) {
                            diff.x_ /= dist;
                            diff.y_ /= dist;
                            enemy_pos = enemy_pos + (diff * 2.0f);
                        }
                    }
                }

                float length = sqrtf((direction.x_ * direction.x_) + (direction.y_ * direction.y_));
                if (length > 1.0f) {
                    direction.x_ /= length;
                    direction.y_ /= length;

                    Vec2f velocity = direction * enemies[e].GetSpeed() * delta_time;

                    Vec2f next_x_pos = enemy_pos + Vec2f{ velocity.x_, 0.0f };
                    float cx = next_x_pos.x_ + enemies[e].GetImageWidth() / 2.0f;
                    float cy = enemy_pos.y_ + enemies[e].GetImageHeight() / 2.0f;
                    if (!world.IsSolid(cx, cy)) {
                        enemy_pos.x_ = next_x_pos.x_;
                    }

                    Vec2f next_y_pos = enemy_pos + Vec2f{ 0.0f, velocity.y_ };
                    cx = enemy_pos.x_ + enemies[e].GetImageWidth() / 2.0f;
                    cy = next_y_pos.y_ + enemies[e].GetImageHeight() / 2.0f;
                    if (!world.IsSolid(cx, cy)) {
                        enemy_pos.y_ = next_y_pos.y_;
                    }

                    enemies[e].SetPosition(enemy_pos);
                }
                else {
                    enemies[e].SetPosition(enemy_pos);
                }

                Vec2f hero_center = hero.GetCollisionPoint();
                Vec2f enemy_center = enemies[e].GetCollisionPoint();

                float dx = hero_center.x_ - enemy_center.x_;
                float dy = hero_center.y_ - enemy_center.y_;
                float distance = sqrtf((dx * dx) + (dy * dy));

                if (distance < 35.0f && damage_cooldown <= 0.0f) {
                    hero.takeDamage(10);
                    damage_cooldown = 1.0f;
                }
            }
        }

        if (damage_cooldown > 0.0f) {
            damage_cooldown -= delta_time;
        }

        if (!any_enemy_alive) {
            enemy_spawn_timer -= delta_time;
            if (enemy_spawn_timer <= 0.0f) {
                for (int e = 0; e < kMaxEnemies; ++e) {
                    if (!enemies[e].isAlive()) {
                        Vec2f h_pos = hero.GetPosition();

                        Vec2f offsets[8] = {
                            Vec2f(-150.0f,    0.0f),
                            Vec2f(150.0f,    0.0f),
                            Vec2f(0.0f, -150.0f),
                            Vec2f(0.0f,  150.0f),
                            Vec2f(-120.0f, -120.0f),
                            Vec2f(120.0f, -120.0f),
                            Vec2f(-120.0f,  120.0f),
                            Vec2f(120.0f,  120.0f)
                        };

                        Vec2f spawn_pos = h_pos + Vec2f(0.0f, -100.0f);
                        for (int i = 0; i < 8; ++i) {
                            int idx = (e + i) % 8;
                            Vec2f test_pos = h_pos + offsets[idx];

                            float cx = test_pos.x_ + enemies[e].GetImageWidth() / 2.0f;
                            float cy = test_pos.y_ + enemies[e].GetImageHeight() / 2.0f;

                            if (!world.IsSolid(cx, cy)) {
                                spawn_pos = test_pos;
                                break;
                            }
                        }

                        enemies[e].Spawn(spawn_pos);
                        enemy_spawn_timer = 2.0f;
                        break;
                    }
                }
            }
        }

        camera.Follow(hero.GetPosition());
        world.Update(delta_time);

        world.Draw(window, camera.GetPosition());
        hero.Draw(window, camera.GetPosition());

        for (int e = 0; e < kMaxEnemies; ++e) {
            if (enemies[e].isAlive()) {
                enemies[e].Draw(window, camera.GetPosition());
            }
        }

        for (int i = 0; i < kMaxProjectiles; ++i) {
            projectiles[i].Draw(window, camera.GetPosition());
        }

        
        window.present();
        running = !window.keyPressed(VK_ESCAPE);
        std::cout << "FPS: " << 1.0f / delta_time << std::endl;
    }

    return 0;
}
// ----------------------------------------------------------------------

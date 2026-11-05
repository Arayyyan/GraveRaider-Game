
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


// -----------------------------------------------------------

int main()
{
    std::string example = "All systems nominal";
    std::cout << example << std::endl;

    GamesEngineeringBase::Timer timer;

    GamesEngineeringBase::Window window;
    window.create(1024, 800, "GraveRaider", false);
    
            
    // BackgroundEntity background({0, 0});
    //SpriteEntity sprite({ 100, 100 }, "./Resources/player_front.png");
    HeroEntity hero(Vec2f(100.0f, 100.0f));
    WorldEntity world({ 64,64 }, window.getHeight());

    if (!world.load("./Resources/level.txt")) {
        std::cout << "Failed to load level file" << std::endl;
    }

    static constexpr int kMaxEnemies = 4;
    EnemyEntity enemies[kMaxEnemies];
    float respawn_timer = 0.0f;
    bool waiting_for_respawn = false;

    enemies[0].Spawn(Vec2f(400.0f, 300.0f));
    enemies[1].Spawn(Vec2f(400.0f, 300.0f));
    enemies[2].Spawn(Vec2f(400.0f, 300.0f));
    enemies[3].Spawn(Vec2f(400.0f, 300.0f));

    
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
            Vec2f shoot_dir = { 0.0f, -1.0f };
            Vec2f input = hero.GetInputAxis();
            if (input.x_ != 0.0f || input.y_ != 0.0f) {
                shoot_dir = input;
            }

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

                enemies[e].UpdateTowardsPlayer(delta_time, hero.GetPosition());

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
            if (!waiting_for_respawn) {
                waiting_for_respawn = true;
                respawn_timer = 3.0f;
            }
        }
        
        if (waiting_for_respawn) {
            respawn_timer -= delta_time;
            if (respawn_timer <= 0.0f) {
                Vec2f h_pos = hero.GetPosition();

                enemies[0].Spawn(h_pos + Vec2f(-200.0f, -200.0f));
                enemies[1].Spawn(h_pos + Vec2f(-200.0f, -200.0f));
                enemies[2].Spawn(h_pos + Vec2f(-200.0f, -200.0f));
                enemies[3].Spawn(h_pos + Vec2f(-200.0f, -200.0f));

                waiting_for_respawn = false;
                
            }
        }

        
        
        camera.Follow(hero.GetPosition());
        world.Update(delta_time);

        // background.Draw(window);
        world.Draw(window, camera.GetPosition());

        //sprite.Draw(window);
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

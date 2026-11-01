
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
    HeroEntity hero(Vec2f(100, (window.getHeight() - 128) ) );
    WorldEntity world({ 64,64 }, window.getHeight());
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
        hero.Update(delta_time);
        camera.Follow(hero.GetPosition());
        world.Update(delta_time);

        // background.Draw(window);
        world.Draw(window, camera.GetPosition());

        //sprite.Draw(window);
        hero.Draw(window, camera.GetPosition());

        window.present();
        running = !window.keyPressed(VK_ESCAPE);

		std::cout << "FPS: " << 1.0f / delta_time << std::endl;

    }


    return 0;
}

// ----------------------------------------------------------------------

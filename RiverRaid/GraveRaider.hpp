    #pragma once
#include <random>
#include <iostream>
#include "GameEntity.hpp"
#include <string>
#include "Camera.hpp"


/*

WORLD ENTITY DESIGN
===================

- [x] Load the Tiles
- [x] Store the Tiles
- [x] Draw the Tiles
- [\] Choose Which Tiles to Draw
- [?] Create & Destroy the Tiles (Manage)
- [X] Scroll The Tiles

- [ ] Delta Time Scroll Speed

*/

class WorldEntity : public VisualEntity {

    public:
    static constexpr int kTileType = 6;
    static constexpr int kNumWalls = 4;
    static constexpr int kWorldHeight = 1000;

    WorldEntity(const Vec2f&pos, int view_height) 
    : VisualEntity(pos)
    , view_height_(view_height) 
    {
        std::cout << "Constructor logic for initializing the world entity" << std::endl;

        for (int i=0; i<kTileType; ++i) {
            std::string image_filename;
            std::string alpha_filename;

            image_filename = "./Resources/"+std::to_string(i)+".png";
            alpha_filename = "./Resources/alpha"+std::to_string(i)+".png";
            std::cout << "Loading image: " << image_filename << std::endl;
            std::cout << "Loading alpha image: " << alpha_filename << std::endl;

            tiles[i].load(image_filename);
            alphas[i].load(alpha_filename);
        }

        for (int i = 0; i < kNumWalls; ++i) {
            std::string wall_filename = "./Resources/Wall" + std::to_string(i + 1) + ".png";
            std::cout << "Loading wall image: " << wall_filename << std::endl;
            walls[i].load(wall_filename);
        }

        // all tiles share the same height, so take it from the first loaded image
        tile_height_ = static_cast<int>(tiles[0].height);

        std::mt19937 random{std::random_device{}()};
        std::uniform_int_distribution<int> pick(0, kTileType - 1);

        for (int n=0; n<kWorldHeight; ++n) {
             //world[n] = 0; // Initialize each world tile to 0
            world[n]=pick(random);
        }

        // Use this to see what tiles are showing up
        std::cout << "First 15 world tile IDs: ";
        for (int i = 0; i < 15; ++i) {
            std::cout << world[i] << " ";
        }
        std::cout << std::endl;
     
    }

    bool IsSolid(float world_x, float world_y) const {
        const int tile_size = 32;
        int tile_x = static_cast<int>(floorf(world_x / tile_size));
        int tile_y = static_cast<int>(floorf(world_y / tile_size));

        int map_index = (tile_x + tile_y * 73);
        map_index = ((map_index % kWorldHeight) + kWorldHeight) % kWorldHeight;

        int t = world[map_index];

        return (t >= 10);
    }

    virtual bool load(std::string filename) {

        std::ifstream infile(filename);

        if (!infile.is_open()) {
            std::cout << "Failed to open file: " << filename << std::endl;
            return false;
        }

        std::string line;

        unsigned int n = 0;

        while (std::getline(infile, line)) {

            int tile_id;
            std::istringstream iss(line);
            if (!(iss >> tile_id))
            {
                break;
            }
            world[n++]=tile_id;

            std::cout << "Got tile_id: " << n << " "<< tile_id << std::endl;
        }
        return true;
    }

    void SetShowALphas(bool show) {
        show_alphas_ = show;
    }


    virtual void Update(float delta_time) override
    {

    }

    inline int clamp_value(int val, int low, int high) 
    {
        if (val < low) return low;
        if (val > high) return high;
        return val;
    }

    virtual void Draw(GamesEngineeringBase::Window& window) override {
        Draw(window, { 0.0f, 0.0f });
    }
    virtual void Draw(GamesEngineeringBase::Window& window, const Vec2f& camera_pos){        

        const int tile_size = 32;

        int cols = (window.getWidth() / tile_size) + 4;
        int rows = (window.getHeight() / tile_size) + 4;

        int start_tile_x = static_cast<int>(floorf(camera_pos.x_ / tile_size));
        int start_tile_y = static_cast<int>(floorf(camera_pos.y_ / tile_size));

        for (int y = 0; y < rows; ++y) 
        {
            for (int x = 0; x < cols; ++x) 
            {
                int tile_x = start_tile_x + x;
                int tile_y = start_tile_y + y;

                int map_index = (tile_x + tile_y * 73);
                map_index = ((map_index % kWorldHeight) + kWorldHeight) % kWorldHeight;

                int t = world[map_index];
                //t = clamp_value(t, 0, kTileType - 1);

                float screen_x = (tile_x * tile_size) - camera_pos.x_;
                float screen_y = (tile_y * tile_size) - camera_pos.y_;

                int floor_t = clamp_value(t >= 10 ? 0 : t, 0, kTileType - 1);
                Renderer_draw_image(window, { screen_x, screen_y }, tiles[t]);

                if (t >= 10) {
                    int wall_index = (t - 10) % kNumWalls;

                    float wall_screen_y = screen_y - (walls[wall_index].height - tile_size);
                    Renderer_draw_image(window, { screen_x, screen_y }, walls[wall_index]);
                }
                
            }
        }
    }

    void SetTile(int tile_x, int tile_y, int tile_id) {
        int map_index = (tile_x + tile_y * 73);
        map_index = ((map_index % kWorldHeight) + kWorldHeight) % kWorldHeight;
        world[map_index] = tile_id;
    }

    bool IsLand(const Vec2f& screen_point) {

        const int world_y = static_cast<int>(scroll_offset) + view_height_ - 1 - screen_point.y_;

        if ( world_y < 0 )  { return false; }

        const int tile = world_y / tile_height_;
        const int row = tile_height_ - 1 - ( world_y % tile_height_);
        
        return alphas[TileType(tile)].at(static_cast<int>(screen_point.x_), row, 0) < 128;
    }

    int TileType(int tile_index) const {
        return world[((tile_index % kWorldHeight) + kWorldHeight) % kWorldHeight];
    }


    void SetScrollSpeed(float speed) {
        scroll_speed = speed;
    }


    private:
    // Private member variables for storing the tiles
    GamesEngineeringBase::Image tiles[kTileType];
    GamesEngineeringBase::Image walls[kNumWalls];
    GamesEngineeringBase::Image alphas[kTileType];

    int world[kWorldHeight];

    float scroll_speed = 0.0f;
    int scroll_offset = 0;
    bool show_alphas_ = false;
    int view_height_;
    int tile_height_ = 0;
};


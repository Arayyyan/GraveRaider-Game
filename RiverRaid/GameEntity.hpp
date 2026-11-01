#pragma once

#include "vec2f.hpp"
#include "renderer.hpp"
#include "shapes.hpp"

class SpriteEntity : public VisualEntity {
   
    public:

    SpriteEntity(const Vec2f& position, const std::string & image_filename)
        : VisualEntity(position)
        , image_()
    {
        image_.load(image_filename);
    }

    virtual void Update(float delta_time) override {
        // ImageEntity might not have any update logic for now
    }

    virtual void Draw(GamesEngineeringBase::Window& window) override {
        Renderer_draw_image(window, GetPosition(), image_);
    }

    virtual const GamesEngineeringBase::Image& GetImage() const { return image_; }
    virtual int GetImageWidth() const { return GetImage().width; }
    virtual int GetImageHeight() const { return GetImage().height; }

protected:
    GamesEngineeringBase::Image image_;

};

class HeroEntity : public SpriteEntity
{
public:
    explicit HeroEntity(const Vec2f& position)
        : SpriteEntity(position, "./Resources/player_front.png")
    {
        frontImage.load("./Resources/player_front.png");
        backImage.load("./Resources/player_back.png");
        sideImage.load("./Resources/player_side.png");
    }

    void SetInputAxis(const Vec2f& input) 
    { 
        inputAxis_ = input; 

        if (input.y_ < 0.0f) {
            currentDirection = 1;
        }
        else if (input.y_ > 0.0f) {
            currentDirection = 0;
        }
        else if (input.x_ != 0.0f) {
            currentDirection = 2;
        }
    }
    const Vec2f & GetInputAxis() const { return inputAxis_; }

    virtual void Update(float delta_time) {
        
        Move(inputAxis_ * speed_ * delta_time);

    }

    virtual void Draw(GamesEngineeringBase::Window& window) override {
        Draw(window, { 0.0f, 0.0f });
    }

    void Draw(GamesEngineeringBase::Window& window, const Vec2f& camera_pos) {
        GamesEngineeringBase::Image* activeImage = &frontImage;
        if (currentDirection == 1) {
            activeImage = &backImage;
        }
        else if (currentDirection == 2) {
            activeImage = &sideImage;
        }

        Vec2f screen_pos = { GetPosition().x_ - camera_pos.x_, GetPosition().y_ - camera_pos.y_ };

        Renderer_draw_image(window, screen_pos, *activeImage);

        Renderer_drawCircle(window,
            static_cast<int>(screen_pos.x_ + GetImageWidth() / 2),
            static_cast<int>(screen_pos.y_ + GetImageWidth() / 2),
            static_cast<int>(collisionCircle_.getRadius()),
            255, 0, 255, 
            1 );
    }


    Vec2f GetCollisionPoint() const {
        return { GetPosition().x_ + GetImageWidth() / 2, 
                 GetPosition().y_ + GetImageHeight() / 2 };
    }

    

protected:
    float speed_ = 250.0f; // Example speed value
    Vec2f inputAxis_{0.0f, 0.0f};
    Circle collisionCircle_{16.0f};

    GamesEngineeringBase::Image frontImage;
    GamesEngineeringBase::Image backImage;
    GamesEngineeringBase::Image sideImage;

    int currentDirection = 0;

};


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

    
    virtual void Update(float delta_time) override{
        
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

        DrawHealthBar(window);
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

    //Player Health
    int health_ = 100;
    int max_health_ = 100;

    void takeDamage(int amount) {
        health_ -= amount;
        if (health_ < 0) {
            health_ = 0;
        }
    }

    bool isAlive() const {
        return health_ > 0;
    }

    void DrawHealthBar(GamesEngineeringBase::Window& window) 
    {
        int winWidth = static_cast<int>(window.getWidth());
        int winHeight = static_cast<int>(window.getHeight());
        if (winWidth <= 0) return;

        int barWidth = 250;
        int barHeight = 10;

        if (barWidth > winWidth - 40) {
            barWidth = winWidth - 40;
        }

        //used a fixed margin from the right edge of the window
       
        int xPos = winWidth - barWidth - 20;
        int yPos = 20;

        //ensures health doesn't go out of bounds and wrap around
        float healthPercent = static_cast<float>(health_) / max_health_;
        if (healthPercent < 0.0f) healthPercent = 0.0f;
        if (healthPercent > 1.0f) healthPercent = 1.0f;

        int healthWidth = static_cast<int>(healthPercent * barWidth);


        //draws health bar pixel by pixel
        for (int y = 0; y < barHeight; ++y) {
            for (int x = 0; x < barWidth; ++x) {
                int drawX = xPos + x;
                int drawY = yPos + y;

                if (x < healthWidth) {
                    //red for health
                    window.draw(xPos + x, yPos + y, 255, 0, 0);
                }
                else {
                    //dark grey for health lost
                    window.draw(xPos + x, yPos + y, 50, 50, 50);
                }
            }
        }
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

class EnemyEntity : SpriteEntity {
public:
    EnemyEntity()
        : SpriteEntity({0.0f, 0.0f}, "./Resources/enemy_front.png"), active_(false)
    {
        health_ = 50;
        speed_ = 120.0f;
    }
    explicit EnemyEntity(const Vec2f& position)
        : SpriteEntity(position, "./Resources/enemy_front.png"), active_(true)
    {
        health_ = 50;
        speed_ = 120.0f;
    }
    
    virtual void Update(float delta_time) override {
        //default update if needed
    }
    void UpdateTowardsPlayer(float delta_time, const Vec2f& player_pos) {
        Vec2f current_pos = GetPosition();

        Vec2f direction = player_pos - current_pos;

        float length = sqrtf((direction.x_ * direction.x_) + (direction.y_ * direction.y_));
        if (length > 1.0f) {
            direction.x_ /= length;
            direction.y_ /= length;

            Move(direction * speed_ * delta_time);
        }
    }

    Vec2f GetCollisionPoint() const{
        return{ GetPosition().x_ + GetImageWidth() / 2, GetPosition().y_ + GetImageWidth() / 2 };
    }
    void Draw(GamesEngineeringBase::Window& window, const Vec2f camera_pos) {
        Vec2f screen_pos = { GetPosition().x_ - camera_pos.x_, GetPosition().y_ - camera_pos.y_ };
        Renderer_draw_image(window, screen_pos, image_);
    }


    int health_ = 50;

    void takeDamage(int amount) {
        health_ -= amount;
        if (health_ < 0) {
            health_ = 0;
            active_ = false;
        }
    }

    bool isAlive() const {
        return health_ > 0;
    }

    bool isActive() const { return active_; }

    void Spawn(const Vec2f& position) {
        SetPosition(position);
        health_ = 50;
        active_ = true;
    }

protected:
    float speed_ = 120.0f;
    bool active_ = false;
};

class ProjectileEntity : public PositionEntity {
public:
    ProjectileEntity() : PositionEntity({0.0f, 0.0f}), velocity_{0.0f, 0.0f}, active_(false){}

    ProjectileEntity(const Vec2f& position, const Vec2f& velocity)
        : PositionEntity(position), velocity_(velocity), active_(true){ }

    virtual void Update(float delta_time) override {
        if (!active_) return;
        Move(velocity_ * delta_time);
    }

    void Draw(GamesEngineeringBase::Window& window, const Vec2f& camera_pos) {
        if (!active_) return;
        Vec2f screen_pos = { GetPosition().x_ - camera_pos.x_, GetPosition().y_ - camera_pos.y_ };
        Renderer_drawCircle(window,
            static_cast<int>(screen_pos.x_),
            static_cast<int>(screen_pos.y_),
            6, 0, 255, 255, 1);
    }

    bool isActive() const { return active_; }
    void Deactivate() { active_ = false; }
    void Activate(const Vec2f pos, const Vec2f vel) {
        SetPosition(pos);
        velocity_ = vel;
        active_ = true;
    }

private:
    Vec2f velocity_;
    bool active_;
};
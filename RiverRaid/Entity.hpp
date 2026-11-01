#pragma once

#include "vec2f.hpp"



class Entity {
    public:
    Entity() = default;

    virtual void Update(float delta_time) =0;
};

// ------------------------------------------------------------

class PositionEntity : public Entity {
    public:

    PositionEntity(const Vec2f & position)
    : Entity()
    , position_{position}
    {

    }

    const Vec2f& GetPosition() const { return position_; }
    void SetPosition(const Vec2f& position) { position_ = position; }
    void Move(const Vec2f& delta) 
    { 
        position_ = position_ + delta; 
    }

private:
    Vec2f position_;

};

class VisualEntity : public PositionEntity 
{
    public:

   VisualEntity(const Vec2f& position)
        : PositionEntity(position)
    {
    }

    virtual void Update(float delta_time) override =0;
    virtual void Draw(GamesEngineeringBase::Window& window) =0; 
};


class BackgroundEntity : public VisualEntity {

public:
    BackgroundEntity(const Vec2f& position)
        : VisualEntity(position)
    {

    }

    virtual void Update(float delta_time) override {
        // Background might not have any update logic for now
    }

    virtual void Draw(GamesEngineeringBase::Window& window) override {
        // Implement background drawing logic here
        for (unsigned int y = 0; y < window.getHeight(); y += 1) 
        {
            for (unsigned int x = 0; x < window.getWidth(); x += 1) 
            {
                 window.draw(x, y, 255, 128, 0);
            }
        }
    }
};



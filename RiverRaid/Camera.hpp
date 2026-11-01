#pragma once
#include "vec2f.hpp"
#include <cmath>

class Camera 
{
public:
	Camera(float window_width, float window_height)
		: position_{0.0f, 0.0f}
		, width_(window_width)
		, height_(window_height){}

	//using this to update camera position to lock onto player or other potential targets
	void Follow(const Vec2f& target_pos) {
		//center camera on target
		position_.x_ = roundf(target_pos.x_ - (width_ / 2.0f));
		position_.y_ = target_pos.y_ - (height_ / 2.0f);
	}

	//get the camera offset vector for rendering world elemtns
	Vec2f GetPosition() const {
		return position_;
	}

	void SetPosition(const Vec2f pos) {
		position_ = pos;
	}

private:
	Vec2f position_;
	float width_;
	float height_;

};
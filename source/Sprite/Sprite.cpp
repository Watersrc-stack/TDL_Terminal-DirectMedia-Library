
#include "Sprite/Sprite.hpp"
#include "Vector.hpp"
#include "Rect.hpp"
#include "Pixel/Pixel.hpp"
#include "Window/window.hpp"
#include <optional>
#include <algorithm>
#include <cmath>

/**
 * @brief Create a Sprite object
 * 
 * @param texture the texture of the sprite to create
 * @param pos the position of the sprite
 * @return tdl::Sprite* the sprite created
 */
tdl::Sprite *tdl::Sprite::createSprite(tdl::Texture *texture, tdl::Vector2u pos)
{
    return new tdl::Sprite(texture, pos);
}

/**
 * @brief Create a Sprite object with an pos and a rect
 * 
 * @param texture the texture of the sprite to create
 * @param pos the position of the sprite
 * @param rect the rect of the sprite
 * @return tdl::Sprite* the sprite created
 */
tdl::Sprite* tdl::Sprite::createSprite(tdl::Texture *texture, tdl::Vector2u pos, tdl::RectU rect)
{
    return new tdl::Sprite(texture, pos, rect);
}

/**
 * @brief Construct a new tdl::Sprite::Sprite object
 * 
 * @param texture the texture of the sprite
 * @param pos the position of the sprite
 */
tdl::Sprite::Sprite(tdl::Texture *texture, tdl::Vector2u pos)
{
    _texture = texture;
    _pos = pos;
    tdl::Vector2u size = texture->getSize();
    _rect = tdl::RectU(0, 0, x(size), y(size));
}

/**
 * @brief Construct a new tdl::Sprite::Sprite object
 * 
 * @param texture the texture of the sprite
 * @param pos the position of the sprite
 * @param rect the rect of the sprite
 */
tdl::Sprite::Sprite(Texture *texture, Vector2u pos, RectU rect)
{
    _texture = texture;
    _pos = pos;
    _rect = rect;
}

/**
 * @brief Destroy the tdl::Sprite::Sprite object
 * @note it will delete the texture of the sprite
 */
tdl::Sprite::~Sprite()
{
    delete _texture;
}

/**
 * @brief check if the pixel is black
 * 
 * @param pixel the pixel to check
 * @return true if the pixel is black
 * @return false if the pixel is not black
 */
bool tdl::Sprite::isBlackPixel(Pixel pixel)
{
    return static_cast<uint8_t>(GET_R(pixel.color)) == 0 && static_cast<uint8_t>(GET_G(pixel.color)) == 0 && static_cast<uint8_t>(GET_B(pixel.color)) == 0;
}

tdl::Pixel tdl::Sprite::lerp(tdl::Pixel a, tdl::Pixel b, double t) {
    return tdl::Pixel(
        static_cast<uint8_t>((1 - t) * GET_R(a.color) + t * GET_R(b.color)),
        static_cast<uint8_t>((1 - t) * GET_G(a.color) + t * GET_G(b.color)),
        static_cast<uint8_t>((1 - t) * GET_B(a.color) + t * GET_B(b.color)),
        static_cast<uint8_t>((1 - t) * GET_A(a.color) + t * GET_A(b.color))
    );
}

/**
 * @brief draw the sprite on the window
 * This is the main function of the sprite it will draw the sprite according to the variable set in the sprite class
 * this is what is handle :
 * - the position of the sprite
 * - the rect of the sprite
 * - the tint of the sprite
 * - the rotation of the sprite
 * - the texture of the sprite and his repetition
 * 
 * @param window the window to draw the sprite on
 */
void tdl::Sprite::drawOn(Window *window)
{
    u_int32_t i = x(_pos);
    u_int32_t j = y(_pos);
    u_int32_t width = tdl::width(_rect);
    u_int32_t height = tdl::height(_rect);

    RectU textureRect =_texture->getRect();

    static tdl::Vector2f saveScale = Vector2f(1.0, 1.0);
    if (_texture->getScale() != saveScale) {
        _texture->resizeImage();
        saveScale = _texture->getScale();
    }
    double centerX = (width / 2);
    double centerY = (height / 2);
    double theta = _rotation * M_PI / 180;

    for (u_int32_t y = tdl::y(textureRect), y_tot = tdl::y(textureRect); y_tot < height + tdl::y(textureRect); y++, y_tot++) {
        for (u_int32_t x = tdl::x(textureRect), x_tot = tdl::x(textureRect); x_tot < width + tdl::x(textureRect); x++, x_tot++) {
            double xRot = (i - centerX) * cos(theta) - (j - centerY) * sin(theta) + centerX;
            double yRot = (i - centerX) * sin(theta) + (j - centerY) * cos(theta) + centerY;

            tdl::Pixel color = window->getPixel(Vector2u(i, j)) + _texture->getPixel(Vector2u(x, y));
            if (!isBlackPixel(color) && _tint.has_value())
                color = color + _tint.value();
            window->setPixel(Vector2u(floor(xRot), floor(yRot)), color);
            window->setPixel(Vector2u(ceil(xRot), floor(yRot)), color);

            if ((x >= tdl::width(textureRect) + tdl::x(textureRect) - 1) && !_texture->getRepeat())
                break;
            if ((x >= tdl::width(textureRect) + tdl::x(textureRect) - 1) && _texture->getRepeat())
                x = tdl::x(textureRect);
            i++;
        }
        if ( y >= tdl::height(textureRect) + tdl::y(textureRect) - 1 && !_texture->getRepeat())
            break;
        if (( y >= tdl::height(textureRect)+ tdl::y(textureRect) - 1 ) && _texture->getRepeat())
            y = tdl::y(textureRect);
        i = x(_pos);
        j++;
    }
}

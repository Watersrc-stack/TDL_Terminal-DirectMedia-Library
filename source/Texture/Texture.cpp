
#include <iostream>
#include "Texture/Texture.hpp"
#include "Vector.hpp"
#include "Rect.hpp"
#include "Pixel/Pixel.hpp"

namespace tdl {

    /**
     * @brief function to create a texture
     * 
     * @param path the path to the png file to load
     * @return Texture* the texture created
     * @overload
     */
    Texture *Texture::createTexture(std::string path)
    {
        return new Texture(path, Vector2f(1, 1), false);
    }

    /**
     * @brief function to create a texture
     * 
     * @param path the path to the png file to load
     * @param scale the scale of the texture
     * @return Texture* the texture created
     * @overload
     */
    Texture *Texture::createTexture(std::string path, Vector2f scale)
    {
        return new Texture(path, scale, false);
    }

    /**
     * @brief function to create a texture
     * 
     * @param path the path to the png file to load
     * @param repeat the repeat of the texture
     * @return Texture* the texture created
     * @overload
     */
    Texture *Texture::createTexture(std::string path, bool repeat)
    {
        return new Texture(path, Vector2f(1, 1), repeat);
    }

    /**
     * @brief function to create a texture
     * 
     * @param path the path to the png file to load
     * @param scale the scale of the texture
     * @param repeat the repeat of the texture
     * @return Texture* the texture created
     */
    Texture *Texture::createTexture(std::string path, Vector2f scale, bool repeat)
    {
        return new Texture(path, scale, repeat);
    }

    /**
     * @brief Construct a new Texture:: Texture object
     * 
     * @param path the path to the png file to load
     * @param scale the scale of the texture
     * @param repeat the repeat of the texture
     */
    Texture::Texture(std::string path, Vector2f scale, bool repeat) : TextureLoader(path), _scale(scale), _repeat(repeat)
    {
        loadPixels();
    }

    /**
     * @brief Destroy the Texture:: Texture object
     * 
     */
    Texture::~Texture()
    {
    }

    /**
     * @brief load the image form in pixelData. The load is based from the TextureLoader data
     * @note if you call this function out of the constructor, you can reload the image
     */
    void Texture::loadPixels()
    {
        if (_pixelData.size() > 0)
        _pixelData.clear();
        for (u_int32_t y = 0; y < tdl::y(_size); y++) {
            std::vector<Pixel> row;
            for (u_int32_t x = 0; x < tdl::x(_size); x++) {
                png_byte *ptr = &(_row_pointers[y][x * _channels]);
                Pixel color;
                if (_channels == 3 || _channels == 4) {
                    color = Pixel(ptr[0], ptr[1], ptr[2], _channels == 4 ? ptr[3] : 255);
                } else if (_channels == 2) {
                    color = Pixel(ptr[0], ptr[0], ptr[0], ptr[1]);
                } else if (_channels == 1) {
                    color = Pixel(ptr[0], ptr[0], ptr[0], 255);
                }
                row.push_back(color);
            }
            _pixelData.push_back(row);
        }
    }

    /**
     * @brief resize the image to the scale set in the texture
     * the algorithm used is the Nearest Neighbor it is the fastest but the less accurate
     * 
     * @note the function does not resize the image if the width or height is 0
     * @warning the function can occur data loss if the size is to small
     * @exceptsafe strong the function does not resize the image if the width or height is 0
     */
    void Texture::resizeImage()
    {
        Vector2u endSize = Vector2u(tdl::x(_size) * x(_scale), tdl::y(_size) * y(_scale));
        if (x(endSize) == 0 || y(endSize) == 0)
            std::cerr << "Width and Height must be greater than 0" << std::endl;
        else {
            if (!_pixelData.empty())
                _pixelData.clear();
            std::vector<std::vector<Pixel>> newPixelsTab(y(endSize), std::vector<Pixel>(x(endSize),Pixel(0, 0, 0,0)));
            for (u_int32_t y = 0; y < tdl::y(endSize); y++) {
                for (u_int32_t x = 0; x < tdl::x(endSize); x++) {
                    int x_ratio = (int) ((x * (tdl::x(_size) - 1)) / tdl::x(endSize));
                    int y_ratio = (int) ((y * (tdl::y(_size) - 1)) / tdl::y(endSize));
                    Pixel color = _pixelData[y_ratio][x_ratio];
                    newPixelsTab[y][x] = color;
                }
            }
        }
    }
}   
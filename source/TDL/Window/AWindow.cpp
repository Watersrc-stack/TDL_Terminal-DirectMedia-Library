
#include <iostream>
#include <cstring>
#include <utility>
#include <vector>

#include <unistd.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>

#include "TDL/Signal/SignalHandler.hpp"
#include "TDL/Matrix/PixelMatrix.hpp"
#include "TDL/Event/Mouse/Linux/Mouse.hpp"
#include "TDL/Event/Mouse/EventMouseData.hpp"
#include "TDL/Event/Event.hpp"
#include "TDL/Window/AWindow.hpp"

void tdl::AWindow::disableEcho()
{
    tcgetattr(_fd, &_tty);
    _tty.c_lflag &=(~ICANON & ~ECHO);
    _tty.c_cc[VMIN] = 1;
    _tty.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO,TCSANOW,&_tty);
}

void tdl::AWindow::draw()
{
    if (!_content.empty()) {
        write(_fd, _content.c_str(), _content.size());
        _content = "";
    }
}

void tdl::AWindow::printFrameRate() {
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    if (elapsed_seconds.count() >= 1) {
        std::cerr << "Frame rate : " << framecounter << std::endl;
        framecounter = 0;
        start = std::chrono::system_clock::now();
    }
    framecounter++;
}

/**
 * @brief reset the pixel table of the window with black pixel
 * 
 */
void tdl::AWindow::clearPixel()
{
    if (getUpdate()) {
        getMatrix().clear();
        setUpdate(false);
    }
}

void tdl::AWindow::update(bool all) {
    CharColor charColor;
    Vector2u pos = Vector2u(0, 0);
    Vector2u oldPos = Vector2u(0, 0);
    Pixel pixels[6] = {Pixel(0, 0, 0, 0)};
    Pixel oldPixels[6] = {Pixel(0, 0, 0, 0)};
    u_int32_t sizeX = getMatrix().getSize().x();
    u_int32_t sizeY = getMatrix().getSize().y();

    Pixel oldForeColor = Pixel(0, 0, 0, 0);
    Pixel oldBackColor = Pixel(0, 0, 0, 0);

    if (_update)
        _update = false;
    setUpdate(true);
    for (u_int32_t i = 0; i < sizeY; i += 3) {
        for (u_int32_t j = 0; j < sizeX; j += 2) {
            if (getMatrix().getPixelChar(j, i, pixels) == false)
                continue;
            getOldMatrix().setPixelChar(j, i, pixels);
            charColor = getMatrix().computeCharColor(Vector2u(j, i), pixels, charColor);
            if (!charColor.shape)
                continue;
            if (pos.x() != oldPos.x() + 2) {
                moveCursor(pos);
            }
            if (charColor.ForeGround != oldForeColor) {
                setRGBFrontGround(charColor.ForeGround);
                oldForeColor = charColor.ForeGround;
            }
            if ((charColor.BackGround != oldBackColor)) {
                setRGBBackGround(charColor.BackGround);
                oldBackColor = charColor.BackGround;
            }
            printPixel(charColor.shape);
            oldPos = pos;
            pos.x() += 2;
        }
        pos.x() = 0;
        pos.y() += 2;
    }
    _firstLoop = false;
}
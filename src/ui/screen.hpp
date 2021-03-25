#pragma once

#include <bitset>

class Screen
{
public:
	Screen();
	~Screen() {};
    const std::bitset<64 * 32> getPixels() { return _pixels; };

private:
    std::bitset<64 * 32> _pixels;
};
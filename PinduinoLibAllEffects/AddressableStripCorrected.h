#ifndef ADDRESSABLE_STRIP_CORRECTED_H
#define ADDRESSABLE_STRIP_CORRECTED_H

#include <Adafruit_NeoPixel.h>

class AddressableStrip {
private:
  Adafruit_NeoPixel *_strip;

public:
  AddressableStrip(int num, int pin, void* unused) {
    _strip = new Adafruit_NeoPixel(num, pin, NEO_GRB + NEO_KHZ800);
  }

  ~AddressableStrip() {
    delete _strip;
  }

  Adafruit_NeoPixel* strip() { return _strip; }
  void clear() { _strip->clear(); }

  // Safe pixel setting with bounds checking
  inline void setPixelColorSafe(int pixel, int r, int g, int b) {
    if (pixel >= 0 && pixel < _strip->numPixels()) {
      _strip->setPixelColor(pixel, r, g, b);
    }
  }

  // Static effects
  void colorRGB(int r, int g, int b) {
    for (int i = 0; i < _strip->numPixels(); i++) {
      setPixelColorSafe(i, r, g, b);
    }
  }

  void color(String color) {
    if (color == "red") colorRGB(255, 0, 0);
    else if (color == "green") colorRGB(0, 255, 0);
    else if (color == "blue") colorRGB(0, 0, 255);
    else if (color == "yellow") colorRGB(255, 255, 0);
    else if (color == "cyan") colorRGB(0, 255, 255);
    else if (color == "purple") colorRGB(255, 0, 255);
    else if (color == "orange") colorRGB(255, 165, 0);
    else if (color == "pink") colorRGB(255, 192, 203);
    else colorRGB(255, 255, 255); // Default white
  }

  void fadeInRGB(int r, int g, int b, float time) {
    int steps = time / 50;
    for (int i = 0; i <= steps; i++) {
      float factor = (float)i / steps;
      for (int j = 0; j < _strip->numPixels(); j++) {
        setPixelColorSafe(j, r * factor, g * factor, b * factor);
      }
      _strip->show();
      delay(50);
    }
  }

  void fadeIn(String color, float time) {
    if (color == "yellow") fadeInRGB(255, 255, 0, time);
    else if (color == "cyan") fadeInRGB(0, 255, 255, time);
    else if (color == "orange") fadeInRGB(255, 165, 0, time);
    else fadeInRGB(255, 255, 255, time); // Default white
  }

  void fadeRGB2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float time) {
    int steps = time / 50;
    for (int i = 0; i <= steps; i++) {
      float factor = (float)i / steps;
      for (int j = 0; j < _strip->numPixels(); j++) {
        int r = r1 + (r2 - r1) * factor;
        int g = g1 + (g2 - g1) * factor;
        int b = b1 + (b2 - b1) * factor;
        setPixelColorSafe(j, r, g, b);
      }
      _strip->show();
      delay(50);
    }
  }

  void fadeColor2Color(String color1, String color2, float time) {
    int r1 = (color1 == "green") ? 0 : 255;
    int g1 = (color1 == "green") ? 255 : 255;
    int b1 = (color1 == "green") ? 0 : 255;
    int r2 = (color2 == "purple") ? 255 : 255;
    int g2 = (color2 == "purple") ? 0 : 255;
    int b2 = (color2 == "purple") ? 255 : 255;
    fadeRGB2RGB(r1, g1, b1, r2, g2, b2, time);
  }

  void fadeOut(float time) {
    int steps = time / 50;
    for (int i = steps; i >= 0; i--) {
      float factor = (float)i / steps;
      for (int j = 0; j < _strip->numPixels(); j++) {
        uint32_t color = _strip->getPixelColor(j);
        int r = ((color >> 16) & 0xFF) * factor;
        int g = ((color >> 8) & 0xFF) * factor;
        int b = (color & 0xFF) * factor;
        setPixelColorSafe(j, r, g, b);
      }
      _strip->show();
      delay(50);
    }
  }

  void RGBBand(int pos, int r, int g, int b, int span) {
    _strip->clear();
    for (int i = pos - span / 2; i <= pos + span / 2; i++) {
      setPixelColorSafe(i, r, g, b);
    }
  }

  void RGBBandCont(int pos, int r, int g, int b, int span, int startLED, int endLED) {
    _strip->clear();
    for (int i = pos - span / 2; i <= pos + span / 2; i++) {
      if (i >= startLED && i <= endLED) {
        setPixelColorSafe(i, r, g, b);
      }
    }
  }

  // Dynamic effects
  void chaseRGB(int r, int g, int b, int span, int time, int dir) {
    int steps = _strip->numPixels() / span;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (steps - i - 1);
      for (int j = 0; j < span; j++) {
        int pixel = pos * span + j;
        setPixelColorSafe(pixel, r, g, b);
      }
      _strip->show();
      delay(time);
    }
  }

  void chase(String color, float span, int time, int dir) {
    if (color == "cyan") chaseRGB(0, 255, 255, span, time, dir);
    else chaseRGB(255, 255, 255, span, time, dir);
  }

  void chase2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir) {
    int steps = _strip->numPixels() / span;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (steps - i - 1);
      for (int j = 0; j < span / 2; j++) {
        int pixel1 = pos * span + j;
        setPixelColorSafe(pixel1, r1, g1, b1);
      }
      for (int j = span / 2; j < span; j++) {
        int pixel2 = pos * span + j;
        setPixelColorSafe(pixel2, r2, g2, b2);
      }
      _strip->show();
      delay(time);
    }
  }

  void chase2Color(String color1, String color2, float span, int time, int dir) {
    int r1 = (color1 == "blue") ? 0 : 255;
    int g1 = (color1 == "blue") ? 0 : 255;
    int b1 = (color1 == "blue") ? 255 : 255;
    int r2 = (color2 == "yellow") ? 255 : 255;
    int g2 = (color2 == "yellow") ? 255 : 255;
    int b2 = (color2 == "yellow") ? 0 : 255;
    chase2RGB(r1, g1, b1, r2, g2, b2, span, time, dir);
  }

  void chase2RGBFromPoint(int pos, float r1, float g1, float b1, float r2, float g2, float b2, int span, int time) {
    int steps = max(pos, _strip->numPixels() - pos) / span;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      for (int j = 0; j < span / 2; j++) {
        int pixelLeft = pos - (i * span) - j;
        int pixelRight = pos + (i * span) + j;
        setPixelColorSafe(pixelLeft, r1, g1, b1);
        setPixelColorSafe(pixelRight, r2, g2, b2);
      }
      _strip->show();
      delay(time);
    }
  }

  void chase2ColorFromPoint(int pos, String color1, String color2, int span, int time) {
    int r1 = (color1 == "blue") ? 0 : 255;
    int g1 = (color1 == "blue") ? 0 : 255;
    int b1 = (color1 == "blue") ? 255 : 255;
    int r2 = (color2 == "yellow") ? 255 : 255;
    int g2 = (color2 == "yellow") ? 255 : 255;
    int b2 = (color2 == "yellow") ? 0 : 255;
    chase2RGBFromPoint(pos, r1, g1, b1, r2, g2, b2, span, time);
  }

  void chase2RGBCont(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir, int startLED, int endLED) {
    int steps = (endLED - startLED) / span;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (steps - i - 1);
      for (int j = 0; j < span / 2; j++) {
        int pixel1 = startLED + pos * span + j;
        if (pixel1 >= startLED && pixel1 <= endLED) setPixelColorSafe(pixel1, r1, g1, b1);
      }
      for (int j = span / 2; j < span; j++) {
        int pixel2 = startLED + pos * span + j;
        if (pixel2 >= startLED && pixel2 <= endLED) setPixelColorSafe(pixel2, r2, g2, b2);
      }
      _strip->show();
      delay(time);
    }
  }

  void chase2ColorCont(String color1, String color2, float span, int time, int dir, int startLED, int endLED) {
    int r1 = (color1 == "cyan") ? 0 : 255;
    int g1 = (color1 == "cyan") ? 255 : 255;
    int b1 = (color1 == "cyan") ? 255 : 255;
    int r2 = (color2 == "orange") ? 255 : 255;
    int g2 = (color2 == "orange") ? 165 : 255;
    int b2 = (color2 == "orange") ? 0 : 255;
    chase2RGBCont(r1, g1, b1, r2, g2, b2, span, time, dir, startLED, endLED);
  }

  void chaseColorCont(String color, float span, int time, int dir, int startLED, int endLED) {
    int r = (color == "pink") ? 255 : 255;
    int g = (color == "pink") ? 192 : 255;
    int b = (color == "pink") ? 203 : 255;
    int steps = (endLED - startLED) / span;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (steps - i - 1);
      for (int j = 0; j < span; j++) {
        int pixel = startLED + pos * span + j;
        if (pixel >= startLED && pixel <= endLED) setPixelColorSafe(pixel, r, g, b);
      }
      _strip->show();
      delay(time);
    }
  }

  void spreadInFromPoint2RGB(int pos, float r1, float g1, float b1, float r2, float g2, float b2, int time) {
    int steps = max(pos, _strip->numPixels() - pos) / 2;
    for (int i = steps; i >= 0; i--) {
      _strip->clear();
      for (int j = 0; j < _strip->numPixels(); j++) {
        if (j <= pos - i) setPixelColorSafe(j, r1, g1, b1);
        else if (j >= pos + i) setPixelColorSafe(j, r2, g2, b2);
      }
      _strip->show();
      delay(time / steps);
    }
  }

  void spreadInFromPoint2Color(int pos, String color1, String color2, int time) {
    int r1 = (color1 == "green") ? 0 : 255;
    int g1 = (color1 == "green") ? 255 : 255;
    int b1 = (color1 == "green") ? 0 : 255;
    int r2 = (color2 == "yellow") ? 255 : 255;
    int g2 = (color2 == "yellow") ? 255 : 255;
    int b2 = (color2 == "yellow") ? 0 : 255;
    spreadInFromPoint2RGB(pos, r1, g1, b1, r2, g2, b2, time);
  }

  void spreadInFromPointRGB(int pos, float r, float g, float b, int time) {
    int steps = max(pos, _strip->numPixels() - pos) / 2;
    for (int i = steps; i >= 0; i--) {
      _strip->clear();
      for (int j = pos - i; j <= pos + i; j++) {
        setPixelColorSafe(j, r, g, b);
      }
      _strip->show();
      delay(time / steps);
    }
  }

  void spreadInFromPoint(int pos, String color, int time) {
    if (color == "cyan") spreadInFromPointRGB(pos, 0, 255, 255, time);
    else spreadInFromPointRGB(pos, 255, 255, 255, time);
  }

  void spreadOutFromPoint(int pos, float time) {
    int steps = max(pos, _strip->numPixels() - pos) / 2;
    for (int i = 0; i <= steps; i++) {
      _strip->clear();
      for (int j = 0; j < _strip->numPixels(); j++) {
        if (j < pos - i || j > pos + i) {
          uint32_t color = _strip->getPixelColor(j);
          int r = (color >> 16) & 0xFF;
          int g = (color >> 8) & 0xFF;
          int b = color & 0xFF;
          setPixelColorSafe(j, r, g, b);
        }
      }
      _strip->show();
      delay(time / steps);
    }
  }

  void spreadOutToPoint(int pos, float time) {
    int steps = max(pos, _strip->numPixels() - pos) / 2;
    for (int i = steps; i >= 0; i--) {
      _strip->clear();
      for (int j = 0; j < _strip->numPixels(); j++) {
        if (j <= pos - i || j >= pos + i) {
          uint32_t color = _strip->getPixelColor(j);
          int r = (color >> 16) & 0xFF;
          int g = (color >> 8) & 0xFF;
          int b = color & 0xFF;
          setPixelColorSafe(j, r, g, b);
        }
      }
      _strip->show();
      delay(time / steps);
    }
  }

  void explosionRGB(int pos, int r, int g, int b, int span) {
    _strip->clear();
    for (int i = pos - span / 2; i <= pos + span / 2; i++) {
      setPixelColorSafe(i, r, g, b);
    }
  }

  void explosion(int pos, String color, int span) {
    if (color == "purple") explosionRGB(pos, 255, 0, 255, span);
    else explosionRGB(pos, 255, 165, 0, span); // Orange as fallback
  }

  void rainbow(int wait) {
    for (int i = 0; i < 256; i++) {
      for (int j = 0; j < _strip->numPixels(); j++) {
        int pixelHue = (i + (j * 256 / _strip->numPixels())) % 256;
        uint32_t color = _strip->gamma32(_strip->ColorHSV(pixelHue * 256));
        int r = (color >> 16) & 0xFF;
        int g = (color >> 8) & 0xFF;
        int b = color & 0xFF;
        setPixelColorSafe(j, r, g, b);
      }
      _strip->show();
      delay(wait);
    }
  }

  void rainbowCycle(int wait) {
    for (int i = 0; i < 256; i++) {
      for (int j = 0; j < _strip->numPixels(); j++) {
        int pixelHue = (i * 256 / _strip->numPixels() + j) % 256;
        uint32_t color = _strip->gamma32(_strip->ColorHSV(pixelHue * 256));
        int r = (color >> 16) & 0xFF;
        int g = (color >> 8) & 0xFF;
        int b = color & 0xFF;
        setPixelColorSafe(j, r, g, b);
      }
      _strip->show();
      delay(wait);
    }
  }

  void RGBBullet(int pos, int r, int g, int b, int span, int dir) {
    int steps = _strip->numPixels() - span + 1;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int start = (dir > 0) ? i : (_strip->numPixels() - span - i);
      for (int j = 0; j < span; j++) {
        int pixel = start + j;
        setPixelColorSafe(pixel, r, g, b);
      }
      _strip->show();
      delay(20);
    }
  }

  void RGBBulletCont(int pos, int r, int g, int b, int span, int dir) {
    RGBBullet(pos, r, g, b, span, dir); // Simplified continuous version
  }

  void bulletP2P_2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir, int start, int end) {
    int steps = abs(end - start) / span;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int pos = (dir > 0) ? start + i * span : end - i * span;
      for (int j = 0; j < span / 2; j++) {
        int pixel1 = pos + j;
        if (pixel1 >= start && pixel1 <= end) setPixelColorSafe(pixel1, r1, g1, b1);
      }
      for (int j = span / 2; j < span; j++) {
        int pixel2 = pos + j;
        if (pixel2 >= start && pixel2 <= end) setPixelColorSafe(pixel2, r2, g2, b2);
      }
      _strip->show();
      delay(time);
    }
  }

  void bulletP2P_2Color(String color1, String color2, float span, int time, int dir, int start, int end) {
    int r1 = (color1 == "blue") ? 0 : 255;
    int g1 = (color1 == "blue") ? 0 : 255;
    int b1 = (color1 == "blue") ? 255 : 255;
    int r2 = (color2 == "yellow") ? 255 : 255;
    int g2 = (color2 == "yellow") ? 255 : 255;
    int b2 = (color2 == "yellow") ? 0 : 255;
    bulletP2P_2RGB(r1, g1, b1, r2, g2, b2, span, time, dir, start, end);
  }

  void bulletFromPoint2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int start_pos) {
    int steps = max(start_pos, _strip->numPixels() - start_pos) / span;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      for (int j = 0; j < span / 2; j++) {
        int pixelLeft = start_pos - (i * span) - j;
        int pixelRight = start_pos + (i * span) + j;
        setPixelColorSafe(pixelLeft, r1, g1, b1);
        setPixelColorSafe(pixelRight, r2, g2, b2);
      }
      _strip->show();
      delay(time);
    }
  }

  void bulletFromPointRGB(int r, int g, int b, int span, int time, int start_pos) {
    int steps = max(start_pos, _strip->numPixels() - start_pos) / span;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      for (int j = 0; j < span; j++) {
        int pixelLeft = start_pos - (i * span) - j;
        int pixelRight = start_pos + (i * span) + j;
        setPixelColorSafe(pixelLeft, r, g, b);
        setPixelColorSafe(pixelRight, r, g, b);
      }
      _strip->show();
      delay(time);
    }
  }

  void bulletFromPoint(String color, float span, int time, int start_pos) {
    if (color == "orange") bulletFromPointRGB(255, 165, 0, span, time, start_pos);
    else bulletFromPointRGB(255, 255, 255, span, time, start_pos);
  }

  void bulletFromPoint2Color(String color1, String color2, float span, int time, int start_pos) {
    int r1 = (color1 == "red") ? 255 : 255;
    int g1 = (color1 == "red") ? 0 : 255;
    int b1 = (color1 == "red") ? 0 : 255;
    int r2 = (color2 == "blue") ? 0 : 255;
    int g2 = (color2 == "blue") ? 0 : 255;
    int b2 = (color2 == "blue") ? 255 : 255;
    bulletFromPoint2RGB(r1, g1, b1, r2, g2, b2, span, time, start_pos);
  }

  void bullet2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir) {
    int steps = _strip->numPixels() - span + 1;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (_strip->numPixels() - span - i);
      for (int j = 0; j < span / 2; j++) {
        int pixel1 = pos + j;
        setPixelColorSafe(pixel1, r1, g1, b1);
      }
      for (int j = span / 2; j < span; j++) {
        int pixel2 = pos + j;
        setPixelColorSafe(pixel2, r2, g2, b2);
      }
      _strip->show();
      delay(time);
    }
  }

  void bulletRGB(int r, int g, int b, int span, int time, int dir) {
    int steps = _strip->numPixels() - span + 1;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (_strip->numPixels() - span - i);
      for (int j = 0; j < span; j++) {
        int pixel = pos + j;
        setPixelColorSafe(pixel, r, g, b);
      }
      _strip->show();
      delay(time);
    }
  }

  void bullet2Color(String color1, String color2, float span, int time, int dir) {
    int r1 = (color1 == "cyan") ? 0 : 255;
    int g1 = (color1 == "cyan") ? 255 : 255;
    int b1 = (color1 == "cyan") ? 255 : 255;
    int r2 = (color2 == "pink") ? 255 : 255;
    int g2 = (color2 == "pink") ? 192 : 255;
    int b2 = (color2 == "pink") ? 203 : 255;
    bullet2RGB(r1, g1, b1, r2, g2, b2, span, time, dir);
  }

  void bullet(String color, float span, int time, int dir) {
    if (color == "yellow") bulletRGB(255, 255, 0, span, time, dir);
    else bulletRGB(255, 255, 255, span, time, dir);
  }

  void multiBullet2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir, int num) {
    int steps = _strip->numPixels() / (span * num);
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      for (int k = 0; k < num; k++) {
        int pos = (dir > 0) ? i + k * steps : (_strip->numPixels() - span - (i + k * steps));
        for (int j = 0; j < span / 2; j++) {
          int pixel1 = pos + j;
          setPixelColorSafe(pixel1, r1, g1, b1);
        }
        for (int j = span / 2; j < span; j++) {
          int pixel2 = pos + j;
          setPixelColorSafe(pixel2, r2, g2, b2);
        }
      }
      _strip->show();
      delay(time);
    }
  }

  void multiBulletRGB(int r, int g, int b, int span, int time, int dir, int num) {
    int steps = _strip->numPixels() / (span * num);
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      for (int k = 0; k < num; k++) {
        int pos = (dir > 0) ? i + k * steps : (_strip->numPixels() - span - (i + k * steps));
        for (int j = 0; j < span; j++) {
          int pixel = pos + j;
          setPixelColorSafe(pixel, r, g, b);
        }
      }
      _strip->show();
      delay(time);
    }
  }

  void multiBullet(String color, float span, int time, int dir, int num) {
    if (color == "purple") multiBulletRGB(255, 0, 255, span, time, dir, num);
    else multiBulletRGB(255, 255, 255, span, time, dir, num);
  }

  void multiBullet2Color(String color1, String color2, float span, int time, int dir, int num) {
    int r1 = (color1 == "blue") ? 0 : 255;
    int g1 = (color1 == "blue") ? 0 : 255;
    int b1 = (color1 == "blue") ? 255 : 255;
    int r2 = (color2 == "orange") ? 255 : 255;
    int g2 = (color2 == "orange") ? 165 : 255;
    int b2 = (color2 == "orange") ? 0 : 255;
    multiBullet2RGB(r1, g1, b1, r2, g2, b2, span, time, dir, num);
  }

  void Matrix2RGB(float r1, float g1, float b1, float r2, float g2, float b2, float span, int time, int dir) {
    int steps = _strip->numPixels() / span;
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (steps - i - 1);
      for (int j = 0; j < span; j++) {
        int pixel = pos * span + j;
        setPixelColorSafe(pixel, (j % 2 == 0) ? r1 : r2, (j % 2 == 0) ? g1 : g2, (j % 2 == 0) ? b1 : b2);
      }
      _strip->show();
      delay(time);
    }
  }

  void Matrix2Color(String color1, String color2, float span, int time, int dir) {
    int r1 = (color1 == "red") ? 255 : 255;
    int g1 = (color1 == "red") ? 0 : 255;
    int b1 = (color1 == "red") ? 0 : 255;
    int r2 = (color2 == "yellow") ? 255 : 255;
    int g2 = (color2 == "yellow") ? 255 : 255;
    int b2 = (color2 == "yellow") ? 0 : 255;
    Matrix2RGB(r1, g1, b1, r2, g2, b2, span, time, dir);
  }

  void RGBMatrix(int rows, int r, int g, int b, int span, int dir) {
    int steps = _strip->numPixels() / (span * rows);
    for (int i = 0; i < steps; i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (steps - i - 1);
      for (int k = 0; k < rows; k++) {
        for (int j = 0; j < span; j++) {
          int pixel = pos + k * steps * span + j;
          setPixelColorSafe(pixel, r, g, b);
        }
      }
      _strip->show();
      delay(20);
    }
  }

  void sparkleRGB(int r, int g, int b, int density, int speed) {
    for (int i = 0; i < density; i++) {
      int pixel = random(_strip->numPixels());
      setPixelColorSafe(pixel, r, g, b);
      _strip->show();
      delay(speed);
      setPixelColorSafe(pixel, 0, 0, 0);
    }
  }

  void sparkle(String color, int density, int speed) {
    if (color == "orange") sparkleRGB(255, 165, 0, density, speed);
    else if (color == "pink") sparkleRGB(255, 192, 203, density, speed);
    else sparkleRGB(255, 255, 255, density, speed);
  }

  void dataStreamRGB(int r, int g, int b, int density, int speed, int dir) {
    for (int i = 0; i < _strip->numPixels(); i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (_strip->numPixels() - i - 1);
      for (int j = 0; j < density; j++) {
        int pixel = pos - j;
        setPixelColorSafe(pixel, r, g, b);
      }
      _strip->show();
      delay(speed);
    }
  }

  void dataStream(String color, int density, int speed, int dir) {
    if (color == "green") dataStreamRGB(0, 255, 0, density, speed, dir);
    else dataStreamRGB(255, 255, 255, density, speed, dir);
  }

  void dataStreamNoTailRGB(int r, int g, int b, int density, int speed, int dir) {
    for (int i = 0; i < _strip->numPixels(); i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (_strip->numPixels() - i - 1);
      setPixelColorSafe(pos, r, g, b);
      _strip->show();
      delay(speed);
    }
  }

  void dataStreamNoTail(String color, int density, int speed, int dir) {
    if (color == "yellow") dataStreamNoTailRGB(255, 255, 0, density, speed, dir);
    else dataStreamNoTailRGB(255, 255, 255, density, speed, dir);
  }

  void dataStreamNoTail2RGB(float r1, float g1, float b1, float r2, float g2, float b2, int density, int speed, int dir) {
    for (int i = 0; i < _strip->numPixels(); i++) {
      _strip->clear();
      int pos = (dir > 0) ? i : (_strip->numPixels() - i - 1);
      setPixelColorSafe(pos, (i % 2 == 0) ? r1 : r2, (i % 2 == 0) ? g1 : g2, (i % 2 == 0) ? b1 : b2);
      _strip->show();
      delay(speed);
    }
  }

  void dataStreamNoTail2Color(String color1, String color2, int density, int speed, int dir) {
    int r1 = (color1 == "red") ? 255 : 255;
    int g1 = (color1 == "red") ? 0 : 255;
    int b1 = (color1 == "red") ? 0 : 255;
    int r2 = (color2 == "blue") ? 0 : 255;
    int g2 = (color2 == "blue") ? 0 : 255;
    int b2 = (color2 == "blue") ? 255 : 255;
    dataStreamNoTail2RGB(r1, g1, b1, r2, g2, b2, density, speed, dir);
  }

  void equalizerRGB(float r1, float g1, float b1, float r2, float g2, float b2, int span, int spd, int tipFallDelaySpan, int dir, int nofade) {
    for (int i = 0; i < _strip->numPixels(); i++) {
      _strip->clear();
      int height = random(span);
      for (int j = 0; j < height; j++) {
        int pixel = (dir > 0) ? j : (_strip->numPixels() - 1 - j);
        setPixelColorSafe(pixel, (j < height / 2) ? r1 : r2, (j < height / 2) ? g1 : g2, (j < height / 2) ? b1 : b2);
      }
      _strip->show();
      delay(spd);
    }
  }

  void equalizer(String color1, String color2, int span, int spd, int tipFallDelaySpan, int dir) {
    int r1 = (color1 == "blue") ? 0 : 255;
    int g1 = (color1 == "blue") ? 0 : 255;
    int b1 = (color1 == "blue") ? 255 : 255;
    int r2 = (color2 == "yellow") ? 255 : 255;
    int g2 = (color2 == "yellow") ? 255 : 255;
    int b2 = (color2 == "yellow") ? 0 : 255;
    equalizerRGB(r1, g1, b1, r2, g2, b2, span, spd, tipFallDelaySpan, dir, 0);
  }

  void fire(int density, int speed) {
    for (int i = 0; i < density; i++) {
      int pixel = random(_strip->numPixels());
      int flicker = random(255);
      setPixelColorSafe(pixel, 255, flicker, 0); // Red/orange/yellow
      _strip->show();
      delay(speed);
      setPixelColorSafe(pixel, 0, 0, 0);
    }
  }

  void meteorRain(int r, int g, int b, int meteorSize, int meteorTrailDecay, boolean meteorRandomDecay, int SpeedDelay) {
    _strip->clear();
    for (int i = 0; i < _strip->numPixels() + _strip->numPixels(); i++) {
      for (int j = 0; j < _strip->numPixels(); j++) {
        if ((!meteorRandomDecay) || (random(10) > 5)) {
          int brightness = _strip->getPixelColor(j) * (meteorTrailDecay / 256.0);
          setPixelColorSafe(j, brightness, brightness, brightness);
        }
      }
      for (int j = 0; j < meteorSize; j++) {
        int pixel = i - meteorSize + j;
        setPixelColorSafe(pixel, r, g, b);
      }
      _strip->show();
      delay(SpeedDelay);
    }
  }
};

#endif
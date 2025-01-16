#ifndef ICONS_H
#define ICONS_H

#include <GxEPD2_3C.h>

// Helper function to invert colors
uint16_t invertColor(uint16_t color, bool invert);

// Icon drawing functions
void iconCloud(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t r, bool invert = false);
void iconSun(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t r, bool invert = false);
void iconMoon(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t r, bool invert = false);
void iconClearDay(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconClearNight(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconRain(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconSleet(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconSnow(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconWind(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconFog(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconCloudy(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconCloudyDay(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconCloudyNight(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconHail(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconThunderstorm(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconTornado(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s, bool invert = false);
void iconMoonPhase(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t r, float phase, bool invert = false);
void iconSunRise(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, bool direction, bool invert = false);
void iconBattery(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, byte percent, bool invert = false);
void fillEllipsis(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c, bool invert = false);

#endif

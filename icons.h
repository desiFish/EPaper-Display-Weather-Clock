#ifndef ICONS_H
#define ICONS_H

#include <GxEPD2_3C.h>

// Icon drawing functions
void iconCloud(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t r);
void iconSun(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t r);
void iconMoon(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t r);
void iconClearDay(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconClearNight(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconRain(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconSleet(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconSnow(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconWind(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconFog(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconCloudy(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconCloudyDay(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconCloudyNight(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconHail(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconThunderstorm(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconTornado(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t s);
void iconMoonPhase(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t r, float phase);
void iconSunRise(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, bool direction);
void iconBattery(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, byte percent);
void fillEllipsis(GxEPD2_3C<GxEPD2_420c_Z21, GxEPD2_420c_Z21::HEIGHT> &display, uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t c);

#endif

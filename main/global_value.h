#ifndef GLOBAL_VALUE_H
#define GLOBAL_VALUE_H

#include <QStandardPaths>
#include <mainwindow.h>

class MainWindow;

extern MainWindow *mainWindow;

#ifdef DEVICE_EVB
const int font_size = 32;
const int font_size_big = 35;
// top part
const QString return_resource_str = ":/image/main/return_big.png";
const int return_icon_width = 212;
const int return_icon_height = 70;
const int top_icon_size = 60;
#else
const int font_size = 17;
const int font_size_big = 22;
// top part
const QString return_resource_str = ":/image/main/return.png";
const int return_icon_width = 115;
const int return_icon_height = 40;
const int top_icon_size = 40;
#endif

// gallery
const QString GALLERY_SEARCH_PATH = QStandardPaths::writableLocation(QStandardPaths::HomeLocation).append("/mnt");

#endif

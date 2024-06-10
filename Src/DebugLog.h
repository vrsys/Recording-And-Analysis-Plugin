//code used from: https://stackoverflow.com/questions/43732825/use-debug-log-from-c
#ifndef RENDERINGPLUGIN_DEBUGLOG_H
#define RENDERINGPLUGIN_DEBUGLOG_H

#pragma once
#include <string>
#include <stdio.h>
#include <sstream>
#include <iostream>
#include "IUnityGraphics.h"

extern "C"{
//Create a callback delegate
typedef void(*FuncCallBack)(const char* message, int color, int size);
static FuncCallBack callbackInstance = nullptr;
void UNITY_INTERFACE_EXPORT UNITY_INTERFACE_API RegisterDebugCallback(FuncCallBack cb);
}

//Color Enum
enum class Color { Red, Green, Blue, Black, White, Yellow, Orange };

/**
 * @class Debug
 * @brief This class is used to call external debug functions. Using this class debug log of the plugin can be printed in the Unity Log.
 */
class  Debug{
public:
    static void Log(const char* message, Color color = Color::Black);
    static void Log(const std::string message, Color color = Color::Black);
    static void Log(const int message, Color color = Color::Black);
    static void Log(const char message, Color color = Color::Black);
    static void Log(const float message, Color color = Color::Black);
    static void Log(const double message, Color color = Color::Black);
    static void Log(const bool message, Color color = Color::Black);

private:
    static void send_log(const std::stringstream &ss, const Color &color);
};


#endif //RENDERINGPLUGIN_DEBUGLOG_H
//
// Created by Mortiferum on 12.02.2022.
//

#include "DebugLog.h"
#include <cstring>
#include <sstream>

//-------------------------------------------------------------------
void  Debug::Log(const char* message, Color color) {
    if (callbackInstance != nullptr) {
        try {
            callbackInstance(message, (int) color, (int) strlen(message));
        } catch (std::exception &e){
            std::cout << "An exception occurred trying to log to the delegate.\n";
        }
    } else
        std::cout << message << "\n";
}

void  Debug::Log(const std::string message, Color color) {
    const char* tmsg = message.c_str();
    Log(tmsg, color);
}

void  Debug::Log(const int message, Color color) {
    std::stringstream ss;
    ss << message;
    send_log(ss, color);
}

void  Debug::Log(const char message, Color color) {
    std::stringstream ss;
    ss << message;
    send_log(ss, color);
}

void  Debug::Log(const float message, Color color) {
    std::stringstream ss;
    ss << message;
    send_log(ss, color);
}

void  Debug::Log(const double message, Color color) {
    std::stringstream ss;
    ss << message;
    send_log(ss, color);
}

void Debug::Log(const bool message, Color color) {
    std::stringstream ss;
    if (message)
        ss << "true";
    else
        ss << "false";

    send_log(ss, color);
}

void Debug::send_log(const std::stringstream &ss, const Color &color) {
    const std::string tmp = ss.str();
    const char* tmsg = tmp.c_str();
    if (callbackInstance != nullptr)
        callbackInstance(tmsg, (int)color, (int)strlen(tmsg));
    else {
        std::cout << "\033[1;31mbold";
        std::cout << tmsg;
        std::cout << "\033[0m\n";
    }
}
//-------------------------------------------------------------------

//Create a callback delegate
void RegisterDebugCallback(FuncCallBack cb) {
    callbackInstance = cb;
}
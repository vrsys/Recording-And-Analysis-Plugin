// VRSYS plugin of Virtual Reality and Visualization Group (Bauhaus-University Weimar)
//  _    ______  _______  _______
// | |  / / __ \/ ___/\ \/ / ___/
// | | / / /_/ /\__ \  \  /\__ \
// | |/ / _, _/___/ /  / /___/ /
// |___/_/ |_|/____/  /_//____/
//
//  __                            __                       __   __   __    ___ .  . ___
// |__)  /\  |  | |__|  /\  |  | /__`    |  | |\ | | \  / |__  |__) /__` |  |   /\   |
// |__) /~~\ \__/ |  | /~~\ \__/ .__/    \__/ | \| |  \/  |___ |  \ .__/ |  |  /~~\  |
//
//       ___               __
// |  | |__  |  |\/|  /\  |__)
// |/\| |___ |  |  | /~~\ |  \
//
// Copyright (c) 2024 Virtual Reality and Visualization Group
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//-----------------------------------------------------------------
//   Authors:        Anton Lammert
//   Date:           2024
//-----------------------------------------------------------------

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
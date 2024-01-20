#ifndef LOGGER_H
#define LOGGER_H

#include "../globals.h"

void InitConsole();
void DisplayConsole();

void ResetLog();
void WriteOnLog(const char* msg);
void ResetErr();
void WriteOnErr(const char* msg);
void ResetUI(BOOL at_start);
void SetUIPosPtr(const unsigned short pos);
void WriteOnUi(const char* msg);
void SetUiChOnCurr(const char character);

// Reminder MUST be size of strout is >(padding+1)
void TextPadLeft(const char* str, const unsigned short padding, char* strout);
void TextPadRight(const char* str, const unsigned short padding, char* strout);
void TextPadCenter(const char* str, const unsigned short padding, char* strout);

#endif
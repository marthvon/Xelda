#include "logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char ui[640];
    unsigned short curr_ui;
    BOOL update_ui;

    char logs[640];
    unsigned short curr_logs;
    BOOL update_logs;
    
    char errs[640];
    unsigned short curr_errs;
    BOOL update_errs;

    char output[2048];
} Logger;
Logger console;

void InitConsole() {
    memset(console.ui, 0, sizeof(char)*640);
    memset(console.logs, 0, sizeof(char)*640);
    memset(console.errs, 0, sizeof(char)*640);
    memset(console.output, 0, sizeof(char)*2048);
}

void DisplayConsole() {
    char* outptr = console.output;
    BOOL is_update = FALSE;
    if(console.update_ui)
        strcpy(outptr, console.ui);
    outptr += strlen(console.ui);
    if(console.update_ui || console.update_logs)
        strcpy(outptr, console.logs);
    outptr += strlen(console.logs);
    if(console.update_ui || console.update_logs || console.update_errs) {
        strcpy(outptr, console.errs);
        console.update_errs = FALSE;
        console.update_ui = FALSE;
        console.update_logs = FALSE;
        is_update = TRUE;
    }
    
    if(is_update) {
        system("clear");
        printf(console.output);
    }
}

void ResetLog() {
    console.curr_logs = 0;
    console.logs[0] = 0;
    console.update_logs = TRUE;
}

void WriteOnLog(const char* msg) {
    const unsigned short size = strlen(msg);
    if((console.curr_logs + size) >= 640) {
        const unsigned short shift = console.curr_logs + size - 639;
        console.curr_logs -= shift;
        memmove(console.logs, console.logs+shift, console.curr_logs);
    }
    strcpy(console.logs+console.curr_logs, msg);
    console.curr_logs += size;
    console.logs[console.curr_logs] = 0;
    console.update_logs = TRUE;
}

void ResetErr() {
    console.curr_errs = 0;
    console.errs[0] = 0;
    console.update_errs = TRUE;
}

void WriteOnErr(const char* msg) {
    const unsigned short size = strlen(msg);
    if((console.curr_errs + size) >= 640) {
        const unsigned short shift = console.curr_errs + size - 639;
        console.curr_errs -= shift;
        memmove(console.errs, console.errs+shift, console.curr_errs);
    }
    strcpy(console.errs+console.curr_errs, msg);
    console.curr_errs += size;
    console.errs[console.curr_errs] = 0;
    console.update_errs = TRUE;
}

void ResetUI(BOOL at_start) {
    if(at_start)
        console.curr_ui = 0;
    else 
        console.curr_ui = strlen(console.ui);
    console.ui[console.curr_ui] = 0;
    console.update_ui = TRUE;
}

void WriteOnUi(const char* msg) {
    unsigned short size = strlen(msg);
    if((console.curr_ui + size) >= 640)
        size -= console.curr_ui + size - 639;
    strcpy(console.ui+console.curr_ui, msg);
    console.curr_ui += size;
    console.update_ui = TRUE;
}

void SetUIPosPtr(const unsigned short pos) {
    console.curr_ui = pos;
}

void SetUiChOnCurr(const char character) {
    console.ui[console.curr_ui] = character;
    console.update_ui = TRUE;
}

void TextPadLeft(const char* str, const unsigned short padding, char* strout) {
    const unsigned short size = strlen(str);
    strout[padding] = 0;
    if(size >= padding) {
        memcpy(strout, str, size);
        return;
    }
    const unsigned short space = padding - size;
    memset(strout, ' ', sizeof(char)*space);
    strcpy(strout+space, str);
}

void TextPadRight(const char* str, const unsigned short padding, char* strout) {
    const unsigned short size = strlen(str);
    strout[padding] = 0;
    if(size >= padding) {
        memcpy(strout, str, size);
        return;
    }
    const unsigned short space = padding - size;
    strcpy(strout, str);
    memset(strout+size, ' ', sizeof(char)*space);
}

void TextPadCenter(const char* str, const unsigned short padding, char* strout) {
    const unsigned short size = strlen(str);
    strout[padding] = 0;
    if(size >= padding){
        memcpy(strout, str, size);
        return;
    }
    const unsigned short space = padding - size;
    const unsigned short left_space = (space>>1) + (space&0b1);
    memset(strout, ' ', sizeof(char)*(left_space));
    strcpy(strout+left_space, str);
    memset(strout+left_space+size, ' ', sizeof(char)*(space - left_space));
}

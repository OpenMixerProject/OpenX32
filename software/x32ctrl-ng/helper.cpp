/*
    ____                  __   ______ ___  
   / __ \                 \ \ / /___ \__ \ 
  | |  | |_ __   ___ _ __  \ V /  __) | ) |
  | |  | | '_ \ / _ \ '_ \  > <  |__ < / / 
  | |__| | |_) |  __/ | | |/ . \ ___) / /_ 
   \____/| .__/ \___|_| |_/_/ \_\____/____|
         | |                               
         |_|                               
  
  OpenX32 - The OpenSource Operating System for the Behringer X32 Audio Mixing Console
  Copyright 2025 OpenMixerProject
  https://github.com/OpenMixerProject/OpenX32
  
  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  version 3 as published by the Free Software Foundation.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
*/

#include "helper.h"

Helper::Helper(Config* config){
    this->config = config;
}

void Helper::Log(const char* format, ...)
{
    va_list args;
    va_start(args, format);

    vprintf(format, args);
    fflush(stdout); // immediately write to console!

    va_end(args);
}

void Helper::Debug(const char* format, ...)
{
    if (config->IsDebug()){
        va_list args;
        va_start(args, format);

        vprintf(format, args);
        fflush(stdout); // immediately write to console!

        va_end(args);
    }
}

unsigned int Helper::Checksum(char* str) {
   unsigned int sum = 0;
   while (*str) {
      sum += *str;
      str++;
   } 
   return sum;
}

int Helper::ReadConfig(const char* filename, const char* key, char* value_buffer, size_t buffer_size) {
    FILE *file = NULL;
    char line[64];
    int found = -2;

    // open file
    file = fopen(filename, "r");
    if (file == NULL) {
        // no configuration-file
        strncpy(value_buffer, "Mysterious", buffer_size);
        return -1;
    }

    size_t key_len = strlen(key);
    char search_pattern[key_len + 2];
    snprintf(search_pattern, sizeof(search_pattern), "%s", key);

    // read file line by line
    while (fgets(line, sizeof(line), file) != NULL) {
        if (strncmp(line, search_pattern, strlen(search_pattern)) == 0) {
            char* value_start = line + strlen(search_pattern);

            // remove spaces (if any)
            while (*value_start == ' ' || *value_start == '\t') {
                value_start++;
            }

            // remove unwanted characters from the end
            char* newline_pos = strchr(value_start, '\n');
            if (newline_pos != NULL) {
                *newline_pos = '\0';
            }
            char* cr_pos = strchr(value_start, '\r');
            if (cr_pos != NULL) {
                *cr_pos = '\0';
            }

            // copy value to buffer
            size_t value_len = strlen(value_start);
            if (value_len >= buffer_size) {
                found = -3; // buffer to small
                break;
            }
            strncpy(value_buffer, value_start, buffer_size - 1);
            value_buffer[buffer_size - 1] = '\0';
            found = 0; // success
            break; // value found. End loop
        }
    }
    fclose(file);

	return found;
}
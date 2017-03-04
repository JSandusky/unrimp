# CMake file for compiling the sqlite3 static library under Windows (for ease of use)
#
# Copyright (c) 2012-2016 Sebastien Rombauts (sebastien.rombauts@gmail.com)
#
# Distributed under the MIT License (MIT) (See accompanying file LICENSE.txt
# or copy at http://opensource.org/licenses/MIT) 

# add sources of the "sqlite3" static library

project(sqlite3 C)

add_library(sqlite3 STATIC
 sqlite3.c
 sqlite3.h
)

set_property(TARGET sqlite3 PROPERTY POSITION_INDEPENDENT_CODE TRUE)

#ifndef HPP_MAGIC_H
#define HPP_MAGIC_H

#define HPP_UNIQUE_NAME3(line) hpp_unique_name_ ## line
#define HPP_UNIQUE_NAME2(line) HPP_UNIQUE_NAME ## 3(line)
#define HPP_UNIQUE_NAME HPP_UNIQUE_NAME2(__LINE__)

#endif

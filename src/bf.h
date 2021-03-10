/*
MIT License

Copyright (c) 2021 Josh Schiavone

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <iostream>
#include <vector>
#include <iterator>
#include <sstream>
#include <fstream>
#include <stdio.h>
#include <algorithm>
#include <ctype.h>

#define BF_SUCCESS_CODE_STANDARD 0
#define BF_ERROR_CODE_STANDARD -1

#define BF_SET_VALUE(s_value, k_value) ((s_value) = (k_value))
#define BF_FLUSH(ENDSTREAM) (std::cout << ENDSTREAM)

/*
 *Brainfuck instructions
 *as predefs
*/
#define BF_INCREMENT_VALUE '+'
#define BF_DECREMENT_VALUE '-'
#define BF_CELL_SHIFT_BACK '<'
#define BF_CELL_SHIFT_FRONT '>'
#define BF_READ_STDIN_BUFFER ','
#define BF_SHOW_STDOUT_BUFFER '.'
#define BF_LOOP_BEGIN '['
#define BF_LOOP_END ']'
#define BF_RESET '\0'

/*
 * Brainfuck instructions as
 * opcode
*/

#define OPCODE_MOVE_FRONT 1
#define OPCODE_MOVE_BACK 2
#define OPCODE_INCREMENT 3
#define OPCODE_DECREMENT 4
#define OPCODE_READ_STDIN_BUFFER 5
#define OPCODE_SHOW_STDOUT_BUFFER 6
#define OPCODE_LOOP_BEGIN 7
#define OPCODE_LOOP_END 8
#define OPCODE_RESET 9

typedef char bf_word;
typedef uint32_t bf32_int;
typedef int bfint_t;

namespace bf {

  constexpr bf_word nullchar = '\0';
  const bfint_t max_memory_size = 30000; // tape

  template <typename RType>
  RType bfuck_should_succeed(RType successor, RType successor_value) {
    return (std::move(successor) == successor_value);
  }

  class Environment {
  public:
    std::vector<bf_word> program_memory, program_instruction;
    std::vector<bf_word>::iterator bf_instruction_pointer, bf_memory_pointer;
    explicit Environment();

    void wipe_instruction_buffer(bool exec);
    bool check_program_file_extension(std::string fn);
  };

  template <typename BFBytes, typename BFStream, typename BFType>
  class Stream : public Environment {
  public:
    bfint_t entry_point_line(Environment& env, BFBytes& linestream, bfint_t bracks=0);
    bfint_t entry_point_stream(Environment& env, BFStream& bfstream, bfint_t bracks=0);
  protected:
    void output_bf_stream(BFType printablechar_);
  private:
    std::istringstream istream;
  };

  class Parser : protected Stream<std::string, std::istream, bf_word> {
  public:
    void interpret_bf_instructions(Environment& env);
  private:
    bfint_t temp_value;
  private:
    void get_loop_closer(Environment& env);
    void get_loop_opener(Environment& env);
  };

  class User : protected Parser {
  public:
    bool load_interactive_interpreter(Environment& env);
  private:
    bfint_t temp_bracks = BF_SUCCESS_CODE_STANDARD;
    std::string interpreter_prompt;
  };

  namespace exp {
    #include <exception>
    class BFException : public std::exception {
    public:
      BFException(const char* const message): errmessage_ {message} {};

      const char* what() const noexcept { return errmessage_; }
    private:
      const char* errmessage_;
    };

  } //namespace: exp


}

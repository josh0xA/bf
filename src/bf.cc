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

#include "bf.h"

namespace bf {

  Environment::Environment() : program_memory(bf::max_memory_size) {
    wipe_instruction_buffer(true);
    bf_memory_pointer = program_memory.begin();
  }

  void Environment::wipe_instruction_buffer(bool exec) {
    program_instruction.clear();
    bf_instruction_pointer = program_instruction.begin();
  }

  bool Environment::check_program_file_extension(std::string fn) {
    if (fn.substr(fn.find_last_of(".") + 1) == "bf") {
      return true;
    } else { return false; }
  }

  template <typename BFBytes, typename BFStream, typename BFType>
  bfint_t Stream<BFBytes, BFStream, BFType>::entry_point_line(Environment& env, BFBytes& linestream, bfint_t bracks) {
    istream = std::istringstream(linestream);
    return entry_point_stream(env, istream, bracks);
  }

  template <typename BFBytes, typename BFStream, typename BFType>
  bfint_t Stream<BFBytes, BFStream, BFType>::entry_point_stream(Environment& env, BFStream& bfstream, bfint_t bracks) {
    bf_word stream_word;
    if (bracks == BF_SUCCESS_CODE_STANDARD) { env.program_instruction.push_back(bf::nullchar); }
    while (bf::bfuck_should_succeed<bool> (true, true)) {
      bfstream >> stream_word;
      if (!bfstream) {
        break;
      }
      switch (stream_word) {
        case BF_CELL_SHIFT_BACK: env.program_instruction.push_back(stream_word); break;
        case BF_CELL_SHIFT_FRONT: env.program_instruction.push_back(stream_word); break;
        case BF_INCREMENT_VALUE: env.program_instruction.push_back(stream_word); break;
        case BF_DECREMENT_VALUE: env.program_instruction.push_back(stream_word); break;
        case BF_READ_STDIN_BUFFER: env.program_instruction.push_back(stream_word); break;
        case BF_SHOW_STDOUT_BUFFER: env.program_instruction.push_back(stream_word); break;
        case BF_LOOP_BEGIN:
          bracks++;
          env.program_instruction.push_back(stream_word);
          break;
        case BF_LOOP_END:
          bracks--;
          env.program_instruction.push_back(stream_word);
          break;
        default: break;

      }

    }
    if (bracks == BF_SUCCESS_CODE_STANDARD) { env.program_instruction.push_back(bf::nullchar); }
    return bracks;
  }

  template <typename BFBytes, typename BFStream, typename BFType>
  void Stream<BFBytes, BFStream, BFType>::output_bf_stream(BFType printablechar_) {
    isprint(printablechar_) ? std::cout << printablechar_
      : std::cout << std::dec << static_cast<bfint_t> (printablechar_);
  }

  void Parser::get_loop_closer(Environment& env) {
    temp_value = 1;
    do {
      env.bf_instruction_pointer++;
      if (*env.bf_instruction_pointer == '[') temp_value++;
      else if (*env.bf_instruction_pointer == ']') temp_value--;
    } while (temp_value != BF_SUCCESS_CODE_STANDARD);
  }


  void Parser::get_loop_opener(Environment& env) {
    temp_value = BF_SUCCESS_CODE_STANDARD;
    do {
      if (*env.bf_instruction_pointer == '[') temp_value++;
      else if (*env.bf_instruction_pointer == ']') temp_value--;
      env.bf_instruction_pointer--;
    } while (temp_value != BF_SUCCESS_CODE_STANDARD);
  }

  void Parser::interpret_bf_instructions(Environment& env) {
    env.bf_instruction_pointer = env.program_instruction.begin();
    while (env.bf_instruction_pointer != env.program_instruction.end()) {
      switch (*env.bf_instruction_pointer) {
        case BF_INCREMENT_VALUE: (*env.bf_memory_pointer)++; env.bf_instruction_pointer++; break;
        case BF_DECREMENT_VALUE: (*env.bf_memory_pointer)--; env.bf_instruction_pointer++; break;
        case BF_CELL_SHIFT_FRONT:
                if (env.bf_memory_pointer != (env.program_memory.end()--)) {
                  env.bf_memory_pointer++;
                }
                env.bf_instruction_pointer++;
                break;
        case BF_CELL_SHIFT_BACK:
                if (env.bf_memory_pointer != env.program_memory.begin()) {
                   env.bf_memory_pointer--;
                }
                env.bf_instruction_pointer++;
                break;
        case BF_SHOW_STDOUT_BUFFER: output_bf_stream(*env.bf_memory_pointer); env.bf_instruction_pointer++; break;
        case BF_READ_STDIN_BUFFER:
                bf_word input_word; std::cin >> input_word;
                (*env.bf_memory_pointer) = input_word;
                env.bf_instruction_pointer++;
                break;
        case BF_LOOP_BEGIN:
                if (!(*env.bf_memory_pointer))
                  get_loop_closer(env);
                env.bf_instruction_pointer++;
                break;
        case BF_LOOP_END:
               get_loop_opener(env); env.bf_instruction_pointer++; break;
        case bf::nullchar: env.bf_instruction_pointer++; break;
      }

    }
  }

  bool User::load_interactive_interpreter(Environment& env) {
    while (bf::bfuck_should_succeed<bool>(true, true)) {
      std::cout << ": ";
      std::getline(std::cin, interpreter_prompt);
      if (!std::cin) {
        throw "(error): broken input stream\n";
        break;
      }
      if (temp_bracks == BF_SUCCESS_CODE_STANDARD) { env.wipe_instruction_buffer(true);  }
      BF_SET_VALUE(temp_bracks, entry_point_line(env, interpreter_prompt, temp_bracks));
      if (temp_bracks == BF_SUCCESS_CODE_STANDARD) {
        interpret_bf_instructions(env);
        output_bf_stream(*env.bf_memory_pointer);
        BF_FLUSH(std::endl);
      } else {
          std::cout << "... ";
      }
    }
    return true;
  }


} // namespace: bf

int main(int argc, char** argv) {

  bf::Environment env;
  bf::Stream<std::string, std::istream, bf_word> bf_stream;
  bf::Parser parser;

  if (argc == BF_SUCCESS_CODE_STANDARD + 1) {
    bf::User usr;
    std::cout << "The Brainfuck Interactive Console Interpreter\n\t\tJosh Schiavone\n";
    usr.load_interactive_interpreter(env);
  } else {
      if (env.check_program_file_extension(argv[1])) {
        std::ifstream stream = std::ifstream(argv[1]);
        if (stream.is_open()) {
          bfint_t bracks = bf_stream.entry_point_stream(env, stream);
          if (bracks == BF_SUCCESS_CODE_STANDARD) {
            parser.interpret_bf_instructions(env);
          } else {
              throw bf::exp::BFException("(error): could not match brackets\n");
          }
        } else {
            throw bf::exp::BFException("(error): .bf source could not be located\n");
        }
      } else {
          throw bf::exp::BFException("(error): source file must contain a valid brainfuck extension\n");
      }
  }

  return 0;
}

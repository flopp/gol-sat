/*******************************************************************************
 * gol-sat
 *
 * Copyright (c) 2015 Florian Pigorsch
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/

#include "commandline.h"
#include <boost/program_options.hpp>
#include <iostream>
#include <stdexcept>

namespace po = boost::program_options;

void usage(char* program, const po::options_description& desc) {
    std::cout << "Usage: " << program << " [OPTIONS]... PATTERN_FILE\n"
              << desc << std::endl;
}

bool parseCommandLine(int argc, char** argv, Options& options) {
    po::options_description desc("Allowed options");
    desc.add_options()("help", "Display this help message")(
        "forward,f", "Perform forward computation (default is backwards)")(
        "grow,g", "Allow for field size growth")(
        "evolutions,e", po::value<int>(),
        "Set number of computed evolution steps (default is 1)");

    po::options_description hidden("Hidden options");
    hidden.add_options()("pattern", po::value<std::string>());

    po::positional_options_description p;
    p.add("pattern", -1);

    po::options_description cmdline_options;
    cmdline_options.add(desc).add(hidden);

    try {
        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv)
                      .options(cmdline_options)
                      .positional(p)
                      .run(),
                  vm);
        po::notify(vm);

        if (vm.count("help")) {
            usage(argv[0], desc);
            return false;
        }

        if (vm.count("evolutions")) {
            options.evolutions = vm["evolutions"].as<int>();
        }
        if (vm.count("forward")) {
            options.backwards = false;
        }
        if (vm.count("grow")) {
            options.grow = true;
        }
        if (vm.count("pattern")) {
            options.pattern = vm["pattern"].as<std::string>();
        }

        if (options.pattern.empty()) {
            throw std::runtime_error("No PATTERN_FILE given");
        }
        if (options.evolutions < 1) {
            throw std::runtime_error(
                "Specified number of evolutions must be >= 1");
        }

        return true;
    } catch (std::exception& e) {
        std::cout << "-- Error: " << e.what() << "\n\n";
        usage(argv[0], desc);
        return false;
    }
}

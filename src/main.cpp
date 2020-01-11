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
#include "field.h"
#include "formula.h"
#include "pattern.h"
#include "satSolver.h"
#include <fstream>
#include <iostream>

int main(int argc, char** argv) {
    Options options;
    if (!parseCommandLine(argc, argv, options)) {
        return 1;
    }

    std::cout << "-- Reading pattern from file: " << options.pattern
              << std::endl;
    Pattern pat;
    std::ifstream f(options.pattern);
    if (!f) {
        std::cout << "-- Error: Cannot open " << options.pattern << std::endl;
        return 1;
    }
    try {
        pat.load(f);
    } catch (std::exception& e) {
        std::cout << "-- Error: " << e.what() << std::endl;
        return 1;
    }

    SatSolver s;

    std::cout << "-- Building formula for " << options.evolutions
              << " evolution steps..." << std::endl;
    std::vector<Field> fields;
    for (int g = 0; g <= options.evolutions; ++g) {
        if (!options.grow) {
            fields.push_back(Field(s, pat.width(), pat.height()));
        } else {
            if (options.backwards) {
                fields.push_back(
                    Field(s, pat.width() + 2 * (options.evolutions - g),
                          pat.height() + 2 * (options.evolutions - g)));
            } else {
                fields.push_back(
                    Field(s, pat.width() + 2 * g, pat.height() + 2 * g));
            }
        }
        if (g > 0) {
            transition(s, fields[g - 1], fields[g]);
        }
    }

    if (options.backwards) {
        std::cout << "-- Setting pattern constraint on last generation..."
                  << std::endl;
        patternConstraint(s, fields.back(), pat);
    } else {
        std::cout << "-- Setting pattern constraint on first generation..."
                  << std::endl;
        patternConstraint(s, fields.front(), pat);
    }

    std::cout << "-- Solving formula..." << std::endl;
    if (!s.solve()) {
        std::cout
            << "-- Formula is not solvable. The selected pattern is probably "
               "too restrictive!"
            << std::endl;
        return 1;
    }

    std::cout << std::endl;
    for (int g = 0; g <= options.evolutions; ++g) {
        if (options.backwards) {
            if (g == 0) {
                std::cout << "-- Initial generation:" << std::endl;
            } else if (g == options.evolutions) {
                std::cout << "-- Evolves to final generation (from pattern):"
                          << std::endl;
            } else {
                std::cout << "-- Evolves to:" << std::endl;
            }
        } else {
            if (g == 0) {
                std::cout << "-- Initial generation (from pattern):"
                          << std::endl;
            } else if (g == options.evolutions) {
                std::cout << "-- Evolves to final generation:" << std::endl;
            } else {
                std::cout << "-- Evolves to:" << std::endl;
            }
        }
        fields[g].print(std::cout, s);
        std::cout << std::endl;
    }

    return 0;
}

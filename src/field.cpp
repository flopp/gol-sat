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

#include "field.h"

Field::Field(SatSolver& s, int width, int height)
    : m_width(width), m_height(height), m_literals(width * height) {
    m_false = Minisat::mkLit(s.newVar());
    s.addClause(~m_false);

    for (auto& lit : m_literals) {
        lit = Minisat::mkLit(s.newVar());
    }
}

const Minisat::Lit& Field::operator()(int x, int y) const {
    if (x < 0 || x >= width() || y < 0 || y >= height()) {
        return m_false;
    }
    return m_literals[x + y * m_width];
}

void Field::print(std::ostream& os, const SatSolver& s) const {
    for (int y = 0; y < height(); ++y) {
        for (int x = 0; x < width(); ++x) {
            const Minisat::lbool value = s.modelValue(operator()(x, y));
            if (Minisat::toInt(value) == 1 /* = Minisat::l_False */) {
                os << ".";
            } else if (Minisat::toInt(value) == 0 /* = Minisat::l_True */) {
                os << "X";
            } else {
                os << "?";
            }
        }
        os << std::endl;
    }
}

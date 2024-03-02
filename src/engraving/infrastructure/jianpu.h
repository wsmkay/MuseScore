/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore BVBA and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef MU_ENGRAVING_JIANPU_H
#define MU_ENGRAVING_JIANPU_H

#include <array>
// #include <map>

// #include "types/string.h"
#include "types/symid.h"

namespace mu::engraving {
class Jianpu
{
public:

    static bool init();

    struct Code {
        char32_t jianpuCode = 0;
        char32_t musicSymBlockCode = 0;

        bool isValid() const { return jianpuCode != 0 || musicSymBlockCode != 0; }
    };

    static const Code code(SymId id);
    static char32_t jianpuCode(SymId id);

    // static const std::map<String, StringList>& jianpuRanges();
    // static constexpr const char* JIANPU_ALL_SYMBOLS = "All symbols";

private:

    static bool initGlyphNamesJson();

    static std::array<Code, size_t(SymId::lastSym) + 1> s_symIdCodes;
};
}

#endif // MU_ENGRAVING_JIANPU_H

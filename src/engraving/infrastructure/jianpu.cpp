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
#include "jianpu.h"

#include "io/file.h"
#include "serialization/json.h"

#include "types/symnames.h"

#include "dom/mscore.h"

#include "log.h"

using namespace mu;
using namespace mu::io;
using namespace mu::engraving;

std::array<Jianpu::Code, size_t(SymId::lastSym) + 1> Jianpu::s_symIdCodes { {  } };

const Jianpu::Code Jianpu::code(SymId id)
{
    return s_symIdCodes.at(static_cast<size_t>(id));
}

char32_t Jianpu::jianpuCode(SymId id)
{
    return s_symIdCodes.at(static_cast<size_t>(id)).jianpuCode;
}

bool Jianpu::init()
{
    bool ok = initGlyphNamesJson();

    return ok;
}

bool Jianpu::initGlyphNamesJson()
{
    File file(":/fonts/jianpu/jianpuGlyphnames.json");
    if (!file.open(IODevice::ReadOnly)) {
        LOGE() << "could not open glyph names JSON file.";
        return false;
    }

    std::string error;
    JsonObject glyphNamesJson = JsonDocument::fromJson(file.readAll(), &error).rootObject();
    file.close();

    if (!error.empty()) {
        LOGE() << "JSON parse error in glyph names file: " << error;
        return false;
    }

    IF_ASSERT_FAILED(!glyphNamesJson.empty()) {
        LOGE() << "Could not read glyph names JSON";
        return false;
    }

    for (size_t i = 0; i < s_symIdCodes.size(); ++i) {
        SymId sym = static_cast<SymId>(i);
        if (sym == SymId::noSym || sym == SymId::lastSym) {
            continue;
        }

        std::string name(SymNames::nameForSymId(sym).ascii());
        JsonObject symObj = glyphNamesJson.value(name).toObject();
        if (!symObj.isValid()) {
            continue;
        }

        bool ok;
        char32_t code = symObj.value("codepoint").toString().mid(2).toUInt(&ok, 16);
        if (ok) {
            s_symIdCodes[i].jianpuCode = code;
        } else if (MScore::debugMode) {
            LOGD() << "could not read codepoint for glyph " << name;
        }

        char32_t alernativeCode = symObj.value("alternateCodepoint").toString().mid(2).toUInt(&ok, 16);
        if (ok) {
            s_symIdCodes[i].musicSymBlockCode = alernativeCode;
        } else if (MScore::debugMode) {
            LOGD() << "could not read alternate codepoint for glyph " << name;
        }
    }
    return true;
}

//---------------------------------------------------------
//   jianpuRanges
//    read jianpu ranges.json file
//---------------------------------------------------------

// const std::map<String, StringList>& Jianpu::jianpuRanges()
// {
//     static std::map<String, StringList> ranges;
//     StringList allSymbols;

//     if (ranges.empty()) {
//         File fi(":/fonts/jianpu/ranges.json");
//         if (!fi.open(IODevice::ReadOnly)) {
//             LOGE() << "failed open: " << fi.filePath();
//         }
//         std::string error;
//         JsonObject o = JsonDocument::fromJson(fi.readAll(), &error).rootObject();
//         if (!error.empty()) {
//             LOGE() << "failed parse, err: " << error << ", file: " << fi.filePath();
//         }

//         for (auto s : o.keys()) {
//             JsonObject range = o.value(s).toObject();
//             String desc      = range.value("description").toString();
//             JsonArray glyphs = range.value("glyphs").toArray();
//             if (glyphs.size() > 0) {
//                 StringList glyphNames;
//                 for (size_t i = 0; i < glyphs.size(); ++i) {
//                     glyphNames.append(glyphs.at(i).toString());
//                 }
//                 ranges.insert({ desc, glyphNames });
//                 allSymbols << glyphNames;
//             }
//         }
//         ranges.insert({ String::fromAscii(JIANPU_ALL_SYMBOLS), allSymbols });     // TODO: make translatable as well as ranges.json
//     }
//     return ranges;
// }

/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2024 MuseScore BVBA and others
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

#ifndef MU_ENGRAVING_SOUNDFLAG_H
#define MU_ENGRAVING_SOUNDFLAG_H

#include "engravingitem.h"

#include "draw/types/font.h"

namespace mu::engraving {
class SoundFlag final : public EngravingItem
{
    OBJECT_ALLOCATOR(engraving, SoundFlag)
    DECLARE_CLASSOF(ElementType::SOUND_FLAG)

public:
    explicit SoundFlag(EngravingItem* parent = nullptr);

    SoundFlag* clone() const override;
    bool isEditable() const override;

    void setSelected(bool f) override;

    using PresetCodes = StringList;
    using PlayingTechniqueCodes = StringList;

    const PresetCodes& soundPresets() const;
    void setSoundPresets(const PresetCodes& soundPresets);

    const PlayingTechniqueCodes& playingTechniques() const;
    void setPlayingTechniques(const PlayingTechniqueCodes& techniques);

    void clear();

    bool shouldHide() const;

    void undoChangeSoundFlag(const PresetCodes& presets, const PlayingTechniqueCodes& techniques);

    char16_t iconCode() const;
    draw::Font iconFont() const;
    void setIconFontSize(double size);
    Color iconBackgroundColor() const;

private:
    PresetCodes m_soundPresets;
    PlayingTechniqueCodes m_playingTechniques;

    draw::Font m_iconFont;
};
}

#endif // MU_ENGRAVING_SOUNDFLAG_H

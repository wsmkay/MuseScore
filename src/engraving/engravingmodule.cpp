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
#include "engravingmodule.h"

#include "modularity/ioc.h"
#include "global/allocator.h"

#include "draw/ifontprovider.h"
#include "draw/internal/ifontsdatabase.h"

#include "infrastructure/jianpu.h"
#include "infrastructure/smufl.h"
#include "infrastructure/localfileinfoprovider.h"

#ifndef ENGRAVING_NO_INTERNAL
#include "internal/engravingconfiguration.h"
#include "internal/engravingfontsprovider.h"
#endif

#ifndef ENGRAVING_NO_ACCESSIBILITY
#include "engraving/accessibility/accessibleitem.h"
#endif

#include "engraving/style/defaultstyle.h"

#include "engraving/dom/mscore.h"
#include "engraving/dom/masterscore.h"

#include "rendering/dev/scorerenderer.h"
#include "rendering/stable/scorerenderer.h"
#include "rendering/single/singlerenderer.h"

#include "compat/scoreaccess.h"

#include "log.h"

using namespace mu::engraving;
using namespace mu::modularity;
using namespace mu::draw;

static void engraving_init_qrc()
{
#ifndef NO_QT_SUPPORT
    Q_INIT_RESOURCE(engraving);

    Q_INIT_RESOURCE(fonts_Leland);
    Q_INIT_RESOURCE(fonts_Bravura);
    Q_INIT_RESOURCE(fonts_Campania);
    Q_INIT_RESOURCE(fonts_Edwin);
    Q_INIT_RESOURCE(fonts_FreeSans);
    Q_INIT_RESOURCE(fonts_FreeSerif);
    Q_INIT_RESOURCE(fonts_Gootville);
    Q_INIT_RESOURCE(fonts_Jianpu);
    Q_INIT_RESOURCE(fonts_MScore);
    Q_INIT_RESOURCE(fonts_MuseJazz);
    Q_INIT_RESOURCE(fonts_Smufl);
    Q_INIT_RESOURCE(fonts_Petaluma);
    Q_INIT_RESOURCE(fonts_FinaleMaestro);
    Q_INIT_RESOURCE(fonts_FinaleBroadway);
    Q_INIT_RESOURCE(fonts_Tabulature);
#endif
}

std::string EngravingModule::moduleName() const
{
    return "engraving";
}

void EngravingModule::registerExports()
{
#ifndef ENGRAVING_NO_INTERNAL

    m_configuration = std::make_shared<EngravingConfiguration>();
    m_engravingfonts = std::make_shared<EngravingFontsProvider>();

    ioc()->registerExport<IEngravingConfiguration>(moduleName(), m_configuration);
    ioc()->registerExport<IEngravingFontsProvider>(moduleName(), m_engravingfonts);
#endif

    // internal
/**
Versions:
* dev - current working version, use it for modify
* stable - stable version of layout, don't modify it
*
* see layout/README.h
*/
    ioc()->registerExport<rendering::IScoreRenderer>(moduleName(), new rendering::dev::ScoreRenderer());
    //ioc()->registerExport<rendering::IScoreRenderer>(moduleName(), new rendering::stable::ScoreRenderer());

    ioc()->registerExport<rendering::ISingleRenderer>(moduleName(), new rendering::single::SingleRenderer());
}

void EngravingModule::resolveImports()
{
}

void EngravingModule::registerResources()
{
    engraving_init_qrc();
}

void EngravingModule::registerUiTypes()
{
    MScore::registerUiTypes();
}

void EngravingModule::onInit(const IApplication::RunMode& mode)
{
    if (mode == IApplication::RunMode::AudioPluginRegistration) {
        return;
    }

#ifndef ENGRAVING_NO_INTERNAL
    // Init fonts
#ifdef MUE_COMPILE_USE_QTFONTMETRICS
    {
        // Symbols
        Smufl::init();
        Jianpu::init();

        m_engravingfonts->addFont("Leland",     "Leland",      ":/fonts/leland/Leland.otf");
        m_engravingfonts->addFont("Bravura",    "Bravura",     ":/fonts/bravura/Bravura.otf");
        m_engravingfonts->addFont("Emmentaler", "MScore",      ":/fonts/mscore/mscore.ttf");
        m_engravingfonts->addFont("Gonville",   "Gootville",   ":/fonts/gootville/Gootville.otf");
        m_engravingfonts->addFont("MuseJazz",   "MuseJazz",    ":/fonts/musejazz/MuseJazz.otf");
        m_engravingfonts->addFont("Petaluma",   "Petaluma",    ":/fonts/petaluma/Petaluma.otf");
        m_engravingfonts->addFont("Finale Maestro", "Finale Maestro", ":/fonts/finalemaestro/FinaleMaestro.otf");
        m_engravingfonts->addFont("Finale Broadway", "Finale Broadway", ":/fonts/finalebroadway/FinaleBroadway.otf");
        m_engravingfonts->addFont("Jianpu", "Jianpu", ":/fonts/jianpu/Jianpu.otf");

        if (m_engravingfonts->fontByName("Jianpu")) {
            m_engravingfonts->setFallbackFont("Jianpu");
        } else {
            m_engravingfonts->setFallbackFont("Bravura");
        }

        //! NOTE It may be necessary to draw something with these fonts without requesting the fonts themselves
        //! (for example, simply specifying the family name for painter).
        //! But if they are not loaded, then they are not added to the font database and,
        //! accordingly, they are drawn incorrectly
        m_engravingfonts->loadAllFonts();

        // Text
        const std::vector<io::path_t> textFonts = {
            ":/fonts/musejazz/MuseJazzText.otf",
            ":/fonts/campania/Campania.otf",
            ":/fonts/edwin/Edwin-Roman.otf",
            ":/fonts/edwin/Edwin-Bold.otf",
            ":/fonts/edwin/Edwin-Italic.otf",
            ":/fonts/edwin/Edwin-BdIta.otf",
            ":/fonts/FreeSans.ttf",
            ":/fonts/FreeSerif.ttf",
            ":/fonts/FreeSerifBold.ttf",
            ":/fonts/FreeSerifItalic.ttf",
            ":/fonts/FreeSerifBoldItalic.ttf",
            ":/fonts/mscoreTab.ttf",
            ":/fonts/mscore-BC.ttf",
            ":/fonts/leland/LelandText.otf",
            ":/fonts/leland/Leland.otf",
            ":/fonts/bravura/BravuraText.otf",
            ":/fonts/gootville/GootvilleText.otf",
            ":/fonts/mscore/MScoreText.ttf",
            ":/fonts/petaluma/PetalumaText.otf",
            ":/fonts/petaluma/PetalumaScript.otf",
            ":/fonts/finalemaestro/FinaleMaestroText.otf",
            ":/fonts/finalebroadway/FinaleBroadwayText.otf",
            ":/fonts/jianpu/Jianpu.otf",
        };

        std::shared_ptr<IFontProvider> fontProvider = ioc()->resolve<IFontProvider>("fonts");
        for (const io::path_t& font : textFonts) {
            int loadStatusCode = fontProvider->addTextFont(font);
            if (loadStatusCode == -1) {
                LOGE() << "Fatal error: cannot load internal font " << font;
            }
        }

        fontProvider->insertSubstitution(u"Leland Text",    u"Bravura Text");
        fontProvider->insertSubstitution(u"Bravura Text",   u"Leland Text");
        fontProvider->insertSubstitution(u"MScore Text",    u"Leland Text");
        fontProvider->insertSubstitution(u"Gootville Text", u"Leland Text");
        fontProvider->insertSubstitution(u"MuseJazz Text",  u"Leland Text");
        fontProvider->insertSubstitution(u"Petaluma Text",  u"MuseJazz Text");
        fontProvider->insertSubstitution(u"Finale Maestro Text", u"Leland Text");
        fontProvider->insertSubstitution(u"Finale Broadway Text", u"MuseJazz Text");
        fontProvider->insertSubstitution(u"ScoreFont",      u"Leland Text");// alias for current Musical Text Font
    }
#else // MUE_COMPILE_USE_QTFONTMETRICS
    {
        using namespace mu::draw;

        std::shared_ptr<IFontsDatabase> fdb = ioc()->resolve<IFontsDatabase>(moduleName());

        // Text
        fdb->addFont(FontDataKey("Edwin", false, false), ":/fonts/edwin/Edwin-Roman.otf");
        fdb->addFont(FontDataKey("Edwin", false, true), ":/fonts/edwin/Edwin-Italic.otf");
        fdb->addFont(FontDataKey("Edwin", true, false), ":/fonts/edwin/Edwin-Bold.otf");
        fdb->addFont(FontDataKey("Edwin", true, true), ":/fonts/edwin/Edwin-BdIta.otf");

        // MusicSymbol[Text]
        fdb->addFont(FontDataKey("Bravura"), ":/fonts/bravura/Bravura.otf");
        fdb->addFont(FontDataKey("Bravura Text"), ":/fonts/bravura/BravuraText.otf");
        fdb->addFont(FontDataKey("Leland"), ":/fonts/leland/Leland.otf");
        fdb->addFont(FontDataKey("Leland Text"), ":/fonts/leland/LelandText.otf");

        // Tabulature
        //fdb->addFont(FontDataKey("MuseScoreTab"), ":/fonts/MuseScoreTab.ttf");
        fdb->addFont(FontDataKey("FreeSerif"), ":/fonts/FreeSerif.ttf");

        fdb->setDefaultFont(Font::Type::Unknown, FontDataKey("Edwin"));
        fdb->setDefaultFont(Font::Type::Text, FontDataKey("Edwin"));
        fdb->setDefaultFont(Font::Type::MusicSymbolText, FontDataKey("Bravura Text"));
        fdb->setDefaultFont(Font::Type::MusicSymbol, FontDataKey("Bravura"));
        fdb->setDefaultFont(Font::Type::Tablature, FontDataKey("FreeSerif"));

        // Symbols
        Smufl::init();

        m_engravingfonts->addFont("Leland",     "Leland",      ":/fonts/leland/Leland.otf");
        m_engravingfonts->addFont("Bravura",    "Bravura",     ":/fonts/bravura/Bravura.otf");
        m_engravingfonts->addFont("Emmentaler", "MScore",      ":/fonts/mscore/mscore.ttf");
        m_engravingfonts->addFont("Gonville",   "Gootville",   ":/fonts/gootville/Gootville.otf");
        m_engravingfonts->addFont("MuseJazz",   "MuseJazz",    ":/fonts/musejazz/MuseJazz.otf");
        m_engravingfonts->addFont("Petaluma",   "Petaluma",    ":/fonts/petaluma/Petaluma.otf");
        m_engravingfonts->addFont("Finale Maestro", "Finale Maestro", ":/fonts/finalemaestro/FinaleMaestro.otf");
        m_engravingfonts->addFont("Finale Broadway", "Finale Broadway", ":/fonts/finalebroadway/FinaleBroadway.otf");

        m_engravingfonts->setFallbackFont("Bravura");

        //! NOTE It may be necessary to draw something with these fonts without requesting the fonts themselves
        //! (for example, simply specifying the family name for painter).
        //! But if they are not loaded, then they are not added to the font database and,
        //! accordingly, they are drawn incorrectly
        m_engravingfonts->loadAllFonts();
    }
#endif // MUE_COMPILE_USE_QTFONTMETRICS

    m_configuration->init();

    DefaultStyle::instance()->init(m_configuration->defaultStyleFilePath(),
                                   m_configuration->partStyleFilePath());
#endif // ENGRAVING_NO_INTERNAL

    MScore::init();     // initialize dom

    MScore::setNudgeStep(0.1);     // cursor key (default 0.1)
    MScore::setNudgeStep10(1.0);     // Ctrl + cursor key (default 1.0)
    MScore::setNudgeStep50(0.01);     // Alt  + cursor key (default 0.01)

    // Palette
    {
#ifndef ENGRAVING_NO_ACCESSIBILITY
        AccessibleItem::enabled = false;
#endif
        gpaletteScore = compat::ScoreAccess::createMasterScore();
        gpaletteScore->setFileInfoProvider(std::make_shared<LocalFileInfoProvider>(""));

#ifndef ENGRAVING_NO_ACCESSIBILITY
        AccessibleItem::enabled = true;
#endif

        if (EngravingObject::elementsProvider()) {
            EngravingObject::elementsProvider()->unreg(gpaletteScore);
        }

#ifndef ENGRAVING_NO_INTERNAL
        gpaletteScore->setStyle(DefaultStyle::baseStyle());
        gpaletteScore->style().set(Sid::MusicalTextFont, String(u"Leland Text"));
        IEngravingFontPtr scoreFont = m_engravingfonts->fontByName("Leland");
        gpaletteScore->setEngravingFont(scoreFont);
        gpaletteScore->setNoteHeadWidth(scoreFont->width(SymId::noteheadBlack, gpaletteScore->style().spatium()) / SPATIUM20);
#endif
    }

    //! NOTE And some initialization in the `Notation::init()`
}

void EngravingModule::onDestroy()
{
    delete gpaletteScore;
    gpaletteScore = nullptr;
}

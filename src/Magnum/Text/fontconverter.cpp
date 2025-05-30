/*
    This file is part of Magnum.

    Copyright © 2010, 2011, 2012, 2013, 2014, 2015, 2016, 2017, 2018, 2019,
                2020, 2021, 2022, 2023, 2024, 2025
              Vladimír Vondruš <mosra@centrum.cz>

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Corrade/PluginManager/Manager.h>
#include <Corrade/Utility/Arguments.h>
#include <Corrade/Utility/DebugStl.h>
#include <Corrade/Utility/Path.h>

#include "Magnum/PixelFormat.h"
#include "Magnum/Math/ConfigurationValue.h"
#include "Magnum/Text/AbstractFont.h"
#include "Magnum/Text/AbstractFontConverter.h"
#include "Magnum/Text/DistanceFieldGlyphCacheGL.h"
#include "Magnum/Trade/AbstractImageConverter.h"

#ifdef MAGNUM_TARGET_EGL
#include "Magnum/Platform/WindowlessEglApplication.h"
#elif defined(CORRADE_TARGET_IOS)
#include "Magnum/Platform/WindowlessIosApplication.h"
#elif defined(CORRADE_TARGET_APPLE)
#include "Magnum/Platform/WindowlessCglApplication.h"
#elif defined(CORRADE_TARGET_UNIX)
#include "Magnum/Platform/WindowlessGlxApplication.h"
#elif defined(CORRADE_TARGET_WINDOWS)
#include "Magnum/Platform/WindowlessWglApplication.h"
#else
#error no windowless application available on this platform
#endif

namespace Magnum { namespace {

/**
@page magnum-fontconverter Font conversion utility
@brief Converts font to raster one of given atlas size

@tableofcontents
@m_footernavigation
@m_keywords{magnum-fontconverter fontconverter}

This utility is built if `MAGNUM_WITH_FONTCONVERTER` is enabled when building
Magnum. To use this utility with CMake, you need to request the `fontconverter`
component of the `Magnum` package and use the `Magnum::fontconverter` target
for example in a custom command:

@code{.cmake}
find_package(Magnum REQUIRED fontconverter)

add_custom_command(OUTPUT ... COMMAND Magnum::fontconverter ...)
@endcode

See @ref building, @ref cmake and the @ref Text namespace for more information.

@note This executable is available only if Magnum is compiled with
    @ref MAGNUM_TARGET_GL enabled (done by default). See @ref building-features
    for more information.

@section magnum-fontconverter-example Example usage

Making raster font from TTF file with default set of characters using
@ref Text::FreeTypeFont "FreeTypeFont" font plugin and
@ref Text::MagnumFontConverter "MagnumFontConverter" converter plugin:

@code{.sh}
magnum-fontconverter DejaVuSans.ttf myfont \
    --font FreeTypeFont --converter MagnumFontConverter
@endcode

According to @ref Text::MagnumFontConverter "MagnumFontConverter" plugin
documentation, this will generate files `myfont.conf` and `myfont.tga` in
current directory. You can then load and use them via the
@ref Text::MagnumFont "MagnumFont" plugin.

@section magnum-fontconverter-usage Full usage documentation

@code{.sh}
magnum-fontconverter [--magnum-...] [-h|--help] --font FONT
    --converter CONVERTER [--plugin-dir DIR] [--characters CHARACTERS]
    [--font-size N] [--atlas-size "X Y"] [--output-size "X Y"] [--radius N]
    [--] input output
@endcode

Arguments:

-   `input` --- input font
-   `output` --- output filename prefix
-   `-h`, `--help` --- display help message and exit
-   `--font FONT` --- font plugin
-   `--converter CONVERTER` --- font converter plugin
-   `--plugin-dir DIR` --- override base plugin dir
-   `--characters CHARACTERS` --- characters to include in the output (default:
    `abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789?!:;,.&nbsp;`)
-   `--font-size N` --- input font size (default: `128`)
-   `--atlas-size "X Y"` --- glyph atlas size (default: `"2048 2048"`)
-   `--output-size "X Y"` --- output atlas size. If set to zero size, distance
    field computation will not be used. (default: `"256 256"`)
-   `--radius N` --- distance field computation radius (default: `24`)
-   `--magnum-...` --- engine-specific options (see
    @ref GL-Context-usage-command-line for details)

The resulting font files can be then used as specified in the documentation of
`converter` plugin.
*/

#ifndef DOXYGEN_GENERATING_OUTPUT
class FontConverter: public Platform::WindowlessApplication {
    public:
        explicit FontConverter(const Arguments& arguments);

        int exec() override;

    private:
        Utility::Arguments args;
};

FontConverter::FontConverter(const Arguments& arguments): Platform::WindowlessApplication{arguments, NoCreate} {
    args.addArgument("input").setHelp("input", "input font")
        .addArgument("output").setHelp("output", "output filename prefix")
        .addNamedArgument("font").setHelp("font", "font plugin")
        .addNamedArgument("converter").setHelp("converter", "font converter plugin")
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        .addOption("plugin-dir").setHelp("plugin-dir", "override base plugin dir", "DIR")
        #endif
        .addOption("characters", "abcdefghijklmnopqrstuvwxyz"
                                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                 "0123456789?!:;,. ").setHelp("characters", "characters to include in the output")
        .addOption("font-size", "128").setHelp("font-size", "input font size", "N")
        .addOption("atlas-size", "2048 2048").setHelp("atlas-size", "glyph atlas size", "\"X Y\"")
        .addOption("output-size", "256 256").setHelp("output-size", "output atlas size. If set to zero size, distance field computation will not be used.", "\"X Y\"")
        .addOption("radius", "24").setHelp("radius", "distance field computation radius", "N")
        .addSkippedPrefix("magnum", "engine-specific options")
        .setGlobalHelp("Converts font to raster one of given atlas size.")
        .parse(arguments.argc, arguments.argv);

    createContext();
}

int FontConverter::exec() {
    /* Font converter dependencies */
    PluginManager::Manager<Trade::AbstractImageConverter> imageConverterManager{
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::filename(Trade::AbstractImageConverter::pluginSearchPaths().back()))
        #endif
    };

    /* Load font */
    PluginManager::Manager<Text::AbstractFont> fontManager{
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::filename(Text::AbstractFont::pluginSearchPaths().back()))
        #endif
    };
    Containers::Pointer<Text::AbstractFont> font = fontManager.loadAndInstantiate(args.value("font"));
    if(!font) return 1;

    /* Register the image converter manager for potential dependencies
       (MagnumFontConverter needs TgaImageConverter, for example) */
    PluginManager::Manager<Text::AbstractFontConverter> converterManager{
        #ifndef CORRADE_PLUGINMANAGER_NO_DYNAMIC_PLUGIN_SUPPORT
        args.value("plugin-dir").empty() ? Containers::String{} :
        Utility::Path::join(args.value("plugin-dir"), Utility::Path::filename(Text::AbstractFontConverter::pluginSearchPaths().back()))
        #endif
    };
    converterManager.registerExternalManager(imageConverterManager);

    /* Load font converter */
    Containers::Pointer<Text::AbstractFontConverter> converter = converterManager.loadAndInstantiate(args.value("converter"));
    if(!converter) return 2;

    /* Open font */
    if(!font->openFile(args.value("input"), args.value<Float>("font-size"))) {
        Error() << "Cannot open font" << args.value("input");
        return 3;
    }

    /* Create distance field glyph cache if radius is specified */
    Containers::Pointer<Text::GlyphCacheGL> cache;
    if(!args.value<Vector2i>("output-size").isZero()) {
        Debug() << "Populating distance field glyph cache...";

        cache.emplace<Text::DistanceFieldGlyphCacheGL>(
            args.value<Vector2i>("atlas-size"),
            args.value<Vector2i>("output-size"),
            args.value<UnsignedInt>("radius"));

    /* Otherwise use normal cache */
    } else {
        Debug() << "Zero-size distance field output specified, populating normal glyph cache...";

        cache.emplace<Text::GlyphCacheGL>(PixelFormat::R8Unorm, args.value<Vector2i>("atlas-size"));
    }

    /* Fill the cache */
    font->fillGlyphCache(*cache, args.value("characters"));

    Debug() << "Converting font...";

    /* Convert the font */
    if(!converter->exportFontToFile(*font, *cache, args.value("output"), args.value("characters"))) {
        Error() << "Cannot export font to" << args.value("output");
        std::exit(1);
    }

    Debug() << "Done.";

    return 0;
}
#endif

}}

MAGNUM_WINDOWLESSAPPLICATION_MAIN(Magnum::FontConverter)

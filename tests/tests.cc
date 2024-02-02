#include <filesystem>

#include <gtest/gtest.h>

#include <lunar_magic_wrapper.h>


namespace fs = std::filesystem;

using namespace lunar_magic_wrapper;

class LunarMagicTest : public testing::Test {
private:
    constexpr static auto LM_PATH{ "lunar_magic.exe" };
    constexpr static auto ROM_PATH{ "rom.smc" };
    constexpr static auto OUT_ROM_PATH{ "out_rom.smc" };
    constexpr static auto LEVEL_PATH{ "level.mwl" };
    constexpr static auto SHARED_PALETTE_PATH{ "shared_palette.pal" };
    constexpr static auto PALETTE_PATH{ "palette.pal" };
    constexpr static auto PARTIAL_MAP16_PATH{ "partial.map16" };
    constexpr static auto ALL_MAP16_PATH{ "all.map16" };
    constexpr static auto LEVELS_PATH{ "levels" };

protected:
    std::unique_ptr<LunarMagicWrapper> lm;
    fs::path rom;
    fs::path out_rom;

    fs::path in_level;
    fs::path out_level;

    fs::path in_palette;

    fs::path in_shared_palette;
    fs::path out_shared_palette;

    fs::path in_partial_map16;
    fs::path in_all_map16;
    fs::path out_all_map16;

    fs::path in_levels;
    fs::path out_levels;

    static fs::path MakePath(const fs::path& rel_path) {
        fs::path dir{ __FILE__ };
        dir = dir.parent_path();
        dir /= rel_path;

        return dir;
    }

    static void SetUpROM(const fs::path& original_rom) {
        fs::copy_file(original_rom, ROM_PATH, fs::copy_options::overwrite_existing);
        fs::copy_file(original_rom, OUT_ROM_PATH, fs::copy_options::overwrite_existing);
    }

    void SetUp() override {
        fs::path lm_path{ MakePath(LM_PATH) };

        if (!fs::exists(lm_path)) {
            throw std::runtime_error("'tests/lunar_magic.exe' not found, needs to be supplied manually!");
        }

        if (!fs::exists(MakePath(ROM_PATH))) {
            throw std::runtime_error("'tests/rom.smc' not found, needs to be supplied manually!");
        }

        SetUpROM(MakePath(ROM_PATH));

        lm = std::make_unique<LunarMagicWrapper>(lm_path);
        rom = ROM_PATH;
        out_rom = OUT_ROM_PATH;

        in_level = MakePath(LEVEL_PATH);
        out_level = LEVEL_PATH;

        in_palette = MakePath(PALETTE_PATH);

        in_shared_palette = MakePath(SHARED_PALETTE_PATH);
        out_shared_palette = SHARED_PALETTE_PATH;

        in_partial_map16 = MakePath(PARTIAL_MAP16_PATH);
        in_all_map16 = MakePath(ALL_MAP16_PATH);
        out_all_map16 = ALL_MAP16_PATH;

        in_levels = MakePath(LEVELS_PATH);
        out_levels = LEVELS_PATH;
    }
};

TEST_F(LunarMagicTest, ExportingGFXWorks) {
    EXPECT_NO_THROW(lm->exportGFX(rom));
}

TEST_F(LunarMagicTest, ExportingExGFXWorks) {
    EXPECT_NO_THROW(lm->exportExGFX(rom));
}

TEST_F(LunarMagicTest, ImportingGFXWorks) {
    EXPECT_NO_THROW(lm->importGFX(rom));
}

TEST_F(LunarMagicTest, ImportingExGFXWorks) {
    EXPECT_NO_THROW(lm->importExGFX(rom));
}

TEST_F(LunarMagicTest, ImportingAllGraphicsWorks) {
    EXPECT_NO_THROW(lm->importAllGraphics(rom));
}

TEST_F(LunarMagicTest, ExportingLevelWorks) {
    EXPECT_NO_THROW(lm->exportLevel(rom, out_level, 0x105));
}

TEST_F(LunarMagicTest, ImportingLevelWorks) {
    EXPECT_NO_THROW(lm->importLevel(rom, in_level));
}

TEST_F(LunarMagicTest, ImportingLevelSpecifiedWorks) {
    EXPECT_NO_THROW(lm->importLevel(rom, in_level, 0x106));
}

TEST_F(LunarMagicTest, ImportingPartialMap16Works) {
    EXPECT_NO_THROW(lm->importMap16(rom, in_partial_map16, 0x105));
}

TEST_F(LunarMagicTest, ImportingPartialMap16WithCoordinatesWorks) {
    EXPECT_NO_THROW(lm->importMap16(rom, in_partial_map16, 0x105, std::make_pair(0x1, 0x1)));
}

TEST_F(LunarMagicTest, ImportingCustomPaletteWorks) {
    EXPECT_NO_THROW(lm->importCustomPalette(rom, in_palette, 0x105));
}

TEST_F(LunarMagicTest, ExportingSharedPaletteWorks) {
    EXPECT_NO_THROW(lm->exportSharedPalette(rom, out_shared_palette));
}

TEST_F(LunarMagicTest, ImportingSharedPaletteWorks) {
    EXPECT_NO_THROW(lm->importSharedPalette(rom, in_shared_palette));
}

TEST_F(LunarMagicTest, ExportingAllMap16Works) {
    EXPECT_NO_THROW(lm->exportAllMap16(rom, out_all_map16));
}

TEST_F(LunarMagicTest, ImportingAllMap16Works) {
    EXPECT_NO_THROW(lm->importAllMap16(rom, in_all_map16));
}

TEST_F(LunarMagicTest, ExportingMultipleLevelsWorks) {
    EXPECT_NO_THROW(lm->exportMultipleLevels(rom, out_levels, "level "));
}

TEST_F(LunarMagicTest, ExportingMultipleLevelsWithOptionsWorks) {
    EXPECT_NO_THROW(lm->exportMultipleLevels(rom, out_levels, "level ", LevelExportOption::MODIFIED_ONLY));
}

TEST_F(LunarMagicTest, ImportingMultipleLevelsWorks) {
    EXPECT_NO_THROW(lm->importMultipleLevels(rom, in_levels));
}

TEST_F(LunarMagicTest, ImportingMultipleLevelsWithOptionsWorks) {
    EXPECT_NO_THROW(lm->importMultipleLevels(rom, in_levels, LevelImportOption::CLEAR_SECONDARY_EXITS));
}

TEST_F(LunarMagicTest, ExpandingROMWorks) {
    // TODO maybe check SA-1 sizes here? kinda annoying to set up though
    for (const auto size : { ROMSize::_2MB, ROMSize::_3MB, ROMSize::_4MB }) {
        EXPECT_NO_THROW(lm->expandROM(rom, size));
    }
}

TEST_F(LunarMagicTest, ChangingCompressionWorks) {
    for (const auto compression : { CompressionOption::LC_LZ2_Orig, CompressionOption::LC_LZ2_Speed,
                                    CompressionOption::LC_LZ3 }) {
        EXPECT_NO_THROW(lm->changeCompression(rom, compression));
    }
}

TEST_F(LunarMagicTest, TransferingGlobalEaxanimationWorks) {
    EXPECT_NO_THROW(lm->transferGlobalExanimation(rom, out_rom));
}

TEST_F(LunarMagicTest, TransferingOverworldWorks) {
    EXPECT_NO_THROW(lm->transferOverworld(rom, out_rom));
}

TEST_F(LunarMagicTest, TransferingTitleScreenWorks) {
    EXPECT_NO_THROW(lm->transferTitleScreen(rom, out_rom));
}

TEST_F(LunarMagicTest, TransferingCreditsWorks) {
    EXPECT_NO_THROW(lm->transferCredits(rom, out_rom));
}

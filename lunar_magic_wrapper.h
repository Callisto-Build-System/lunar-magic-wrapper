#ifndef LUNAR_MAGIC_WRAPPER_LUNAR_MAGIC_WRAPPER_H
#define LUNAR_MAGIC_WRAPPER_LUNAR_MAGIC_WRAPPER_H

#include <filesystem>
#include <utility>
#include <optional>

namespace fs = std::filesystem;

namespace lunar_magic_wrapper {

    enum class LevelExportOption {
        NONE = 0,
        MODIFIED_ONLY = 1
    };

    constexpr LevelExportOption operator|(LevelExportOption lhs, LevelExportOption rhs) {
        return static_cast<LevelExportOption>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    enum class LevelImportOption {
        NONE = 0,
        CLEAR_SECONDARY_EXITS = 1
    };

    constexpr LevelImportOption operator|(LevelImportOption lhs, LevelImportOption rhs) {
        return static_cast<LevelImportOption>(static_cast<int>(lhs) | static_cast<int>(rhs));
    }

    enum class ROMSize {
        _2MB,
        _3MB,
        _4MB,
        _6MB_SA1,
        _8MB_SA1
    };

    enum class CompressionOption {
        LC_LZ2_Orig,
        LC_LZ2_Speed,
        LC_LZ3
    };

    class LunarMagicWrapper {
    private:
        const fs::path lunar_magic_path;
        // TODO get lunar magic version and store it

        // TODO ensure_version(low, high) function to ensure function is actually available in that LM version (?)
        // might not be strictly necessary, if it's not available it'll error out anyway

        static std::string romSizeToString(ROMSize rom_size);

    public:
        explicit LunarMagicWrapper(fs::path lunar_magic_path) : lunar_magic_path(std::move(lunar_magic_path)) {}

        int exportGFX(const fs::path& rom_path);

        int exportExGFX(const fs::path& rom_path);

        int importGFX(const fs::path& rom_path);

        int importExGFX(const fs::path& rom_path);

        int importAllGraphics(const fs::path& rom_path);

        int exportLevel(const fs::path& rom_path, const fs::path& mwl_path, uint8_t level_number);

        int importLevel(const fs::path& rom_path, const fs::path& mwl_path, std::optional<uint8_t> level_number);

        int importMap16(const fs::path& rom_path, const fs::path& map16_path, uint8_t level_number,
                        std::optional<std::pair<size_t, size_t>> coordinates);

        int importCustomPalette(const fs::path& rom_path, const fs::path& palette_path, uint8_t level_number);

        int exportSharedPalette(const fs::path& rom_path, const fs::path& shared_palette_path);

        int importSharedPalette(const fs::path& rom_path, const fs::path& shared_palette_path);

        int exportAllMap16(const fs::path& rom_path, const fs::path& all_map16_path);

        int importAllMap16(const fs::path& rom_path, const fs::path& all_map16_path);

        int exportMultLevels(const fs::path& rom_path, const fs::path& mwl_path_and_prefix,
                             std::optional<LevelExportOption> options);

        int importMultLevels(const fs::path& rom_path, const fs::path& directory_path,
                             std::optional<LevelImportOption> options);

        int expandROM(const fs::path& rom_path, ROMSize rom_size);

        int changeCompression(const fs::path& rom_path, CompressionOption);

        int transferLevelGlobalExanimation(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        int transferOverworld(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        int transferTitlescreen(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        int transferCredits(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        int exportTitleMoves(const fs::path& rom_path, const fs::path& title_moves_path);

        int importTitleMoves(const fs::path& rom_path, const fs::path& title_moves_path);
    };

} // lunar_magic_wrapper

#endif //LUNAR_MAGIC_WRAPPER_LUNAR_MAGIC_WRAPPER_H

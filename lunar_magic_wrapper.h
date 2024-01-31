#ifndef LUNAR_MAGIC_WRAPPER_LUNAR_MAGIC_WRAPPER_H
#define LUNAR_MAGIC_WRAPPER_LUNAR_MAGIC_WRAPPER_H

#include <filesystem>
#include <utility>
#include <optional>

#include <fmt/format.h>

namespace fs = std::filesystem;

namespace lunar_magic_wrapper {

    class LunarMagicWrapperException : public std::runtime_error {
        using std::runtime_error::runtime_error;
    };

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
    protected:
        const fs::path lunar_magic_path;
        // TODO get lunar magic version and store it

        // TODO ensure_version(low, high) function to ensure function is actually available in that LM version (?)
        // might not be strictly necessary, if it's not available it'll error out anyway

        static std::string romSizeToString(ROMSize rom_size);

        static std::string compressionOptionToString(CompressionOption compression_option);

        bool inline call(const std::string& call_string) {
            return std::system(fmt::format(R"(""{}" {}")", lunar_magic_path.string(), call_string).c_str()) == 0;
        }

    public:
        explicit LunarMagicWrapper(fs::path lunar_magic_path) : lunar_magic_path(std::move(lunar_magic_path)) {}

        void exportGFX(const fs::path& rom_path);

        void exportExGFX(const fs::path& rom_path);

        void importGFX(const fs::path& rom_path);

        void importExGFX(const fs::path& rom_path);

        void importAllGraphics(const fs::path& rom_path);

        void exportLevel(const fs::path& rom_path, const fs::path& mwl_path, uint16_t level_number);

        void importLevel(const fs::path& rom_path, const fs::path& mwl_path,
                         std::optional<uint16_t> level_number = std::nullopt);

        void importMap16(const fs::path& rom_path, const fs::path& map16_path, uint16_t level_number,
                        std::optional<std::pair<size_t, size_t>> coordinates = std::nullopt);

        void importCustomPalette(const fs::path& rom_path, const fs::path& palette_path, uint16_t level_number);

        void exportSharedPalette(const fs::path& rom_path, const fs::path& shared_palette_path);

        void importSharedPalette(const fs::path& rom_path, const fs::path& shared_palette_path);

        void exportAllMap16(const fs::path& rom_path, const fs::path& all_map16_path);

        void importAllMap16(const fs::path& rom_path, const fs::path& all_map16_path);

        void exportMultipleLevels(const fs::path& rom_path, const fs::path& directory_path,
                                 const std::string& file_name_start,
                                 std::optional<LevelExportOption> options = std::nullopt);

        void importMultipleLevels(const fs::path& rom_path, const fs::path& directory_path,
                                 std::optional<LevelImportOption> options = std::nullopt);

        void expandROM(const fs::path& rom_path, ROMSize rom_size);

        void changeCompression(const fs::path& rom_path, CompressionOption option);

        void transferGlobalExanimation(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        void transferOverworld(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        void transferTitleScreen(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        void transferCredits(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        void exportTitleMoves(const fs::path& rom_path, const fs::path& title_moves_path);

        void importTitleMoves(const fs::path& rom_path, const fs::path& title_moves_path);
    };

} // lunar_magic_wrapper

#endif //LUNAR_MAGIC_WRAPPER_LUNAR_MAGIC_WRAPPER_H

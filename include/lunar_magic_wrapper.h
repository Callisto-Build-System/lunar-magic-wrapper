#ifndef LUNAR_MAGIC_WRAPPER_LUNAR_MAGIC_WRAPPER_H
#define LUNAR_MAGIC_WRAPPER_LUNAR_MAGIC_WRAPPER_H

#include <filesystem>
#include <utility>
#include <optional>
#include <fstream>

#include "fmt/format.h"

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

    struct Result{
    public:
        const std::vector<std::string> out{};
        const std::vector<std::string> err{};

        const bool succeeded{};
        const std::optional<int> error_code{};

        Result(int exit_code, std::vector<std::string> out, std::vector<std::string> err)
        : out(std::move(out)), err(std::move(err)), succeeded(exit_code == 0),
        error_code(exit_code == 0 ? std::nullopt : std::make_optional(exit_code)) {}
    };

    class LunarMagicWrapperException : public std::runtime_error {
    private:
        const Result result;

    public:
        LunarMagicWrapperException(const std::string& message, Result result)
        : std::runtime_error(message), result(std::move(result)) {}

        [[nodiscard]] const Result& getResult() const {
            return result;
        }
    };

    class LunarMagicWrapper {
    protected:
        const fs::path lunar_magic_path;
        // TODO get lunar magic version and store it

        // TODO ensure_version(low, high) function to ensure function is actually available in that LM version (?)
        // might not be strictly necessary, if it's not available it'll error out anyway

        static std::string romSizeToString(ROMSize rom_size);

        static std::string compressionOptionToString(CompressionOption compression_option);

        Result inline call(const std::string& call_string) {
            const auto temp_out{ fs::temp_directory_path() / "lm_wrapper_out.log" };
            const auto temp_err{ fs::temp_directory_path() / "lm_wrapper_err.log" };

            const auto str{ fmt::format(R"(""{}" {} > "{}" 2> "{}"")",
                                        lunar_magic_path.string(), call_string,
                                        temp_out.string(), temp_err.string()) };

            const auto exit_code{ std::system(str.c_str()) };

            std::ifstream out_file(temp_out);
            std::string line;
            std::vector<std::string> out{};
            while (std::getline(out_file, line)) {
                out.push_back(line);
            }
            out_file.close();

            std::ifstream err_file(temp_err);
            std::vector<std::string> err{};
            while (std::getline(err_file, line)) {
                err.push_back(line);
            }
            err_file.close();

            return { exit_code, out, err };
        }

    public:
        explicit LunarMagicWrapper(fs::path lunar_magic_path) : lunar_magic_path(std::move(lunar_magic_path)) {}

        Result exportGFX(const fs::path& rom_path);

        Result exportExGFX(const fs::path& rom_path);

        Result importGFX(const fs::path& rom_path);

        Result importExGFX(const fs::path& rom_path);

        Result importAllGraphics(const fs::path& rom_path);

        Result exportLevel(const fs::path& rom_path, const fs::path& mwl_path, uint16_t level_number);

        Result importLevel(const fs::path& rom_path, const fs::path& mwl_path,
                         std::optional<uint16_t> level_number = std::nullopt);

        Result importMap16(const fs::path& rom_path, const fs::path& map16_path, uint16_t level_number,
                        std::optional<std::pair<size_t, size_t>> coordinates = std::nullopt);

        Result importCustomPalette(const fs::path& rom_path, const fs::path& palette_path, uint16_t level_number);

        Result exportSharedPalette(const fs::path& rom_path, const fs::path& shared_palette_path);

        Result importSharedPalette(const fs::path& rom_path, const fs::path& shared_palette_path);

        Result exportAllMap16(const fs::path& rom_path, const fs::path& all_map16_path);

        Result importAllMap16(const fs::path& rom_path, const fs::path& all_map16_path);

        Result exportMultipleLevels(const fs::path& rom_path, const fs::path& directory_path,
                                 const std::string& file_name_start,
                                 std::optional<LevelExportOption> options = std::nullopt);

        Result importMultipleLevels(const fs::path& rom_path, const fs::path& directory_path,
                                 std::optional<LevelImportOption> options = std::nullopt);

        Result expandROM(const fs::path& rom_path, ROMSize rom_size);

        Result changeCompression(const fs::path& rom_path, CompressionOption option);

        Result transferGlobalExanimation(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        Result transferOverworld(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        Result transferTitleScreen(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        Result transferCredits(const fs::path& source_rom_path, const fs::path& destination_rom_path);

        Result exportTitleMoves(const fs::path& rom_path, const fs::path& title_moves_path);

        Result importTitleMoves(const fs::path& rom_path, const fs::path& title_moves_path);
    };

} // lunar_magic_wrapper

#endif //LUNAR_MAGIC_WRAPPER_LUNAR_MAGIC_WRAPPER_H

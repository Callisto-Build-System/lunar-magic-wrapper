#include "lunar_magic_wrapper.h"

namespace lunar_magic_wrapper {
    std::string LunarMagicWrapper::romSizeToString(ROMSize rom_size) {
        switch(rom_size) {
            case ROMSize::_2MB:
                return "2MB";

            case ROMSize::_3MB:
                return "3MB";

            case ROMSize::_4MB:
                return "4MB";

            case ROMSize::_6MB_SA1:
                return "6MB_SA1";

            case ROMSize::_8MB_SA1:
                return "8MB_SA1";
        }

        throw std::runtime_error("Unknown ROM size passed");
    }

    std::string LunarMagicWrapper::compressionOptionToString(CompressionOption compression_option) {
        switch(compression_option) {
            case CompressionOption::LC_LZ2_Orig:
                return "LC_LZ2_Orig";

            case CompressionOption::LC_LZ2_Speed:
                return "LC_LZ2_Speed";

            case CompressionOption::LC_LZ3:
                return "LC_LZ3";
        }

        throw std::runtime_error("Unknown compression option passed");
    }

    void LunarMagicWrapper::exportGFX(const fs::path &rom_path) {
        auto succeeded{ call(fmt::format(R"(-ExportGFX "{}")", rom_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to export GFX from '{}' using '{}'",
                rom_path.string(),
                lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::exportExGFX(const fs::path &rom_path) {
        auto succeeded{ call(fmt::format(R"(-ExportExGFX "{}")", rom_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to export ExGFX from '{}' using '{}'",
                rom_path.string(),
                lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::importGFX(const fs::path &rom_path) {
        auto succeeded{ call(fmt::format(R"(-ImportExGFX "{}")", rom_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to import GFX into '{}' using '{}'",
                rom_path.string(),
                lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::importExGFX(const fs::path &rom_path) {
        auto succeeded{ call(fmt::format(R"(-ImportExGFX "{}")", rom_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to import ExGFX into '{}' using '{}'",
                rom_path.string(),
                lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::importAllGraphics(const fs::path &rom_path) {
        auto succeeded{ call(fmt::format(R"(-ImportAllGraphics "{}")", rom_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to import all graphics into '{}' using '{}'",
                rom_path.string(),
                lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::exportLevel(const fs::path &rom_path, const fs::path &mwl_path, uint16_t level_number) {
        auto succeeded{ call(fmt::format(R"(-ExportLevel "{}" "{}" {:X})", rom_path.string(), mwl_path.string(), level_number)) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to export level {:03X} from '{}' to '{}' using '{}'",
                level_number,
                rom_path.string(),
                mwl_path.string(),
                lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::importLevel(const fs::path &rom_path, const fs::path &mwl_path,
                                       std::optional<uint16_t> level_number) {
        if (level_number) {
            auto succeeded{ call(fmt::format(R"(-ImportLevel "{}" "{}" {:X})", rom_path.string(),
                                             mwl_path.string(), *level_number)) };

            if (!succeeded) {
                throw LunarMagicWrapperException(fmt::format(
                    "Failed to import level {:03X} from '{}' into '{}' using '{}'",
                    *level_number, mwl_path.string(), rom_path.string(),
                    lunar_magic_path.string()
                ));
            }
        } else {
            auto succeeded{ call(fmt::format(R"(-ImportLevel "{}" "{}")", rom_path.string(), mwl_path.string())) };
            if (!succeeded) {
                throw LunarMagicWrapperException(fmt::format(
                    "Failed to import level from '{}' into '{}' using '{}'",
                    mwl_path.string(), rom_path.string(),
                    lunar_magic_path.string()
                ));
            }
        }
    }

    void LunarMagicWrapper::importMap16(const fs::path &rom_path, const fs::path &map16_path, uint16_t level_number,
                                       std::optional<std::pair<size_t, size_t>> coordinates) {
        // TODO check why this has a level number parameter (it's not documented ...)

        if (coordinates) {
            // TODO check if this is actually how coordinates are handled (it's not documented ...)
            auto succeeded{ call(fmt::format(R"(-ImportMap16 "{}" "{}" {:X} {:X},{:X})", rom_path.string(), map16_path.string(),
                                    level_number, coordinates->first, coordinates->second)) };

            if (!succeeded) {
                throw LunarMagicWrapperException(fmt::format(
                        "Failed to import map16 file '{}' into level {:03X} of '{}' at coordinates X={:X}, Y={:X} "
                        "using '{}'",
                        map16_path.string(), level_number, rom_path.string(), coordinates->first, coordinates->second,
                        lunar_magic_path.string()
                ));
            }
        } else {
            auto succeeded{ call(fmt::format(R"(-ImportMap16 "{}" "{}" {:X})", rom_path.string(), map16_path.string(),
                                    level_number)) };

            if (!succeeded) {
                throw LunarMagicWrapperException(fmt::format(
                        "Failed to import map16 file '{}' into level {:03X} of '{}' using '{}'",
                        map16_path.string(), level_number, rom_path.string(), lunar_magic_path.string()
                ));
            }
        }
    }

    void LunarMagicWrapper::importCustomPalette(const fs::path &rom_path, const fs::path &palette_path,
                                                uint16_t level_number) {
        auto succeeded{ call(fmt::format(R"(-ImportCustomPalette "{}" "{}" {:X})", rom_path.string(), palette_path.string(),
                                level_number)) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to import custom palette '{}' into level {:03X} of '{}' using '{}'",
                palette_path.string(), level_number, rom_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::exportSharedPalette(const fs::path &rom_path, const fs::path &shared_palette_path) {
        auto succeeded{ call(fmt::format(R"(-ExportSharedPalette "{}" {})", rom_path.string(), shared_palette_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to export shared palette to '{}' from '{}' using '{}'",
                shared_palette_path.string(), rom_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::importSharedPalette(const fs::path &rom_path, const fs::path &shared_palette_path) {
        auto succeeded{ call(fmt::format(R"(-ImportSharedPalette "{}" {})", rom_path.string(), shared_palette_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to import shared palette '{}' into '{}' using '{}'",
                shared_palette_path.string(), rom_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::exportAllMap16(const fs::path &rom_path, const fs::path &all_map16_path) {
        auto succeeded{ call(fmt::format(R"(-ExportAllMap16 "{}" {})", rom_path.string(), all_map16_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to export all map16 file to '{}' from '{}' using '{}'",
                all_map16_path.string(), rom_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::importAllMap16(const fs::path &rom_path, const fs::path &all_map16_path) {
        auto succeeded{ call(fmt::format(R"(-ImportAllMap16 "{}" {})", rom_path.string(), all_map16_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to import all map16 file '{}' into '{}' using '{}'",
                all_map16_path.string(), rom_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::exportMultipleLevels(const fs::path &rom_path, const fs::path &directory_path,
                                                 const std::string &file_name_start,
                                                 std::optional<LevelExportOption> options) {
        auto full_path{ directory_path };
        full_path /= file_name_start;

        if (options) {
            const auto options_as_int{ static_cast<size_t>(*options) };
            auto succeeded{ call(fmt::format(R"(-ExportMultLevels "{}" "{}" {:X})",
                                             rom_path.string(), full_path.string(), options_as_int)) };
            if (!succeeded) {
                throw LunarMagicWrapperException(fmt::format(
                    "Failed to export multiple levels with prefix '{}' to '{}' from '{}' with options '{:X}' using '{}'",
                    file_name_start, directory_path.string(), rom_path.string(), options_as_int,
                    lunar_magic_path.string()
                ));
            }
        } else {
            auto succeeded{ call(fmt::format(R"(-ExportMultLevels "{}" "{}")",
                                             rom_path.string(), full_path.string())) };
            if (!succeeded) {
                throw LunarMagicWrapperException(fmt::format(
                        "Failed to export multiple levels with prefix '{}' to '{}' from '{}' using '{}'",
                        file_name_start, directory_path.string(), rom_path.string(),
                        lunar_magic_path.string()
                ));
            }
        }
    }

    void LunarMagicWrapper::importMultipleLevels(const fs::path &rom_path, const fs::path &directory_path,
                                                 std::optional<LevelImportOption> options) {
        if (options) {
            const auto options_as_int{ static_cast<size_t>(*options) };

            auto succeeded{ call(fmt::format(R"(-ImportMultLevels "{}" "{}" {:X})",
                                             rom_path.string(), directory_path.string(), options_as_int)) };

            if (!succeeded) {
                throw LunarMagicWrapperException(fmt::format(
                    "Failed to import multiple levels from '{}' into '{}' with options '{:X}' using '{}'",
                    directory_path.string(), rom_path.string(), options_as_int, lunar_magic_path.string()
                ));
            }
        } else {
            auto succeeded{ call(fmt::format(R"(-ImportMultLevels "{}" "{}")",
                                             rom_path.string(), directory_path.string())) };

            if (!succeeded) {
                throw LunarMagicWrapperException(fmt::format(
                    "Failed to import multiple levels from '{}' into '{}' using '{}'",
                    directory_path.string(), rom_path.string(), lunar_magic_path.string()
                ));
            }
        }
    }

    void LunarMagicWrapper::expandROM(const fs::path &rom_path, ROMSize rom_size) {
        auto succeeded{ call(fmt::format(R"(-ExpandROM "{}" {})", rom_path.string(), romSizeToString(rom_size))) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to expand '{}' to {} using '{}'",
                rom_path.string(), romSizeToString(rom_size), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::changeCompression(const fs::path &rom_path, CompressionOption option) {
        auto succeeded{ call(fmt::format(R"(-ChangeCompression "{}" {})", rom_path.string(),
                                         compressionOptionToString(option))) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to change compression in '{}' to {} using '{}'",
                rom_path.string(), compressionOptionToString(option),
                lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::transferGlobalExanimation(const fs::path &source_rom_path,
                                                      const fs::path &destination_rom_path) {
        auto succeeded{ call(fmt::format(R"(-TransferLevelGlobalExAnim "{}" "{}")",
                                         destination_rom_path.string(), source_rom_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to transfer global ExAnimation from '{}' to '{}' using '{}'",
                source_rom_path.string(), destination_rom_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::transferOverworld(const fs::path &source_rom_path, const fs::path &destination_rom_path) {
        auto succeeded{ call(fmt::format(R"(-TransferOverworld "{}" "{}")",
                                         destination_rom_path.string(), source_rom_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to transfer overworld from '{}' to '{}' using '{}'",
                source_rom_path.string(), destination_rom_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::transferTitleScreen(const fs::path &source_rom_path, const fs::path &destination_rom_path) {
        auto succeeded{ call(fmt::format(R"(-TransferTitleScreen "{}" "{}")",
                                         destination_rom_path.string(), source_rom_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to transfer title screen from '{}' to '{}' using '{}'",
                source_rom_path.string(), destination_rom_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::transferCredits(const fs::path &source_rom_path, const fs::path &destination_rom_path) {
        auto succeeded{ call(fmt::format(R"(-TransferCredits "{}" "{}")",
                                         destination_rom_path.string(), source_rom_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to transfer credits from '{}' to '{}' using '{}'",
                source_rom_path.string(), destination_rom_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::exportTitleMoves(const fs::path &rom_path, const fs::path &title_moves_path) {
        auto succeeded{ call(fmt::format(R"(-ExportTitleMoves "{}" "{}")",
                                         rom_path.string(), title_moves_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to export title moves from '{}' to '{}' using '{}'",
                rom_path.string(), title_moves_path.string(), lunar_magic_path.string()
            ));
        }
    }

    void LunarMagicWrapper::importTitleMoves(const fs::path &rom_path, const fs::path &title_moves_path) {
        auto succeeded{ call(fmt::format(R"(-ImportTitleMoves "{}" "{}")",
                                         rom_path.string(), title_moves_path.string())) };

        if (!succeeded) {
            throw LunarMagicWrapperException(fmt::format(
                "Failed to import title moves from '{}' into '{}' using '{}'",
                rom_path.string(), title_moves_path.string(), lunar_magic_path.string()
            ));
        }
    }
} // lunar_magic_wrapper

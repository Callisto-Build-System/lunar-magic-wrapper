//! This crate provides a lightweight wrapper around [Lunar Magic](http://fusoya.eludevisibility.org/lm/index.html)'s
//! command line functions.
//! It supports all available command line functions as of Lunar Magic 3.40.
//!
//! Note that this crate currently only works on Windows and 
//! relies on `cmd` to invoke Lunar Magic, as this is currently
//! the only way I'm aware of to capture its text output.

use std::{error::Error, fmt, fs::File, io::{BufRead, BufReader}, path::PathBuf, process::Command};

use tempfile::tempdir;
use bitflags::bitflags;

/// A wrapper around Lunar Magic.
/// 
/// Up to date as of Lunar Magic 3.40.
#[derive(Debug)]
pub struct LunarMagicWrapper {
    lunar_magic_path: PathBuf 
}

/// Contains errors raised as a result of an operation using 
/// a [LunarMagicWrapper].
#[derive(Debug)]
pub enum WrapperErr {
    /// Raised when no Lunar Magic is found at the path given to the wrapper.
    LunarMagicMissing{ command: String },

    /// Raised when an operation by Lunar Magic fails.
    Operation{ code: Option<i32>, command: String, output: Vec<String> },

    /// Raised when the underlying command couldn't be executed by the OS.
    FailedToExecute{ command: String },

    /// Raised when no temp file for logging Lunar Magic's output was found.
    NoTempFile{ command: String },

    /// Raised when no temporary directory to keep the Lunar Magic log
    /// file could be created.
    NoTempDir{ command: String },
}

impl fmt::Display for WrapperErr {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let err_msg = match self {
            WrapperErr::LunarMagicMissing { command } => {
                format!("Lunar Magic not found while performing operation '{}'", command)
            },
            WrapperErr::Operation { code, command, output } => {
                if let Some(code) = code {
                    format!(
                        "Lunar Magic returned with error code {} while performing operation '{}' \
                        with the following output:\n\t{}",
                        code, command, output.join("\n\t")
                    )
                } else {
                    format!(
                        "Lunar Magic failed while performing operation '{}' \
                        with the following output:\n\t{}",
                        command, output.join("\n\t")
                    )
                }
            },
            WrapperErr::FailedToExecute { command } => {
                format!(
                    "Failed to execute Lunar Magic while attempting to perform \
                    operation '{}'",
                    command
                )
            },
            WrapperErr::NoTempDir { command } => {
                format!(
                    "Failed to create temporary folder while attempting to perform \
                    operation '{}'",
                    command
                )
            },
            WrapperErr::NoTempFile { command } => {
                format!(
                    "Failed to read temporary log file while attempting to perform \
                    operation '{}'",
                    command
                )
            }
        };

        write!(f, "{}", err_msg)
    }
}

impl Error for WrapperErr {
    fn source(&self) -> Option<&(dyn Error + 'static)> {
        None
    }
}

/// Contains all valid ROM sizes that can be used with
/// [LunarMagicWrapper::expand_rom].
#[derive(Debug)]
pub enum RomSize {
    _2mb,
    _3mb,
    _4mb,
    _6mbSa1,
    _8mbSa1,
}

impl ToString for RomSize {
    fn to_string(&self) -> String {
        String::from(match self {
            RomSize::_2mb => "2MB",
            RomSize::_3mb => "3MB",
            RomSize::_4mb => "4MB",
            RomSize::_6mbSa1 => "6MB_SA1", 
            RomSize::_8mbSa1 => "8MB_SA1", 
        })
    }
}

/// Result of invoking an operation through a [LunarMagicWrapper].
/// 
/// Contains the text output of Lunar Magic if the operation succeeded
/// or a [WrapperErr] otherwise.
pub type ResultL = Result<Vec<String>, WrapperErr>;

/// Contains all valid ROM compression formats that can be used with
/// [LunarMagicWrapper::change_compression].
#[derive(Debug)]
pub enum CompressionFormat {
    LcLz2Orig,
    LcLz2Speed,
    LcLz3,
}

impl ToString for CompressionFormat {
    fn to_string(&self) -> String {
        String::from(match self {
            CompressionFormat::LcLz2Orig => "LC_LZ2_Orig",
            CompressionFormat::LcLz2Speed => "LC_LZ2_Speed",
            CompressionFormat::LcLz3 => "LC_LZ3"
        })
    }
}

bitflags! {
    /// Contains currently available flags when importing
    /// multiple levels into the ROM using [LunarMagicWrapper::import_mult_levels].
    pub struct LevelImportFlag: u32 {
        const None = 0b0000;
        const ClearSecondaryExits = 0b0001;
    }

    /// Contains currently available flags when importing
    /// multiple levels into the ROM using [LunarMagicWrapper::export_mult_levels].
    pub struct LevelExportFlag: u32 {
        const None = 0b0000;
        const OnlyModifiedLevels = 0b0001;
    }
}

impl LunarMagicWrapper {
    /// Returns a new [LunarMagicWrapper].
    /// 
    /// Note that there doesn't necessarily have to be a 
    /// Lunar Magic executable at the passed path at time 
    /// of creation. It only needs to exist once you try 
    /// to actually use the wrapper to perform operations.
    /// 
    /// # Examples
    /// 
    /// ```
    /// use lunar_magic_wrapper::LunarMagicWrapper;
    /// 
    /// let lm_wrapper = LunarMagicWrapper::new("C:/programs/LunarMagic/lunar_magic.exe");
    /// ```
    pub fn new<P: Into<PathBuf>>(path: P) -> Self {
        LunarMagicWrapper { lunar_magic_path: path.into() }
    }

    /// Exports Graphics from the passed ROM and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// 
    /// ```
    /// # use lunar_magic_wrapper::LunarMagicWrapper;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.export_gfx("C:/hacks/my_project/my_hack.smc");
    /// ```
    pub fn export_gfx(&self, rom_path: &str) -> ResultL {
        self.run_command(&format!("-ExportGFX {}", rom_path))
    }

    /// Exports ExGraphics from the passed ROM and returns Lunar Magic's 
    /// text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// 
    /// ```
    /// # use lunar_magic_wrapper::LunarMagicWrapper;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.export_exgfx("C:/hacks/my_project/my_hack.smc");
    /// ```
    pub fn export_exgfx(&self, rom_path: &str) -> ResultL {
        self.run_command(&format!("-ExportExGFX {}", rom_path))
    }

    /// Imports Graphics into the passed ROM and returns Lunar Magic's 
    /// text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// 
    /// ```
    /// # use lunar_magic_wrapper::LunarMagicWrapper;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_gfx("C:/hacks/my_project/my_hack.smc");
    /// ```
    pub fn import_gfx(&self, rom_path: &str) -> ResultL {
        self.run_command(&format!("-ImportGFX {}", rom_path))
    }

    /// Imports ExGraphics into the passed ROM and returns Lunar Magic's 
    /// text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// 
    /// ```
    /// # use lunar_magic_wrapper::LunarMagicWrapper;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_exgfx("C:/hacks/my_project/my_hack.smc");
    /// ```
    pub fn import_exgfx(&self, rom_path: &str) -> ResultL {
        self.run_command(&format!("-ImportExGFX {}", rom_path))
    }

    /// Imports all graphics into the passed ROM and returns Lunar Magic's 
    /// text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// 
    /// ```
    /// # use lunar_magic_wrapper::LunarMagicWrapper;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_all_graphics("C:/hacks/my_project/my_hack.smc");
    /// ```
    pub fn import_all_graphics(&self, rom_path: &str) -> ResultL {
        self.run_command(&format!("-ImportAllGraphics {}", rom_path))
    }

    /// Exports the specified level number as an MWL at the specified location from the passed ROM
    /// and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    ///
    /// # Examples
    /// 
    /// ```
    /// # use lunar_magic_wrapper::LunarMagicWrapper;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.export_level(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/levels/level 105.mwl",
    ///     105
    /// );
    /// ```
    pub fn export_level(&self, rom_path: &str, mwl_path: &str, level_number: u16) -> ResultL
    {
        self.run_command(&format!(
            "-ExportLevel {} {} {}",
            rom_path,
            mwl_path,
            level_number
        ))
    }

    /// Imports the specified MWL file as the (optionally) specified level number
    /// into the passed ROM and returns Lunar Magic's text output or 
    /// a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// 
    /// Without specifying a level number:
    /// ```
    /// # use lunar_magic_wrapper::LunarMagicWrapper;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_level(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/levels/level 105.mwl",
    ///     None
    /// );
    /// ```
    /// 
    /// With specifying a level number:
    /// ```
    /// # use lunar_magic_wrapper::LunarMagicWrapper;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_level(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/levels/level 105.mwl",
    ///     Some(107)
    /// );
    /// ```
    pub fn import_level(&self, rom_path: &str, mwl_path: &str, level_number: Option<u16>) -> ResultL 
    {
        if let Some(level_number) = level_number {
            self.run_command(&format!(
                "-ImportLevel {} {} {}",
                rom_path,
                mwl_path,
                level_number
            ))
        } else {
            self.run_command(&format!(
                "-ImportLevel {} {}",
                rom_path,
                mwl_path
            ))
        }
    }

    /// Imports the specified map16 file into the passed ROM at the (optionally)
    /// specified X, Y map16 location using the tileset of the specified level
    /// and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// 
    /// Without a location:
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_map16(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/tiles.map16",
    ///     105,
    ///     None
    /// );
    /// ```
    /// 
    /// With a location:
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_map16(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/tiles.map16",
    ///     105,
    ///     Some((0x02, 0x40))
    /// );
    /// ```
    pub fn import_map16(
        &self, rom_path: 
        &str, map16_path: 
        &str, level_number: u16, 
        location: Option<(u32, u32)>
    ) -> ResultL 
    {
        if let Some((x, y)) = location {
            return self.run_command(&format!(
                "-ImportMap16 {} {} {} {},{}",
                rom_path,
                map16_path,
                level_number,
                x, y
            ))
        } else {
            self.run_command(&format!(
                "-ImportMap16 {} {} {}",
                rom_path,
                map16_path,
                level_number
            ))
        }
    }

    /// Imports the passed custom palette file into the specified level in the passed 
    /// ROM and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_custom_palette(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/my_palette.pal",
    ///     105
    /// );
    /// ```
    pub fn import_custom_palette(&self, rom_path: &str, palette_path: &str, level_number: u16)
    -> ResultL {
        self.run_command(&format!(
            "-ImportCustomPalette {} {} {}",
            rom_path, palette_path, level_number
        ))
    }

    /// Exports shared palette from the passed ROM to the specified output path
    /// and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.export_shared_palette(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/shared.pal"
    /// );
    /// ```
    pub fn export_shared_palette(&self, rom_path: &str, palette_path: &str) -> ResultL {
        self.run_command(&format!(
            "-ExportSharedPalette {} {}",
            rom_path, palette_path
        ))
    }

    /// Imports passed shared palette into the passed ROM
    /// and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_shared_palette(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/shared.pal"
    /// );
    /// ```
    pub fn import_shared_palette(&self, rom_path: &str, palette_path: &str) -> ResultL {
        self.run_command(&format!(
            "-ImportSharedPalette {} {}",
            rom_path, palette_path
        ))
    }

    /// Exports all map16 data from the passed ROM to the specified output path
    /// and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.export_all_map16(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/all.map16"
    /// );
    /// ```
    pub fn export_all_map16(&self, rom_path: &str, map16_path: &str) -> ResultL {
        self.run_command(&format!(
            "-ExportAllMap16 {} {}",
            rom_path, map16_path
        ))
    }

    /// Imports the passed all map16 file into the passed ROM
    /// and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_all_map16(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/all.map16"
    /// );
    /// ```
    pub fn import_all_map16(&self, rom_path: &str, map16_path: &str) -> ResultL {
        self.run_command(&format!(
            "-ImportAllMap16 {} {}",
            rom_path, map16_path
        ))
    }

    /// Exports multiple levels from the passed ROM to the specified 
    /// location using the (optionally) specified flags and returns 
    /// Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// Flags can be specified using the [LevelExportFlag] enum.
    /// Note that if flags are omitted, Lunar Magic will use its 
    /// default settings for them.
    /// 
    /// # Examples
    /// 
    /// Without flags:
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// // MWL files will be prefixed with "level ", i.e. "level 105.mwl", etc.
    /// // and be contained in `C:/hacks/my_project/resources/levels`
    /// let output = lm_wrapper.export_mult_levels(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/levels/level ",
    ///     None
    /// );
    /// ```
    /// 
    /// With flags:
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// // MWL files will be prefixed with "level ", i.e. "level 105.mwl", etc.
    /// // and be contained in `C:/hacks/my_project/resources/levels`
    /// let output = lm_wrapper.export_mult_levels(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/levels/level ",
    ///     Some(LevelExportFlag::None)
    /// );
    /// ```
    pub fn export_mult_levels(
        &self, 
        rom_path: &str, 
        mwl_path: &str,
        flags: Option<LevelExportFlag>
    ) -> ResultL {
        if let Some(flags) = flags {
            self.run_command(&format!(
                "-ExportMultLevels {} {} {}",
                rom_path, mwl_path, flags.bits()
            ))
        } else {
            self.run_command(&format!(
                "-ExportMultLevels {} {}",
                rom_path, mwl_path
            ))
        }
    }

    /// Imports multiple levels into the passed ROM from the specified 
    /// location using the (optionally) specified flags and returns 
    /// Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// Flags can be specified using the [LevelImportFlag] enum.
    /// Note that if flags are omitted, Lunar Magic will use its 
    /// default settings for them.
    /// 
    /// # Examples
    /// 
    /// Without flags:
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_mult_levels(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/levels",
    ///     None
    /// );
    /// ```
    /// 
    /// With flags:
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_mult_levels(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/levels",
    ///     Some(LevelImportFlag::None)
    /// );
    /// ```
    pub fn import_mult_levels(
        &self, 
        rom_path: &str, 
        level_directory: &str,
        flags: Option<LevelImportFlag>
    ) -> ResultL {
        if let Some(flags) = flags {
            self.run_command(&format!(
                "-ImportMultLevels {} {} {}",
                rom_path, level_directory, flags.bits()
            ))
        } else {
            self.run_command(&format!(
                "-ImportMultLevels {} {}",
                rom_path, level_directory
            ))
        }
    }

    /// Expands the passed ROM to the specified size 
    /// and returns Lunar Magic's text output or an 
    /// [WrapperErr] if something went wrong.
    ///
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.expand_rom(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     RomSize::_4mb
    /// );
    /// ```
    pub fn expand_rom(&self, rom_path: &str, rom_size: RomSize) -> ResultL {
        self.run_command(&format!(
            "-ExpandROM {} {}",
            rom_path,
            rom_size.to_string()
        ))
    }

    /// Changes the compression of the passed ROM to the specified format 
    /// and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    ///
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.change_compression(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     CompressionFormat::LcLz2Speed
    /// );
    /// ```
    pub fn change_compression(&self, rom_path: &str, compression_format: CompressionFormat) -> ResultL
    {
        self.run_command(&format!(
            "-ChangeCompression {} {}",
            rom_path,
            compression_format.to_string()
        ))
    }

    /// Transfers level global ExAnimation data from source ROM to destination ROM and 
    /// return Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.transfer_level_global_exanim(
    ///     "C:/hacks/my_project/destination.smc",
    ///     "C:/hacks/my_project/source.smc"
    /// );
    /// ```
    pub fn transfer_level_global_exanim(&self, dest_rom_path: &str, src_rom_path: &str) -> ResultL {
        self.run_command(&format!(
            "-TransferLevelGlobalExAnim {} {}",
            dest_rom_path, src_rom_path
        ))
    }

    /// Transfers overworld data from source ROM to destination ROM and 
    /// return Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.transfer_overworld(
    ///     "C:/hacks/my_project/destination.smc",
    ///     "C:/hacks/my_project/source.smc"
    /// );
    /// ```
    pub fn transfer_overworld(&self, dest_rom_path: &str, src_rom_path: &str) -> ResultL {
        self.run_command(&format!(
            "-TransferOverworld {} {}",
            dest_rom_path, src_rom_path
        ))
    }

    /// Transfers title screen data from source ROM to destination ROM and 
    /// return Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.transfer_title_screen(
    ///     "C:/hacks/my_project/destination.smc",
    ///     "C:/hacks/my_project/source.smc"
    /// );
    /// ```
    pub fn transfer_title_screen(&self, dest_rom_path: &str, src_rom_path: &str) -> ResultL {
        self.run_command(&format!(
            "-TransferTitleScreen {} {}",
            dest_rom_path, src_rom_path
        ))
    }

    /// Transfers credit data from source ROM to destination ROM and 
    /// return Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.transfer_credits(
    ///     "C:/hacks/my_project/destination.smc",
    ///     "C:/hacks/my_project/source.smc"
    /// );
    /// ```
    pub fn transfer_credits(&self, dest_rom_path: &str, src_rom_path: &str) -> ResultL {
        self.run_command(&format!(
            "-TransferCredits {} {}",
            dest_rom_path, src_rom_path
        ))
    }

    /// Exports title screen movement data from the passed ROM to the specified location
    /// and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.export_title_moves(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/title_screen_movement.zst"
    /// );
    /// ```
    pub fn export_title_moves(&self, rom_path: &str, title_moves_path: &str) -> ResultL {
        self.run_command(&format!(
            "-ExportTitleMoves {} {}",
            rom_path, title_moves_path
        ))
    }

    /// Imports title screen movement data into the passed ROM from the specified location
    /// and returns Lunar Magic's text output or a [WrapperErr] if something went wrong.
    /// 
    /// # Examples
    /// ```
    /// # use lunar_magic_wrapper::*;
    /// # let lm_wrapper = LunarMagicWrapper::new("C:/lunar_magic.exe");
    /// let output = lm_wrapper.import_title_moves(
    ///     "C:/hacks/my_project/my_hack.smc",
    ///     "C:/hacks/my_project/resources/title_screen_movement.zst"
    /// );
    /// ```
    pub fn import_title_moves(&self, rom_path: &str, title_moves_path: &str) -> ResultL {
        self.run_command(&format!(
            "-ImportTitleMoves {} {}",
            rom_path, title_moves_path
        ))
    }

    fn run_command(&self, command_string: &str) -> ResultL {
        if !self.lunar_magic_path.exists() {
            return Err(WrapperErr::LunarMagicMissing { command: format!("{} {}",
                self.lunar_magic_path.to_string_lossy().to_string(),
                command_string
            ) });
        }

        self.run_and_log(command_string)
    }

    fn run_and_log(&self, command_string: &str) -> ResultL {
        let main_command = format!(
            "{} {}",
            self.lunar_magic_path.to_string_lossy().to_string(),
            command_string,
        );

        if let Ok(log_dir) = tempdir() {
            let log_file_path = log_dir.path().join("lunar_magic.log");

            // Unfortunately, Lunar Magic writes directly to the console rather than to 
            // standard output/error and the only way I've found to suppress and get 
            // its output is to pipe it into a file with >, which I think I can only
            // really manage by running via cmd here
            let args = format!(
                "{} > {}", 
                &main_command,
                log_file_path.to_string_lossy().to_string()
            );

            let cmd = Command::new("cmd")
                .args(["/C", &args])
                .output();
            
            if let Ok(result) = cmd {
                if let Ok(log_file) = File::open(log_file_path) {
                    let lines = BufReader::new(log_file).lines();
                    let output = lines.map(|l| l.expect("Failed to read line")).collect();

                    if !result.status.success() {
                        return Err(WrapperErr::Operation { code: result.status.code(), command: main_command, output })
                    } else {
                        return Ok(output);
                    }
                } else {
                    return Err(WrapperErr::NoTempFile { command: main_command })
                }
            } else {
                return Err(WrapperErr::FailedToExecute { command: main_command });
            }
        } else {
            return Err(WrapperErr::NoTempDir { command: main_command });
        }
    }
}

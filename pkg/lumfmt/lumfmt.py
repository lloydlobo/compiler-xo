"""
# `lumfmt` - Formatter for Lumina

Usage:
    $ python3 pkg/lumfmt/lumfmt.py test.lum --verbose --write --spaces 8

Output:
    $ lumfmt test.lum
    Loading file 'test.lum'...
    Loaded successfully
    Total     time: 0.00420 seconds.

# .fmt.lum - Configuration for formatting

align_comment :: true    // Align consecutive end-of-line comments
indent :: 4              // Number of spaces to indent each line
trailing_newline :: true // Add a trailing newline to end of file
"""


import argparse
import os
import sys
from time import perf_counter

CONFIG_FILENAME = ".fmt.lum"


def format_code(input_code, filename: str, flag_spaces: int,
                flag_verbose: bool):
    if flag_verbose:
        print_to_term(text=input_code, filename=filename)
        print("Formatting...")

    buf = ""  # TODO: use array, then join when done
    indent_level = 0
    in_comment_block = False
    indent = ' ' * flag_spaces

    lines = input_code.split("\n")
    for line in lines:
        line = line.strip()

        if in_comment_block:
            if "*/" in line:
                in_comment_block = False
            buf += line + '\n'
        else:
            if "/*" in line:
                in_comment_block = True
            elif "//" in line:
                comment_prefix = ' '
                code_before_comment, comment = line.split("//", 1)
                if len(code_before_comment) == 0:
                    comment_prefix = ''
                buf += indent * indent_level + code_before_comment.strip() + \
                    comment_prefix + "//" + ' ' + comment.strip() + '\n'
            else:
                if '{' in line:
                    buf += indent * indent_level + line + '\n'
                    indent_level += 1
                elif '}' in line:
                    indent_level -= 1
                    buf += indent * indent_level + line + '\n'
                else:
                    buf += indent * indent_level + line + '\n'

    if flag_verbose:
        print("Formatted successfully")
        print_to_term(text=buf, filename=filename)

    return buf


def print_to_term(text, filename: str):
    indent = " " * 2
    print("-" * 80)
    print(f"{indent*2} | File: {filename}")
    print("-" * 80)

    lines = text.split("\n")
    line_count = len(lines)
    line_num_width = len(str(line_count))

    for i, line in enumerate(lines, start=1):
        line_num = str(i).rjust(line_num_width)
        print(f"{indent}{line_num} | {line}")

    print("-" * 80)


def init_config(filename: str, flag_verbose: bool):
    """
    align_comment :: true    // Align consecutive end-of-line comments
    indent :: 4              // Number of spaces to indent each line
    trailing_newline :: true // Add a trailing newline to end of file
    """

    config = {
        "align_comment": True,
        "indent": 4,
        "trailing_newline": True,
    }

    config_file = load_file(filename)
    if flag_verbose:
        print_to_term(config_file, CONFIG_FILENAME)
    if config_file is None:
        return None  # just use config in argparse cli defaults

    lines = config_file.splitlines()  # lines = input_code.split("\n")

    for line in lines:
        line = line.strip()

        if "::" in line:
            ident, ident_value = line.split("::", 1)
            val = config.get(ident.strip())
            if val is None:
                sys.exit(
                    f"error: {ident} is not a valid configuration property of {CONFIG_FILENAME}")
            config[ident.strip()] = ident_value.split("//", 1)[0].strip()

    print("TODO: parse config values. e.g. `'true'` to `True` `'1'` to `1`")

    return config


# .
# ...
# Busy work this point ownwards ....
# ...
# ..


def try_valid_os_path_like(filename: os.PathLike, name: (None | str), ext: str,
                           err_name: str, err_ext: str):
    fname, fext = os.path.splitext(filename)

    if len(err_name.strip()) == 0 or len(err_ext.strip()) == 0:
        sys.exit(f"error(dev): errmsg_ not found while validating {filename}")

    # Expected a valid <input>.lum file. Got `{fname}`
    if len(fname) == 0:
        sys.exit(f"error: {err_name}. Got `{fname}`")
    # Expected a valid .fmt.lum file. Got `{fname}`.
    if name is not None and fname != name:
        sys.exit(f"error: {err_name}. Got `{fname}`. Expected `{name}`")
    # Expected file with extension `.lum`. Got `{fext}`
    if fext != ext:
        sys.exit(f"error: {err_ext}. Got `{fext}`")


# -> str | None
def load_file(filename: str):
    unformatted_code = None

    with open(filename, 'r') as file:
        unformatted_code = file.read()
    return unformatted_code


# -> int | None
def unload_file(filename: str, data: str):
    chars_written = None

    with open(filename, 'w') as file:
        chars_written = file.write(data.strip())
    return chars_written


class Profile:
    def __init__(self):
        self.start_time = None

    def start(self) -> None:
        self.start_time = perf_counter()

    def stop(self) -> (float | None):
        end_time = perf_counter()
        if self.start_time is not None:
            elapsed_time = end_time - self.start_time
            return round(elapsed_time, 6)


def run(workspace_id: int, workspace_label: str):
    profile = Profile()
    prof_elapsed_frontend = None
    prof_elapsed_formatter = None

    # init_argparse

    parser = argparse.ArgumentParser(description="File processing script")
    parser.add_argument("filename", help="Name of the input file")
    parser.add_argument("--spaces", type=int, choices=[2, 4, 8], default=4,
                        help="Set the number of spaces for indentation")
    parser.add_argument("--verbose", action="store_true",
                        help="Enable verbose logging")
    parser.add_argument("--write", action="store_true",
                        help="Write the formatted code to file")
    parser.add_argument("--out", help="Specify the output file to write to")

    args = parser.parse_args()
    filename = args.filename
    flag_verbose = args.verbose
    flag_spaces = args.spaces
    flag_write = args.write
    flag_out_filename = args.out

    # Log out start of program.

    print(f"$ lumfmt {filename}")

    # validate_args

    try_valid_os_path_like(filename=filename, name=None, ext=".lum",
                           err_name="Expected a valid <input>.lum file.",
                           err_ext="Expected file with extension `.lum`.")
    if flag_out_filename is not None:
        try_valid_os_path_like(filename=filename, name=None, ext=".lum",
                               err_name="Expected a valid <output>.lum file.",
                               err_ext="Expected file with extension `.lum`.")

    # read user config file settings

    cfg_path = "/" + CONFIG_FILENAME

    found_cfg = os.path.isabs(cfg_path) and (os.path.dirname(cfg_path) == '/')
    if found_cfg:
        if flag_verbose:
            print(f"Found config file {cfg_path} in root directory.")

        user_config = init_config(CONFIG_FILENAME, flag_verbose)
        if user_config is not None:
            if flag_verbose:
                print(user_config)
            print("TODO: parse config values. e.g. `'true'` to `True` `'1'` to `1`")

    # read input file

    profile.start()
    unformatted_code = None
    if flag_verbose:
        print(f"Loading file '{filename}'...")
    unformatted_code = load_file(filename)
    if len(unformatted_code) == 0 or unformatted_code is None:
        sys.exit(f"error: File {filename} seems to be empty")

    if flag_verbose and unformatted_code is not None:
        print("Loaded successfully")
    prof_elapsed_frontend = profile.stop()

    # format input source

    profile.start()
    formatted_code = None
    formatted_code = format_code(input_code=unformatted_code,
                                 filename=filename, flag_spaces=flag_spaces,
                                 flag_verbose=flag_verbose)
    if formatted_code is None:
        sys.exit(f"error: Failed to format {filename}")
    prof_elapsed_formatter = profile.stop()

    # write formatted source to input file or output file (if provided)

    if flag_write:
        profile.start()
        outfile = filename
        if flag_out_filename is not None:
            outfile = flag_out_filename
        if flag_write and flag_verbose:
            print(f"Writing formatted code to file '{filename}'...")
        chars_written = unload_file(filename=outfile, data=formatted_code)
        if flag_write:
            print(f"{chars_written} bytes written to {filename}")
        perf_elapsed_unload = profile.stop()
        if prof_elapsed_frontend and perf_elapsed_unload:
            prof_elapsed_frontend += round(perf_elapsed_unload, 6)

    # show program flow profile results

    if flag_verbose:
        print("\n")
        print(f"Stats for Workspace {workspace_id} {workspace_label}:")
        print(f"Front-end  time: {prof_elapsed_frontend} seconds.")
        print(f"Formatter  time: {prof_elapsed_formatter} seconds.")


def main():
    profile = Profile()
    profile.start()

    run(workspace_id=2, workspace_label='("Format")')

    prof_elapsed_main = profile.stop()
    print(f"Total      time: {prof_elapsed_main} seconds.")


if __name__ == "__main__":
    main()

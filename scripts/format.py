"""
# Formatter for Lumina

Usage:
    $ python3 scripts/format.py test.lum --verbose --write

Output:
    $ lumina fmt test.lum
    Loading file 'test.lum'...
    Loaded successfully
    Total     time: 0.00420 seconds.
"""

import argparse
import os
import sys
from time import perf_counter


def format_code(input_code, filename: str, flag_spaces: int,
                flag_verbose: bool):
    if flag_spaces not in [2, 4, 8]:
        # FIXME: do not use exceptions
        raise ValueError("Spaces must be 2, 4, or 8")

    if flag_verbose:
        print_table_cat(text=input_code, filename=filename)
        print("Formatting...")

    buf = ""
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
        print_table_cat(text=buf, filename=filename)

    return buf


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


def print_table_cat(text, filename: str):
    indent = " "*2
    print("-"*80)
    print(f"{indent*2} | File: {filename}")
    print("-"*80)

    lines = text.split("\n")
    line_count = len(lines)
    line_num_width = len(str(line_count))

    for i, line in enumerate(lines, start=1):
        line_num = str(i).rjust(line_num_width)
        print(f"{indent}{line_num} | {line}")

    print("-"*80)


def load_file(filename: str) -> str:
    with open(filename, 'r') as file:
        unformatted_code = file.read()
    return unformatted_code


def unload_file(filename: str, data: str) -> int:
    chars_written = None
    with open(filename, 'w') as file:
        chars_written = file.write(data.strip())
        return chars_written


def run(workspace_id: int, workspace_label: str):
    profile = Profile()
    prof_elapsed_frontend = None
    prof_elapsed_formatter = None

    parser = argparse.ArgumentParser(description="File processing script")
    parser.add_argument("filename", help="Name of the input file")
    parser.add_argument("--spaces", type=int, choices=[2, 4, 8], default=4,
                        help="Set the number of spaces for indentation")
    parser.add_argument("--verbose", action="store_true",
                        help="Enable verbose logging")
    parser.add_argument("--write", action="store_true",
                        help="Write the formatted code to file")
    parser.add_argument(
        "--out", help="Specify the output file name to write to")

    args = parser.parse_args()

    filename = args.filename  # TODO: panic if extension is not `.lum`
    print(f"$ lumina fmt {filename}")

    _, fext = os.path.splitext(filename)
    if fext != ".lum":
        sys.exit(f"error: Expected file with extension `.lum`. Found `{fext}`")

    flag_verbose = args.verbose
    flag_spaces = args.spaces
    flag_write = args.write
    flag_out_filename = args.out

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

    profile.start()
    formatted_code = None
    formatted_code = format_code(input_code=unformatted_code,
                                 filename=filename, flag_spaces=flag_spaces,
                                 flag_verbose=flag_verbose)
    prof_elapsed_formatter = profile.stop()

    if flag_write:
        profile.start()
        out_filename = filename
        if flag_out_filename is not None:
            out_filename = "__tmp_fmt_" + filename

        if formatted_code is not None:
            if flag_write and flag_verbose:
                print(f"Writing formatted code to file '{filename}'...")
            chars_written = unload_file(
                filename=out_filename, data=formatted_code)
            if flag_write:
                print(f"{chars_written} bytes written to {filename}")

        perf_elapsed_unload = profile.stop()
        if prof_elapsed_frontend and perf_elapsed_unload:
            prof_elapsed_frontend += perf_elapsed_unload

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

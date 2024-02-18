# hx-comm
This tool allows you to work with comments when editing code in Helix editor.
The main differences from the commenting in Helix (via the keyboard shortcut Ctrl-C):
- the ability to configure the necessary identifiers of comments (for example, when editing a file with a C extension, you can use `//` or `/* .. */`);
- if you lack support for HTML and CSS commenting, so you can do it with this utility.

## How it works
Helix allows you to call an external app and pass selected text as input and insert the output of this app back through the use of the `:pipe <external app>`. And `hx-comm` uses this ability.

This utility also uses 2 files as command line parameters:
1. The first file `comm-lang` contains the extension of the file being edited in Helix. The contents of this file do not need to be changed manually, the bash script `hx-comm-lang` was created for this purpose.
2. The second file `comm-lang-table` contains mappings of comment identifiers for different file extensions. You can edit it yourself by adding the file extensions you need and the start and end identifiers of comments associated with them. Be careful that if you do not use the end comment identifier, there must be a newline character immediately after the start identifier. The `*` must be on the last line and it indicates the use of following comment identifiers for any other extensions.

## Dependencies
The following packages must be installed on your system:
1. C compiler (`clang` or `gcc`). I have only used and tested with Clang.
2. `inotify-tools`
3. `sed`
4. `git`

`inotify-tools` and `sed` needs to run `hx-comm-lang` bash script.

## Installation
1. Clone this repo:
```sh
git clone https://github.com/b52es/hx-comm.git
```
2. Change dir:
```sh
cd hx-comm
```
3. Compile:
```sh
clang hx-comm-edit.c -o hx-comm-edit
```
4. Make it executable:
```sh
chmod +x hx-comm-edit; chmod +x hx-comm-lang
```
5. Set environment variable (for bash):
```sh
echo "export HX_COMM_DIR=${PWD}" >> $HOME/.bashrc
```
if you are using another shell then you should change the above command accordingly.

6. Open and edit your Helix `config.toml` by adding following lines in section `[keys.normal]`:
```
# "#" - used to comment out the entire line
"#" = ["extend_to_line_bounds", ":pipe ${HX_COMM_DIR}/hx-comm-edit ${HX_COMM_DIR}/comm-lang ${HX_COMM_DIR}/comm-lang-table"]
# "$" - used to comment out a single line from the current cursor position to the end of the line
"$" = ["select_mode", "goto_line_end_newline", "normal_mode", ":pipe ${HX_COMM_DIR}/hx-comm-edit ${HX_COMM_DIR}/comm-lang ${HX_COMM_DIR}/comm-lang-table"]
# "^" runs the hx-comm-lang script, after which you need to save the current file via ":w"
"^" = ":sh ${HX_COMM_DIR}/hx-comm-lang"
```
You can select another keyboard shortcuts, see [Helix key remapping docs](https://docs.helix-editor.com/remapping.html).

7. Log out and log in to your Linux machine.

## Usage
1. Open some code file in Helix.
2. Press key `^` and then save the file by typing `:w`. This is how the bash script `hx-comm-lang` will work and write the extension of the opened file to the `comm-lang` file.
3. Select some lines of code using `x` key.
4. Press `#` or `$` to comment them out.
5. Press `#` or `$` again to uncomment them back.

PS. `#` and `$` works the same when multiple lines are selected. Their main difference appears when nothing is selected:
- `#` - selects the entire current line;
- `$` - selects the line from the current cursor position to the end of the current line.

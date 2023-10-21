# Lumina Syntax Support for Visual Studio Code

Lumina Syntax Support for Visual Studio Code is an extension that works seamlessly with the [Hydrogen](https://github.com/orosmatthew/hydrogen-cpp/tree/main) language compiler and [Hydrogen for Visual Studio Code](https://github.com/Zedritsch/hydrogen-vsc/tree/main) syntax support extension.

## Installation

### Using a VSIX File

To install Lumina Syntax Support for Visual Studio Code, follow these steps:

#### Prerequisites

Before you proceed, ensure you have the `vsce` extension manager for Visual Studio Code. If you don't have it installed, you can get it from [github.com/microsoft/vscode-vsce](https://github.com/microsoft/vscode-vsce).

#### Installation Steps

1. **Install the VSCE Tool**:

   If you haven't already installed the `vsce` tool, you can do so globally using npm. Open a command prompt or terminal and run the following command:

   ```shell
   npm install -g vsce
   ```

   This command installs the `vsce` tool globally on your system.

2. **Package Your Extension**:

   - Ensure that your extension's code is ready.
   - Open a command prompt or terminal and navigate to the directory where your extension's code is located.

   Use the `vsce package` command to package your extension into a VSIX file. Run the following command in your extension's directory:

   ```shell
   vsce package
   ```

   This command will create a `.vsix` file in the same directory as your extension's code. This `.vsix` file is the compiled extension package.

3. **Install the Extension**:

   - Open Visual Studio Code.
   - Navigate to the Extensions view by clicking the square icon on the left sidebar or pressing `Ctrl+Shift+X` (or `Cmd+Shift+X` on macOS).

   - Click the three dots menu icon (⋮) at the top of the Extensions view and select "Install from VSIX."

   - Browse to the location of the generated `.vsix` file (usually located in your workspace's root directory) and select it.

   - Visual Studio Code will install the extension. You should see a notification confirming the installation.

4. **Reload or Restart**:

   After the extension is installed, you can use it as you would with any other VS Code extension. Make sure to reload or restart VS Code if necessary.

Now, Lumina Syntax Support for Visual Studio Code is installed and ready for use.
If you make changes to your extension's code, you can re-run the `vsce package` command to update the `.vsix` file for distribution or local installation in Visual Studio Code.
To compile and package the extension as a `.vsix` file, follow the instructions mentioned above.

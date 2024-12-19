#include "../include/help.h"

DataFrame listCommand() {
    DataFrame df;
    df.type = "group";
    df.columns = { "Type", "Command", "Description" };
    string type[] = { "HELP", "APP", "PROCESS", "PC", "FILE SYSTEM", "SCREEN", "KEYBOARD", "WEBCAM" };
    df.data = {
        { type[0], "help get", "Show the command list of this application."},

        { type[1], "app list", "Get the list of running apps on the device."},
        { type[1], "app stop <ID>", "Close the running app with the given ID." },
        { type[1], "app start <path>", "Start the app at the specified path." },

        { type[2], "app list", "Get the list of running apps on the device."},
        { type[2], "app stop <ID>", "Close the running app with the given ID." },
        { type[2], "app start <path>", "Start the app at the specified path." },

        { type[3], "pc shutdown", "Shut down the device." },
        { type[3], "pc restart", "Restart the device." },

        { type[4], "file explore <path>", "Show the directory tree (1-level) of the <path>." },
        { type[4], "file copy <source> <dest>", "Copy the file or directory <source> to <dest>." },
        { type[4], "file get <path>", "Get the file or directory specified by <path>." },
        { type[4], "file deletebin <path>", "Move the file to the Recycle Bin." },
        { type[4], "file deletepmn <path>", "Delete the file permanently." },

        { type[5], "screen get", "Get the screenshot of the device." },

        { type[6], "key log <duration>", "Get the keylogger data of the device in <duration> seconds." },
        { type[6], "key lock <duration>", "Lock the keyboard of the device in <duration> seconds." },

        { type[7], "webcam getimg", "Get the image from the webcam of the device." },
        { type[7], "webcame getvid <duration>", "Get the video from the webcam of the device in <duration> seconds." }
    };
    return df;
}

Response showHelp() {
    DataFrame command_table = listCommand();
    
    string note = R"(
        <div class="note">
            <p><b>Note:</b></p>
            <ul>
                <li>All commands must be written in lowercase.</li>
                <li><code>&lt;duration&gt;</code> must be a positive integer.</li>
                <li><code>&lt;path&gt;</code> must be enclosed in quotes "" if it contains spaces, and directories should be separated by a slash "/". 
                    For example: <code>&lt;path&gt; = "D:/HCMUS.1/HK3/MMT/Project/Description/example.jpg".</code></li>
            </ul>
        </div>
    )";

    return { note + html_table(command_table), vector<BYTE>{} };
}
# Remote PC Control via Email
This is a project for the Computer Networking course in Semester 3, HCMUS.

## Description
**Remote PC Control via Email** allows an administrator to send commands to a remote PC through email to perform various system operations.  

The application operates by leveraging email as a communication medium between the admin and the target PC (server). Commands are sent to a designated host email, which the client application continuously monitors. When a valid command is detected, the client relays it to the server application via a secure TCP connection. The server processes the command, executes the requested operation, and returns a response to the client. Finally, the client uses email to deliver the results back to the administrator.  

## Features
The application provides the following capabilities for the admin to control the server remotely:

1. **Process and Application Management**
   - List all running processes/applications.
   - Start a new process/application by specifying its path.
   - Stop a running process/application by its ID.

2. **System Control**
   - Shutdown the server machine.
   - Restart the server machine.

3. **File Operations**
   - Browse directories tree on the server.
   - Retrieve files from the server.
   - Copy files from one location to another.
   - Delete files temporarily or permanently.

5. **Keyboard Monitoring and Control**
   - Capture keystrokes over a specified duration (keylogger).
   - Lock the keyboard for a specified duration to prevent input.

6. **Screen and Webcam Capture**
   - Take screenshots of the server screen.
   - Capture images or record videos from the server's webcam.
  
7. **Command Listing**
   - List all available commands that the admin can use, along with their usage instructions.
  
## Installation Guide

To use the application, follow these steps:

1. **Download the Installers**  
   - Visit the [Releases](https://github.com/dwctien/Remote-Control/releases/tag/v1.0.0) page on the project's GitHub repository.  
   - Download the following files:  
     - `RemoteControl-Client.msi` for the client application.  
     - `RemoteControl-Server.msi` for the server application.  

2. **Install the Applications**  
   - Double-click on the `.msi` file you downloaded.  
   - Follow the on-screen instructions to complete the installation process.

3. **Run the Applications**  
   - After installation, shortcuts will be created on your desktop.
   - Double-click the respective shortcut to start the application.
  
> [!NOTE]
> For a deeper understanding of how this project is designed, please refer to the accompanying `report.pdf` file available in the repository.

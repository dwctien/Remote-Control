# Remote PC Control via Email

## Description
Remote PC Control via Email is a client-server application developed as part of the Computer Networking course project. This application enables an admin to remotely control and monitor server activities by leveraging the Gmail API and socket programming. Key functionalities include:

- Admin sends requests via Gmail with specific subjects (e.g., "get screenshot") and server IP addresses.
- The client reads unread emails using Gmail API and forwards the requests to the server.
- The server processes requests and sends results (e.g., PNG files) back to the client via sockets.
- The client returns the results to the admin via Gmail API without requiring re-authentication.

The implementation features:
- OAuth2 authentication for Gmail API.
- Socket programming for client-server communication.
- Handling of various admin commands to perform operations like capturing screenshots.

## Run the Application
You can run the application in two ways:

### 1. Build from Source Code (for Developers)
To build the application from source, follow these steps:

1. Clone the repository:
   ```bash
   git clone https://github.com/yourusername/your-repo-name.git
   cd your-repo-name
   ```

2. Install required dependencies:
   - **Windows**: Ensure you have Visual Studio 2022 installed.
   - **Dependencies**:
     - Gmail API library
     - OpenSSL (for secure sockets)
     - OpenCV (for screenshot functionality)

3. Open the project in Visual Studio 2022.

4. Build the solution:
   - Set the build configuration to `Release`.
   - Select **Build > Build Solution** or press `Ctrl+Shift+B`.

5. Run the server and client executables as described in the installation steps below.

### 2. Install Pre-Built Application (for End Users)
To install and use the pre-built application:

1. Download the `.msi` installer from the [Releases](https://github.com/yourusername/your-repo-name/releases) page.

2. Run the installer and follow the on-screen instructions.

3. After installation:
   - A shortcut will be created on your desktop.
   - Double-click the shortcut to start the application.

4. **Testing:**
   - Send a test email to the client email address configured in the Gmail API.
   - Monitor the console for log outputs and results.

For more details, refer to the documentation provided in the `docs/` folder of the repository.

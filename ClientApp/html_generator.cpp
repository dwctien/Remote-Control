#include "html_generator.h"

string html_msg(const string& msg, bool status, bool bold_all) {
    ostringstream html;
    string _class = (bold_all ? "bold " : "") + string(status ? "ok" : "error");
    html << "<p lang=\"en\" class=\"message " << _class << "\">" << msg;
    if (!status) {
        html << " Please try again later.";
    }
    html << "</p>";
    return html.str();
}

string html_mail(const string& request, const string& content) {
    ostringstream html;

html << R"(
    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8" />
        <style>
            * {
                box-sizing: border-box;
            }

            html {
                font-family: 'Roboto', sans-serif;
            }

            p, td, th, span, ul {
                color: #333;
                font-size: 16px;
            }

            .outer-container {
                background-color: #d4ebf8; /* #e0f7fa */
                border: 2px solid #0a3981; /* #00796b */
                border-radius: 10px;
                padding: 20px;
                width: 740px;
                margin: 50px auto;
            }

            .app__name {
                text-align: center;
                font-size: 28px;
                color: #0a3981; /* #1e9d95 */
                font-weight: bold;
            }

            .inner-container {
                background-color: white;
                border-radius: 10px;
                padding: 20px;
                box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
            }

            .app__greeting,
            .app__desc {
                text-align: center;
            }

            .divider {
                border-bottom: 1px solid #ccc;
                margin: 20px 0;
            }

            .request {
                font-weight: bold;
                word-break: break-all;
            }

            /* CSS for message */
            .message {
                margin: 0;
            }

            .message.bold {
                font-weight: bold;
            }

            .message.ok {
                color: #0a3981; /* #1e9d95 */
            }

            .message.error {
                color: red;
            }

            .ascii {
                font-family: 'Courier New', monospace;
                font-size: 16px;
                margin: 0;
                margin-left: 40px;
                white-space: pre-wrap;
                font-weight: bold;
            }
        </style>
    </head>
    <body>
    <div class="outer-container">
        <p class="app__name">Remote Control by Email Service</p>
        <div class="inner-container">
            <p class="app__greeting">Greeting from <span style="font-weight: bold;">Group X</span> - 23CTT2, University of Science, VNUHCM.</p>
            <p class="app__desc">This is our final project for <span style="font-weight: bold;">Computer Networking</span> Course (CSC10008).</p>
            <div class="divider"></div>

            <p>This mail responses to the request: <span class="request" lang="en">)" << request << R"(</span></p>

            <section>)" << content << R"(</section>
        </div>
    </div>
</body>
</html>)";

    return html.str();
}
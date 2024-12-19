#include "include/html_generator.h"

string html_escape(const string& input) {
    string escaped;
    for (char c : input) {
        switch (c) {
        case '<': escaped += "&lt;"; break;
        case '>': escaped += "&gt;"; break;
        default: escaped += c; break;
        }
    }
    return escaped;
}


string html_table(const DataFrame& df, const string& note, const string& format) {
    ostringstream html;

    // Add a note if provided
    if (!note.empty()) {
        html << "<p><span style=\"font-weight: bold;\">Note: </span>" << note << "</p>";
    }

    size_t n_col = df.columns.size();
    const auto& columns = df.columns;
    const auto& data = df.data;
    const auto& type = df.type;

    // Start table with class
    html << "<table class=\"" << format << "\" style=\"border-collapse: collapse; table-layout: auto;\">";

    // Add table header with styling
    vector<string> column_widths = { "80px", "250px", "120px" };
    html << "<tr class=\"first-row\" style=\"background-color: #4f8aff;\">";
    for (const auto& column : columns) {
        html << "<th style=\"border: 1px solid #ddd; padding: 8px;\">" << column << "</th>";
    }
    html << "</tr>";

    // Add table rows
    if (type == "single") {
        size_t n_row = data.size();
        for (size_t i = 0; i < n_row; ++i) {
            html << "<tr style=\"background-color: "
                << ((i % 2 == 0) ? "#ffffff;" : "#d4ebf8;")
                << " border: 1px solid #ddd;\">";
            for (size_t j = 0; j < n_col; ++j) {
                html << "<td style=\"width: " << column_widths[j] << "; border: 1px solid #ddd; padding: 8px; text-align: center;\">" << data[i][j] << "</td>";
            }
            html << "</tr>";
        }
    }
    else if (type == "group") {
        string previous_type; // Track the previous type value
        size_t rowspan_count = 0; // Count rows for rowspan
        bool alternate_color = false; // Toggle for alternating colors

        for (size_t i = 0; i < data.size(); ++i) {
            const auto& row = data[i];
            const string& current_type = row[0];

            // Check if the type is the same as the previous one
            if (current_type != previous_type) {
                // Start a new group, toggle color
                alternate_color = !alternate_color;

                // Count rows for this type
                rowspan_count = 1;
                for (size_t j = i + 1; j < data.size(); ++j) {
                    if (data[j][0] == current_type) ++rowspan_count;
                    else break;
                }

                // Print the merged cell with rowspan
                html << "<tr style=\"background-color: "
                    << (alternate_color ? "#ffffff;" : "#d4ebf8;")
                    << " border: 1px solid #ddd;\">";
                html << "<td rowspan=\"" << rowspan_count
                    << "\" style=\"font-weight: bold; border: 1px solid #ddd; padding: 8px; text-align: center;\">"
                    << current_type << "</td>";
            }
            else {
                // Start a new row for the same group
                html << "<tr style=\"background-color: "
                    << (alternate_color ? "#ffffff;" : "#d4ebf8;")
                    << " border: 1px solid #ddd;\">";
            }

            // Add other columns
            for (size_t j = 1; j < n_col; ++j) {
                if (j == 1) { // second column
                    html << "<td style=\"border: 1px solid #ddd; padding: 8px; text-align: center;\">" << html_escape(row[j]) << "</td>";
                }
                else if (j == 2) { // third column
                    html << "<td style=\"border: 1px solid #ddd; padding: 8px; text-align: left;\">" << html_escape(row[j]) << "</td>";
                }
            }
            html << "</tr>";

            previous_type = current_type; // Update the previous type
        }
    }

    html << "</table>";
    return html.str();
}

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

string html_tree(const string& path, const vector<string>& sub_dirs) {
    ostringstream html;
    string formatted_path = path.empty() ? "this device" : path;
    //replace(formatted_path.begin(), formatted_path.end(), '\\', '/');

    if (!formatted_path.empty() && formatted_path.back() != '/') {
        formatted_path += '/';
    }

    html << "<p>The directory tree for <span style=\"font-weight: bold;\">" << formatted_path
        << "</span> (1 - level):</p>";

    ostringstream ascii_tree;
    ascii_tree << formatted_path;

    for (size_t i = 0; i < sub_dirs.size(); ++i) {
        string name = sub_dirs[i];
        string full_path = formatted_path + name;

        if (fs::is_directory(fs::path(full_path))) {
            if (name.back() != '/') {
                name += '/';
            }
        }

        string prefix = (i < sub_dirs.size() - 1) ? "&#9500;&#9472;&#9472;" : "&#9492;&#9472;&#9472;";
        ascii_tree << "\n" << prefix << " " << name;
    }

    html << "<p class=\"ascii\">" << ascii_tree.str() << "</p>";

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
            
            /* CSS for tables */
            table {
                margin: 20px auto; /* Căn giữa bảng */
                border-collapse: collapse;
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
#include "html_generator.h"

string html_table(const DataFrame& df, const string& note, const string& format) {
    ostringstream html;

    if (!note.empty()) {
        html << "<p><span style=\"font-weight: bold;\">Note: </span>" << note << "</p>";
    }

    size_t n_col = df.columns.size();
    const auto& columns = df.columns;
    const auto& data = df.data;
    const auto& type = df.type;

    html << "<table class=\"" << format << "\">";

    // Add table header
    html << "<tr class=\"first-row\">";
    for (const auto& column : columns) {
        html << "<th>" << column << "</th>";
    }
    html << "</tr>";

    // Add table rows
    if (type == "single") {
        size_t n_row = data.size();
        for (size_t i = 0; i < n_row; ++i) {
            html << "<tr class=\"" << ((i % 2 == 1) ? "odd-row " : "")
                << ((i == n_row - 1) ? "last-row " : "") << "\">";
            for (size_t j = 0; j < n_col; ++j) {
                html << "<td>" << data[i][j] << "</td>";
            }
            html << "</tr>";
        }
    }
    else if (type == "group") {
        size_t n_group = data.size();
        for (size_t i = 0; i < n_group; ++i) {
            const auto& group_name = data[i][0];
            const auto& group_data = data[i];
            size_t n_row = group_data.size();

            // First row with rowspan
            html << "<tr class=\"" << ((i % 2 == 1) ? "odd-row " : "")
                << ((i == n_group - 1 && n_row == 1) ? "last-row " : "") << "\">";
            html << "<td rowspan=\"" << n_row << "\" style=\"font-weight: bold;\">" << group_name << "</td>";
            for (size_t k = 1; k < n_col; ++k) {
                html << "<td>" << group_data[0][k] << "</td>";
            }
            html << "</tr>";

            // Remaining rows
            for (size_t j = 1; j < n_row; ++j) {
                html << "<tr class=\"" << ((i % 2 == 1) ? "odd-row " : "")
                    << ((i == n_group - 1 && j == n_row - 1) ? "last-row " : "") << "\">";
                for (size_t k = 1; k < n_col; ++k) {
                    html << "<td>" << group_data[j][k] << "</td>";
                }
                html << "</tr>";
            }
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
    replace(formatted_path.begin(), formatted_path.end(), '\\', '/');

    if (!formatted_path.empty() && formatted_path.back() != '/') {
        formatted_path += '/';
    }

    html << "<p>The directory tree for <span style=\"font-weight: bold;\">" << formatted_path
        << "</span> (1 - level):</p>";

    ostringstream ascii_tree;
    ascii_tree << formatted_path;

    for (size_t i = 0; i < sub_dirs.size(); ++i) {
        string name = sub_dirs[i];
        string prefix = (i < sub_dirs.size() - 1) ? "├──" : "└──";
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
            /* Your CSS here */
        </style>
    </head>
    <body>
        <div class='main'>
            <div class="container">
                <p class="app__name">Remote Control with Email Service</p>
                <p class="app__greeting">Greeting from <span style='font-weight: bold;'>Group 8</span> - Honors Program 2020, University of Science, VNUHCM.</p>
                <p class="app__desc">This is our final project for <span style='font-weight: bold;'>Computer Networking</span> course (CSC10008) and thank you for using our application!</p>
                <div class='divider'></div>
            </div>
            <div class="container">
                <p>This mail responses to the request: <span class="request" lang="en">)" << request << R"(</span></p>
            </div>
            <section>)" << content << R"(</section>
        </div>
    </body>
    </html>)";

    return html.str();
}
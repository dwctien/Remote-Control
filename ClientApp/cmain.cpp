#include "mail_service.h"

int main() {
	
	string recipient = "tienduc712@gmail.com";
	string subject = "Test Email";
    string body = R"(
<html>
<head>
    <style>
        body {
            font-family: Arial, sans-serif;
            line-height: 1.5;
            text-align: center; /* Center-align content */
        }
        h1 {
            color: #4CAF50; /* Green color */
        }
        .highlight {
            font-weight: bold; /* Bold text */
            color: #FF5722; /* Orange color */
        }
        .border-box {
            border: 2px solid #000; /* Black border */
            padding: 10px;
            margin: 20px auto;
            display: inline-block;
            text-align: left; /* Align text inside the box */
        }
        table {
            border-collapse: collapse;
            width: 80%;
            margin: 20px auto;
        }
        table, th, td {
            border: 1px solid #000;
        }
        th, td {
            padding: 10px;
            text-align: center;
        }
        th {
            background-color: #4CAF50;
            color: white;
        }
    </style>
</head>
<body>
    <h1>Welcome to Our Newsletter</h1>
    <p>Dear <span class="highlight">Subscriber</span>,</p>
    <div class="border-box">
        <p>This is an <span class="highlight">important announcement</span> with the latest updates:</p>
        <ul>
            <li><span class="highlight">Feature 1:</span> Now available for all users.</li>
            <li><span class="highlight">Feature 2:</span> Improved performance.</li>
            <li><span class="highlight">Feature 3:</span> New exciting tools.</li>
        </ul>
    </div>
    <h2>Monthly Report</h2>
    <table>
        <tr>
            <th>Category</th>
            <th>Status</th>
            <th>Progress</th>
        </tr>
        <tr>
            <td>Feature Development</td>
            <td>Completed</td>
            <td>100%</td>
        </tr>
        <tr>
            <td>Bug Fixes</td>
            <td>In Progress</td>
            <td>80%</td>
        </tr>
        <tr>
            <td>Testing</td>
            <td>Pending</td>
            <td>50%</td>
        </tr>
    </table>
    <p>Thank you for your support!</p>
    <p><i>The Development Team</i></p>
</body>
</html>
)";

    send_email(recipient, subject, body);

	return 0;
}
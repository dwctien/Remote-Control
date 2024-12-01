#include "mail_service.h"

int main() {
	
	string recipient = "tienduc712@gmail.com";
	string subject = "Test Email";
    string body = R"(
<!DOCTYPE html>
<html>
<head>
    <style>
        body {
            font-family: Arial, sans-serif;
            margin: 0;
            padding: 0;
        }
        .container {
            border: 2px solid #FFA500; /* Cam */
            border-radius: 10px;
            padding: 20px;
            width: 80%;
            margin: 0 auto;
            background-color: #FFF3E0;
        }
        .header {
            font-weight: bold;
            font-size: 24px;
            text-align: center;
            margin-bottom: 20px;
            color: #D35400;
        }
        table {
            width: 100%;
            border-collapse: collapse;
        }
        th, td {
            border: 1px solid #FFA500;
            padding: 10px;
            text-align: left;
        }
        th {
            background-color: #FFC107;
            color: #333;
        }
        tr:nth-child(even) {
            background-color: #FFF8E1;
        }
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            Running Applications on Server
        </div>
        <table>
            <tr>
                <th>No.</th>
                <th>Application Name</th>
                <th>Process ID</th>
                <th>Thread Count</th>
            </tr>
            <!-- Example rows, replace with actual data -->
            <tr>
                <td>1</td>
                <td>Application A</td>
                <td>1234</td>
                <td>15</td>
            </tr>
            <tr>
                <td>2</td>
                <td>Application B</td>
                <td>5678</td>
                <td>8</td>
            </tr>
            <!-- Add more rows dynamically -->
        </table>
    </div>
</body>
</html>
)";


    send_email(recipient, subject, body);

	return 0;
}
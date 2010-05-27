This is the unit test for javascript in resource folder.

The test could only run on windows, NOT on phone!

To execute the test, first run this command in command window to start the server:
java.exe -jar JsTestDriver-1.2.1.jar --port 12121  --browser "C:\Program Files\Mozilla Firefox\firefox.exe"

Then run this command to run the tests:
java.exe -jar JsTestDriver-1.2.1.jar --config jsTestDriver.conf --verbose --tests all --captureConsole

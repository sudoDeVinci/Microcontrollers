#include <Arduino.h>


String MIMEStr[] = {
    "image/png",
    "image/jpeg",
    "application/x-www-form-urlencoded"
};

String HeaderStr[] = {
  "POST / HTTP/1.1",
  "Host: ",
  "...",
  "Timestamp: "
};

/**
 * Generate a header for a given HTTPS packet.
 */
String generateHeader(MIMEType type, int bodyLength, IPAddress HOST, String macAddress, String timestamp) {

  String mimeType = MIMEStr[static_cast<int>(type)];

  int end = strlen("\r\n");

  int headerLength = HeaderStr[0].length() + end +
                     HeaderStr[1].length() + HOST.toString().length() + end +
                     HeaderStr[2].length() + mimeType.length() + end + 
                     HeaderStr[3].length() + end + 
                     HeaderStr[4].length() + String(bodyLength).length() + end +
                     HeaderStr[5].length() + macAddress.length() + end +
                     HeaderStr[6].length() + timestamp.length();


  String header;
  header.reserve(headerLength+1);

  header += HeaderStr[0]+"\r\n";
  header += HeaderStr[1] + HOST.toString() + "\r\n";
  header += HeaderStr[2] + mimeType +"\r\n";
  header += HeaderStr[3] + "\r\n";
  header += HeaderStr[4] + String(bodyLength) + "\r\n";
  header += HeaderStr[5] + macAddress + "\r\n";
  header += HeaderStr[6] + timestamp + "\r\n";
  
  return header;
}
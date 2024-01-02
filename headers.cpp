#include <Arduino.h>

enum class MIMEType {
    IMAGE_PNG,
    IMAGE_JPG,
    APP_FORM,
    ...
};

String MIMEStr[] = {
    "image/png",
    "image/jpeg",
    "application/x-www-form-urlencoded",
    ...
};

String HeaderStr[] = {
  "POST / HTTP/1.1",
  "Host: ",
  ...
  "Timestamp: "
};

String generateHeader(MIMEType type, int bodyLength, IPAddress HOST, String macAddress, String timestamp) {

  String mimeType = MIMEStr[static_cast<int>(type)];

  int end = strlen("\r\n");

  int headerLength = HeaderStr[0].length() + end +
                     ...
                     HeaderStr[12].length() + timestamp.length();


  String header;
  header.reserve(headerLength+1);

  header += HeaderStr[0]+"\r\n";
  header += HeaderStr[1] + HOST.toString() + "\r\n";
  ...
  header += HeaderStr[12] + timestamp + "\r\n";
  
  return header;
}
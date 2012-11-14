//ARDUINO 1.0+ ONLY
//ARDUINO 1.0+ ONLY
#include <Ethernet.h>
#include <SPI.h>

  #include <Adafruit_PN532.h>

#define SCK  (2)
#define MOSI (3)
#define SS   (4)
#define MISO (5)

Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);
////////////////////////////////////////////////////////////////////////
//CONFIGURE
////////////////////////////////////////////////////////////////////////
byte server[] = { 192,168,1,100 }; //ip Address of the server you will connect to
//174,123,231,247 }; //ip Address of the server you will connect to
//

//The location to go to on the server
//make sure to keep HTTP/1.0 at the end, this is telling it what type of file it is
//String location = "/TEST_PHP_RESPOND_ARDUINO.PHP?id='1254' HTTP/1.0";
String location = "/nfcconnect/HeaderRequest.php"; //ListaEntradas
//location = "/TEST_PHP_RESPOND_ARDUINO.PHP?id=";//'1254' HTTP/1.0";
///~bildr/examples/ethernet/ HTTP/1.0";

// if need to change the MAC address (Very Rare)
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x71, 0x8C };
////////////////////////////////////////////////////////////////////////

EthernetClient client;

char inString[32]; // string for incoming serial data
int stringPos = 0; // string index counter
boolean startRead = false; // is reading?
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };	// Buffer to store the returned UID
  uint8_t uidLength;	
  
void setup(){
  Ethernet.begin(mac);
  Serial.begin(115200);
  
  Serial.println("Hello! NFC");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();
  
  Serial.println("Waiting for an ISO14443A card");
  
}

void loop(){
  
  //colocadop pelo miro
///    if (client.available()) { // need to see if response has been read into the buffer yet
  //    char c = client.read(); //get next character and print it
  //    Serial.print(c);
    //  passa=0;

 //   }
  //colocado pelo miro 
  
   boolean success;
			// Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  
  if (success) {
    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      //Serial.print(" 0x");
      Serial.print(uid[i], DEC); 
    }
    Serial.println("");
	// Wait 1 second before continuing
	//delay(1000);
  
  
  
  
  String pageValue = connectAndRead(); //connect to the server and read the output

  Serial.println(pageValue); //print out the findings.


  delay(5000); //wait 5 seconds before connecting again
}
}

String connectAndRead(){
  //connect to the server
//String Aenviar location + ;
  Serial.println("connecting...");

  //port 80 is typical of a www page
  if (client.connect(server, 80)) {
    Serial.println("connected");
    client.print("GET ");
    client.print(location);
    client.print("?CardID=");
    for (uint8_t i=0; i < uidLength; i++) 
    {
      //Serial.print(" 0x");
      client.print(uid[i], DEC); 
    }
    client.println(" HTTP/1.1");// HTTP/1.0");
//    client.println("CardID: 9879");
   client.println("Host: arduino1");
  
   
    client.println();

    //Connected - Read the page
    return readPage(); //go and read the output

  }else{
    return "connection failed";
  }
}

String readPage(){
  //read the page, and capture & return everything between '<' and '>'

  stringPos = 0;
  memset( &inString, 0, 32 ); //clear inString memory

  while(true){

    if (client.available()) {
      char c = client.read();

 //Serial.print(c);//posto pelo miro
   

      if (c == '*' ) { //'<' is our begining character
        startRead = true; //Ready to start reading the part 
      }else if(startRead){

        if(c != '|'){ //'>' is our ending character
          inString[stringPos] = c;
          stringPos ++;
        }else{
          //got what we need here! We can disconnect now
          startRead = false;
          client.stop();
          client.flush();
          Serial.println("disconnecting.");
          return inString;        
        }

      }
    }

  }

}

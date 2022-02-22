#include <Adafruit_NeoPixel.h>

#define rowCount 8
#define columnCount 5
#define bytesPerRow 10
#define maxBrightness 0x04

Adafruit_NeoPixel pixels(rowCount * columnCount, 13, NEO_RGB + NEO_KHZ800);


  uint32_t getPixelIndex(int row, int column)
  {
    //return ((rowCount - 1) - row) + (rowCount * column);
    return (row) + (rowCount * column);
  }
  
class Row
{
public:
  uint8_t data[bytesPerRow];
  int length;
  
  void Show(int row)
  {
    uint32_t color = getPriorityColor(data[0]);
    uint32_t index = getPixelIndex(row, 0);
    pixels.setPixelColor(index, color);
    
    color = getDeviceColor(data[1]);
    index = getPixelIndex(row, 1);
    pixels.setPixelColor(index, color);
    
    color = getDeviceColor(data[2]);
    index = getPixelIndex(row, 2);
    pixels.setPixelColor(index, color);
    
    color = getCommandColor(data[3]);
    index = getPixelIndex(row, 3);
    pixels.setPixelColor(index, color);
    
    color = getLengthColor(length);
    index = getPixelIndex(row, 4);
    pixels.setPixelColor(index, color);
    
/*
    uint32_t color = getPriorityColor(data[0]);
    pixels.setPixelColor(start + 0, color);
    color = getDeviceColor(data[1]);
    pixels.setPixelColor(start + 1, color);
    color = getDeviceColor(data[2]);
    pixels.setPixelColor(start + 2, color);
    color = getCommandColor(data[3]);
    pixels.setPixelColor(start + 3, color);
    color = getLengthColor(length);
    pixels.setPixelColor(start + 4, color);
    */
  } 

  
  uint32_t getPriorityColor(uint8_t byte)
  {
    switch(byte)
    {
      // uninitialized - keep this black
      case 0x00: return 0;

      // these are sent when the PCM is idle - keep this black
      case 0xE8: return 0;

      // logging
      case 0x6C: return pixels.Color(maxBrightness, 0x00, 0x00);

      // reflash
      case 0x6D: return pixels.Color(maxBrightness, 0x00, 0x00);

      // these are unknown but are common when the PCM is booting
      case 0x8A: return pixels.Color(0x00, 0x00, maxBrightness);
      case 0x88: return pixels.Color(0x00, maxBrightness, 0x00);
      case 0x48: return pixels.Color(maxBrightness, maxBrightness, 0x00);
      case 0x49: return pixels.Color(maxBrightness, 0x00, maxBrightness);
      case 0xC9: return pixels.Color(0x00, maxBrightness, maxBrightness);
      case 0xCB: return pixels.Color(maxBrightness, maxBrightness, maxBrightness);
      default: return pixels.Color(0x00, 0x00, 0x00);
    }
  }

  uint32_t getDeviceColor(uint8_t byte)
  {
    switch(byte)
    {
      case 0x00: return 0;
      case 0x10: return pixels.Color(maxBrightness, 0x00, 0x00);
      case 0xF0: return pixels.Color(0x00, 0x00, maxBrightness);
      case 0xFF: return pixels.Color(maxBrightness, maxBrightness, maxBrightness);
      default: return pixels.Color(0x00, maxBrightness / 2, 0x00);
    }
  }

  uint32_t getCommandColor(uint8_t byte)
  {
    switch(byte)
    {
      case 0x00: return 0;

      // Logging
      case 0x6A: return pixels.Color(maxBrightness, 0x00, 0x00);
      case 0x2A: return pixels.Color(0x00, 0x00, maxBrightness);

      // Read requests - responses use 0x36, below
      case 0x35: return pixels.Color(0x00, 0x00, maxBrightness);

      // Flashing
      case 0x36: return pixels.Color(maxBrightness, 0x00, 0x00);
      case 0x76: return pixels.Color(0x00, 0x00, maxBrightness);
      
      default: return pixels.Color(0x00, maxBrightness, 0x00);
    }
  }

  uint32_t getLengthColor(uint8_t length)
  {
    if (length <= 6)
    {
      return pixels.Color(0, 0, 0);
    }
    
    if (length <= 12)
    {
      return pixels.Color(maxBrightness / 4, maxBrightness / 4, maxBrightness / 4);
    }

    if (length <= 20)
    {
      return pixels.Color(maxBrightness / 2, maxBrightness / 2, maxBrightness / 2);
    }

    if (length <= 40)
    {
      return pixels.Color(maxBrightness * 3 / 4, maxBrightness * 3 / 4, maxBrightness * 3 / 4);
    }

    return pixels.Color(maxBrightness, maxBrightness, maxBrightness);
  }
};

class Grid
{
private:
    Row Rows[rowCount];
    int currentRow;
    int previousRow;
    int repeats;

public:
    Grid()
    {
        currentRow = 0;
        previousRow = 0;
        repeats = 0;
    }

    void Clear()
    {
        for (int index = 0; index < (rowCount * columnCount); index++)
        {
            pixels.setPixelColor(index, pixels.Color(0, 0, 0));
        }

        pixels.show();
    }

    void AddRow(byte* data, int length)
    {
        int change = false;

        if (length != Rows[previousRow].length)
            change = true;

        Rows[currentRow].length = length;

        for (int index = 0; index < min(length, bytesPerRow); index++)
        {
            if (data[index] != Rows[previousRow].data[index])
                change = true;

            Rows[currentRow].data[index] = data[index];
        }

        if (change)
        {
            repeats = 0;

            Draw();

            previousRow = currentRow;
            currentRow++;
            currentRow = currentRow % rowCount;
        }
        else
        {
            if (repeats == 0)
            {
                Draw();
            }

            for (int column = 0; column < columnCount; column++)
            {
                int pixelIndex = getPixelIndex(0, column);
                pixels.setPixelColor(pixelIndex, 0, 0, 0);
            }

            pixels.setPixelColor(getPixelIndex(0, repeats % columnCount), 0, 0, maxBrightness);
            repeats++;
        }

        pixels.show();
    }

    void Draw()
    {
        for (int displayRow = 0; displayRow < rowCount; displayRow++)
        {
            int sourceRow = currentRow - displayRow;
            if (sourceRow < 0)
            {
                sourceRow += rowCount;
            }

            Rows[sourceRow].Show(displayRow);
        }
    }
};

Grid Grid;

void setup()
{
  //pixels.setBrightness(120);
  pixels.begin();
  Grid.Clear();

  for(int row = 0; row < rowCount; row++)
  {
    for(int column = 0; column < columnCount; column++)
    {
      int index = getPixelIndex(row, column);
      pixels.setPixelColor(index, pixels.Color(0x00, 0xFF, 0x00));
      pixels.show();
      delay(5);
      pixels.setPixelColor(index, pixels.Color(0, 0, 0));
      pixels.show();
    }
  }

  /*
  // 0, 0 = lower left
  pixels.setPixelColor(getPixelIndex(0,0), pixels.Color(maxBrightness, maxBrightness, maxBrightness));
  // 0, 4 = lower right
  pixels.setPixelColor(getPixelIndex(0,4), pixels.Color(0, 0, maxBrightness, 0));
  // 7, 0 = top left
  pixels.setPixelColor(getPixelIndex(7,0), pixels.Color(0, maxBrightness, 0));
  // 7, 4 = top right
  pixels.setPixelColor(getPixelIndex(7,4), pixels.Color(maxBrightness, 0, 0));
  pixels.show();
  */
  
#define _DEBUG
#define VPW_DEBUG
  //set up initial read command

  CMD_Request_Read[5] = highByte(Read_Length); //Set the Request_Read message to have the Read_Length Specified (1,000 bytes, 4000 bytes
  CMD_Request_Read[6] = lowByte(Read_Length);
  CMD_Request_Read[7] = (Read_From_Address >> 16) & 0xFF; //high byte
  CMD_Request_Read[8] = (Read_From_Address >> 8) & 0xFF; //mid byte
  CMD_Request_Read[9] = (Read_From_Address) & 0xFF; //low byte


  //pinMode(2,INPUT_PULLUP);
  pinMode(SS, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  pinMode(A5, OUTPUT);
  digitalWrite(A5, HIGH);

  writer_init();
  reader_init();
  // enable all interrupts
  //sei();


  //set up serial port for debugging with data to PC screen
  Serial.begin(115200);
  Serial.println(VERSION);


  VPW_del = 4;

  //  Serial.println("press any key to continue");
  //  while(!Serial.available())
  //  {}
  //  Serial.read();

}

void loop()
{
    if (packet_waiting) // if there was a message and it was recieved propperly
    {
        packet_waiting = 0;

        if (packet_size > 15) //Only print the large messages.  Small messages are printed using Print_MSG() other places
        {

            int size = packet_size - reader_End_Cut;
            Serial.println(); //blank line to seperate the .bin reads
            for (int x = reader_Begin_Byte; x < size; x++) //loop to output the message
            {
                byte b;
                if (x < 15)b = packet_data[x]; //completed message for small messages
                else    b = reader_data[x]; //large data moved over to reader data so it's not overwritten by next message.
                // if(x<0||x>=size)continue; //  the first 10 are the command.  Last 3 are the check sum.  To skip use if(x<10||x>=size-3)
                if (b <= 0x0F) Serial.print("0"); //if a single character add a leading zero.
                Serial.print(b, HEX); //print the data to the screen
                Serial.print(" ");
                if ((x + 1 - reader_Begin_Byte) % 16 == 0)Serial.println();
            }
            Serial.println();
            Serial.println();
        }
        else
        {
            memcpy(VPW_data, (const void*)packet_data, 15); //move data into the VPW data for use in other parts of the program
            VPW_len = packet_size;

            ECU_Live = 1; //flag to note ECU is sending messages
            VPW_Message = ""; //clear message

            //Store message into VPW_Message string
            for (int x = 0; x < VPW_len - 1; x++) //(-1) to remove CRC
            {
                if (VPW_data[x] <= 0x0F) VPW_Message += "0";
                VPW_Message += String(VPW_data[x], HEX);
            }
            VPW_Message.toUpperCase();

            //    Print_MSG();
            if (Have_Key < 11) VPW_Decode();
            //  if(have_Seed==0)VPW_Decode(); //use long messages until we get the seed key going
            //  else   VPW_Decode2(); //if we are into the writing sequence then use decode2 (less printing)
            VPW_Message = "0 0 0 0"; //clear so doesn't reprint
        }
    } //end if message recieved
} // End of Main Void



//***************************************************************************

void Print_MSG()
{
    Serial.print("A: ");

    for (int x = 0; x < VPW_len - !Print_CRC; x++) //(-1) to remove CRC
    {
        if (VPW_data[x] <= 0x0F) Serial.print("0"); //if a single character add a leading zero.
        Serial.print(VPW_data[x], HEX);
        Serial.print(" ");
    }
    Serial.println(" ");
}

void Print_MSG_Nln()
{
    Serial.print("B: ");

    int length = VPW_len - !Print_CRC;
    for (int x = 0; x < length; x++) //(-1) to remove CRC
    {
        if (VPW_data[x] <= 0x0F) Serial.print("0"); //if a single character add a leading zero.
        Serial.print(VPW_data[x], HEX);
        Serial.print(" ");
    }

    Grid.AddRow(VPW_data, length);
}

/*
  void send_VPW(byte* msg,byte len)
  {

  digitalWrite (CS_VPW, LOW);
  SPI.transfer (CMD_SEND_VPW_MSG);
  for (int i=0; i<len; i++)
  {
  delayMicroseconds(VPW_DEL);
  SPI.transfer(msg[i]);
  }
  digitalWrite (CS_VPW, HIGH);

  }

*/

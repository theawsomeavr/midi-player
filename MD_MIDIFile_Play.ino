// Test playing a succession of MIDI files from the SD card.
// Example program to demonstrate the use of the MIDFile library
// Just for fun light up a LED in time to the music.
//
// Hardware required:
//  SD card interface - change SD_SELECT for SPI comms
//  3 LEDs (optional) - to display current status and beat. 
//            Change pin definitions for specific hardware setup - defined below.
uint16_t count1;
#include <MIDI.h>
MIDI_CREATE_DEFAULT_INSTANCE();
//#include <Wire.h> 
#include <LiquidCrystal.h>
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
//LiquidCrystal lcd(0x3F,16,2);
enum lcd_state  { LSBegin, LSSelect, LSShowFile, LSGotFile };
enum midi_state { MSBegin, MSLoad, MSOpen, MSProcess, MSClose };
enum seq_state  { LCDSeq, MIDISeq };
#include <SdFat.h>
#include <MD_MIDIFile.h>
#define  FNAME_SIZE    30        // 8.3 + '\0' character file names
#define PLAYLIST_FILE "PLAYLIST.TXT"  // file of file names
#define MIDI_EXT    ".mid"
#define USE_MIDI  1
uint16_t  plCount = 0;

#if USE_MIDI // set up for direct MIDI serial output

#define DEBUG(x)
#define DEBUGX(x)
#define SERIAL_RATE 31250

#else // don't use MIDI to allow printing debug statements

#define DEBUG(x)  Serial.print(x)
#define DEBUGX(x) Serial.print(x, HEX)
#define SERIAL_RATE 57600
int a;
#endif // USE_MIDI
  uint16_t  count = 0;// count of files
  char      fname[FNAME_SIZE];
char nombre[FNAME_SIZE];
// SD chip select pin for SPI comms.
// Arduino Ethernet shield, pin 4.
// Default SD chip select is the SPI SS pin (10).
// Other hardware will be different as documented for that hardware.
#define  SD_SELECT  53
SdFat SD;
// LED definitions for user indicators
#define READY_LED     A0 // when finished
#define SMF_ERROR_LED A2 // SMF error
#define SD_ERROR_LED  A4 // SD error
#define BEAT_LED      A2 // toggles to the 'beat'
int button[3]={A6,A3,A1};
#define WAIT_DELAY  2000  // ms

#define ARRAY_SIZE(a) (sizeof(a)/sizeof(a[0]))



// The files in the tune list should be located on the SD card 
// or an error will occur opening the file and the next in the 
// list will be opened (skips errors).
  static lcd_state s = LSBegin;
  static uint8_t  plIndex = 0;
//  static char fname[FNAME_SIZE];
  static SdFile plFile;

void check(void)
// create a play list file on the SD card with the names of the files.
// This will then be used in the menu.
{
   //if(count!=4){
  SdFile    plFile;   // play list file
  SdFile    mFile;    // MIDI file


  // open/create the play list file
  if (!plFile.open(PLAYLIST_FILE, O_CREAT|O_WRITE))
//    LCDErrMessage("PL create fail", true);

  SD.vwd()->rewind();
  while (mFile.openNext(SD.vwd(), O_READ))
  {
    mFile.getName(fname, FNAME_SIZE);


    if (mFile.isFile())
    {
      if (strcmp(MIDI_EXT, &fname[strlen(fname)-strlen(MIDI_EXT)]) == 0)
      // only include files with MIDI extension
      {
        //plFile.write(fname,FNAME_SIZE);
     /*   DEBUG("\nFile ");
    DEBUG();
    DEBUG(" ");*/
    
     mFile.getName(nombre, FNAME_SIZE);
//tuneList[count]={nombre};
   // DEBUG(tuneList[8]);
    lcd.clear();
     lcd.setCursor(0,1);
    lcd.print(nombre);
      lcd.setCursor(0,0);  // use the next file name and play it
    lcd.print("File: ");
    lcd.setCursor(10,0);
    lcd.print(count);
      lcd.setCursor(11,0);
    lcd.print("/");
      lcd.setCursor(13,0);
    lcd.print(count1);
        count1++;
        //delay(500);
      }
     // xx++;
    }
    mFile.close();
  }
  //DEBUG("\nList completed");

  // close the play list file
  plFile.close();
  
delay(500);
}
  
/*void createPlaylistFile(void)
// create a play list file on the SD card with the names of the files.
// This will then be used in the menu.
{
  SdFile    plFile;   // play list file
  SdFile    mFile;    // MIDI file


  // open/create the play list file
  if (!plFile.open(PLAYLIST_FILE, O_CREAT|O_WRITE))
//    LCDErrMessage("PL create fail", true);

  SD.vwd()->rewind();
  while (mFile.openNext(SD.vwd(), O_READ))
  {
    mFile.getName(fname, FNAME_SIZE);


    if (mFile.isFile())
    {
      if (strcmp(MIDI_EXT, &fname[strlen(fname)-strlen(MIDI_EXT)]) == 0)
      // only include files with MIDI extension
      {
        //plFile.write(fname,FNAME_SIZE);
         DEBUG("\nFile ");
    DEBUG(count);
    DEBUG(" ");
     mFile.getName(nombre, FNAME_SIZE);
tuneList[count]={nombre};
    DEBUG(tuneList[count]);
     
        count++;
      }
      
    }
    mFile.close();
  }
  DEBUG("\nList completed");

  // close the play list file
  plFile.close();
//Serial.print(mFile.read());
  //return(count);
  
}*/

///char *tuneList[];
// These don't play as they need more than 16 tracks but will run if MIDIFile.h is changed
//#define MIDI_FILE  "SYMPH9.MID"   // 29 tracks
//#define MIDI_FILE  "CHATCHOO.MID"   // 17 tracks
//#define MIDI_FILE  "STRIPPER.MID"   // 25 tracks

//SdFat SD;
MD_MIDIFile SMF;

void midiCallback(midi_event *pev)
// Called by the MIDIFile library when a file event needs to be processed
// thru the midi communications interface.
// This callback is set up in the setup() function.
{

  for (uint8_t i=0; i<pev->size; i++)
  {
 if(pev->data[2]>10){
     MIDI.sendNoteOn(pev->data[1], pev->data[2],pev->channel+1 );
 }
 else{
  MIDI.sendNoteOff(pev->data[1], 0,pev->channel+1 );
 }
  }
}

void sysexCallback(sysex_event *pev)
// Called by the MIDIFile library when a system Exclusive (sysex) file event needs 
// to be processed through the midi communications interface. Most sysex events cannot 
// really be processed, so we just ignore it here.
// This callback is set up in the setup() function.
{
  DEBUG("\nS T");
  DEBUG(pev->track);
  DEBUG(": Data ");
  for (uint8_t i=0; i<pev->size; i++)
  {
    DEBUGX(pev->data[i]);
  DEBUG(' ');
  }
}

void midiSilence(void)
// Turn everything off on every channel.
// Some midi files are badly behaved and leave notes hanging, so between songs turn
// off all the notes and sound
{
  midi_event  ev;

  // All sound off
  // When All Sound Off is received all oscillators will turn off, and their volume
  // envelopes are set to zero as soon as possible.
  ev.size = 0;
  ev.data[ev.size++] = 0xb0;
  ev.data[ev.size++] = 120;
  ev.data[ev.size++] = 0;

  for (ev.channel = 0; ev.channel < 16; ev.channel++)
    midiCallback(&ev);
}
 
void setup(void)
{
  MIDI.begin();
    if (!SD.begin(SD_SELECT, SPI_FULL_SPEED))
  {
    DEBUG("\nSD init fail!");
    digitalWrite(SD_ERROR_LED, HIGH);
    while (true) ;
  }
//  plCount = createPlaylistFile();
 /// DEBUG(plCount);
  // Initialize MIDIFile
  SMF.begin(&SD);
  SMF.setMidiHandler(midiCallback);
  SMF.setSysexHandler(sysexCallback);
  Serial.begin(SERIAL_RATE);

 
//Serial.print(mFile.read());
  //return(count);

lcd.begin(16,2);
  // Print a message to the LCD.
  //lcd.backlight();
  // Set up LED 
  pinMode(button[0],INPUT_PULLUP);
  pinMode(button[1],INPUT_PULLUP);
  pinMode(button[2],INPUT_PULLUP);
  pinMode(READY_LED, OUTPUT);
  pinMode(SD_ERROR_LED, OUTPUT);
  pinMode(SMF_ERROR_LED, OUTPUT);
check();
  
count1--;
  DEBUG("\n[MidiFile Play List]");

  // Initialize SD


  digitalWrite(READY_LED, HIGH);
}

void tickMetronome(void)
// flash a LED to the beat
{
  static uint32_t lastBeatTime = 0;
  static boolean  inBeat = false;
  uint16_t  beatTime;

  beatTime = 60000/SMF.getTempo();    // msec/beat = ((60sec/min)*(1000 ms/sec))/(beats/min)
  if (!inBeat)
  {
    if ((millis() - lastBeatTime) >= beatTime)
    {
      lastBeatTime = millis();
      digitalWrite(BEAT_LED, HIGH);
      inBeat = true;
    }
  }
  else
  {
    if ((millis() - lastBeatTime) >= 100) // keep the flash on for 100ms only
    {
      digitalWrite(BEAT_LED, LOW);
      inBeat = false;
    }
  }

}
int i=6;
int v;
void loop(void)
{  count=0;
  //char *tuneList[count1];
 // delay(500);
  
  //if(v==0){
    // create a play list file on the SD card with the names of the files.
// This will then be used in the menu.
if(digitalRead(button[0])==0){
 i++;
  //if(count!=4){
  SdFile    plFile;   // play list file
  SdFile    mFile;    // MIDI file


  // open/create the play list file
  if (!plFile.open(PLAYLIST_FILE, O_CREAT|O_WRITE))
//    LCDErrMessage("PL create fail", true);

  SD.vwd()->rewind();
  while (mFile.openNext(SD.vwd(), O_READ))
  {
    mFile.getName(fname, FNAME_SIZE);


    if (mFile.isFile())
    {
      if (strcmp(MIDI_EXT, &fname[strlen(fname)-strlen(MIDI_EXT)]) == 0&&count<i)
      // only include files with MIDI extension
      {
        //plFile.write(fname,FNAME_SIZE);
     /*   DEBUG("\nFile ");
    DEBUG();
    DEBUG(" ");*/
    
     mFile.getName(nombre, FNAME_SIZE);
//tuneList[count]={nombre};
   // DEBUG(tuneList[8]);
    lcd.clear();
     lcd.setCursor(0,1);
    lcd.print(nombre);
      lcd.setCursor(0,0);  // use the next file name and play it
    lcd.print("File: ");
    lcd.setCursor(10,0);
    lcd.print(count);
      lcd.setCursor(12,0);
    lcd.print("/");
          lcd.setCursor(13,0);
    lcd.print(count1);
        count++;
        //delay(500);
      }
     // xx++;
    }
    mFile.close();
  }
  //DEBUG("\nList completed");

  // close the play list file
  plFile.close();
  
delay(250);
}
  // /* DEBUG("\nFile: ");
    //DEBUG(nombre);
    int  err;
	

	  // reset LEDs
	  digitalWrite(READY_LED, LOW);
	  digitalWrite(SD_ERROR_LED, LOW);

if(digitalRead(button[2])==0){
 i--;
  //if(count!=4){
  SdFile    plFile;   // play list file
  SdFile    mFile;    // MIDI file


  // open/create the play list file
  if (!plFile.open(PLAYLIST_FILE, O_CREAT|O_WRITE))
//    LCDErrMessage("PL create fail", true);

  SD.vwd()->rewind();
  while (mFile.openNext(SD.vwd(), O_READ))
  {
    mFile.getName(fname, FNAME_SIZE);


    if (mFile.isFile())
    {
      if (strcmp(MIDI_EXT, &fname[strlen(fname)-strlen(MIDI_EXT)]) == 0&&count<i)
      // only include files with MIDI extension
      {
        //plFile.write(fname,FNAME_SIZE);
     /*   DEBUG("\nFile ");
    DEBUG();
    DEBUG(" ");*/
    
     mFile.getName(nombre, FNAME_SIZE);
//tuneList[count]={nombre};
   // DEBUG(tuneList[8]);
    lcd.clear();
     lcd.setCursor(0,1);
    lcd.print(nombre);
      lcd.setCursor(0,0);  // use the next file name and play it
    lcd.print("File: ");
    lcd.setCursor(10,0);
    lcd.print(count);
          lcd.setCursor(12,0);
    lcd.print("/");
          lcd.setCursor(13,0);
    lcd.print(count1);
        count++;
        //delay(500);
      }
     // xx++;
    }
    mFile.close();
  }
  //DEBUG("\nList completed");

  // close the play list file
  plFile.close();
  
delay(250);
}
 //Serial.print(tuneList[i]);

    if(digitalRead(button[1])==0){
	  SMF.setFilename(nombre);
   lcd.setCursor(16,0);
    lcd.print("p");
   
	  err = SMF.load();
	  if (err != -1)
	  {
		DEBUG("\nSMF load Error ");
		DEBUG(err);
		digitalWrite(SMF_ERROR_LED, HIGH);
		delay(WAIT_DELAY);
	  }
	  else
	  {
		// play the file
		while (!SMF.isEOF())
		{
			if (SMF.getNextEvent())
			tickMetronome();
		}

		// done with this one
		SMF.close();
		midiSilence();

		// signal finish LED with a dignified pause
		digitalWrite(READY_LED, HIGH);
		delay(WAIT_DELAY);
	  
	}
    }
}

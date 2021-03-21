#include "Arduino.h"
#include "Audio.h"

// Digital I/O used
#define SD_CS          5
#define SPI_SCK       18
#define SPI_MISO      19
#define SPI_MOSI      23

#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

Audio audio;


const char *name(File& f) {
#ifdef SDFATFS_USED
    static char buf[64];
    buf[0] = 0;
    f.getName(buf, sizeof(buf));
    return (const char *)buf;
#else
    return f.name();
#endif
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
    char path[256] = "";    
    int len = 0;         
    
    Serial.printf("Listing directory: %s\n", dirname);
    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }
    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(name(file));
            if(levels){
                // generate complete path if SdFat is used
                if ( (name(file))[0] != '/' ) {
                    strcpy(path, dirname); 
                    len = strlen(path);
                    if ( !(len == 1 && path[0] == '/') )    // not root (/) 
                        path[len++] = '/'; 
                }
                //
                strcpy(path+len, name(file));
                listDir(fs, path, levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(name(file));
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}


void setup() {
    Serial.begin(115200);
    Serial.println();

    // separate SPI initialisation only required if the SPI standard GPIO is not used
    // pinMode(SD_CS, OUTPUT);      
    // digitalWrite(SD_CS, HIGH);
    // SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    // SPI.setFrequency(25000000);
     
    if (!SD.begin(SD_CS, SD_SCK_MHZ(25))) 
        SD.initErrorHalt();     // SdFat-lib helper function
      
    Serial.printf("SD FAT-type = %d\n", SD.fatType());
    
    // set SdFatConfig MAINTAIN_FREE_CLUSTER_COUNT non-zero. 
    // Then only the first call will take time.
    // Serial.printf("SD_UsedBytes = %.6f GB\n", SD.usedBytes()/(1024*1024*1024.0));
    Serial.printf("SD_TotalBytes = %.6f GB\n", SD.totalBytes()/(1024*1024*1024.0));

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(1); // 0...21
    
    listDir(SD, "/", 5);

    audio.connecttoFS(SD, "/320k_test.mp3"); 
//    audio.connecttoFS(SD, "/test.mp3");
//    audio.connecttoSD("sample1.m4a");
//    audio.connecttoFS(SD_MMC, "sample1_ÄÖÜäöüß.m4a");

}

void loop()
{
    audio.loop();
    if(Serial.available()){ // put streamURL in serial monitor
        audio.stopSong();
        String r=Serial.readString(); r.trim();
        if(r.length()>5) audio.connecttohost(r.c_str());
        log_i("free heap=%i", ESP.getFreeHeap());
    }
}

// optional
void audio_info(const char *info){
    Serial.print("info        "); Serial.println(info);
}
void audio_id3data(const char *info){  //id3 metadata
    Serial.print("id3data     ");Serial.println(info);
}
void audio_eof_mp3(const char *info){  //end of file
    Serial.print("eof_mp3     ");Serial.println(info);
}
void audio_showstation(const char *info){
    Serial.print("station     ");Serial.println(info);
}
void audio_showstreamtitle(const char *info){
    Serial.print("streamtitle ");Serial.println(info);
}
void audio_bitrate(const char *info){
    Serial.print("bitrate     ");Serial.println(info);
}
void audio_commercial(const char *info){  //duration in sec
    Serial.print("commercial  ");Serial.println(info);
}
void audio_icyurl(const char *info){  //homepage
    Serial.print("icyurl      ");Serial.println(info);
}
void audio_lasthost(const char *info){  //stream URL played
    Serial.print("lasthost    ");Serial.println(info);
}
void audio_eof_speech(const char *info){
    Serial.print("eof_speech  ");Serial.println(info);
}

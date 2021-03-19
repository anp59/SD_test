/*
SD-Player based on ESP32-audioI2S. 
It works with different SD-Libs (SD, SD_MMC, SPDIFF, SDFAT)
with minimal adjustments to the code in ESP32-audioI2S.
*/

#include "Arduino.h"
#include "WiFiMulti.h"
#include "Audio.h"

// Digital I/O used
#ifndef SS
    #define SS         5
#endif
#define SD_CS          5
#define SPI_SCK       18
#define SPI_MISO      19
#define SPI_MOSI      23

#define I2S_DOUT      25
#define I2S_BCLK      27
#define I2S_LRC       26

Audio audio;


const char *name(File& f)
{
#ifdef SDFATFS_USED
    static char buf[256];
    buf[0] = 0;
    f.getName(buf, sizeof(buf));
    return (const char *)buf;
#else
    return f.name();
#endif
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
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
                listDir(fs, name(file), levels -1);
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

#if SD_IMPL == 0
    if ( !SD.begin(SD_CS, SPI, 25000000) )
        Serial.println("Card Mount Failed");

#elif SD_IMPL == 1
    if ( !SD.begin() )
        Serial.println("Card Mount Failed");
    
#elif SD_IMPL == 2
    if ( !SD.begin() )
        Serial.println("Card Mount Failed");

#elif SD_IMPL == 3
    //if (!SD.begin(SD_CS, SD_SCK_MHZ(25))) 
    if ( !SD.begin() ) 
        SD.initErrorHalt();
        
    Serial.printf("\n\nSdFat type = %d\n", SD.fatType());
    //Serial.printf("SD_Card type = %d\n", SD.card()->type());
#endif

    // set SdFatConfig MAINTAIN_FREE_CLUSTER_COUNT non-zero. 
    // Then only the first call will take time.
    // Serial.printf("SD_UsedBytes = %.6f GB\n", SD.usedBytes()/(1024*1024*1024.0));
    
    // Serial.printf("SD_TotalBytes = %.6f GB\n", SD.totalBytes()/(1024*1024*1024.0));

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(10); // 0...21
    listDir(SD, "/", 5);
    
    //audio.connecttoFS(SD, "/320k_test.mp3");
    audio.connecttoFS(SD, "/test.mp3");
//    audio.connecttoFS(SD_MMC, "sample1_ÄÖÜäöüß.m4a");
//    audio.connecttoSD("sample1.m4a");
//    audio.connecttohost("http://www.wdr.de/wdrlive/media/einslive.m3u");
//    audio.connecttohost("http://macslons-irish-pub-radio.com/media.asx");
//    audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.aac"); //  128k aac
//    audio.connecttohost("http://mp3.ffh.de/radioffh/hqlivestream.mp3"); //  128k mp3
//    audio.connecttospeech("Wenn die Hunde schlafen, kann der Wolf gut Schafe stehlen.", "de");
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

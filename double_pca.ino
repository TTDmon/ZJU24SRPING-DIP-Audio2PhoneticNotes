#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <math.h>
#include <BluetoothSerial.h>

Adafruit_PWMServoDriver pca1 = Adafruit_PWMServoDriver(0x40);
Adafruit_PWMServoDriver pca2 = Adafruit_PWMServoDriver(0x41);

//---------------------Define-------------------------------

#define PI 3.14159265

const int totalSpeakers = 24;

enum WaveType {
    SINE_WAVE,
    COSINE_SQUARED_WAVE
};

typedef struct {  
    float frequency;  
    unsigned long duration;  
    float fmDepth; 
    float fmFrequency; 
    WaveType waveType;  
    int channel; 
} SpeakerConfig;  


#define START_CHANNEL_PCA1   0
#define START_CHANNEL_PCA2   12

//----------------------------------------------------------

//--------------------Main Function-------------------------

void playSound(const SpeakerConfig configs[], size_t numActiveSpeakers, size_t totalSpeakers) {  
    unsigned long startTime = millis();  
    unsigned long elapsedTime;  
    int pwmValues[totalSpeakers] = {0}; 
  
    while ((elapsedTime = millis() - startTime) < configs[0].duration) { 
        for (size_t i = 0; i < numActiveSpeakers; i++) {  
            const SpeakerConfig& config = configs[i];  

            float fmSignal = sin(config.fmFrequency * 2 * PI * elapsedTime / 1000.0f) * config.fmDepth;
            float shapedFrequency = config.frequency + fmSignal;

            float waveValue;  
            if (config.waveType == COSINE_SQUARED_WAVE) {  
                waveValue = cos(shapedFrequency * 2 * PI * elapsedTime / 1000.0f) * cos(shapedFrequency * 2 * PI * elapsedTime / 1000.0f) * sin(shapedFrequency * 2 * PI * elapsedTime / 1000.0f);
                waveValue = (waveValue + 1.0f) / 2.0f;  
            } else {  
                waveValue = sin(shapedFrequency * 2 * PI * elapsedTime / 1000.0f);  
            }
  
            int pwmValue = (waveValue + 1.0f) * 127.5f; 
            pwmValues[i] = pwmValue; 
  
            // 写入PWM值到PCA9685的通道
            if(config.channel < START_CHANNEL_PCA2 + 1) {
                pca1.setPWM(config.channel, 0, pwmValues[i] * 16); // 扩大到12位精度
            } else {
                pca2.setPWM(config.channel - START_CHANNEL_PCA2, 0, pwmValues[i] * 16);
            }
        }  
  
        delay(5); 
    }  
  
    // 停止播放声音  
    for (size_t i = 0; i < totalSpeakers + 1; ++i) {  
        if(i < START_CHANNEL_PCA2 + 1) {
            pca1.setPWM(i, 0, 0); 
        } else {
            pca2.setPWM(i - START_CHANNEL_PCA2, 0, 0);
        }
    }  
}

void playSound_two_by_two(const SpeakerConfig configs[], size_t numActiveSpeakers, size_t totalSpeakers) {
    unsigned long startTime;
    unsigned long elapsedTime;
    int pwmValues[totalSpeakers] = {0}; 

    for (size_t pairStart = 0; pairStart < numActiveSpeakers; pairStart += 2) {
        startTime = millis();  // 重新设置每对扬声器的起始时间
        while ((elapsedTime = millis() - startTime) < configs[pairStart].duration) {
            // 在这个循环中，我们只激活当前的一对扬声器
            for (size_t i = pairStart; i < pairStart + 2 && i < numActiveSpeakers; i++) {
                const SpeakerConfig& config = configs[i];
                float fmSignal = sin(config.fmFrequency * 2 * PI * elapsedTime / 1000.0f) * config.fmDepth;
                float shapedFrequency = config.frequency + fmSignal;

                float waveValue;
                if (config.waveType == COSINE_SQUARED_WAVE) {
                    waveValue = cos(shapedFrequency * 2 * PI * elapsedTime / 1000.0f) * cos(shapedFrequency * 2 * PI * elapsedTime / 1000.0f) * sin(shapedFrequency * 2 * PI * elapsedTime / 1000.0f);
                    waveValue = (waveValue + 1.0f) / 2.0f; 
                } else {
                    waveValue = sin(shapedFrequency * 2 * PI * elapsedTime / 1000.0f);
                }

                int pwmValue = (waveValue + 1.0f) * 127.5f; 
                pwmValues[i] = pwmValue;

                if (config.channel < START_CHANNEL_PCA2 + 1 ) {
                    pca1.setPWM(config.channel, 0, pwmValues[i] * 16); 
                } else {
                    pca2.setPWM(config.channel - START_CHANNEL_PCA2, 0, pwmValues[i] * 16);
                }
            }
            delay(5); // 稍作等待，然后继续
        }

        // 在播放下一对扬声器之前，确保当前对扬声器已停止
        for (size_t i = 0; i < totalSpeakers + 1; ++i) {
            if(i < START_CHANNEL_PCA2 + 1 ) {
                pca1.setPWM(i, 0, 0); 
            } else {
                pca2.setPWM(i - START_CHANNEL_PCA2, 0, 0);
            }
        }
    }
}


void playSound_one_by_one(const SpeakerConfig configs[], size_t numActiveSpeakers, size_t totalSpeakers) {
    unsigned long startTime;
    unsigned long elapsedTime;
    int pwmValues[totalSpeakers] = {0}; 

    for (size_t i = 0; i < numActiveSpeakers; ++i) {
        startTime = millis();  // 重新设置每个扬声器的起始时间
        while ((elapsedTime = millis() - startTime) < configs[i].duration) {
            // 在这个循环中，只激活当前的一个扬声器
            const SpeakerConfig& config = configs[i];
            float fmSignal = sin(config.fmFrequency * 2 * PI * elapsedTime / 1000.0f) * config.fmDepth;
            float shapedFrequency = config.frequency + fmSignal;

            float waveValue;
            if (config.waveType == COSINE_SQUARED_WAVE) {
                waveValue = cos(shapedFrequency * 2 * PI * elapsedTime / 1000.0f) * cos(shapedFrequency * 2 * PI * elapsedTime / 1000.0f) * sin(shapedFrequency * 2 * PI * elapsedTime / 1000.0f);
                waveValue = (waveValue + 1.0f) / 2.0f; 
            } else {
                waveValue = sin(shapedFrequency * 2 * PI * elapsedTime / 1000.0f);
            }

            int pwmValue = (waveValue + 1.0f) * 127.5f; 
            
            pwmValues[i] = pwmValue;

            if (config.channel < START_CHANNEL_PCA2 + 1) {
                pca1.setPWM(config.channel, 0, pwmValues[i]); 
            } else {
                pca2.setPWM(config.channel - START_CHANNEL_PCA2, 0, pwmValues[i] * 16);
            }

            delay(5); // 稍作等待，然后继续
        }

        // 在播放下一个扬声器之前，确保当前扬声器已停止
        if (configs[i].channel < START_CHANNEL_PCA2) {
            pca1.setPWM(configs[i].channel, 0, 0); 
        } else {
            pca2.setPWM(configs[i].channel - START_CHANNEL_PCA2, 0, 0);
        }
    }
}

//--------------------------------------------------------

//-------------------辅音因素阵列--------------------------
    SpeakerConfig configs_H[8] = { 
        {60, 400, 0, 0, COSINE_SQUARED_WAVE, 0},  
        {60, 400, 0, 0, COSINE_SQUARED_WAVE, 1},  
        {60, 400, 0, 0, COSINE_SQUARED_WAVE, 2},   
        {60, 400, 0, 0, COSINE_SQUARED_WAVE, 3}, 
        {60, 400, 0, 0, COSINE_SQUARED_WAVE, 4},
        {60, 400, 0, 0, COSINE_SQUARED_WAVE, 5},
        {60, 400, 0, 0, COSINE_SQUARED_WAVE, 6},
        {60, 400, 0, 0, COSINE_SQUARED_WAVE, 7},  
    };   
    
    SpeakerConfig configs_Y[8] = { 
        {60, 400, 0, 0, SINE_WAVE, 3},  
        {60, 400, 0, 0, SINE_WAVE, 4},  
        {60, 400, 0, 0, SINE_WAVE, 5},   
        {60, 400, 0, 0, SINE_WAVE, 6},  
        {60, 400, 0, 0, SINE_WAVE, 13},  
        {60, 400, 0, 0, SINE_WAVE, 14},  
        {60, 400, 0, 0, SINE_WAVE, 15},   
        {60, 400, 0, 0, SINE_WAVE, 16},  
    };  

    SpeakerConfig configs_W[8] = { 
        {60, 400, 5, 8, SINE_WAVE, 3},  
        {60, 400, 5, 8, SINE_WAVE, 4},  
        {60, 400, 5, 8, SINE_WAVE, 5},   
        {60, 400, 5, 8, SINE_WAVE, 6},  
        {60, 400, 5, 8, SINE_WAVE, 13},  
        {60, 400, 5, 8, SINE_WAVE, 14},  
        {60, 400, 5, 8, SINE_WAVE, 15},   
        {60, 400, 5, 8, SINE_WAVE, 16},  
    }; 

    SpeakerConfig configs_F[4] = {
        {300, 400, 0, 0, SINE_WAVE, 6},  
        {300, 400, 0, 0, SINE_WAVE, 12},  
        {300, 400, 0, 0, SINE_WAVE, 18},   
        {300, 400, 0, 0, SINE_WAVE, 24},  
    };  

    SpeakerConfig configs_V[4] = {
        {300, 400, 5, 8, SINE_WAVE, 6},  
        {300, 400, 5, 8, SINE_WAVE, 12},  
        {300, 400, 5, 8, SINE_WAVE, 18},   
        {300, 400, 5, 8, SINE_WAVE, 24},  
    };  

    SpeakerConfig configs_P[4] = {
        {300, 100, 0, 0, SINE_WAVE, 5},  
        {300, 100, 0, 0, SINE_WAVE, 6},  
        {300, 100, 0, 0, SINE_WAVE, 11},   
        {300, 100, 0, 0, SINE_WAVE, 12},  
    };  

    SpeakerConfig configs_B[4] = {
        {300, 100, 5, 30, SINE_WAVE, 5},  
        {300, 100, 5, 30, SINE_WAVE, 6},  
        {300, 100, 5, 30, SINE_WAVE, 11},   
        {300, 100, 5, 30, SINE_WAVE, 12},  
    };  

    SpeakerConfig configs_M[4] = {
        {60, 400, 5, 8, SINE_WAVE, 5},  
        {60, 400, 5, 8, SINE_WAVE, 6},  
        {60, 400, 5, 8, SINE_WAVE, 11},   
        {60, 400, 5, 8, SINE_WAVE, 12},  
    };  

    SpeakerConfig configs_SH[4] = {
        {300, 400, 0, 0, SINE_WAVE, 17},  
        {300, 400, 0, 0, SINE_WAVE, 18},  
        {300, 400, 0, 0, SINE_WAVE, 23},   
        {300, 400, 0, 0, SINE_WAVE, 24},  
    };  

    SpeakerConfig configs_ZH[4] = {
        {300, 400, 5, 8, SINE_WAVE, 13},  
        {300, 400, 5, 8, SINE_WAVE, 14},  
        {300, 400, 5, 8, SINE_WAVE, 19},   
        {300, 400, 5, 8, SINE_WAVE, 20},  
    };  

    SpeakerConfig configs_L[4] = {
        {300, 400, 5, 30, SINE_WAVE, 17},  
        {300, 400, 5, 30, SINE_WAVE, 18},  
        {300, 400, 5, 30, SINE_WAVE, 23},   
        {300, 400, 5, 30, SINE_WAVE, 24},  
    };  

     SpeakerConfig configs_TH[4] = {
        {300, 400, 0, 0, SINE_WAVE, 3},  
        {300, 400, 0, 0, SINE_WAVE, 4},  
        {300, 400, 0, 0, SINE_WAVE, 9},   
        {300, 400, 0, 0, SINE_WAVE, 10},  
    };  

    SpeakerConfig configs_DH[4] = { 
        {300, 400, 5, 8, SINE_WAVE, 3},  
        {300, 400, 5, 8, SINE_WAVE, 4},  
        {300, 400, 5, 8, SINE_WAVE, 9},   
        {300, 400, 5, 8, SINE_WAVE, 10},  
    }; 

    SpeakerConfig configs_T[4] = {
        {300, 100, 0, 0, SINE_WAVE, 15},  
        {300, 100, 0, 0, SINE_WAVE, 16},  
        {300, 100, 0, 0, SINE_WAVE, 21},   
        {300, 100, 0, 0, SINE_WAVE, 22},  
    }; 

    SpeakerConfig configs_D[4] = {
        {300, 100, 5, 30, SINE_WAVE, 15},  
        {300, 100, 5, 30, SINE_WAVE, 16},  
        {300, 100, 5, 30, SINE_WAVE, 21},   
        {300, 100, 5, 30, SINE_WAVE, 22},  
    };  

    SpeakerConfig configs_N[4] = {
        {60, 400, 5, 8, SINE_WAVE, 15},  
        {60, 400, 5, 8, SINE_WAVE, 16},  
        {60, 400, 5, 8, SINE_WAVE, 21},   
        {60, 400, 5, 8, SINE_WAVE, 22},  
    };  

    SpeakerConfig configs_NG[4] = {
        {60, 400, 5, 8, SINE_WAVE, 1},  
        {60, 400, 5, 8, SINE_WAVE, 2},  
        {60, 400, 5, 8, SINE_WAVE, 7},   
        {60, 400, 5, 8, SINE_WAVE, 8},  
    };

    SpeakerConfig configs_K[4] = {
        {300, 100, 0, 0, SINE_WAVE, 1},  
        {300, 100, 0, 0, SINE_WAVE, 2},  
        {300, 100, 0, 0, SINE_WAVE, 7},   
        {300, 100, 0, 0, SINE_WAVE, 8},  
    }; 

    SpeakerConfig configs_G[4] = {
        {300, 100, 5, 30, SINE_WAVE, 1},  
        {300, 100, 5, 30, SINE_WAVE, 2},  
        {300, 100, 5, 30, SINE_WAVE, 7},   
        {300, 100, 5, 30, SINE_WAVE, 8},  
    };  

    SpeakerConfig configs_R[4] = {
        {300, 400, 5, 30, SINE_WAVE, 13},  
        {300, 400, 5, 30, SINE_WAVE, 14},  
        {300, 400, 5, 30, SINE_WAVE, 19},   
        {300, 400, 5, 30, SINE_WAVE, 20},  
    };  

    SpeakerConfig configs_S[4] = {
        {300, 400, 0, 0, SINE_WAVE, 1},  
        {300, 400, 0, 0, SINE_WAVE, 7},  
        {300, 400, 0, 0, SINE_WAVE, 13},   
        {300, 400, 0, 0, SINE_WAVE, 19},  
    };  

    SpeakerConfig configs_Z[4] = {
        {300, 400, 5, 8, SINE_WAVE, 1},  
        {300, 400, 5, 8, SINE_WAVE, 7},  
        {300, 400, 5, 8, SINE_WAVE, 13},   
        {300, 400, 5, 8, SINE_WAVE, 19},  
    };  

    SpeakerConfig configs_CH[4] = {
        {300, 400, 0, 0, COSINE_SQUARED_WAVE, 1},  
        {300, 400, 0, 0, COSINE_SQUARED_WAVE, 6},  
        {300, 400, 0, 0, COSINE_SQUARED_WAVE, 7},   
        {300, 400, 0, 0, COSINE_SQUARED_WAVE, 12},  
    }; 

    SpeakerConfig configs_J[4] = {
        {300, 400, 5, 8, SINE_WAVE, 1},  
        {300, 400, 5, 8, SINE_WAVE, 6},  
        {300, 400, 5, 8, SINE_WAVE, 7},   
        {300, 400, 5, 8, SINE_WAVE, 12},  
    };  
//----------------------------------------------------


//----------------------元音音素阵列----------------------------


SpeakerConfig configs_EE[6] = { 
        {300, 480, 0, 0, SINE_WAVE, 6},  
        {300, 480, 0, 0, SINE_WAVE, 5},  
        {300, 480, 0, 0, SINE_WAVE, 4},   
        {300, 480, 0, 0, SINE_WAVE, 3}, 
        {300, 480, 0, 0, SINE_WAVE, 2},
        {300, 480, 0, 0, SINE_WAVE, 1}, 
    };  

SpeakerConfig configs_AH[12] = { 
        {60, 480, 0, 0, SINE_WAVE, 1},  
        {60, 480, 0, 0, SINE_WAVE, 7},  
        {60, 480, 0, 0, SINE_WAVE, 2},
        {60, 480, 0, 0, SINE_WAVE, 8},
        {60, 480, 0, 0, SINE_WAVE, 3},
        {60, 480, 0, 0, SINE_WAVE, 9},
        {60, 480, 0, 0, SINE_WAVE, 4},
        {60, 480, 0, 0, SINE_WAVE, 10},
        {60, 480, 0, 0, SINE_WAVE, 5},
        {60, 480, 0, 0, SINE_WAVE, 11},
        {60, 480, 0, 0, SINE_WAVE, 6},
        {60, 480, 0, 0, SINE_WAVE, 12},
    };  


SpeakerConfig configs_OO[12] = { 
        {300, 480, 5, 30, SINE_WAVE, 18},  
        {300, 480, 5, 30, SINE_WAVE, 24},
        {300, 480, 5, 30, SINE_WAVE, 17},
        {300, 480, 5, 30, SINE_WAVE, 23},
        {300, 480, 5, 30, SINE_WAVE, 16},
        {300, 480, 5, 30, SINE_WAVE, 22},
        {300, 480, 5, 30, SINE_WAVE, 15},
        {300, 480, 5, 30, SINE_WAVE, 21},
        {300, 480, 5, 30, SINE_WAVE, 14},
        {300, 480, 5, 30, SINE_WAVE, 20},
        {300, 480, 5, 30, SINE_WAVE, 13},
        {300, 480, 5, 30, SINE_WAVE, 19},
    };  

SpeakerConfig configs_I[4] = { 
        {300, 240, 0, 0, SINE_WAVE, 1},  
        {300, 240, 0, 0, SINE_WAVE, 2},
        {300, 240, 0, 0, SINE_WAVE, 3},
        {300, 240, 0, 0, SINE_WAVE, 4},
    };  

SpeakerConfig configs_UU[8] = { 
        {300, 240, 5, 30, SINE_WAVE, 13},  
        {300, 240, 5, 30, SINE_WAVE, 19},
        {300, 240, 5, 30, SINE_WAVE, 14},
        {300, 240, 5, 30, SINE_WAVE, 20},
        {300, 240, 5, 30, SINE_WAVE, 15},
        {300, 240, 5, 30, SINE_WAVE, 21},
        {300, 240, 5, 30, SINE_WAVE, 16},
        {300, 240, 5, 30, SINE_WAVE, 22},
    };

SpeakerConfig configs_AE[6] = { 
        {300, 480, 0, 0, SINE_WAVE, 1},  
        {300, 480, 0, 0, SINE_WAVE, 7},
        {300, 480, 0, 0, SINE_WAVE, 8},
        {300, 480, 0, 0, SINE_WAVE, 9},
        {300, 480, 0, 0, SINE_WAVE, 3},
        {300, 480, 0, 0, SINE_WAVE, 2},
    };

SpeakerConfig configs_AW[6] = { 
        {300, 480, 0, 0, SINE_WAVE, 16},  
        {300, 480, 0, 0, SINE_WAVE, 22},
        {300, 480, 0, 0, SINE_WAVE, 23},
        {300, 480, 0, 0, SINE_WAVE, 24},
        {300, 480, 0, 0, SINE_WAVE, 18},
        {300, 480, 0, 0, SINE_WAVE, 17},
    };

SpeakerConfig configs_ER[6] = { 
        {300, 480, 0, 0, SINE_WAVE, 13},  
        {300, 480, 0, 0, SINE_WAVE, 19},
        {300, 480, 0, 0, SINE_WAVE, 20},
        {300, 480, 0, 0, SINE_WAVE, 21},
        {300, 480, 0, 0, SINE_WAVE, 15},
        {300, 480, 0, 0, SINE_WAVE, 14},
    };

SpeakerConfig configs_OW[3] = { 
        {300, 480, 0, 0, SINE_WAVE, 6},  
        {300, 480, 0, 0, SINE_WAVE, 4},
        {300, 480, 0, 0, SINE_WAVE, 2},
    };

SpeakerConfig configs_OY[3] = { 
        {300, 480, 0, 0, SINE_WAVE, 13},  
        {300, 480, 0, 0, SINE_WAVE, 15},
        {300, 480, 0, 0, SINE_WAVE, 17},
    };

SpeakerConfig configs_AY[8] = { 
        {300, 480, 5, 30, SINE_WAVE, 3},  
        {300, 480, 5, 30, SINE_WAVE, 4},
        {300, 480, 5, 30, SINE_WAVE, 5},
        {300, 480, 5, 30, SINE_WAVE, 6},  
        {300, 480, 5, 30, SINE_WAVE, 12},
        {300, 480, 5, 30, SINE_WAVE, 11},
        {300, 480, 5, 30, SINE_WAVE, 10},  
        {300, 480, 5, 30, SINE_WAVE, 9},
    };

SpeakerConfig configs_OE[8] = { 
        {300, 480, 0, 0, COSINE_SQUARED_WAVE, 3}, 
        {300, 480, 0, 0, COSINE_SQUARED_WAVE, 4}, 
        {300, 480, 0, 0, COSINE_SQUARED_WAVE, 9}, 
        {300, 480, 0, 0, COSINE_SQUARED_WAVE, 10}, 
        {300, 480, 0, 0, COSINE_SQUARED_WAVE, 15}, 
        {300, 480, 0, 0, COSINE_SQUARED_WAVE, 16}, 
        {300, 480, 0, 0, COSINE_SQUARED_WAVE, 21}, 
        {300, 480, 0, 0, COSINE_SQUARED_WAVE, 22},  
    };


SpeakerConfig configs_UH_1[4] = {
        {300, 240, 0, 0, SINE_WAVE, 6}, 
        {300, 240, 0, 0, SINE_WAVE, 12}, 
        {300, 240, 0, 0, SINE_WAVE, 18}, 
        {300, 240, 0, 0, SINE_WAVE, 24},
     };
SpeakerConfig configs_UH_2[4]={
        {300, 240, 0, 0, SINE_WAVE, 4}, 
        {300, 240, 0, 0, SINE_WAVE, 10}, 
        {300, 240, 0, 0, SINE_WAVE, 16}, 
        {300, 240, 0, 0, SINE_WAVE, 22}, 
     };
         


SpeakerConfig configs_EH_1[4] = { 
        {300, 240, 0, 0, SINE_WAVE, 1}, 
        {300, 240, 0, 0, SINE_WAVE, 7}, 
        {300, 240, 0, 0, SINE_WAVE, 13}, 
        {300, 240, 0, 0, SINE_WAVE, 19},
     };
SpeakerConfig configs_EH_2[4] = { 
        {300, 240, 0, 0, SINE_WAVE, 3}, 
        {300, 240, 0, 0, SINE_WAVE, 9}, 
        {300, 240, 0, 0, SINE_WAVE, 15}, 
        {300, 240, 0, 0, SINE_WAVE, 21}, 
     };

SpeakerConfig configs_I_1[4] = 
     {
        {300, 480, 5, 30, SINE_WAVE, 1}, 
        {300, 480, 5, 30, SINE_WAVE, 7}, 
        {300, 480, 5, 30, SINE_WAVE, 13}, 
        {300, 480, 5, 30, SINE_WAVE, 19},
     };
SpeakerConfig configs_I_2[4]=
     {
        {300, 480, 5, 30, SINE_WAVE, 2}, 
        {300, 480, 5, 30, SINE_WAVE, 8}, 
        {300, 480, 5, 30, SINE_WAVE, 14}, 
        {300, 480, 5, 30, SINE_WAVE, 20},
     };
SpeakerConfig configs_I_3[4]=
     {
        {300, 480, 5, 30, SINE_WAVE, 3}, 
        {300, 480, 5, 30, SINE_WAVE, 9}, 
        {300, 480, 5, 30, SINE_WAVE, 15}, 
        {300, 480, 5, 30, SINE_WAVE, 21},
     };
SpeakerConfig configs_I_4[4]=
     {
        {300, 480, 5, 30, SINE_WAVE, 4}, 
        {300, 480, 5, 30, SINE_WAVE, 10}, 
        {300, 480, 5, 30, SINE_WAVE, 16}, 
        {300, 480, 5, 30, SINE_WAVE, 22},
     };
SpeakerConfig configs_I_5[4]=
     {
        {300, 480, 5, 30, SINE_WAVE, 3}, 
        {300, 480, 5, 30, SINE_WAVE, 9}, 
        {300, 480, 5, 30, SINE_WAVE, 15}, 
        {300, 480, 5, 30, SINE_WAVE, 21},
     };
SpeakerConfig configs_I_6[4]=
     {
        {300, 480, 5, 30, SINE_WAVE, 2}, 
        {300, 480, 5, 30, SINE_WAVE, 8}, 
        {300, 480, 5, 30, SINE_WAVE, 14}, 
        {300, 480, 5, 30, SINE_WAVE, 20},
     };
SpeakerConfig configs_I_7[4]=
     {
        {300, 480, 5, 30, SINE_WAVE, 1}, 
        {300, 480, 5, 30, SINE_WAVE, 7}, 
        {300, 480, 5, 30, SINE_WAVE, 13}, 
        {300, 480, 5, 30, SINE_WAVE, 19},
     };
         
//----------------------------------------------------

//-------------------setup & loop--------------------

void setup() {  
    Serial.begin(9600);
    Wire.begin(21,22);
    pca1.begin();
    pca2.begin();
    //蓝牙模块的启动
    SerialBT.begin("ESP32-BT"); //蓝牙设备名称
    Serial.printf("bluetooth init complete!")
}


void loop() {
    //读取蓝牙通信内容
    if (SerialBT.available()) {
        // 读取从客户端发送的数据
        String received = SerialBT.readString();
        Serial.print("Received: ");
        Serial.println(received);
    }

    if (Serial.available() > 0) {
    // 读取接收到的字符
    char incomingChar = received;
    Serial.println(incomingChar);

    // 根据接收到的字符执行不同的操作
    switch (incomingChar) {
      //播放辅音音素------------------------------------
      case 'H':
          playSound(configs_H, 8, 24);        
          break;
      case 'Y':
          playSound(configs_Y, 8, 24);        
          break;
      case 'W':
          playSound(configs_W, 8, 24);        
          break;
      case 'F':
          playSound(configs_F, 4, 24);        
          break;
      case 'V':
          playSound(configs_V, 4, 24);       
          break;
      case 'P':
          playSound(configs_P, 4, 24);
          break;
      case 'B':
          playSound(configs_B, 4, 24);
          break;
      case 'M':
          playSound(configs_M, 4, 24);
          break;
      case '1'://'SH':
          playSound(configs_SH, 4, 24);        
          break;
      case '2'://'ZH':
          playSound(configs_ZH, 4, 24);        
          break;
      case 'L':
          playSound(configs_L, 4, 24);
          break;
      case '3'://'TH':
          playSound(configs_TH, 4, 24);        
          break;
      case '4'://'DH':
          playSound(configs_DH, 4, 24);        
          break;
      case 'T':
          playSound(configs_T, 4, 24);        
          break;
      case 'D':
          playSound(configs_D, 4, 24);        
          break;
      case 'N':
          playSound(configs_N, 4, 24);       
          break;
      case '5'://'NG':
          playSound(configs_NG, 4, 24);        
          break;
      case 'K':
          playSound(configs_K, 4, 24);        
          break;
      case 'G':
          playSound(configs_G, 4, 24);        
          break;
      case 'R':
          playSound(configs_R, 4, 24);        
          break;
      case 'S':
          playSound(configs_S, 4, 24);        
          break;
      case 'Z':
          playSound(configs_Z, 4, 24);        
          break;
      case '6'://'CH':
          playSound(configs_CH, 4, 24);        
          break;
      case 'J':
          playSound(configs_J, 4, 24);        
          break;

    //------------------------播放元音音素------------------------------------
      case '!'/*'EE'*/:
          playSound_one_by_one(configs_EE,6,6);
          break;
      case '@'://'AH':
          playSound_two_by_two(configs_AH,12,24);
          break;
      case '#'://'OO':
          playSound_two_by_two(configs_OO,12,24);
          break;
      case '$'://'I':
          playSound_one_by_one(configs_I,4,4);
          break;
      case '%'://'UU':
          playSound_two_by_two(configs_UU,8,24);
          break;
      case '^'://'AE':
          playSound_one_by_one(configs_AE,6,6);
          break;
      case '&'://'AW':
          playSound_one_by_one(configs_AW,6,6);
          break;
      case '*'://'ER':
          playSound_one_by_one(configs_ER,6,6);
          break;
      case '('://'OW':
          playSound_one_by_one(configs_OW,3,3);
          break;
      case ')'://'OY':
          playSound_one_by_one(configs_OY,3,3);
          break;
      case '+'://'AY':
          playSound_one_by_one(configs_AY,8,8);
          break;
      case '-'://'OE':
          playSound_two_by_two(configs_OE,8,24);
          break;
      case '['://'UH':
          playSound(configs_UH_1,4,24);
          Serial.println("UH1");
          delay(240);
          Serial.println("UH2");
          playSound(configs_UH_2,4,24);
          break;
      case ']'://'EH':
          playSound(configs_EH_1,4,24);
          delay(240);
          playSound(configs_EH_2,4,24);
          break;
      case '_'://'I':
          playSound(configs_I_1,4,24);
          delay(480);
          playSound(configs_I_2,4,24);
          delay(480);
          playSound(configs_I_3,4,24);
          delay(480);
          playSound(configs_I_4,4,24);
          delay(480);
          playSound(configs_I_5,4,24);
          delay(480);
          playSound(configs_I_6,4,24);
          delay(480);
          playSound(configs_I_7,4,24);
          break;
      default:
          Serial.println("no such");
          break;
    }
  }
}

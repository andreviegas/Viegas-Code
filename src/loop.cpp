#include "loop.h"

void PiscoCode::loop(uint32_t Millis) {  
     if ( pwmCounter == 0 ) {
        if ( currentPhase == NOT_SEQUENCING ) {
           LedOff();
        }
     }
     if ( currentPhase != NOT_SEQUENCING ) {              // Indica que há sinalizacao a fazer
        if ( pwmCounter == 0 && currentPhaseDuration != mSec_veryShortBlink && currentPhase != REPEAT_SEQUENCE &&                     // Está no inicio do ciclo de pwm, e o digito não é o zero e não está repetindo o codigo e...
             currentPhase != FINAL_PAUSE ) {                                                                                       // não está na pausa final
           LedOn();
        } 

        switch (currentPhase) {
            case START_SEQUENCE:
                 if ( millisUltimaEtapa == 0 ) { millisUltimaEtapa = Millis; }                                                    // No inicio do processo, quando a sinalizacao é criada, o millisUltimaEtapa é zerado.
                 if ( pwmCounter == dimmedPWM  ) {                                                                                // Está no pwm de fundo desejado    
                    LedOff();
                 }
                 if ( ((uint32_t)(Millis - millisUltimaEtapa) > currentPhaseDuration) && pwmCounter == pwmSequence ) {                          // Tempo da etapa atual esgotado
                    if ( isNegative ) {
                       currentPhase = NEGATIVE_SIGN_ON;                                                                                // Altera a etapa atual para ligar o LED      
                       currentPhaseDuration = mSec_longBlink*4;
                    } else {
                       currentPhase = READ_NEXT_DIGIT;                                                                                // Altera a etapa atual para ligar o LED  
                    }               
                    millisUltimaEtapa = Millis;                                                                                      // Registra inicio de uma nova etapa                  
                 }
                 break;
            case NEGATIVE_SIGN_ON:
                 if ( pwmCounter == pwmSequence  ) {                                                                                // Está no pwm de fundo desejado    
                    LedOff();
                 }                
                 if ( ((uint32_t)(Millis - millisUltimaEtapa) > currentPhaseDuration) && pwmCounter == pwmSequence ) {                          // Tempo da etapa atual esgotado
                     currentPhase = NEGATIVE_SIGN_OFF;                                                                                   // Finaliza de vez a sinalização                      
                     currentPhaseDuration = mSec_longBlink*4;
                     millisUltimaEtapa = Millis;                                                                                      // Registra inicio de uma nova etapa                  
                 }
                 break;
            case NEGATIVE_SIGN_OFF:
                 if ( pwmCounter == dimmedPWM  ) {                                                                                       // Está no pwm de fundo desejado    
                    LedOff();
                 }                
                 if ( ((uint32_t)(Millis - millisUltimaEtapa) > currentPhaseDuration) && pwmCounter == pwmSequence ) {                          // Tempo da etapa atual esgotado
                     currentPhase = READ_NEXT_DIGIT;                                                                                   // Finaliza de vez a sinalização                      
                     currentPhaseDuration = mSec_betweenDigits;
                     millisUltimaEtapa = Millis;                                                                                      // Registra inicio de uma nova etapa                  
                 }
                 break;
            case READ_NEXT_DIGIT:
                 if ( currentDigit == MAX_DIGITS && sequenceTimes-- ) {                                                              // Indica que não há mais digitos para showDec, final do codigo
                    currentPhase = END_SEQUENCE;                                                                                // Desativa sinalização.
                    currentPhaseDuration = mSec_betweenDigits;                                                                           // Seta a duração atual para tempo apagado entre dígitos
                    millisUltimaEtapa = Millis;                                                                                      // Registra inicio de uma nova etapa                  
                 } else {                                                                                                               // Indica que há números para piscar
                    currentPhaseDuration = mSec_shortBlink;                                                                               // Seta quantos uSec será a duração do pisco longo
                    if ( blinksToShow[currentDigit] == 0 ) {                                                                          // Indica que esse digito iniciou em zero e deve ter um pisco super curto
                       currentPhaseDuration = mSec_veryShortBlink;                                                                        // Ativar pisco super curto pois digito é igual a zero
                    }
                    
                    currentPhase = SEQUENCING_ON;                                                                                       // Altera a etapa atual para ligar o LED
                    millisUltimaEtapa = Millis;                                                                                         // Registra inicio de uma nova etapa
                 }
                 break;
            case SEQUENCING_ON:
                 if ( pwmCounter == pwmSequence  ) {                                                                          // Está no pwm do brilho desejado    
                    LedOff();
                 }
                 if ( ((uint32_t)(Millis - millisUltimaEtapa) > currentPhaseDuration) && pwmCounter == pwmSequence ) {                          // Tempo da etapa atual esgotado
                    LedOff();
                    currentPhase = SEQUENCING_OFF;                                                                                      // Altera a etapa atual para desligar o LED
                    millisUltimaEtapa = Millis;                                                                                         // Registra inicio de uma nova etapa
                    blinksToShow[currentDigit]--;                                                                                     // Decrementa a quantidade de piscos do digito atual
                    if ( blinksToShow[currentDigit] > 0 ) {                                                                           // Indica que ainda não mudou para outro digito
                       currentPhaseDuration = mSec_betweenBlink;                                                                            // Seta a duração atual para tempo apagado entre piscos              
                    } else {
                       currentPhaseDuration = mSec_betweenDigits;                                                                           // Seta a duração atual para tempo apagado entre dígitos
                    }
                 }
                 break;
            case SEQUENCING_OFF:
                 if ( pwmCounter == dimmedPWM  ) {                                                                                // Está no pwm de fundo desejado    
                    LedOff();
                 }                
                 if ( ((uint32_t)(Millis - millisUltimaEtapa) > currentPhaseDuration) && pwmCounter == pwmSequence ) {                          // Tempo da etapa atual esgotado
                    if ( blinksToShow[currentDigit] > 0 ) {                                                                           // Indica que ainda não mudou para outro digito
                       currentPhase = SEQUENCING_ON;                                                                                    // Altera a etapa atual para ligar o LED
                       millisUltimaEtapa = Millis;                                                                                      // Registra inicio de uma nova etapa
                          currentPhaseDuration = mSec_shortBlink;                                                                          // Seta quantos uSec será a duração do pisco longo
                    } else {                 
                       if ( currentDigit < MAX_DIGITS) {
                          currentDigit++;                                                                                                // Pula para o próximo digito.
                       }
                       currentPhase = READ_NEXT_DIGIT;                                                                                // Altera a etapa atual para ligar o LED
                       millisUltimaEtapa = Millis;                                                                                      // Registra inicio de uma nova etapa                  
                    }
                 }
                 break;
            case END_SEQUENCE:
                 if ( pwmCounter == dimmedPWM  ) {                                                                                // Está no pwm de fundo desejado    
                    LedOff();
                 }                
                 if ( ((uint32_t)(Millis - millisUltimaEtapa) > currentPhaseDuration) && pwmCounter == pwmSequence ) {                          // Tempo da etapa atual esgotado
                     if ( sequenceTimes == 0 ) {
                        currentPhase = FINAL_PAUSE;                                                                                   // Finaliza de vez a sinalização                      
                        currentPhaseDuration = mSec_betweenDigits;
                     } else {
                        currentPhase = REPEAT_SEQUENCE;                                                                                // Finaliza de vez a sinalização                      
                        currentPhaseDuration = mSec_betweenCodes;                                                                          // Seta quantos uSec será a duração entre códigos
                     }
                     millisUltimaEtapa = Millis;                                                                                      // Registra inicio de uma nova etapa                  
                 }
                 break;
            case FINAL_PAUSE:
                 if ( ((uint32_t)(Millis - millisUltimaEtapa) > currentPhaseDuration) && pwmCounter == pwmSequence ) {                          // Tempo da etapa atual esgotado
                    currentPhase = NOT_SEQUENCING;                                                                                // Finaliza de vez a sinalização                      
                    millisUltimaEtapa = Millis;                                                                                      // Registra inicio de uma nova etapa                  
                 }
                 break;
             case REPEAT_SEQUENCE:
                 if ( ((uint32_t)(Millis - millisUltimaEtapa) > currentPhaseDuration) && pwmCounter == pwmSequence ) {                          // Tempo da etapa atual esgotado
                    for(int8_t dig=(MAX_DIGITS - 1); dig>=0 ; dig--) {
                        blinksToShow[dig] = digitToShow[dig];                                                                          // Copia os valores para a variavel blinksToShow
                    }
                    currentDigit = lastCurrentDigit;
                    currentPhase = START_SEQUENCE;                                                                                            // Armazena a nova etapa
                    millisUltimaEtapa = Millis;                                                                                       // Registra inicio de uma nova etapa
                    currentPhaseDuration = mSec_betweenDigits;                                                                           // Seta a duração atual para tempo apagado entre dígitos
                 }
                 break;
                
        }
     }
     if ( pwmCounter++ > pwmMax ) {
         pwmCounter = 0;
     }
}
